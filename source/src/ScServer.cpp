
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

	void ScServer::evaluate(QString code) {
		//QString command;
				
		//QString command = QStringLiteral("[ \"%1,\" ]").arg(code);
		//'/s_new'
		QString command = QStringLiteral("[\'%1\']").arg(code);
		emit print(tr("ScServer::evaluate(%1)").arg(command));
		bool silent = false;
		QByteArray bytesToWrite = command.toUtf8();
		size_t writtenBytes = write(bytesToWrite);

		if (writtenBytes != bytesToWrite.size()) {
			emit print("Error when passing data to server!");
			return;
		}

		char commandChar = silent ? '\x1b' : '\x0c';
		write(&commandChar, 1);
		//write(bytesToWrite);
	}

	void ScServer::kill() {
		//int cmd_num = 3;
		evaluate("/quit");
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