#ifndef PROJECT_COMMANDS_H
#define PROJECT_COMMANDS_H

#include <QUndoCommand>

#include "project.h"

/*
 * The only 3 operations that change a project document.
 * Bigger edits (add block, connect, delete with references, ...) are macros of these.
 *
 * Ownership: a node that is not in the document belongs to the command that
 * took it out (or created it); the command deletes it when it is destroyed.
 */

class InsertNodeCmd : public QUndoCommand
{
    Project_class *m_ptr_Project;
    XmlNode_class *m_ptr_Parent;
    XmlNode_class *m_ptr_Node;
    int            m_int_Row;
    bool           m_b_Owned = true;
public:
    InsertNodeCmd(Project_class *project, XmlNode_class *parent, int row, XmlNode_class *node, const QString &text = QString()){
        m_ptr_Project = project;
        m_ptr_Parent  = parent;
        m_ptr_Node    = node;
        m_int_Row     = row;
        setText(text);
    }
    ~InsertNodeCmd() override {
        if(m_b_Owned)
            delete m_ptr_Node;
    }

    void redo() override {
        m_ptr_Project->insertNode(m_ptr_Parent, m_ptr_Node, m_int_Row);
        m_b_Owned = false;
    }
    void undo() override {
        m_ptr_Project->takeNode(m_ptr_Node);
        m_b_Owned = true;
    }
};

class RemoveNodeCmd : public QUndoCommand
{
    Project_class *m_ptr_Project;
    XmlNode_class *m_ptr_Parent;
    XmlNode_class *m_ptr_Node;
    int            m_int_Row;
    bool           m_b_Owned = false;
public:
    RemoveNodeCmd(Project_class *project, XmlNode_class *node, const QString &text = QString()){
        m_ptr_Project   = project;
        m_ptr_Parent    = node->parent();
        m_ptr_Node      = node;
        m_int_Row       = node->row();
        setText(text);
    }
    ~RemoveNodeCmd() override {
        if(m_b_Owned)
            delete m_ptr_Node;
    }

    void redo() override {
        m_ptr_Project->takeNode(m_ptr_Node);
        m_b_Owned = true;
    }
    void undo() override {
        m_ptr_Project->insertNode(m_ptr_Parent, m_ptr_Node, m_int_Row);
        m_b_Owned = false;
    }
};

class MoveNodeCmd : public QUndoCommand
{
    Project_class *m_ptr_Project;
    XmlNode_class *m_ptr_Node;
    XmlNode_class *m_ptr_OldParent;
    XmlNode_class *m_ptr_NewParent;
    int            m_int_OldRow;
    int            m_int_NewRow;
public:
    /* newRow = row after the node was taken out of its old place */
    MoveNodeCmd(Project_class *project, XmlNode_class *node, XmlNode_class *newParent, int newRow, const QString &text = QString()){
        m_ptr_Project = project;
        m_ptr_Node = node;
        m_ptr_OldParent = node->parent();
        m_ptr_NewParent = newParent;
        m_int_OldRow = node->row();
        m_int_NewRow = newRow;
        setText(text);
    }
    void redo() override {
        m_ptr_Project->takeNode(m_ptr_Node, true);
        m_ptr_Project->insertNode(m_ptr_NewParent, m_ptr_Node, m_int_NewRow);
    }
    void undo() override {
        m_ptr_Project->takeNode(m_ptr_Node, true);
        m_ptr_Project->insertNode(m_ptr_OldParent, m_ptr_Node, m_int_OldRow);
    }
};

class SetAttributeCmd : public QUndoCommand
{
    Project_class *m_ptr_Project;
    XmlNode_class *m_ptr_Node;
    QString        m_str_Key;
    QString        m_str_Old;
    QString        m_str_New;
    bool           m_b_HadOld;
    int            m_int_MergeId;
public:
    /* mergeId >= 0: consecutive edits of the same attribute become one undo step (typing) */
    SetAttributeCmd(Project_class *project, XmlNode_class *node, const QString &key, const QString &value,
                    const QString &text = QString(), int mergeId = -1)
        : m_ptr_Project(project), m_ptr_Node(node), m_str_Key(key),
        m_str_Old(node->value(key)), m_str_New(value), m_b_HadOld(node->hasAttr(key)), m_int_MergeId(mergeId)
    {
        setText(text);
    }

    int  id() const override {
        return m_int_MergeId;
    }

    bool mergeWith(const QUndoCommand *other) override {
        auto *cmd = static_cast<const SetAttributeCmd *>(other);
        if(cmd->m_ptr_Node != m_ptr_Node || cmd->m_str_Key != m_str_Key)
            return false;
        m_str_New = cmd->m_str_New;
        return true;
    }

    void redo() override {
        m_ptr_Project->setAttribute(m_ptr_Node, m_str_Key, m_str_New);
    }

    void undo() override {
        if(!m_b_HadOld){
            m_ptr_Project->rmAttribute(m_ptr_Node, m_str_Key);
        }else{
            m_ptr_Project->setAttribute(m_ptr_Node, m_str_Key, m_str_Old);
        }
    }
};

#endif // PROJECT_COMMANDS_H
