#ifndef PROJECTTREE_CLASS_H
#define PROJECTTREE_CLASS_H

#include <QObject>
#include <QAbstractItemModel>

#include "../../Document/xmlnode.h"
#include "../../SchemaManager/schemamanager.h"

class Project_class;

class TreeItem_class{
public: /* Typedef and enums */
    enum Kind_e{
        KindRoot,
        KindNode,
        KindGroup,
        KindRef
    };
public: /* Members */
    Kind_e          Kind = KindNode;
    XmlNode_class  *Node  = nullptr;
    TreeItem_class *Parent = nullptr;
    QVector<TreeItem_class *> Children;
public: /* Functions */
    TreeItem_class() {};
    ~TreeItem_class(){
        qDeleteAll(Children);
    }
};

class ProjectTree_class : public QAbstractItemModel
{
    Q_OBJECT
public: /* Typedefs and enums */
    enum Roles_e{
        RoleTitle = Qt::UserRole + 1,
        RoleUuid,           // type id
        RoleIUuid,          // instance id (empty for groups)
        RoleIcon,           // row Icon
        RoleColor,          // row Color
        RoleKind,           // node, group, ref, unknown
        RoleIoType,         // in, out or empty
        RoleDragEnabled,    // output channels can be placed only once
        RoleDescription,    // Description of Node
        RoleEditable,       // can be renamed / deleted (schema)
    };
    typedef SchemaManager_class::NodeType_s NodeType_s;

private: /* Members           */
    Project_class      *m_ptr_Project  = nullptr; //Used to extract project data
    TreeItem_class     *m_ptr_TreeRoot = nullptr;
    QHash<XmlNode_class *, TreeItem_class *>  m_hash_Items; // Node items only
    QHash<XmlNode_class *, TreeItem_class *>  m_hash_Refs;  // Ref items, by SignalRef node
public:  /* Members           */

private: /* Function          */
    TreeItem_class *itemFor(const QModelIndex &index) const;
    QModelIndex     indexFor(TreeItem_class *item)    const;

    QVector<XmlNode_class  *> visibleChildren(XmlNode_class  *node) const;
    void syncNode(XmlNode_class *node);
    void syncRefsOf(const QString &targetIUuid);

    void sync  (TreeItem_class *item);
    void forget(TreeItem_class *item);

    void onReset();
    void onInserted(XmlNode_class *parent, XmlNode_class *node);
    void onRemoved (XmlNode_class *parent, XmlNode_class *node);
    void onAttributeChanged(XmlNode_class *node, const QString &key);

public:  /* Functions         */
    explicit ProjectTree_class(Project_class *project = nullptr);
    ~ProjectTree_class();

    /* ===== Mandatory for QAbstract List ====== */
    QModelIndex index (int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child)                                       const override;
    int         rowCount   (const QModelIndex &parent = QModelIndex())                 const override;
    int         columnCount(const QModelIndex &parent = QModelIndex())                 const override;
    QVariant    data(const QModelIndex &index, int role)                               const override;
    QHash<int, QByteArray> roleNames()                                                 const override;

    /* ===== Qml ===== */
    Q_INVOKABLE QModelIndex indexOf(const QString &iUuid) const;
signals:

};

#endif // PROJECTTREE_CLASS_H
