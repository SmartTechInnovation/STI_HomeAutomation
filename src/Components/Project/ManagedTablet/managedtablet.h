#ifndef MANAGEDTABLET_H
#define MANAGEDTABLET_H

#include <QString>
#include <QVector>
#include "../Generic/generic.h"

class ManagedTablet_class : public Generic_class
{
public:
    QString Room;
public:
    ManagedTablet_class();
};

class ManagedTablets_class : public Generic_class
{
public: /* Typedef and enums */
public: /* Members */
    QVector<ManagedTablet_class *> m_vec_ManagedTablets;
public: /* Functions */
    ManagedTablets_class() {}
};

#endif // MANAGEDTABLET_H
