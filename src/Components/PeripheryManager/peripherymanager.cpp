#include "peripherymanager.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>

#include "peripherymanager_tokens.h"
#include "../../Setup/Setup.hpp"
#include "../Logger/logger.h"

PeripheryManager_class S_PeripheryManager;

#define PERIPHERY_NAME "Periphery Manager"

PeripheryManager_class::PeripheryManager_class(QObject *parent) : QObject(parent) {

}

PeripheryManager_class::~PeripheryManager_class(){

}

void PeripheryManager_class::begin(){
    load(PERIPHERY_PATH);
}

bool PeripheryManager_class::isTagSection(const QString &tagName){
    for(auto &section : m_map_Sections){
        if(section->value(ATTR_TAG_NAME) == tagName)
            return true;
    }
    return false;
}

XmlNode_class *PeripheryManager_class::getNode(const QString &tagName){
    for(auto &section : m_map_Sections){
        if(section->value(ATTR_TAG_NAME) == tagName)
            return section;
    }
    for(auto &node : m_map_NodeTypes){
        if(node->value(ATTR_TAG_NAME) == tagName)
            return node;
    }
    return nullptr;
}

XmlNode_class *PeripheryManager_class::getNode(const QUuid &uuid){
    auto finded = m_map_NodeTypes.find(uuid);
    if(finded != m_map_NodeTypes.end())
        return finded.value();
    return nullptr;
}

bool PeripheryManager_class::load(const QString &FolderPath){
    if(!QDir(FolderPath).exists()){
        S_Logger.write(Log::Warning, PERIPHERY_NAME, "Periphery library not found: " + FolderPath, "Periphery library", "Copy the Periphery folder next to Legacy", true);
        return false;
    }
    QDirIterator it(FolderPath, QStringList{ "*.xml" }, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()){
        QString Path = it.next();
        QFile descriptorFile(Path);
        if(!descriptorFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            continue;
        }
        QXmlStreamReader descriptorXml(&descriptorFile);
        /* === Read until Element is detected */
        while(!descriptorXml.atEnd() && !descriptorXml.isStartElement()) {
            descriptorXml.readNext();
        }
        if(!descriptorXml.isStartElement())
            continue;
        XmlNode_class *root = XmlNode_class::read(descriptorXml);
        if(descriptorXml.hasError()){
            S_Logger.write(Log::Warning, PERIPHERY_NAME, Path + ": " + descriptorXml.errorString() + " line " + QString::number(descriptorXml.lineNumber()), "Periphery descriptor", "Fix the XML", true);
            continue;
        }
        parceNodeTypeXml(root);
    }
    return true;
}

void PeripheryManager_class::parceNodeTypeXml(XmlNode_class *node){
    if(node->Tag == TOKEN_NODE_TYPE){
        QUuid nodeUuid = QUuid(node->value(ATTR_UUID));
        if(node->hasAttr("Section") && node->value("Section") == "true"){
            m_map_Sections[nodeUuid] = node;
        }else{
            m_map_NodeTypes[nodeUuid] = node;
        }
    }
    for(auto &child : node->Children){
        parceNodeTypeXml(child);
    }
}

QVariantList PeripheryManager_class::getSections(){
    QVariantList Result;

    for(auto &section : m_map_Sections){
        QVariantMap Section;
        Section[ATTR_UUID]  = section->value(ATTR_UUID);
        Section[ATTR_TITLE] = section->value(ATTR_TITLE);
        Section[ATTR_ICON]  = section->value(ATTR_ICON);
        Result.append(Section);
    }
    return Result;
}

QVariantMap PeripheryManager_class::getSubMenu(const QUuid &uuid){
    QVariantMap Result;

    XmlNode_class *selectedNode = nullptr;
    auto finded = m_map_Sections.find(uuid);
    if(finded != m_map_Sections.end()){
        selectedNode = finded.value();
    }else{
        finded = m_map_NodeTypes.find(uuid);
        if(finded != m_map_NodeTypes.end()){
            selectedNode = finded.value();
        }
    }
    if(selectedNode != nullptr){
        Result[ATTR_TITLE] = selectedNode->value(ATTR_TITLE);
        Result[ATTR_MENU]  = selectedNode->value(ATTR_MENU);
        Result[ATTR_ICON]  = selectedNode->value(ATTR_ICON);
        QVariantList Items;
        for(auto &child : selectedNode->Children){
            if(child->Tag == TOKEN_DESCRIPTION){
                Result[TOKEN_DESCRIPTION] = child->value(ATTR_TEXT);
            }else if(child->Tag == TOKEN_CHILD){
                XmlNode_class *nodeRef = getNode(QUuid(child->value(ATTR_UUID)));
                if(nodeRef != nullptr){
                    QVariantMap Item;
                    Item[ATTR_UUID]    = nodeRef->value(ATTR_UUID);
                    Item[ATTR_TITLE]   = nodeRef->value(ATTR_TITLE);
                    Item[ATTR_ICON]    = nodeRef->value(ATTR_ICON);
                    Item[ATTR_VISIBLE] = child->value(ATTR_VISIBLE) == "true";
                    for(auto &child : nodeRef->Children){
                        if(child->Tag == TOKEN_DESCRIPTION){
                            Item[TOKEN_DESCRIPTION] = child->value(ATTR_TEXT);
                            break;
                        }
                    }
                    Items.append(Item);
                }
            }
        }
        Result[ATTR_ITEMS] = Items;
    }
    return Result;
}