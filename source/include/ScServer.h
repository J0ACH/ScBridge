#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>

//#include "oscpkt.hh"
//#include "udp.hh"
//using namespace oscpkt;


#include <osc/OscReceivedElements.h>
#include <osc/OscOutboundPacketStream.h>

//using namespace std;

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
		
		//PacketReader pr;


		void processOscMessage(const osc::ReceivedMessage &);

		void processOscPacket(const osc::ReceivedPacket & packet)
		{
			if (packet.IsMessage())
				processOscMessage(osc::ReceivedMessage(packet));
			else
				processOscBundle(osc::ReceivedBundle(packet));
		}

		void processOscBundle(const osc::ReceivedBundle & bundle)
		{
			for (auto iter = bundle.ElementsBegin(); iter != bundle.ElementsEnd(); ++iter)
			{
				const osc::ReceivedBundleElement & element = *iter;
				if (element.IsMessage())
					processOscMessage(osc::ReceivedMessage(element));
				else
					processOscBundle(osc::ReceivedBundle(element));
			}
		}



		private slots:
		void onProcessStateChanged(QProcess::ProcessState state);
		void processMsgRecived();
		void serverMsgRecived();
	};

}

#endif // ! SCSERVER_H