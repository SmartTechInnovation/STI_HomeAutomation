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
#include <QStandardItemModel>

#include "Page/page.h"
#include "Room/room.h"
#include "User/user.h"
#include "Category/category.h"
#include "Variable/variable.h"
#include "Controller/controller.h"

#define WORKSPACE_SCALE 10

class Project_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString      title      READ getTitle    NOTIFY titleChanged    )
    Q_PROPERTY(QString      path       READ getPath     NOTIFY pathChanged     )
    Q_PROPERTY(bool         modified   READ isModified  NOTIFY modifiedChanged )
    Q_PROPERTY(bool         hasFile    READ hasFile     NOTIFY pathChanged     )
    Q_PROPERTY(bool         canUndo    READ canUndo     NOTIFY undoStateChanged)
    Q_PROPERTY(bool         canRedo    READ canRedo     NOTIFY undoStateChanged)
    Q_PROPERTY(QString      undoText   READ undoText    NOTIFY undoStateChanged)
    Q_PROPERTY(QString      redoText   READ redoText    NOTIFY undoStateChanged)
    Q_PROPERTY(QString      pageFormat READ pageFormat  NOTIFY infoChanged     )
    Q_PROPERTY(QSize        pageSize   READ pageSize    NOTIFY infoChanged     )
    Q_PROPERTY(int          pageCount  READ pageCount   NOTIFY pagesChanged    )
    Q_PROPERTY(int          pageActv   READ pageActv    NOTIFY pageActvChanged WRITE setPageActv)
    Q_PROPERTY(QVariantMap  info       READ info        NOTIFY infoChanged     )
    Q_PROPERTY(QVariantList pages      READ pages       NOTIFY pagesChanged    )
    Q_PROPERTY(QObject     *tree       READ tree        NOTIFY treeChanged     )


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

    struct RoomsTree_s{
        QString Title;
        QString Icon;
        QVector<Room_class *> m_vec_Rooms;
    };

    struct CategoriesTree_s{
        QString Title;
        QString Icon;
        QVector<Category_class *> m_vec_Categories;
    };

    struct UsersTree_s{
        QString Title;
        QString Icon;
        QVector<User_class *> m_vec_Users;
    };

    struct VariablesTree_s{
        QString Title;
        QString Icon;
        QMap<int, QVector<Variable_class *>> m_map_Variables;
    };

private: /* Members */
    MetaData_s m_MetaData;
    QString    m_str_Workspace;
    QString    m_str_RootPath;
    QString    m_str_ProjectPath;
    QUndoStack m_UndoStack;

    RoomsTree_s           m_RoomsTree;
    CategoriesTree_s      m_CategoriesTree;
    VariablesTree_s       m_VariablesTree;
    UsersTree_s           m_UsersTree;
    QVector<Page_class *> m_vec_Pages;
    Page_class           *m_ptr_ActivePage = nullptr;

    Controller_class      m_Controller;

    QStandardItemModel    *m_ptr_Tree     = nullptr;
    bool                   m_b_Tree_Dirty = false;


private: /* Functions */
    QString       _getPath()       const;
    int           _getActivePage() const;
    static QString       _paperToString(PaperFormat_e format);
    static PaperFormat_e _paperFromString(const QString &Format);
    static QSize         _paperToSize(PaperFormat_e format);


public:
    explicit Project_class(QObject *parent = nullptr);
    ~Project_class();

    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

    void    setTitle(const QString &Title);
    void    setPath (const QString &path);

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
    QString      pageFormat()       const { return _paperToString(m_MetaData.paperFormat);}
    QSize        pageSize()         const { return _paperToSize(m_MetaData.paperFormat);  }
    int          pageCount()        const { return m_vec_Pages.size();                    }
    int          pageActv()         const { return _getActivePage();                      }
    QVariantMap  info()             const;
    QVariantList pages()            const;
    QObject     *tree()             const { return m_ptr_Tree;                            }

    void         setPageActv(int index);

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
};

#endif // PROJECT_CLASS_H
