#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>

#include "Oscpkt.h"
#include <iostream>

using namespace oscpkt;
using namespace std;

namespace SC {

	class ScServer : public QProcess
	{
		Q_OBJECT


	public:
		ScServer(QObject *parent);

		enum ServerState { OFF, BOOTING, ON, SHUTTING };

		void setPath(QString);

		public slots:
		void switchServer();
		void startServer();
		void stopServer();
		void evaluate(QString);

	signals:
		void print(QString);
		void changeState(ScServer::ServerState);

	private:
		QString mScServerPath;
		QUdpSocket *udpSocket;
		int udpSocketPort;
		ServerState mState;

		QByteArray oscData;
		
		PacketReader pr;

		private slots:
		void onProcessStateChanged(QProcess::ProcessState state);
		void processMsgRecived();
		void serverMsgRecived();
	};

}

#endif // ! SCSERVER_H