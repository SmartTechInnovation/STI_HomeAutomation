#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QVector>
#include <QVariantMap>

#include "../../PeripheryManager/xmlnode.h"
#include "../Generic/generic.h"

class Controller_class : public Generic_class
{
public: /*Typedefs and enums */

public: /* Members */
    QString IpAddress;
    int     PortHttp  = 80;
    int     PortHttps = 443;
    QString SerialNr;
    QString Model;

    /* Periphery */
    QVector<XmlNode_class *> m_vec_Periphery;
public:
    Controller_class();

    QVariantMap getContextMenu(const QUuid &uuid) const;
};

#endif // CONTROLLER_H
