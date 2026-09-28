#include "blockbase.h"

BlockBase_class::BlockBase_class() {
    Uuid = QUuid::createUuid();
}

BlockBase_class::BlockBase_class(QUuid uuid){
    Uuid = uuid;
}

BlockBase_class &BlockBase_class::operator=(const BlockBase_class &Ref){
    Uuid        = Ref.Uuid;
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
        m_vec_Inputs.push_back(newPort);
    }
    for(auto &port : Ref.m_vec_Properties){
        Port_class *newPort = new Port_class();
        *newPort = *port;
        m_vec_Inputs.push_back(newPort);
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
    m_vec_Outputs.push_back(newPort);
}