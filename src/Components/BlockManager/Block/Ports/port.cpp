#include "port.h"

Port_class::Port_class(QObject *parent) : QObject(parent) {
    m_ptr_Parent = parent;
    m_Uuid       = QUuid();
}

void Port_class::operator=(const Port_class &port){
    m_Uuid             = port.m_Uuid;
    m_str_Name         = port.m_str_Name;
    m_str_ExtName      = port.m_str_ExtName;
    m_color            = port.m_color;
    m_b_Visible        = port.m_b_Visible;
    m_b_Connected      = port.m_b_Connected;
    m_int_PortType     = port.m_int_PortType;
    m_ptr_LiveDataRef  = port.m_ptr_LiveDataRef;
    m_Position         = port.m_Position;
    m_bool_CustomColor = port.m_bool_CustomColor;
    m_str_Docs         = port.m_str_Docs;
    m_str_DefaultValue = port.m_str_DefaultValue;
    m_str_Unit         = port.m_str_Unit;
    m_vec_Variant      = port.m_vec_Variant;
    m_Uuid             = port.m_Uuid;
    m_vec_Sources      = port.m_vec_Sources;
}

bool Port_class::addSource(const QUuid &Source){
    if(Source.isNull() || m_vec_Sources.contains(Source)) return false;
    m_vec_Sources.push_back(Source);
    return true;
}

bool Port_class::removeSource(const QUuid &Source){
    return m_vec_Sources.removeAll(Source) > 0;
}

QVariantMap Port_class::getQVariantMap(){
    QVariantMap portMap;
    portMap["uuid"]      = m_Uuid.toString(QUuid::WithoutBraces);
    portMap["name"]      = m_str_Name;
    portMap["extName"]   = m_str_ExtName;
    portMap["color"]     = getColor();
    portMap["visible"]   = m_b_Visible;
    portMap["connected"] = m_b_Connected;
    portMap["type"]      = toString((PortType_e)m_int_PortType);
    portMap["typeId"]    = m_int_PortType;
    portMap["unit"]      = m_str_Unit;
    return portMap;
}

QString    Port_class::toString(PortType_e PortType){
    QString str_DataType = "Custom";
    switch (PortType){
        case PortType_Void:   str_DataType = "Void";   break;
        case PortType_Bool:   str_DataType = "Bool";   break;
        case PortType_Int:    str_DataType = "Int";    break;
        case PortType_Float:  str_DataType = "Float";  break;
        case PortType_String: str_DataType = "String"; break;
        case PortType_Enum:   str_DataType = "Enum";   break;
        case PortType_Date:   str_DataType = "Date";   break;
        case PortType_Custom: str_DataType = "Custom"; break;
        default:              str_DataType = "Custom";
    }
    return str_DataType;
}

Port_class::PortType_e Port_class::fromString(QString PortType){
    if(PortType == "Void")   return PortType_Void;
    if(PortType == "Bool")   return PortType_Bool;
    if(PortType == "Int")    return PortType_Int;
    if(PortType == "Float")  return PortType_Float;
    if(PortType == "String") return PortType_String;
    if(PortType == "Enum")   return PortType_Enum;
    if(PortType == "Date")   return PortType_Date;
    if(PortType == "Custom") return PortType_Custom;

    return PortType_Custom;
}

QColor     Port_class::getColor(PortType_e PortType){
    QColor portColor = 0xA9C7FF;

    switch (PortType){
    case PortType_Void:   portColor = 0xA9C7FF; break;
    case PortType_Bool:   portColor = 0x9B0000; break;
    case PortType_Int:    portColor = 0x13DB9D; break;
    case PortType_Float:  portColor = 0xA0F032; break;
    case PortType_String: portColor = 0xFF00D4; break;
    case PortType_Custom: portColor = 0xFCD116; break;
    default:              portColor = 0xFCD116;
    }
    return portColor;
}

bool       Port_class::compatible(PortType_e out, PortType_e in){
    bool Result = false;
    if(out == in){
        Result = true;
    }else if(out == PortType_Bool){
        if(in == PortType_Int)   Result = true;
        if(in == PortType_Float) Result = true;
    }else if(out == PortType_Int){
        if(in == PortType_Bool)  Result = true;
        if(in == PortType_Float) Result = true;
    }else if(out == PortType_Float){
        if(in == PortType_Bool)  Result = true;
        if(in == PortType_Int)   Result = true;
    }
    return Result;
}