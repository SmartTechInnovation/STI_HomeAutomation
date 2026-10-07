#include "projecttree.h"

#include "../project.h"

#include "../../PeripheryManager/peripherymanager.h"
#include "src/Components/PeripheryManager/peripherymanager_tokens.h"

ProjectTree_class::ProjectTree_class(QObject *parent) : QObject(parent) {
    m_Parent = (Project_class *)parent;
    m_ptr_Root = new QStandardItemModel(this);
    m_ptr_Root->setItemRoleNames({
        { Roles_e::RoleDisplay,   "display"   },
        { Roles_e::RoleTitle,     "title"     },
        { Roles_e::RoleUuid,      "uuid"      },
        { Roles_e::RoleIUuid,     "iUuid"     },
        { Roles_e::RoleIcon,      "icon"      },
        { Roles_e::RoleColor,     "color"     },
        { Roles_e::RoleDraggable, "draggable" },
    });
}

ProjectTree_class::~ProjectTree_class(){

}

QStandardItem *ProjectTree_class::getItem(const QString &title,
                       const QUuid   &uuid,
                       const QUuid   &iUuid,
                       const QString &icon,
                       const QColor  &color,
                       bool  draggable)
{
    QStandardItem *newItem = new QStandardItem(title);
    newItem->setEditable(false);
    newItem->setData(title,     RoleTitle    );
    newItem->setData(uuid,      RoleUuid     );
    newItem->setData(iUuid,     RoleIUuid    );
    newItem->setData(icon,      RoleIcon     );
    newItem->setData(color,     RoleColor    );
    newItem->setData(draggable, RoleDraggable);

    return newItem;
}

void ProjectTree_class::build(){
    if(!m_ptr_Root) return;
    if(!m_Parent  ) return;

    m_ptr_Root->clear();
    QStandardItem *RowProject = getItem(m_Parent->m_MetaData.infoData.Title, QUuid(), m_Parent->m_MetaData.instanceUuid, "qrc:/icons/navigation/ui-panel-left.svg");
    /* ==== Rooms ==== */
    QStandardItem *RowRooms   = getItem(m_Parent->m_RoomsTree.Title, m_Parent->m_RoomsTree.Uuid, m_Parent->m_RoomsTree.IUuid, "qrc:/icons/misc/layers-3.svg");
    for(auto &room : m_Parent->m_RoomsTree.m_vec_Rooms){
        RowRooms->appendRow(getItem(room->Title, room->Uuid, room->IUuid, !room->Icon.isEmpty() ? room->Icon : "qrc:/icons/misc/layer-1.svg"));
    }
    RowProject->appendRow(RowRooms);
    /* ==== Categories ==== */
    QStandardItem *RowCategories = getItem(m_Parent->m_CategoriesTree.Title, m_Parent->m_CategoriesTree.Uuid, m_Parent->m_CategoriesTree.IUuid, "qrc:/icons/misc/library.svg");
    for(auto &category : m_Parent->m_CategoriesTree.m_vec_Categories){
        RowCategories->appendRow(getItem(category->Title, category->Uuid, category->IUuid, !category->Icon.isEmpty() ? category->Icon : "qrc:/icons/misc/tag.svg"));
    }
    RowProject->appendRow(RowCategories);
    /* ====    Users, Rights, User Groups  ==== */
        /* ====    Users ===== */
        QStandardItem *RowUsers = getItem(m_Parent->m_UsersTree.Title,       m_Parent->m_UsersTree.Uuid, m_Parent->m_UsersTree.IUuid, "qrc:/icons/misc/users.svg");
        for(auto &user : m_Parent->m_UsersTree.m_vec_Users){
            RowUsers->appendRow(getItem(user->Username,       user->Uuid, user->IUuid, "qrc:/icons/misc/user.svg"));
        }
        RowProject->appendRow(RowUsers);
        /* ====   Rights   ===== */
        QStandardItem *RowRights = getItem(m_Parent->m_RightsTree.Title,     m_Parent->m_RightsTree.Uuid, m_Parent->m_RightsTree.IUuid, "qrc:/icons/misc/password.svg");
        for(auto &right : m_Parent->m_RightsTree.m_vec_Rights){
            RowRights->appendRow(getItem(right->Title,        right->Uuid, right->IUuid, "qrc:/icons/misc/password.svg"));
        }
        RowUsers->appendRow(RowRights);
        /* ====   User Group   ===== */
        QStandardItem *RowUserGroup = getItem(m_Parent->m_UserGroupsTree.Title,   m_Parent->m_UserGroupsTree.Uuid, m_Parent->m_UserGroupsTree.IUuid, "qrc:/icons/misc/users.svg");
        for(auto &userGroup : m_Parent->m_UserGroupsTree.m_vec_UserGroups){
            RowUserGroup->appendRow(getItem(userGroup->Title,        userGroup->Uuid, userGroup->IUuid, "qrc:/icons/misc/users.svg"));
        }
        RowUsers->appendRow(RowUserGroup);
    /* ======================================== */
    /*  ====== Managed Tablets ====== */
    QStandardItem *RowManagedTablets = getItem(m_Parent->m_ManagedTabletsTree.Title, m_Parent->m_ManagedTabletsTree.Uuid, m_Parent->m_ManagedTabletsTree.IUuid, "qrc:/icons/software/desktop.svg");
    for(auto &tablet : m_Parent->m_ManagedTabletsTree.m_vec_ManagedTablets){
        RowManagedTablets->appendRow(getItem(tablet->Title,         tablet->Uuid, tablet->IUuid, "qrc:/icons/software/desktop.svg"));
    }
    RowProject->appendRow(RowManagedTablets);
    /* ======     Variables    ====== */
    QStandardItem *RowVariables = getItem(m_Parent->m_VariablesTree.Title, m_Parent->m_VariablesTree.Uuid, m_Parent->m_VariablesTree.IUuid, "qrc:/icons/misc/blocks.svg");
    for(auto &variableMap : m_Parent->m_VariablesTree.m_vec_VariablesType){
        Port_class::PortType_e VariableType = Port_class::fromString(variableMap->Title);
        QString Icon = "qrc:/icons/misc/variable.svg";
        switch(VariableType){
        case Port_class::PortType_Void:     Icon = "qrc:/icons/misc/variable.svg";       break;
        case Port_class::PortType_Bool:     Icon = "qrc:/icons/software/bool.svg";       break;
        case Port_class::PortType_Int:      Icon = "qrc:/icons/signals/wave-square.svg"; break;
        case Port_class::PortType_Float:    Icon = "qrc:/icons/signals/wave-sine.svg";   break;
        case Port_class::PortType_String:   Icon = "qrc:/icons/software/text.svg";       break;
        case Port_class::PortType_Enum:     Icon = "qrc:/icons/misc/library.svg";        break;
        case Port_class::PortType_Date:     Icon = "qrc:/icons/misc/calendar.svg";       break;
        case Port_class::PortType_Custom:   Icon = "qrc:/icons/misc/blocks.svg";         break;
        }
        QStandardItem *RowVariableType = getItem(variableMap->Title, variableMap->Uuid, variableMap->IUuid, Icon, Port_class::getColor(VariableType));
        for(auto &variable : variableMap->m_vec_Variables){
            RowVariableType->appendRow(getItem(variable->Title,     variable->Uuid, variable->IUuid, Icon, Port_class::getColor(VariableType), true));
        }
        RowVariables->appendRow(RowVariableType);
    }
    RowProject->appendRow(RowVariables);
    /* ===== Root Device ======= */
    QStandardItem *RowDevice    = getItem(m_Parent->m_Controller.Title, m_Parent->m_Controller.Uuid, m_Parent->m_Controller.IUuid, "qrc:/icons/hardware/server.svg");
    for(auto &section : m_Parent->m_Controller.m_vec_Periphery){
        parceSection(section, RowDevice);
    }
    RowProject->appendRow(RowDevice);

    m_ptr_Root->appendRow(RowProject);
    emit m_Parent->treeChanged();
}

void ProjectTree_class::parceSection(XmlNode_class *xmlRoot, QStandardItem *itemRoot){
    if(xmlRoot){
        XmlNode_class *nodeRef = S_PeripheryManager.getNode(xmlRoot->Tag);
        if(nodeRef){
            QStandardItem *RowNode = nullptr;
            QUuid instanceUuid = QUuid().createUuidV7();
            QString iUuidXml = xmlRoot->value(ATTR_IUUID);
            if(!iUuidXml.isEmpty()){
                instanceUuid = QUuid(iUuidXml);
            }
            if(xmlRoot->hasAttr(ATTR_UUID)){
                RowNode = getItem(xmlRoot->value(ATTR_TITLE), QUuid(xmlRoot->value(ATTR_UUID)), instanceUuid, nodeRef->value(ATTR_ICON));
            }else{
                RowNode = getItem(xmlRoot->value(ATTR_TITLE), QUuid(nodeRef->value(ATTR_UUID)), instanceUuid, nodeRef->value(ATTR_ICON));
            }
            if(itemRoot)
                itemRoot->appendRow(RowNode);
            for(auto &child : xmlRoot->Children){
                parceSection(child, RowNode);
            }
        }
    }
}