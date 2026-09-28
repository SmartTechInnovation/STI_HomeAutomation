#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QUrl>
#include <QVariantList>
#include <QVector>

#include "../Project/project.h"

class ProjectManager_class : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList recentProjects  READ recentProjects NOTIFY recentProjectsChanged)
    Q_PROPERTY(QVariantList openedTabs      READ openedTabs     NOTIFY openedTabsChanged)
    Q_PROPERTY(int          activeTabIndex  READ activeTabIndex NOTIFY activeTabIndexChanged)

private: /* Typedef and enums */
    struct RecentProject_s{
        QString Title;
        QString Location;
        QString Workspace;
        QString CreationDate;
        QString ModifiedDate;
        QString Path;
    };

private: /* Members */
    QVector<Project_class *>   m_vec_OpenedProjects;
    QVector<RecentProject_s *> m_vec_RecentProjects;
    Project_class             *m_ptr_ActiveProject = nullptr;
    private:
        void _loadRecent();
        void _pushRecent(Project_class *ProjectRef);

    public:
        explicit ProjectManager_class(QObject *parent = nullptr);

        void begin();


        QVariantList    recentProjects() const;
        QVariantList    openedTabs()     const;
        int             activeTabIndex() const;

        Q_INVOKABLE int newProject();
        Q_INVOKABLE int openProject(const QUrl &projectPath);
        Q_INVOKABLE int saveProject();
        Q_INVOKABLE int undoProject();
        Q_INVOKABLE int redoProject();
        Q_INVOKABLE int openPath(const QUrl &projectPath);
        Q_INVOKABLE int closeProject(const int &Index);
        Q_INVOKABLE int setActiveTab(const int &Index);

    signals:

        void recentProjectsChanged();
        void openedTabsChanged();
        void activeTabIndexChanged();

};

extern ProjectManager_class S_ProjectManager;

#endif // PROJECTMANAGER_H
