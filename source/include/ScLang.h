#ifndef SCLANG_H
#define SCLANG_H

#include <QDebug>
#include <QApplication>
#include <QProcess>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QBuffer>
#include <QDataStream>
#include <QDateTime>
#include <QTimer>

#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>

namespace SC {

	class ScLang : public QProcess {
		Q_OBJECT

	public:
		ScLang(QObject *parent);

		void setPath(QString);

		enum class InterpretState { OFF, BOOTING, ON, SHUTTING };

		public slots:
		void switchInterpretr();
		void startInterpreter();
		void stopInterpretr();
		void evaluate(QString);

	signals:
		void print(QString);
		void changeState(ScLang::InterpretState);

	private:
		qint32 arrayToInt(QByteArray);

		QLocalServer *mIpcServer;
		QLocalSocket *mIpcSocket;
		QString mScLangPath, mScSynthPath;
		QString mIpcServerName;

		QByteArray mIpcData;
		int mReadSize = 0;

		bool mTerminationRequested;
		QDateTime mTerminationRequestTime;

		InterpretState mState;

		private slots:
		void onProcessStateChanged(QProcess::ProcessState state);
		void onInterpreterMsg();

		void onNewIpcConnection();
		void onFinalizeIpcConnection();
		void onIpcMsg();
		void parserIpcMsg(QString, QString);
	};

} // namespace SupercolliderBridge





#endif // ! SCLANG_H
