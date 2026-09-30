#ifndef PROJECT_COMMANDS_H
#define PROJECT_COMMANDS_H

#include <QUndoCommand>
#include <QCoreApplication>

#include "project.h"


/* ============================ Pages ============================ */

class AddPageCmd : public QUndoCommand {
    Project_class *m_ptr_Project = nullptr;
    Page_class    *m_ptr_Page    = nullptr;
    int            m_int_Index   = 0;
    bool           m_bool_Owned  = true;
public:
    AddPageCmd(Project_class *projectRef, Page_class *page, int index)
        : m_ptr_Project(projectRef), m_ptr_Page(page), m_int_Index(index)
    {
        setText(QCoreApplication::translate("Undo", "Add page %1").arg(page->title()));
    }
    ~AddPageCmd() override {
        if(m_bool_Owned){
            delete m_ptr_Page;
        }
    }

    void redo() override {
        m_ptr_Project->insertPage(m_int_Index, m_ptr_Page);
        m_ptr_Project->setPageActv(m_int_Index);
        m_bool_Owned = false;
    }

    void undo() override {
        m_ptr_Project->takePage(m_int_Index);
        m_bool_Owned = true;
    }
};

class RemovePageCmd : public QUndoCommand
{
    Project_class *m_ptr_Project;
    Page_class    *m_ptr_Page;
    int            m_int_Index;
    bool           m_b_Owned = false;
public:
    RemovePageCmd(Project_class *project, int index)
        : m_ptr_Project(project), m_int_Index(index)
    {
        m_ptr_Page = m_ptr_Project->getPage(index);
        setText(QCoreApplication::translate("Undo", "Remove page %1").arg(m_ptr_Page ? m_ptr_Page->Title : QString()));
    }
    ~RemovePageCmd() override { if(m_b_Owned) delete m_ptr_Page; }

    void redo() override {

        m_ptr_Project->takePage(m_int_Index);
        m_b_Owned = true;
    }
    void undo() override {
        m_ptr_Project->insertPage(m_int_Index, m_ptr_Page);
        m_b_Owned = false;
        m_ptr_Project->setPageActv(m_int_Index);
    }
};

class MovePageCmd : public QUndoCommand{
    Project_class *m_ptr_Project;
    int            m_int_From;
    int            m_int_To;
public:
    MovePageCmd(Project_class *project, int from, int to)
        : m_ptr_Project(project), m_int_From(from), m_int_To(to)
    {
        setText(QCoreApplication::translate("Undo", "Move page"));
    }
    void redo() override {
        m_ptr_Project->movePage(m_int_From, m_int_To);
    }
    void undo() override {
        m_ptr_Project->movePage(m_int_To, m_int_From);
    }
};

class RenamePageCmd : public QUndoCommand
{
    Project_class *m_ptr_Project;
    Page_class    *m_ptr_Page;
    QString        m_str_Old;
    QString        m_str_New;
public:
    RenamePageCmd(Project_class *project, Page_class *page, const QString &newTitle)
        : m_ptr_Project(project), m_ptr_Page(page), m_str_Old(page->Title), m_str_New(newTitle)
    {
        setText(QCoreApplication::translate("Undo", "Rename page"));
    }
    void redo() override {
        m_ptr_Page->Title = m_str_New;
        emit m_ptr_Project->pagesChanged();
    }

    void undo() override {
        m_ptr_Page->Title = m_str_Old;
        emit m_ptr_Project->pagesChanged();
    }
};


#endif // PROJECT_COMMANDS_H
