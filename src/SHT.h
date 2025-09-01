//
// Created by redha on 3/10/2025.
//

#ifndef ETHAN1_SHT_H
#define ETHAN1_SHT_H


class SHT {
public:
    SHT();
    void init();
    void update();

    void clear();

    [[nodiscard]] float getTempC() const;
    [[nodiscard]] float getTempF() const;
    [[nodiscard]] float getHumidity() const;

private:
    unsigned long lastHeaterEnable = 0;
    bool isHeaterEnabled = false;

    float tempC;
    float humidity;
};


#endif //ETHAN1_SHT_H
