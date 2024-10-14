#include "KDownloaderChild.h"
#include "klog.h"

void KDownloaderChild::resetInfo(std::tuple<QString, QString, QString, QString, bool> fileInfo, const QString& downloadDir)
{
    //qDebug()<<"dddd";
    //: m_fileUrl(std::get<0>(fileInfo)), m_downloadDir(downloadDir), m_filename(std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1])
    m_networkManager = new QNetworkAccessManager(this);
    m_fileUrl = std::get<0>(fileInfo);
    m_downloadDir = downloadDir;
    //m_filename = std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1];
    //std::get<2>(fileInfo)+(std::get<2>(fileInfo)!="/"?(std::get<2>(fileInfo).endsWith("/")?"":"/"):"")+
    m_filename=std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1];
    if(m_downloadDir.endsWith("/")){
        m_downloadDir.chop(1);
    }
    while (m_filename.startsWith("/")) {
        m_filename.remove(0,1);
    }
    // qDebug()<<m_fileUrl<<" "<<m_filename;
    // qDebug()<<m_downloadDir;
}

void KDownloaderChild::startDownload()
{
    QUrl url1(m_fileUrl);
    QNetworkRequest request1(url1);

    reply1 = m_networkManager->get(request1);
    m_file = new QFile(m_downloadDir + (m_filename.startsWith("/")?"":"/") + m_filename);
    // qDebug()<<m_downloadDir + (m_filename.startsWith("/")?"":"/") + m_filename;
    // qDebug()<<m_downloadDir+" "+m_filename;
    if(!m_file->open(QIODevice::WriteOnly)){
        emit downloadFailed();
    }
    //连接信号
    connect(reply1, &QNetworkReply::finished, this, &KDownloaderChild::onDownloadFinished,Qt::QueuedConnection);
    connect(reply1, &QNetworkReply::downloadProgress, this, &KDownloaderChild::onDownloadProgress,Qt::QueuedConnection);
    connect(reply1,&QNetworkReply::readyRead,this,&KDownloaderChild::downloadToRead,Qt::QueuedConnection);
    // // 记录文件对象


}

void KDownloaderChild::downloadToRead(){
    //QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply1->error()==QNetworkReply::NoError){
        m_file->write(reply1->readAll());
    }else{
        //TO CANCEL //
        Klog::GetStaticKlog()->write(reply1->errorString());
        m_file->close();
        reply1->deleteLater();
        m_networkManager->deleteLater();
        emit downloadFailed();
        //TO CANCEL //
    }
    //reply->deleteLater();
}


void KDownloaderChild::onDownloadFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        if (m_file->isOpen()) {
            //m_file->write(reply->readAll());
            m_file->close();
            reply1->deleteLater();
            m_networkManager->deleteLater();
            //qDebug() << "File downloaded successfully to: " << m_downloadDir + "/" + m_filename;
            emit downloadSuc();
        }
        else {
            //qDebug() << "Error opening file for writing.";
            errorInfo = "Error opening file for writing.";
            m_file->close();
            reply1->deleteLater();
            m_networkManager->deleteLater();
            emit downloadFailed();
        }
    }
    else {
       // qDebug() << "Download failed. Error: " << reply->errorString();
        m_file->close();
        errorInfo = reply->errorString();
        reply1->deleteLater();
        m_networkManager->deleteLater();
        emit downloadFailed();
    }

    reply->deleteLater();
}
void KDownloaderChild::onDownloadProgress(double bytesReceived, double bytesTotal)
{
    progress = bytesReceived / bytesTotal*100;
    //qDebug()<<m_filename <<"progress:" << progress<<"%";
}

