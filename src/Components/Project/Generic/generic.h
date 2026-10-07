#ifndef GENERIC_H
#define GENERIC_H

#include <QString>
#include <QUuid>
#include <QVector>

class Generic_class
{
public: /* Typedef and enums */
    struct SubMenu_s{
        QUuid   Uuid;
        QString Title;
        QString IconPath;
        QString Command;
        bool    Visible;
    };

    struct ContextMenu_s{
        QString Title;
        SubMenu_s MainMenu;
        QVector<SubMenu_s> Shortcuts;
    };
public: /* Members */
    QUuid Uuid;
    QUuid IUuid;
    QString Title;

    ContextMenu_s ContextMenu;
public:
    Generic_class();
};

#endif // GENERIC_H
