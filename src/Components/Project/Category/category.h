#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>
#include <QColor>
#include <QVector>

#include "../Generic/generic.h"

class Category_class : public Generic_class
{
public: /* Typedef and enums */

public: /* Members */
    QString Icon;
    QColor  Color;
    size_t  Order;
    bool    Favorite;
    QString Docs;
public:
    Category_class();
};

class Categories_class : public Generic_class
{
public: /* Typedef and enums */
public: /* Members */
    QVector<Category_class *> m_vec_Categories;
public: /* Funcitons */
    Categories_class() { }
};

#endif // CATEGORY_H
