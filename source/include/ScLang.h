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
		void evaluate(QString);

		enum class InterpretState { OFF, BOOTING, ON, SHUTTING };
		enum class MsgType {
			NORMAL,
			ERROR
		};

		public slots:
		void reverse();
		void startLanguage();
		void stopLanguage();

	signals:
		void print(QString);
		void changeState(ScLang::InterpretState);

	private:
		void onStart();
		void onResponse(const QString&, const QString&);
		qint32 arrayToInt(QByteArray);

		QLocalServer * mIpcServer;
		QLocalSocket *mIpcSocket;
		QString mScLangPath;
		QString mIpcServerName;
		QByteArray mIpcData;
		int mReadSize = 0;

		bool mTerminationRequested;
		QDateTime mTerminationRequestTime;
		int lateFlagBreakTime;


		InterpretState mState;
		bool bridgeProcessRun();


		void msgParser(QString);

		private slots:
		void onReadyRead();
		void onNewIpcConnection();
		void onProcessStateChanged(QProcess::ProcessState state);
		void onIpcData();
		void finalizeConnection();
	};

} // namespace SupercolliderBridge





#endif // ! SCLANG_H
