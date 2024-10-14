#include "Tool.h"
#include "Kthread.h"
#include "qthreadpool.h"
#include "KDownloader.h"
Tool* Tool::tool = new Tool;
Tool::Tool()
{
    QDir defaultDownloadAddressQDir(defaultDownloadAddress);
    if (!defaultDownloadAddressQDir.exists())
        defaultDownloadAddressQDir.mkpath(".");
    toolConfigurationMap.insert("Maximum single download tasks(int)", 0);
    toolConfigurationMap.insert("MD5 verification failed. Should we download it again(Y/N)", 1);
    toolConfigurationMap.insert("Output logs(Y/N)", 2);
    toolConfigurationMap.insert("Default download address(string)", 3);
    toolConfigurationMap.insert("How many rows are displayed on one page(int)", 4);
}
Tool::~Tool(){};
Tool* Tool::getStaticTool(){return tool;}

// void Tool::ToolsCapacityChanged(){};

QString Tool::showSetConfiguration()
{
    QTextStream input(stdin);
    QTextStream output(stdout);
    output << "Initial startup initialization settings:" << Qt::endl;
    QString initialConfigurationQString = "Maximum single download tasks(int):";
    output << "Maximum single download tasks(int):" << Qt::endl;
    QString configuration;
    input >> configuration;
    this->numberOfThread = configuration.toInt();
    emit ToolsCapacityChanged();
    initialConfigurationQString.append(configuration).append("\n");
    initialConfigurationQString.append("MD5 verification failed. Should we download it again(Y/N):");
    output << "MD5 verification failed. Should we download it again(Y/N):" << Qt::endl;
    input >> configuration;
    this->tryAgainWhenMd5CheckFailed = configuration=="Y"?true:false;
    initialConfigurationQString.append(configuration).append("\n");
    initialConfigurationQString.append("Output logs(Y/N):");
    output << "Output logs(Y/N):" << Qt::endl;
    input >> configuration;
    this->outputLogs= configuration == "Y" ? true : false;
    initialConfigurationQString.append(configuration).append("\n");
    initialConfigurationQString.append("Default download address(string):");
    output << "Default download address(string):" << Qt::endl;
    input >> configuration;
    this->defaultDownloadAddress = configuration ;
    initialConfigurationQString.append(configuration).append("\n");
    output << "How many rows are displayed on one page(int):" << Qt::endl;
    initialConfigurationQString.append("How many rows are displayed on one page(int):");
    input >> configuration;
    this->theNumberOfRowsInOnePage=configuration.toInt();
    initialConfigurationQString.append(configuration).append("\n");
    return initialConfigurationQString;
}

bool Tool::setConfiguration(QFile &file)
{
    QString con = Tool::getStaticTool()->showSetConfiguration();
    file.open(QIODevice::WriteOnly);
    bool returnedData= file.write(con.toUtf8());
    file.close();
    return returnedData;
}

bool Tool::readConfiguration(QFile& file)
{
    file.open(QIODevice::ReadOnly);
    QString readFile = file.readAll();
    QStringList readFileListAll = readFile.split("\n");
    QTextStream input(stdin);
    QTextStream output(stdout);
    // for(int i=0;i<readFileListAll.size();i++){
    //     qDebug()<<i<<" "<<readFileListAll[i];
    // }

    for (int i = 0; i < readFileListAll.size()-1;i++) {
        QStringList readFileList=readFileListAll[i].split(":");
        //toolConfigurationMap.insert("Maximum single download tasks(int)", 0);
        //toolConfigurationMap.insert("MD5 verification failed. Should we download it again(Y/N)", 1);
        //toolConfigurationMap.insert("Output logs(Y/N)", 2);
        //toolConfigurationMap.insert("Default download address(string)", 3);
        //qDebug()<<readFileList[0]<<" "<<readFileList[1];
        switch (toolConfigurationMap[readFileList[0]])
        {
        case 0: {
            //qDebug()<<readFileList[0]<<readFileList[1];
            this->numberOfThread = readFileList[1].toInt();
            //qDebug()<<numberOfThread;
            break;
        }
        case 1: {
            QString readFiletryAgainWhenMd5CheckFailed = readFileList[1];
            if (readFiletryAgainWhenMd5CheckFailed != "Y" && readFiletryAgainWhenMd5CheckFailed != "N")
                output << "There is an issue with the configuration file. Please use RSTCON to reset it" << Qt::endl;
            else
                this->tryAgainWhenMd5CheckFailed = readFiletryAgainWhenMd5CheckFailed == "Y";
            break;
        }

        case 2: {
            QString readFiletryAgainWhenMd5CheckFailed = readFileList[1];
            if (readFiletryAgainWhenMd5CheckFailed != "Y" && readFiletryAgainWhenMd5CheckFailed != "N")
                output << "There is an issue with the configuration file. Please use RSTCON to reset it" << Qt::endl;
            else
                this->tryAgainWhenMd5CheckFailed = readFiletryAgainWhenMd5CheckFailed == "Y";
            break;
        }

        case 3: {
            this->defaultDownloadAddress = readFileList[1];
            //qDebug()<<readFileList[1];
            break;
        }
        case 4: {
            this->theNumberOfRowsInOnePage = readFileList[1].toInt();
            break;
        }

        default: {
            output << "There is an issue with the configuration file. Please use RSTCON to reset it"<<Qt::endl;
            break;
        }
            
        }
    }


    return readFile.size()>0;
}

void Tool::setToolConfiguration(int numberOfThread, bool tryAgainWhenMd5CheckFailed, bool outputLogs, QString defaultDownloadAddress,int theNumberOfRowsInOnePage)
{
    this->numberOfThread = numberOfThread;
    this->tryAgainWhenMd5CheckFailed = tryAgainWhenMd5CheckFailed;
    this->outputLogs = outputLogs;
    this->defaultDownloadAddress = defaultDownloadAddress;
    this->theNumberOfRowsInOnePage = theNumberOfRowsInOnePage;
}

void Tool::showToolConfiguration()
{
    QTextStream input(stdin);
    QTextStream output(stdout);
    output << "Maximum single download tasks(int):" << this->numberOfThread << Qt::endl << Qt::endl;
    output << "MD5 verification failed. Should we download it again(Y/N):" << this->tryAgainWhenMd5CheckFailed << Qt::endl << Qt::endl;
    output << "Output logs(Y/N):" << this->outputLogs << Qt::endl << Qt::endl;
    output << "Default download address(string):" << this->defaultDownloadAddress << Qt::endl << Qt::endl;
    output << "How many rows are displayed on one page(int):" << this->theNumberOfRowsInOnePage << Qt::endl << Qt::endl;
}

void Tool::showCommand()
{
    QTextStream input(stdin);
    QTextStream output(stdout);
    output << "RSTCON//Reset configuration" << Qt::endl<< Qt::endl;
    output << "DLFILE [String] [String]//Download the file, the first parameter is the file URL, and the second parameter is the local address where the file should be stored,if not filled in, the default download address will be used" << Qt::endl << Qt::endl;
    output << "SHOWCOMMAND//Display Command" << Qt::endl << Qt::endl;
    output << "SHOWCONFIG//Display configuration" << Qt::endl << Qt::endl;

}

void Tool::initialize()
{

    QDir initializeConfiguration(targetConfigurationPath);
    if (!initializeConfiguration.exists())
        initializeConfiguration.mkpath(".");
    QFile initializeConfigurationFile(targetConfigurationPath + "/Configuration.kconf");
    if (!initializeConfigurationFile.exists()) {//不存在配置文件
        Tool::getStaticTool()->setConfiguration(initializeConfigurationFile);
    }
    else {//存在配置文件
        Tool::getStaticTool()->readConfiguration(initializeConfigurationFile);
    }
}

void Tool::downloadFileFromURL(QString fileURL, QString fileLocalAddress)
{
    //qDebug() << "Whether to recursively call";
    //QString RE;
    //QTextStream input(stdin);
    //input >> RE;
    QTextStream input(stdin);
    QTextStream output(stdout);
    output<<"Do you allow recursive operations?[Y/N]";
    output.flush();
    QString recursive="";
    input>>recursive;
    Kthread* KT = new Kthread(fileURL, fileLocalAddress, recursive=='Y');
    //qDebug() << fileLocalAddress;
    //KDownloader *a=new KDownloader(QUrl(fileURL), false, "C:/Users/28707/Desktop/soft/downloadtest");
    QThreadPool::globalInstance()->start(KT);
   // QCoreApplication::exec();
   // QThreadPool::globalInstance()->start(KT);
    //QThreadPool::globalInstance()->waitForDone();
    ////KDownloader* a = new KDownloader();
    //KDownloader a(QUrl("https://ftp.ncbi.nlm.nih.gov/blast/db"), false, "C:/Users/28707/Desktop/soft/REPTILE_TEST/REPTILE_TEST/out/build/downloadtest");
    //KDownloader a(QUrl("https://ftp.ncbi.nlm.nih.gov/blast/db/"), false, "C:/Users/28707/Desktop/soft/downloadtest");
}

void Tool::processCommand(QString command)
{
    //qDebug() << command;
    QTextStream input(stdin);
    QTextStream output(stdout);
    QStringList commandList = command.split(" ");
    //cout << commandList.size();
    //cout << commandList[0].toStdString();
    switch(commandQMap.value(commandList[0],-1)) {
            case 0: {//RSTCON
                    QDir dirConfiguration(targetConfigurationPath);
                    QFileInfoList configurationFileInformation = dirConfiguration.entryInfoList();
                    foreach(const QFileInfo fileInformation, configurationFileInformation)
                    {
                        if (fileInformation.fileName() == "Configuration.kconf") {
                            QFile fileConfigurationRemove(fileInformation.filePath());
                            //QTextStream input(stdin);
                            //QTextStream output(stdout);
                            //output << fileInformation.filePath();
                            if (fileConfigurationRemove.remove())
                                initialize();

                                //cout << "There is a problem, please manually delete the original configuration file" << endl;
                        }

                    }
                    break;
                }
            //{ "DLFILE", 1 },
            //{ "SHOWCOMMAND",2 },
            //{ "SHOWCONFIG",3 }
            case 1: {

                    QString fileURL = commandList[1];
                    if (commandList.size() == 2) {
                        downloadFileFromURL(fileURL, defaultDownloadAddress);
                    }
                    else if(commandList.size() == 3)
                    {
                        QString fileLocalAddress = commandList[2];
                        //qDebug() << fileLocalAddress;
                        downloadFileFromURL(fileURL, fileLocalAddress);
                    }else {
                        output << "Wrong number of parameters" << Qt::endl;
                    }
        
        
                    break;
            }
            case 2: {
                    showCommand();
                    break;
            }
            case 3: {
                    showToolConfiguration();
                    break;
            }
            case 4: {
                    showDownload(thePage);
                    break;
            }
            case 5: {
                    thePage = thePage + theNumberOfRowsInOnePage > threadV->size() ? 0 : thePage + theNumberOfRowsInOnePage;
                    showDownload(thePage);
                    break;
            }
            default: {
                    output << "Commands Error" << Qt::endl;
                    break;
            }
        }
    }





int Tool::GetNumberOfThread(){return this->numberOfThread;}
bool Tool::GetTryAgainWhenMd5CheckFailed(){return this->tryAgainWhenMd5CheckFailed;}
bool Tool::GetOutputLogs(){return this->outputLogs;}
QString Tool::GetdefaultDownloadAddress(){return this->defaultDownloadAddress;}
int Tool::GetTheNumberOfRowsInOnePage(){return this->theNumberOfRowsInOnePage;}

void Tool::showDownload(int start)
{
    //qDebug() << threadV->size();
    auto it = threadV->begin();
    //qDebug() <<it.value()->file->progress;
    for(int i=0;i<start;i++){
        it++;
    }


    QTextStream output(stdout);
    for (int i = start; i < start+this->theNumberOfRowsInOnePage; i++) {
        if (it != threadV->end()) {
            output << i << "."<<Qt::endl;
            // if(it.value()->isdouble){
            //     QString md5file = it.value()->md5file->m_filename;
            //     double md5progress = it.value()->md5file->progress;
            //     qDebug() << "----" << md5file << ":" << md5progress<<"%";
            //     QString file = it.value()->file->m_filename;
            //     double fileprogress = it.value()->file->progress;
            //     qDebug() << "---------" << file << ":" << fileprogress<<"%";
            // }else{
            //     QString file = it.value()->file->m_filename;
            //     double fileprogress = it.value()->file->progress;
            //     qDebug() << "---------" << file << ":" << fileprogress<<"%";

            // }

            if (it.value()->md5file != nullptr) {
                QString md5file = it.value()->md5file->m_filename;
                double md5progress = it.value()->md5file->progress;
                output << "----" << md5file << ":" << md5progress<<"%"<<Qt::endl;
            }
            if (it.value()->file!=nullptr) {
                QString file = it.value()->file->m_filename;
                double fileprogress = it.value()->file->progress;
                output << "---------" << file << ":" << fileprogress<<"%"<<Qt::endl;
            }

            it++;
        }
        //QMap<QString, KDownloaderThread*>* threadV = new QMap<QString, KDownloaderThread*>;
    }
    //qDebug() << "DDDDDDDDDDDDDDD";



}


