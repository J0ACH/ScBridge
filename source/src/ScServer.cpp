
#include "ScServer.h"

namespace SC {

	ScServer::ScServer(QObject *parent) : QProcess(parent) {
		mScServerPath = "";
		connect(this, SIGNAL(readyRead()), this, SLOT(incomingMsg()));
	}

	void ScServer::setPath(QString path) {
		QString file = "scsynth";
		QString extension = "exe";
		mScServerPath = QString("%1/%2.%3").arg(path, file, extension);
		emit print(tr("ScServer::setPath (%1)").arg(mScServerPath));
	}

	void ScServer::begin() {
		this->setProgram(mScServerPath);
		// this->setArguments("");

		//if (mStateInterpret == StateInterpret::OFF) {

			//emit actPrint(tr("ScBridge::startInterpretr %1").arg(mIpcServerName), MessageType::STATUS);

			QStringList scServerArguments;
			scServerArguments << "-u" << "8080";

			start(mScServerPath, scServerArguments);
			bool processStarted = QProcess::waitForStarted();
			if (!processStarted)
			{
				emit print(tr("Failed to start scserver!"));
			}
			else
			{
				emit print(tr("Start scserver!"));
				/*
				if (!mIpcServer->isListening()) {
					// avoid a warning on stderr
					mIpcServer->listen(mIpcServerName);
				}
				QString command = QStringLiteral("ScIDE.connect(\"%1\")").arg(mIpcServerName);
				emit print("BridgeProcess::INTERPRET_BOOTING");
				mBridgeProcess = BridgeProcess::INTERPRET_BOOTING;
				this->evaluate(command);
				*/
			}
		//}
		//else
		//{
		//	emit print("Interpret is running");
		//}
	}

	void ScServer::incomingMsg() {
	
		QByteArray out = QProcess::readAll();
		QString postString = QString::fromUtf8(out);
		QStringList postList = postString.split("\r\n");

		foreach(QString oneLine, postList)
		{
			oneLine = oneLine.replace("\t", "    ");
			emit print(oneLine);
			qDebug() << oneLine;
		}
	}

}