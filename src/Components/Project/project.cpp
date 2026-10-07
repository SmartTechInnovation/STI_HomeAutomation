#include "project.h"

#include <QDir>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QDateTime>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "project_tokens.h"
#include "project_commands.h"
#include "../Logger/logger.h"
#include "../BlockManager/blockmanager.h"
#include "../PeripheryManager/peripherymanager.h"

#define PROJECT_NAME "Project"
#define DATE_FORMAT  "yyyy-MM-dd HH:mm:ss"

/* ================= Local helpers ================= */

QString nowString(){
    return QDateTime::currentDateTime().toString(DATE_FORMAT);
}

QString boolString(bool value){
    return value ? QStringLiteral("true") : QStringLiteral("false");
}

QString colorString(const QColor &color){
    return color.isValid() ? color.name() : QString();
}

/* ================================================== */

Project_class::Project_class(QObject *parent) : QObject(parent) {
    m_ProjectTree = new ProjectTree_class(this);
}

Project_class::~Project_class(){
    m_UndoStack.clear();
    qDeleteAll(m_vec_Pages); m_vec_Pages.clear();
}

void Project_class::build(){

}

void Project_class::undo(){
    m_UndoStack.undo();
}

void Project_class::redo(){
    m_UndoStack.redo();
}

QVariantMap Project_class::infoMap() const{
    auto resolveUrl = [this](const QString &path) -> QString {
        if(path.isEmpty()) return QString();
        if(path.startsWith("qrc:") || path.startsWith("file:") || path.startsWith("http")) return path;
        QFileInfo fileInfo(path);
        const QString absolute = fileInfo.isAbsolute() ? path : QDir(m_str_RootPath).filePath(path);
        return QUrl::fromLocalFile(absolute).toString();
    };

    QVariantMap company;
    company["name"]    = m_MetaData.company.Name;
    company["address"] = m_MetaData.company.Address;
    company["phone"]   = m_MetaData.company.Phone;
    company["email"]   = m_MetaData.company.Email;
    company["web"]     = m_MetaData.company.Web;
    company["logoUrl"] = resolveUrl(m_MetaData.company.Logo);

    QVariantMap client;
    client["name"]    = m_MetaData.client.Name;
    client["address"] = m_MetaData.client.Address;
    client["phone"]   = m_MetaData.client.Phone;
    client["email"]   = m_MetaData.client.Email;

    QVariantMap Result;
    Result["title"]       = m_MetaData.infoData.Title;
    Result["configVers"]  = m_MetaData.infoData.ConfigVers;
    Result["cDate"]       = m_MetaData.infoData.CreationDate;
    Result["mDate"]       = m_MetaData.infoData.ModifiedDate;
    Result["author"]      = m_MetaData.infoData.Author;
    Result["street"]      = m_MetaData.locationData.Street;
    Result["town"]        = m_MetaData.locationData.Town;
    Result["postCode"]    = m_MetaData.locationData.PostCode;
    Result["country"]     = m_MetaData.locationData.Country;
    Result["latitude"]    = m_MetaData.locationData.Latitude;
    Result["longitude"]   = m_MetaData.locationData.Longitude;
    Result["timezone"]    = m_MetaData.locationData.Timezone;
    Result["paperFormat"] = toString(m_MetaData.paperFormat);
    Result["company"]     = company;
    Result["client"]      = client;
    return Result;
}

QVariantList Project_class::pageList() const{
    QVariantList Result;
    for(int i = 0; i < m_vec_Pages.size(); i++){
        QVariantMap pageMap;
        pageMap["title"]  = m_vec_Pages[i]->Title;
        pageMap["mDate"]  = m_vec_Pages[i]->ModifiedDate;
        pageMap["index"]  = i;
        pageMap["number"] = i + 2;            // page 1 = title page (Home)
        pageMap["pageRef"]   = QVariant::fromValue<QObject *>(m_vec_Pages[i]);
        Result.push_back(pageMap);
    }
    return Result;
}

QVariantMap Project_class::context() const{
    QVariantMap Context;

    Context = m_Controller.getContextMenu(m_SelectedUuid);
    if(Context.size() != 0){
        return Context;
    }

    return Context;
}

void Project_class::setPageActv(int index){
    if(index < -1)                  index = -1;
    if(index >= m_vec_Pages.size()) index = m_vec_Pages.size() - 1;
    if(index == getPageActv()){
        return;
    }
    if(index == -1){
        m_ptr_ActivePage = nullptr;
    }else{
        m_ptr_ActivePage = m_vec_Pages[index];
    }
    emit pageActvChanged();
}

void Project_class::setSelectedUuid(const QString &uuid){
    if(uuid.isEmpty())
        m_SelectedUuid = QUuid();
    else
        m_SelectedUuid = QUuid(uuid);
    emit selectedUuidChanged();
    emit contextChanged();
}

void Project_class::setSelectedIUuid(const QString &uuid){
    if(uuid.isEmpty())
        m_SelectedIUuid = QUuid();
    else
        m_SelectedIUuid = QUuid(uuid);
    emit selectedIUuidChanged();
}

bool Project_class::save(QString *error){
    const QString previousDate = m_MetaData.infoData.ModifiedDate;
    m_MetaData.infoData.ModifiedDate = nowString();

    if(QFile::exists(m_str_ProjectPath)){
        const QString backupPath = m_str_ProjectPath + ".bak";
        QFile::remove(backupPath);
        QFile::copy(m_str_ProjectPath, backupPath);
    }

    QSaveFile projectFile(m_str_ProjectPath);
    if(!projectFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        m_MetaData.infoData.ModifiedDate = previousDate;
        if(error) *error = tr("Cannot write project: ") + projectFile.errorString();
        return false;
    }
    QXmlStreamWriter xml(&projectFile);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(4);
    xml.writeStartDocument();

    /* ===== Project ==== */
    xml.writeStartElement(TOKEN_PROJECT);
    xml.writeAttribute(ATTR_TITLE,           m_MetaData.infoData.Title);
    xml.writeAttribute(ATTR_CONF_VERS,       m_MetaData.infoData.ConfigVers);
    xml.writeAttribute(ATTR_CREATION_DATE,   m_MetaData.infoData.CreationDate);
    xml.writeAttribute(ATTR_MODIFIED_DATE,   m_MetaData.infoData.ModifiedDate);
    xml.writeAttribute(ATTR_AUTHOR,          m_MetaData.infoData.Author);
    xml.writeAttribute(ATTR_STREET,          m_MetaData.locationData.Street);
    xml.writeAttribute(ATTR_TOWN,            m_MetaData.locationData.Town);
    xml.writeAttribute(ATTR_POSTCODE,        m_MetaData.locationData.PostCode);
    xml.writeAttribute(ATTR_COUNTRY,         m_MetaData.locationData.Country);
    xml.writeAttribute(ATTR_LONGITUDE,       m_MetaData.locationData.Longitude);
    xml.writeAttribute(ATTR_LATITUDE,        m_MetaData.locationData.Latitude);
    xml.writeAttribute(ATTR_TIMEZONE,        m_MetaData.locationData.Timezone);
    xml.writeAttribute(ATTR_ICON_PATH,       m_MetaData.iconPath);
    xml.writeAttribute(ATTR_PAPER_FORMAT,    toString(m_MetaData.paperFormat));
    xml.writeAttribute(ATTR_U_ELEVATION,     m_MetaData.unitsTypes.UnitElevation);
    xml.writeAttribute(ATTR_U_AREA,          m_MetaData.unitsTypes.UnitArea);
    xml.writeAttribute(ATTR_U_TEMP,          m_MetaData.unitsTypes.UnitTemp);
    xml.writeAttribute(ATTR_U_WIND,          m_MetaData.unitsTypes.UnitWind);
    xml.writeAttribute(ATTR_U_PRECIPITATION, m_MetaData.unitsTypes.UnitPrecipitatation);
    xml.writeAttribute(ATTR_U_PRESSURE,      m_MetaData.unitsTypes.UnitPressure);
    xml.writeAttribute(ATTR_U_CURRENCY,      m_MetaData.unitsTypes.Currency);
    xml.writeAttribute(ATTR_F_CLOCK,         boolString(m_MetaData.format24h));
    xml.writeAttribute(ATTR_TELEMETRY,       boolString(m_MetaData.telemetry));

    /* ===== Company data =====*/
    xml.writeStartElement(TOKEN_COMPANY);
    xml.writeAttribute(ATTR_NAME,            m_MetaData.company.Name);
    xml.writeAttribute(ATTR_ADDRESS,         m_MetaData.company.Address);
    xml.writeAttribute(ATTR_PHONE,           m_MetaData.company.Phone);
    xml.writeAttribute(ATTR_EMAIL,           m_MetaData.company.Email);
    xml.writeAttribute(ATTR_WEB,             m_MetaData.company.Web);
    xml.writeAttribute(ATTR_LOGO_PATH,       m_MetaData.company.Logo);
    xml.writeEndElement();

    /* ===== Client Data ===== */
    xml.writeStartElement(TOKEN_CLIENT);
    xml.writeAttribute(ATTR_NAME,            m_MetaData.client.Name);
    xml.writeAttribute(ATTR_ADDRESS,         m_MetaData.client.Address);
    xml.writeAttribute(ATTR_PHONE,           m_MetaData.client.Phone);
    xml.writeAttribute(ATTR_EMAIL,           m_MetaData.client.Email);
    xml.writeEndElement();

    /* =====    Rooms    ===== */
    xml.writeStartElement(TOKEN_ROOMS);
    xml.writeAttribute(ATTR_TITLE,           m_RoomsTree.Title);
    for(auto &room : m_RoomsTree.m_vec_Rooms){
        xml.writeStartElement(TOKEN_ROOM);
        xml.writeAttribute(ATTR_TITLE,       room->Title);
        xml.writeAttribute(ATTR_ICON_PATH,   room->Icon);
        xml.writeAttribute(ATTR_AREA,        QString::number(room->Area));
        xml.writeAttribute(ATTR_COLOR,       colorString(room->Color));
        xml.writeAttribute(ATTR_ORDER,       QString::number(room->Order));
        xml.writeAttribute(ATTR_TYPE,        Room_class::toString(room->Type));
        xml.writeAttribute(ATTR_FAVORITE,    boolString(room->Favorite));
        xml.writeAttribute(ATTR_DOCS,        room->Docs);
        xml.writeEndElement();
    }
    xml.writeEndElement();

    /* =====  Categories ===== */
    xml.writeStartElement(TOKEN_CATEGORIES);
    xml.writeAttribute(ATTR_TITLE,           m_CategoriesTree.Title);
    for(auto &category : m_CategoriesTree.m_vec_Categories){
        xml.writeStartElement(TOKEN_CATEGORY);
        xml.writeAttribute(ATTR_TITLE,       category->Title);
        xml.writeAttribute(ATTR_ICON_PATH,   category->Icon);
        xml.writeAttribute(ATTR_COLOR,       colorString(category->Color));
        xml.writeAttribute(ATTR_ORDER,       QString::number(category->Order));
        xml.writeAttribute(ATTR_FAVORITE,    boolString(category->Favorite));
        xml.writeAttribute(ATTR_DOCS,        category->Docs);
        xml.writeEndElement();
    }
    xml.writeEndElement();

    /* =====    Users    ===== */
    xml.writeStartElement(TOKEN_USERS);
    xml.writeAttribute(ATTR_TITLE,            m_UsersTree.Title);
    for(auto &user : m_UsersTree.m_vec_Users){
        xml.writeStartElement(TOKEN_USER);
        xml.writeAttribute(ATTR_USERNAME,     user->Username);
        xml.writeAttribute(ATTR_PASSWORD,     user->PassHash);
        xml.writeAttribute(ATTR_MAIN,         boolString(user->Admin));
        xml.writeEndElement();
    }
    xml.writeEndElement();

    /* =====    Rights    ===== */
    xml.writeStartElement(TOKEN_RIGHTS);
    xml.writeAttribute(ATTR_TITLE,            m_RightsTree.Title);
    for(auto &right : m_RightsTree.m_vec_Rights){
        xml.writeStartElement(TOKEN_PERMISSION);
        xml.writeAttribute(ATTR_TITLE,        right->Title);
        xml.writeAttribute(ATTR_RIGHTS_ID,    right->PermisionId);
        xml.writeEndElement();
    }
    xml.writeEndElement();

    /* =====   User Groups  ===== */
    xml.writeStartElement(TOKEN_USER_GROUPS);
    xml.writeAttribute(ATTR_TITLE,            m_UserGroupsTree.Title);
    for(auto &uGroup : m_UserGroupsTree.m_vec_UserGroups){
        xml.writeStartElement(TOKEN_USER_GROUP);
        xml.writeAttribute(ATTR_TITLE,        uGroup->Title);
        xml.writeAttribute(ATTR_PERMISSIONS,  QString::number(uGroup->Permissions));
        xml.writeEndElement();
    }
    xml.writeEndElement();

    /* =====  Managed Tablets ===== */
    xml.writeStartElement(TOKEN_M_TABLETS);
    xml.writeAttribute(ATTR_TITLE,            m_ManagedTabletsTree.Title);
    for(auto &tablet : m_ManagedTabletsTree.m_vec_ManagedTablets){
        xml.writeStartElement(TOKEN_TABLET);
        xml.writeAttribute(ATTR_TITLE,        tablet->Title);
        xml.writeAttribute(ATTR_ROOM,         tablet->Room);
        xml.writeEndElement();
    }
    xml.writeEndElement();

    /* ===== Variables  ====== */
    xml.writeStartElement(TOKEN_VARIABLES);
    xml.writeAttribute(ATTR_TITLE,            m_VariablesTree.Title);
    for(auto &variablesType : m_VariablesTree.m_vec_VariablesType){
        for(auto &variable : variablesType->m_vec_Variables){
            xml.writeStartElement(TOKEN_VARIABLE);
            xml.writeAttribute(ATTR_TITLE,    variable->Title);
            xml.writeAttribute(ATTR_TYPE,     Port_class::toString((Port_class::PortType_e)variable->Type));
            xml.writeAttribute(ATTR_UUID,     variable->Uuid.toString(QUuid::WithoutBraces));
            xml.writeAttribute(ATTR_DEFAULT,  variable->Default);
            for(auto &source : variable->m_vec_SignalSources){
                xml.writeStartElement(TOKEN_SOURCE);
                xml.writeAttribute(ATTR_UUID, source.toString(QUuid::WithoutBraces));
                xml.writeEndElement();
            }
            xml.writeEndElement();
        }
    }
    xml.writeEndElement();

    /* ====== Root Device ====== */
    xml.writeStartElement(TOKEN_ROOT_DEV);
    xml.writeAttribute(ATTR_TITLE,            m_Controller.Title);
    xml.writeAttribute(ATTR_IP_ADDR,          m_Controller.IpAddress);
    xml.writeAttribute(ATTR_PORT_HTTP,        QString::number(m_Controller.PortHttp));
    xml.writeAttribute(ATTR_PORT_HTTPS,       QString::number(m_Controller.PortHttps));
    xml.writeAttribute(ATTR_SERIAL_NUM,       m_Controller.SerialNr);
    xml.writeAttribute(ATTR_DEV_MODEL,        m_Controller.Model);
    /* ====== Periphery  ======= */

    xml.writeEndElement();


    m_UndoStack.setClean();
    emit infoChanged();
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
    QXmlStreamReader xml(&projectFile);

    /* ===== Temp Variables ===== */
    Variable_class       *m_ptr_Variable      = nullptr;
    BlockBase_class      *m_ptr_BlockInstance = nullptr;
    Page_class           *m_ptr_Page          = nullptr;
    /* ========================== */

    while(!xml.atEnd() && !xml.hasError()){
        QXmlStreamReader::TokenType token     = xml.readNext();
        QStringView                 tokenName = xml.name();
        if(token == QXmlStreamReader::StartElement){
            if(tokenName == TOKEN_PROJECT){
                m_MetaData.instanceUuid             = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                m_MetaData.infoData.Title           = xml.attributes().value(ATTR_TITLE).toString();
                m_MetaData.infoData.ConfigVers      = xml.attributes().value(ATTR_CONF_VERS).toString();
                m_MetaData.infoData.CreationDate    = xml.attributes().value(ATTR_CREATION_DATE).toString();
                m_MetaData.infoData.ModifiedDate    = xml.attributes().value(ATTR_MODIFIED_DATE).toString();
                m_MetaData.locationData.Street      = xml.attributes().value(ATTR_STREET).toString();
                m_MetaData.locationData.Town        = xml.attributes().value(ATTR_TOWN).toString();
                m_MetaData.locationData.PostCode    = xml.attributes().value(ATTR_POSTCODE).toString();
                m_MetaData.locationData.Country     = xml.attributes().value(ATTR_COUNTRY).toString();
                m_MetaData.locationData.Longitude   = xml.attributes().value(ATTR_LONGITUDE).toString();
                m_MetaData.locationData.Latitude    = xml.attributes().value(ATTR_LATITUDE).toString();
                m_MetaData.locationData.Timezone    = xml.attributes().value(ATTR_TIMEZONE).toString();
                m_MetaData.iconPath                 = xml.attributes().value(ATTR_ICON_PATH).toString();
                m_MetaData.paperFormat              = fromString(xml.attributes().value(ATTR_PAPER_FORMAT).toString());
                m_MetaData.unitsTypes.UnitElevation = xml.attributes().value(ATTR_U_ELEVATION).toString();
                m_MetaData.unitsTypes.UnitArea      = xml.attributes().value(ATTR_U_AREA).toString();
                m_MetaData.unitsTypes.UnitTemp      = xml.attributes().value(ATTR_U_TEMP).toString();
                m_MetaData.unitsTypes.UnitWind      = xml.attributes().value(ATTR_U_WIND).toString();
                m_MetaData.unitsTypes.UnitPrecipitatation = xml.attributes().value(ATTR_U_PRECIPITATION).toString();
                m_MetaData.unitsTypes.UnitPressure  = xml.attributes().value(ATTR_U_PRESSURE).toString();
                m_MetaData.unitsTypes.Currency      = xml.attributes().value(ATTR_U_CURRENCY).toString();
                m_MetaData.format24h                = xml.attributes().value(ATTR_F_CLOCK).toString()   == "true";
                m_MetaData.telemetry                = xml.attributes().value(ATTR_TELEMETRY).toString() == "true";
            }else if(tokenName == TOKEN_COMPANY){
                m_MetaData.company.Name             = xml.attributes().value(ATTR_NAME).toString();
                m_MetaData.company.Address          = xml.attributes().value(ATTR_ADDRESS).toString();
                m_MetaData.company.Email            = xml.attributes().value(ATTR_EMAIL).toString();
                m_MetaData.company.Phone            = xml.attributes().value(ATTR_PHONE).toString();
                m_MetaData.company.Logo             = xml.attributes().value(ATTR_LOGO_PATH).toString();
                m_MetaData.company.Web              = xml.attributes().value(ATTR_WEB).toString();
            }else if(tokenName == TOKEN_CLIENT){
                m_MetaData.client.Name              = xml.attributes().value(ATTR_NAME).toString();
                m_MetaData.client.Address           = xml.attributes().value(ATTR_ADDRESS).toString();
                m_MetaData.client.Email             = xml.attributes().value(ATTR_EMAIL).toString();
                m_MetaData.client.Phone             = xml.attributes().value(ATTR_PHONE).toString();
            }else if(tokenName == TOKEN_ROOMS){
                m_RoomsTree.Uuid                    = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_RoomsTree.IUuid                   = QUuid().createUuidV7();
                m_RoomsTree.Title                   = xml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_ROOM){
                Room_class *newRoom = new Room_class();
                newRoom->IUuid                      = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                newRoom->Title                      = xml.attributes().value(ATTR_TITLE).toString();
                newRoom->Icon                       = xml.attributes().value(ATTR_ICON_PATH).toString();
                newRoom->Area                       = xml.attributes().value(ATTR_AREA).toInt();
                newRoom->Color                      = xml.attributes().value(ATTR_COLOR).toString();
                newRoom->Order                      = xml.attributes().value(ATTR_ORDER).toInt();
                newRoom->Type                       = Room_class::fromString(xml.attributes().value(ATTR_TYPE).toString());
                newRoom->Favorite                   = xml.attributes().value(ATTR_ORDER).toString() == "true";
                newRoom->Docs                       = xml.attributes().value(ATTR_DOCS).toString();
                m_RoomsTree.m_vec_Rooms.push_back(newRoom);
            }
            else if(tokenName == TOKEN_CATEGORIES){
                m_CategoriesTree.Uuid               = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_CategoriesTree.IUuid              = QUuid().createUuidV7();
                m_CategoriesTree.Title              = xml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_CATEGORY){
                Category_class *newCategory= new Category_class();
                newCategory->IUuid                  = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                newCategory->Title                  = xml.attributes().value(ATTR_TITLE).toString();
                newCategory->Icon                   = xml.attributes().value(ATTR_ICON_PATH).toString();
                newCategory->Color                  = xml.attributes().value(ATTR_COLOR).toString();
                newCategory->Order                  = xml.attributes().value(ATTR_ORDER).toInt();
                newCategory->Favorite               = xml.attributes().value(ATTR_ORDER).toString() == "true";
                newCategory->Docs                   = xml.attributes().value(ATTR_DOCS).toString();
                m_CategoriesTree.m_vec_Categories.push_back(newCategory);
            }else if(tokenName == TOKEN_USERS){
                m_UsersTree.Uuid                    = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_UsersTree.IUuid                   = QUuid().createUuidV7();
                m_UsersTree.Title                   = xml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_USER){
                User_class *newUser = new User_class();
                newUser->IUuid                      = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                newUser->Username                   = xml.attributes().value(ATTR_USERNAME).toString();
                newUser->PassHash                   = xml.attributes().value(ATTR_PASSWORD).toString();
                newUser->Admin                      = xml.attributes().value(ATTR_MAIN).toString() == "true";
                m_UsersTree.m_vec_Users.push_back(newUser);
            }else if(tokenName == TOKEN_RIGHTS){
                m_RightsTree.Uuid                   = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_RightsTree.IUuid                  = QUuid().createUuidV7();
                m_RightsTree.Title                  = xml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_PERMISSION){
                Right_class *newPermission = new Right_class();
                newPermission->IUuid                = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                newPermission->Title                = xml.attributes().value(ATTR_TITLE).toString();
                newPermission->PermisionId          = xml.attributes().value(ATTR_RIGHTS_ID).toString();
                m_RightsTree.m_vec_Rights.append(newPermission);
            }else if(tokenName == TOKEN_USER_GROUPS){
                m_UserGroupsTree.Uuid               = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_UserGroupsTree.IUuid              = QUuid().createUuidV7();
                m_UserGroupsTree.Title              = xml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_USER_GROUP){
                UserGroup_class *newUserGroup = new UserGroup_class();
                newUserGroup->IUuid                 = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                newUserGroup->Title                 = xml.attributes().value(ATTR_TITLE).toString();
                newUserGroup->Permissions           = xml.attributes().value(ATTR_RIGHTS_ID).toInt();
                m_UserGroupsTree.m_vec_UserGroups.push_back(newUserGroup);
            }else if(tokenName == TOKEN_M_TABLETS){
                m_ManagedTabletsTree.Uuid           = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_ManagedTabletsTree.IUuid          = QUuid().createUuidV7();
                m_ManagedTabletsTree.Title          = xml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_TABLET){
                ManagedTablet_class *newManagedTablet = new ManagedTablet_class();
                newManagedTablet->IUuid             = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                newManagedTablet->Title             = xml.attributes().value(ATTR_TITLE).toString();
                newManagedTablet->Room              = xml.attributes().value(ATTR_ROOM).toString();
                m_ManagedTabletsTree.m_vec_ManagedTablets.push_back(newManagedTablet);
            }else if(tokenName == TOKEN_VARIABLES){
                m_VariablesTree.Uuid                = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_VariablesTree.IUuid               = QUuid().createUuidV7();
                m_VariablesTree.Title               = xml.attributes().value(ATTR_TITLE).toString();
            }else if(tokenName == TOKEN_VARIABLE){
                Variable_class *newVariable = new Variable_class();
                newVariable->IUuid                  = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                newVariable->Title                  = xml.attributes().value(ATTR_TITLE).toString();
                QString      Type                   = xml.attributes().value(ATTR_TYPE).toString();
                newVariable->Type                   = Port_class::fromString(xml.attributes().value(ATTR_TYPE).toString());
                VariableType_class *VariableTypeRef = nullptr;
                for(auto &varType : m_VariablesTree.m_vec_VariablesType){
                    if(varType->Title == Type){
                        VariableTypeRef = varType;
                        break;
                    }
                }
                if(VariableTypeRef){
                    VariableTypeRef->m_vec_Variables.push_back(newVariable);
                }else{
                    VariableTypeRef = new VariableType_class;
                    VariableTypeRef->Uuid  = QUuid().createUuidV7();
                    VariableTypeRef->Title = Type;
                    VariableTypeRef->m_vec_Variables.push_back(newVariable);
                    m_VariablesTree.m_vec_VariablesType.push_back(VariableTypeRef);
                }
                if(m_ptr_Variable == nullptr){
                    m_ptr_Variable = newVariable;
                }
            }else if(tokenName == TOKEN_SOURCE){
                QUuid   SourceUuid                  = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                if(m_ptr_Variable != nullptr){
                    m_ptr_Variable->m_vec_SignalSources.push_back(SourceUuid);
                }
            }else if(tokenName == TOKEN_ROOT_DEV){
                m_Controller.Uuid                   = QUuid(xml.attributes().value(ATTR_UUID).toString());
                m_Controller.IUuid                  = QUuid().createUuidV7();
                m_Controller.Title                  = xml.attributes().value(ATTR_TITLE).toString();
                m_Controller.IpAddress              = xml.attributes().value(ATTR_IP_ADDR).toString();
                m_Controller.PortHttp               = xml.attributes().value(ATTR_PORT_HTTP).toInt();
                m_Controller.PortHttps              = xml.attributes().value(ATTR_PORT_HTTPS).toInt();
                m_Controller.SerialNr               = xml.attributes().value(ATTR_SERIAL_NUM).toString();
                m_Controller.Model                  = xml.attributes().value(ATTR_DEV_MODEL).toString();
            }else if(S_PeripheryManager.isTagSection(tokenName.toString())){
                XmlNode_class *newNode = XmlNode_class::read(xml);
                m_Controller.m_vec_Periphery.append(newNode);
            }else if(tokenName == TOKEN_PROGRAM){

            }else if(tokenName == TOKEN_PAGE){
                if(m_ptr_Page == nullptr){
                    m_ptr_Page = new Page_class(this);
                    m_vec_Pages.push_back(m_ptr_Page);
                }
                if(m_ptr_Page){
                    m_ptr_Page->Title               = xml.attributes().value(ATTR_TITLE).toString();
                    m_ptr_Page->ModifiedDate        = xml.attributes().value(ATTR_MODIFIED_DATE).toString();
                }
            }else if(tokenName == TOKEN_SIGNAL_IN){

            }else if(tokenName == TOKEN_SIGNAL_OUT){

            }else if(tokenName == TOKEN_BLOCK){
                QString BlockTitle                  = xml.attributes().value(ATTR_TITLE).toString();
                QUuid   BlockUuid                   = QUuid(xml.attributes().value(ATTR_UUID).toString());
                QUuid   BlockInstanceUuid           = QUuid(xml.attributes().value(ATTR_IUUID).toString());
                m_ptr_BlockInstance = S_BlockManager.getInstance(BlockUuid);
                if(m_ptr_BlockInstance != nullptr){
                    m_ptr_BlockInstance->IUuid        = BlockInstanceUuid;
                    m_ptr_BlockInstance->Title        = BlockTitle;
                    m_ptr_BlockInstance->Position.setX( xml.attributes().value(ATTR_PX).toFloat());
                    m_ptr_BlockInstance->Position.setY( xml.attributes().value(ATTR_PY).toFloat());
                    m_ptr_BlockInstance->Width        = xml.attributes().value(ATTR_WIDTH).toFloat();
                    m_ptr_BlockInstance->Color        = xml.attributes().value(ATTR_COLOR).toString();
                    m_ptr_BlockInstance->Room         = xml.attributes().value(ATTR_ROOM).toString();
                    m_ptr_BlockInstance->Category     = xml.attributes().value(ATTR_CATEGORY).toString();

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
            }else if(tokenName == TOKEN_PAGE){
                m_ptr_Page = nullptr;
            }else if(tokenName == TOKEN_BLOCK){
                m_ptr_BlockInstance = nullptr;
            }
        }
    }

    if(xml.hasError()){
        S_Logger.write(Log::Warning, PROJECT_NAME, projectFileInfo.absoluteFilePath() + ": " + xml.errorString() + " line " + QString::number(xml.lineNumber()), "Project Manager", "Fix the XML", true);
        return false;
    }

    this->build();
    m_ProjectTree->build();
    return true;
}

Page_class *Project_class::getPage (int Index){
    if(Index < 0 || Index >= m_vec_Pages.size())
        return nullptr;
    return m_vec_Pages[Index];
}

Page_class *Project_class::takePage(int Index){
    if(Index < 0 || Index >= m_vec_Pages.size())
        return nullptr;
    Page_class *pageRef = m_vec_Pages.takeAt(Index);
    if(m_ptr_ActivePage == pageRef)
        m_ptr_ActivePage = nullptr;
    emit pagesChanged();
    emit pageActvChanged();
    return pageRef;
}

void Project_class::insertPage(int Index, Page_class *PageRef){
    if(Index < 0)                  Index = 0;
    if(Index > m_vec_Pages.size()) Index = m_vec_Pages.size();
    m_vec_Pages.insert(Index, PageRef);
    emit pagesChanged();
    emit pageActvChanged();
}

void Project_class::movePage(int From, int To){
    if(From < 0 || From >= m_vec_Pages.size()) return;
    if(To   < 0) To = 0;
    if(To   >= m_vec_Pages.max_size() - 1) To = m_vec_Pages.max_size() - 1;
    if(From == To) return;
    m_vec_Pages.move(From, To);
    emit pagesChanged();
    emit pageActvChanged();
}

int Project_class::cmdAddPage(int Index, const QString &title){
    if(Index < 0 || Index > m_vec_Pages.size()) Index = m_vec_Pages.size();
    Page_class *page = new Page_class(this);
    page->Title = title.isEmpty() ? tr("Page") : title;
    m_UndoStack.push(new AddPageCmd(this, page, Index));
    return Index;
}

void Project_class::cmdRemovePage(int index){
    if(index < 0 || index >= m_vec_Pages.size()) return;
    m_UndoStack.push(new RemovePageCmd(this, index));
}

void Project_class::cmdMovePage(int From, int To){
    if(From < 0 || From >= m_vec_Pages.size()) return;
    if(To   < 0) To = 0;
    if(To   >= m_vec_Pages.size()) To = m_vec_Pages.size() - 1;
    if(From == To) return;
    m_UndoStack.push(new MovePageCmd(this, From, To));
}

void Project_class::cmdRenamePage(int index, const QString &title){
    if(index < 0 || index >= m_vec_Pages.size()) return;
    Page_class *pageRef = m_vec_Pages[index];
    const QString newTitle = title.trimmed();
    if(newTitle.isEmpty() || newTitle == pageRef->Title) return;
    m_UndoStack.push(new RenamePageCmd(this, pageRef, newTitle));
}

int Project_class::getPageActv() const{
    if(m_ptr_ActivePage == nullptr)
        return -1;

    for(int i = 0; i < m_vec_Pages.size(); i++){
        if(m_ptr_ActivePage == m_vec_Pages[i]){
            return i;
        }
    }
    return -1;
}

Project_class::PaperFormat_e Project_class::fromString(const QString &format){
    if(format == "A4") return PaperFormat_e::PaperA4;
    if(format == "A3") return PaperFormat_e::PaperA3;
    if(format == "A2") return PaperFormat_e::PaperA2;
    if(format == "A1") return PaperFormat_e::PaperA1;
    if(format == "A0") return PaperFormat_e::PaperA0;
    return PaperMax;
}

QString Project_class::toString(const PaperFormat_e &format){
    QString Result = "Undefined";
    switch(format){
        case PaperA4: Result = "A4"; break;
        case PaperA3: Result = "A3"; break;
        case PaperA2: Result = "A2"; break;
        case PaperA1: Result = "A1"; break;
        case PaperA0: Result = "A0"; break;
        case PaperMax:               break;
        default:      Result = "A4";
    }
    return Result;
}

QSize Project_class::toSize(const PaperFormat_e &format){
    QSize PageSize = QSize(420, 297);
    switch(format){
    case PaperA4: PageSize = QSize(297, 210);  break;
    case PaperA3: PageSize = QSize(420, 297);  break;
    case PaperA2: PageSize = QSize(594, 420);  break;
    case PaperA1: PageSize = QSize(841, 594);  break;
    case PaperA0: PageSize = QSize(1189, 841); break;
    case PaperMax:                             break;
    default:      PageSize = QSize(420, 297);
    }
    return PageSize * WORKSPACE_SCALE;
}