#include "blockmanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QXmlStreamReader>

#include <algorithm>

#include <QDebug>

#include "src/Setup/Setup.hpp"

BlockManager_class S_BlockManager;

BlockManager_class::BlockManager_class(QObject *parent) : QObject(parent) {

}

void BlockManager_class::begin(){
    loadLegacyBook(LEGACY_BLOCKS_PATH);
    emit categoriesChanged();
}

bool BlockManager_class::loadLegacyBook(const QString &folderPath){
    bool Result = true;

    QDirIterator it(folderPath,
                    QStringList{ "Descriptor.xml" },
                    QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString path = it.next();

        QFileInfo descInfo(path);
        QString   dirName = descInfo.dir().dirName();

        if(dirName == "Legacy"){
            if (!loadLegacyCategories(path))
                Result = false;
        }else{
            if(!loadBlockDescriptor(path)){
                Result = false;
            }
        }
    }
    return Result;
}

bool BlockManager_class::loadLegacyCategories(const QString &descriptorPath){
    bool Result = true;

    QFile descriptorFile(descriptorPath);
    QFileInfo descriptorFileInfo(descriptorPath);
    if(!descriptorFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qCritical() << descriptorFile.errorString();
        Result = false;
        return Result;
    }
    QXmlStreamReader descriptorXml(&descriptorFile);
    while(!descriptorXml.atEnd() && !descriptorXml.hasError()){
        QXmlStreamReader::TokenType token = descriptorXml.readNext();
        QStringView                 tokenName = descriptorXml.name();

        if(token == QXmlStreamReader::StartElement){
            if(tokenName == "Category"){
                Category_s newCategory;
                newCategory.name        = descriptorXml.attributes().value("Name").toString();
                newCategory.iconPath    = descriptorXml.attributes().value("Icon").toString();
                newCategory.visible     = descriptorXml.attributes().value("Visible").toString() == "true";
                newCategory.order       = m_int_NextOrder++;

                if(!newCategory.iconPath.isEmpty() && newCategory.iconPath.contains("qrc:") == false){
                    newCategory.iconPath = "file:///" + descriptorFileInfo.dir().absolutePath() + "/" + newCategory.iconPath;
                }

                if(m_map_LegacyCategories.find(newCategory.name) == m_map_LegacyCategories.end()){
                    m_map_LegacyCategories[newCategory.name] = newCategory;
                }else{
                    Category_s *existingCategory = &m_map_LegacyCategories[newCategory.name];
                    existingCategory->name     = newCategory.name;
                    existingCategory->iconPath = newCategory.iconPath;
                    existingCategory->visible  = newCategory.visible;
                    existingCategory->order    = newCategory.order;
                }
            }
        }else if(token == QXmlStreamReader::EndElement){

        }
    }
    if(descriptorXml.hasError()){
        qWarning() << "Load Block Categories: " + descriptorXml.errorString();
    }
    return Result;
}

bool BlockManager_class::loadBlockDescriptor (const QString &descriptorPath){
    bool Result = true;

    QFile     descriptorFile(descriptorPath);
    QFileInfo descriptorFileInfo(descriptorPath);
    if(!descriptorFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qCritical() << descriptorFile.errorString();
        Result = false;
        return Result;
    }
    QXmlStreamReader descriptorXml(&descriptorFile);
    while(!descriptorXml.atEnd() && !descriptorXml.hasError()){
        QXmlStreamReader::TokenType token = descriptorXml.readNext();
        QStringView                 tokenName = descriptorXml.name();

        if(token == QXmlStreamReader::StartElement){
            if(tokenName == "Block"){
                QString blockUUID     = descriptorXml.attributes().value("Uuid").toString();
                QString blockName     = descriptorXml.attributes().value("Title").toString();
                QString blockType     = descriptorXml.attributes().value("Name").toString();
                QColor  blockColor    = descriptorXml.attributes().value("Color").toString();
                qreal   blockWidth    = descriptorXml.attributes().value("Width").toFloat();
                QString blockCategory = descriptorXml.attributes().value("Category").toString();
                QString blockIcon     = descriptorXml.attributes().value("Icon").toString();

                if(!blockIcon.isEmpty() && blockIcon.contains("qrc:") == false){
                    blockIcon = "file:///" + descriptorFileInfo.dir().absolutePath() + "/" + blockIcon;
                }

                if(m_new_BlockBase == nullptr){
                    if(blockUUID.isEmpty()) m_new_BlockBase = new BlockBase_class();
                    else                    m_new_BlockBase = new BlockBase_class(QUuid(blockUUID));
                    if(m_new_BlockBase != nullptr){
                        m_new_BlockBase->setName(blockName);
                        m_new_BlockBase->setType(blockType);
                        m_new_BlockBase->setColor(blockColor);
                        m_new_BlockBase->setWidth(blockWidth);
                        m_new_BlockBase->setCategory(blockCategory);
                        m_new_BlockBase->setIcon(blockIcon);
                    }else{
                        qCritical() << "Load Block Descriptor: alloc new Block failed";
                    }
                }
            }else if(tokenName == "Description"){
                QString blockDescription = descriptorXml.attributes().value("Text").toString();
                if(m_new_BlockBase != nullptr){
                    m_new_BlockBase->setDescription(blockDescription);
                }
            }else if(tokenName == "Input"){
                if(m_new_BlockBase != nullptr){
                    QString portShortName    = descriptorXml.attributes().value("ShortName").toString();
                    QString portExtendedName = descriptorXml.attributes().value("ExtName").toString();
                    QString portType         = descriptorXml.attributes().value("Type").toString();
                    bool    portVisible      = descriptorXml.attributes().value("Visible").toString() == "true";
                    QString portColor        = descriptorXml.attributes().value("Color").toString();

                    Port_class newInputPort;
                    newInputPort.setName(portShortName, portExtendedName);
                    newInputPort.setPortType(Port_class::fromString(portType));
                    if(!portColor.isEmpty()) newInputPort.setColor(portColor);
                    newInputPort.setVisible(portVisible);

                    m_new_BlockBase->addInput(newInputPort);
                }
            }else if(tokenName == "Output"){
                if(m_new_BlockBase != nullptr){
                    QString portShortName    = descriptorXml.attributes().value("ShortName").toString();
                    QString portExtendedName = descriptorXml.attributes().value("ExtName").toString();
                    QString portType         = descriptorXml.attributes().value("Type").toString();
                    bool    portVisible      = descriptorXml.attributes().value("Visible").toString() == "true";
                    QString portColor        = descriptorXml.attributes().value("Color").toString();

                    Port_class newOutputPort;
                    newOutputPort.setName(portShortName, portExtendedName);
                    newOutputPort.setPortType(Port_class::fromString(portType));
                    if(!portColor.isEmpty()) newOutputPort.setColor(portColor);
                    newOutputPort.setVisible(portVisible);

                    m_new_BlockBase->addOutput(newOutputPort);
                }
            }
        }else if(token == QXmlStreamReader::EndElement){
            if(tokenName == "Block"){
                if(m_new_BlockBase != nullptr){
                    auto findedCategory = m_map_LegacyCategories.find(m_new_BlockBase->getCategory());
                    if(findedCategory != m_map_LegacyCategories.end()){
                        findedCategory->blockList.push_back(m_new_BlockBase);
                    }else{
                        Category_s newCategory;
                        newCategory.name      = m_new_BlockBase->getCategory();
                        newCategory.visible   = false;
                        newCategory.order     = 1000 + m_int_NextOrder++;
                        newCategory.blockList.push_back(m_new_BlockBase);
                        m_map_LegacyCategories[newCategory.name] = newCategory;
                    }
                    m_new_BlockBase = nullptr;
                }
            }
        }
    }
    if(descriptorXml.hasError()){
        qWarning() << "Load Block Categories: " + descriptorXml.errorString();
    }

    return Result;
}

/* ================= QML API ================= */

QVariantList BlockManager_class::_blocksToVariant(const BlockList &blockList){
    QVariantList Result;

    for(BlockBase_class *block : blockList){
        if(block == nullptr) continue;

        QVariantMap blockMap;
        blockMap["uuid"]     = block->getId().toString(QUuid::WithoutBraces);
        blockMap["type"]     = block->getType();
        blockMap["title"]    = block->getName();
        blockMap["color"]    = block->getColor().name();
        blockMap["icon"]     = block->getIconPath();
        blockMap["description"] = block->getDescription();
        blockMap["width"]    = block->getWidth();
        blockMap["category"] = block->getCategory();
        blockMap["inputs"]   = block->getInputs().size();
        blockMap["outputs"]  = block->getOutputs().size();

        Result.push_back(blockMap);
    }
    return Result;
}

QVariantList BlockManager_class::_categoriesToVariant(bool onlyVisible) const{
    QVector<const Category_s *> vec_Sorted;

    for(auto it = m_map_LegacyCategories.cbegin(); it != m_map_LegacyCategories.cend(); ++it){
        if(onlyVisible && !it->visible) continue;
        vec_Sorted.push_back(&it.value());
    }

    std::sort(vec_Sorted.begin(), vec_Sorted.end(),
              [](const Category_s *a, const Category_s *b){ return a->order < b->order; });

    QVariantList Result;
    for(const Category_s *category : vec_Sorted){
        QVariantMap categoryMap;
        categoryMap["name"]    = category->name;
        categoryMap["icon"]    = category->iconPath;
        categoryMap["visible"] = category->visible;
        categoryMap["count"]   = category->blockList.size();
        categoryMap["blocks"]  = _blocksToVariant(category->blockList);

        Result.push_back(categoryMap);
    }
    return Result;
}

QVariantList BlockManager_class::categories() const{
    return _categoriesToVariant(false);
}

QVariantList BlockManager_class::shortcutCategories() const{
    return _categoriesToVariant(true);
}

QVariantList BlockManager_class::blocksOfCategory(const QString &categoryName) const{
    auto findedCategory = m_map_LegacyCategories.find(categoryName);
    if(findedCategory == m_map_LegacyCategories.end()) return QVariantList();

    return _blocksToVariant(findedCategory->blockList);
}
