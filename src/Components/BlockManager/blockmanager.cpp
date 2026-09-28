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

/* ================= QML API ================= */


QVariantList BlockManager_class::categories() const{
    QVariantList Result;

    QVector<const Category_s *> vec_Sorted;
    for(auto it = m_map_LegacyCategories.cbegin(); it != m_map_LegacyCategories.cend(); ++it){
        vec_Sorted.push_back(&it.value());
    }
    std::sort(vec_Sorted.begin(), vec_Sorted.end(),
              [](const Category_s *a, const Category_s *b){ return a->order < b->order; });

    for(const Category_s *category : vec_Sorted){
        QVariantMap categoryMap;
        categoryMap["name"]    = category->title;
        categoryMap["icon"]    = category->iconPath;
        categoryMap["visible"] = category->visible;
        categoryMap["count"]   = category->blockList.size();

        QVariantList blocksList;
        for(auto &block : category->blockList){
            QVariantMap blockMap;
            blockMap["uuid"]     = block->Uuid.toString(QUuid::WithoutBraces);
            blockMap["title"]    = block->Title;
            blockMap["icon"]     = block->IconPath;
            blockMap["description"] = block->Description;
            blocksList.push_back(blockMap);
        }
        categoryMap["blocks"]  = blocksList;

        Result.push_back(categoryMap);
    }

    return Result;
}

QVariantList BlockManager_class::shortcutCategories() const{
    QVariantList Result;

    QVector<const Category_s *> vec_Sorted;
    for(auto it = m_map_LegacyCategories.cbegin(); it != m_map_LegacyCategories.cend(); ++it){
        if(!it->visible) continue;
        vec_Sorted.push_back(&it.value());
    }
    std::sort(vec_Sorted.begin(), vec_Sorted.end(),
              [](const Category_s *a, const Category_s *b){ return a->order < b->order; });

    for(const Category_s *category : vec_Sorted){
        QVariantMap categoryMap;
        categoryMap["name"]    = category->title;
        categoryMap["icon"]    = category->iconPath;
        categoryMap["visible"] = category->visible;
        categoryMap["count"]   = category->blockList.size();

        QVariantList blocksList;
        for(auto &block : category->blockList){
            QVariantMap blockMap;
            blockMap["uuid"]     = block->Uuid.toString(QUuid::WithoutBraces);
            blockMap["title"]    = block->Title;
            blockMap["icon"]     = block->IconPath;
            blockMap["description"] = block->Description;
            blocksList.push_back(blockMap);
        }
        categoryMap["blocks"]  = blocksList;

        Result.push_back(categoryMap);
    }

    return Result;
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

    /* Working Temporary Members */
    int m_int_NextOrder = 0;

    QXmlStreamReader descriptorXml(&descriptorFile);
    while(!descriptorXml.atEnd() && !descriptorXml.hasError()){
        QXmlStreamReader::TokenType token = descriptorXml.readNext();
        QStringView                 tokenName = descriptorXml.name();

        if(token == QXmlStreamReader::StartElement){
            if(tokenName == "Category"){
                Category_s newCategory;
                newCategory.title       = descriptorXml.attributes().value("Name").toString();
                newCategory.iconPath    = descriptorXml.attributes().value("Icon").toString();
                newCategory.visible     = descriptorXml.attributes().value("Visible").toString() == "true";
                newCategory.order       = m_int_NextOrder++;

                if(!newCategory.iconPath.isEmpty() && newCategory.iconPath.contains("qrc:") == false){
                    newCategory.iconPath = "file:///" + descriptorFileInfo.dir().absolutePath() + "/" + newCategory.iconPath;
                }

                if(m_map_LegacyCategories.find(newCategory.title) == m_map_LegacyCategories.end()){
                    m_map_LegacyCategories[newCategory.title] = newCategory;
                }else{
                    Category_s *existingCategory = &m_map_LegacyCategories[newCategory.title];
                    existingCategory->title    = newCategory.title;
                    existingCategory->iconPath = newCategory.iconPath;
                    existingCategory->visible  = newCategory.visible;
                    existingCategory->order    = newCategory.order;
                }
            }
        }else if(token == QXmlStreamReader::EndElement){

        }
    }
    if(descriptorXml.hasError()){
        qWarning() << "Load Categories: " + descriptorXml.errorString() + " :" + QString::number(descriptorXml.lineNumber()) + ":" + QString::number(descriptorXml.columnNumber());
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
    /* Working Temporary Members */
    BlockBase_class *m_ptr_BlockBase  = nullptr;
    Port_class       m_temp_PortBase;
    int              m_int_NextOrder = 0;

    QXmlStreamReader descriptorXml(&descriptorFile);
    while(!descriptorXml.atEnd() && !descriptorXml.hasError()){
        QXmlStreamReader::TokenType token = descriptorXml.readNext();
        QStringView                 tokenName = descriptorXml.name();

        if(token == QXmlStreamReader::StartElement){
            if(tokenName == "Block"){
                QString blockUUID     = descriptorXml.attributes().value("Uuid").toString();
                QString blockTitle    = descriptorXml.attributes().value("Title").toString();
                QString blockType     = descriptorXml.attributes().value("Name").toString();
                QColor  blockColor    = descriptorXml.attributes().value("Color").toString();
                qreal   blockWidth    = descriptorXml.attributes().value("Width").toFloat();
                QString blockCategory = descriptorXml.attributes().value("Category").toString();
                QString blockIcon     = descriptorXml.attributes().value("Icon").toString();

                if(!blockIcon.isEmpty() && blockIcon.contains("qrc:") == false){
                    blockIcon = "file:///" + descriptorFileInfo.dir().absolutePath() + "/" + blockIcon;
                }

                if(m_ptr_BlockBase == nullptr){
                    if(blockUUID.isEmpty()) m_ptr_BlockBase = new BlockBase_class();
                    else                    m_ptr_BlockBase = new BlockBase_class(QUuid(blockUUID));
                    if(m_ptr_BlockBase != nullptr){
                        m_ptr_BlockBase->Title = blockTitle;
                        m_ptr_BlockBase->Type  = blockType;
                        m_ptr_BlockBase->Color = blockColor;
                        m_ptr_BlockBase->Width = blockWidth;
                        m_ptr_BlockBase->Category = blockCategory;
                        m_ptr_BlockBase->IconPath = blockIcon;

                        auto findedCategory = m_map_LegacyCategories.find(m_ptr_BlockBase->Category);
                        if(findedCategory != m_map_LegacyCategories.end()){
                            findedCategory->blockList.push_back(m_ptr_BlockBase);
                        }else{
                            Category_s newCategory;
                            newCategory.title      = m_ptr_BlockBase->Category;
                            newCategory.visible   = false;
                            newCategory.order     = 1000 + m_int_NextOrder++;
                            newCategory.blockList.push_back(m_ptr_BlockBase);
                            m_map_LegacyCategories[newCategory.title] = newCategory;
                        }
                    }else{
                        qCritical() << "Load Block Descriptor: alloc new Block failed";
                    }
                }
            }else if(tokenName == "Description"){
                if(m_ptr_BlockBase != nullptr){
                    QString blockDescription = descriptorXml.attributes().value("Text").toString();
                    m_ptr_BlockBase->Description = blockDescription;
                }
            }else if(tokenName == "Input" || tokenName == "Output" || tokenName == "Property"){
                if(m_ptr_BlockBase != nullptr){
                    QString portShortName    = descriptorXml.attributes().value("ShortName").toString();
                    QString portExtendedName = descriptorXml.attributes().value("ExtName").toString();
                    QString portType         = descriptorXml.attributes().value("Type").toString();
                    bool    portVisible      = descriptorXml.attributes().value("Visible").toString() == "true";
                    QString portColor        = descriptorXml.attributes().value("Color").toString();
                    QString portDefaultValue = descriptorXml.attributes().value("Default").toString();
                    QString portUnit         = descriptorXml.attributes().value("Unit").toString();

                    m_temp_PortBase = Port_class();
                    m_temp_PortBase.setName(portShortName, portExtendedName);
                    m_temp_PortBase.setPortType(Port_class::fromString(portType));
                    if(!portColor.isEmpty())  m_temp_PortBase.setColor(portColor);
                    m_temp_PortBase.setVisible(portVisible);
                    m_temp_PortBase.setDefaultValue(portDefaultValue);
                    m_temp_PortBase.setUnit(portUnit);
                }
            }else if(tokenName == "Docs"){
                QString docsText = descriptorXml.attributes().value("Text").toString();
                if(m_ptr_BlockBase != nullptr){
                    m_temp_PortBase.setDocumentation(docsText);
                }
            }else if(tokenName == "Variant"){
                QString propertyVariantData = descriptorXml.attributes().value("Data").toString();
                QString propertyVariantText = descriptorXml.attributes().value("Text").toString();

                Port_class::PortVariant_s newVariant;
                newVariant.Data = propertyVariantData;
                newVariant.Text = propertyVariantText;
                if(m_ptr_BlockBase != nullptr){
                    m_temp_PortBase.addVariant(newVariant);
                }
            }
        }else if(token == QXmlStreamReader::EndElement){
            if(tokenName == "Block"){
                m_ptr_BlockBase = nullptr;
            }else if(tokenName == "Input"){
                if(m_ptr_BlockBase != nullptr){
                    m_ptr_BlockBase->addInput(m_temp_PortBase);
                }
            }else if(tokenName == "Output"){
                if(m_ptr_BlockBase != nullptr){
                    m_ptr_BlockBase->addOutput(m_temp_PortBase);
                }
            }else if(tokenName == "Property"){
                if(m_ptr_BlockBase != nullptr){
                    m_ptr_BlockBase->addProperty(m_temp_PortBase);
                }
            }
        }
    }
    if(descriptorXml.hasError()){
        qWarning() << "Load Blocks Description: " + descriptorFileInfo.dir().dirName() + ": " + descriptorXml.errorString() + " :" + QString::number(descriptorXml.lineNumber()) + ":" + QString::number(descriptorXml.columnNumber());
    }

    return Result;
}

BlockBase_class *BlockManager_class::getInstance(QUuid &Uuid){
    BlockBase_class *refBlockBase = nullptr;
    for(auto &category : m_map_LegacyCategories){
        for(auto &block : category.blockList){
            if(block->Uuid == Uuid){
                refBlockBase = block;
                break;
            }
        }
        if(refBlockBase != nullptr)
            break;
    }
    if(refBlockBase != nullptr){
        BlockBase_class *newInstance = new BlockBase_class(refBlockBase->Uuid);
        *newInstance = *refBlockBase;
        return newInstance;
    }
    return nullptr;
}