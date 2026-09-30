#ifndef PAGE_H
#define PAGE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QUndoStack>
#include <QVariantList>

#include "../../BlockManager/Block/blockbase.h"

class Page_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString      title        READ title        NOTIFY titleChanged)
    Q_PROPERTY(QString      modifiedDate READ modifiedDate NOTIFY modifiedDateChanged)
    Q_PROPERTY(QVariantList blockList    READ blockList    NOTIFY blockListChanged)
    Q_PROPERTY(QVariantList connections  READ connections  NOTIFY connectionsChanged)

    /* View state - kept while the app runs, not saved */
    Q_PROPERTY(qreal viewX     MEMBER m_real_ViewX     NOTIFY viewChanged)
    Q_PROPERTY(qreal viewY     MEMBER m_real_ViewY     NOTIFY viewChanged)
    Q_PROPERTY(qreal viewScale MEMBER m_real_ViewScale NOTIFY viewChanged)
    Q_PROPERTY(bool  viewValid MEMBER m_b_ViewValid    NOTIFY viewChanged)

public: /* Typedef and enums */

public: /* Members */
    QString Title;
    QString ModifiedDate;

private: /* Members */
    QVector<BlockBase_class *> m_vec_Blocks;
    QVariantList               m_list_Connections;

    qreal m_real_ViewX     = 0;
    qreal m_real_ViewY     = 0;
    qreal m_real_ViewScale = 0.2;
    bool  m_b_ViewValid    = false;

public:
    explicit Page_class(QObject *parent = nullptr);
    ~Page_class();

    QString      title()        const { return Title;              }
    QString      modifiedDate() const { return ModifiedDate;       }
    QVariantList blockList()    const;
    QVariantList connections()  const { return m_list_Connections; }

    void addBlockInstance(BlockBase_class *BlockRef);

signals:
    void titleChanged();
    void modifiedDateChanged();
    void blockListChanged();
    void connectionsChanged();
    void viewChanged();
};

#endif // PAGE_H
