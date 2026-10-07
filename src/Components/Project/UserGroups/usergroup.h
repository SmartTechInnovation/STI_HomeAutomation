#ifndef USERGROUP_H
#define USERGROUP_H

#include <QString>
#include <QUuid>
#include <QVector>

#include "../Generic/generic.h"

class UserGroup_class : public Generic_class
{
public:
    uint64_t Permissions;
public:
    UserGroup_class();
};

class UserGroups_class : public Generic_class
{
public: /* Members   */
    QVector<UserGroup_class *> m_vec_UserGroups;
public: /* Functions */
    UserGroups_class() {}
};

#endif // USERGROUP_H
