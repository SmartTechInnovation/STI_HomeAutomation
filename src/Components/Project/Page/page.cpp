#include "page.h"

#include <QDateTime>
#include <QHash>

#include "../project.h"
#include "../../Document/document_tokens.h"
#include "../../BlockManager/blockmanager.h"
#include "../../DataTypes/datatypes.h"

Page_class::Page_class(Project_class *project, XmlNode_class *pageNode) {
    m_ptr_Project = project;
    m_ptr_PageRef = pageNode;

    for(XmlNode_class *child : m_ptr_PageRef->children()){
        if(child->TagName == TOKEN_BLOCK_INST){
            m_vec_Blocks.append(child);
        }else if(child->TagName == TOKEN_SIGNAL_IN){
             m_vec_Signals_In.append(child);
        }else if(child->TagName == TOKEN_SIGNAL_OUT){
            m_vec_Signals_Out.append(child);
        }else{
            m_vec_Unrecognised.append(child);
        }
    }

    connect(project, &Project_class::nodeInserted,     this, &Page_class::onInserted);
    connect(project, &Project_class::nodeRemoved,      this, &Page_class::onRemoved);
    connect(project, &Project_class::attributeChanged, this, &Page_class::onAttributeChanged);
}

Page_class::~Page_class(){

}

void Page_class::onInserted(XmlNode_class *parent, XmlNode_class *node){
    if(!m_ptr_PageRef->hasChild(node))
        return;
    if(node->TagName == TOKEN_BLOCK_INST){
        m_vec_Blocks.append(node);
        emit blocksChanged();
    }else if(node->TagName == TOKEN_SIGNAL_IN){

    }else if(node->TagName == TOKEN_SIGNAL_OUT){

    }
}

void Page_class::onRemoved(XmlNode_class *parent, XmlNode_class *node){
    if(m_ptr_PageRef->hasChild(node))
        return;
    if(node->TagName == TOKEN_BLOCK_INST){
        m_vec_Blocks.removeOne(node);
        emit blocksChanged();
    }else if(node->TagName == TOKEN_SIGNAL_IN){

    }else if(node->TagName == TOKEN_SIGNAL_OUT){

    }
}

void Page_class::onAttributeChanged(XmlNode_class *node,   const QString &tag){
    if(!m_ptr_PageRef->hasChild(node))
        return;
    if(node->TagName == TOKEN_BLOCK_INST){
        emit blocksChanged();
    }else if(node->TagName == TOKEN_SIGNAL_IN){

    }else if(node->TagName == TOKEN_SIGNAL_OUT){

    }
}

XmlNode_class *Page_class::pageRef() const{
    return m_ptr_PageRef;
}

QString Page_class::iUuid() const{
    return m_ptr_PageRef->value(ATTR_IUUID);
}

QString Page_class::title() const{
    return m_ptr_PageRef->value(ATTR_TITLE);
}

QString Page_class::modifiedDate() const{
    return m_ptr_PageRef->value(ATTR_MODIFIED_DATE);
}

QVariantList Page_class::getBlocks() const{
    QVariantList BlockList;
    for(auto &item : m_vec_Blocks){
        QVariantMap BlockMap;
        BlockManager_class::BlockType_s *blockRef = S_BlockManager.getBlockTypeByUuid(QUuid(item->value(ATTR_UUID)));

        BlockMap["tagName"] = item->TagName;
        BlockMap["iUuid"]   = item->value(ATTR_IUUID);
        BlockMap["uuid"]    = item->value(ATTR_UUID);
        BlockMap["pX"]      = item->value(ATTR_PX,    QStringLiteral("0")).toDouble();
        BlockMap["pY"]      = item->value(ATTR_PY,    QStringLiteral("0")).toDouble();
        BlockMap["title"]   = item->value(ATTR_TITLE, QStringLiteral("Missing"));
        BlockMap["type"]    = item->value(ATTR_TYPE,  QStringLiteral(""));
        BlockMap["color"]   = item->value(ATTR_COLOR, QStringLiteral("#7D8491"));
        BlockMap["width"]   = item->value(ATTR_WIDTH, QStringLiteral("400")).toDouble();
        BlockMap["compact"] = item->value(ATTR_COMPACT, QStringLiteral("false")) == "true";

        if(blockRef){
            BlockMap["icon"] = blockRef->Icon;
        }else{
            BlockMap["icon"] = "qrc:/icons/misc/unknown.svg";
        }

        BlockMap["missing"] = blockRef == nullptr;

        QVariantList PortInputList;
        QVariantList PortOutputList;
        QVariantList PortProperties;
        if(blockRef != nullptr){
            auto defPortMap = [this](BlockManager_class::PortDefinition_s port) -> QVariantMap {
                QVariantMap PortMap;
                PortMap["shortName"] = port.ShortName;
                PortMap["title"]     = port.Title;
                PortMap["dataType"]  = port.DataType;
                PortMap["default"]   = port.Default;
                PortMap["unit"]      = port.Unit;
                PortMap["docs"]      = port.Docs;
                PortMap["visible"]   = port.Visible;
                PortMap["color"]     = S_DataTypes.color(port.DataType);
                QVariantList PortVariantList;
                for(auto &Variant : port.Variants){
                    QVariantMap PortVariantMap;
                    PortVariantMap["data"] = Variant.Data;
                    PortVariantMap["text"] = Variant.Text;
                    PortVariantList.append(PortVariantMap);
                }
                PortMap["variants"]  = PortVariantList;
                return PortMap;
            };

            for(auto &port: blockRef->Inputs){
                PortInputList.append(defPortMap(port));
            }
            for(auto &port: blockRef->Properties){
                PortInputList.append(defPortMap(port));
            }
            for(auto &port: blockRef->Outputs){
                PortOutputList.append(defPortMap(port));
            }
        }

        BlockMap["inputs"]  = PortInputList;
        BlockMap["outputs"] = PortOutputList;
        BlockMap["properties"] = PortProperties;

        BlockList.append(BlockMap);
    }
    return BlockList;
}

QVariantMap Page_class::getBlockGhost(const QString &uuid) const{
    QVariantMap BlockGhost;
    BlockManager_class::BlockType_s *blockRef = S_BlockManager.getBlockTypeByUuid(QUuid(uuid));

    BlockGhost["tagName"] = TOKEN_BLOCK_INST;
    BlockGhost["iUuid"]   = QUuid();
    BlockGhost["uuid"]    = blockRef->Uuid.toString(QUuid::WithoutBraces);
    BlockGhost["pX"]      = 0;
    BlockGhost["pY"]      = 0;
    BlockGhost["title"]   = blockRef->Title;
    BlockGhost["type"]    = blockRef->Type;
    BlockGhost["color"]   = blockRef->Color;
    BlockGhost["width"]   = blockRef->Width;
    BlockGhost["compact"] = false;

    if(blockRef){
        BlockGhost["icon"] = blockRef->Icon;
    }else{
        BlockGhost["icon"] = "qrc:/icons/misc/unknown.svg";
    }

    BlockGhost["missing"] = blockRef == nullptr;

    QVariantList PortInputList;
    QVariantList PortOutputList;
    QVariantList PortProperties;
    if(blockRef != nullptr){
        auto defPortMap = [this](BlockManager_class::PortDefinition_s port) -> QVariantMap {
            QVariantMap PortMap;
            PortMap["shortName"] = port.ShortName;
            PortMap["title"]     = port.Title;
            PortMap["dataType"]  = port.DataType;
            PortMap["default"]   = port.Default;
            PortMap["unit"]      = port.Unit;
            PortMap["docs"]      = port.Docs;
            PortMap["visible"]   = port.Visible;
            PortMap["color"]     = S_DataTypes.color(port.DataType);
            QVariantList PortVariantList;
            for(auto &Variant : port.Variants){
                QVariantMap PortVariantMap;
                PortVariantMap["data"] = Variant.Data;
                PortVariantMap["text"] = Variant.Text;
                PortVariantList.append(PortVariantMap);
            }
            PortMap["variants"]  = PortVariantList;
            return PortMap;
        };

        for(auto &port: blockRef->Inputs){
            PortInputList.append(defPortMap(port));
        }
        for(auto &port: blockRef->Properties){
            PortInputList.append(defPortMap(port));
        }
        for(auto &port: blockRef->Outputs){
            PortOutputList.append(defPortMap(port));
        }
    }

    BlockGhost["inputs"]  = PortInputList;
    BlockGhost["outputs"] = PortOutputList;
    BlockGhost["properties"] = PortProperties;

    return BlockGhost;
}

QVariantList Page_class::connections() const{
    QVariantList Result;


    return Result;
}