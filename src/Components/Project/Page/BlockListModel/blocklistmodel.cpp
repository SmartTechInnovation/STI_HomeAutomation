#include "blocklistmodel.h"

#include "src/Components/BlockManager/Block/blockbase.h"

BlockListModel_class::BlockListModel_class(Page_class *pageRef) {
    m_PageParentRef = pageRef;
}


int BlockListModel_class::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_vec_Blocks.size();
}

/* ===== Helper Function ===== */
static QVariantList portList(const QVector<Port_class *> &ports)
{
    QVariantList out;
    for (Port_class *p : ports) {
        if(p != nullptr){
            QVariantMap m;
            m["name"]  =     p->getName();
            m["color"] =     p->getColor();
            m["connected"] = p->getConnected();
            out.append(m);
        }
    }
    return out;
}

QVariant BlockListModel_class::data(const QModelIndex &index, int role) const{
    if(index.row() < 0 || index.row() >= m_vec_Blocks.size()) return {};

    BlockBase_class *BlockRef = m_vec_Blocks.at(index.row());
    switch(role){
    case BlockIdRole:       return BlockRef->getId().toString(QUuid::WithoutBraces);
    case BlockTypeRole:     return BlockRef->getType();
    case BlockTitleRole:    return BlockRef->getName();
    case BlockXRole:        return BlockRef->getX();
    case BlockYRole:        return BlockRef->getY();
    case BlockWidthRole:    return BlockRef->getWidth();
    case BlockInputsRole:   return portList(BlockRef->getInputs());
    case BlockOutputsRole:  return portList(BlockRef->getOutputs());
    case BlockSelectedRole: return BlockRef->getSelected();
    default:                return {};
    }
    return {};
}

QHash<int, QByteArray> BlockListModel_class::roleNames() const{
    return {
        { BlockIdRole,       "blockId" },
        { BlockTypeRole,     "Type"    },
        { BlockTitleRole,    "Title"   },
        { BlockXRole,        "x"       },
        { BlockYRole,        "y"       },
        { BlockWidthRole,    "w"       },
        { BlockInputsRole,   "inputs"  },
        { BlockOutputsRole,  "outputs" },
        { BlockSelectedRole, "selected"},
    };
}