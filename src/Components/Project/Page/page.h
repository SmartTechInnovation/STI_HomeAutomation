#ifndef PAGE_H
#define PAGE_H

#include <QString>
#include <QVector>
#include <QVariantList>
#include <QAbstractListModel>

#include "../../Document/xmlnode.h"

class Project_class;

class Page_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString      iUuid        READ iUuid         CONSTANT)
    Q_PROPERTY(QString      title        READ title         NOTIFY titleChanged)
    Q_PROPERTY(QString      modifiedDate READ modifiedDate  NOTIFY modifiedDateChanged)
    Q_PROPERTY(QVariantList blocks       READ getBlocks     NOTIFY blocksChanged)
    Q_PROPERTY(QVariantList connections  READ connections   NOTIFY connectionsChanged)

    /* View state - kept while the app runs, not saved */
    Q_PROPERTY(qreal viewX     MEMBER m_real_ViewX     NOTIFY viewChanged)
    Q_PROPERTY(qreal viewY     MEMBER m_real_ViewY     NOTIFY viewChanged)
    Q_PROPERTY(qreal viewScale MEMBER m_real_ViewScale NOTIFY viewChanged)
    Q_PROPERTY(bool  viewValid MEMBER m_b_ViewValid    NOTIFY viewChanged)

public: /* Typedef and enums */

private: /* Members */
    Project_class         *m_ptr_Project = nullptr;
    XmlNode_class         *m_ptr_PageRef = nullptr;

    QVector<XmlNode_class *> m_vec_Blocks;
    QVector<XmlNode_class *> m_vec_Signals_In;
    QVector<XmlNode_class *> m_vec_Signals_Out;
    QVector<XmlNode_class *> m_vec_Unrecognised;

    qreal m_real_ViewX     = 0;
    qreal m_real_ViewY     = 0;
    qreal m_real_ViewScale = 1;
    bool  m_b_ViewValid    = false;

private: /* Functions */
    void onInserted        (XmlNode_class *parent, XmlNode_class *node);
    void onRemoved         (XmlNode_class *parent, XmlNode_class *node);
    void onAttributeChanged(XmlNode_class *node,   const QString &tag);

public:
    explicit Page_class(Project_class *project, XmlNode_class *pageNode);
    ~Page_class();

    XmlNode_class *pageRef()      const;
    QString        iUuid()        const;
    QString        title()        const;
    QString        modifiedDate() const;

    /* Qml API */
    QVariantList   getBlocks()     const;
    QVariantList   connections()   const;

    Q_INVOKABLE QVariantMap getBlockGhost(const QString &uuid) const;

signals:
    void titleChanged();
    void modifiedDateChanged();
    void blocksChanged();
    void blockGhostChanged();
    void connectionsChanged();
    void viewChanged();
};

#endif // PAGE_H
