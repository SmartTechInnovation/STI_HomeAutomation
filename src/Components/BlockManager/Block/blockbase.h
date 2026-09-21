#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include <QUuid>
#include <QString>
#include <QColor>
#include <QVector>
#include <QPointF>

#include "Ports/port.h"

class BlockBase_class
{
protected: /* Members */
    QUuid   m_uuid_BlockId;                 // UUID
    QString m_str_Name         = "Unnamed"; // Editable Name
    QString m_str_Type         = "Unnamed"; // Fixed Type Name
    QString m_str_Description  = "";
    QColor  m_color            = 0x7D8491;  // Block Color
    qreal   m_real_Width       = 400;       // Block Width
    bool    m_b_Selected       = false;     // Selected
    QPointF m_Point_Position   = QPointF(0,0);
    QString m_str_IconPath     = "";

    QString m_str_Room         = "Not Assigned"; // Block Room
    QString m_str_Category     = "Not Assigned"; // Block Category

    QVector<Port_class *> m_vec_Inputs;
    QVector<Port_class *> m_vec_Outputs;

public:
    virtual void setType(const QString &Type);
    virtual void setWidth(float width);
    virtual void addInput(Port_class  &newInputPort);
    virtual void addOutput(Port_class &newOutputPort);

public:
    BlockBase_class();
    BlockBase_class(QUuid uuid);

    virtual void setName (const QString &Name);
    virtual void setColor(const QColor  &Color);
    virtual void setIcon(const QString &IconPath);
    virtual void setDescription(const QString &Description);
    virtual void select  (bool value);
    virtual void setRoom (const QString  &Room);
    virtual void setCategory(const QString &Category);

    inline  QUuid   &getId()       { return m_uuid_BlockId; };
    inline  QString &getName()     { return m_str_Name;     };
    inline  QString &getType()     { return m_str_Type;     };
    inline  QColor  &getColor()    { return m_color;        };
    inline  qreal   &getWidth()    { return m_real_Width;   };
    inline  QString &getIconPath() { return m_str_IconPath; };
    inline  QString &getDescription() {return m_str_Description;   };
    inline  bool    &getSelected() { return m_b_Selected;   };
    inline  QString &getRoom()     { return m_str_Room;     };
    inline  QString &getCategory() { return m_str_Category; };
    inline  qreal   &getX()        { return m_Point_Position.rx(); };
    inline  qreal   &getY()        { return m_Point_Position.ry(); };
    inline  QPointF &getPosition() { return m_Point_Position;      };
    inline  QVector<Port_class *> &getInputs()  { return m_vec_Inputs;  };
    inline  QVector<Port_class *> &getOutputs() { return m_vec_Outputs; };
};

#endif // BLOCKBASE_H
