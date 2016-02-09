#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>
#include <QPainter>
#include <QFrame>
#include <QTimer>
#include <QDebug>

class Button : public QWidget
{
	Q_OBJECT

public:
	Button(QWidget *parent);
	~Button();

	QRectF boundingRect() const;

	void setIconPath(QString path);

protected:
	void paintEvent(QPaintEvent *);

private:
	bool isPressed, isOver;

	QString iconPath;

	void prepisePopisek();

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

	void addAction();
	int fadeTimeIn;
	int fadeTimeOut;
	int fps;
	int backgroundAlpha;

	QTimer *timer;

signals:
	void pressAct();

	public slots:
	void alphaUpdate();

	
};

#endif // BUTTON_H

