#pragma once
#include "qobject.h"
#include "qrunnable.h"
#include "qtextstream.h"
#include "KDownloaderThread.h"
#include "KDownloaderChildThread.h"
class Kthread : public QObject,public QRunnable
{
public:
	Kthread();
	Kthread(QString,QString,bool);
	~Kthread();
	void run() override;
	QString GetFileURL();
	QString GetFileLocalAddress();
	void SetFileURL(QString);
	void SetFileLocalAddress(QString);
	QMap<QString, KDownloaderThread*>* threadV;
    KDownloaderChildThread ab;
private:
	bool isRecursive = false;
	QString fileURL;
	QString fileLocalAddress;
};

