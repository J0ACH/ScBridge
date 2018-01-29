
#include "ScServer.h"

#define IP_MTU_SIZE 1536
#define OUTPUT_BUFFER_SIZE 1024

namespace SC {

	ScServer::ScServer(QObject *parent) : QProcess(parent) {
		mScServerPath = "";
		udpSocket = new QUdpSocket(this);
		udpSocketPort = 8050;
		mState = ServerState::OFF;
		
		clockStatus = new QTimer(this);

		connect(
			this, SIGNAL(stateChanged(QProcess::ProcessState)),
			this, SLOT(processStateChanged(QProcess::ProcessState))
		);
		connect(this, SIGNAL(readyRead()), this, SLOT(processMsgRecived()));
		connect(udpSocket, SIGNAL(readyRead()), this, SLOT(serverMsgRecived()));
		connect(clockStatus, SIGNAL(timeout()), this, SLOT(status()));
	}

	void ScServer::setPath(QString path) {
		QString file = "scsynth";
		QString extension = "exe";
		mScServerPath = QString("%1/%2.%3").arg(path, file, extension);
		emit print(tr("ScServer::setPath (%1)").arg(mScServerPath));
	}
	void ScServer::setPort(int port) { udpSocketPort = port; }

	void ScServer::evaluate(QString code) {

		char buffer[IP_MTU_SIZE];
		OutboundPacketStream p(buffer, IP_MTU_SIZE);

		QByteArray ba = code.toLatin1();
		const char *msg = ba.data();

		if (code != "/status") {
			emit print(tr("ScServer::evaluate(%1)").arg(QString(msg)));
		}

		p.Clear();
		p << BeginMessage(msg) << EndMessage;
		udpSocket->writeDatagram(p.Data(), p.Size(), QHostAddress::LocalHost, udpSocketPort);
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
			emit print("Failed to start scserver!");
		}
		else
		{
			emit print("Start scserver!");
			udpSocket->connectToHost(QHostAddress::LocalHost, udpSocketPort);
			clockStatus->start(1000);
		}
	}
	void ScServer::stopServer() {
		mState = ServerState::SHUTTING;
		emit changeState(mState);
		clockStatus->stop();
		this->quit();
		//udpSocket->disconnectFromHost();
	}

	void ScServer::processStateChanged(QProcess::ProcessState state)
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
		QString argTypes = QString(message.TypeTags());

		ReceivedMessageArgumentStream args = message.ArgumentStream();
		ReceivedMessage::const_iterator arg = message.ArgumentsBegin();

		if (pattern != "/status.reply") {
			emit print("ScServer::parseOscMsg");
			emit print(tr("- pattern: %1 argCnt:%2, argTypes:%3").arg(
				pattern,
				QString::number(argCnt),
				argTypes
			));

			for (int i = 0; i < argCnt; i++) {
				if (arg->IsBool()) {
					bool a = (arg++)->AsBoolUnchecked();
					emit print(tr("\t %1) - bool: %2").arg(QString::number(i), a ? "true" : "false"));
				}
				else if (arg->IsInt32()) {
					int a = (arg++)->AsInt32Unchecked();
					emit print(tr("\t %1) - int: %2").arg(QString::number(i), QString::number(a)));
				}
				else if (arg->IsFloat()) {
					float a = (arg++)->AsFloatUnchecked();
					emit print(tr("\t %1) - float: %2").arg(QString::number(i), QString::number(a)));
				}
				else if (arg->IsString()) {
					const char *a = (arg++)->AsStringUnchecked();
					emit print(tr("\t %1) - string: %2").arg(QString::number(i), a));
				}
				else {
					emit print("\t %1) - unknown type");
				}
			}
		}

		if (pattern == "/done") {
			const char *cmd;
			args >> cmd;
			if (cmd == "/quit") {
				emit print("QUIT DONE");
			}
			else {
				emit print(tr("DONE: %1").arg(cmd));
			}
		}
		else if (pattern == "/status.reply") {
			int unused, ugenCount, synthCount, groupCount, defCount;
			float avgCPU, peakCPU;

			args >> unused >> ugenCount >> synthCount >> groupCount >> defCount >> avgCPU >> peakCPU;
			emit statusReplay(ugenCount, synthCount, groupCount, defCount, avgCPU, peakCPU);
		}
		else if (pattern == "/version.reply") {
			const char *programName, *versionPatch, *gitBranchName, *commitHash;
			int versionMajor, versionMinor;

			args >> programName >> versionMajor >> versionMinor >> versionPatch >> gitBranchName >> commitHash;

			emit print(tr("ScServer::parseOscMsg /version.replay program:%1, version:%2.%3%4, branch:%5, commit:%6").arg(
				programName,
				QString::number(versionMajor),
				QString::number(versionMinor),
				versionPatch,
				gitBranchName,
				commitHash
			));
		}
		else {

		}
	}
	void ScServer::parseOscBundle(ReceivedBundle bundle) {
		//QString pattern = QString(message.AddressPattern());
		emit print(tr("ScServer::parseOscBundle").arg(""));
	}

	////////////////////////////////////////

	void ScServer::quit() { sendOsc("/quit"); }
	void ScServer::status() { sendOsc("/status"); }
	void ScServer::version() { sendOsc("/version"); }

	void ScServer::d_load(QString path) {
		QByteArray ba = path.toLatin1();
		const char *msg = ba.data();
		//packetStream.Clear();
		//packetStream << BeginMessage("/d_load") << msg << EndMessage;
		//udpSocket->writeDatagram(packetStream.Data(), packetStream.Size(), QHostAddress::LocalHost, udpSocketPort);
	}
	void ScServer::s_new(QString sDefName, int id) {
		//packetStream.Clear();
		//packetStream << BeginMessage("/s_new") << sDefName.toLatin1().data() << id << EndMessage;
		//udpSocket->writeDatagram(packetStream.Data(), packetStream.Size(), QHostAddress::LocalHost, udpSocketPort);
	};
	void ScServer::n_free(int id) {
		//packetStream.Clear();
		//packetStream << BeginMessage("/n_free") << id << EndMessage;
		//udpSocket->writeDatagram(packetStream.Data(), packetStream.Size(), QHostAddress::LocalHost, udpSocketPort);
	};

	// create osc message //////////////////////////////////////

	void ScServer::sendOsc(QString pattern) {
		char buffer[OUTPUT_BUFFER_SIZE];
		OutboundPacketStream packetStream(buffer, OUTPUT_BUFFER_SIZE);
		packetStream << BeginMessage(pattern.toLatin1().data()) << EndMessage;
		udpSocket->writeDatagram(packetStream.Data(), packetStream.Size(), QHostAddress::LocalHost, udpSocketPort);
	}

}


