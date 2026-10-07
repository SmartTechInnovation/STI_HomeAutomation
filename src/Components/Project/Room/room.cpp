#include "room.h"

Room_class::Room_class() {

}

QString    Room_class::toString(RoomType_e Type){
    QString Result = "Unk";
    switch(Type){
        case Room_Common:
            Result = "Common";
            break;
        case Room_Bedroom:
            Result = "Bedroom";
            break;
        case Room_Central:
            Result = "Central";
            break;
        case Room_Outdoor:
            Result = "Outdoor";
            break;
        case Room_Other:
            Result = "Other";
            break;
        case Room_Max:
            break;
        default:
            break;
    }
    return Result;
}

Room_class::RoomType_e Room_class::fromString(const QString &Type){
    if(Type == "Common")  return RoomType_e::Room_Common;
    if(Type == "Bedroom") return RoomType_e::Room_Bedroom;
    if(Type == "Central") return RoomType_e::Room_Central;
    if(Type == "Outdoor") return RoomType_e::Room_Outdoor;
    if(Type == "Other")   return RoomType_e::Room_Other;
    return RoomType_e::Room_Max;
}

Rooms_class::Rooms_class(){

}