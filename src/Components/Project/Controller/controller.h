#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QVector>

class Controller_class
{
public: /*Typedefs and enums */
    struct VirtualInputs_s{
        QString Title;

    };

    struct VirtualOutputs_s{
        QString Title;

    };
public: /* Members */
    QString Title;
    QString IpAddress;
    int     PortHttp  = 80;
    int     PortHttps = 443;
    QString SerialNr;
    QString Model;

    /* Periphery */
    VirtualInputs_s VirtualInputs;
    VirtualInputs_s VirtualOutputs;
public:
    Controller_class();
};

#endif // CONTROLLER_H
