#ifndef SCTESTER_H
#define SCTESTER_H

#include <QDebug>
#include <QTabWidget>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

#include "ScServer.h"
#include "ScLang.h"

using namespace SC;

class PageServer : public QWidget {
	Q_OBJECT

public:
	PageServer(QWidget *parent = Q_NULLPTR);

	public slots:
	void cmdLineEvaluated();

	//signals:
		//void codeEvaluated(QString);

private:
	ScServer * server;
	QGroupBox *groupSC;
	QLabel *status;
};

class PageLang : public QWidget {
	Q_OBJECT

public:
	PageLang(QWidget *parent = Q_NULLPTR);

	public slots:
	void cmdLineEvaluated();

private:
	ScLang * lang;
	QGroupBox *groupSC;
	QLabel *status;
};

#endif // ! SCTESTER_H