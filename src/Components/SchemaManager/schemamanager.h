#ifndef SCHEMAMANAGER_H
#define SCHEMAMANAGER_H

#include <QObject>
#include <QHash>
#include <QVector>
#include <QUuid>
#include <QVariantMap>

#include "../Document/xmlnode.h"

class SchemaManager_class : public QObject
{
    Q_OBJECT
public: /* Typedef and enums */
    struct Variant_s {
        QString Data;
        QString Text;
        QString Icon;
    };

    struct Property_s {
        QString TagName;
        QString Title;
        QString Type;
        QString Default;
        QString Unit;
        QString Docs;
        QString Group;
        bool    ReadOnly = false;
        bool    Hidden   = false;
        QVector<Variant_s> Variants;
    };

    struct Child_s {
        QUuid   Uuid;
        QString Title;
        bool    Mandatory = false;
        bool    Visible   = false;
    };

    struct NodeType_s {
        QUuid   Uuid;
        QString TagName;
        QString Title;
        QString Icon;
        QString MenuName;
        QString Description;
        QString IoType;
        QString DataType;
        int     Max     = -1; // Max number of children (e.g. one protocol per port)
        bool    Section = false;
        bool    Hidden  = false;
        bool    Editable= true;
        QVector<Property_s> Properties;
        QVector<Child_s   > Children;
    };

private: /* Members   */
    QVector<NodeType_s *>       m_vec_NodeTypes;
    QHash<QUuid,   NodeType_s*> m_hash_NodeTypeByUuid;
    QHash<QString, NodeType_s*> m_hash_NodeTypeByTag;
public:  /* Members   */

private: /* Functions */
    void loadFile(const QString &filePath);
    bool parse   (const XmlNode_class *node, const QString &source);

public:  /* Functions */
    explicit SchemaManager_class(QObject *parent = nullptr);
    ~SchemaManager_class();

    void begin();

    const NodeType_s *getNode(const QUuid   &uuid)       const;
    const NodeType_s *getNode(const QString &tagName)    const;
    const NodeType_s *getNode(const XmlNode_class *node) const;

    /* NodeTypes with Section="true" - created automatically under RootDevice */
    QVector<const NodeType_s *> sections() const;
};

extern SchemaManager_class S_SchemaManager;

#endif // SCHEMAMANAGER_H
