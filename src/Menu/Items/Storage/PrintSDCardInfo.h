//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_PRINTSDCARDINFO_H
#define STALEMATE_PRINTSDCARDINFO_H

#include "StorageActionItem.h"

class PrintSDCardInfo : public StorageActionItem {
public:
    PrintSDCardInfo() : StorageActionItem(F("Print SD Card Info")) {}

private:
    void doAction() override {
        getStorage()->printCardInfo();
    }
};


#endif //STALEMATE_PRINTSDCARDINFO_H
