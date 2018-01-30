
#include "ScServer.h"

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
		connect(udpSocket, SIGNAL(readyRead()), this, SLOT(oscMsgRecived()));
		connect(clockStatus, SIGNAL(timeout()), this, SLOT(status()));
	}

	void ScServer::setPath(QString path) {
		QString file = "scsynth";
		QString extension = "exe";
		mScServerPath = QString("%1/%2.%3").arg(path, file, extension);
		emit print(tr("ScServer::setPath (%1)").arg(mScServerPath));
	}
	void ScServer::setPort(int port) { udpSocketPort = port; }

	void ScServer::evaluate(QString pattern) {
		emit print(tr("ScServer::evaluate: %1").arg(pattern));
		sendOsc(pattern);
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

	void ScServer::oscMsgRecived()
	{
		PacketReader pr;
		while (udpSocket->hasPendingDatagrams())
		{
			size_t datagramSize = udpSocket->pendingDatagramSize();
			QByteArray array(datagramSize, 0);
			qint64 readSize = udpSocket->readDatagram(array.data(), datagramSize);
			if (readSize == -1)
				continue;

			pr.init(array.data(), datagramSize);
			while (pr.isOk() && (oscmsg = pr.popMessage()) != 0) { oscMsgParser(oscmsg); }
		}
	}
	void ScServer::oscMsgParser(Message *message) {
		QString pattern = QString::fromStdString(message->addressPattern());
		QString argTypes = QString::fromStdString(message->typeTags());
		int argCnt = argTypes.size();

		Message::ArgReader args = message->arg();

		if (message->match("/status.reply"))
		{
			int unused, ugenCount, synthCount, groupCount, defCount;
			float avgCPU, peakCPU;
			args.popInt32(unused)
				.popInt32(ugenCount)
				.popInt32(synthCount)
				.popInt32(groupCount)
				.popInt32(defCount)
				.popFloat(avgCPU)
				.popFloat(peakCPU)
				.isOkNoMoreArgs();
			emit statusReplay(ugenCount, synthCount, groupCount, defCount, avgCPU, peakCPU);
		}
		else if (message->match("/version.reply"))
		{
			std::string programName, versionPatch, gitBranchName, commitHash;
			int versionMajor, versionMinor;
			args.popStr(programName)
				.popInt32(versionMajor)
				.popInt32(versionMinor)
				.popStr(versionPatch)
				.popStr(gitBranchName)
				.popStr(commitHash)
				.isOkNoMoreArgs();

			emit print(tr(
				"ScServer::parseOscMsg /version.replay program:%1, version:%2.%3%4, branch:%5, commit:%6"
			).arg(
				QString::fromStdString(programName),
				QString::number(versionMajor),
				QString::number(versionMinor),
				QString::fromStdString(versionPatch),
				QString::fromStdString(gitBranchName),
				QString::fromStdString(commitHash)
			));
		}
		else
		{
			emit print("ScServer::parseOscMsg NOT matched pattern");
			emit print(tr("- pattern: %1 argCnt:%2, argTypes:%3").arg(
				pattern,
				QString::number(argCnt),
				argTypes
			));

			for (int i = 0; i < argCnt; i++) {
				if (args.isBool()) {
					bool boolean;
					args.popBool(boolean);
					emit print(tr("\t %1) - bool: %2").arg(QString::number(i), boolean ? "true" : "false"));
				}
				else if (args.isInt32()) {
					int num;
					args.popInt32(num);
					emit print(tr("\t %1) - int: %2").arg(QString::number(i), QString::number(num)));
				}
				else if (args.isFloat()) {
					float num;
					args.popFloat(num);
					emit print(tr("\t %1) - float: %2").arg(QString::number(i), QString::number(num)));
				}
				else if (args.isStr()) {
					std::string txt;
					args.popStr(txt);
					emit print(tr("\t %1) - string: %2").arg(QString::number(i), QString::fromStdString(txt)));
				}
			}
			args.isOkNoMoreArgs();
		}
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
	void ScServer::s_new(QString sDefName, int id) { sendOsc("/s_new", sDefName, id); }
	void ScServer::n_free(int id) { sendOsc("/n_free", id); }

	// create osc message //////////////////////////////////////

	void ScServer::sendOsc(QString pattern) {
	//	emit print(tr("ScServer::sendOsc: %1").arg(pattern));
		PacketWriter pw;
		Message msg;
		//msg.init(QStringLiteral("\"%1\"").arg(pattern).toStdString());
		msg.init(pattern.toStdString());
		pw.init().addMessage(msg);
		udpSocket->writeDatagram(pw.packetData(), pw.packetSize(), QHostAddress::LocalHost, udpSocketPort);
	}
	void ScServer::sendOsc(QString pattern, int num) {
		PacketWriter pw;
		Message msg;
		msg.init(pattern.toStdString()).pushInt32(num);
		pw.init().addMessage(msg);
		udpSocket->writeDatagram(pw.packetData(), pw.packetSize(), QHostAddress::LocalHost, udpSocketPort);
	}
	void ScServer::sendOsc(QString pattern, QString txt, int num) {
		PacketWriter pw;
		Message msg;
		msg.init(pattern.toStdString()).pushStr(txt.toStdString()).pushInt32(num);
		pw.init().addMessage(msg);
		udpSocket->writeDatagram(pw.packetData(), pw.packetSize(), QHostAddress::LocalHost, udpSocketPort);
	}

}


