
#include "ScBridge.h"

namespace SC {

	ScBridge::ScBridge(QObject *parent, QString userName) : QProcess(parent) {
		qDebug() << "ScBridge username:" << userName;
		mIpcServer = new QLocalServer(this);
		mIpcSocket = NULL;
		mIpcServerName = "SCBridge_" + QString::number(QCoreApplication::applicationPid());
		mTerminationRequested = false;

		lateFlagBreakTime = 500;

		mStateInterpret = StateInterpret::OFF;
		mStateServer = StateServer::OFF;
		mBridgeProcess = BridgeProcess::NaN;

		connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
		//connect(this, SIGNAL(finished(int, ExitStatus)), this, SLOT(killInterpreter()));
		//connect(mIpcServer, SIGNAL(newConnection()), this, SLOT(onNewIpcConnection()));

		this->setPath("C:/Program Files/SuperCollider-3.8.0");
	}

	void ScBridge::begin() { this->actInterpretStart(); }

	void ScBridge::kill() {
		qDebug() << "ScBridge::kill()";
		//if (mStateInterpret == StateInterpret::ON)
		//{
		this->evaluate("Server.killAll");
		this->evaluate("0.exit");
		//this->evaluate("Server.default.boot");
		//}
	}

	void ScBridge::setPath(QString path) {
		QString file = "sclang";
		QString extension = "exe";
		mScLangPath = QString("%1/%2.%3").arg(path, file, extension);
		qDebug() << "ScBridge2::setPath " << mScLangPath;
	}

	void ScBridge::actInterpretStart() {
		this->setProgram(mScLangPath);
		// this->setArguments("");

		if (mStateInterpret == StateInterpret::OFF) {

			//emit actPrint(tr("ScBridge::startInterpretr %1").arg(mIpcServerName), MessageType::STATUS);

			QString sclangCommand = "sclang";
			QString configFile;

			QStringList sclangArguments;
			if (!configFile.isEmpty())
				sclangArguments << "-l" << configFile;
			sclangArguments << "-i" << "scqt";

			start(mScLangPath, sclangArguments);
			bool processStarted = QProcess::waitForStarted();
			if (!processStarted)
			{
				emit print(tr("Failed to start interpreter!"));
			}
			else
			{
				if (!mIpcServer->isListening()) {
					// avoid a warning on stderr
					mIpcServer->listen(mIpcServerName);
				}
				QString command = QStringLiteral("ScIDE.connect(\"%1\")").arg(mIpcServerName);
				this->evaluate(command);
				emit print("BridgeProcess::INTERPRET_BOOTING");
				mBridgeProcess = BridgeProcess::INTERPRET_BOOTING;
			}
		}
		else
		{
			emit print("Interpret is running");
		}
	}
	void ScBridge::interpretStarted() {
		mStateInterpret = StateInterpret::ON;
		mBridgeProcess = BridgeProcess::NaN;
		emit print("Interpret start finish");
	}

	void ScBridge::onReadyRead() {
		if (mTerminationRequested) {
			// when stopping the language, we don't want to post for longer than 200 ms to prevent the UI to freeze
			if (QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - mTerminationRequestTime.toMSecsSinceEpoch() > 200)
				return;
		}

		QByteArray out = QProcess::readAll();
		QString postString = QString::fromUtf8(out);
		QStringList postList = postString.split("\r\n");

		foreach(QString oneLine, postList)
		{
			oneLine = oneLine.replace("\t", "    ");
			emit print(oneLine);
			qDebug() << oneLine;
		}

		this->msgParser(postString);
	}

	void ScBridge::onNewIpcConnection() {
		qDebug() << "ScBridge::onNewIpcConnection";
		if (mIpcSocket)
			// we can handle only one ipc connection at a time
			mIpcSocket->disconnect();

		mIpcSocket = mIpcServer->nextPendingConnection();

		connect(mIpcSocket, SIGNAL(disconnected()), this, SLOT(finalizeConnection()));
		connect(mIpcSocket, SIGNAL(readyRead()), this, SLOT(onIpcData()));
	}

	void ScBridge::evaluate(QString code) {
		emit print(QString("ScBridge::evaluate -> %1").arg(code));
		bool synced = false;
		bool silent = false;
		bool printing = false;

		qint64 evalTime = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
		qint64 syncTime;
		int durationTime;

		QTimer time;
		time.setSingleShot(true);
		time.start(lateFlagBreakTime);
		//answer.append("NaN");

		if (state() != QProcess::Running) {
			emit print("Interpreter is not running!");
			return;
		}

		QString command;
		{
			QEventLoop loop;
			loop.connect(&time, SIGNAL(timeout()), SLOT(quit()));
			loop.connect(this, SIGNAL(actSynced()), SLOT(quit()));
			command = QStringLiteral("[\"syncFlag\",%1]").arg(code);

			QByteArray bytesToWrite = command.toUtf8();
			size_t writtenBytes = write(bytesToWrite);

			if (writtenBytes != bytesToWrite.size()) {
				emit print("Error when passing data to interpreter!");
				return;
			}

			char commandChar = silent ? '\x1b' : '\x0c';
			if (printing) { emit print(tr("evaluate: %1\r\n").arg(code)); }
			write(&commandChar, 1);

			loop.exec();
		}

		syncTime = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
		durationTime = syncTime - evalTime;

		if (printing) {
			emit print(tr("synced [%1 ms]\r\n").arg(QString::number(durationTime)));
		}


	}

	void ScBridge::msgParser(QString msg)
	{
		//qDebug() << "msg: " << msg;
		//emit print(QString("msg: %1").arg(msg));

		if (msg.contains("ERROR"))
		{
			emit print(QString("ERROR: %1").arg(msg));

			/*
			QStringList msgLines = postString.split("\n");
			for (int i = 0; i < msgLines.size(); i = i + 1)
			{
			QString msg = msgLines.at(i);
			msg = msg.replace("\r", "");

			if (msg.startsWith("ERROR:"))
			{
			emit msgErrorAct(msg);
			}
			else
			{
			//emit
			}
			*/
		}
		else if (msg.contains("WARNING")) {
			emit print(QString("WARNING: %1").arg(msg));
		}
		else if (msg.contains("***")) {
			emit print(QString("STATUS: %1").arg(msg));
		}
		else if (msg.contains("->"))
		{
			if (msg.contains("syncFlag")) {
				//	emit actSynced(); 
				emit print(QString("SYNC: %1").arg(msg));
			}

			else if (msg.contains("answerFlag"))
			{
				//answer = QVariant();
				//qDebug() << "msg [answerFlag]: " << msg;
				QStringList incomingMSG = msg.split("->");
				foreach(QString oneMSG, incomingMSG)
				{
					if (oneMSG.contains("answerFlag"))
					{
						oneMSG = oneMSG.replace("\r", "");
						oneMSG = oneMSG.replace("\n", "");
						//qDebug() << "oneMSG: " << oneMSG;

						QStringList msgParts = oneMSG.split(",");
						if (msgParts.size() == 1)
						{
							QString onePart = msgParts.at(1);
							onePart = onePart.replace(" ", "");
							onePart = onePart.replace("[", "");
							onePart = onePart.replace("]", "");

							//qDebug() << "oneAnswer : " << msgParts;
							//answer = QVariant(onePart);
						}
						else
						{
							QStringList answerList;
							for (int i = 1; i < msgParts.size(); i = i + 1)
							{
								QString onePart = msgParts.at(i);
								onePart = onePart.replace(" ", "");
								onePart = onePart.replace("[", "");
								onePart = onePart.replace("]", "");

								answerList.append(onePart);
							}
							//answer = QVariant(answerList);
						}
					}
				}
				//emit actAnswered();
			}
			else {
				//qDebug() << msg;
				//if (msg.startsWith("\r\n"))	{ msg = msg.replace("\r\n", ""); }
				//if (!msg.isEmpty())	{ emit msgResultAct(msg); }
				//emit msgResultAct(tr("%1\r\n").arg(msg));
				emit print(QString("ANSWER: %1").arg(msg));
			}
		}
		else if (msg.contains("bundle")) {
			emit print(QString("BUNDLE: %1").arg(msg));
		}

		else {
			//qDebug() << msg;
			if (msg.startsWith("\r\n")) { msg = msg.replace("\r\n", ""); }
			if (!msg.isEmpty()) { emit print(msg); }
		}
	}

}