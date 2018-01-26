
#include "ScServer.h"

namespace SC {

	ScServer::ScServer(QObject *parent) : QProcess(parent) {
		mScServerPath = "";
		udpSocket = new QUdpSocket(this);
		udpSocketPort = 8050;
		mState = ServerState::OFF;

		connect(
			this, SIGNAL(stateChanged(QProcess::ProcessState)),
			this, SLOT(onProcessStateChanged(QProcess::ProcessState))
		);
		connect(this, SIGNAL(readyRead()), this, SLOT(processMsgRecived()));
		connect(udpSocket, SIGNAL(readyRead()), this, SLOT(serverMsgRecived()));
	}

	void ScServer::setPath(QString path) {
		QString file = "scsynth";
		QString extension = "exe";
		mScServerPath = QString("%1/%2.%3").arg(path, file, extension);
		emit print(tr("ScServer::setPath (%1)").arg(mScServerPath));
	}

	void ScServer::evaluate(QString code) {
		if (mState == ServerState::ON)
		{
			//QString msg = QStringLiteral("[%1]").arg(code);
			//emit print(tr("ScServer::evaluate(%1)").arg(msg));
			emit print(tr("ScServer::evaluate(%1)").arg(code));
			QByteArray ba = code.toUtf8();
			//QByteArray ba = msg.toUtf8();
			udpSocket->writeDatagram(ba.data(), ba.size(), QHostAddress::LocalHost, udpSocketPort);
		}
	}

	void ScServer::switchServer() {

		switch (mState) {
		case ServerState::OFF:
			startServer();
			break;
		default:
			stopServer();
		}
	}

	void ScServer::startServer() {
		QStringList scServerArguments;

		scServerArguments << "-u" << QString::number(udpSocketPort);
		start(mScServerPath, scServerArguments);
		bool processStarted = QProcess::waitForStarted();
		if (!processStarted)
		{
			emit print(tr("Failed to start scserver!"));
		}
		else
		{
			emit print(tr("Start scserver!"));
			udpSocket->connectToHost(QHostAddress::LocalHost, udpSocketPort);
		}
	}

	void ScServer::stopServer() {
		evaluate("/quit");
		//udpSocket->disconnectFromHost();
	}

	void ScServer::onProcessStateChanged(QProcess::ProcessState state)
	{
		switch (state) {
		case QProcess::Starting:
			mState = ServerState::BOOTING;
			break;
		case QProcess::Running:
			mState = ServerState::ON;
			break;
		case QProcess::NotRunning:
			mState = ServerState::OFF;
			break;
		}
		emit changeState(mState);
	}

	void ScServer::processMsgRecived() {

		QByteArray out = QProcess::readAll();
		QString postString = QString::fromUtf8(out);
		QStringList postList = postString.split("\r\n");

		foreach(QString oneLine, postList)
		{
			oneLine = oneLine.replace("\t", "    ");
			emit print(oneLine);
		}
	}

	void ScServer::serverMsgRecived()
	{
		QByteArray msg;
		int msgSize;

		while (udpSocket->hasPendingDatagrams())
		{
			size_t datagramSize = udpSocket->pendingDatagramSize();
			QByteArray array(datagramSize, 0);
			qint64 readSize = udpSocket->readDatagram(array.data(), datagramSize);
			if (readSize == -1) { continue; }
			msg = array;
			msgSize = datagramSize;
		}

		for (int i = 0; i < msgSize; i++) {
			QString ch = msg.at(i);
			emit print(tr("msg[%1]: %2").arg(QString::number(i), ch));
		}



		//emit print("ScServer::serverMsgRecived()");
		/*
		while (udpSocket->hasPendingDatagrams())
		{
			size_t datagramSize = udpSocket->pendingDatagramSize();
			QByteArray array(datagramSize, 0);
			qint64 readSize = udpSocket->readDatagram(array.data(), datagramSize);
			if (readSize == -1) { continue; }

			emit print(tr("ScServer read end readSize: %1").arg(QString::number(readSize)));

			pr.init(array.data(), datagramSize);
			oscpkt::Message *msg;
			while (pr.isOk() && (msg = pr.popMessage()) != 0) {
				if (msg->match("/quit").isOkNoMoreArgs()) {
					emit print("ScServer QUIT call");
				}
			}
		}
		*/



	}


}


