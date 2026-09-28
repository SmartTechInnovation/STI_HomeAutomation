#ifndef USER_H
#define USER_H

#include <QString>

class User_class
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

#endif // USER_H
