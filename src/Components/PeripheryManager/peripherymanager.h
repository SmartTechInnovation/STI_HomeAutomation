#ifndef PERIPHERYMANAGER_H
#define PERIPHERYMANAGER_H

#include <QObject>
#include <QMap>
#include <QUuid>

#include "xmlnode.h"

class PeripheryManager_class : public QObject
{
    Q_OBJECT
private: /* Typedef and enums */
public:  /* Typedef and enums */

private: /* Members */
    QMap<QUuid, XmlNode_class *> m_map_Sections;
    QMap<QUuid, XmlNode_class *> m_map_NodeTypes;
    QMap<QUuid, XmlNode_class *> m_map_NodeTemplates;
    QMap<QUuid, XmlNode_class *> m_map_DeviceTemplates;
public:  /* Members */

private: /* Functions */
    bool load(const QString &Path);
    void parceNodeTypeXml(XmlNode_class *node);
public:  /* Functions */
    explicit PeripheryManager_class(QObject *parent = nullptr);
    ~PeripheryManager_class();

    void begin();
    bool isTagSection(const QString &tagName);
    XmlNode_class *getNode(const QString &tagName);
    XmlNode_class *getNode(const QUuid &uuid);

    /* ===== QML ===== */
    Q_INVOKABLE QVariantList getSections();
    Q_INVOKABLE QVariantMap  getSubMenu(const QUuid &uuid);
};

extern PeripheryManager_class S_PeripheryManager;

#endif // PERIPHERYMANAGER_H