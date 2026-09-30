#include "blockbase.h"

BlockBase_class::BlockBase_class() {
    Uuid = QUuid::createUuid();
}

BlockBase_class::BlockBase_class(QUuid uuid, QUuid instanceUuid){
    Uuid  = uuid;
    IUuid = instanceUuid;
}

BlockBase_class &BlockBase_class::operator=(const BlockBase_class &Ref){
    Uuid        = Ref.Uuid;
    IUuid = QUuid(); // New uniq Id
    Title       = Ref.Title;
    Type        = Ref.Type;
    Description = Ref.Description;
    Color       = Ref.Color;
    Width       = Ref.Width;
    IconPath    = Ref.IconPath;
    Room        = Ref.Room;
    Category    = Ref.Category;
    for(auto &port : Ref.m_vec_Inputs){
        Port_class *newPort = new Port_class();
        *newPort = *port;
        m_vec_Inputs.push_back(newPort);
    }
    for(auto &port : Ref.m_vec_Outputs){
        Port_class *newPort = new Port_class();
        *newPort = *port;
        m_vec_Outputs.push_back(newPort);
    }
    for(auto &port : Ref.m_vec_Properties){
        Port_class *newPort = new Port_class();
        *newPort = *port;
        m_vec_Properties.push_back(newPort);
    }

    return *this;
}

void BlockBase_class::addInput(Port_class  &newInputPort){
    Port_class *newPort = new Port_class();
    *newPort = newInputPort;
    m_vec_Inputs.push_back(newPort);
}

void BlockBase_class::addOutput(Port_class &newOutputPort){
    Port_class *newPort = new Port_class();
    *newPort = newOutputPort;
    m_vec_Outputs.push_back(newPort);
}

void BlockBase_class::addProperty(Port_class &newPropertyPort){
    Port_class *newPort = new Port_class();
    *newPort = newPropertyPort;
    m_vec_Properties.push_back(newPort);
}

QVariantMap BlockBase_class::getQVariantMap(){
    QVariantMap blockMap;
    blockMap["uuid"]     = Uuid.toString(QUuid::WithoutBraces);
    blockMap["iUuid"]    = IUuid.toString(QUuid::WithoutBraces);
    blockMap["title"]    = Title;
    blockMap["type"]     = Type;
    blockMap["color"]    = Color;
    blockMap["icon"]     = IconPath;
    blockMap["position"]  = Position;
    blockMap["width"]    = Width;
    blockMap["room"]     = Room;
    blockMap["category"] = Category;
    QVariantList inputsVariants, outputsVariants, propertiesVariants;
    for(int i = 0; i < m_vec_Inputs.size(); i++){
        QVariantMap portMap = m_vec_Inputs[i]->getQVariantMap();
        portMap["index"] = i;
        inputsVariants.push_back(portMap);
    }
    for(int i = 0; i < m_vec_Outputs.size(); i++){
        QVariantMap portMap = m_vec_Outputs[i]->getQVariantMap();
        portMap["index"] = i;
        outputsVariants.push_back(portMap);
    }
    for(int i = 0; i < m_vec_Properties.size(); i++){
        QVariantMap portMap = m_vec_Properties[i]->getQVariantMap();
        portMap["index"] = i;
        propertiesVariants.push_back(portMap);
    }
    blockMap["inputs"]     = inputsVariants;
    blockMap["outputs"]    = outputsVariants;
    blockMap["properties"] = propertiesVariants;

    return blockMap;
}