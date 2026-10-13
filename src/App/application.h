#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QString>

class Application_class : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Language READ getLanguage WRITE setLanguage NOTIFY languageChanged)
public: /* Typedef and enums */
    enum Language_e{
        Lang_EN,
        Lang_RU,
        Lang_RO,
        Lang_ES,
    };
public: /* Members */
    Language_e Language = Lang_EN;
    QString    Organisation;
    QString    ApplicationName;

private: /* Functions */
    void firstConfig();
    void loadConfig();
    void saveConfig();

public: /* Support */
    static Language_e fromString(const QString    &lang);
    static QString    toString  (const Language_e &lang);

public:
    Application_class();
    void begin();

    QString getLanguage()                    const { return toString(Language);   }
    void    setLanguage(const QString &lang)       { Language = fromString(lang); }

signals:
    void languageChanged();
};

extern Application_class Application;

#endif // APPLICATION_H
