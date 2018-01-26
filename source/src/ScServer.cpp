
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
			emit print(tr("ScServer::evaluate(%1)").arg(code));
			QByteArray ba = code.toUtf8();
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
		emit print("ScLang::onProcessStateChanged()");

		switch (state) {
		case QProcess::Starting:
			emit print("ScServer::Starting");
			mState = ServerState::BOOTING;
			emit changeState(mState);
			break;

		case QProcess::Running:
			emit print("ScServer::Running");
			mState = ServerState::ON;
			emit changeState(mState);
			break;

		case QProcess::NotRunning:
			emit print("ScServer::NotRunning");
			mState = ServerState::OFF;
			emit changeState(mState);
			break;
		}
	}

	void ScServer::processMsgRecived() {

		emit print("ScServer::processMsgRecived() START !!!!!!!!");

		QByteArray out = QProcess::readAll();
		QString postString = QString::fromUtf8(out);
		QStringList postList = postString.split("\r\n");

		foreach(QString oneLine, postList)
		{
			oneLine = oneLine.replace("\t", "    ");
			emit print(oneLine);
			qDebug() << oneLine;
		}

		emit print("ScServer::processMsgRecived() END !!!!!!!!");
	}

	void ScServer::serverMsgRecived()
	{
		/*
		QByteArray datagram;
		QHostAddress sender;
		quint16 senderPort;

		while (udpSocket->hasPendingDatagrams())
		{
			datagram.resize(udpSocket->pendingDatagramSize());
			udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
		}

		QString postString = QString::fromUtf8(datagram);
		QString postSize = QString::number(datagram.size());
		QString postSender = sender.toString();
		QString postPort = QString::number(senderPort);

		emit print(tr("ScServer::serverMsgRecived /n/t - data: %1 /n/t - size: %2 /n/t - sender: %3, /n/t - port: %4").arg(
			postString, postSize, postSender, postPort)
		);
		*/

		while (udpSocket->hasPendingDatagrams())
		{
			size_t datagramSize = udpSocket->pendingDatagramSize();
			QByteArray array(datagramSize, 0);
			qint64 readSize = udpSocket->readDatagram(array.data(), datagramSize);
			if (readSize == -1)
				continue;

			//processOscPacket(osc::ReceivedPacket(array.data(), datagramSize));

			/*
			*/
			QString postString = QString::fromUtf8(array.data());
			QString postSize = QString::number(datagramSize);
			emit print(tr("ScServer::serverMsgRecived /n/t - data: %1 /n/t - size: %2").arg(postString, postSize));
		}

		/*
		foreach(QString oneLine, list)
		{
			emit print(tr("ScServer::serverMsgRecived /n/t - list: %1").arg(oneLine));
		}
		*/
	}


}


