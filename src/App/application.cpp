#include "application.h"

#include <QDir>
#include <QDirIterator>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStandardPaths>
#include <QQmlEngine>

#include "src/Components/Logger/logger.h"
#include "src/Components/SchemaManager/schemamanager.h"
#include "src/Components/BlockManager/blockmanager.h"
#include "src/Components/ProjectManager/projectmanager.h"

#include "../Setup/Setup.hpp"

#define APPLICATION_NAME "Application"

Application_class Application;

Application_class::Application_class() {

}

void Application_class::begin(){
    S_Logger.begin();

    loadConfig();

    S_SchemaManager.begin();
    S_BlockManager.begin();
    S_ProjectManager.begin();
}

void Application_class::firstConfig(){
    /* This function need to be done for first setup */
    const QString templatesPath = CACHE_APP_PATH + TEMPLATES_DIR_NAME;

    if(!QDir().exists(templatesPath + PERIPHERY_DIR_NAME)){
        QDir().mkpath(templatesPath + PERIPHERY_DIR_NAME);
    }
    if(!QDir().exists(templatesPath + BLOCKS_DIR_NAME)){
        QDir().mkpath(templatesPath + BLOCKS_DIR_NAME);
    }
    if(!QDir().exists(templatesPath + SCHEMA_DIR_NAME)){
        QDir().mkpath(templatesPath + SCHEMA_DIR_NAME);
    }
    if(!QDir().exists(PROJECTS_PATH + ApplicationName)){
        QDir().mkpath(PROJECTS_PATH + ApplicationName);
    }

    saveConfig();
}

void Application_class::loadConfig(){
    const QString configDirPath = CACHE_APP_PATH + CONFIG_DIR_NAME;
    QDir().mkpath(configDirPath);

    const QString appConfigFilePath = configDirPath + "/" + CONFIG_FILE_NAME;
    QFile appConfigFile(appConfigFilePath);

    if(!appConfigFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        S_Logger.write(Log::Warning, APPLICATION_NAME, "Cannot open app config", "Error on loading cache", "Trying to create new file", true);
        firstConfig();
        return;
    }
    QXmlStreamReader configXml(&appConfigFile);
    while(!configXml.atEnd() && !configXml.hasError()){
        QXmlStreamReader::TokenType token     = configXml.readNext();
        QStringView                 tokenName = configXml.name();
        if(token == QXmlStreamReader::StartElement){
            if(tokenName == "Application"){
                Language = fromString(configXml.attributes().value("Lang").toString());
            }
        }else if(token == QXmlStreamReader::EndElement){

        }
    }
    if(configXml.hasError()){
        S_Logger.write(Log::Error, APPLICATION_NAME, "App Config xml error: " + configXml.errorString() + " :" + QString::number(configXml.lineNumber()) + ":" + QString::number(configXml.columnNumber()), "App Config loading.", "", true);
        return;
    }
}

void Application_class::saveConfig(){
    const QString configDirPath = CACHE_APP_PATH + CONFIG_DIR_NAME;
    QDir().mkpath(configDirPath);

    const QString appConfigFilePath = configDirPath + "/" + CONFIG_FILE_NAME;
    QFile appConfigFile(appConfigFilePath);

    if(!appConfigFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        S_Logger.write(Log::Warning, APPLICATION_NAME, "Cannot write app config.", "Error on writing cache", "Check permissions", true);
        return;
    }
    QXmlStreamWriter configXml(&appConfigFile);

    configXml.setAutoFormatting(true);
    configXml.writeStartDocument();

    configXml.writeStartElement("Application");
    configXml.writeAttribute("Lang",    toString(Language));
    configXml.writeEndElement();

    configXml.writeEndDocument();

    appConfigFile.close();
}

Application_class::Language_e Application_class::fromString(const QString &lang){
    if(lang == "EN") return Language_e::Lang_EN;
    if(lang == "RU") return Language_e::Lang_RU;
    if(lang == "RO") return Language_e::Lang_RO;
    if(lang == "ES") return Language_e::Lang_ES;
    return Language_e::Lang_EN;
}

QString Application_class::toString(const Language_e &lang){
    switch(lang){
        case Language_e::Lang_EN: return "EN";
        case Language_e::Lang_RU: return "RU";
        case Language_e::Lang_RO: return "RO";
        case Language_e::Lang_ES: return "ES";
    }
    return "EN";
}