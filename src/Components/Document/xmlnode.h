#ifndef XMLNODE_H
#define XMLNODE_H

#include <QString>
#include <QVector>
#include <QHash>
#include <QUuid>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <functional>

class XmlNode_class {
public:  /* Typedef and enums */
private: /* Typedef and enums */
    struct Attribute_s {
        QString Name;
        QString Value;
    };

public: /* Members    */
    QString TagName;
private: /* Members   */
    XmlNode_class                 *m_ptr_Parent = nullptr;
    QHash<QString, Attribute_s *>  m_hash_Attributes;
    QVector<Attribute_s   *>       m_vec_Attributes;
    QVector<XmlNode_class *>       m_vec_Children;
public: /* Functional */
    explicit XmlNode_class(const QString &tag = QString());
    ~XmlNode_class();

    /* ===== Xml ===== */
    static XmlNode_class *read   (QXmlStreamReader &xml);
    static XmlNode_class *readDoc(QXmlStreamReader &xml);
    void                  write  (QXmlStreamWriter &xml);

    /* ==== Tree ==== */
    XmlNode_class                  *parent()   const;
    const QVector<XmlNode_class *> &children() const;
    int                             row()      const;
    void                            visit(const std::function<void(XmlNode_class *)> &callback);


    void           insertChild(XmlNode_class *node, int row);
    void           appendChild(XmlNode_class *node);
    XmlNode_class *takeChild  (XmlNode_class *node);
    XmlNode_class *ancestor   (const QString &parentTag);

    /* Attributes */
    bool    hasAttr   (const QString &name) const;
    void    removeAttr(const QString &name);
    QString value     (const QString &name, const QString &fallback = QString()) const;
    void    setValue  (const QString &name, const QString &value);
};

#endif // XMLNODE_H
