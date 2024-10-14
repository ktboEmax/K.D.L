#include "Kthread.h"
#include "KDownloader.h"
Kthread::Kthread(){setAutoDelete(false);}

Kthread::Kthread(QString url,QString path,bool recursive)
{
	setAutoDelete(true);
	this->fileURL = url;
	this->fileLocalAddress = path;
	this->isRecursive = recursive;
}
Kthread::~Kthread(){
    //QTextStream input(stdin);
    //QTextStream output(stdout);
    //output << "Download completed: " << this->fileURL << " " << this->fileLocalAddress << Qt::endl;
}
void Kthread::run()
{

    while(this->fileURL.endsWith("/")){
        this->fileURL.chop(1);
    }
    while(this->fileLocalAddress.endsWith("/")){
        this->fileLocalAddress.chop(1);
    }
    KDownloader *KD=new KDownloader(QUrl(this->fileURL),isRecursive,this->fileLocalAddress);
    connect(KD, &KDownloader::ok, this,[this, &KD](){
        ab.quit();
        ab.wait();
        KD->deleteLater();
        deleteLater();
    });
    ab.down();

}

QString Kthread::GetFileURL(){return QString();}
QString Kthread::GetFileLocalAddress(){return QString();}
void Kthread::SetFileURL(QString a){this->fileURL = a;}
void Kthread::SetFileLocalAddress(QString a){this->fileLocalAddress = a;}
