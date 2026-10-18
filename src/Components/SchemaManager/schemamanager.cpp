#include "schemamanager.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

#include "../Logger/logger.h"
#include "../Document/document_tokens.h"
#include "../../App/application.h"
#include "../../Setup/Setup.hpp"

SchemaManager_class S_SchemaManager;

#define SCHEMA_NAME "Schema Manager"

SchemaManager_class::SchemaManager_class(QObject *parent) : QObject(parent) {

}

SchemaManager_class::~SchemaManager_class(){
    qDeleteAll(m_vec_NodeTypes);
}

void SchemaManager_class::begin(){
    const QString schemaName    = "Project_" + Application.toString(Application.Language);
    const QString schemaPath    = CACHE_APP_PATH + TEMPLATES_DIR_NAME + SCHEMA_DIR_NAME + "/" + schemaName + ".xml";
    const QString peripheryPath = CACHE_APP_PATH + TEMPLATES_DIR_NAME + PERIPHERY_DIR_NAME;
    loadFile(schemaPath);
    if(QDir(peripheryPath).exists()){
        QDirIterator it(peripheryPath, QStringList{ "*.xml" }, QDir::Files, QDirIterator::Subdirectories);
        while(it.hasNext()){
            loadFile(it.next());
        }
    }else{
        S_Logger.write(Log::Warning, SCHEMA_NAME, QStringLiteral("Periphery library not found: ") + peripheryPath,
                       "Periphery library", "Copy the Periphery folder next to blocks", true);
    }
}

const SchemaManager_class::NodeType_s *SchemaManager_class::getNode(const QUuid   &uuid) const{
    return m_hash_NodeTypeByUuid.value(uuid, nullptr);
}

const SchemaManager_class::NodeType_s *SchemaManager_class::getNode(const QString &tagName) const{
    return m_hash_NodeTypeByTag.value(tagName, nullptr);
}

const SchemaManager_class::NodeType_s *SchemaManager_class::getNode(const XmlNode_class *node) const{
    const NodeType_s *findedNode = getNode(node->TagName);
    if(findedNode != nullptr)
        return findedNode;
    else
        return getNode(node->value(ATTR_UUID));
}

QVector<const SchemaManager_class::NodeType_s *> SchemaManager_class::sections() const{
    QVector<const NodeType_s *> Result;
    for(const NodeType_s *nodeType : m_vec_NodeTypes){
        if(nodeType->Section)
            Result.append(nodeType);
    }
    return Result;
}

void SchemaManager_class::loadFile(const QString &filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        S_Logger.write(Log::Warning, SCHEMA_NAME, filePath + ": " + file.errorString());
    }
    QXmlStreamReader xml(&file);
    XmlNode_class *root = XmlNode_class::readDoc(xml);
    if(root == nullptr){
        S_Logger.write(Log::Warning, SCHEMA_NAME, filePath + ": " + xml.errorString() + " line " + QString::number(xml.lineNumber()),
                       "Schema descriptor", "Fix the XML", true);
        return;
    }
    parse(root, filePath);
    delete root;
}

bool SchemaManager_class::parse(const XmlNode_class *node, const QString &source){
    bool Result = true;
    if(node->TagName != TAG_NODE_TYPE){
        for(auto &child : node->children()){
            parse(child, source);
        }
        return true;
    }

    NodeType_s *newNode = new NodeType_s;
    newNode->Uuid       = QUuid(node->value(ATTR_UUID));
    newNode->TagName    = node->value(ATTR_TAG_NAME);
    newNode->Title      = node->value(ATTR_TITLE,      newNode->TagName);
    newNode->Icon       = node->value(ATTR_ICON);
    newNode->MenuName   = node->value(ATTR_MENU);
    newNode->IoType     = node->value(ATTR_IO_TYPE);
    newNode->Max        = node->value(ATTR_MAX,        "-1").toInt();
    newNode->Section    = node->value(ATTR_SECTION) == "true";
    newNode->Hidden     = node->value(ATTR_HIDDEN)  == "true";
    newNode->Editable   = !(node->value(ATTR_EDITABLE)== "false");

    for(auto &child : node->children()){
        if(child->TagName == TAG_DESCRIPTION){
            newNode->Description    = child->value(ATTR_TEXT);
        }else if(child->TagName == TAG_PROPERTY){
            Property_s  newProperty;
            newProperty.TagName     = child->value(ATTR_TAG_NAME);
            newProperty.Title       = child->value(ATTR_TITLE);
            newProperty.Type        = child->value(ATTR_TYPE);
            newProperty.Default     = child->value(ATTR_DEFAULT);
            newProperty.Unit        = child->value(ATTR_UNIT);
            newProperty.ReadOnly    = child->value(ATTR_READ_ONLY) == "true";
            newProperty.Hidden      = child->value(ATTR_HIDDEN)    == "true";
            for(auto &variant : child->children()){
                if(variant->TagName == TAG_DOCS){
                    newProperty.Docs= variant->value(ATTR_TEXT);
                }else if(variant->TagName == TAG_VARIANT){
                    Variant_s newVariant;
                    newVariant.Data = variant->value(ATTR_DATA);
                    newVariant.Text = variant->value(ATTR_TEXT);
                    newVariant.Icon = variant->value(ATTR_ICON);
                    newProperty.Variants.append(newVariant);
                }
            }
            if(!newProperty.TagName.isEmpty())
                newNode->Properties.append(newProperty);
        }else if(child->TagName == TAG_CHILD){
            Child_s newChildDef;
            newChildDef.Uuid        = QUuid(child->value(ATTR_UUID));
            newChildDef.Title       = child->value(ATTR_TITLE);
            newChildDef.Visible     = child->value(ATTR_VISIBLE)   == "true";
            newChildDef.Mandatory   = child->value(ATTR_MANDATORY) == "true";
            newNode->Children.append(newChildDef);
        }
    }
    if(newNode->TagName.isEmpty()){
        S_Logger.write(Log::Warning, SCHEMA_NAME, source + ": NodeType without TagName (" + newNode->Title + ")");
        Result = false;
    }
    if(m_hash_NodeTypeByTag.contains(newNode->TagName)){
        S_Logger.write(Log::Warning, SCHEMA_NAME, source + ": duplicate TagName " + newNode->TagName + ", ignored",
                       "Schema descriptor", "Every NodeType needs a unique TagName", true);
        Result = false;
    }
    if(!newNode->Uuid.isNull() && m_hash_NodeTypeByUuid.contains(newNode->Uuid)){
        S_Logger.write(Log::Warning, SCHEMA_NAME, source + ": " + newNode->TagName + " has the same Uuid as " + m_hash_NodeTypeByUuid[newNode->Uuid]->TagName,
                       "Schema descriptor", "Every NodeType needs a unique Uuid", true);
        Result = false;
    }
    if(newNode->Uuid.isNull()){
        Result = false;
    }
    if(!Result){
        delete newNode;
        return Result;
    }
    m_hash_NodeTypeByUuid[newNode->Uuid]    = newNode;
    m_hash_NodeTypeByTag [newNode->TagName] = newNode;
    m_vec_NodeTypes.append(newNode);
    return Result;
}