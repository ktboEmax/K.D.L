#pragma once
#include "qobject.h"
#include "qrunnable.h"
#include "KDownloaderChild.h"
#include "qmutex.h"
#include "qthread.h"
#include "KDownloaderChildThread.h"

class KDownloaderChild;
class KDownloaderThread :public QThread {
	Q_OBJECT
public:
	explicit KDownloaderThread(
		std::tuple<QString, QString, QString, QString, bool>
		,std::tuple<QString, QString, QString, QString, bool>
		,QString);
	explicit KDownloaderThread(
		std::tuple<QString, QString, QString, QString, bool>
		, QString);
	void run() override;
    bool isdouble=false;
    bool isMD5Ready=false;
    bool isFileReady=false;
	void processError(KDownloaderChild*, KDownloaderChild*);
    void processSuc();
	//QMap<QString, KDownloaderThread*>* threadV;
    static QMutex mutexThreadV;
    static QMutex mutexLog;
	//KDownloaderChild* md5file = nullptr;
	//KDownloaderChild* file = nullptr;
	KDownloaderChild* md5file = nullptr;
	KDownloaderChild* file = nullptr;
	std::string replaceSlashes(const std::string& input);
	std::string FileDigest(const std::string& file);
    QString indexThreadV="";
    //KDownloaderChildThread *ak=new KDownloaderChildThread();
public slots:
    void deleteLaterSlot();
    void downloadFaildSolve();
    void MD5downloadFaildSolve();
    void signaleFileDownloadFaildSolve();
signals:
    void downloadCompeleted0();
    void downloadCompeleted1();
private:
   // QNetworkAccessManager* m_networkManager;
	std::tuple<QString, QString, QString, QString, bool> fileInfo;
	std::tuple<QString, QString, QString, QString, bool> MD5fileInfo;
	QString filePath;
	bool isNeedToMD5Check;


};

