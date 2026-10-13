#include "projecttree.h"

#include "../project.h"

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

}

void ProjectTree_class::parceSection(XmlNode_class *xmlRoot, QStandardItem *itemRoot){

}