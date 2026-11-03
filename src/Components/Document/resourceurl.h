#ifndef RESOURCEURL_H
#define RESOURCEURL_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

#include "../Logger/logger.h"

#define RESOURCE_URL_NAME "RESOURCE_URL"

QString resolveResourceUrl(const QString &path, const QString &descriptorFilePath){
    if(path.isEmpty())
        return path;
    if(path.startsWith(QStringLiteral("http:")) || path.startsWith(QStringLiteral("https:")) || path.startsWith(QStringLiteral("file:")))
        return path;

    QString localName = path;
    if(path.startsWith(QStringLiteral("qrc:"))){
        if(QFile::exists(path.mid(3)))          // "qrc:/icons/x.svg" -> ":/icons/x.svg"
            return path;
        localName = QFileInfo(path.mid(4)).fileName();
    }else if(path.startsWith(QStringLiteral(":/"))){
        if(QFile::exists(path))
            return QStringLiteral("qrc") + path;
        localName = QFileInfo(path).fileName();
    }

    const QDir    descriptorDir = QFileInfo(descriptorFilePath).absoluteDir();
    const QString localPath     = QDir::isAbsolutePath(localName) ? localName : descriptorDir.filePath(localName);
    if(QFile::exists(localPath))
        return QUrl::fromLocalFile(QFileInfo(localPath).absoluteFilePath()).toString();

    S_Logger.write(Log::Warning, RESOURCE_URL_NAME, descriptorFilePath + ": file not found: " + path);
    return QString();
}


#endif // RESOURCEURL_H
