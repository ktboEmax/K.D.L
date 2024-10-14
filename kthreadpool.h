#ifndef KTHREADPOOL_H
#define KTHREADPOOL_H

#include <QObject>
#include "KDownloaderThread.h"
#include "Tool.h"
class KThreadPool:public QObject
{
    Q_OBJECT
public:
    KThreadPool();
    void start(KDownloaderThread*);
    void function();
private:
    int capacity=0;
    int primaryCapacity=0;
    QVector<KDownloaderThread*> KthreadQueue;
    //Kthread* next=nullptr;
    bool isFunctionUsed=false;
    int temporaryCapicity=0;
    static QMutex qmutex;
    static QMutex functionMutex;
signals:
    void threadPoolProcessCompelete();
};

#endif // KTHREADPOOL_H
