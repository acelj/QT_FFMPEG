#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPaly.h"

class XPaly : public QWidget
{
    Q_OBJECT

public:
    XPaly(QWidget *parent = Q_NULLPTR);
	~XPaly();

	// ��ʱ���� ��������ʾ(����)
	void timerEvent(QTimerEvent *e);
	
	// ���ڳߴ�仯
	void  resizeEvent(QResizeEvent *e);

	// ˫��ȫ��
	void mouseDoubleClickEvent(QMouseEvent *e);

	void SetPause(bool isPause);
private:
    Ui::XPalyClass ui;

	bool isSliderPress = false;
};
