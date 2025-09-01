//
// Created by redha on 9/1/2025.
//

#ifndef STALEMATE_STORAGEACTIONITEM_H
#define STALEMATE_STORAGEACTIONITEM_H

#include <Arduino.h>
#include "Storage.h"
#include "Menu/MenuAction.h"

class StorageActionItem : public MenuAction {
public:
    StorageActionItem(const __FlashStringHelper *title) : MenuAction(title) {}
    void setStorage(Storage* storage) { this->storage = storage; }

protected:
    Storage* getStorage() const { return storage; }

private:
    Storage* storage;
};


#endif //STALEMATE_STORAGEACTIONITEM_H
