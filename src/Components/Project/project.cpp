#include "project.h"

#include <QDir>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QDateTime>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QRegularExpression>

#include "project_tokens.h"
#include "project_commands.h"
#include "../Logger/logger.h"
#include "src/Components/Document/document_tokens.h"

#define PROJECT_NAME "Project"
#define DATE_FORMAT  "yyyy-MM-dd HH:mm:ss"

/* ================= Local helpers ================= */

QString nowString(){
    return QDateTime::currentDateTime().toString(DATE_FORMAT);
}

/* ================================================== */

Project_class::Project_class(QObject *parent) : QObject(parent) {
    m_ptr_Tree = new ProjectTree_class(this);

    connect(&m_UndoStack, &QUndoStack::cleanChanged,   this, &Project_class::modifiedChanged);
    connect(this,         &Project_class::pathChanged, this, &Project_class::modifiedChanged);
    connect(&m_UndoStack, &QUndoStack::indexChanged,   this, &Project_class::undoStateChanged);
    connect(&m_UndoStack, &QUndoStack::canUndoChanged, this, &Project_class::undoStateChanged);
    connect(&m_UndoStack, &QUndoStack::canRedoChanged, this, &Project_class::undoStateChanged);
}

Project_class::~Project_class(){
    m_UndoStack.clear();
    qDeleteAll(m_hash_PageByIUuid);
    m_hash_PageByIUuid.clear();
    if(m_ptr_Root){
        delete m_ptr_Root;
        m_ptr_Root = nullptr;
    }
}

void Project_class::insertNode(XmlNode_class *parent, XmlNode_class *node, int row){
    parent->insertChild(node, row);

    indexNode(node);
    touchPage(node);

    emit nodeInserted(parent, node);
    emit selectionChanged();
}

void Project_class::takeNode(XmlNode_class *node, bool moving){
    XmlNode_class *parent = node->parent();
    if(parent == nullptr)
        return; // Node without parent is only root
    touchPage(node);
    unIndexNode(node);
    parent->takeChild(node);

    emit nodeRemoved(parent, node);
    emit selectionChanged();
}

void Project_class::setAttribute(XmlNode_class *node,   const QString &tag, const QString &value){
    node->setValue(tag, value);

    if(tag != ATTR_MODIFIED_DATE){
        touchPage(node);
    }
    emit attributeChanged(node, tag);
    if(node == m_ptr_Selected)
        emit selectionChanged();    // refresh properties panel only when it shows this node
}

void Project_class::rmAttribute(XmlNode_class *node,   const QString &tag){
    node->removeAttr(tag);

    emit attributeChanged(node, tag);
    if(node == m_ptr_Selected)
        emit selectionChanged();
}

void Project_class::indexNode(XmlNode_class *node){
    auto indexFunc = [this](XmlNode_class *node){
        const QUuid id = QUuid(node->value(ATTR_IUUID));
        if(!id.isNull()){
            m_hash_NodeByIUuid[id] = node;
        }
    };
    node->visit(indexFunc);
}

void Project_class::unIndexNode(XmlNode_class *node){
    auto indexFunc = [this](XmlNode_class *node){
        const QUuid id = QUuid(node->value(ATTR_IUUID));

        if(!id.isNull() && m_hash_NodeByIUuid.value(id) == node){
            m_hash_NodeByIUuid.remove(id);
        }
    };
    node->visit(indexFunc);
}

void Project_class::touchPage(XmlNode_class *node){
    if(node == nullptr)
        return;
    XmlNode_class *nodePage = node->ancestor(TOKEN_PAGE);
    if(nodePage == nullptr || nodePage->parent() == nullptr)
        return;
    nodePage->setValue(ATTR_MODIFIED_DATE, nowString());
    emit attributeChanged(nodePage, ATTR_MODIFIED_DATE);
}

QString Project_class::uniqueTitle(XmlNode_class *parent, const QString &base){
    if(parent == nullptr){
        return base;
    }
    QSet<QString> nameUsed;
    for(const auto *child : parent->children())
        nameUsed.insert(child->value(ATTR_TITLE));
    if(!nameUsed.contains(base))
        return base;
    int index = 1;
    QString candidate = base + "_" + QString::number(index);
    while(nameUsed.contains(candidate)){
        index++;
        candidate = base + "_" + QString::number(index);
    }
    return candidate;
}

void Project_class::setDocument(XmlNode_class *root){
    m_UndoStack.clear();

    if(m_ptr_Root){
        delete m_ptr_Root;
        m_ptr_Root = nullptr;
    }

    m_ptr_Root     = root;
    m_ptr_Selected = nullptr;
    indexNode(m_ptr_Root);

    for(auto &child : m_ptr_Root->children()){
        if(child->TagName == TOKEN_PROGRAM){
            m_ptr_Program = child;
        }
    }

    emit documentReset();
    emit titleChanged();
    emit infoChanged();
    emit pagesChanged();
    emit pageActvChanged();
    emit selectedUuidChanged();
    emit selectedIUuidChanged();
    emit selectionChanged();
    emit undoStateChanged();
    emit modifiedChanged();
}

XmlNode_class *Project_class::createNode(XmlNode_class *parent, const NodeType_s *nodeTemplate){
    if(nodeTemplate == nullptr){
        return nullptr;
    }
    XmlNode_class *newNode = new XmlNode_class();
    newNode->TagName = nodeTemplate->TagName;
    newNode->setValue(ATTR_UUID,    nodeTemplate->Uuid.toString(QUuid::WithoutBraces));
    newNode->setValue(ATTR_IUUID,   QUuid().createUuidV7().toString(QUuid::WithoutBraces));
    newNode->setValue(ATTR_TITLE,   nodeTemplate->Title);

    for(auto &property : nodeTemplate->Properties){
        if(property.TagName == ATTR_TITLE){
            newNode->setValue(ATTR_TITLE, uniqueTitle(parent, nodeTemplate->Title));
        }else{
            newNode->setValue(property.TagName, property.Default);
        }
    }
    for(auto &child    : nodeTemplate->Children){
        const NodeType_s *childTemplate = S_SchemaManager.getNode(child.Uuid);
        if(child.Mandatory && childTemplate){ /* If child needs to be auto added */
            newNode->appendChild(createNode(newNode, childTemplate));
        }
    }
    return newNode;
}

/* ===== Document access (views / C++) ===== */
XmlNode_class *Project_class::getRoot() const {
    return m_ptr_Root;
}

XmlNode_class *Project_class::getNode(const QUuid   &iUuid) const  {
    return m_hash_NodeByIUuid.value(iUuid, nullptr);
}

XmlNode_class *Project_class::getNode(const QString &iUuid) const  {
    if(iUuid.isEmpty())
        return nullptr;
    return getNode(QUuid(iUuid));
}

/* ===== File ===== */
bool Project_class::newDocument(const QString &title){
    const NodeType_s *projectTemplate = S_SchemaManager.getNode(TOKEN_PROJECT);
    if(projectTemplate == nullptr){
        S_Logger.write(Log::Error, PROJECT_NAME, "Error on loading project template", "Project creation fail", "Check templates, or reinstall app", true);
        return false;
    }
    XmlNode_class *rootNode = createNode(nullptr, projectTemplate);
    rootNode->setValue(ATTR_TITLE,          title);
    rootNode->setValue(ATTR_CREATION_DATE,  nowString());
    rootNode->setValue(ATTR_MODIFIED_DATE,  nowString());

    m_str_ProjectPath.clear();
    m_str_ProjectDir.clear();

    setDocument(rootNode);
    emit pathChanged();
    return true;
}

bool Project_class::load(const QString &projectPath){
    QFile projectFile(projectPath);
    if(!projectFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        S_Logger.write(Log::Error, PROJECT_NAME, tr("Cannot open project: ") + projectFile.errorString(), "Loading Error", "Check file permission", true);
        return false;
    }
    QXmlStreamReader xml(&projectFile);
    XmlNode_class   *root = XmlNode_class::readDoc(xml);
    if(root == nullptr){
        S_Logger.write(Log::Error, PROJECT_NAME, projectPath + ": " + xml.errorString() + " line " + QString::number(xml.lineNumber()), "Loading Error", "Check file integrity", true);
        return false;
    }
    if(root->TagName != TOKEN_PROJECT){
        S_Logger.write(Log::Error, PROJECT_NAME, projectPath + tr(": the root element is not <Project>"), "Loading Error", "Check file integrity", true);
        delete root;
        return false;
    }
    m_str_ProjectPath = projectPath;
    m_str_ProjectDir  = QFileInfo(projectPath).dir().path();

    setDocument(root);

    emit pathChanged();
    return true;
}

bool Project_class::save(){
    if(m_ptr_Root == nullptr) return false;
    if(m_str_ProjectPath.isEmpty()){
        emit saveAsRequested();
        return false;
    }
    QDir().mkpath(m_str_ProjectDir);
    if(QFile::exists(m_str_ProjectPath)){
        const QString backupPath = m_str_ProjectPath + ".bak";
        QFile::remove(backupPath);
        QFile::copy(m_str_ProjectPath, backupPath);
    }

    const QString previousDate = m_ptr_Root->value(ATTR_MODIFIED_DATE);
    m_ptr_Root->setValue(ATTR_MODIFIED_DATE, nowString());

    QSaveFile projectFile(m_str_ProjectPath);
    if(!projectFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        m_ptr_Root->setValue(ATTR_MODIFIED_DATE, previousDate);
        S_Logger.write(Log::Error, PROJECT_NAME, tr("Cannot write project: ") + projectFile.errorString(), "Saving Error", "Check file permissino", true);
        return false;
    }
    QXmlStreamWriter xml(&projectFile);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(4);
    xml.writeStartDocument();
    m_ptr_Root->write(xml);
    xml.writeEndDocument();

    if(xml.hasError() || !projectFile.commit()){
        m_ptr_Root->setValue(ATTR_MODIFIED_DATE, previousDate);
        S_Logger.write(Log::Error, PROJECT_NAME, tr("Cannot write project: ") + projectFile.errorString(), "Saving Error", "Check file integrity", true);
        return false;
    }

    m_UndoStack.setClean();
    emit infoChanged();
    return true;
}

bool Project_class::saveAs(const QString &folderPath){
    if(m_ptr_Root == nullptr) return false;
    QString folderName = m_ptr_Root->value(ATTR_TITLE).trimmed();
    folderName.replace(QRegularExpression(QStringLiteral("[\\\\/:*?\"<>|]")), QStringLiteral("_"));
    if(folderName.isEmpty())
        folderName = QStringLiteral("Project");
    const QString projectFolder = QDir(folderPath).filePath(folderName);
    const QString projectFile   = QDir(projectFolder).filePath(QStringLiteral("Project.xml"));
    if(QFile::exists(projectFile)){
        S_Logger.write(Log::Error, PROJECT_NAME, tr("A project already exists in ") + projectFolder, "SavingAs Error", "Select diferent directory", true);
        return false;
    }
    if(!QDir().mkpath(projectFolder)){
        S_Logger.write(Log::Error, PROJECT_NAME, tr("Cannot create folder ") + projectFolder, "SavingAs Error", "Select diferent directory", true);
        return false;
    }
    m_str_ProjectPath = projectFile;
    m_str_ProjectDir  = projectFolder;
    emit pathChanged();
    return save();
}

bool    Project_class::isModified()  const { return !m_UndoStack.isClean();       }
bool    Project_class::hasFile()     const { return !m_str_ProjectPath.isEmpty(); }
bool    Project_class::canUndo()     const { return m_UndoStack.canUndo();        }
bool    Project_class::canRedo()     const { return m_UndoStack.canRedo();        }
QString Project_class::undoText()    const { return m_UndoStack.undoText();       }
QString Project_class::redoText()    const { return m_UndoStack.redoText();       }

int Project_class::pageCount()       const{
    if(m_ptr_Program == nullptr)
        return 0;
    return m_ptr_Program->children().size();
}
int Project_class::pageActv()         const{
    if(m_ptr_Program == nullptr || m_ptr_ActivePage == nullptr)
        return -1;

    int index = 0;
    for(auto &child : pages()){
        if(child == m_ptr_ActivePage)
            return index;
        index++;
    }
    return -1;
}

QVector<XmlNode_class *> Project_class::pages() const{
    if(m_ptr_Program == nullptr) return QVector<XmlNode_class *>();
    return m_ptr_Program->children();
}

QVariantList Project_class::pageList(){
    QVariantList Result;
    int index = 0;
    if(m_ptr_Program == nullptr)
        return Result;

    auto toObject = [this](XmlNode_class *pageNode) -> Page_class *{
        if(pageNode == nullptr)
            return nullptr;
        QUuid instanceUuid = QUuid(pageNode->value(ATTR_IUUID));
        Page_class *findedPage = m_hash_PageByIUuid.value(instanceUuid, nullptr);
        if(findedPage == nullptr){
            findedPage = new Page_class(this, pageNode);
            m_hash_PageByIUuid[instanceUuid] = findedPage;
        }
        return findedPage;
    };
    for(auto &child : pages()){
        if(child->TagName == TOKEN_PAGE){
            QVariantMap pageMap;
            pageMap["iUuid"]    = child->value(ATTR_IUUID);
            pageMap["title"]    = child->value(ATTR_TITLE);
            pageMap["mDate"]    = child->value(ATTR_MODIFIED_DATE);
            pageMap["index"]    = index;
            pageMap["number"]   = index + 2;
            pageMap["pageRef"]  = QVariant::fromValue<QObject *>(toObject(child));
            index++;
            Result.append(pageMap);
        }
    }
    return Result;
}
QVariantMap Project_class::info()          const{
    QVariantMap Result;
    if(m_ptr_Root == nullptr) return Result;

    auto resolveUrl = [this](const QString &path) -> QString {
        if(path.isEmpty()) return QString();
        if(path.startsWith("qrc:") || path.startsWith("file:") || path.startsWith("http")) return path;
        const QString absolute = QFileInfo(path).isAbsolute() ? path : QDir(m_str_ProjectDir).filePath(path);
        return QUrl::fromLocalFile(absolute).toString();
    };

    auto paperFormatToSize = [this](const QString &format) -> QSize {
        if(format == "A4") return QSize(297, 210);
        if(format == "A3") return QSize(420, 297);
        if(format == "A2") return QSize(594, 420);
        if(format == "A1") return QSize(841, 594);
        if(format == "A0") return QSize(1189, 841);
        return QSize(420, 297);
    };

    const XmlNode_class *root = m_ptr_Root;
    if(root == nullptr) return Result;
    Result["title"]      = root->value(ATTR_TITLE);
    Result["configVers"] = root->value(ATTR_CONF_VERS);
    Result["cDate"]      = root->value(ATTR_CREATION_DATE);
    Result["mDate"]      = root->value(ATTR_MODIFIED_DATE);
    Result["author"]     = root->value(ATTR_AUTHOR);
    Result["street"]     = root->value(ATTR_STREET);
    Result["town"]       = root->value(ATTR_TOWN);
    Result["postCode"]   = root->value(ATTR_POSTCODE);
    Result["country"]    = root->value(ATTR_COUNTRY);
    Result["latitude"]   = root->value(ATTR_LATITUDE);
    Result["longitude"]  = root->value(ATTR_LONGITUDE);
    Result["timezone"]   = root->value(ATTR_TIMEZONE);
    Result["paperFormat"]= root->value(ATTR_PAPER_FORMAT, "A3");
    Result["paperSize"]  = paperFormatToSize(root->value(ATTR_PAPER_FORMAT, "A3")) * WORKSPACE_SCALE;

    XmlNode_class *CompanyRef = nullptr;
    XmlNode_class *ClientRef  = nullptr;
    for(auto &child : root->children()){
        if(child->TagName == TOKEN_COMPANY)
            CompanyRef = child;
        else if(child->TagName == TOKEN_CLIENT){
            ClientRef  = child;
        }
        if(CompanyRef && ClientRef)
            break;
    }
    QVariantMap Company;
    QVariantMap Client;
    if(CompanyRef){
        Company["iUuid"]    = CompanyRef->value(ATTR_IUUID);
        Company["name"]     = CompanyRef->value(ATTR_NAME);
        Company["address"]  = CompanyRef->value(ATTR_ADDRESS);
        Company["phone"]    = CompanyRef->value(ATTR_PHONE);
        Company["email"]    = CompanyRef->value(ATTR_EMAIL);
        Company["web"]      = CompanyRef->value(ATTR_WEB);
        Company["logoUrl"]  = resolveUrl(CompanyRef->value(ATTR_LOGO_PATH));
    }
    if(ClientRef){
        Client["iUuid"]    = ClientRef->value(ATTR_IUUID);
        Client["name"]     = ClientRef->value(ATTR_NAME);
        Client["address"]  = ClientRef->value(ATTR_ADDRESS);
        Client["phone"]    = ClientRef->value(ATTR_PHONE);
        Client["email"]    = ClientRef->value(ATTR_EMAIL);
    }
    Result["company"]   = Company;
    Result["client"]    = Client;
    return Result;
}

QObject *Project_class::tree() const{
    return m_ptr_Tree;
}

QString  Project_class::selectedUuid() const{
    if(m_ptr_Selected){
        const NodeType_s *nodeTemplate = S_SchemaManager.getNode(m_ptr_Selected->TagName);
        if(nodeTemplate)
            return nodeTemplate->Uuid.toString(QUuid::WithoutBraces);
        else
            return m_ptr_Selected->value(ATTR_UUID);
    }
    return QString();
}

QString  Project_class::selectedIUuid() const{
    if(m_ptr_Selected)
        return m_ptr_Selected->value(ATTR_IUUID);
    return QString();
}

QVariantMap Project_class::selection() const{ // Needed for properties menu
    QVariantMap Result;
    if(m_ptr_Selected == nullptr) return Result;

    const NodeType_s *nodeTemplate = S_SchemaManager.getNode(m_ptr_Selected);

    QVariantMap instanceNodeMap;
    instanceNodeMap["iUuid"]     = m_ptr_Selected->value(ATTR_IUUID);
    instanceNodeMap["title"]     = m_ptr_Selected->value(ATTR_TITLE);
    instanceNodeMap["editable"]  = nodeTemplate ? nodeTemplate->Editable : false;
    Result["instance"] = instanceNodeMap;

    QVariantMap templateNodeMap;
    if(nodeTemplate){
        templateNodeMap["uuid"]        = nodeTemplate->Uuid.toString(QUuid::WithoutBraces);
        templateNodeMap["title"]       = nodeTemplate->Title;
        templateNodeMap["icon"]        = nodeTemplate->Icon;
        templateNodeMap["description"] = nodeTemplate->Description;

        QString menuName = nodeTemplate->MenuName;
        if(!nodeTemplate->Children.isEmpty() && menuName.isEmpty())
            menuName = tr("Add");
        templateNodeMap["menuName"]    = menuName;

        bool menuVisible = false;
        QVariantList childList;
        for(auto &childDef : nodeTemplate->Children){
            const NodeType_s *childTemplate = S_SchemaManager.getNode(childDef.Uuid);
            if(!childTemplate) continue;

            QVariantMap childMap;
            childMap["uuid"]        = childTemplate->Uuid.toString(QUuid::WithoutBraces);;
            childMap["title"]       = childTemplate->Title;
            childMap["icon"]        = childTemplate->Icon;
            childMap["description"] = childTemplate->Description;
            childMap["visible"]     = childDef.Visible;
            if(childDef.Visible == false) //if we have invisible child
                menuVisible = true;
            childMap["enabled"]     = nodeTemplate->Max != -1 ? (m_ptr_Selected->children().size() < nodeTemplate->Max) : true;
            childList.append(childMap);
        }
        templateNodeMap["menuVisible"] = menuVisible;
        templateNodeMap["items"]       = childList;
    }else{
        templateNodeMap["uuid"]        = QUuid().toString(QUuid::WithoutBraces);
        templateNodeMap["title"]       = tr("Missing config");
        templateNodeMap["icon"]        = QStringLiteral("qrc:/icons/misc/unknown.svg");
        templateNodeMap["description"] = "Signal not found in DB";
        templateNodeMap["menuName"]    = QString();
        templateNodeMap["menuVisible"] = false;
        templateNodeMap["items"]       = QVariantList();
    }
    Result["template"] = templateNodeMap;

    QVariantList propertiesList;
    if(nodeTemplate){
        for(auto &property : nodeTemplate->Properties){
            QVariantMap propertyMap;
            propertyMap["tag"]   = property.TagName;
            propertyMap["name"]  = property.Title;
            propertyMap["value"] = m_ptr_Selected->value(property.TagName);
            propertiesList.append(propertyMap);
        }
    }else{
        for(auto &attribute: m_ptr_Selected->attributes()){
            QVariantMap propertyMap;
            propertyMap["tag"]   = attribute->Name;
            propertyMap["name"]  = attribute->Name;
            propertyMap["value"] = attribute->Value;
            propertiesList.append(propertyMap);
        }
    }
    Result["properties"] = propertiesList;
    return Result;
}

QString Project_class::getPath() const{
    return m_str_ProjectPath;
}

QString Project_class::getDir() const{
    return m_str_ProjectDir;
}

QString Project_class::getTitle() const{
    if(m_ptr_Root)
        return m_ptr_Root->value(ATTR_TITLE);
    return tr("Unnamed");
}

void Project_class::setPageActv(int index){
    if(index < 0 || index >= pageCount()){
        m_ptr_ActivePage = nullptr;
    }else{
        m_ptr_ActivePage = pages().at(index);
    }
    emit pageActvChanged();
}

void Project_class::setSelectedIUuid(const QString &iUuid){
    XmlNode_class *selected = getNode(iUuid);
    if(selected == m_ptr_Selected) return;
    m_ptr_Selected = selected;
    emit selectionChanged();
    emit selectedIUuidChanged();
    emit selectedUuidChanged();
}

void Project_class::undo(){ m_UndoStack.undo(); }

void Project_class::redo(){ m_UndoStack.redo(); }

int Project_class::cmdAddPage(int index, const QString &title){
    if(m_ptr_Program == nullptr) return -1;
    if(index < 0 || index > pageCount())
        index = pageCount();

    XmlNode_class    *newPage      = new XmlNode_class(TOKEN_PAGE);
    newPage->setValue(ATTR_IUUID,           QUuid().createUuidV7().toString(QUuid::WithoutBraces));
    newPage->setValue(ATTR_TITLE,           uniqueTitle(m_ptr_Program, title));
    newPage->setValue(ATTR_MODIFIED_DATE,   nowString());

    m_UndoStack.push(new InsertNodeCmd(this, m_ptr_Program, index, newPage, tr("Add page %1").arg(newPage->value(ATTR_TITLE))));
    setPageActv(index);
    emit pagesChanged();
    return pageActv();
}

void Project_class::cmdRemovePage(int index){
    int currentIndex = 0;
    if(index < 0 || index >= pageCount())
        return;
    XmlNode_class *selectedNode = m_ptr_Program->children()[index];
    m_UndoStack.push(new RemoveNodeCmd(this, selectedNode, tr("Rm page %1").arg(selectedNode->value(ATTR_TITLE))));
    if(m_ptr_ActivePage == selectedNode){
        setPageActv(qMin(index, pageCount() - 1));
    }
    emit pagesChanged();
    emit pageActvChanged();
}

void Project_class::cmdMovePage(int From, int To){
    int pageCnt = pageCount();
    if(From < 0 || From >= pageCnt) return;
    To = qBound(0, To, pageCnt-1);
    if(From == To) return;

    XmlNode_class *takePage = m_ptr_Program->takeChild(pages().at(From));
    m_ptr_Program->insertChild(takePage, To);

    emit pagesChanged();
    emit pageActvChanged();
}

void Project_class::cmdRenamePage(int index, const QString &title){
    const QString newTitle = title.trimmed();
    int pageCnt = pageCount();
    if(index < 0 || index >= pageCnt) return;
    XmlNode_class *selectedNode = m_ptr_Program->children()[index];
    if(newTitle.isEmpty() || newTitle == selectedNode->value(ATTR_TITLE)) return;

    m_UndoStack.push(new SetAttributeCmd(this, selectedNode, ATTR_TITLE, newTitle, tr("Rename Page")));
    emit pagesChanged();
}

void Project_class::cmdAddChild(const QString &targetIUuid, const QString &templateUuid) {
    const NodeType_s *templateNode   = S_SchemaManager.getNode(QUuid(templateUuid));
    XmlNode_class    *parentNode   = getNode(targetIUuid);
    const NodeType_s *templateParent = S_SchemaManager.getNode(parentNode);
    if(targetIUuid.isEmpty() && m_ptr_Selected){
        parentNode = m_ptr_Selected;
    }
    if(parentNode == nullptr || templateNode == nullptr)     return;
    if(parentNode->children().size() >= templateParent->Max && templateParent->Max != -1) return;
    XmlNode_class   *newNode = createNode(parentNode, templateNode);

    m_UndoStack.push(new InsertNodeCmd(this, parentNode, -1, newNode, tr("Add %1").arg(newNode->TagName)));
    setSelectedIUuid(newNode->value(ATTR_IUUID));
    emit selectedIUuidChanged();
}

bool Project_class::cmdRemove(const QString &targetIUuid){
    XmlNode_class *targetNode = getNode(targetIUuid);
    if(targetNode == nullptr) return false;

    m_UndoStack.push(new RemoveNodeCmd(this, targetNode, tr("Delete %1").arg(targetNode->value(ATTR_VALUE))));
    return true;
}

void Project_class::cmdRename(const QString &targetIUuid, const QString &newTitle){
    XmlNode_class *targetNode = getNode(targetIUuid);
    if(targetNode == nullptr) return;
    if(newTitle.isEmpty())    return;
    m_UndoStack.push(new SetAttributeCmd(this, targetNode, ATTR_TITLE, newTitle.trimmed(), tr("Rename %1").arg(newTitle)));
}

void Project_class::cmdSetProperty(const QString &targetIUuid, const QString &name, const QVariant &value){
    XmlNode_class *targetNode = getNode(targetIUuid);
    if(targetNode == nullptr || name.isEmpty())
        return;
    QString strValue;
    if(value.typeId() == QMetaType::Bool){
        strValue = value.toBool() ? "true" : "false";
    }else{
        strValue = value.toString();
    }
    m_UndoStack.push(new SetAttributeCmd(this, targetNode, name, strValue));
}