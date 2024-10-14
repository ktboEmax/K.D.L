#include "KDownloaderThread.h"
#include "md5.h"
#include <QCoreApplication>
#include <QThread>
#include <QObject>
#include <QDebug>
#include "Tool.h"
#include "QThreadPool"
#include "klog.h"
QMutex KDownloaderThread::mutexThreadV;
QMutex KDownloaderThread::mutexLog;
KDownloaderThread::KDownloaderThread(std::tuple<QString, QString, QString, QString, bool> MFI, std::tuple<QString, QString, QString, QString, bool> FI, QString FP)
{
    //qDebug()<<"llllll";
    //setAutoDelete(true);
	isNeedToMD5Check = true;
	MD5fileInfo = MFI;
	fileInfo = FI;
	filePath = FP;
    isMD5Ready=false;
    isFileReady=false;
    isdouble=true;
    while (filePath.endsWith("/")) {
        filePath.chop(1);
    }
}

KDownloaderThread::KDownloaderThread(std::tuple<QString, QString, QString, QString, bool> FI, QString FP)
{
    //setAutoDelete(true);
    isMD5Ready=false;
    isFileReady=false;
	isNeedToMD5Check = false;
	fileInfo = FI;
	filePath = FP;
    //qDebug()<<FP;
    isdouble=false;
    while (filePath.endsWith("/")) {
        filePath.chop(1);
    }
}


void KDownloaderThread::MD5downloadFaildSolve(){
    //qDebug()<<"lllllllllopopopopopopopopolllllllllllllll";
    if(Tool::getStaticTool()->outputLogs){
        mutexLog.lock();
        QString thisTime="["+QDateTime::currentDateTime().toString("HH:mm:ss")+"]"+indexThreadV+" Download failed";
        Klog::GetStaticKlog()->write(thisTime);
        mutexLog.unlock();
    }
    quit();
    wait();
        if (Tool::getStaticTool()->GetTryAgainWhenMd5CheckFailed()) {
            QString MD5FileName = std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
            QFile QFILE_md5(filePath + "/" + MD5FileName);
            QFILE_md5.remove();
            md5file->deleteLater();
            md5file=new KDownloaderChild();
            QObject::connect(md5file, &KDownloaderChild::downloadSuc, this, [this]() {
                    this->isMD5Ready = true;
                    processSuc();
                    //ak->quit();

                    //QObject::deleteLater();
                },Qt::QueuedConnection);
            connect(md5file,&KDownloaderChild::downloadFailed,this,&KDownloaderThread::MD5downloadFaildSolve,Qt::QueuedConnection);
            md5file->resetInfo(MD5fileInfo, filePath);
            md5file->startDownload();
            exec();
        }
}

void KDownloaderThread::downloadFaildSolve(){
    if(Tool::getStaticTool()->outputLogs){
        mutexLog.lock();
        QString thisTime="["+QDateTime::currentDateTime().toString("HH:mm:ss")+"]"+indexThreadV+" Download failed";
        Klog::GetStaticKlog()->write(thisTime);
        mutexLog.unlock();
    }
    quit();
    wait();
    if (Tool::getStaticTool()->GetTryAgainWhenMd5CheckFailed()) {
        QString FileName = std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1];
        QFile QFILE_file(filePath +"/" + FileName);
        QFILE_file.remove();
        file->deleteLater();
        file = new KDownloaderChild();


        QObject::connect(file, &KDownloaderChild::downloadSuc, this, [this]() {
                //qDebug()<<"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
                this->isFileReady = true;

                processSuc();
                //ak->quit();

                //emit downloadCompeleted();
            },Qt::QueuedConnection);
        connect(file,&KDownloaderChild::downloadFailed,this, &KDownloaderThread::downloadFaildSolve,Qt::QueuedConnection);

        file->resetInfo(fileInfo, filePath);
        file->startDownload();
        exec();
    }


}
void KDownloaderThread::signaleFileDownloadFaildSolve(){
    quit();
    wait();
    if (Tool::getStaticTool()->GetTryAgainWhenMd5CheckFailed()) {
        QString FileName = std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1];
        QFile QFILE_file(filePath + "/" + FileName);
        QFILE_file.remove();
        file->deleteLater();
        file=new KDownloaderChild();
        QObject::connect(file, &KDownloaderChild::downloadFailed, this, &KDownloaderThread::signaleFileDownloadFaildSolve,Qt::QueuedConnection);
        QObject::connect(file, &KDownloaderChild::downloadSuc, this, &KDownloaderThread::deleteLaterSlot,Qt::QueuedConnection);
        file->resetInfo(fileInfo, filePath);
        file->startDownload();
        exec();
    }


}

void KDownloaderThread::run()
{

	if (isNeedToMD5Check) {
        indexThreadV=std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1] + "/" + std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
        mutexThreadV.lock();
        //threadV->insert(std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1] + "/" + std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1], this);
        Tool::getStaticTool()->threadV->insert(indexThreadV, this);
        mutexThreadV.unlock();
		//qDebug() << std::get<0>(MD5fileInfo) << std::get<0>(fileInfo);
        md5file = new KDownloaderChild();
        file = new KDownloaderChild();
        // md5file->moveToThread(this);
        // file->moveToThread(this);
        //qDebug()<<MD5FileName<<" "<<FileName;
        QObject::connect(md5file, &KDownloaderChild::downloadSuc, this, [this]() {
            this->isMD5Ready = true;

            processSuc();
            //ak->quit();

            //QObject::deleteLater();
        },Qt::QueuedConnection);

        QObject::connect(file, &KDownloaderChild::downloadSuc, this, [this]() {

            this->isFileReady = true;

            processSuc();
            //ak->quit();

            //emit downloadCompeleted();
        },Qt::QueuedConnection);


        QObject::connect(md5file, &KDownloaderChild::downloadFailed, this, &KDownloaderThread::MD5downloadFaildSolve,Qt::QueuedConnection);
        QObject::connect(file, &KDownloaderChild::downloadFailed, this, &KDownloaderThread::downloadFaildSolve,Qt::QueuedConnection);
        // QObject::connect(file, &KDownloaderChild::downloadFailed, this, [this]() {
        //         qDebug()<<"llllllllllllllllllllllllllllllllllllllllllllllllllllll";
        //         if (Tool::getStaticTool()->GetTryAgainWhenMd5CheckFailed()) {
        //             QString FileName = std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
        //             QFile QFILE_file(filePath + "/" + FileName);
        //             QFILE_file.remove();
        //             file->startDownload();
        //         }
        //     },Qt::QueuedConnection);


        // QObject::connect(md5file, &KDownloaderChild::downloadFailed, this, [this]() {
        //      qDebug()<<"lllllllllopopopopopopopopolllllllllllllll";
        //     // if (Tool::getStaticTool()->GetTryAgainWhenMd5CheckFailed()) {
        //     //     QString MD5FileName = std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
        //     //     QFile QFILE_md5(filePath + "/" + MD5FileName);
        //     //     QFILE_md5.remove();
        //     //     md5file->startDownload();
        //     // }
        // },Qt::QueuedConnection);

        // QObject::connect(file, &KDownloaderChild::downloadFailed, this, [this]() {
        //     qDebug()<<"llllllllllllllllllllllllllllllllllllllllllllllllllllll";
        //     // if (Tool::getStaticTool()->GetTryAgainWhenMd5CheckFailed()) {
        //     //     QString FileName = std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
        //     //     QFile QFILE_file(filePath + "/" + FileName);
        //     //     QFILE_file.remove();
        //     //     file->startDownload();
        //     // }
        // },Qt::QueuedConnection);

        file->resetInfo(fileInfo, filePath);
        md5file->resetInfo(MD5fileInfo, filePath);
        md5file->startDownload();
        file->startDownload();
        exec();
	}
	else {
        indexThreadV=std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1];
        //qDebug()<<indexThreadV;
        Tool::getStaticTool()->threadV->insert(indexThreadV, this);
        //m_networkManager=new QNetworkAccessManager();
        file = new KDownloaderChild();
        //file->setParent(this);
        QObject::connect(file, &KDownloaderChild::downloadFailed, this, &KDownloaderThread::signaleFileDownloadFaildSolve,Qt::QueuedConnection);

        QObject::connect(file, &KDownloaderChild::downloadSuc, this, &KDownloaderThread::deleteLaterSlot,Qt::QueuedConnection);
        file->resetInfo(fileInfo, filePath);
        file->startDownload();
        exec();

	}








}

void KDownloaderThread::deleteLaterSlot()
{
    mutexThreadV.lock();
    Tool::getStaticTool()->threadV->remove(indexThreadV);
    mutexThreadV.unlock();
    //qDebug()<<indexThreadV<<" "<<file->m_filename;
    if(Tool::getStaticTool()->outputLogs){
        mutexLog.lock();
        QString thisTime="["+QDateTime::currentDateTime().toString("HH:mm:ss")+"]"+indexThreadV+" Download successful";
        Klog::GetStaticKlog()->write(thisTime);
        mutexLog.unlock();
    }

    file->deleteLater(); // 安全删除
    //md5file->deleteLater();
    emit downloadCompeleted0();
    this->quit();
    //emit downloadCompeleted();
    //qDebug()<<"12354678";
    this->wait();
    QObject::deleteLater(); // 明确调用 QObject 的 deleteLater  // 安全删除当前线程

}



void KDownloaderThread::processError(KDownloaderChild* md5file, KDownloaderChild* file) {


    qDebug()<<md5file<<file;




}

string KDownloaderThread::replaceSlashes(const std::string& input) {
	std::string result = input;
	std::string toReplace = "/";
	std::string replaceWith = "//";

	size_t pos = 0;
	while ((pos = result.find(toReplace, pos)) != std::string::npos) {
		result.replace(pos, toReplace.length(), replaceWith);
		pos += replaceWith.length(); // Move past the newly replaced substring
	}

	return result;
}

string KDownloaderThread::FileDigest(const string& file) {



    ifstream in(file.c_str(), ios::binary);
    if (!in)
        return "";

    MD5 md5;
    std::streamsize length;
    char buffer[1024];
    while (!in.eof()) {
        in.read(buffer, 1024);
        length = in.gcount();
        if (length > 0)
            md5.update(buffer, length);
    }
    in.close();
    return md5.toString();
}


void KDownloaderThread::processSuc() {

	if (isFileReady && isMD5Ready) {
        if(Tool::getStaticTool()->outputLogs){
            mutexLog.lock();
            QString thisTime="["+QDateTime::currentDateTime().toString("HH:mm:ss")+"]"+indexThreadV+" Download successful";
            Klog::GetStaticKlog()->write(thisTime);
            mutexLog.unlock();
        }



        QString MD5FileName = std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
        QString FileName= std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1];
        QFile QFILE_md5(filePath+"/"+MD5FileName);
        QFile QFILE_file(filePath+(filePath.endsWith("/")?"":"/") + FileName);
        //qDebug()<<filePath+"/"+MD5FileName<<" "<<filePath+(filePath.endsWith("/")?"":"/") + FileName;
        //qDebug()<<filePath+(filePath.endsWith("/")?"":"/") + FileName;
        QFILE_md5.open(QIODevice::ReadOnly);
        QString MD5string = QFILE_md5.readAll().split(' ')[0];
        //qDebug() << MD5string<< filePath.toStdString() + FileName.toStdString();
        //qDebug()<<FileDigest(replaceSlashes( filePath.toStdString() + FileName.toStdString()));
        QFILE_md5.close();
        if (MD5string.toStdString() == FileDigest(replaceSlashes( filePath.toStdString()+"/" + FileName.toStdString()))) {
            mutexThreadV.lock();
            Tool::getStaticTool()->threadV->remove(indexThreadV);
            mutexThreadV.unlock();
            //qDebug()<<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;";
            // QString index = "";
            // if (!std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1].endsWith("/")) {
            //     index = std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1] + "/" + std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
            // }
            // else {
            //     index = std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1]  + std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
            // }
            QFILE_file.rename(filePath+"/" +"[CHECKED]" + std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1]);
            //qDebug()<<filePath +"[CHECKED]" + std::get<0>(fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
            QFILE_md5.close();
            emit downloadCompeleted0();
            quit();
            wait();
            md5file->deleteLater();
            file->deleteLater();
            QObject::deleteLater();

        }else{
            quit();
            wait();


            if (Tool::getStaticTool()->GetTryAgainWhenMd5CheckFailed()) {
                // QString MD5FileName = std::get<0>(MD5fileInfo).split("/")[std::get<0>(MD5fileInfo).split("/").size() - 1];
                // QString MD5FileDir=std::get<2>(fileInfo);
                // while (MD5FileDir.endsWith("/")) {
                //     MD5FileDir.chop(1);
                // }
                // MD5FileName.replace("/","");
                QFile QFILE_md5(filePath + "/" + MD5FileName);
                QFILE_md5.remove();
                md5file->deleteLater();
                md5file=new KDownloaderChild();
                QObject::connect(md5file, &KDownloaderChild::downloadSuc, this, [this]() {
                        this->isMD5Ready = true;
                        processSuc();
                        //ak->quit();

                        //QObject::deleteLater();
                    },Qt::QueuedConnection);
                connect(md5file,&KDownloaderChild::downloadFailed,this,&KDownloaderThread::MD5downloadFaildSolve,Qt::QueuedConnection);
                md5file->resetInfo(MD5fileInfo, filePath);
                md5file->startDownload();


                // QString FileName = std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1];
                // QString FileDir=std::get<2>(fileInfo);
                // while (FileDir.endsWith("/")) {
                //     FileDir.chop(1);
                // }
                // FileName.replace("/","");
                QFile QFILE_file(filePath + "/" + FileName);

                QFILE_file.remove();
                file->deleteLater();
                file = new KDownloaderChild();


                QObject::connect(file, &KDownloaderChild::downloadSuc, this, [this]() {
                        //qDebug()<<"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
                        this->isFileReady = true;

                        processSuc();
                        //ak->quit();

                        //emit downloadCompeleted();
                    },Qt::QueuedConnection);
                connect(file,&KDownloaderChild::downloadFailed,this, &KDownloaderThread::downloadFaildSolve,Qt::QueuedConnection);

                file->resetInfo(fileInfo, filePath);
                file->startDownload();
                exec();
            }else{
                md5file->deleteLater();
                file->deleteLater();
                QObject::deleteLater();

            }

        }


	}



}
