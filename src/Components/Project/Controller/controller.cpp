#include "controller.h"

#include "../../PeripheryManager/peripherymanager.h"

Controller_class::Controller_class() {}

QVariantMap Controller_class::getContextMenu(const QUuid &uuid) const{
    QVariantMap Result;

    Result = S_PeripheryManager.getSubMenu(uuid);

    return Result;
}