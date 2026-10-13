#include "page.h"

#include <QDateTime>
#include <QHash>

#include "../../Document/document_tokens.h"

Page_class::Page_class(Project_class *project, XmlNode_class *pageNode) {
    m_ptr_Project = project;
    m_ptr_NodeRef = pageNode;
}

Page_class::~Page_class(){

}

XmlNode_class *Page_class::nodeRef() const{
    return m_ptr_NodeRef;
}

QString Page_class::iUuid() const{
    return m_ptr_NodeRef->value(ATTR_IUUID);
}

QString Page_class::title() const{
    return m_ptr_NodeRef->value(ATTR_TITLE);
}

QString Page_class::modifiedDate() const{
    return m_ptr_NodeRef->value(ATTR_MODIFIED_DATE);
}

QVariantList Page_class::connections() const{
    QVariantList Result;


    return Result;
}