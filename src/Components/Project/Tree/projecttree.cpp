#include "projecttree.h"

#include "../project.h"
#include "../../Document/document_tokens.h"

ProjectTree_class::ProjectTree_class(Project_class *project) : QAbstractItemModel(project) {
    m_ptr_Project = project;

    m_ptr_TreeRoot       = new TreeItem_class();
    m_ptr_TreeRoot->Kind = TreeItem_class::KindRoot;


    connect(project, &Project_class::documentReset,    this, &ProjectTree_class::onReset);
    connect(project, &Project_class::nodeInserted,     this, &ProjectTree_class::onInserted);
    connect(project, &Project_class::nodeRemoved,      this, &ProjectTree_class::onRemoved);
    connect(project, &Project_class::attributeChanged, this, &ProjectTree_class::onAttributeChanged);
}

ProjectTree_class::~ProjectTree_class(){
    delete m_ptr_TreeRoot;
}

/* ===================== Structure ===================== */

QVector<XmlNode_class *> ProjectTree_class::visibleChildren(XmlNode_class *node) const {
    QVector<XmlNode_class *> Result;
    const SchemaManager_class::NodeType_s *templateNode = S_SchemaManager.getNode(node);
    for(auto *child : node->children()){
        const SchemaManager_class::NodeType_s *templateChild = S_SchemaManager.getNode(child);
        if(templateChild){
            if(!templateChild->Hidden){
                Result.append(child);
            }
        }else if(child->TagName != TOKEN_PROGRAM &&
                 child->TagName != TOKEN_SOURCE  &&
                 templateNode   != nullptr){
            Result.append(child);
        }
    }
    return Result;
}

void ProjectTree_class::forget(TreeItem_class *item){
    if(item->Kind == TreeItem_class::KindNode && m_hash_Items.value(item->Node) == item){
        m_hash_Items.remove(item->Node);
    }
    for(auto &child : item->Children)
        forget(child);
}

/* Brings the children of an item in line with the document (remove / move / insert, no reset) */
void ProjectTree_class::sync(TreeItem_class *item){
    QModelIndex modelIndex = indexFor(item);

    /* Clean unexistent xml Nodes from Tree */
    for(int index = item->Children.size() - 1; index >= 0; --index){
        TreeItem_class *child = item->Children.at(index);
        if(!item->Node->children().contains(child->Node)){
            beginRemoveRows(modelIndex, index, index);
            TreeItem_class *takeChild = item->Children.takeAt(index);
            forget(takeChild);
            delete takeChild;
            endRemoveRows();
        }
    }

    QVector<XmlNode_class *> visChildren = visibleChildren(item->Node);

    for(auto &child : visChildren){
        int foundIndex = -1;
        for(auto &treeNode : item->Children){
            if(child == treeNode->Node){ // Node Already Exist
                foundIndex = item->Children.indexOf(treeNode);
                break;
            }
        }
        int xmlChildIndex = visChildren.indexOf(child);
        if(foundIndex >= 0 && foundIndex != xmlChildIndex ){ /* if node index is diferent than existing one */
            beginMoveRows(modelIndex, foundIndex, foundIndex, modelIndex, xmlChildIndex);
            item->Children.move(foundIndex, xmlChildIndex);
            endMoveRows();
        }else if(foundIndex == -1){
            beginInsertRows(modelIndex, xmlChildIndex, xmlChildIndex);
            TreeItem_class *newTreeItem = new TreeItem_class();
            newTreeItem->Kind   = TreeItem_class::KindNode;
            newTreeItem->Node   = child;
            newTreeItem->Parent = item;
            m_hash_Items[child] = newTreeItem;
            item->Children.insert(xmlChildIndex, newTreeItem);
            endInsertRows();
        }
    }
    for(auto &child : item->Children)
        sync(child);

}

void ProjectTree_class::syncNode(XmlNode_class *node){
    TreeItem_class *item = m_hash_Items.value(node, nullptr);
    if(item == nullptr)
        return;
    sync(item);
    const QModelIndex index = indexFor(item);
    emit dataChanged(index, index);
}

void ProjectTree_class::syncRefsOf(const QString &targetIUuid){
    if(XmlNode_class *target = m_ptr_Project->getNode(targetIUuid))
        syncNode(target);
}

/* ===================== Document changes ===================== */

void ProjectTree_class::onReset(){
    beginResetModel();
    qDeleteAll(m_ptr_TreeRoot->Children);
    m_ptr_TreeRoot->Children.clear();
    m_hash_Items.clear();
    m_hash_Refs.clear();

    TreeItem_class *rootProject = new TreeItem_class();
    rootProject->Kind   = TreeItem_class::KindNode;
    rootProject->Node   = m_ptr_Project->getRoot();
    rootProject->Parent = m_ptr_TreeRoot;
    m_ptr_TreeRoot->Children.push_back(rootProject);

    m_hash_Items[rootProject->Node] = rootProject;

    sync(rootProject);

    endResetModel();
}

void ProjectTree_class::onInserted(XmlNode_class *parent, XmlNode_class *node){
    syncNode(parent);
    //syncNode(node->parent());
    //node->visit([this](XmlNode_class *n){
    //    if(Project_class::isSignalRef(n))
    //        syncRefsOf(n->value(ATTR_IO));
    //});
}

void ProjectTree_class::onRemoved(XmlNode_class *parent, XmlNode_class *node){
    syncNode(parent);
    //node->visit([this](XmlNode_class *n){
    //    if(Project_class::isSignalRef(n))
    //        syncRefsOf(n->value(ATTR_IO));
    //});
}

void ProjectTree_class::onAttributeChanged(XmlNode_class *node, const QString &key){
    Q_UNUSED(key)
    /* only refresh roles (title, icon, ...) - dataChanged does not force a re-layout */
    if(TreeItem_class *item = m_hash_Items.value(node, nullptr)){
        const QModelIndex index = indexFor(item);
        emit dataChanged(index, index);
    }
    //if(Item_s *item = m_hash_Items.value(node, nullptr)){
    //    const QModelIndex index = indexFor(item);
    //    emit dataChanged(index, index);
    //}
    ///* grouping attribute (Variable Type) */
    //const auto *parentType = node->parent() ? m_ptr_Project->typeOf(node->parent()) : nullptr;
    //if(parentType && parentType->GroupBy == key)
    //    syncNode(node->parent());

    ///* page renamed -> ref rows show the page title */
    //if(node->Tag == TOKEN_PAGE){
    //    for(auto it = m_hash_Refs.begin(); it != m_hash_Refs.end(); ++it){
    //        if(it.key()->parent() == node){
    //            const QModelIndex index = indexFor(it.value());
    //            emit dataChanged(index, index);
    //        }
    //    }
    //}
    //if(key == ATTR_IO){
    //    const auto items = m_hash_Items;
    //    for(auto it = items.begin(); it != items.end(); ++it)
    //        if(m_hash_Items.contains(it.key())) sync(it.value());
    //}
}

/* ===================== Model ===================== */

TreeItem_class *ProjectTree_class::itemFor(const QModelIndex &index) const {
    if(index.isValid()){
        return static_cast<TreeItem_class *>(index.internalPointer());
    }
    return m_ptr_TreeRoot;
}

QModelIndex ProjectTree_class::indexFor(TreeItem_class *item) const {
    if(item == nullptr || item == m_ptr_TreeRoot || item->Parent == nullptr)
        return QModelIndex();
    return createIndex(item->Parent->Children.indexOf(item), 0, item);
}

QModelIndex ProjectTree_class::index(int row, int column, const QModelIndex &parent) const {
    TreeItem_class *parentItem = itemFor(parent);
    if(column != 0 || row < 0 || row >= parentItem->Children.size())
        return QModelIndex();
    return createIndex(row, column, parentItem->Children[row]);
}

QModelIndex ProjectTree_class::parent(const QModelIndex &child) const {
    if(!child.isValid())
        return QModelIndex();
    return indexFor(itemFor(child)->Parent);
}

int ProjectTree_class::rowCount(const QModelIndex &parent) const {
    if(parent.column() > 0)
        return 0;
    return itemFor(parent)->Children.size();
}

int ProjectTree_class::columnCount(const QModelIndex &) const {
    return 1;
}

QModelIndex ProjectTree_class::indexOf(const QString &iUuid) const {
    XmlNode_class *node = m_ptr_Project->getNode(iUuid);
    if(node == nullptr) return QModelIndex();
    if(TreeItem_class *item = m_hash_Items.value(node, nullptr)) return indexFor(item);
    if(TreeItem_class *item = m_hash_Refs.value(node, nullptr))  return indexFor(item);
    return QModelIndex();
}

QHash<int, QByteArray> ProjectTree_class::roleNames() const {
    return {
             { Qt::DisplayRole,  "display"     },
             { RoleTitle,        "title"       },
             { RoleUuid,         "uuid"        },
             { RoleIUuid,        "iUuid"       },
             { RoleIcon,         "icon"        },
             { RoleColor,        "color"       },
             { RoleKind,         "kind"        },
             { RoleIoType,       "ioType"      },
             { RoleDragEnabled,  "dragEnabled" },
             { RoleDescription,  "description" },
             { RoleEditable,     "editable"    },
             };
}

QVariant ProjectTree_class::data(const QModelIndex &index, int role) const {
    QVariant Result;
    const TreeItem_class *itemRef = index.isValid() ? static_cast<TreeItem_class *>(index.internalPointer()) : nullptr;
    if(itemRef == nullptr) return Result;
    XmlNode_class *nodeRef = itemRef->Node;

    const SchemaManager_class::NodeType_s *nodeTemplate = S_SchemaManager.getNode(nodeRef);
    QString iconPath = nodeRef->value(ATTR_ICON);
    if(iconPath.isEmpty() && nodeTemplate)
        iconPath = nodeTemplate->Icon;
    else if(nodeTemplate == nullptr){
        iconPath = QStringLiteral("qrc:/icons/misc/unknown.svg");
    }

    if(itemRef->Kind == TreeItem_class::KindNode){
        switch (role) {
        case Qt::DisplayRole:
        case RoleTitle:         return nodeRef->value(ATTR_TITLE);
        case RoleUuid:          return nodeRef->value(ATTR_UUID );
        case RoleIUuid:         return nodeRef->value(ATTR_IUUID);
        case RoleIcon:          return iconPath;
        case RoleColor:         return nodeRef->value(ATTR_COLOR);
        case RoleKind:          return nodeTemplate ? "node" : "unknown";
        case RoleIoType:        return nodeTemplate ? nodeTemplate->IoType           : QString();
        case RoleDragEnabled:   return nodeTemplate ? nodeTemplate->IoType.isEmpty() : false;
        case RoleDescription:   return nodeTemplate ? nodeTemplate->Description      : QString();
        case RoleEditable:      return nodeTemplate ? nodeTemplate->Editable         : false;
        }
    }else if(itemRef->Kind == TreeItem_class::KindGroup){
        switch(role){
        case RoleEditable:
            return false;
        case Qt::DisplayRole:
        case RoleTitle:
        case RoleUuid:
        case RoleIUuid:
        case RoleIcon:
        case RoleColor:
        case RoleKind:
        case RoleIoType:
        case RoleDragEnabled:
        case RoleDescription:
            return QString();
        }
    }else if(itemRef->Kind == TreeItem_class::KindRef){
        switch(role){
        case RoleEditable:
            return false;
        case Qt::DisplayRole:
        case RoleTitle:
        case RoleUuid:
        case RoleIUuid:
        case RoleIcon:
        case RoleColor:
        case RoleKind:
        case RoleIoType:
        case RoleDragEnabled:
        case RoleDescription:
            return QString();
        }
    }else if(itemRef->Kind == TreeItem_class::KindRoot){
        switch(role){
        case RoleEditable:
            return false;
        case Qt::DisplayRole:
        case RoleTitle:
        case RoleUuid:
        case RoleIUuid:
        case RoleIcon:
        case RoleColor:
        case RoleKind:
        case RoleIoType:
        case RoleDragEnabled:
        case RoleDescription:
            QString();
        }
    }
    return QVariant();
}