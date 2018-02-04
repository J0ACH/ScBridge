#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QDateTime>
#include <QTimer>

#include <oscpkt.h>
using namespace oscpkt;

/*

const qint32 kSECONDS_FROM_1900_to_1970 = (qint32)2208988800UL; 
const double kOSCtoSecs = 2.328306436538696e-10;

const double kSecondsToOSCunits = 4294967296.; // pow(2,32)
const double kMicrosToOSCunits = 4294.967296; // pow(2,32)/1e6
const double kNanosToOSCunits = 4.294967296; // pow(2,32)/1e9

static inline std::chrono::system_clock::time_point getTime()
{
	return std::chrono::system_clock::now();
}

template <typename TimePoint>
static inline double secondsSinceEpoch(TimePoint const & tp)
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count() * 1e-9;
}

template <typename TimePoint>
static inline int64 OSCTime(TimePoint const & tp)
{
	using namespace std::chrono;
	typedef typename TimePoint::duration Duration;
	Duration sinceEpoch = tp.time_since_epoch();
	seconds secs = duration_cast<seconds>(sinceEpoch);

	nanoseconds nsecs = sinceEpoch - secs;

	return ((int64)(secs.count() + kSECONDS_FROM_1900_to_1970) << 32)
		+ (int64)(nsecs.count() * kNanosToOSCunits);
}

*/


namespace SC {

#define SEC_PER_YEAR 31536000
#define SEC_TO_EPOCH 2207520000




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
		void evaluate(QString parrent, QString arg1 = "", QString arg2 = "", QString arg3 = "");

		void notify(int, int);
		void quit();
		void status();
		void version();
		void d_load(QString);
		void s_new(QString, int);
		void n_free(int);

		void bundleTime();

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

		const quint64 sec_1900_1970 = 2208988800; // pozn.: pocet sekund bez 17 prestupnych let 
		const quint64 msec_1900_1970 = 2208988800000;// pozn.: pocet milisekund bez 17 prestupnych let 
		qint64 sec_1970_init;

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







	};

}

#endif // ! SCSERVER_H