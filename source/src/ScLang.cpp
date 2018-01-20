
#include "ScLang.h"

namespace SC {

	ScLang::ScLang(QObject *parent) : QProcess(parent) {
		mIpcServer = new QLocalServer(this);
		mIpcSocket = NULL;
		mIpcServerName = "SCBridge_" + QString::number(QCoreApplication::applicationPid());
		mTerminationRequested = false;

		mState = InterpretState::OFF;

		connect(this, SIGNAL(readyRead()), this, SLOT(onInterpreterMsg()));
		connect(mIpcServer, SIGNAL(newConnection()), this, SLOT(onNewIpcConnection()));
		connect(
			this, SIGNAL(stateChanged(QProcess::ProcessState)),
			this, SLOT(onProcessStateChanged(QProcess::ProcessState))
		);
		//connect(this, SIGNAL(finished(int, ExitStatus)), this, SLOT(killInterpreter()));
	}

	void ScLang::setPath(QString path) {
		QString file = "sclang";
		QString extension = "exe";
		mScLangPath = QString("%1/%2.%3").arg(path, file, extension);
		qDebug() << "ScLang::setPath " << mScLangPath;
	}

	void ScLang::switchInterpretr() {
		//if (mState == InterpretState::OFF) { this->startLanguage(); }
		//else if (mState == InterpretState::ON) { this->kill(); }

		switch (mState) {
		case InterpretState::OFF:
			startInterpreter();
			break;
		default:
			stopInterpretr();
		}
	}

	void ScLang::startInterpreter() {

		if (mState != InterpretState::OFF) {
			emit print("Interpreter is already running.");
			return;
		}
		else
		{
			QString configFile;
			QStringList sclangArguments;
			if (!configFile.isEmpty()) { sclangArguments << "-l" << configFile; }
			sclangArguments << "-i" << "scqt";

			start(mScLangPath, sclangArguments);
			bool processStarted = QProcess::waitForStarted();
			if (!processStarted)
			{
				emit print("Failed to start interpreter!");
			}
		}
	}

	void ScLang::stopInterpretr() {
		qDebug() << "ScLang::stopLanguage()";
		mState = InterpretState::SHUTTING;
		emit changeState(mState);
		emit print("InterpretState::SHUTTING");

		if (state() != QProcess::Running) {
			emit print("Interpreter is not running!");
			return;
		}

		evaluate("0.exit");
		mTerminationRequested = true;
		mTerminationRequestTime = QDateTime::currentDateTimeUtc();

		bool finished = waitForFinished(1000);
		if (!finished && (state() != QProcess::NotRunning)) {
			terminate();
			bool reallyFinished = waitForFinished(200);
			if (!reallyFinished)
				emit print("Failed to stop interpreter!");
		}
		closeWriteChannel();
		mTerminationRequested = false;
	}

	void ScLang::evaluate(QString code) {

		bool silent = false;

		if (state() != QProcess::Running) {
			emit print("Interpreter is not running!");
			return;
		}

		QByteArray bytesToWrite = code.toUtf8();
		size_t writtenBytes = write(bytesToWrite);

		if (writtenBytes != bytesToWrite.size()) {
			emit print("Error when passing data to interpreter!");
			return;
		}

		char commandChar = silent ? '\x1b' : '\x0c';
		write(&commandChar, 1);
	}

	void ScLang::onNewIpcConnection() {
		emit print("ScBridge::onNewIpcConnection");
		if (mIpcSocket)
			// we can handle only one ipc connection at a time
			mIpcSocket->disconnect();

		mIpcSocket = mIpcServer->nextPendingConnection();

		mState = InterpretState::ON;
		emit changeState(mState);
		emit print("InterpretState::ON");

		connect(mIpcSocket, SIGNAL(disconnected()), this, SLOT(onFinalizeIpcConnection()));
		//connect(mIpcSocket, SIGNAL(readyRead()), this, SLOT(onIpcMsg()));
	}

	void ScLang::onFinalizeIpcConnection() {
		emit print("ScLang::finalizedConection()");

		mIpcData.clear();
		mIpcSocket->deleteLater();
		mIpcSocket = NULL;

		mState = InterpretState::OFF;
		emit changeState(mState);
		emit print("InterpretState::OFF");
	}

	void ScLang::onProcessStateChanged(QProcess::ProcessState state)
	{
		emit print("ScLang::onProcessStateChanged()");

		switch (state) {
		case QProcess::Starting:
			break;

		case QProcess::Running:
			mState = InterpretState::BOOTING;
			emit changeState(mState);
			emit print("InterpretState::BOOTING");

			if (!mIpcServer->isListening()) {
				// avoid a warning on stderr
				mIpcServer->listen(mIpcServerName);
			}
			this->evaluate(QStringLiteral("ScIDE.connect(\"%1\")").arg(mIpcServerName));
			break;

		case QProcess::NotRunning:
			QString message;
			switch (exitStatus()) {
			case QProcess::CrashExit:
				message = tr("Interpreter has crashed or stopped forcefully. [Exit code: %1]\n").arg(exitCode());
				break;
			default:
				message = tr("Interpreter has quit. [Exit code: %1]\n").arg(exitCode());
			}
			emit print(message);
			
			//mCompiled = false;
			break;
		}
	}

	void ScLang::onIpcMsg() {
		mIpcData.append(mIpcSocket->readAll());
		// After we have put the data in the buffer, process it
		int avail = mIpcData.length();
		do {
			if (mReadSize == 0 && avail > 4) {
				mReadSize = arrayToInt(mIpcData.left(4));
				mIpcData.remove(0, 4);
				avail -= 4;
			}

			if (mReadSize > 0 && avail >= mReadSize) {
				QByteArray baReceived(mIpcData.left(mReadSize));
				mIpcData.remove(0, mReadSize);
				mReadSize = 0;
				avail -= mReadSize;

				QDataStream in(baReceived);
				in.setVersion(QDataStream::Qt_4_6);
				QString selector, message;
				in >> selector;
				if (in.status() != QDataStream::Ok)
					return;

				in >> message;
				if (in.status() != QDataStream::Ok)
					return;

				//emit print(tr("ScLang::onResponse selector: %1; data:%2").arg(selector, message));
				/*
				mState = InterpretState::ON;
				emit changeState(mState);
				emit print("ScLang::onStart");
				*/

				if (selector == QStringLiteral("defaultServerRunningChanged")) {
					emit print("ScLang::onResponse selector: defaultServerRunningChanged");
					emit print(tr("ScLang::onResponse data: %1").arg(message));

					//mState = InterpretState::ON;
					//emit changeState(mState);
					//emit print("InterpretState::ON");
				}

				/*
				else if (selector == QStringLiteral("introspection")) {
				// vypise vsechy classy v SC
				//emit print("ScLang::onResponse selector: introspection");

				//using ScLanguage::Introspection;

				auto watcher = new QFutureWatcher<Introspection>(this);
				connect(watcher, &QFutureWatcher<Introspection>::finished, [=] {
				try {
				Introspection newIntrospection = watcher->result();
				mIntrospection = std::move(newIntrospection);
				emit introspectionChanged();
				}
				catch (std::exception & e) {
				MainWindow::instance()->showStatusMessage(e.what());
				}
				watcher->deleteLater();
				});

				// Start the computation.
				QFuture<Introspection> future = QtConcurrent::run([](QString data) {
				return ScLanguage::Introspection(data);
				}, data);
				watcher->setFuture(future);

				}
				*/

				else if (selector == QStringLiteral("classLibraryRecompiled")) {
					emit print("ScLang::onResponse selector: classLibraryRecompiled");
					//mCompiled = true;
					//emit classLibraryRecompiled();
				}

				else if (selector == QStringLiteral("requestCurrentPath")) {

					emit print("ScLang::onResponse selector: requestCurrentPath");
					//	Main::documentManager()->sendActiveDocument();
				}
			}
		} while ((mReadSize == 0 && avail > 4) || (mReadSize > 0 && avail > mReadSize));

	}

	qint32 ScLang::arrayToInt(QByteArray source)
	{
		qint32 temp;
		QDataStream data(&source, QIODevice::ReadWrite);
		data >> temp;
		return temp;
	}

	void ScLang::onInterpreterMsg()
	{
		if (mTerminationRequested) {
			// when stopping the language, we don't want to post for longer than 200 ms to prevent the UI to freeze
			if (QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - mTerminationRequestTime.toMSecsSinceEpoch() > 200)
				return;
		}

		QByteArray out = QProcess::readAll();
		QString msg = QString::fromUtf8(out);
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