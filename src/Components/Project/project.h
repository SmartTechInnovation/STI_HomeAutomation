#ifndef PROJECT_CLASS_H
#define PROJECT_CLASS_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>
#include <QSize>
#include <QUuid>
#include <QUndoStack>
#include <QVariantMap>
#include <QVariantList>

#include "../Document/xmlnode.h"
#include "../SchemaManager/schemamanager.h"

#include "Page/page.h"
#include "Tree/projecttree.h"

#define WORKSPACE_SCALE 10

/*
 * Project = one document (tree of XmlNode_class read from Project.xml).
 *
 *  - every entity has an IUuid (index m_hash_Index)
 *  - every change goes through 3 undo commands: InsertNode, RemoveNode, SetAttribute
 *    (see project_commands.h); the views (tree, pages, properties) listen to the
 *    signals below and update themselves
 *  - Save writes the tree back 1:1 (unknown tags / attributes are kept)
 */

class Project_class : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool         canUndo       READ canUndo       NOTIFY undoStateChanged)
    Q_PROPERTY(bool         canRedo       READ canRedo       NOTIFY undoStateChanged)
    Q_PROPERTY(QString      undoText      READ undoText      NOTIFY undoStateChanged)
    Q_PROPERTY(QString      redoText      READ redoText      NOTIFY undoStateChanged)
    Q_PROPERTY(QVariantMap  info          READ info          NOTIFY infoChanged     )
    Q_PROPERTY(QVariantList pageList      READ pageList      NOTIFY pagesChanged    )
    Q_PROPERTY(int          pageCount     READ pageCount     NOTIFY pagesChanged    )
    Q_PROPERTY(int          pageActv      READ pageActv      NOTIFY pageActvChanged WRITE setPageActv)
    Q_PROPERTY(QObject      *tree         READ tree          NOTIFY treeChanged     )
    Q_PROPERTY(QString      selectedUuid  READ selectedUuid  NOTIFY selectedUuidChanged)
    Q_PROPERTY(QString      selectedIUuid READ selectedIUuid NOTIFY selectedIUuidChanged WRITE setSelectedIUuid)
    Q_PROPERTY(QVariantMap  selection     READ selection     NOTIFY selectionDataChanged)
    Q_PROPERTY(QVariantMap  context       READ context       NOTIFY contextChanged  )


private: /* Typedefs and enums */
    typedef SchemaManager_class::NodeType_s     NodeType_s;
    typedef SchemaManager_class::Property_s     Property_s;

private: /* Members */
    QString                        m_str_ProjectPath;
    QString                        m_str_RootPath;

    XmlNode_class                 *m_ptr_Root       = nullptr;
    XmlNode_class                 *m_ptr_ActivePage = nullptr;
    XmlNode_class                 *m_ptr_Program    = nullptr;
    XmlNode_class                 *m_ptr_Selected   = nullptr;
    ProjectTree_class             *m_ptr_Tree       = nullptr;
    QHash<QUuid, XmlNode_class *>  m_hash_NodeByIUuid;
    QHash<QUuid, Page_class    *>  m_hash_PageByIUuid;
    QUndoStack                     m_UndoStack;


private: /* Functions */
    friend class InsertNodeCmd;
    friend class RemoveNodeCmd;
    friend class MoveNodeCmd;
    friend class SetAttributeCmd;
    /* Document primitives - only called by the undo commands */
    void insertNode  (XmlNode_class *parent, XmlNode_class *node, int row);
    void takeNode    (XmlNode_class *node,   bool moving = false);
    void setAttribute(XmlNode_class *node,   const QString &tag, const QString &value);
    void rmAttribute (XmlNode_class *node,   const QString &tag);

    void indexNode  (XmlNode_class *node);
    void unIndexNode(XmlNode_class *node);
    void touchPage  (XmlNode_class *node);

    QString uniqueTitle(XmlNode_class *parent, const QString &base);
    void    setDocument(XmlNode_class *root);

    XmlNode_class *createNode(XmlNode_class *parent, const NodeType_s *nodeTemplate);

    QVector<XmlNode_class *> pages() const;

public:
    explicit Project_class(QObject *parent = nullptr);
    ~Project_class();

    /* ===== File ===== */
    bool newDocument(const QString &title);
    bool load(const QString &projectPath);
    bool save();
    bool saveAs(const QString &folderPath);

    bool         isModified()       const;
    bool         hasFile()          const;
    bool         canUndo()          const;
    bool         canRedo()          const;
    QString      undoText()         const;
    QString      redoText()         const;
    int          pageCount()        const;
    int          pageActv()         const;
    QVariantList pageList();
    QVariantMap  info()             const;

    QObject     *tree()             const;
    QString      selectedUuid()     const;
    QString      selectedIUuid()    const;
    QVariantMap  selection()        const;
    QVariantMap  context()          const;


    void         setPageActv(int index);
    void         setSelectedIUuid(const QString &uuid);

    /* ===== QML API - every change goes through the undo stack ===== */
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

    /* Pages */
    Q_INVOKABLE int  cmdAddPage   (int index, const QString &title);
    Q_INVOKABLE void cmdRemovePage(int index);
    Q_INVOKABLE void cmdMovePage  (int from, int to);
    Q_INVOKABLE void cmdRenamePage(int index, const QString &title);

    /* Tree */


signals:
    /* ==== QML ==== */
    void titleChanged();
    void pathChanged();
    void modifiedChanged();
    void undoStateChanged();
    void infoChanged();
    void pagesChanged();
    void pageActvChanged();
    void treeChanged();
    void selectedUuidChanged();
    void selectedIUuidChanged();
    void selectionDataChanged();
    void contextChanged();
    void saveAsRequested();

    /* ==== C++ Views ==== */
    void documentReset();
    void nodeInserted(XmlNode_class *node);
    void nodeRemoved (XmlNode_class *parent, XmlNode_class *node);
    void attributeChanged(XmlNode_class *node, const QString &tag);
};

#endif // PROJECT_CLASS_H
