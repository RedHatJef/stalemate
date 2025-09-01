//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_PRINTSDCARDFILES_H
#define STALEMATE_PRINTSDCARDFILES_H

#include "StorageActionItem.h"

class PrintSDCardFiles : public StorageActionItem {
public:
    PrintSDCardFiles() : StorageActionItem(F("List files on SD card")) {}

private:
    void doAction() override {
        getStorage()->printFiles();
    }
};

#endif //STALEMATE_PRINTSDCARDFILES_H
