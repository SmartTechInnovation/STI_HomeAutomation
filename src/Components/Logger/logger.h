#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QString>

/* ==== Nivele de log ====
 * C++ : S_Logger.write(Log::Error, "BlockManager", "Mesaj", "Titlu", "Sfat", true);
 * QML : Logger.write(Log.Error, "Page", "Mesaj", "Titlu", "", true)
 */
namespace Log {
    Q_NAMESPACE
    enum Type_e {
        Debug,
        Info,
        Warning,
        Error,
        Critical,
    };
    Q_ENUM_NS(Type_e)
}

class Logger_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath CONSTANT)

public:
    explicit Logger_class(QObject *parent = nullptr);
    ~Logger_class() override;

    // Se apeleaza dupa QGuiApplication + setOrganizationName/setApplicationName
    void begin(int keepDays = 14);

    QString filePath() const { return m_str_FilePath; }

    // showTab = true -> popup in dreapta-jos (verde/galben/rosu)
    Q_INVOKABLE void write(int Type, const QString &Component, const QString &Message,
                           const QString &Title = QString(), const QString &Tips = QString(),
                           bool showTab = false);

signals:
    void notify(int type, const QString &title, const QString &message, const QString &tips);

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
    void writeToFile(const char *level, const QString &msg);
    void cleanupOldLogs(const QString &dir, int keepDays);

    QFile   m_file;
    QMutex  m_mutex;
    QString m_str_FilePath;

    static QtMessageHandler s_prevHandler;
};

extern Logger_class S_Logger;

#endif // LOGGER_H
