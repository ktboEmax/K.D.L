#include "kthreadpool.h"
QMutex KThreadPool::qmutex;
QMutex KThreadPool::functionMutex;
KThreadPool::KThreadPool() {
    capacity= Tool::getStaticTool()->numberOfThread;
    // qDebug()<<Tool::getStaticTool()->GetNumberOfThread();
    // qDebug()<<capacity;
    primaryCapacity=Tool::getStaticTool()->numberOfThread;
    connect(Tool::getStaticTool(),&Tool::ToolsCapacityChanged,this,[this](){
         capacity+= Tool::getStaticTool()->numberOfThread-primaryCapacity;
         primaryCapacity=Tool::getStaticTool()->numberOfThread;
    },Qt::QueuedConnection);
}


void KThreadPool::start(KDownloaderThread* KThreadExa){
    KthreadQueue.push_back(KThreadExa);
    function();
}


void KThreadPool::function(){
    //qDebug()<<capacity<<"llllllllllllll";
    while(capacity>0&&KthreadQueue.size()>0){
        KDownloaderThread* KT= *KthreadQueue.begin();
        connect(KT,&KDownloaderThread::downloadCompeleted0,this,[this](){
            qmutex.lock();
            temporaryCapicity++;
            qmutex.unlock();
            if(!isFunctionUsed){
                isFunctionUsed=true;
                function();
            }

        },Qt::QueuedConnection);
        KT->start();
        KthreadQueue.pop_front();
        KT->msleep(100);
        capacity--;
    }

    if(temporaryCapicity>0){
        capacity=capacity+temporaryCapicity;
        qmutex.lock();
        temporaryCapicity=0;
        qmutex.unlock();
        //isFunctionUsed=true;
        if(capacity==Tool::getStaticTool()->GetNumberOfThread()&&KthreadQueue.size()==0){
            emit threadPoolProcessCompelete();
            this->deleteLater();
        }

        function();
    }
    isFunctionUsed=false;
}
