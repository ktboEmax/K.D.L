
#include <QtCore/QCoreApplication>
#include <qdir.h>
#include <iostream>
#include "KDownloader.h"
#include "Tool.h"
#include "klog.h"
#include "qthreadpool.h"
using namespace std;



int main(int argc, char* argv[])
{

    QCoreApplication application(argc, argv);
    Tool::getStaticTool()->setParent(&application);
    Klog::GetStaticKlog()->setParent(&application);
    Tool::getStaticTool()->initialize();
    //QThreadPool::globalInstance()->setMaxThreadCount(Tool::getStaticTool()->GetNumberOfThread() + 1);

    QTextStream input(stdin);
    QTextStream output(stdout);
    QThreadPool::globalInstance()->setMaxThreadCount(10);
    string acceptedCommand;
    output<<(" _  __  _____    _")<<Qt::endl;
    output<<("| |/ / |  __ \\  | |")<<Qt::endl;
    output<<("| ' /  | |  | | | |")<<Qt::endl;
    output<<("|  <   | |  | | | |")<<Qt::endl;
    output<<("| . \\ _| |__| |_| |____")<<Qt::endl;
    output<<("|_|\\_(_)______(_)______|");
    output<<"v1.0"<<Qt::endl;

    while (true)
    {
        output<<"Please enter the command:"<<Qt::endl;
        std::getline(std::cin, acceptedCommand);
        Tool::getStaticTool()->processCommand(QString::fromStdString(acceptedCommand));
    }


    //KDownloader a(QUrl("https://ftp.ncbi.nlm.nih.gov/blast/db/"), false, "C:/Users/28707/Desktop/soft/downloadtest");
    // KDownloader *KD=new KDownloader(QUrl("https://ftp.ncbi.nlm.nih.gov/blast/db"),false,"C:/Users/28707/Desktop/soft/downloadtest");

    return application.exec();
}
