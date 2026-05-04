#!/usr/bin/env python3
"""
ExplorIR-M CO2 Sensor Terminal
Gas Sensing Solutions Ltd. - UART Interface (9600 8N1)

Requires: pip install pyserial
"""

import sys
import time
import threading
import serial
import serial.tools.list_ports

# ── UART constants (from datasheet) ──────────────────────────────────────────
BAUD_RATE  = 9600
DATA_BITS  = serial.EIGHTBITS
PARITY     = serial.PARITY_NONE
STOP_BITS  = serial.STOPBITS_ONE
RX_TIMEOUT = 2.0   # seconds to wait for a response

# ── Globals ───────────────────────────────────────────────────────────────────
ser: serial.Serial | None = None
_stop_stream   = threading.Event()
_stream_thread: threading.Thread | None = None

# ─────────────────────────────────────────────────────────────────────────────
# Low-level serial helpers
# ─────────────────────────────────────────────────────────────────────────────

def _send(cmd: str) -> str:
    """Send ASCII command + CR LF, return stripped response (single line)."""
    if ser is None or not ser.is_open:
        return "ERROR: port not open"
    ser.reset_input_buffer()
    ser.write((cmd + "\r\n").encode("ascii"))
    deadline = time.time() + RX_TIMEOUT
    buf = ""
    while time.time() < deadline:
        if ser.in_waiting:
            buf += ser.read(ser.in_waiting).decode("ascii", errors="replace")
            if "\n" in buf:
                break
        time.sleep(0.02)
    return buf.strip()


def _send_multiline(cmd: str, expected_lines: int = 2) -> str:
    """Send command, collect multiple CR LF terminated response lines."""
    if ser is None or not ser.is_open:
        return "ERROR: port not open"
    ser.reset_input_buffer()
    ser.write((cmd + "\r\n").encode("ascii"))
    deadline = time.time() + RX_TIMEOUT
    buf = ""
    while time.time() < deadline:
        if ser.in_waiting:
            buf += ser.read(ser.in_waiting).decode("ascii", errors="replace")
            if buf.count("\n") >= expected_lines:
                break
        time.sleep(0.02)
    return buf.strip()


def _scaling_factor() -> int:
    """Query the '.' command and return the integer multiplier."""
    r = _send(".")
    try:
        return int(r.split()[-1])
    except Exception:
        return 1

# ─────────────────────────────────────────────────────────────────────────────
# Port management
# ─────────────────────────────────────────────────────────────────────────────

def port_status() -> str:
    if ser and ser.is_open:
        return f"OPEN  [{ser.port}  {BAUD_RATE} 8N1]"
    return "CLOSED"


def do_open_port():
    global ser
    print("\n  Available ports:")
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("    (none detected)")
    for p in ports:
        print(f"    {p.device:12s}  {p.description}")
    port = input("\n  Enter port name (e.g. COM3 or /dev/ttyUSB0): ").strip()
    if not port:
        print("  Cancelled.")
        return
    try:
        if ser and ser.is_open:
            ser.close()
        ser = serial.Serial(
            port=port, baudrate=BAUD_RATE, bytesize=DATA_BITS,
            parity=PARITY, stopbits=STOP_BITS, timeout=RX_TIMEOUT
        )
        # The sensor begins streaming on the first character received,
        # so discard any initial noise after opening.
        time.sleep(0.3)
        ser.reset_input_buffer()
        print(f"\n  Opened {ser.port} at {BAUD_RATE} baud  (8N1)")
    except Exception as exc:
        print(f"\n  ERROR: {exc}")
        ser = None


def do_close_port():
    global ser
    if ser and ser.is_open:
        ser.close()
        print("  Port closed.")
    else:
        print("  No port is currently open.")

# ─────────────────────────────────────────────────────────────────────────────
# Streaming-mode reader (background thread)
# ─────────────────────────────────────────────────────────────────────────────

def _stream_reader():
    """Continuously read and print lines while _stop_stream is not set."""
    sf = _scaling_factor()
    print(f"  [scaling factor = {sf}]")
    while not _stop_stream.is_set():
        if ser and ser.is_open and ser.in_waiting:
            try:
                raw = ser.readline().decode("ascii", errors="replace").strip()
                if raw:
                    # Try to show ppm alongside the raw value
                    parts = raw.split()
                    if len(parts) == 2 and parts[0] in ("Z", "z"):
                        try:
                            ppm = int(parts[1]) * sf
                            print(f"  {raw}   →  {ppm} ppm")
                        except ValueError:
                            print(f"  {raw}")
                    else:
                        print(f"  {raw}")
            except Exception:
                pass
        time.sleep(0.02)


def do_continuous_stream():
    """Put sensor in streaming mode and display live readings."""
    global _stream_thread
    print("\n  Switching to Mode 1 (streaming)…")
    r = _send("K 1")
    print(f"  Sensor: {r}")
    print("\n  Live readings  (press ENTER to stop)\n")
    _stop_stream.clear()
    _stream_thread = threading.Thread(target=_stream_reader, daemon=True)
    _stream_thread.start()
    input()
    _stop_stream.set()
    _stream_thread.join(timeout=1)
    print("\n  Streaming stopped.")


def do_timed_poll():
    """Poll filtered + unfiltered CO2 on a user-defined interval."""
    print("\n  Timed poll: reads both filtered (Z) and unfiltered (z) CO2 values")
    print("  at a fixed interval and prints them to the terminal.")

    # Get interval from user
    interval_str = input("\n  Enter poll interval in seconds (5–60, default 10): ").strip()
    try:
        interval = float(interval_str) if interval_str else 10.0
        if not 1.0 <= interval <= 3600.0:
            raise ValueError
    except ValueError:
        print("  Invalid interval – using 10 seconds.")
        interval = 10.0

    # Switch to polling mode so the sensor doesn't stream between polls
    print(f"\n  Switching to Mode 2 (polling)… interval = {interval:.1f} s")
    r = _send("K 2")
    print(f"  Mode response : {r}")
    time.sleep(0.1)

    sf = _scaling_factor()
    print(f"  Scaling factor : {sf}")
    print(f"\n  {'Time':10s}  {'Filtered (ppm)':>16s}  {'Unfiltered (ppm)':>18s}  {'Raw Z':>7s}  {'Raw z':>7s}")
    print("  " + "─" * 68)
    print("  Press ENTER to stop.\n")

    # Use an event so the Enter-key thread can signal the poll loop to stop
    stop_event = threading.Event()

    def _wait_for_enter():
        input()
        stop_event.set()

    watcher = threading.Thread(target=_wait_for_enter, daemon=True)
    watcher.start()

    reading_count = 0
    next_poll = time.time()

    while not stop_event.is_set():
        now = time.time()
        if now >= next_poll:
            next_poll = now + interval
            reading_count += 1
            ts = time.strftime("%H:%M:%S")

            # Request filtered then unfiltered
            rz = _send("Z")
            rz_low = _send("z")

            try:
                raw_Z = int(rz.split()[-1])
                ppm_filtered = raw_Z * sf
            except Exception:
                raw_Z, ppm_filtered = "?", "?"

            try:
                raw_z = int(rz_low.split()[-1])
                ppm_unfiltered = raw_z * sf
            except Exception:
                raw_z, ppm_unfiltered = "?", "?"

            print(f"  {ts:10s}  {str(ppm_filtered):>16s}  {str(ppm_unfiltered):>18s}  {str(raw_Z):>7s}  {str(raw_z):>7s}")

        # Sleep in small steps so we notice stop_event promptly
        time.sleep(0.1)

    print(f"\n  Polling stopped. {reading_count} reading(s) taken.")
    # Restore Mode 2 (was already in Mode 2, but make it explicit)
    _send("K 2")

# ─────────────────────────────────────────────────────────────────────────────
# Measurement commands
# ─────────────────────────────────────────────────────────────────────────────

def do_read_filtered():
    sf = _scaling_factor()
    r  = _send("Z")
    print(f"\n  Raw response : {r}")
    try:
        ppm = int(r.split()[-1]) * sf
        print(f"  CO₂ filtered : {ppm} ppm  (scale ×{sf})")
    except Exception:
        pass


def do_read_unfiltered():
    sf = _scaling_factor()
    r  = _send("z")
    print(f"\n  Raw response   : {r}")
    try:
        ppm = int(r.split()[-1]) * sf
        print(f"  CO₂ unfiltered : {ppm} ppm  (scale ×{sf})")
    except Exception:
        pass


def do_scaling_factor():
    r = _send(".")
    print(f"\n  Response : {r}")
    try:
        sf = int(r.split()[-1])
        print(f"  Scaling factor = {sf}  (multiply Z/z value by {sf} to get ppm)")
    except Exception:
        pass


def do_query_measurement():
    """Q command – reports fields defined by the current M mask."""
    r = _send("Q")
    print(f"\n  Response : {r}")
    # Try to decode Z and z fields if present
    sf = _scaling_factor()
    parts = r.split()
    i = 0
    while i < len(parts) - 1:
        if parts[i] in ("Z", "z"):
            try:
                ppm = int(parts[i + 1]) * sf
                label = "filtered" if parts[i] == "Z" else "unfiltered"
                print(f"  CO₂ ({label}) = {ppm} ppm")
            except Exception:
                pass
        i += 1


def do_set_measurement_mask():
    print("""
  Mask values (sum the ones you want):
    2048  d  LED signal normalised (filtered)
    1024  D  LED signal normalised (unfiltered)
     256  h  Zero set-point value
     128  V  Sensor temperature (unfiltered)
      32  o  LED signal strength (filtered)
      16  O  LED signal strength (unfiltered)
       8  v  Sensor temperature (filtered)
       4  Z  CO₂ filtered          ← most common
       2  z  CO₂ unfiltered        ← most common
    e.g. M=6 gives both CO₂ fields""")
    val = input("\n  Enter mask value: ").strip()
    try:
        v = int(val)
        r = _send(f"M {v}")
        print(f"  Response : {r}")
    except Exception:
        print("  Invalid value.")

# ─────────────────────────────────────────────────────────────────────────────
# Mode command
# ─────────────────────────────────────────────────────────────────────────────

def do_set_mode():
    print("""
  0  Sleep / Command mode  – no measurements, instant command response
  1  Streaming mode        – 2 readings/sec (factory default)
  2  Polling mode          – readings only when Q is sent""")
    mode = input("\n  Enter mode (0 / 1 / 2): ").strip()
    if mode not in ("0", "1", "2"):
        print("  Invalid choice.")
        return
    r = _send(f"K {mode}")
    print(f"  Response : {r}")

# ─────────────────────────────────────────────────────────────────────────────
# Digital filter
# ─────────────────────────────────────────────────────────────────────────────

def do_get_filter():
    r = _send("a")
    print(f"\n  Response : {r}  (range 0–255, default 16)")


def do_set_filter():
    print("\n  Higher value → smoother readings, slower response time.")
    print("  Recommended for calibration: 32")
    val = input("  Enter filter value (0–255): ").strip()
    try:
        v = int(val)
        if not 0 <= v <= 255:
            raise ValueError
        r = _send(f"A {v}")
        print(f"  Response : {r}")
    except ValueError:
        print("  Invalid value – must be 0 to 255.")

# ─────────────────────────────────────────────────────────────────────────────
# Zero / Calibration commands
# ─────────────────────────────────────────────────────────────────────────────

def _ensure_active_mode():
    """
    Zero-point commands are locked in Mode 0 (sleep).
    Switch to Mode 2 (polling) if the sensor is currently sleeping.
    Returns True if the sensor is (now) in an active mode.
    """
    # Use the '.' command as a lightweight probe – Mode 0 still answers it.
    # A cleaner check is to read the K state, but GSS has no 'k' read-back,
    # so we attempt a Z read and check for the 'locked' keyword.
    probe = _send("Z")
    if "locked" in probe.lower():
        print("  WARNING: sensor is in Mode 0 (sleep). Switching to Mode 2 (polling)…")
        r = _send("K 2")
        print(f"  Mode response : {r}")
        time.sleep(0.1)
        return True
    return True


def do_zero_fresh_air():
    _ensure_active_mode()
    sf = _scaling_factor()
    print(f"\n  G command: zero in fresh air (~400 ppm CO₂).")
    print(f"  Ensure sensor is in stable fresh air. (scaling factor = {sf})")
    print("  TIP: set filter to 32 first (option 10) for best accuracy.")
    if input("  Confirm? (y/n): ").strip().lower() != "y":
        print("  Cancelled.")
        return
    r = _send("G")
    print(f"  Response : {r}")


def do_zero_nitrogen():
    _ensure_active_mode()
    print("\n  U command: zero in nitrogen (0 ppm CO₂).")
    print("  Ensure sensor is in pure nitrogen and temperature is stable.")
    if input("  Confirm? (y/n): ").strip().lower() != "y":
        print("  Cancelled.")
        return
    r = _send("U")
    print(f"  Response : {r}")


def do_zero_known_gas():
    _ensure_active_mode()
    sf = _scaling_factor()
    print(f"\n  X command: zero with sensor in a known gas concentration.")
    print(f"  Scaling factor = {sf}  → value sent to sensor = ppm ÷ {sf}")
    ppm_str = input("  Enter known concentration in ppm: ").strip()
    try:
        ppm    = int(ppm_str)
        scaled = ppm // sf
        r = _send(f"X {scaled}")
        print(f"  Sent: X {scaled}  ({ppm} ppm ÷ {sf})")
        print(f"  Response : {r}")
    except ValueError:
        print("  Invalid value.")


def do_zero_fine_tune():
    _ensure_active_mode()
    sf = _scaling_factor()
    print(f"\n  F command: fine-tune zero using a known reading vs actual concentration.")
    print(f"  Scaling factor = {sf}  → enter values already divided by {sf}")
    try:
        reported_ppm = int(input("  Sensor reported concentration (ppm): ").strip())
        actual_ppm   = int(input("  Actual / known concentration   (ppm): ").strip())
        r_scaled = reported_ppm // sf
        a_scaled = actual_ppm   // sf
        r = _send(f"F {r_scaled} {a_scaled}")
        print(f"  Sent: F {r_scaled} {a_scaled}")
        print(f"  Response : {r}")
    except ValueError:
        print("  Invalid values.")


def do_zero_manual():
    _ensure_active_mode()
    print("\n  u command: force a specific internal zero-point value.")
    print("  (Advanced – see datasheet. Use with caution.)")
    val = input("  Enter zero-point value: ").strip()
    try:
        v = int(val)
        r = _send(f"u {v}")
        print(f"  Response : {r}")
    except ValueError:
        print("  Invalid value.")

# ─────────────────────────────────────────────────────────────────────────────
# Auto-zero
# ─────────────────────────────────────────────────────────────────────────────

def do_get_autozero():
    r = _send("@")
    print(f"\n  Response : {r}")
    if r.strip() in ("@ 0", "@0"):
        print("  Auto-zero is currently DISABLED.")


def do_set_autozero():
    print("""
  @ command: configure auto-zero intervals.
    Enter '0'         to disable auto-zero.
    Enter 'X.X Y.Y'  to set initial and regular interval in days.
    Example: 1.0 8.0  → first auto-zero after 1 day, then every 8 days.""")
    val = input("\n  Enter value(s): ").strip()
    if val == "0":
        r = _send("@ 0")
    else:
        r = _send(f"@ {val}")
    print(f"  Response : {r}")


def _set_p_command(reg_msb: int, reg_lsb: int, label: str):
    """Shared helper for P 8/9 (auto-zero) and P 10/11 (fresh-air zero)."""
    sf = _scaling_factor()
    print(f"\n  Set {label} CO₂ concentration.")
    print(f"  (Scaling factor = {sf} – enter the ACTUAL ppm you want stored)")
    ppm_str = input("  Enter concentration in ppm (default 400): ").strip()
    try:
        ppm = int(ppm_str)
        msb = ppm // 256
        lsb = ppm - (256 * msb)
        r1 = _send(f"P {reg_msb} {msb}")
        r2 = _send(f"P {reg_lsb} {lsb}")
        print(f"  Sent P {reg_msb} {msb}  →  {r1}")
        print(f"  Sent P {reg_lsb} {lsb}  →  {r2}")
        print(f"  Stored {ppm} ppm  (MSB={msb}, LSB={lsb})")
    except ValueError:
        print("  Invalid value.")


def do_set_autozero_ppm():
    _set_p_command(8, 9, "auto-zero")


def do_set_freshair_ppm():
    _set_p_command(10, 11, "fresh-air zero-point")

# ─────────────────────────────────────────────────────────────────────────────
# Pressure / altitude compensation
# ─────────────────────────────────────────────────────────────────────────────

def _calc_compensation(altitude_ft: float) -> int:
    """
    Compensation Value = 8192 + (SeaLevelDiff * 0.14 / 100) * 8192
    Pressure drops ~0.14% per mbar; ~1 mbar per 27 ft at low altitude.
    """
    mbar_per_ft = 1.0 / 27.0
    sea_level_diff = altitude_ft * mbar_per_ft
    return int(8192 + (sea_level_diff * 0.14 / 100) * 8192)


def do_get_pressure_comp():
    r = _send("s")
    print(f"\n  Response : {r}  (default = 8192 at sea level)")


def do_set_pressure_comp():
    print("""
  S command: set pressure / concentration compensation value.
  Default = 8192 (sea level, 1013 mbar).
  Formula: 8192 + (SeaLevelPressureDiff_mbar × 0.14 / 100) × 8192

  Quick altitude reference:
    Sea level  (0 ft / 0 m)       → 8192
    1000 ft  / 305 m              → 8605
    2000 ft  / 610 m              → 9006
    5000 ft  / 1525 m             → 10142
    10000 ft / 3050 m             → 11816

  You can enter:
    a) A direct compensation value, OR
    b) Your altitude in feet (the value will be calculated for you)""")
    choice = input("\n  Enter (a) direct value or (b) altitude in feet: ").strip().lower()
    if choice == "b":
        try:
            alt = float(input("  Enter altitude in feet: ").strip())
            comp = _calc_compensation(alt)
            print(f"  Calculated compensation value: {comp}")
        except ValueError:
            print("  Invalid altitude.")
            return
    else:
        try:
            comp = int(input("  Enter compensation value (0–65536): ").strip())
        except ValueError:
            print("  Invalid value.")
            return
    if not 0 <= comp <= 65536:
        print("  Value out of range.")
        return
    r = _send(f"S {comp}")
    print(f"  Response : {r}")

# ─────────────────────────────────────────────────────────────────────────────
# Firmware / serial info
# ─────────────────────────────────────────────────────────────────────────────

def do_firmware_info():
    print("\n  Y command requires the sensor to be in Mode 0 (sleep/command).")
    print("  Switching to Mode 0 first…")
    r = _send("K 0")
    print(f"  Mode response : {r}")
    time.sleep(0.2)
    r = _send_multiline("Y", expected_lines=2)
    print(f"\n  Firmware / Serial response:\n")
    for line in r.splitlines():
        print(f"    {line}")
    # Restore Mode 2 (polling) so calibration and read commands work afterwards.
    print("\n  Restoring Mode 2 (polling)…")
    r = _send("K 2")
    print(f"  Mode response : {r}")

# ─────────────────────────────────────────────────────────────────────────────
# Raw command passthrough
# ─────────────────────────────────────────────────────────────────────────────

def do_raw_command():
    print("\n  Enter the command exactly as it should be sent.")
    print("  CR+LF will be appended automatically.")
    print("  Use 'multi' prefix to collect 2 response lines (e.g. 'multi Y')")
    raw = input("  Command: ").strip()
    if not raw:
        return
    if raw.lower().startswith("multi "):
        cmd = raw[6:].strip()
        r = _send_multiline(cmd, expected_lines=2)
    else:
        r = _send(raw)
    print(f"\n  Response : {r}")

# ─────────────────────────────────────────────────────────────────────────────
# UI helpers
# ─────────────────────────────────────────────────────────────────────────────

DIVIDER = "  " + "─" * 56

def _header():
    print()
    print("  " + "═" * 56)
    print("       ExplorIR®-M  CO₂  Sensor  Terminal")
    print("       Gas Sensing Solutions Ltd.")
    print("  " + "═" * 56)


def _menu():
    print(f"\n  Port status : {port_status()}")
    print(DIVIDER)
    print("  PORT")
    print("   1   Open COM port")
    print("   2   Close COM port")
    print(DIVIDER)
    print("  MEASUREMENTS")
    print("   3   Read CO₂ – filtered        (Z)")
    print("   4   Read CO₂ – unfiltered      (z)")
    print("   5   Get scaling factor          (.)")
    print("   6   Query data fields           (Q)")
    print("   7   Set measurement mask        (M)")
    print("   8   Continuous live stream      (K 1 + stream)")
    print("  25   Timed poll  (filtered + unfiltered, every N sec)")
    print(DIVIDER)
    print("  SENSOR MODE")
    print("   9   Set operating mode         (K 0/1/2)")
    print(DIVIDER)
    print("  DIGITAL FILTER")
    print("  10   Get filter value            (a)")
    print("  11   Set filter value            (A)")
    print(DIVIDER)
    print("  ZERO / CALIBRATION")
    print("  12   Zero in fresh air           (G)")
    print("  13   Zero in nitrogen            (U)")
    print("  14   Zero in known concentration (X)")
    print("  15   Fine-tune zero point        (F)")
    print("  16   Manual zero point set       (u)")
    print(DIVIDER)
    print("  AUTO-ZERO")
    print("  17   Get auto-zero config        (@)")
    print("  18   Set auto-zero intervals     (@)")
    print("  19   Set auto-zero CO₂ level     (P 8/9)")
    print(DIVIDER)
    print("  FRESH-AIR ZERO LEVEL")
    print("  20   Set fresh-air zero CO₂ level (P 10/11)")
    print(DIVIDER)
    print("  PRESSURE / ALTITUDE COMPENSATION")
    print("  21   Get compensation value      (s)")
    print("  22   Set compensation value      (S)")
    print(DIVIDER)
    print("  INFO")
    print("  23   Firmware version & serial   (Y)")
    print(DIVIDER)
    print("  ADVANCED")
    print("  24   Send raw command")
    print(DIVIDER)
    print("   0   Exit")
    print()


ACTIONS = {
    "1":  do_open_port,
    "2":  do_close_port,
    "3":  do_read_filtered,
    "4":  do_read_unfiltered,
    "5":  do_scaling_factor,
    "6":  do_query_measurement,
    "7":  do_set_measurement_mask,
    "8":  do_continuous_stream,
    "9":  do_set_mode,
    "10": do_get_filter,
    "11": do_set_filter,
    "12": do_zero_fresh_air,
    "13": do_zero_nitrogen,
    "14": do_zero_known_gas,
    "15": do_zero_fine_tune,
    "16": do_zero_manual,
    "17": do_get_autozero,
    "18": do_set_autozero,
    "19": do_set_autozero_ppm,
    "20": do_set_freshair_ppm,
    "21": do_get_pressure_comp,
    "22": do_set_pressure_comp,
    "23": do_firmware_info,
    "24": do_raw_command,
    "25": do_timed_poll,
}

PORT_REQUIRED = set(ACTIONS.keys()) - {"1"}


def main():
    _header()
    while True:
        _menu()
        choice = input("  Select option: ").strip()

        if choice == "0":
            if ser and ser.is_open:
                ser.close()
            print("\n  Goodbye!\n")
            sys.exit(0)

        if choice not in ACTIONS:
            print("  Unknown option – please try again.")
            continue

        if choice in PORT_REQUIRED and (ser is None or not ser.is_open):
            print("\n  No port is open.  Use option 1 to open a port first.")
            input("  Press ENTER to continue…")
            continue

        try:
            ACTIONS[choice]()
        except serial.SerialException as exc:
            print(f"\n  Serial error: {exc}")
        except KeyboardInterrupt:
            print("\n  Interrupted.")

        input("\n  Press ENTER to continue…")


if __name__ == "__main__":
    main()
