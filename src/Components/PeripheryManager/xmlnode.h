#ifndef XMLNODE_H
#define XMLNODE_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/*
 * Generic XML element: tag + ordered attributes + children.
 * Used for the RootDevice periphery (sections, interfaces, protocols,
 * devices, channels) and for periphery templates / device models.
 */

class XmlNode_class {
private: /* Typedefs and enums */
    struct Attribute_s {
        QString Name;
        QString Value;
    };
private: /* Members   */
    QMap<QString, Attribute_s *> m_map_Attributes;
    QVector<Attribute_s *>       m_vec_Attributes;
public:  /* Members   */
    QString Tag;
    QVector<XmlNode_class *> Children;
public:  /* Functions */
    explicit XmlNode_class(){

    }
    ~XmlNode_class(){
        qDeleteAll(m_vec_Attributes);
        qDeleteAll(Children);
        m_map_Attributes.clear();
        m_vec_Attributes.clear();
    }

    QString value(const QString &name) const {
        auto finded = m_map_Attributes.find(name);
        if(finded != m_map_Attributes.end()){
            return finded.value()->Value;
        }
        return QString();
    }

    bool hasAttr(const QString &name) const {
        auto finded = m_map_Attributes.find(name);
        if(finded != m_map_Attributes.end()){
            return true;
        }
        return false;
    }
    void setValue(const QString &name, const QString &value){
        auto finded = m_map_Attributes.find(name);
        if(finded != m_map_Attributes.end()){
            finded.value()->Value = value;
        }else{
            Attribute_s *newAttribute = new Attribute_s;
            newAttribute->Name  = name;
            newAttribute->Value = value;
            m_vec_Attributes.append(newAttribute);
            m_map_Attributes[name] = newAttribute;
        }
    }

    static XmlNode_class *read(QXmlStreamReader &xml){
        XmlNode_class *node = new XmlNode_class();
        node->Tag    = xml.name().toString();
        for(const QXmlStreamAttribute &attribute : xml.attributes()){
            Attribute_s *newAttribute = new Attribute_s;
            newAttribute->Name  = attribute.name().toString();
            newAttribute->Value = attribute.value().toString();
            node->m_vec_Attributes.append(newAttribute);
            node->m_map_Attributes[newAttribute->Name] = newAttribute;
        }
        while(!xml.atEnd() && !xml.hasError()){
            xml.readNext();
            if(xml.isStartElement())
                node->Children.append(read(xml));
            else if(xml.isEndElement()) break;
        }
        return node;
    }

    void write(QXmlStreamWriter &xml) const {
        xml.writeStartElement(Tag);
        for(const auto &attribute : m_vec_Attributes){
            xml.writeAttribute(attribute->Name, attribute->Value);
        }
        for(const auto &child: Children){
            child->write(xml);
        }
        xml.writeEndElement();
    }
};

#endif // XMLNODE_H
