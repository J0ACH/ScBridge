
#include "ScServer.h"

namespace SC {

	ScServer::ScServer(QObject *parent) : QProcess(parent) {
		mScServerPath = "";
		portTargetServer = 8050;
		portListenServer = 8051;
		mState = ServerState::OFF;
		connect(this, SIGNAL(readyRead()), this, SLOT(processMsgRecived()));
	}

	void ScServer::setPath(QString path) {
		QString file = "scsynth";
		QString extension = "exe";
		mScServerPath = QString("%1/%2.%3").arg(path, file, extension);
		emit print(tr("ScServer::setPath (%1)").arg(mScServerPath));
	}

	void ScServer::begin() {
		QStringList scServerArguments;
		if (mState == ServerState::OFF)
		{
			scServerArguments << "-u" << QString::number(portTargetServer);
			start(mScServerPath, scServerArguments);
			mState = ServerState::BOOTING;
			bool processStarted = QProcess::waitForStarted();
			if (!processStarted)
			{
				mState = ServerState::OFF;
				emit print(tr("Failed to start scserver!"));
			}
			else
			{
				emit print(tr("Start scserver!"));
				udpSocket = new QUdpSocket(this);
				udpSocket->bind(QHostAddress::LocalHost, portListenServer);
				connect(udpSocket, SIGNAL(readyRead()), this, SLOT(serverMsgRecived()));
				mState = ServerState::ON;
			}
		}
		else
		{
			emit print("ScServer is running");
		}
	}

	void ScServer::evaluate(QString code) {
		if (mState == ServerState::ON)
		{
			emit print(tr("ScServer::evaluate(%1)").arg(code));
			QByteArray ba = code.toUtf8();
			udpSocket->writeDatagram(ba.data(), ba.size(), QHostAddress::LocalHost, portTargetServer);
		}
	}

	void ScServer::kill() {
		evaluate("/quit");
		//udpSocket->close();
		mState = ServerState::OFF;
	}

	void ScServer::reverse() {
		if (mState == ServerState::OFF) { this->begin(); }
		else if (mState == ServerState::ON) { this->kill(); }

	}

	void ScServer::processMsgRecived() {

		//emit print("ScServer::incomingMsg");

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

	void ScServer::serverMsgRecived()
	{
		QByteArray datagram;
		QHostAddress sender;
		quint16 senderPort;

		while (udpSocket->hasPendingDatagrams())
		{
			datagram.resize(udpSocket->pendingDatagramSize());
			udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
		}

		QString postString = QString::fromUtf8(datagram);
		emit print(tr("ScServer::serverMsgRecived (%1)").arg(postString));
	}


}


