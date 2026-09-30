#include "page.h"

#include <QDateTime>
#include <QHash>

#include "../../BlockManager/blockmanager.h"

Page_class::Page_class(QObject *parent) : QAbstractListModel(parent) {
     ModifiedDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

void Page_class::addBlockInstance(BlockBase_class *BlockRef){
    m_vec_Blocks.push_back(BlockRef);
}

Page_class::~Page_class(){
    qDeleteAll(m_vec_Blocks);
    m_vec_Blocks.clear();
}

/* ================= Model ================= */

int Page_class::rowCount(const QModelIndex &parent) const{
    if(parent.isValid()) return 0;
    return m_vec_Blocks.size();
}

QVariant Page_class::data(const QModelIndex &index, int role) const{
    if(!index.isValid() || index.row() < 0 || index.row() >= m_vec_Blocks.size())
        return QVariant();

    //const BlockBase_class *block = m_vec_Blocks[index.row()];
    //switch(role){
    //case UuidRole:   return block->Uuid.toString(QUuid::WithoutBraces);
    //case TitleRole:  return displayTitle(block);
    //case KindRole:   return block->Kind;
    //case VarRole:    return block->VarRef.toString(QUuid::WithoutBraces);
    //case TypeRole:   return block->Type;
    //case ColorRole:  return block->Color.name();
    //case IconRole:   return block->IconPath;
    //case PxRole:     return block->Position.x();
    //case PyRole:     return block->Position.y();
    //case WidthRole:  return block->Width;
    //case InputsRole: return BlockManager_class::inputsToVariant(block);
    //case OutputsRole:{
    //    QVariantList outputs = BlockManager_class::portsToVariant(block->m_vec_Outputs, true);
    //    for(int i = 0; i < outputs.size(); i++){
    //        QVariantMap portMap = outputs[i].toMap();
    //        portMap["connected"] = m_set_UsedOutputs.contains(block->m_vec_Outputs[i]->getUuid());
    //        outputs[i] = portMap;
    //    }
    //    return outputs;
    //}
    //default: break;
    //}
    return QVariant();
}

QHash<int, QByteArray> Page_class::roleNames() const{
    return {
             { UuidRole,    "uuid"    },
             { KindRole,    "kind"    },
             { VarRole,     "varRef"  },
             { TitleRole,   "title"   },
             { TypeRole,    "type"    },
             { ColorRole,   "color"   },
             { IconRole,    "icon"    },
             { PxRole,      "px"      },
             { PyRole,      "py"      },
             { WidthRole,   "bWidth"  },
             { InputsRole,  "inputs"  },
             { OutputsRole, "outputs" },
             };
}
