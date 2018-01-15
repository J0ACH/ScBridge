
#include "ScBridge.h"

namespace SC {

	ScBridge::ScBridge(QObject *parent, QString userName) : QProcess(parent) {
		qDebug() << "ScBridge username:" << userName;
		mIpcServer = new QLocalServer(this);
		mIpcSocket = NULL;
		mIpcServerName = "SCBridge_" + QString::number(QCoreApplication::applicationPid());
		mTerminationRequested = false;

		mStateInterpret = StateInterpret::OFF;
		mStateServer = StateServer::OFF;
		mBridgeProcess = BridgeProcess::NaN;

		connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
		connect(this, SIGNAL(finished(int, ExitStatus)), this, SLOT(killInterpreter()));
		connect(mIpcServer, SIGNAL(newConnection()), this, SLOT(onNewIpcConnection()));

		this->setPath("C:/Program Files/SuperCollider-3.8.0");

		// mScLangPath = "C:/Program Files/SuperCollider-3.8.0/sclang.exe";
		// m_path = "C:/Program Files/SuperCollider-3.8.0/scsynth.exe";

	}

	void ScBridge::begin() {
		mBridgeProcess = BridgeProcess::INTERPRET_BOOTING;
		qDebug() << "mBridgeProcess = INTERPRET_BOOTING;";
		this->startInterpretr();
	}

	void ScBridge::end() {

	}

	void ScBridge::setPath(QString path) {
		QString file = "sclang";
		QString extension = "exe";
		mScLangPath = QString("%1/%2.%3").arg(path, file, extension);
		qDebug() << "ScBridge2::setPath " << mScLangPath;
	}

	void ScBridge::startInterpretr() {
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
			//QProcess::start(sclangCommand, sclangArguments);
			bool processStarted = QProcess::waitForStarted();
			if (!processStarted)
			{
				qDebug() << "Failed to start interpreter!";
				//emit actPrint(tr("Failed to start interpreter!"), MessageType::STATUS);
			}
			else
			{
				if (!mIpcServer->isListening()) {
					// avoid a warning on stderr
					mIpcServer->listen(mIpcServerName);
					qDebug() << "mIpcServer->listen";
				}
				QString command = QStringLiteral("ScIDE.connect(\"%1\")").arg(mIpcServerName);
				this->evaluate(command);
			}
		}
	}

	void ScBridge::killInterpreter() {
		qDebug() << "ScBridge::quit";

		if (state() != QProcess::Running) {
			emit print("Interpreter is not running!");
			return;
		}

		this->evaluate("0.exit");
		closeWriteChannel();

		//mCompiled = false;
		mTerminationRequested = true;
		mTerminationRequestTime = QDateTime::currentDateTimeUtc();

		bool finished = waitForFinished(200);
		if (!finished && (state() != QProcess::NotRunning)) {

#ifdef Q_OS_WIN32
			kill();
#else
			terminate();
#endif
			bool reallyFinished = waitForFinished(200);
			if (!reallyFinished)
				emit print("Failed to stop interpreter!");
			else
			{

			}
		}
		mTerminationRequested = false;
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

		//this->msgFilterNEW(postString);
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
	}

}