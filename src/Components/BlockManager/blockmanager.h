#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include <QObject>
#include <QColor>
#include <QHash>
#include <QVector>
#include <QString>
#include <QUuid>
#include <QVariantList>

class BlockManager_class : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList categories         READ categories         NOTIFY categoriesChanged)

public:  /* Typedefs and enums */
    struct PortVariant_s {
        QString Data;
        QString Text;
    };

    struct PortDefinition_s {
        QString ShortName;
        QString Title;
        QString DataType;
        QString Default;
        QString Unit;
        QString Docs;
        bool    Visible = false;
        QVector<PortVariant_s> Variants;
    };

    struct BlockType_s {
        QUuid   Uuid;   /* Unique identifier */
        QString Title;  /* Default Title for Block, can be modified */
        QString Type;   /* Block of Type, cannot be modified */
        QString Docs;
        QString Category;
        QString Icon;
        QColor  Color;
        qreal   Width = 400;
        QVector<PortDefinition_s> Inputs;
        QVector<PortDefinition_s> Outputs;
        QVector<PortDefinition_s> Properties;
    };

private: /* Typedefs and enums */
    struct Category_s{
        QString Title;
        QString Icon;
        bool    Visible = false;
        QVector<const BlockType_s *> BlockTypes;
    };

private: /* Members */
    QVector<BlockType_s *>      m_vec_BlockTypes;
    QHash<QUuid, BlockType_s *> m_hash_BlockTypeByUuid;
    QVector<Category_s *>       m_vec_Categories;
    QMap<QString, Category_s *> m_map_Categories; // Key: Name, Data: Categories

private: /* Functions */
    void loadCategories(const QString &descriptorPath);
    void loadBlockType (const QString &descriptorPath);


public:  /* Functions */
    BlockManager_class(QObject *parent = nullptr);
    ~BlockManager_class();

    void begin();

    BlockType_s *getBlockTypeByUuid(QUuid Uuid);

    /* ==== QML API ==== */
    QVariantList categories()         const;
signals:
    void categoriesChanged();
};

extern BlockManager_class S_BlockManager;

#endif // BLOCKMANAGER_H
