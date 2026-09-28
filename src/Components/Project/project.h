#ifndef PROJECT_CLASS_H
#define PROJECT_CLASS_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QColor>
#include <QUndoStack>
#include <QMap>

#include "Page/page.h"
#include "Room/room.h"
#include "User/user.h"
#include "Category/category.h"
#include "Variable/variable.h"
#include "Controller/controller.h"

class Project_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString path  READ getPath)
    //Q_PROPERTY(bool    modified READ isModified  NOTIFY modifiedChanged)
    //Q_PROPERTY(bool    canUndo  READ canUndo  NOTIFY undoStateChanged)
    //Q_PROPERTY(bool    canRedo  READ canRedo  NOTIFY undoStateChanged)
    //Q_PROPERTY(QString undoText READ undoText NOTIFY undoStateChanged)
    //Q_PROPERTY(QString redoText READ redoText NOTIFY undoStateChanged)
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

    struct CustomerData_s{
        QString Customer;
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
        CustomerData_s customerData;
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

private: /* Functions */
    QString       _getPath() const;
    QString       _paperToString(PaperFormat_e format);
    PaperFormat_e _paperFromString(const QString &Format);

public:
    explicit Project_class(QObject *parent = nullptr);

    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

    void    setTitle   (const QString &Title)      { m_MetaData.infoData.Title = Title;     }
    void    setPath    (const QString &path)       { m_str_RootPath            = path;      }

    QString getTitle()         const { return m_MetaData.infoData.Title;        }
    QString getCreationDate()  const { return m_MetaData.infoData.CreationDate; }
    QString getModifiedDate()  const { return m_MetaData.infoData.ModifiedDate; }
    QString getLocation()      const { return m_MetaData.locationData.Street;   }
    QString getWorkspace()     const { return m_str_Workspace;                  }
    QString getPath()          const { return m_str_RootPath;                   }

    bool save(QString *error);
    bool load(const QString &projectPath, QString *error);


signals:
    void titleChanged();
};

#endif // PROJECT_CLASS_H
