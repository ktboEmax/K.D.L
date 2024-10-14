#include "klog.h"
#include "QDir"
Klog* Klog::staticKlog=new Klog;

Klog* Klog::GetStaticKlog(){
    return staticKlog;
}


Klog::Klog() {
    initialization();
}
void Klog::initialization(){
    QDir logDir(QDir::currentPath()+"/logs");
    //qDebug()<<QDir::currentPath()+"/"+"logs";
    if(!logDir.exists()){
        logDir.mkpath(".");
    }
}


void Klog::write(QString dataLogs){
    initialization();
    QString formattedDate = QDate::currentDate().toString("yyyy-MM-dd");
    QString fileName="["+ formattedDate+"]"+"log"+".txt";
    QFile logFile(QDir::currentPath()+"/"+"logs"+"/"+fileName);
    logFile.open(QIODevice::Append| QIODevice::Text);
    logFile.write(dataLogs.toUtf8()+"\n");
    logFile.close();
}
