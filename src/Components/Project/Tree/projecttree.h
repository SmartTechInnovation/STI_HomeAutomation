#ifndef PROJECTTREE_CLASS_H
#define PROJECTTREE_CLASS_H

#include <QObject>
#include <QStandardItemModel>

#include "../../PeripheryManager/xmlnode.h"

class Project_class;

class ProjectTree_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *tree READ getTree)
public: /* Typedefs and enums */
    enum Roles_e{
        RoleDisplay = Qt::UserRole, // Default
        RoleTitle,                  // Title
        RoleUuid,                   // Id of model
        RoleIUuid,                  // Unique identifier
        RoleIcon,                   // Icon Path
        RoleColor,                  // Icon Color
        RoleDraggable,              // Item Draggable
    };
private: /* Members           */
    Project_class      *m_Parent       = nullptr; //Used to extract project data
    QStandardItemModel *m_ptr_Root     = nullptr;
    bool                m_b_Tree_Dirty = false;

public:  /* Members           */

private: /* Function          */
    QStandardItem *getItem(const QString &title,
                           const QUuid   &uuid,
                           const QUuid   &iUuid,
                           const QString &icon,
                           const QColor  &color = QColor(0xF7F7F7),
                           bool  draggable = false);
    void parceSection(XmlNode_class *xmlRoot, QStandardItem *itemRoot);

public:  /* Functions         */
    explicit ProjectTree_class(QObject *parent = nullptr);
    ~ProjectTree_class();

    void build();

    QObject *getTree() const { return m_ptr_Root; };
signals:
    void treeChanged();
};

#endif // PROJECTTREE_CLASS_H
