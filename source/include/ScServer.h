#ifndef SCSERVER_H
#define SCSERVER_H

#include <QDebug>
#include <QProcess>

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

		private slots:
		void incomingMsg();
	};

}

#endif // ! SCSERVER_H