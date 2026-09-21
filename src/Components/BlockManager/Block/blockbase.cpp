#include "blockbase.h"

BlockBase_class::BlockBase_class() {
    m_uuid_BlockId = QUuid::createUuid();
}

BlockBase_class::BlockBase_class(QUuid uuid){
    m_uuid_BlockId = uuid;
}

void BlockBase_class::setType(const QString &Type){
    m_str_Type = Type;
}

void BlockBase_class::setWidth(float width){
    m_real_Width = width;
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

void BlockBase_class::setName (const QString &Name){
    m_str_Name = Name;
}

void BlockBase_class::setColor(const QColor  &Color){
    m_color = Color;
}

void BlockBase_class::setIcon(const QString &IconPath){
    m_str_IconPath = IconPath;
}

void BlockBase_class::setDescription(const QString &Description){
    m_str_Description = Description;
}

void BlockBase_class::select(bool value){
    m_b_Selected = value;
}

void BlockBase_class::setRoom (const QString  &Room){
    m_str_Room = Room;
}

void BlockBase_class::setCategory(const QString &Category){
    m_str_Category = Category;
}
