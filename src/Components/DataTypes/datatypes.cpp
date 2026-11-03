#include "datatypes.h"

DataTypes_class S_DataTypes;

DataTypes_class::DataTypes_class(QObject *parent) : QObject(parent) {
    m_UnknownDataType.Name  = QString();
    m_UnknownDataType.Color = QColor(0x7D8491);

    addDataType(QStringLiteral("Void"),   QColor(0xA9C7FF));
    addDataType(QStringLiteral("Bool"),   QColor(0x9B0000), { QStringLiteral("Int"),  QStringLiteral("Float") });
    addDataType(QStringLiteral("Int"),    QColor(0x13DB9D), { QStringLiteral("Bool"), QStringLiteral("Float") });
    addDataType(QStringLiteral("Float"),  QColor(0xA0F032), { QStringLiteral("Bool"), QStringLiteral("Int")   });
    addDataType(QStringLiteral("String"), QColor(0xFF00D4));
    addDataType(QStringLiteral("Enum"),   QColor(0xFCD116));
    addDataType(QStringLiteral("Date"),   QColor(0xFCD116));
    addDataType(QStringLiteral("Custom"), QColor(0xFCD116));
}

void DataTypes_class::addDataType(const QString &name, const QColor &color, const QStringList &convertsTo){
    DataType_s newDataType;
    newDataType.Name       = name;
    newDataType.Color      = color;
    newDataType.ConvertsTo = convertsTo;
    m_hash_DataTypeByName[name] = newDataType;
}

QColor DataTypes_class::color(const QString &dataTypeName) const {
    return m_hash_DataTypeByName.value(dataTypeName, m_UnknownDataType).Color;
}

bool DataTypes_class::canConnect(const QString &outputDataType, const QString &inputDataType) const {
    if(!m_hash_DataTypeByName.contains(outputDataType) || !m_hash_DataTypeByName.contains(inputDataType))
        return false;
    if(outputDataType == inputDataType)
        return true;
    return m_hash_DataTypeByName.value(outputDataType).ConvertsTo.contains(inputDataType);
}
