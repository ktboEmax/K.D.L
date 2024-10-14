#pragma once
#include "kthreadpool.h"
#include "qregularexpression.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkreply.h"
#include "qtextstream.h"
#include "tuple"
class KDownloaderThread;
class KDownloader:public QObject
{
	Q_OBJECT
public:
    KDownloader();
    ~KDownloader();
    KDownloader(const QUrl& url, bool recursion,QString);
	void Download(const QUrl& url,bool recursion);
	void setManager(QNetworkAccessManager* a);
	QNetworkAccessManager* getManager();
	void processCompeleted();
	
	//QMap<QString, KDownloaderThread* >* threadV;
public slots:
	void onFinished(QNetworkReply* reply);
	void parseLinks(const QString& html, QString);
	//void fetchFiles(const QUrl& html);
signals:
	void processCompelete();
signals:
	void ok();
private:
    KThreadPool *testpool=new KThreadPool();
    bool isFirstRecordUrl=true;
    QString firstRecordUrl="";
    int firestRecordUrlSize=0;
	QString filePathKDownloadChild;
	QUrl fileUrlKDownloader;
	bool recursion;
	QNetworkAccessManager* manager;
	QVector<std::tuple<QString, QString, QString, QString, bool>> fileMD5Vector;
	QMap<QString, std::tuple<QString, QString, QString, QString, bool>> fileQMap;
	QMap<std::tuple<QString, QString, QString, QString, bool>, std::tuple<QString, QString, QString, QString, bool>> downloadQMap;
    static QMutex testPoolMutexLocker;

};

