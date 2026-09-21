#ifndef PROJECT_CLASS_H
#define PROJECT_CLASS_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QUndoStack>

#include "Page/page.h"

class Project_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name     READ getName     NOTIFY metaChanged)
    Q_PROPERTY(QString path     READ getPath     NOTIFY metaChanged)
    //Q_PROPERTY(bool    modified READ isModified  NOTIFY modifiedChanged)
    //Q_PROPERTY(bool    canUndo  READ canUndo  NOTIFY undoStateChanged)
    //Q_PROPERTY(bool    canRedo  READ canRedo  NOTIFY undoStateChanged)
    //Q_PROPERTY(QString undoText READ undoText NOTIFY undoStateChanged)
    //Q_PROPERTY(QString redoText READ redoText NOTIFY undoStateChanged)

private: /* Members */
    QString    m_str_Name;
    QString    m_str_ProjectPath;
    QUndoStack m_UndoStack;

    QVector<Page_class *> m_vec_Pages;

private: /* Functions */
    QString _getPath() const;

public:
    explicit Project_class(QObject *parent = nullptr);

    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

    QString getName()          const       { return m_str_Name;        };
    QString getPath()          const       { return m_str_ProjectPath; };

    bool save(QString *error);
    bool load(const QString &projectPath, QString *error);


signals:
    void metaChanged();
};

#endif // PROJECT_CLASS_H
