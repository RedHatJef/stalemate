//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_SCD40_CO2_H
#define ETHAN1_SCD40_CO2_H


class SCD40_CO2 {
public:
    SCD40_CO2();
    void init(float altitudeM);
    void update();
    void clear();

    bool runSelfTest();

    [[nodiscard]] unsigned int getCO2PPM() const;

private:
    unsigned int co2ppm;
};


#endif //ETHAN1_SCD40_CO2_H
