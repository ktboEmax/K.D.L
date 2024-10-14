#include "KDownloader.h"
#include "KDownloaderChild.h"
#include "klog.h"
#include "kthreadpool.h"
#include "qdir.h"
#include "qthreadpool.h"

using namespace std;
QMutex KDownloader::testPoolMutexLocker;
KDownloader::KDownloader(){manager = new QNetworkAccessManager();}

KDownloader::~KDownloader()
{

}

KDownloader::KDownloader(const QUrl& url, bool recursion,QString filePath)
{
    connect(testpool,&KThreadPool::threadPoolProcessCompelete,this,[this](){
        emit ok();
    });
    QString urlToString=url.toString();
    if(!urlToString.startsWith("https://")){
        urlToString="https://"+urlToString;
        qDebug()<<urlToString;
    }
    //qDebug()<<urlToString<<" "<<url.toString();
    firstRecordUrl=urlToString;
    firestRecordUrlSize=firstRecordUrl.size()+1;
    this->recursion=recursion;
    isFirstRecordUrl=false;
    //qDebug() << "DDDDDDDDDDDDDDDDDD";
    filePathKDownloadChild = filePath;
    if (!QDir(filePath).exists())
        QDir(filePath).mkdir(".");
    manager = new QNetworkAccessManager(this);
    fileUrlKDownloader = url;
    Download(url, recursion);
}

void KDownloader::Download(const QUrl& url, bool recursion)
{

    connect(manager, &QNetworkAccessManager::finished, this, &KDownloader::onFinished,Qt::QueuedConnection);
    QNetworkRequest request(url);
    manager->get(request);
}

void KDownloader::setManager(QNetworkAccessManager* a)
{
    manager = a;
}

QNetworkAccessManager* KDownloader::getManager()
{
    return manager;
}

void KDownloader::processCompeleted()
{

    //qDebug() << "CCCCCC";
    for (auto it = fileMD5Vector.begin(); it != fileMD5Vector.end(); ) {

        QString md5 = std::get<3>(*it);

        // 检查 fileQMap 是否包含该 MD5 值
        if (fileQMap.contains(md5)) {
            //qDebug()<<"ddddddddddddddddddddddddddddddddd";
            // 将当前元素插入到 downloadQMap
            downloadQMap.insert(*it, fileQMap[md5]); // 将元素插入到 downloadQMap

            // 从 fileQMap 中移除元素
            fileQMap.remove(md5);
            //qDebug()<<fileQMap.size();
            // 从 fileMD5Vector 中安全地删除当前元素并更新迭代器
            it = fileMD5Vector.erase(it);
        } else {
            // 仅当没有删除元素时才前进迭代器
            ++it;
        }
    }


    //qDebug() << downloadQMap.size();
    // // 遍历 downloadQMap

    for (auto i = fileQMap.begin(); i != fileQMap.end(); /* no increment here */) {
        KDownloaderThread* threadDownloadQMap = new KDownloaderThread(i.value(), this->filePathKDownloadChild + std::get<2>(i.value()));
        //threadDownloadQMap->start();
        testPoolMutexLocker.lock();
        testpool->start(threadDownloadQMap);
        testPoolMutexLocker.unlock();
        // 删除当前元素并更新迭代器
        i = fileQMap.erase(i);
    }

    for (auto i = downloadQMap.begin(); i != downloadQMap.end(); /* no increment here */) {
        // 创建下载线程
        KDownloaderThread* threadDownloadQMap = new KDownloaderThread(i.key(), i.value(), this->filePathKDownloadChild + std::get<2>(i.value()));
        //KDownloaderThread* threadDownloadQMap = new KDownloaderThread(i.value(), this->filePathKDownloadChild + std::get<2>(i.value()));
        // connect(threadDownloadQMap,&KDownloaderThread::downloadCompeleted0,this,[](){
        //     qDebug()<<"ooooooooooooooooooo";
        // });
        testPoolMutexLocker.lock();
        testpool->start(threadDownloadQMap);
        testPoolMutexLocker.unlock();
        //threadDownloadQMap->start();
        // 删除当前元素并更新迭代器
        //threadDownloadQMap->msleep(50);
        i = downloadQMap.erase(i); // erase 返回下一个有效迭代器

    }




    // 遍历 fileMD5Vector

    //     // 复制一份
    // QVector<std::tuple<QString, QString, QString, QString, bool>> toDelete;
    // qDebug()<<fileMD5Vector.size();
    // for (QVector<std::tuple<QString, QString, QString, QString, bool>>::iterator i = fileMD5Vector.begin(); i != fileMD5Vector.end(); ++i) {
    //     KDownloaderThread* threadDownloadQMap = new KDownloaderThread(*i, this->filePathKDownloadChild + std::get<2>(*i));
    //     // connect(threadDownloadQMap, &KDownloaderThread::downloadCompeleted, threadDownloadQMap, [threadDownloadQMap]() {
    //     //         //qDebug()<<"ddddddddddddddddddddddddddddddddddddd";
    //     //         delete threadDownloadQMap;
    //     //     }, Qt::QueuedConnection);
    //     testPoolMutexLocker.lock();
    //     testpool->start(threadDownloadQMap);
    //     testPoolMutexLocker.unlock();
    //     //threadDownloadQMap->start();
    //     //threadDownloadQMap->msleep(100);

    //     // 将当前元素添加到待删除列表中
    //     toDelete.push_back(*i);
    // }

    // // 在循环结束后删除所有待删除的元素
    // for (const auto& item : toDelete) {
    //     fileMD5Vector.erase(std::remove(fileMD5Vector.begin(), fileMD5Vector.end(), item), fileMD5Vector.end());
    // }



    //QThreadPool::globalInstance()->waitForDone(); // 确保所有任务完成



    //emit ok();

    //QCoreApplication::exec();
    //for (auto i = fileQMap.begin(); i != fileQMap.end(); i++) {
    //	KDownloaderChild *downloader=new KDownloaderChild(i.value(), this->filePathKDownloadChild);
    //
    //}


    //for (auto i = fileMD5Vector.begin(); i != fileMD5Vector.end(); i++) {

    //}




    //QString fileUrl = "https://ftp.ncbi.nlm.nih.gov/blast/db/18S_fungal_sequences.tar.gz";
    //   QString filename = "18S_fungal_sequences.tar.gz";

    //   std::tuple<QString, QString, QString, QString, bool> file = { "https://ftp.ncbi.nlm.nih.gov/blast/db/18S_fungal_sequences.tar.gz" ,"57","M","18S_fungal_sequences",false };
    //   // 文件保存目录
    //   QString downloadDir = "C:/Users/28707/Desktop/soft/REPTILE_TEST/REPTILE_TEST/out/build/downloadtest";

    //   // 创建 Downloader 对象并开始下载
    //  KDownloaderChild downloader(file, downloadDir);

    //for (auto i = fileQMap.begin(); i != fileQMap.end(); i++) {
    //	KDownloaderChild *downloader=new KDownloaderChild(i.value(), this->filePathKDownloadChild);
    //	//downloader->startDownload();
    //}

    //for (auto i = downloadQMap.begin(); i != downloadQMap.end(); i++) {
    //	KDownloaderChild* downloader = new KDownloaderChild(i.value(), this->filePathKDownloadChild);
    //	//downloader->startDownload();
    //}

    //for (auto i = fileMD5Vector.begin(); i != fileMD5Vector.end(); i++) {

    //}

    //// 定义文件信息
    //QString fileUrl = "https://ftp.ncbi.nlm.nih.gov/blast/db/16S_ribosomal_RNA-nucl-metadata.json";
    //QString fileSize = "1024";
    //QString unit = "KB";
    //QString filename = "18S_fungal_sequences.tar.gz.md5";
    //bool isMD5Suffix = true;
    //std::tuple<QString, QString, QString, QString, bool> fileInfo = std::make_tuple(fileUrl, fileSize, unit, filename, isMD5Suffix);

    //// 指定文件下载目录
    //QString downloadDir = "C:/Users/28707/Desktop/soft/REPTILE_TEST/REPTILE_TEST/out/build/downloadtest";

    //KDownloaderChild downloader(fileInfo, downloadDir);


}



void KDownloader::onFinished(QNetworkReply* reply) {




    if (reply->error() == QNetworkReply::NoError) {
        QString html = reply->readAll();
        //qDebug()<<html;
        QVariant requestInfoVariant = reply->property("requestInfo");
        QString dir="";
        //qDebug()<<reply->url().toString()<<isFirstRecordUrl;
        if(isFirstRecordUrl){
            //firstRecordUrl=reply->url().toString().split("/")[reply->url().toString().split("/").size()-2];
            firstRecordUrl=reply->url().toString();
            firestRecordUrlSize=firstRecordUrl.size();
        }else{
            dir=reply->url().toString().mid(firestRecordUrlSize,reply->url().toString().size()-firestRecordUrlSize);
        }
        parseLinks(html,dir);
    }
    else {
        Klog::GetStaticKlog()->write("["+QDateTime::currentDateTime().toString("HH:mm:ss")+"]"+reply->errorString());
        qDebug() << "Error:" << reply->errorString();

    }
    //reply->deleteLater();
    //manager->deleteLater();
}

void KDownloader::parseLinks(const QString& html,QString fileDir) {
    //qDebug()<<fileDir;
    //QRegularExpression regex("<a href=\"([^\"]+)\">");
    //QRegularExpression regex("<a href=\"([^\"]+)\">[^<]+</a>\\s*\\d{4}-\\d{2}-\\d{2}\\s+\\d{2}:\\d{2}\\s+(\\d+\\s*[KMG])");
    //QRegularExpression regex("<a href=\"([^\"]+)\">[^<]+</a>\\s*\\d{4}-\\d{2}-\\d{2}\\s+\\d{2}:\\d{2}\\s+(\\d+)(\\s*[KMG]?)");
    //QRegularExpression regex("<a href=\"([^\"]+)\">[^<]+</a>\\s*\\d{4}-\\d{2}-\\d{2}\\s+\\d{2}:\\d{2}\\s+(\\d+)(\\s*[KMG]?)");
    //QRegularExpression regex("<a href=\"([^\"]+)\">([^<]+)</a>\\s+(\\d{4}-\\d{2}-\\d{2})\\s+(\\d{2}:\\d{2})\\s+(-|\\d+)(\\s*[KMG]?)?");
    QRegularExpression regex("<a href=\"([^\"]+)\">([^<]+)</a>\\s+(\\d{4}-\\d{2}-\\d{2})\\s+(\\d{2}:\\d{2})\\s+(-|\\d+)(\\s*[KMG]?)?");
    QRegularExpressionMatchIterator it = regex.globalMatch(html);
    while (it.hasNext())
    {

        while (fileDir.startsWith("/")) {
            fileDir.remove(0,1);
        }

        QRegularExpressionMatch match = it.next();
        QTextStream output(stdout);
        //output << match.captured(1)<<Qt::endl;
        QString filepath = match.captured(1);

        QString filesize = match.captured(5);
        QString unit = match.captured(6).trimmed();
        if(fileDir.indexOf("v5")==-1){
            if(recursion){
                if (!filepath.startsWith("/") && filepath.endsWith("/")) {
                    //qDebug() << filepath;
                    QDir(this->filePathKDownloadChild+ (this->filePathKDownloadChild.endsWith("/")?"":"/") +fileDir+"/" + filepath).mkpath(".");
                    //qDebug()<<this->filePathKDownloadChild+ (this->filePathKDownloadChild.endsWith("/")?"":"/") +fileDir + filepath;
                    connect(manager, &QNetworkAccessManager::finished, this, &KDownloader::onFinished,Qt::QueuedConnection);//,Qt::QueuedConnection
                    QNetworkRequest request(QUrl(fileUrlKDownloader.toString() + "/"+fileDir +filepath.replace("/","")));
                    //qDebug()<<fileUrlKDownloader.toString() + "/"+fileDir +filepath;
                    manager->get(request);
                    filepath+="/";
                }
            }
            if (!filepath.startsWith("/")&&!filepath.endsWith("/")) {
                //qDebug()<<filepath.split(".")[0];
                //if(filepath.split(".")[0]=="nt"){
                    //qDebug()<<fileDir<<" "<<filepath;
                    QString filename =""; //filepath.split(".")[0];
                    QString suffix = filepath.split(".")[filepath.split(".").size() - 1];
                    bool isMD5 = (suffix == "md5" || suffix == "MD5" || suffix == "mD5" || suffix == "Md5");
                    //bool filesuffix = (filepath.split(".")[filepath.split(".").size() - 1])== "md5"?true:false);
                    QString filepath_compelete = fileUrlKDownloader.toString() + (filePathKDownloadChild.endsWith("/")?"":"/") + fileDir + filepath;
                    //qDebug()<<fileUrlKDownloader.toString()<<" "<<fileDir<<" "<<filepath;
                    //qDebug()<<filepath_compelete;
                    //qDebug()<<filepath_compelete;
                    int n=0;
                    if(isMD5){
                        n=filepath.split(".").size()-1;
                    }else{
                        n=filepath.split(".").size();
                    }
                    for(int i=0;i<n;i++){
                        filename+=filepath.split(".")[i];
                        if(i!=n-1){
                            filename+=".";
                        }
                    }
                    //qDebug()<<filename;
                    // if(fileDir!=""){
                    //     qDebug()<<filepath_compelete;

                    // }

                    //filename=fileDir+(fileDir.endsWith("/")?"":"/")+filename;
                    //qDebug()<<filename;
                    // if(fileDir!=""){
                    //     qDebug() << filepath_compelete << filesize << unit << filename << isMD5;

                    // }
                    //qDebug()<<fileDir;
                    //qDebug()<<filepath_compelete;
                    tuple<QString, QString, QString,QString,bool> fileInfo = {filepath_compelete,filesize+unit,"/"+fileDir,filename,isMD5};
                    if (isMD5) {
                        //qDebug() << filepath_compelete << " " << isMD5;
                        fileMD5Vector.push_back(fileInfo);
                    }
                    else {
                        fileQMap.insert(filename, fileInfo);
                    }
                //}
            }
        }


    }
    processCompeleted();
}

//void KDownloader::fetchFiles(const QUrl& fileUrl) {
//	QNetworkRequest request(fileUrl);
//	request.setRawHeader("Range", "bytes=0-"); // 请求文件大小
//	QNetworkReply* reply = manager->get(request);
//	connect(reply, &QNetworkReply::finished, [this, reply, fileUrl]() {
//		QTextStream output(stdout);
//		if (reply->error() == QNetworkReply::NoError) {
//			QList<QByteArray> ranges = reply->rawHeaderList();
//			qint64 size = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
//			output << "File: " << fileUrl.toString() << " Size: " << size << "bytes" << Qt::endl;
//		}
//		else {
//			output << "error" << Qt::endl;
//		
//		
//		}
//	/*	reply->deleteLater();*/
//		});
//
//
//}
