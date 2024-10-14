#pragma once
#include <QCoreApplication>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QEventLoop>
#include "KDownloaderThread.h"
class KDownloaderThread;
class KDownloaderChild : public QObject
{
    Q_OBJECT

public:
    KDownloaderChild()=default;
    KDownloaderChild(std::tuple<QString, QString, QString, QString, bool> fileInfo, const QString& downloadDir)
        : m_filename(std::get<0>(fileInfo).split("/")[std::get<0>(fileInfo).split("/").size() - 1])
    {
        m_fileUrl=std::get<0>(fileInfo);
        m_downloadDir=downloadDir;
        m_networkManager = new QNetworkAccessManager(this);
        startDownload();
    }
    void resetInfo(std::tuple<QString, QString, QString, QString, bool> fileInfo, const QString& downloadDir);
    void startDownload();
    double progress;
    //QMap<QString, KDownloaderThread*>* threadV;
    QString m_filename;
private slots:
    void onDownloadFinished();
    void onDownloadProgress(double bytesReceived, double bytesTotal);
    void downloadToRead();
signals:
    void downloadFailed();
    void downloadSuc();
    void downloadFailed1();
    

private:
    // QUrl url1;
    // QNetworkRequest request1;

    QNetworkReply* reply1;
    QString errorInfo;
    
    QString m_fileUrl;
    QString m_downloadDir;
    
    QNetworkAccessManager* m_networkManager;
    QFile* m_file;
};
