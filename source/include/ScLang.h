#ifndef SCLANG_H
#define SCLANG_H

#include <QDebug>
#include <QApplication>
#include <QProcess>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QDateTime>
#include <QTimer>

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
		void begin();
		void kill();
		void reverse();

	signals:
		void print(QString); 

	private:
		QLocalServer * mIpcServer;
		QLocalSocket *mIpcSocket;
		QString mScLangPath;
		QString mIpcServerName;

		bool mTerminationRequested;
		QDateTime mTerminationRequestTime;
		int lateFlagBreakTime;


		InterpretState mState;
		bool bridgeProcessRun();


		void msgParser(QString);

		private slots:
		void actInterpretStart();
		void interpretStarted();

		void onReadyRead();
		void onNewIpcConnection();
	};

} // namespace SupercolliderBridge





#endif // ! SCLANG_H
