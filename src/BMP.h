//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_BMP_H
#define ETHAN1_BMP_H


class BMP {
public:
    BMP();
    void init();
    void update();

    void clear();

    [[nodiscard]] float getTempC() const;
    [[nodiscard]] float getTempF() const;
    [[nodiscard]] float getPresHPA() const;
    [[nodiscard]] float getAltitudeM() const;
    [[nodiscard]] float getAltitudeF() const;

private:
    float tempC;
    float presHPA;
    float altitudeM;
};


#endif //ETHAN1_BMP_H
