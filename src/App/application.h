#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>

class Application_class : public QObject
{
    Q_OBJECT

public:
    Application_class();
    void begin();
};

#endif // APPLICATION_H
