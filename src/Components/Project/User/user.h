#ifndef USER_H
#define USER_H

#include <QString>
#include <QUuid>
#include <QVector>

#include "../Generic/generic.h"

class User_class : public Generic_class
{
public: /* Typedef and enums */

public: /* Members */
    QString Username;
    QString PassHash;
    bool    Admin;
public:
    User_class();

    void setUsername(const QString &username);
    void setPassword(const QString &password);
};

class Users_class : public Generic_class
{
public: /* Typedef and enums */

public: /* Members */
    QVector<User_class *> m_vec_Users;
public: /* Functions */
    Users_class() {}
};

#endif // USER_H
