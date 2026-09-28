#ifndef PAGE_H
#define PAGE_H

#include <QString>
#include <QVector>
#include <QUndoStack>

#include "../../BlockManager/Block/blockbase.h"

class Page_class
{
public: /* Typedef and enums */

public: /* Members */
    QString Title;

private: /* Members */
    QVector<BlockBase_class *> m_vec_Blocks;
    QUndoStack                *m_UndoStackRef   = nullptr;

public:
    explicit Page_class();

    void addBlockInstance(BlockBase_class *BlockRef);
};

#endif // PAGE_H
