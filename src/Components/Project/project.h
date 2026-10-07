#ifndef PROJECT_CLASS_H
#define PROJECT_CLASS_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QColor>
#include <QUndoStack>
#include <QMap>
#include <QUuid>
#include <QVariantMap>
#include <QVariantList>

#include "Tree/projecttree.h"
#include "Page/page.h"
#include "Room/room.h"
#include "User/user.h"
#include "Rights/rights.h"
#include "UserGroups/usergroup.h"
#include "ManagedTablet/managedtablet.h"
#include "Category/category.h"
#include "Variable/variable.h"
#include "Controller/controller.h"

#define WORKSPACE_SCALE 10

class Project_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString      title         READ getTitle      NOTIFY titleChanged    )
    Q_PROPERTY(QString      path          READ getPath       NOTIFY pathChanged     )
    Q_PROPERTY(bool         modified      READ isModified    NOTIFY modifiedChanged )
    Q_PROPERTY(bool         hasFile       READ hasFile       NOTIFY pathChanged     )
    Q_PROPERTY(bool         canUndo       READ canUndo       NOTIFY undoStateChanged)
    Q_PROPERTY(bool         canRedo       READ canRedo       NOTIFY undoStateChanged)
    Q_PROPERTY(QString      undoText      READ undoText      NOTIFY undoStateChanged)
    Q_PROPERTY(QString      redoText      READ redoText      NOTIFY undoStateChanged)
    Q_PROPERTY(QString      pageFormat    READ pageFormat    NOTIFY infoChanged     )
    Q_PROPERTY(QVariantList pageList      READ pageList      NOTIFY pagesChanged    )
    Q_PROPERTY(QSize        pageSize      READ pageSize      NOTIFY infoChanged     )
    Q_PROPERTY(int          pageCount     READ pageCount     NOTIFY pagesChanged    )
    Q_PROPERTY(int          pageActv      READ pageActv      NOTIFY pageActvChanged WRITE setPageActv)
    Q_PROPERTY(QVariantMap  infoMap       READ infoMap       NOTIFY infoChanged     )
    Q_PROPERTY(QObject      *tree         READ tree          NOTIFY treeChanged     )
    Q_PROPERTY(QString      selectedUuid  READ selectedUuid  NOTIFY selectedUuidChanged  WRITE setSelectedUuid)
    Q_PROPERTY(QString      selectedIUuid READ selectedIUuid NOTIFY selectedIUuidChanged WRITE setSelectedIUuid)
    Q_PROPERTY(QVariantMap  context       READ context       NOTIFY contextChanged  )


private: /* Typedefs and enums */
    enum PaperFormat_e{
        PaperA4, /* 297 x 210 */
        PaperA3, /* 420 x 297 */
        PaperA2, /* 594 x 420 */
        PaperA1, /* 841 x 594 */
        PaperA0, /* 1189 x 841 */
        PaperMax,
    };

    struct InfoData_s{
        QString Title;
        QString CreationDate;
        QString ModifiedDate;
        QString Author;
        QString ConfigVers;
    };

    struct LocationData_s{
        QString Street;
        QString Town;
        QString PostCode;
        QString Country;
        QString Longitude;
        QString Latitude;
        QString Timezone;
    };

    struct ContactData_s{
        QString Name;
        QString Address;
        QString Phone;
        QString Email;
        QString Web;
        QString Logo;
    };

    struct UnitsType_s{
        QString UnitElevation;
        QString UnitArea;
        QString UnitTemp;
        QString UnitWind;
        QString UnitPrecipitatation;
        QString UnitPressure;
        QString Currency;
    };

    struct MetaData_s{
        QUuid          instanceUuid;
        InfoData_s     infoData;
        LocationData_s locationData;
        ContactData_s  company;
        ContactData_s  client;
        UnitsType_s    unitsTypes;
        QString        iconPath;
        PaperFormat_e  paperFormat;
        bool           format24h;
        bool           telemetry;
    };

private: /* Members */
    MetaData_s m_MetaData;
    QString    m_str_Workspace;
    QString    m_str_RootPath;
    QString    m_str_ProjectPath;
    QUndoStack m_UndoStack;

    ProjectTree_class    *m_ProjectTree = nullptr;
    friend class ProjectTree_class; // Used for Tree to acces members of project

    Rooms_class           m_RoomsTree;
    Categories_class      m_CategoriesTree;
    VariableTypes_class   m_VariablesTree;
    Users_class           m_UsersTree;
    Rights_class          m_RightsTree;
    UserGroups_class      m_UserGroupsTree;
    ManagedTablets_class  m_ManagedTabletsTree;
    QVector<Page_class *> m_vec_Pages;
    Page_class           *m_ptr_ActivePage = nullptr;
    QUuid                 m_SelectedUuid  = QUuid();
    QUuid                 m_SelectedIUuid = QUuid();
    Controller_class      m_Controller;

private: /* Functions */
    int                  getPageActv() const;
    static PaperFormat_e fromString(const QString       &Format);
    static QString       toString  (const PaperFormat_e &Format);
    static QSize         toSize    (const PaperFormat_e &Format);

public:
    explicit Project_class(QObject *parent = nullptr);
    ~Project_class();

    void build();

    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

    void    setTitle(const QString &Title){ m_MetaData.infoData.Title = Title;
                                            emit titleChanged();
                                            emit infoChanged(); }
    void    setPath (const QString &path) { m_str_RootPath = path; }

    QString      getTitle()         const { return m_MetaData.infoData.Title;             }
    QString      getCreationDate()  const { return m_MetaData.infoData.CreationDate;      }
    QString      getModifiedDate()  const { return m_MetaData.infoData.ModifiedDate;      }
    QString      getLocation()      const { return m_MetaData.locationData.Street;        }
    QString      getWorkspace()     const { return m_str_Workspace;                       }
    QString      getPath()          const { return m_str_RootPath;                        }
    bool         isModified()       const { return !m_UndoStack.isClean();                }
    bool         hasFile()          const { return !m_str_ProjectPath.isEmpty();          }
    bool         canUndo()          const { return m_UndoStack.canUndo();                 }
    bool         canRedo()          const { return m_UndoStack.canRedo();                 }
    QString      undoText()         const { return m_UndoStack.undoText();                }
    QString      redoText()         const { return m_UndoStack.redoText();                }
    QString      pageFormat()       const { return toString(m_MetaData.paperFormat);      }
    QSize        pageSize()         const { return toSize(m_MetaData.paperFormat);        }
    int          pageCount()        const { return m_vec_Pages.size();                    }
    int          pageActv()         const { return getPageActv();                         }
    QVariantMap  infoMap()          const;
    QVariantList pageList()         const;
    QObject     *tree()             const { return m_ProjectTree->getTree();              }
    QVariantMap  context()          const;
    QString      selectedUuid()     const { return m_SelectedUuid.toString(QUuid::WithoutBraces);  }
    QString      selectedIUuid()    const { return m_SelectedIUuid.toString(QUuid::WithoutBraces); }

    void         setPageActv(int index);
    void         setSelectedUuid (const QString &uuid);
    void         setSelectedIUuid(const QString &uuid);

    bool save(QString *error);
    bool load(const QString &projectPath, QString *error);

    Page_class *getPage (int Index);
    Page_class *takePage(int Index);
    void      insertPage(int Index, Page_class *PageRef);
    void      movePage  (int from, int to);

    /* ==== QML API - every change goes through the undo stack ==== */
    Q_INVOKABLE int  cmdAddPage   (int index, const QString &title);
    Q_INVOKABLE void cmdRemovePage(int index);
    Q_INVOKABLE void cmdMovePage  (int from, int to);
    Q_INVOKABLE void cmdRenamePage(int index, const QString &title);

signals:
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
    void contextChanged();
};

#endif // PROJECT_CLASS_H
