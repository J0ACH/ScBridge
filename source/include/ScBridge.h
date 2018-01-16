#ifndef SCBRIDGE_H
#define SCBRIDGE_H

#include <QDebug>
#include <QApplication>
#include <QProcess>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QDateTime>
#include <QTimer>

namespace SC {

	class ScBridge : public QProcess {
		Q_OBJECT

	public:
		ScBridge(QObject *parent, QString name);
		
		void setPath(QString);
		void evaluate(QString);

		enum class BridgeMsgType {
			NORMAL,
			ERROR
		};

		public slots:
		void begin();
		void kill();

	signals:
		void print(QString); //, MsgType msgType = MsgType::NORMAL

	private:
		QLocalServer * mIpcServer;
		QLocalSocket *mIpcSocket;
		QString mScLangPath;
		QString mIpcServerName;

		bool mTerminationRequested;
		QDateTime mTerminationRequestTime;
		int lateFlagBreakTime;

		enum class StateInterpret { OFF, ON };
		enum class StateServer { OFF, ON };
		enum class BridgeProcess {
			NaN,
			INTERPRET_BOOTING,
			INTERPRET_KILLING,
			SERVER_BOOTING,
			SERVER_KILLING
		};

		StateInterpret mStateInterpret;
		StateServer mStateServer;
		BridgeProcess mBridgeProcess;

		void msgParser(QString);

		private slots:
		void actInterpretStart();
		void interpretStarted();

		void onReadyRead();
		void onNewIpcConnection();
	};

} // namespace SupercolliderBridge





#endif // ! SCBRIDGE_H
