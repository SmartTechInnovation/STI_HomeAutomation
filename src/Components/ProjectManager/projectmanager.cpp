#include "projectmanager.h"

ProjectManager_class::ProjectManager_class(QObject *parent) : QAbstractListModel(parent) {

}

QVariantList ProjectManager_class::recentProjects() const {
    return {};
}

int ProjectManager_class::createProject(const QUrl &ParentFolder, const QString &name){
    return 0;
}

int ProjectManager_class::openProject(const QUrl &projectPath){
    return 0;
}

int ProjectManager_class::openPath(const QString &projectPath){
    return 0;
}


