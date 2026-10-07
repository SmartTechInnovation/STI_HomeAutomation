#ifndef RIGHTS_H
#define RIGHTS_H

#include <QString>
#include <QVector>

#include "../Generic/generic.h"

class Right_class : public Generic_class
{
public:
    QString PermisionId;
public:
    explicit Right_class();
};

class Rights_class : public Generic_class
{
public: /* Members */
    QVector<Right_class *> m_vec_Rights;
public: /* Funcitons */
    explicit Rights_class();
};

#endif // RIGHTS_H
