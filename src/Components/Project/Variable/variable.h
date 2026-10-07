#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QVector>
#include <QUuid>

#include "../Generic/generic.h"

class Variable_class : public Generic_class
{
public: /* Typedefs and enums */

public: /* Members */
    int     Type;
    QString Default;

    QVector<QUuid> m_vec_SignalSources;
public:
    Variable_class();
};

class VariableType_class : public Generic_class
{
public: /* Typedefs and enums */

public: /* Members   */
    QVector<Variable_class *> m_vec_Variables;
public: /* Functions */
    VariableType_class() {}
};

class VariableTypes_class : public Generic_class
{
public: /* Typedefs and enums */

public: /* Members   */
    QVector<VariableType_class *> m_vec_VariablesType;
public: /* Functions */
    explicit VariableTypes_class();
};



#endif // VARIABLE_H
