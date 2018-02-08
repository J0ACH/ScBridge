#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QDateTime>

#include <QTimeZone>
#include <QTime>
#include <QTimer>

#include <oscpkt.h>
using namespace oscpkt;

namespace SC {

	class ScServer : public QProcess
	{
		Q_OBJECT

	public:
		ScServer(QObject *parent);

		enum ServerState { OFF, BOOTING, ON, SHUTTING };

		void setPath(QString);
		void setPort(int);

		public slots:
		void switchServer();
		void startServer();
		void stopServer();
		void evaluate(QString parent, QString arg1 = "", QString arg2 = "", QString arg3 = "");

		void initBundle(QTime time = QTime());
		void sendBundle();

		void notify(int, int);
		void quit();
		void status();
		void version();
		void d_load(QString);
		void s_new(QString, int);
		void n_free(int);

		void printBundleTime();
		void printBundleTimeQT();

	signals:
		void print(QString);
		void changeState(ScServer::ServerState);
		void statusReplay(int, int, int, int, float, float);

	private:
		QString mScServerPath;
		QUdpSocket *udpSocket;
		int udpSocketPort;
		ServerState mState;

		PacketWriter pw;

		Message *oscmsg;
		QTimer *clockStatus;

		void oscMsgParser(Message*);

		// send osc message //////////////////////////////////////

		enum CmdType {
			cmd_none,
			cmd_notify,
			cmd_status,
			cmd_quit,

			cmd_d_load,

			cmd_s_new,

			cmd_n_free,
			cmd_n_set,

			cmd_version
		};
		void sendOsc(CmdType, QVariant arg1 = QVariant(), QVariant arg2 = QVariant());

		private slots:
		void processStateChanged(QProcess::ProcessState state);
		void processMsgRecived();
		void oscMsgRecived();

		// timetag //////////////////////////////////////

		unsigned long long int bundleTime(QTime epoch);
		






	};

}

#endif // ! SCSERVER_H