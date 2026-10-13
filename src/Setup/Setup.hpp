#ifndef SETUP_HPP
#define SETUP_HPP

#include <QStandardPaths>

#define CACHE_APP_PATH     QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
#define PROJECTS_PATH      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
#define CONFIG_DIR_NAME    "/config"
#define TEMPLATES_DIR_NAME "/templates"
#define PERIPHERY_DIR_NAME "/periphery"
#define BLOCKS_DIR_NAME    "/blocks"
#define SCHEMA_DIR_NAME    "/schema"

#define CONFIG_FILE_NAME   "Config.xml"
#define RECENT_FILE_NAME   "Recent.xml"

#endif // SETUP_HPP
