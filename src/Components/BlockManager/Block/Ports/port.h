#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QPointF>
#include <QVector>

class BlockBase_class;

class Port_class : public QObject
{
    Q_OBJECT
public:  /* Typedefs and enums */
    enum PortType_e{
        PortType_Void   = 0,
        PortType_Bool   = 1,
        PortType_Int    = 2,
        PortType_Float  = 3,
        PortType_String = 4,
        PortType_Custom,
    };

private: /* Members */
    QString m_str_Name;     // Port Short    Name
    QString m_str_ExtName;  // Port Extended Name
    QColor  m_color;        // Port Color
    bool    m_b_Visible;    // Port Visible
    bool    m_b_Connected;  // Port Connected
    int     m_int_PortType; // Port Type
    QPointF m_Position;     // Port Position

    bool m_bool_CustomColor = false;

    void *m_ptr_LiveDataRef = nullptr;

    QVector<Port_class *> m_vec_ToPorts;
    QVector<Port_class *> m_vec_FromPorts;

    QObject    *m_ptr_Parent        = nullptr;

public:
    explicit Port_class(QObject *parent = nullptr);

    void operator=(const Port_class &port);

    bool  connectToPort(Port_class *toPort);

    static QString    toString  (PortType_e PortType);
    static PortType_e fromString(QString    PortType);
    static QColor     getColor  (PortType_e PortType);
    static bool       compatible(PortType_e out, PortType_e in);

    void setName(const QString &ShortName, const QString &ExtName) { m_str_Name     = ShortName; m_str_ExtName = ExtName;};
    void setColor(const QColor &color)                             { m_color        = color;   m_bool_CustomColor = true;};
    void setVisible(bool value)                                    { m_b_Visible    = value;                             };
    void setPortType(int type)                                     { m_int_PortType = type;                              };
    void setLiveValue(void *PtrToValue)                            { m_ptr_LiveDataRef = PtrToValue;                     };

    inline QString getName()      const { return m_str_Name;        };
    inline QString getExtName()   const { return m_str_ExtName;     };
    inline QColor  getColor()     const { return m_bool_CustomColor ? m_color : getColor((PortType_e)m_int_PortType); };
    inline bool    getVisible()   const { return m_b_Visible;       };
    inline bool    getConnected() const { return m_b_Connected;     };
    inline int     getPortType()  const { return m_int_PortType;    };
    inline void    *getLiveData() const { return m_ptr_LiveDataRef; };
};

#endif // PORT_H
