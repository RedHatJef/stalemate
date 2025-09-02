//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_STORAGEMENU_H
#define STALEMATE_STORAGEMENU_H

#include "Menu/Menu.h"
#include "Devices/Storage.h"
#include "PrintSDCardFiles.h"
#include "PrintSDCardInfo.h"

class StorageMenu : public Menu {
public:
    StorageMenu() : Menu(F("Storage Menu")) {
        addMenuOption(&printSdCardInfo);
        addMenuOption(&printSdCardFiles);
    }

    void setStorage(Storage* s) {
        storage = s;
        printSdCardInfo.setStorage(s);
        printSdCardFiles.setStorage(s);
    }

private:
    Storage* storage;
    PrintSDCardInfo printSdCardInfo;
    PrintSDCardFiles printSdCardFiles;
};

#endif //STALEMATE_STORAGEMENU_H
