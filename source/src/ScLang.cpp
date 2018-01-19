
#include "ScLang.h"

namespace SC {

	ScLang::ScLang(QObject *parent) : QProcess(parent) {
		mIpcServer = new QLocalServer(this);
		mIpcSocket = NULL;
		mIpcServerName = "SCBridge_" + QString::number(QCoreApplication::applicationPid());
		mTerminationRequested = false;

		mState = InterpretState::OFF;
		lateFlagBreakTime = 500;

		connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
		connect(mIpcServer, SIGNAL(newConnection()), this, SLOT(onNewIpcConnection()));
		connect(
			this, SIGNAL(stateChanged(QProcess::ProcessState)),
			this, SLOT(onProcessStateChanged(QProcess::ProcessState))
		);
		//connect(this, SIGNAL(finished(int, ExitStatus)), this, SLOT(killInterpreter()));
	}

	void ScLang::reverse() {
		//if (mState == InterpretState::OFF) { this->startLanguage(); }
		//else if (mState == InterpretState::ON) { this->kill(); }

		switch (mState) {
		case InterpretState::OFF:
			startLanguage();
			break;
		default:
			stopLanguage();
		}
	}

	void ScLang::startLanguage() {

		if (mState != InterpretState::OFF) {
			emit print("Interpreter is already running.");
			return;
		}
		else
		{
			mState = InterpretState::BOOTING;
			emit changeState(mState);
			emit print("BridgeProcess::INTERPRET_BOOTING");

			QString configFile;
			QStringList sclangArguments;
			if (!configFile.isEmpty()) { sclangArguments << "-l" << configFile; }
			sclangArguments << "-i" << "scqt";

			start(mScLangPath, sclangArguments);
			bool processStarted = QProcess::waitForStarted();
			if (!processStarted)
			{
				mState = InterpretState::OFF;
				emit changeState(mState);
				emit print(tr("Failed to start interpreter!"));
			}
			/*
			else
			{
				if (!mIpcServer->isListening()) {
					// avoid a warning on stderr
					mIpcServer->listen(mIpcServerName);
				}
				QString command = QStringLiteral("ScIDE.connect(\"%1\")").arg(mIpcServerName);

				this->evaluate(command);
			}
			*/
		}
	}

	void ScLang::stopLanguage() {
		qDebug() << "ScLang::stopLanguage()";
		//if (mStateInterpret == StateInterpret::ON)
		//{
		this->evaluate("Server.killAll");
		this->evaluate("0.exit");
		//this->evaluate("Server.default.boot");
		//}
	}



	void ScLang::setPath(QString path) {
		QString file = "sclang";
		QString extension = "exe";
		mScLangPath = QString("%1/%2.%3").arg(path, file, extension);
		qDebug() << "ScLang::setPath " << mScLangPath;
	}

	bool ScLang::bridgeProcessRun() {
		if (mState == InterpretState::OFF) return false;
		return true;
	}

	void ScLang::onReadyRead() {
		if (mTerminationRequested) {
			// when stopping the language, we don't want to post for longer than 200 ms to prevent the UI to freeze
			if (QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - mTerminationRequestTime.toMSecsSinceEpoch() > 200)
				return;
		}

		QByteArray out = QProcess::readAll();
		QString postString = QString::fromUtf8(out);
		emit print(postString);
		/*
		QStringList postList = postString.split("\r\n");

		foreach(QString oneLine, postList)
		{
			oneLine = oneLine.replace("\t", "    ");
			emit print(oneLine);
			qDebug() << oneLine;
		}

		this->msgParser(postString);
		*/
	}

	void ScLang::evaluate(QString code) {
		//emit print(QString("ScLang::evaluate -> %1").arg(code));
		bool synced = false;
		bool silent = false;
		bool printing = false;

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
		//if (printing) { emit print(tr("evaluate: %1\r\n").arg(code)); }
		write(&commandChar, 1);
	}


	void ScLang::onNewIpcConnection() {
		qDebug() << "ScBridge::onNewIpcConnection";
		if (mIpcSocket)
			// we can handle only one ipc connection at a time
			mIpcSocket->disconnect();

		mIpcSocket = mIpcServer->nextPendingConnection();

		connect(mIpcSocket, SIGNAL(disconnected()), this, SLOT(finalizeConnection()));
		connect(mIpcSocket, SIGNAL(readyRead()), this, SLOT(onIpcData()));

	}
	void ScLang::finalizeConnection() {
		emit print("ScLang::finalizedConection()");
		mIpcData.clear();
		mIpcSocket->deleteLater();
		mIpcSocket = NULL;
	}

	void ScLang::onProcessStateChanged(QProcess::ProcessState state)
	{
		emit print("ScLang::onProcessStateChanged()");
		switch (state) {
		case QProcess::Starting:
			break;

		case QProcess::Running:
			onStart();
			break;

		case QProcess::NotRunning:
			//postQuitNotification();
			//mCompiled = false;
			break;
		}
	}


	void ScLang::onIpcData() {
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

				qDebug() << "ScLang::onIpcData selector:" << selector;
				onResponse(selector, message);
				//emit response(selector, message);
			}
		} while ((mReadSize == 0 && avail > 4) || (mReadSize > 0 && avail > mReadSize));
		/*
		*/

		/*

		mIpcData.append(mIpcSocket->readAll());

		while (mIpcData.size() - 1) {
			QBuffer receivedData(&mIpcData);
			receivedData.open(QIODevice::ReadOnly);

			QDataStream in(&receivedData);
			in.setVersion(QDataStream::Qt_4_6);
			QString selector, message;
			in >> selector;
			if (in.status() != QDataStream::Ok)
				return;

			in >> message;
			if (in.status() != QDataStream::Ok)
				return;
			mIpcData.remove(0, receivedData.pos());
			qDebug() << "ScLang::onIpcData selector:" << selector;
			emit print(tr("ScLang::onIpcData selector:%1 data:%2").arg(selector, message));
			//onResponse(selector, message);

		}
		*/

		//QString postString = QString::fromUtf8(mIpcData);
		//emit print(tr("ScLang::onIpcData(%1)").arg(postString));
	}

	qint32 ScLang::arrayToInt(QByteArray source)
	{
		qint32 temp;
		QDataStream data(&source, QIODevice::ReadWrite);
		data >> temp;
		return temp;
	}

	void ScLang::onResponse(const QString & selector, const QString & data)
	{
		if (selector == QStringLiteral("introspection")) {
			emit print("ScLang::onResponse selector: introspection");
			//using ScLanguage::Introspection;
			/*
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
			*/
		}

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

	void ScLang::onStart()
	{
		if (!mIpcServer->isListening()) // avoid a warning on stderr
			mIpcServer->listen(mIpcServerName);

		QString command = QStringLiteral("ScIDE.connect(\"%1\")").arg(mIpcServerName);
		evaluate(command);
	}


	void ScLang::msgParser(QString msg)
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