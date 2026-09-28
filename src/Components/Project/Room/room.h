#ifndef ROOM_H
#define ROOM_H

#include <QString>
#include <QColor>

class Room_class
{
public: /*Typedefs and enums */
    enum RoomType_e{
        Room_Common,
        Room_Bedroom,
        Room_Central,
        Room_Outdoor,
        Room_Other,
        Room_Max,
    };
public: /* Members */
    QString    Title;
    QString    Icon;
    size_t     Area;
    QColor     Color;
    size_t     Order;
    bool       Favorite;
    RoomType_e Type;
    QString    Docs;
public:
    Room_class();

    static QString    toString(RoomType_e Type);
    static RoomType_e fromString(const QString &Type);
};

#endif // ROOM_H
