#ifndef DATATYPES_H
#define DATATYPES_H

#include <QObject>
#include <QColor>
#include <QHash>
#include <QString>
#include <QStringList>

class DataTypes_class : public QObject
{
    Q_OBJECT
public:  /* Typedefs and enums */
    struct DataType_s {
        QString     Name;
        QColor      Color;
        QStringList ConvertsTo;     // an output of this type can feed inputs of these types
    };

private: /* Members */
    QHash<QString, DataType_s> m_hash_DataTypeByName;
    DataType_s                 m_UnknownDataType;

private: /* Functions */
    void addDataType(const QString &name, const QColor &color, const QStringList &convertsTo = QStringList());

public:  /* Functions */
    explicit DataTypes_class(QObject *parent = nullptr);

    Q_INVOKABLE QColor color(const QString &dataTypeName) const;
    Q_INVOKABLE bool   canConnect(const QString &outputDataType, const QString &inputDataType) const;
};

extern DataTypes_class S_DataTypes;

#endif // DATATYPES_H
