#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QAbstractListModel>
#include <QUrl>
#include <QVariantList>
#include <QVector>

#include "../Project/project.h"

class ProjectManager_class : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QVariantList recentProjects READ recentProjects NOTIFY recentProjectsChanged)

private: /* Members */
    QVector<Project_class *> m_vec_OpenedProjects;
    Project_class           *m_ptr_ActiveProject = nullptr;
    private:
        void pushRecent(const QString &projectPath);

    public:
        explicit ProjectManager_class(QObject *parent = nullptr);

        QVariantList    recentProjects() const;

        Q_INVOKABLE int createProject(const QUrl &ParentFolder, const QString &name);
        Q_INVOKABLE int openProject(const QUrl &projectPath);
        Q_INVOKABLE int openPath(const QString &projectPath);

        //Q_INVOKABLE int  saveAll();
        //Q_INVOKABLE int  closeProject(int index);
        //Q_INVOKABLE QObject *getProjectAt(int index) const;
        //Q_INVOKABLE void forgetProject(const QString &projectPath);

    signals:
        void recentProjectsChanged();
};

#endif // PROJECTMANAGER_H
