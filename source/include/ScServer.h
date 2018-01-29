#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QTimer>

#include <osc/OscReceivedElements.h>
#include <osc/OscOutboundPacketStream.h>
using namespace osc;

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

		void quit();
		void status();
		void version();
		void d_load(QString);
		void s_new(int);
		void n_free(int);

	signals:
		void print(QString);
		void changeState(ScServer::ServerState);
		void statusReplay(int, int, int, int, float, float);

	private:
		QString mScServerPath;
		QUdpSocket *udpSocket;
		int udpSocketPort;
		ServerState mState;

		QTimer *clockStatus;

		void parseOscMsg(ReceivedMessage);
		void parseOscBundle(ReceivedBundle);

		private slots:
		void onProcessStateChanged(QProcess::ProcessState state);
		void processMsgRecived();
		void serverMsgRecived();

		void evaluate(QString);
		void evaluate(QString, int);
	};

}

#endif // ! SCSERVER_H