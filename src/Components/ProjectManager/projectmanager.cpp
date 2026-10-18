#include "projectmanager.h"

#include "QFile"
#include "QFileInfo"
#include "QDir"
#include "QDirIterator"
#include "QXmlStreamReader"
#include "QXmlStreamWriter"
#include <QStandardPaths>
#include <QQmlEngine>

#include "../../Setup/Setup.hpp"
#include "../Logger/logger.h"

#include "../Project/project_tokens.h"

#define PROJECT_MANAGER_NAME "Project Manager"

ProjectManager_class S_ProjectManager;

ProjectManager_class::ProjectManager_class(QObject *parent) : QObject(parent) {

}

void ProjectManager_class::begin(){
    loadRecent();
}

void ProjectManager_class::loadRecent(){
    const QString configDirPath = CACHE_APP_PATH + CONFIG_DIR_NAME;
    QDir().mkpath(configDirPath);

    const QString recentCacheFilePath = configDirPath + "/" + RECENT_FILE_NAME;
    QFile recentProjectsFile(recentCacheFilePath);

    if(!recentProjectsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        S_Logger.write(Log::Warning, PROJECT_MANAGER_NAME, "Cannot open recent projects.", "Error on loading cache", "Trying to create new file", true);
        return;
    }
    QXmlStreamReader fileXml(&recentProjectsFile);
    while(!fileXml.atEnd() && ! fileXml.hasError()){
        QXmlStreamReader::TokenType token     = fileXml.readNext();
        QStringView                 tokenName = fileXml.name();
        if(token == QXmlStreamReader::StartElement){
            if(tokenName == TOKEN_PROJECT){
                RecentProject_s *recentProject = new RecentProject_s;
                recentProject->Title        = fileXml.attributes().value(ATTR_TITLE).toString();
                recentProject->Location     = fileXml.attributes().value(ATTR_ADDRESS).toString();
                recentProject->Workspace    = fileXml.attributes().value(ATTR_WORKSPACE).toString();
                recentProject->CreationDate = fileXml.attributes().value(ATTR_CREATION_DATE).toString();
                recentProject->ModifiedDate = fileXml.attributes().value(ATTR_MODIFIED_DATE).toString();
                recentProject->Path         = fileXml.attributes().value(ATTR_PATH).toString();

                m_vec_RecentProjects.push_back(recentProject);
            }
        }else if(token == QXmlStreamReader::EndElement){

        }
    }
    if(fileXml.hasError()){
        S_Logger.write(Log::Error, PROJECT_MANAGER_NAME, "Recent Project xml error: " + fileXml.errorString() + " :" + QString::number(fileXml.lineNumber()) + ":" + QString::number(fileXml.columnNumber()), "Recent projects loading.", "", true);
        return;
    }
    emit recentProjectsChanged();
}

void ProjectManager_class::pushRecent(Project_class *ProjectRef){
    if(ProjectRef == nullptr) return;

    const QString recentProjectsFilePath = CACHE_APP_PATH + CONFIG_DIR_NAME + "/" + RECENT_FILE_NAME;

    QFile recentProjectsFile(recentProjectsFilePath);

    bool finded = false;

    QVariantMap infoMap = ProjectRef->info();

    for(int i = 0; i < m_vec_RecentProjects.size(); i++){
        if(m_vec_RecentProjects[i]->Title == ProjectRef->getTitle() && m_vec_RecentProjects[i]->Path == ProjectRef->getDir()){
            finded = true;
            RecentProject_s *projectRef = m_vec_RecentProjects[i];
            m_vec_RecentProjects.erase(m_vec_RecentProjects.begin()+i);
            m_vec_RecentProjects.push_front(projectRef);
            break;
        }
    }
    if(!finded){
        RecentProject_s *newRecentProject = new RecentProject_s;
        newRecentProject->Title        = infoMap["title"].toString();
        newRecentProject->Location     = infoMap["street"].toString() + ", " + infoMap["town"].toString();
        newRecentProject->Workspace    = "Local";
        newRecentProject->CreationDate = infoMap["cDate"].toString();
        newRecentProject->ModifiedDate = infoMap["mDate"].toString();
        newRecentProject->Path         = ProjectRef->getDir();
        m_vec_RecentProjects.push_front(newRecentProject);
        emit recentProjectsChanged();
    }

    if(!recentProjectsFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        S_Logger.write(Log::Warning, PROJECT_MANAGER_NAME, "Cannot write recent projects.", "Error on writing cache", "Check permissions", true);
        return;
    }
    QXmlStreamWriter fileXml(&recentProjectsFile);

    fileXml.setAutoFormatting(true);
    fileXml.writeStartDocument();

    fileXml.writeStartElement("RecentProjects");

    for(auto &p : m_vec_RecentProjects){
        fileXml.writeStartElement(TOKEN_PROJECT);
        fileXml.writeAttribute(ATTR_TITLE,         p->Title);
        fileXml.writeAttribute(ATTR_ADDRESS,       p->Location);
        fileXml.writeAttribute(ATTR_WORKSPACE,     p->Workspace);
        fileXml.writeAttribute(ATTR_CREATION_DATE, p->CreationDate);
        fileXml.writeAttribute(ATTR_MODIFIED_DATE, p->ModifiedDate);
        fileXml.writeAttribute(ATTR_PATH,          p->Path);
        fileXml.writeEndElement();
    }
    fileXml.writeEndElement();
    fileXml.writeEndDocument();

    recentProjectsFile.close();
}

void ProjectManager_class::trackProject(Project_class *ProjectRef){
    QQmlEngine::setObjectOwnership(ProjectRef, QQmlEngine::CppOwnership);
    connect(ProjectRef, &Project_class::modifiedChanged, this, &ProjectManager_class::openedTabsChanged);
    connect(ProjectRef, &Project_class::titleChanged,    this, &ProjectManager_class::openedTabsChanged);
}

QVariantList ProjectManager_class::recentProjects() const {
    QVariantList Result;
    for(auto &p : m_vec_RecentProjects){
        QVariantMap projectMap;
        projectMap["title"]     = p->Title;
        projectMap["location"]  = p->Location;
        projectMap["workspace"] = p->Workspace;
        projectMap["cDate"]     = p->CreationDate;
        projectMap["mDate"]     = p->ModifiedDate;
        projectMap["path"]      = p->Path;
        Result.push_back(projectMap);
    }
    return Result;
}

QVariantList ProjectManager_class::openedTabs() const {
    QVariantList Result;
    for(const Project_class *project : m_vec_OpenedProjects){
        QVariantMap projectMap;
        projectMap["path"]  = project->getDir();
        projectMap["title"] = project->getTitle();
        projectMap["dirty"] = project->isModified();
        Result.push_back(projectMap);
    }
    return Result;
}

int ProjectManager_class::activeTabIndex() const{
    int Result = -1;
    for(int i = 0; i < m_vec_OpenedProjects.length(); i++){
        if(m_vec_OpenedProjects[i] == m_ptr_ActiveProject){
            Result = i;
            break;
        }
    }
    return Result;
}

QObject *ProjectManager_class::activeProject() const{
    return m_ptr_ActiveProject;
}

int ProjectManager_class::newProject(){
    Project_class *newProject = new Project_class();

    newProject->newDocument(tr("Unnamed"));
    trackProject(newProject);

    m_vec_OpenedProjects.push_back(newProject);
    m_ptr_ActiveProject = newProject;
    emit openedTabsChanged();
    emit activeTabIndexChanged();

    return 0;
}

int ProjectManager_class::openProject(const QUrl &projectPath){
    QString Error;

    QString projectName;
    QString projectXmlPath;

    QDirIterator it(projectPath.path(),
                    QStringList{ "Project.xml" },
                    QDir::Files,
                    QDirIterator::Subdirectories);

    S_Logger.write(Log::Info, PROJECT_MANAGER_NAME, "Loading project: " + projectPath.path());

    while(it.hasNext()){
        projectXmlPath = it.next();

        QFileInfo projInfo(projectXmlPath);
        projectName = projInfo.dir().dirName();
        break;
    }

    if(projectName.isEmpty()){
        S_Logger.write(Log::Type_e::Error, PROJECT_MANAGER_NAME, "Cannot find project file.", "Error on opening project", "Check project path.", true);
        for(int i = 0; i < m_vec_RecentProjects.length(); i++){
            if(m_vec_RecentProjects[i]->Path == projectPath.path()){
                auto *Ref = m_vec_RecentProjects[i];
                m_vec_RecentProjects.erase(m_vec_RecentProjects.begin() + i);
                delete Ref;
                emit recentProjectsChanged();
            }
        }
        return -1;
    }

    bool finded = false;
    for(auto &project : m_vec_OpenedProjects){
        QString strProjectPath = projectPath.path();
        if(strProjectPath[strProjectPath.length()-1] == '/')
            strProjectPath.removeLast();
        if(project->getDir() == strProjectPath){
            finded = true;
            break;
        }
    }

    if(finded){
        S_Logger.write(Log::Info, PROJECT_MANAGER_NAME, "Warning on loading project", "Project is already opened", "", true);
        return -1;
    }

    Project_class *existingProject = new Project_class;

    if(!existingProject->load(projectXmlPath)){
        return -1;
    }

    trackProject(existingProject);

    m_vec_OpenedProjects.push_back(existingProject);
    m_ptr_ActiveProject          = existingProject;
    emit activeTabIndexChanged();
    emit openedTabsChanged();
    pushRecent(existingProject);
    return 0;
}

int ProjectManager_class::saveProject(){
    if(m_ptr_ActiveProject == nullptr)
        return 0;
    if(!m_ptr_ActiveProject->hasFile()){
        emit saveAsRequested();
        return 0;
    }
    m_ptr_ActiveProject->save();
    return 0;
}

int ProjectManager_class::saveProjectAs(const QUrl &folderPath){
    if(m_ptr_ActiveProject == nullptr)
        return -1;
    if(!m_ptr_ActiveProject->saveAs(folderPath.toLocalFile()))
        return -1;
    pushRecent(m_ptr_ActiveProject);
    emit openedTabsChanged();
    return 0;
}

int ProjectManager_class::undoProject(){
    if(m_ptr_ActiveProject != nullptr){
        m_ptr_ActiveProject->undo();
    }
    return 0;
}

int ProjectManager_class::redoProject(){
    if(m_ptr_ActiveProject != nullptr){
        m_ptr_ActiveProject->redo();
    }
    return 0;
}

int ProjectManager_class::openPath(const QUrl &projectPath){
    return 0;
}

int ProjectManager_class::closeProject(const int &Index){
    if(Index < 0 || Index >= m_vec_OpenedProjects.length()){
        return -1;
    }

    if(m_vec_OpenedProjects[Index] == m_ptr_ActiveProject){
        m_ptr_ActiveProject = nullptr;
    }

    if(activeTabIndex() == Index){
        for(int i = Index - 1; i >= 0; i--){
            if(i < m_vec_OpenedProjects.length()){
                m_ptr_ActiveProject = m_vec_OpenedProjects[i];
                break;
            }
        }
    }

    Project_class *takedProject = m_vec_OpenedProjects.takeAt(Index);

    if(takedProject != nullptr){
        delete takedProject;
        takedProject = nullptr;
    }

    emit activeTabIndexChanged();
    emit openedTabsChanged();

    return 0;
}

int ProjectManager_class::setActiveTab(const int &Index){
    if(Index == -1){
        m_ptr_ActiveProject = nullptr;
        emit activeTabIndexChanged();
        return 0;
    }
    if(Index < 0 || Index >= m_vec_OpenedProjects.length()){
        return -1;
    }
    m_ptr_ActiveProject      = m_vec_OpenedProjects[Index];
    emit activeTabIndexChanged();
    return 0;
}

int ProjectManager_class::moveTab(int From, int To){
    if(From < 0 || From > m_vec_OpenedProjects.length()){
        return -1;
    }
    if(To < 0) To = 0;
    if(To >= m_vec_OpenedProjects.length()) To = m_vec_OpenedProjects.length() - 1;
    m_vec_OpenedProjects.move(From, To);
    emit openedTabsChanged();
    emit activeTabIndexChanged();
    return 0;
}

