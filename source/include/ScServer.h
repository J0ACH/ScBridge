#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>

namespace SC {

	class ScServer : public QProcess {
		Q_OBJECT

	public:
		ScServer(QObject *parent);

		void setPath(QString);

		public slots:
		void switchServer();
		void startServer();
		void stopServer();
		void evaluate(QString);

	signals:
		void print(QString);

	private:
		QString mScServerPath;
		QUdpSocket *udpSocket;
		int portTargetServer, portListenServer;

		enum class ServerState { OFF, BOOTING, ON, SHUTTING };
		ServerState mState;

		private slots:
		void processMsgRecived();
		void serverMsgRecived();
	};

}

#endif // ! SCSERVER_H