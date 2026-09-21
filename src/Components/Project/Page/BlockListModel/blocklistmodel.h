#ifndef BLOCKLISTMODEL_H
#define BLOCKLISTMODEL_H

#include <QObject>
#include <QVector>
#include <QAbstractListModel>

class Page_class;
class BlockBase_class;

class BlockListModel_class : public QAbstractListModel
{
    Q_OBJECT
public: /* Typedefs and Enums */
    enum Roles {
        BlockIdRole = Qt::UserRole + 1,
        BlockTypeRole,
        BlockTitleRole,
        BlockXRole,
        BlockYRole,
        BlockWidthRole,
        BlockInputsRole,
        BlockOutputsRole,
        BlockSelectedRole,
    };
private: /* Members */
    Page_class                *m_PageParentRef = nullptr;
    QVector<BlockBase_class *> m_vec_Blocks;

public:
    explicit BlockListModel_class(Page_class *pageRef);

    int      rowCount(const QModelIndex &parent = {})     const override;
    QVariant data    (const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames()                    const override;
};

#endif // BLOCKLISTMODEL_H
