#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QVector>
#include <QUuid>

class Variable_class
{
public: /* Typedefs and enums */

public: /* Members */
    QString Title;
    int     Type;
    QUuid   Uuid;

    QVector<QUuid> m_vec_SignalSources;
public:
    Variable_class();
};

#endif // VARIABLE_H
