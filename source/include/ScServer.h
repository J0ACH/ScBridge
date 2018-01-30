#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>
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

		Message *oscmsg;
		QTimer *clockStatus;

		void oscMsgParser(Message*);

		// create osc message //////////////////////////////////////

		void sendOsc(QString);
		void sendOsc(QString, int);
		void sendOsc(QString, QString, int);

		private slots:
		void processStateChanged(QProcess::ProcessState state);
		void processMsgRecived();
		void oscMsgRecived();

		void evaluate(QString);





	};

}

#endif // ! SCSERVER_H