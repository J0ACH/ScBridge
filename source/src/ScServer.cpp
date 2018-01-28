
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
		/*
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
		*/

		while (udpSocket->hasPendingDatagrams())
		{
			size_t datagramSize = udpSocket->pendingDatagramSize();
			QByteArray array(datagramSize, 0);
			qint64 readSize = udpSocket->readDatagram(array.data(), datagramSize);
			if (readSize == -1)
				continue;

			ReceivedPacket packet = ReceivedPacket(array.data(), datagramSize);
			if (packet.IsMessage()) { parseOscMsg(ReceivedMessage(packet)); }
			else {
				ReceivedBundle bundle(packet);
				for (auto iter = bundle.ElementsBegin(); iter != bundle.ElementsEnd(); ++iter)
				{
					const ReceivedBundleElement & element = *iter;
					if (element.IsMessage()) { parseOscMsg(ReceivedMessage(element)); }
					else { parseOscBundle(ReceivedBundle(element)); }
				}
			}
		}
	}

	void ScServer::parseOscMsg(ReceivedMessage message) {
		QString pattern = QString(message.AddressPattern());
		int argCnt = message.ArgumentCount();

		emit print(tr("ScServer::parseOscMsg pattern: %1 argCnt:%2").arg(
			pattern,
			QString::number(argCnt)
		));

		if (pattern == "/status.reply") {
			int unused;
			int ugenCount;
			int synthCount;
			int groupCount;
			int defCount;
			float avgCPU;
			float peakCPU;

			auto args = message.ArgumentStream();

			args >> unused
				>> ugenCount
				>> synthCount
				>> groupCount
				>> defCount
				>> avgCPU
				>> peakCPU;


			emit print(tr("STATUS.REPLAY: ugenCnt:%1, synthCnt:%2, groupCnt:%3, defCount:%4, avgCPU:%5, peakCPU:%6").arg(
				QString::number(ugenCount),
				QString::number(synthCount),
				QString::number(groupCount),
				QString::number(defCount),
				QString::number(avgCPU),
				QString::number(peakCPU)
			));
		}
		else {

		}
	}
	void ScServer::parseOscBundle(ReceivedBundle bundle) {
		//QString pattern = QString(message.AddressPattern());
		emit print(tr("ScServer::parseOscBundle").arg(""));
	}



}


