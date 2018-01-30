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

		void quit();
		void status();
		void version();
		void d_load(QString);
		void s_new(QString, int);
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

		void parseOscMsg2(Message*);
		void parseOscMsg(ReceivedMessage);
		void parseOscBundle(ReceivedBundle);
		void printOscMsg();

		// create osc message //////////////////////////////////////
		
		void sendOsc(QString pattern);
				
		private slots:
		void processStateChanged(QProcess::ProcessState state);
		void processMsgRecived();
		void serverMsgRecived();

		void evaluate(QString);



		

	};

}

#endif // ! SCSERVER_H