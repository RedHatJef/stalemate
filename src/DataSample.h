//
// Created by redha on 3/12/2025.
//

#ifndef STALEMATE_DATASAMPLE_H
#define STALEMATE_DATASAMPLE_H


class DataSample {
public:
    DataSample();

    char date[11]; // 03/12/2025 = 10 + null = 11
    char time[9]; // 18:12:34 = 8 + null = 9
    float bmp390TempC;
    float sht31TempC;
    float sht31HumidityPercent;
    unsigned int co2PPM;

    void clear();

    static const char* tableHeader;
};


#endif //STALEMATE_DATASAMPLE_H
