#ifndef CANVAN_H
#define CANVAN_H

#include <QtWidgets/QMainWindow>
#include <QDockWidget>
#include <QPushButton>
#include <QTimer>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QStyleFactory>

//#include "ui_canvan.h"

#include "Console.h"
#include "Button.h"

#include <QDebug>

class Canvan : public QMainWindow
{
	Q_OBJECT

public:
	Canvan(
		int originX = 100,
		int originY = 100,
		int sizeX = 600,
		int sizeY = 400
		);

	~Canvan();

	void setTitle(QString titleName);
	void setVersion(int major, int minor, int patch);

signals:
	void sendToConsole(QString);

	public slots:
	void msgConsole(QString);
	void closeCanvan();
	void minimizeCanvan();
	void maximizeCanvan();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void resizeCanvan(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	//Ui::CanvanClass ui;
	
	QMenuBar *menu;
	QWidget *header;
	QWidget *screen;
	QStatusBar *tail;

	QPushButton *closeButton;
	QPushButton *maximizeButton;
	QPushButton *minimizeButton;
	Button *testButton;

	QDockWidget *dock;
	Console *console;

	QLabel *title;
	QLabel *version;
	

	void mySetPalette();


	QPalette *palette;
	QColor *textColor;
	QColor *activeColor;
	QColor *backColor;
	QColor *panelColor;

	QPoint *mCursor;
};



#endif // CANVAN_H
