#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QString>
#include <QVariantList>

#include "Block/blockbase.h"

class BlockManager_class : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList categories         READ categories         NOTIFY categoriesChanged)
    Q_PROPERTY(QVariantList shortcutCategories READ shortcutCategories NOTIFY categoriesChanged)

public:  /* Typedefs and enums */
private: /* Typedefs and enums */
    typedef QVector<BlockBase_class *> BlockList;

    struct Category_s{
        QString   name;
        QString   iconPath;
        bool      visible = false;
        int       order   = 0;
        BlockList blockList;
    };

public:  /* Variables */
private: /* Variables */
    QMap<QString, Category_s> m_map_LegacyCategories; // Categories

    BlockBase_class          *m_new_BlockBase  = nullptr;
    int                       m_int_NextOrder  = 0;

private: /* Functions */
    QVariantList        _categoriesToVariant(bool onlyVisible) const;
    static QVariantList _blocksToVariant    (const BlockList &blockList);

public:  /* Functions */
    BlockManager_class(QObject *parent = nullptr);

    void begin();
    bool loadLegacyBook      (const QString &folderPath);
    bool loadLegacyCategories(const QString &descriptorPath);
    bool loadBlockDescriptor (const QString &descriptorPath);

    /* ==== QML API ==== */
    QVariantList categories()         const;
    QVariantList shortcutCategories() const;

    Q_INVOKABLE QVariantList blocksOfCategory(const QString &categoryName) const;

signals:
    void categoriesChanged();
};

extern BlockManager_class S_BlockManager;

#endif // BLOCKMANAGER_H
