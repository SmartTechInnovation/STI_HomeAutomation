#include "xmlnode.h"

XmlNode_class::XmlNode_class(const QString &tag){
    TagName = tag;
}

XmlNode_class::~XmlNode_class(){
    qDeleteAll(m_vec_Children);
}

XmlNode_class *XmlNode_class::read (QXmlStreamReader &xml){
    XmlNode_class *newNode = new XmlNode_class(xml.name().toString());
    for(const QXmlStreamAttribute &attribute : xml.attributes()){
        Attribute_s *newAttribute = new Attribute_s;
        newAttribute->Name  = attribute.name().toString();
        newAttribute->Value = attribute.value().toString();
        newNode->m_vec_Attributes.append(newAttribute);
        newNode->m_hash_Attributes[newAttribute->Name] = newAttribute;
    }
    while(!xml.atEnd() && !xml.hasError()){
        xml.readNext();
        if(xml.isStartElement()){
            XmlNode_class *child = read(xml);
            child->m_ptr_Parent = newNode;
            newNode->m_vec_Children.append(child);
        }else if(xml.isEndElement())
            break;
    }
    return newNode;
}

XmlNode_class *XmlNode_class::readDoc(QXmlStreamReader &xml){
    while(!xml.atEnd() && !xml.isStartElement())
        xml.readNext();
    if(!xml.isStartElement())
        return nullptr;
    XmlNode_class *root = read(xml);
    if(xml.hasError()){
        delete root;
        return nullptr;
    }
    return root;
}

void XmlNode_class::write(QXmlStreamWriter &xml){
    xml.writeStartElement(TagName);
    for(auto &attribute : m_vec_Attributes){
        xml.writeAttribute(attribute->Name, attribute->Value);
    }
    for(auto &node : m_vec_Children){
        node->write(xml);
    }
    xml.writeEndElement();
}

XmlNode_class *XmlNode_class::parent() const{
    return m_ptr_Parent;
}

const QVector<XmlNode_class *> &XmlNode_class::children() const {
    return m_vec_Children;
}

int XmlNode_class::row() const{
    int index = 0;
    if(m_ptr_Parent){
        for(auto &child : m_ptr_Parent->children()){
            if(child == this)
                return index;
            index++;
        }
    }
    return index;
}

void XmlNode_class::visit(const std::function<void(XmlNode_class *)> &callback){
    callback(this);
    for(auto &node : m_vec_Children)
        node->visit(callback);
}

void XmlNode_class::insertChild(XmlNode_class *node, int row){
    if(row < 0 || row > m_vec_Children.size())
        row = m_vec_Children.size();
    node->m_ptr_Parent = this;
    m_vec_Children.insert(row, node);
}

void XmlNode_class::appendChild(XmlNode_class *node){
    node->m_ptr_Parent = this;
    m_vec_Children.append(node);
}

XmlNode_class *XmlNode_class::takeChild(XmlNode_class *node){
    int index = 0;
    XmlNode_class *findedNode = nullptr;
    for(auto &child : m_vec_Children){
        if(child == node){
            findedNode = m_vec_Children.takeAt(index);
            break;
        }
        index++;
    }
    return findedNode;
}

XmlNode_class *XmlNode_class::ancestor(const QString &parentTag){
    if(m_ptr_Parent == nullptr)
        return nullptr;
    if(m_ptr_Parent->TagName == parentTag)
        return m_ptr_Parent;
    return m_ptr_Parent->ancestor(parentTag);
}

/* Attributes */
bool XmlNode_class::hasAttr(const QString &name) const {
    if(m_hash_Attributes.contains(name)){
        return true;
    }
    return false;
}

void XmlNode_class::removeAttr(const QString &name){
    if(m_hash_Attributes.contains(name)){
        m_hash_Attributes.remove(name);
    }
}

QString XmlNode_class::value(const QString &name, const QString &fallback) const{
    if(m_hash_Attributes.contains(name)){
        Attribute_s *findedAttribute = m_hash_Attributes.value(name);
        return findedAttribute->Value;
    }
    return fallback;
}

void XmlNode_class::setValue(const QString &name, const QString &value){
    if(m_hash_Attributes.contains(name)){
        m_hash_Attributes[name]->Value = value;
    }else{
        Attribute_s *newAttribute = new Attribute_s;
        newAttribute->Name  = name;
        newAttribute->Value = value;
        m_vec_Attributes.append(newAttribute);
        m_hash_Attributes[name] = newAttribute;
    }
}