#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include <QUuid>
#include <QString>
#include <QColor>
#include <QVector>
#include <QPointF>
#include <QVariantMap>

#include "Ports/port.h"

class BlockBase_class
{
public: /* Typedef and enums */
public: /* Members */
    QUuid   Uuid;                 // Block Identifier (From Templates)
    QUuid   IUuid;                // Block Instance Identifier (Unique)
    QString Title        = "Unnamed"; // Editable Name
    QString Type         = "Unnamed"; // Fixed Type Name
    QString Description  = "";
    QColor  Color        = 0x7D8491;  // Block Color
    qreal   Width        = 400;       // Block Width
    bool    Selected     = false;     // Selected
    QPointF Position   = QPointF(0,0);
    QString IconPath     = "";

    QString Room         = "Not Assigned"; // Block Room
    QString Category     = "Not Assigned"; // Block Category

    QVector<Port_class *> m_vec_Inputs;
    QVector<Port_class *> m_vec_Outputs;
    QVector<Port_class *> m_vec_Properties;
public:
    BlockBase_class();
    BlockBase_class(QUuid uuid, QUuid instanceUuid = QUuid());

    BlockBase_class &operator =(const BlockBase_class &Ref);
    void addInput   (Port_class &newInputPort);
    void addOutput  (Port_class &newInputPort);
    void addProperty(Port_class &newPropertyPort);

    QVariantMap getQVariantMap();
};

#endif // BLOCKBASE_H
