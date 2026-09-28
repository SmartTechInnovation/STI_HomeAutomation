#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>
#include <QColor>

class Category_class
{
public: /* Typedef and enums */

public: /* Members */
    QString Title;
    QString Icon;
    QColor  Color;
    size_t  Order;
    bool    Favorite;
    QString Docs;
public:
    Category_class();
};

#endif // CATEGORY_H
