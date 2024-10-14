#pragma once
#include <qdir.h>
#include <qtextstream.h>
using namespace std;
class KDownloaderThread;
class Tool:public QObject
{
    Q_OBJECT
public:
	Tool();
    ~Tool();

	static Tool* getStaticTool();
	QString showSetConfiguration();
	bool setConfiguration(QFile &file);
	bool readConfiguration(QFile& file);
	void setToolConfiguration(int, bool, bool,QString,int);
	void showToolConfiguration();
	void showCommand();
	void initialize();
	void downloadFileFromURL(QString fileURL, QString fileLocalAddress);

	static Tool* tool;

	void processCommand(QString);

    int GetNumberOfThread();
    bool GetTryAgainWhenMd5CheckFailed();
    bool GetOutputLogs();
    QString GetdefaultDownloadAddress();
    int GetTheNumberOfRowsInOnePage();
	QMap<QString, KDownloaderThread*>* threadV=new QMap<QString, KDownloaderThread*>;
	void showDownload(int);

    int thePage = 0;
    int numberOfThread = 0;
    bool tryAgainWhenMd5CheckFailed = true;
    bool outputLogs = true;
    // QString defaultDownloadAddress = QDir::currentPath() + "/defaultDownloadFolder";
    QString defaultDownloadAddress="";
    int theNumberOfRowsInOnePage = 10;


private:

    // int thePage = 0;
    // int numberOfThread = 0;
    // bool tryAgainWhenMd5CheckFailed = true;
    // bool outputLogs = true;
    // QString defaultDownloadAddress = QDir::currentPath() + "/defaultDownloadFolder";
    // int theNumberOfRowsInOnePage = 10;

	QString targetConfigurationPath = QDir::currentPath() + "/configuration";

	QMap<QString, int> toolConfigurationMap;
	//vector<QString> commandVector = {"RSTCON","DLFILE"};
	QMap<QString, int> commandQMap = {
		{"RSTCON",0},
		{"DLFILE",1},
		{"SHOWCOMMAND",2},
		{"SHOWCONFIG",3},
		{"SHOWDOWNLOAD",4},
		{"NEXTPAGE",5}
	};
signals:
    void ToolsCapacityChanged();
};



