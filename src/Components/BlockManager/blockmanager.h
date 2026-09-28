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
        QString   title;
        QString   iconPath;
        bool      visible = false;
        BlockList blockList;
        int       order;
    };

public:  /* Members */
private: /* Members */
    QMap<QString, Category_s> m_map_LegacyCategories; // Key: Name, Data: Categories

private: /* Functions */

public:  /* Functions */
    BlockManager_class(QObject *parent = nullptr);

    /* ==== QML API ==== */
    QVariantList categories()         const;
    QVariantList shortcutCategories() const;

    void begin();
    bool loadLegacyBook      (const QString &folderPath);
    bool loadLegacyCategories(const QString &descriptorPath);
    bool loadBlockDescriptor (const QString &descriptorPath);

    BlockBase_class *getInstance(QUuid &Uuid);

signals:
    void categoriesChanged();
};

extern BlockManager_class S_BlockManager;

#endif // BLOCKMANAGER_H
