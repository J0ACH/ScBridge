
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

	void ScServer::evaluate(QString pattern, QString arg1, QString arg2, QString arg3) {
		PacketWriter pw;
		Message msg;
		msg.init(pattern.toStdString());

		emit print(tr("ScServer::evaluate: pattern:%1").arg(pattern));

		if (!arg1.isEmpty()) {
			if (arg1.toInt()) { msg.pushInt32(arg1.toInt()); }
			else if (arg1.toFloat()) { msg.pushFloat(arg1.toFloat()); }
			else { msg.pushStr(arg2.toStdString()); }
			emit print(tr("ScServer::evaluate: arg1:%1").arg(arg1));
		}
		if (!arg2.isEmpty()) {
			if (arg2.toInt()) { msg.pushInt32(arg2.toInt()); }
			else if (arg2.toFloat()) { msg.pushFloat(arg2.toFloat()); }
			else { msg.pushStr(arg2.toStdString()); }
			emit print(tr("ScServer::evaluate: arg2:%1").arg(arg2));
		}
		if (!arg3.isEmpty()) {
			if (arg3.toInt()) {
				msg.pushInt32(arg3.toInt());
				emit print(tr("ScServer::evaluate: int arg3:%1").arg(arg3));
			}
			else if (arg3.toFloat()) {
				msg.pushFloat(arg3.toFloat());
				emit print(tr("ScServer::evaluate: float arg3:%1").arg(arg3));
			}
			else {
				msg.pushStr(arg3.toStdString());
				emit print(tr("ScServer::evaluate: string arg3:%1").arg(arg3));
			}
		}

		if (msg.isOk()) {
			pw.init().addMessage(msg);
			udpSocket->writeDatagram(pw.packetData(), pw.packetSize(), QHostAddress::LocalHost, udpSocketPort);
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

	////////////////////////////////////////

	void ScServer::notify(int receive, int id) { sendOsc(CmdType::cmd_notify, receive, id); }
	void ScServer::quit() { sendOsc(CmdType::cmd_quit); }
	void ScServer::status() { sendOsc(CmdType::cmd_status); }
	void ScServer::version() { sendOsc(CmdType::cmd_version); }

	void ScServer::d_load(QString path) { sendOsc(CmdType::cmd_d_load, path); }
	void ScServer::s_new(QString sDefName, int id) { sendOsc(CmdType::cmd_s_new, sDefName, id); }
	void ScServer::n_free(int id) { sendOsc(CmdType::cmd_n_free, id); }

	// recive osc message //////////////////////////////////////

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
		size_t time = message->timeTag();
		emit print(tr("ScServer::parseOscMsg timestamp:%1").arg(time));

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

	// send osc message //////////////////////////////////////

	void ScServer::sendOsc(CmdType pattern, QVariant arg1, QVariant arg2) {

		PacketWriter pw;
		Message msg;
		
		QDateTime start = QDateTime(QDate(1900,1,1), QTime(0,0,0,0));
		QDateTime epoch = QDateTime::currentDateTime();
		
		epoch.addSecs(10);
		qint64 timeint = epoch.currentSecsSinceEpoch();
		TimeTag t = TimeTag(timeint);

		switch (pattern)
		{
		case CmdType::cmd_notify:
			msg.init("/notify");
			if (arg1.isValid()) { msg.pushInt32(arg1.toInt()); }
			break;
		case CmdType::cmd_status:
			msg.init("/status");
			break;
		case CmdType::cmd_quit:
			msg.init("/quit");
			break;
		case CmdType::cmd_s_new:
			t = TimeTag(50000);
			msg.init("/s_new", t).pushStr(arg1.toString().toStdString()).pushInt32(arg2.toInt());
			break;
		case CmdType::cmd_n_free:
			msg.init("/n_free").pushInt32(arg1.toInt());
			break;
		case CmdType::cmd_n_set:
			msg.init("/n_set").pushInt32(arg1.toInt());
			break;
		case CmdType::cmd_version:
			msg.init("/version");
			break;
		default:
			emit print("ScServer::sendOsc NOT matched pattern");
			break;
		}

		if (msg.isOk()) {
			pw.init();
			pw.startBundle();
			pw.addMessage(msg);
			pw.endBundle();
			udpSocket->writeDatagram(pw.packetData(), pw.packetSize(), QHostAddress::LocalHost, udpSocketPort);
		}
	}

}


