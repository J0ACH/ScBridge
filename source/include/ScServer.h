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
		int port;
		QUdpSocket *udpSocket;
		QHostAddress *localAddress;

		void initSocket();
		

		private slots:
		void incomingMsg();
		void onDatagramRecived();
		void onSendData(QByteArray data);
	};

}

#endif // ! SCSERVER_H