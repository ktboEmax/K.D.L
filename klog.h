#ifndef KLOG_H
#define KLOG_H

#include <QObject>

class Klog:public QObject
{
    Q_OBJECT
public:
    Klog();
    void initialization();
    void write(QString);
    static Klog* GetStaticKlog();
    static Klog* staticKlog;
private:

};

#endif // KLOG_H
