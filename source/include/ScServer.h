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
		void begin();
		void evaluate(QString);
		void kill();

	signals:
		void print(QString);

	private:
		QString mScServerPath;
		QUdpSocket *udpSocket;
		int portTargetServer, portListenServer;

		private slots:
		void processMsgRecived();
		void serverMsgRecived();
	};

}

#endif // ! SCSERVER_H