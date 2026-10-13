#ifndef PAGE_H
#define PAGE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QUndoStack>
#include <QVariantList>

#include "../../Document/xmlnode.h"

class Project_class;

class Page_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString      iUuid        READ iUuid        CONSTANT)
    Q_PROPERTY(QString      title        READ title        NOTIFY titleChanged)
    Q_PROPERTY(QString      modifiedDate READ modifiedDate NOTIFY modifiedDateChanged)
    Q_PROPERTY(QVariantList connections  READ connections  NOTIFY connectionsChanged)

    /* View state - kept while the app runs, not saved */
    Q_PROPERTY(qreal viewX     MEMBER m_real_ViewX     NOTIFY viewChanged)
    Q_PROPERTY(qreal viewY     MEMBER m_real_ViewY     NOTIFY viewChanged)
    Q_PROPERTY(qreal viewScale MEMBER m_real_ViewScale NOTIFY viewChanged)
    Q_PROPERTY(bool  viewValid MEMBER m_b_ViewValid    NOTIFY viewChanged)

public: /* Typedef and enums */
    enum Roles_e {
        RoleIUuid = Qt::UserRole + 1,
        RoleUuid,
        RoleKind,       // block, signalIn, signalOut
        RoleTitle,
        RoleTypeName,
        RoleColor,
        RoleIcon,
        RolePosX,
        RolePosY,
        RoleWidth,
        RoleInputs,     // inputs + parameters  [{tag, name, extName, type, typeId, color, visible, connected, docs, unit}]
        RoleOutputs,    // [{iUuid, name, extName, type, typeId, color, visible, connected, docs, unit}]
        RoleMissing,    // block descriptor / signal target not found
    };
private: /* Members */
    Project_class         *m_ptr_Project = nullptr;
    XmlNode_class         *m_ptr_NodeRef = nullptr;

    qreal m_real_ViewX     = 0;
    qreal m_real_ViewY     = 0;
    qreal m_real_ViewScale = 0.2;
    bool  m_b_ViewValid    = false;

public:
    explicit Page_class(Project_class *project, XmlNode_class *pageNode);
    ~Page_class();

    XmlNode_class *nodeRef()      const;
    QString        iUuid()        const;
    QString        title()        const;
    QString        modifiedDate() const;
    QVariantList   connections()  const;

signals:
    void titleChanged();
    void modifiedDateChanged();
    void connectionsChanged();
    void viewChanged();
};

#endif // PAGE_H
