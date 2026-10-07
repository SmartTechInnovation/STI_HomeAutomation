#include "variable.h"

#include "../../BlockManager/Block/Ports/port.h"

Variable_class::Variable_class() {}

VariableTypes_class::VariableTypes_class(){
    for(int i = 0; i <= Port_class::PortType_e::PortType_Custom; i++){
        QString StringType = Port_class::toString((Port_class::PortType_e)i);
        VariableType_class *newVariableType = new VariableType_class();
        newVariableType->Uuid  = QUuid().createUuidV7();
        newVariableType->IUuid = QUuid().createUuidV7();
        newVariableType->Title = StringType;
        m_vec_VariablesType.append(newVariableType);
    }
}
