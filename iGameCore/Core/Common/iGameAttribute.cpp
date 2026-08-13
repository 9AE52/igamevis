/**
 * @class   iGameAttribute
 * @brief   iGameAttribute's brief
 */

#include "iGameAttribute.h"

iGame::Attribute iGame::Attribute::None(){
    Attribute att;
    att.pointer = nullptr;
    att.type = IG_NONE;
    att.attachmentType = IG_NONE;
    att.isDeleted = false;
    return att;
}

bool iGame::Attribute::isNone() const {
    return pointer == nullptr || type == IG_NONE || attachmentType == IG_NONE || isDeleted;
}