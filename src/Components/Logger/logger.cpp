#include "logger.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

Logger_class S_Logger;

QtMessageHandler Logger_class::s_prevHandler = nullptr;

// Nivel fortat pentru Log::Critical (Qt nu are nivel "critical" non-fatal separat de error)
static thread_local const char *t_levelOverride = nullptr;
// Protectie la recursivitate (daca QFile insusi arunca un warning)
static thread_local bool t_inHandler = false;

static const char *levelName(QtMsgType t){
    switch(t){
        case QtDebugMsg:    return "DEBUG";
        case QtInfoMsg:     return "INFO ";
        case QtWarningMsg:  return "WARN ";
        case QtCriticalMsg: return "ERROR";
        case QtFatalMsg:    return "FATAL";
    }
    return "?????";
}

Logger_class::Logger_class(QObject *parent) : QObject(parent) {
}

Logger_class::~Logger_class(){
    // S_Logger e global -> se distruge dupa main(); scoatem handler-ul inainte
    if(s_prevHandler){
        qInstallMessageHandler(s_prevHandler);
        s_prevHandler = nullptr;
    }
    QMutexLocker lock(&m_mutex);
    if(m_file.isOpen()){
        m_file.write("===== Session end =====\n\n");
        m_file.close();
    }
}

void Logger_class::begin(int keepDays){
    // Format consola: 14:06:01.123 [warning] BlockManager: mesaj
    qSetMessagePattern("%{time hh:mm:ss.zzz} [%{type}] %{message}");

    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs";
    QDir().mkpath(dir);
    cleanupOldLogs(dir, keepDays);

    // Un fisier pe zi, sesiunile se adauga la final
    m_str_FilePath = dir + "/" + QDate::currentDate().toString("yyyy-MM-dd") + ".log";
    m_file.setFileName(m_str_FilePath);

    if(!m_file.open(QIODevice::WriteOnly | QIODevice::Append)){
        qWarning().noquote() << "Logger: nu pot deschide" << m_str_FilePath << "-" << m_file.errorString();
    } else {
        const QByteArray header = "===== Session start " +
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8() + " | " +
            QCoreApplication::applicationName().toUtf8() + " " +
            QCoreApplication::applicationVersion().toUtf8() + " =====\n";
        m_file.write(header);
        m_file.flush();
    }

    // De aici inainte TOT ce trece prin qInfo/qWarning/qCritical (inclusiv erorile QML) ajunge in fisier
    s_prevHandler = qInstallMessageHandler(messageHandler);

    qInfo().noquote() << "Logger: fisier log ->" << m_str_FilePath;
}

void Logger_class::messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg){
    if(!t_inHandler && type != QtDebugMsg){
        t_inHandler = true;
        S_Logger.writeToFile(t_levelOverride ? t_levelOverride : levelName(type), msg);
        t_inHandler = false;
    }
    // Consola (handler-ul default Qt). La QtFatalMsg face abort -> fisierul e deja scris mai sus.
    if(s_prevHandler){
        s_prevHandler(type, ctx, msg);
    }
}

void Logger_class::writeToFile(const char *level, const QString &msg){
    QMutexLocker lock(&m_mutex);
    if(!m_file.isOpen()){
        return;
    }
    QByteArray line;
    line.reserve(msg.size() + 40);
    line += QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    line += " [";
    line += level;
    line += "] ";
    line += msg.toUtf8();
    line += '\n';
    m_file.write(line);
    m_file.flush();   // flush la fiecare linie -> nu pierdem nimic daca aplicatia cade
}

void Logger_class::cleanupOldLogs(const QString &dir, int keepDays){
    if(keepDays <= 0){
        return;
    }
    const QDateTime limit = QDateTime::currentDateTime().addDays(-keepDays);
    const QFileInfoList files = QDir(dir).entryInfoList({"*.log"}, QDir::Files);
    for(const QFileInfo &fi : files){
        if(fi.lastModified() < limit){
            QFile::remove(fi.absoluteFilePath());
        }
    }
}

void Logger_class::write(int Type, const QString &Component, const QString &Message,
                         const QString &Title, const QString &Tips, bool showTab){
    const QString text = Title.isEmpty()
                       ? Component + ": " + Message
                       : Component + ": [" + Title + "] " + Message;

    switch(Type){
        case Log::Debug:
            qDebug().noquote() << text;
            break;
        case Log::Info:
            qInfo().noquote() << text;
            break;
        case Log::Warning:
            qWarning().noquote() << text;
            break;
        case Log::Error:
            qCritical().noquote() << text;
            break;
        case Log::Critical:
            t_levelOverride = "CRIT ";
            qCritical().noquote() << text;
            t_levelOverride = nullptr;
            break;
        default:
            qWarning().noquote() << "Logger: tip necunoscut" << Type << "-" << text;
            break;
    }

    if(showTab){
        const QString popupTitle = Title.isEmpty() ? Component : Component + ": " + Title;
        emit notify(Type, popupTitle, Message, Tips);   // din alt thread -> queued automat catre QML
    }
}
