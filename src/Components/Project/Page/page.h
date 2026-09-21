#ifndef PAGE_H
#define PAGE_H

#include <QObject>
#include <QUndoStack>

#include "BlockListModel/blocklistmodel.h"

class Page_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *blocks     READ getBlocks      CONSTANT)
    //Q_PROPERTY(QObject *conections READ connectionsModel CONSTANT)

private: /* Members */
    QUndoStack                *m_UndoStackRef   = nullptr;
    BlockListModel_class      *m_BlocksList     = nullptr; /* Container for Blocks */
public:
    explicit Page_class(QObject *parent = nullptr);

    QObject *getBlocks()    const       { return m_BlocksList; };

};

#endif // PAGE_H
