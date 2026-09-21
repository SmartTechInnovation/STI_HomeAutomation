#include "project.h"

Project_class::Project_class(QObject *parent) : QObject(parent) {

}

void Project_class::undo(){
    m_UndoStack.undo();
}

void Project_class::redo(){
    m_UndoStack.redo();
}

bool Project_class::save(QString *error){
    return true;
}

bool Project_class::load(const QString &projectPath, QString *error){
    return true;
}