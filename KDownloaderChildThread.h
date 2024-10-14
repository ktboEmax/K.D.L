#pragma once
#include "qthread.h"

class KDownloaderChildThread : public QThread {
	Q_OBJECT
public:
	KDownloaderChildThread() = default;
	void down() {
		exec();


	}
};