#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QPointF>
#include <QVector>
#include <QUuid>
#include <QVariantMap>

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
        PortType_Enum   = 5,
        PortType_Date   = 6,
        PortType_Custom,
    };

    struct PortVariant_s{
        QString Data;
        QString Text;
    };

private: /* Members */
    QUuid   m_Uuid;         // Port unique Id
    QString m_str_Name;     // Port Short    Name
    QString m_str_ExtName;  // Port Extended Name
    QColor  m_color;        // Port Color
    bool    m_b_Visible;    // Port Visible
    bool    m_b_Connected;  // Port Connected
    int     m_int_PortType; // Port Type
    QPointF m_Position;     // Port Position
    QString m_str_Docs;     // Port Documentation
    QString m_str_DefaultValue; // PortDefaultValue
    QString m_str_Unit;     // Port Unit

    bool m_bool_CustomColor = false;

    void *m_ptr_LiveDataRef = nullptr;

    QVector<Port_class *> m_vec_ToPorts;
    QVector<Port_class *> m_vec_FromPorts;

    QVector<PortVariant_s> m_vec_Variant;
    QVector<QUuid>         m_vec_Sources; //Temporary store source of signal;

    QObject    *m_ptr_Parent        = nullptr;

public:
    explicit Port_class(QObject *parent = nullptr);

    void operator=(const Port_class &port);

    bool  connectToPort(Port_class *toPort);

    static QString    toString  (PortType_e PortType);
    static PortType_e fromString(QString    PortType);
    static QColor     getColor  (PortType_e PortType);
    static bool       compatible(PortType_e out, PortType_e in);

    void setUuid(const QUuid &uuid)                                { m_Uuid         = uuid;                              };
    void setName(const QString &ShortName, const QString &ExtName) { m_str_Name     = ShortName; m_str_ExtName = ExtName;};
    void setColor(const QColor &color)                             { m_color        = color;   m_bool_CustomColor = true;};
    void setVisible(bool value)                                    { m_b_Visible    = value;                             };
    void setPortType(int type)                                     { m_int_PortType = type;                              };
    void setDocumentation(const QString &Docs)                     { m_str_Docs     = Docs;                              };
    void setDefaultValue(const QString &Value)                     { m_str_DefaultValue = Value;                         };
    void setUnit(const QString &Unit)                              { m_str_Unit     = Unit;                              };
    void setLiveValue(void *PtrToValue)                            { m_ptr_LiveDataRef = PtrToValue;                     };
    void addVariant(const PortVariant_s &Variant)                  { m_vec_Variant.push_back(Variant);                   };

    /* ==== Sources (inputs only) ==== */
    bool addSource   (const QUuid &Source);
    bool removeSource(const QUuid &Source);
    void setSources  (const QVector<QUuid> &Sources)               { m_vec_Sources = Sources;                            };

    QVariantMap   getQVariantMap();

    inline QUuid   getUuid()         const { return m_Uuid;            };
    inline QString getName()         const { return m_str_Name;        };
    inline QString getExtName()      const { return m_str_ExtName;     };
    inline QColor  getColor()        const { return m_bool_CustomColor ? m_color : getColor((PortType_e)m_int_PortType); };
    inline bool    getVisible()      const { return m_b_Visible;       };
    inline bool    getConnected()    const { return m_b_Connected;     };
    inline int     getPortType()     const { return m_int_PortType;    };
    inline void    *getLiveData()    const { return m_ptr_LiveDataRef; };
    inline QString getDocs()         const { return m_str_Docs;        };
    inline QString getDefaultValue() const { return m_str_DefaultValue;};
    inline QString getUnit()         const { return m_str_Unit;        };

    inline const QVector<PortVariant_s> &getVariants() const { return m_vec_Variant; };
    inline const QVector<QUuid        > &getSources()  const { return m_vec_Sources; };
};

#endif // PORT_H
