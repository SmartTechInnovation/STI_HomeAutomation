#include "page.h"

#include <QDateTime>
#include <QHash>

Page_class::Page_class(QObject *parent) : QObject(parent) {
     ModifiedDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

Page_class::~Page_class(){
    qDeleteAll(m_vec_Blocks);
    m_vec_Blocks.clear();
}

QVariantList Page_class::blockList()    const{
    QVariantList blockList;
    for(int i = 0; i < m_vec_Blocks.size(); i++){
        QVariantMap blockMap = m_vec_Blocks[i]->getQVariantMap();
        blockMap["index"] = i;
        blockList.push_back(blockMap);
    }
    return blockList;
}

void Page_class::addBlockInstance(BlockBase_class *BlockRef){
    m_vec_Blocks.push_back(BlockRef);
    emit blockListChanged();
}