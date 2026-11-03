#include "blockmanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QXmlStreamReader>
#include <QDebug>

#include "src/Setup/Setup.hpp"
#include "../Logger/logger.h"
#include "../Document/xmlnode.h"
#include "../Document/document_tokens.h"
#include "../Document/resourceurl.h"

BlockManager_class S_BlockManager;

#define BLOCK_MANAGER_NAME "Block Manager"

static XmlNode_class *readDescriptor(const QString &descriptorPath){
    QFile descriptorFile(descriptorPath);
    if(!descriptorFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        S_Logger.write(Log::Warning, BLOCK_MANAGER_NAME, descriptorPath + ": " + descriptorFile.errorString());
        return nullptr;
    }
    QXmlStreamReader xml(&descriptorFile);
    XmlNode_class *root = XmlNode_class::readDoc(xml);
    if(root == nullptr){
        S_Logger.write(Log::Warning, BLOCK_MANAGER_NAME, descriptorPath + ": " + xml.errorString() + " line " + QString::number(xml.lineNumber()),
                       "Block descriptor", "Fix the XML", true);
    }
    return root;
}


BlockManager_class::BlockManager_class(QObject *parent) : QObject(parent) {

}

BlockManager_class::~BlockManager_class(){

}

void BlockManager_class::begin(){
    const QDir blocksDir(CACHE_APP_PATH + TEMPLATES_DIR_NAME + BLOCKS_DIR_NAME);

    /* Categories first: they decide the order of the menus */
    loadCategories(blocksDir.filePath(DESCRIPTOR_FILE_NAME));

    QDirIterator it(blocksDir.path(), QStringList{ DESCRIPTOR_FILE_NAME }, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString descriptorPath = it.next();
        if(QFileInfo(descriptorPath).absoluteDir() != blocksDir)
            loadBlockType(descriptorPath);
    }

    emit categoriesChanged();
}

void BlockManager_class::loadCategories(const QString &descriptorPath){
    XmlNode_class *root = readDescriptor(descriptorPath);
    if(root == nullptr)
        return;
    for(const XmlNode_class *node : root->children()){
        if(node->TagName != TOKEN_CATEGORY)
            continue;
        Category_s *newCategory = new Category_s;
        newCategory->Title   = node->value(ATTR_TITLE);
        newCategory->Icon    = resolveResourceUrl(node->value(ATTR_ICON), descriptorPath);
        newCategory->Visible = node->value(ATTR_VISIBLE) == "true";
        m_map_Categories[newCategory->Title] = newCategory;
        m_vec_Categories.append(newCategory);
    }
    delete root;
}

void BlockManager_class::loadBlockType(const QString &descriptorPath){
    XmlNode_class *root = readDescriptor(descriptorPath);
    if(root == nullptr)
        return;
    if(root->TagName != TOKEN_BLOCK){
        delete root;
        return;
    }

    BlockType_s *newBlockType = new BlockType_s;
    newBlockType->Uuid     = QUuid(root->value(ATTR_UUID));
    newBlockType->Title    = root->value(ATTR_TITLE);
    newBlockType->Type     = root->value(ATTR_TYPE);
    newBlockType->Category = root->value(ATTR_CATEGORY);
    newBlockType->Icon     = resolveResourceUrl(root->value(ATTR_ICON), descriptorPath);
    newBlockType->Color    = QColor(root->value(ATTR_COLOR, QStringLiteral("#7D8491")));
    newBlockType->Width    = root->value(ATTR_WIDTH, QStringLiteral("400")).toDouble();

    auto portDefinition = [this](const XmlNode_class *portNode) -> PortDefinition_s {
        PortDefinition_s newPort;
        newPort.Title       = portNode->value(ATTR_TITLE);
        newPort.ShortName   = portNode->value(ATTR_SHORT_NAME);
        newPort.DataType    = portNode->value(ATTR_DATA_TYPE);
        newPort.Default     = portNode->value(ATTR_DEFAULT);
        newPort.Unit        = portNode->value(ATTR_UNIT);
        newPort.Visible     = portNode->value(ATTR_VISIBLE) == "true";
        for(auto &child : portNode->children()){
            if(child->TagName == TAG_DOCS){
                newPort.Docs = child->value(ATTR_TEXT);
            }else if(child->TagName == TAG_VARIANT){
                PortVariant_s newVariant;
                newVariant.Data = child->value(ATTR_DATA);
                newVariant.Text = child->value(ATTR_TEXT);
                newPort.Variants.append(newVariant);
            }
        }
        return newPort;
    };

    for(const XmlNode_class *child : root->children()){
        if(child->TagName == TAG_DESCRIPTION)
            newBlockType->Docs = child->value(ATTR_TEXT);
        else if(child->TagName == TOKEN_INPUT)
            newBlockType->Inputs.append(portDefinition(child));
        else if(child->TagName == TOKEN_OUTPUT)
            newBlockType->Outputs.append(portDefinition(child));
        else if(child->TagName == TOKEN_PROPERTY)
            newBlockType->Properties.append(portDefinition(child));
    }
    delete root;

    if(newBlockType->Uuid.isNull() || m_hash_BlockTypeByUuid.contains(newBlockType->Uuid)){
        S_Logger.write(Log::Warning, BLOCK_MANAGER_NAME, descriptorPath + ": missing or duplicate Uuid, block ignored",
                       "Block descriptor", "Every block needs a unique Uuid", true);
        delete newBlockType;
        return;
    }
    m_vec_BlockTypes.append(newBlockType);
    m_hash_BlockTypeByUuid[newBlockType->Uuid] = newBlockType;
    auto findCategory = m_map_Categories.find(newBlockType->Category);
    if(findCategory != m_map_Categories.end()){
        findCategory.value()->BlockTypes.append(newBlockType);
    }else{
        Category_s *newCategory = new Category_s;
        newCategory->Title = newBlockType->Category;
        newCategory->BlockTypes.append(newBlockType);
        m_map_Categories[newCategory->Title] = newCategory;
        m_vec_Categories.append(newCategory);
    }
}

BlockManager_class::BlockType_s *BlockManager_class::getBlockTypeByUuid(QUuid Uuid){
    BlockType_s * result = m_hash_BlockTypeByUuid.value(Uuid, nullptr);
    return result;
}

/* ================= QML API ================= */
QVariantList BlockManager_class::categories() const{
    QVariantList Result;
    for(auto &category : m_vec_Categories){
        QVariantList blocksList;
        for(auto &blockType : category->BlockTypes){
            QVariantMap blockMap;
            blockMap["uuid"]        = blockType->Uuid.toString(QUuid::WithoutBraces);
            blockMap["title"]       = blockType->Title;
            blockMap["icon"]        = blockType->Icon;
            blockMap["docs"]        = blockType->Docs;
            blocksList.append(blockMap);
        }
        QVariantMap categoryMap;
        categoryMap["title"]    = category->Title;
        categoryMap["icon"]    = category->Icon;
        categoryMap["visible"] = category->Visible;
        categoryMap["count"]   = blocksList.size();
        categoryMap["blocks"]  = blocksList;
        Result.append(categoryMap);
    }
    return Result;
}