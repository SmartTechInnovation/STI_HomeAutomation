#include "project.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "project_tokens.h"
#include "../Logger/logger.h"
#include "../BlockManager/blockmanager.h"

#define PROJECT_NAME "Project"

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
    m_str_ProjectPath = projectPath;

    QFileInfo projectFileInfo(projectPath);
    m_str_RootPath = projectFileInfo.dir().path();

    QFile projectFile(projectPath);
    if(!projectFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        S_Logger.write(Log::Error, PROJECT_NAME + m_MetaData.infoData.Title, "Cannot open project." + projectFile.errorString(), "Error on loading project", "Check if isn't opened by another app.", true);
        if(error != nullptr){
            *error = "Error on loading project" + projectFile.errorString();
        }
        return false;
    }
    QXmlStreamReader projectXml(&projectFile);

    /* Temp Variables */
    Variable_class       *m_ptr_Variable      = nullptr;
    BlockBase_class      *m_ptr_BlockInstance = nullptr;
    Page_class           *m_ptr_Page          = nullptr;

    /* -------------- */

    while(!projectXml.atEnd() && !projectXml.hasError()){
        QXmlStreamReader::TokenType token     = projectXml.readNext();
        QStringView                 tokenName = projectXml.name();
        if(token == QXmlStreamReader::StartElement){
            if(tokenName == TOKEN_PROJECT){
                m_MetaData.infoData.Title           = projectXml.attributes().value(ATTR_TITLE).toString();
                m_MetaData.infoData.ConfigVers      = projectXml.attributes().value(ATTR_CONF_VERS).toString();
                m_MetaData.infoData.CreationDate    = projectXml.attributes().value(ATTR_CREATION_DATE).toString();
                m_MetaData.infoData.ModifiedDate    = projectXml.attributes().value(ATTR_MODIFIED_DATE).toString();
                m_MetaData.locationData.Street      = projectXml.attributes().value(ATTR_STREET).toString();
                m_MetaData.locationData.Town        = projectXml.attributes().value(ATTR_TOWN).toString();
                m_MetaData.locationData.PostCode    = projectXml.attributes().value(ATTR_POSTCODE).toString();
                m_MetaData.locationData.Country     = projectXml.attributes().value(ATTR_COUNTRY).toString();
                m_MetaData.locationData.Longitude   = projectXml.attributes().value(ATTR_LONGITUDE).toString();
                m_MetaData.locationData.Latitude    = projectXml.attributes().value(ATTR_LATITUDE).toString();
                m_MetaData.locationData.Timezone    = projectXml.attributes().value(ATTR_TIMEZONE).toString();
                m_MetaData.iconPath                 = projectXml.attributes().value(ATTR_ICON_PATH).toString();
                m_MetaData.customerData.Customer    = projectXml.attributes().value(ATTR_CUSTOMER).toString();
                m_MetaData.paperFormat              = _paperFromString(projectXml.attributes().value(ATTR_PAPER_FORMAT).toString());
                m_MetaData.unitsTypes.UnitElevation = projectXml.attributes().value(ATTR_U_ELEVATION).toString();
                m_MetaData.unitsTypes.UnitArea      = projectXml.attributes().value(ATTR_U_AREA).toString();
                m_MetaData.unitsTypes.UnitTemp      = projectXml.attributes().value(ATTR_U_TEMP).toString();
                m_MetaData.unitsTypes.UnitWind      = projectXml.attributes().value(ATTR_U_WIND).toString();
                m_MetaData.unitsTypes.UnitPrecipitatation = projectXml.attributes().value(ATTR_U_PRECIPITATION).toString();
                m_MetaData.unitsTypes.UnitPressure  = projectXml.attributes().value(ATTR_U_PRESSURE).toString();
                m_MetaData.unitsTypes.Currency      = projectXml.attributes().value(ATTR_U_CURRENCY).toString();
                m_MetaData.format24h                = projectXml.attributes().value(ATTR_F_CLOCK).toString()   == "true";
                m_MetaData.telemetry                = projectXml.attributes().value(ATTR_TELEMETRY).toString() == "true";
            }else if(tokenName == TOKEN_ROOMS){
                m_RoomsTree.Title                   = projectXml.attributes().value(ATTR_TITLE).toString();
                m_RoomsTree.Icon                    = projectXml.attributes().value(ATTR_ICON_PATH).toString();
            }else if(tokenName == TOKEN_ROOM){
                Room_class *newRoom = new Room_class();
                newRoom->Title                      = projectXml.attributes().value(ATTR_TITLE).toString();
                newRoom->Icon                       = projectXml.attributes().value(ATTR_ICON_PATH).toString();
                newRoom->Area                       = projectXml.attributes().value(ATTR_AREA).toInt();
                newRoom->Color                      = projectXml.attributes().value(ATTR_COLOR).toString();
                newRoom->Order                      = projectXml.attributes().value(ATTR_ORDER).toInt();
                newRoom->Type                       = Room_class::fromString(projectXml.attributes().value(ATTR_TYPE).toString());
                newRoom->Favorite                   = projectXml.attributes().value(ATTR_ORDER).toString() == "true";
                newRoom->Docs                       = projectXml.attributes().value(ATTR_DOCS).toString();
                m_RoomsTree.m_vec_Rooms.push_back(newRoom);
            }
            else if(tokenName == TOKEN_CATEGORIES){
                m_CategoriesTree.Title              = projectXml.attributes().value(ATTR_TITLE).toString();
                m_CategoriesTree.Icon               = projectXml.attributes().value(ATTR_ICON_PATH).toString();
            }else if(tokenName == TOKEN_CATEGORY){
                Category_class *newCategory= new Category_class();
                newCategory->Title                  = projectXml.attributes().value(ATTR_TITLE).toString();
                newCategory->Icon                   = projectXml.attributes().value(ATTR_ICON_PATH).toString();
                newCategory->Color                  = projectXml.attributes().value(ATTR_COLOR).toString();
                newCategory->Order                  = projectXml.attributes().value(ATTR_ORDER).toInt();
                newCategory->Favorite               = projectXml.attributes().value(ATTR_ORDER).toString() == "true";
                newCategory->Docs                   = projectXml.attributes().value(ATTR_DOCS).toString();
                m_CategoriesTree.m_vec_Categories.push_back(newCategory);
            }else if(tokenName == TOKEN_USERS){
                m_UsersTree.Title                   = projectXml.attributes().value(ATTR_TITLE).toString();
                m_UsersTree.Icon                    = projectXml.attributes().value(ATTR_ICON_PATH).toString();
            }else if(tokenName == TOKEN_USER){
                User_class *newUser = new User_class();
                newUser->Username                   = projectXml.attributes().value(ATTR_USERNAME).toString();
                newUser->PassHash                   = projectXml.attributes().value(ATTR_PASSWORD).toString();
                newUser->Admin                      = projectXml.attributes().value(ATTR_MAIN).toString() == "true";
                m_UsersTree.m_vec_Users.push_back(newUser);
            }else if(tokenName == TOKEN_RIGHTS){
                QString Title                       = projectXml.attributes().value(ATTR_TITLE).toString();
                QString Icon                        = projectXml.attributes().value(ATTR_ICON_PATH).toString();
                /* To do */
            }else if(tokenName == TOKEN_PERMISSION){
                QString Title                       = projectXml.attributes().value(ATTR_TITLE).toString();
                int     RightId                     = projectXml.attributes().value(ATTR_RIGHTS_ID).toInt();
                /* To Do */
            }else if(tokenName == TOKEN_USER_GROUPS){
                QString Title                       = projectXml.attributes().value(ATTR_TITLE).toString();
                QString Icon                        = projectXml.attributes().value(ATTR_ICON_PATH).toString();
            }else if(tokenName == TOKEN_USER_GROUP){
                QString Title                       = projectXml.attributes().value(ATTR_TITLE).toString();
                size_t  Permission                  = projectXml.attributes().value(ATTR_RIGHTS_ID).toInt();
            }else if(tokenName == TOKEN_M_TABLETS){

            }else if(tokenName == TOKEN_TABLET){

            }else if(tokenName == TOKEN_VARIABLES){
                m_VariablesTree.Title               = projectXml.attributes().value(ATTR_TITLE).toString();
                m_VariablesTree.Icon                = projectXml.attributes().value(ATTR_ICON_PATH).toString();
            }else if(tokenName == TOKEN_VARIABLE){
                Variable_class *newVariable = new Variable_class();
                newVariable->Title                  = projectXml.attributes().value(ATTR_TITLE).toString();
                newVariable->Type                   = Port_class::fromString(projectXml.attributes().value(ATTR_TYPE).toString());
                newVariable->Uuid                   = QUuid(projectXml.attributes().value(ATTR_UUID).toString());
                m_VariablesTree.m_map_Variables[newVariable->Type].push_back(newVariable);
                if(m_ptr_Variable == nullptr){
                    m_ptr_Variable = newVariable;
                }
            }else if(tokenName == TOKEN_SOURCE){
                QUuid   SourceUuid                  = QUuid(projectXml.attributes().value(ATTR_UUID).toString());
                if(m_ptr_Variable != nullptr){
                    m_ptr_Variable->m_vec_SignalSources.push_back(SourceUuid);
                }
            }else if(tokenName == TOKEN_ROOT_DEV){
                m_Controller.Title                  = projectXml.attributes().value(ATTR_TITLE).toString();
                m_Controller.IpAddress              = projectXml.attributes().value(ATTR_IP_ADDR).toString();
                m_Controller.PortHttp               = projectXml.attributes().value(ATTR_PORT_HTTP).toInt();
                m_Controller.PortHttps              = projectXml.attributes().value(ATTR_PORT_HTTPS).toInt();
                m_Controller.SerialNr               = projectXml.attributes().value(ATTR_SERIAL_NUM).toString();
                m_Controller.Model                  = projectXml.attributes().value(ATTR_DEV_MODEL).toString();
            }else if(tokenName == TOKEN_VIRTUAL_IN){
                m_Controller.VirtualInputs.Title    = projectXml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_VIRTUAL_OUT){
                m_Controller.VirtualOutputs.Title   = projectXml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_PROGRAM){

            }else if(tokenName == TOKEN_PAGE){
                if(m_ptr_Page == nullptr){
                    m_ptr_Page = new Page_class();
                }
                if(m_ptr_Page){
                    m_ptr_Page->Title               = projectXml.attributes().value(ATTR_TITLE).toString();
                }
            }else if(tokenName == TOKEN_SIGNAL_IN){

            }else if(tokenName == TOKEN_SIGNAL_OUT){

            }else if(tokenName == TOKEN_BLOCK){
                QString BlockTitle                  = projectXml.attributes().value(ATTR_TITLE).toString();
                QUuid   BlockUuid                   = QUuid(projectXml.attributes().value(ATTR_UUID).toString());
                m_ptr_BlockInstance = S_BlockManager.getInstance(BlockUuid);
                if(m_ptr_BlockInstance != nullptr){
                    m_ptr_BlockInstance->Title     = BlockTitle;
                    m_ptr_BlockInstance->Position.setX(projectXml.attributes().value(ATTR_PX).toFloat());
                    m_ptr_BlockInstance->Position.setY(projectXml.attributes().value(ATTR_PY).toFloat());
                    m_ptr_BlockInstance->Width     = projectXml.attributes().value(ATTR_WIDTH).toFloat();
                    m_ptr_BlockInstance->Color     = projectXml.attributes().value(ATTR_COLOR).toString();
                    m_ptr_BlockInstance->Room      = projectXml.attributes().value(ATTR_ROOM).toString();
                    m_ptr_BlockInstance->Category  = projectXml.attributes().value(ATTR_CATEGORY).toString();

                    if(m_ptr_Page != nullptr){
                        m_ptr_Page->addBlockInstance(m_ptr_BlockInstance);
                    }
                }else{
                    S_Logger.write(Log::Warning, PROJECT_NAME + m_MetaData.infoData.Title, "Block Reference not finded for: " + BlockTitle, "Block Reference not found", "Check Library of Blocks", true);
                }
            }else if(tokenName == TOKEN_INPUT){
                //m_ptr_BlockInstance
            }

        }else if(token == QXmlStreamReader::EndElement){
            if(tokenName == TOKEN_VARIABLE){
                m_ptr_Variable = nullptr;
            }else if(tokenName == TOKEN_BLOCK){
                m_ptr_BlockInstance = nullptr;
            }
        }
    }

    return true;
}

QString Project_class::_paperToString(PaperFormat_e format){
    QString Result = "Unk";
    switch(format){
        case PaperA4:
            Result = "A4";
            break;
        case PaperA3:
            Result = "A3";
            break;
        case PaperA2:
            Result = "A2";
            break;
        case PaperA1:
            Result = "A1";
            break;
        case PaperA0:
            Result = "A0";
            break;
        case PaperMax:
            break;
        default:
            Result = "A4";
    }
    return Result;
}

Project_class::PaperFormat_e Project_class::_paperFromString(const QString &format){
    if(format == "A4") return PaperFormat_e::PaperA4;
    if(format == "A3") return PaperFormat_e::PaperA3;
    if(format == "A2") return PaperFormat_e::PaperA2;
    if(format == "A1") return PaperFormat_e::PaperA1;
    if(format == "A0") return PaperFormat_e::PaperA0;
    return PaperMax;
}