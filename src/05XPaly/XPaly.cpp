#include "XPaly.h"
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "XDemuxThread.h"
// static XDemuxThread *dt = 0;   // ָ��ĺô�= ���캯���ĵ��ò���Ӱ���ʼ��
static XDemuxThread dt;
XPaly::XPaly(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	
	dt.Start();

	connect(ui.openFile, &QPushButton::clicked, this, [this] {
		// ѡ���ļ�
		QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ"));
		if (name.isEmpty()) return;
		this->setWindowTitle(name);
		if (!dt.Open(name.toLocal8Bit(), ui.video))   // ��ʧ��
		{
			QMessageBox::information(0, "error", "open file failed!");
			return;
		}
		
		SetPause(dt.isPause);
		// qDebug() << name;
	});

	connect(ui.isPlay, &QPushButton::clicked, this, [this] {
		bool isPause = !dt.isPause;
		SetPause(isPause);
		dt.SetPause(isPause);
	});

	connect(ui.playPos, &QSlider::sliderPressed, this, [this] {
		isSliderPress = true;

	});
	connect(ui.playPos, &QSlider::sliderReleased, this, [this] {    // ���ɿ���ʱ����seek , ���ܱ�֤����Сһ��
		isSliderPress = false;
		double pos = 0.0;
		pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();
		dt.Seek(pos);
	});

	startTimer(40);    // 40ms �Ķ�ʱ���� һ��25֡

}
// ��ʱ���� ��������ʾ
void XPaly::timerEvent(QTimerEvent *e)
{
	if (isSliderPress) return;    // ��ס�������� ֱ�ӷ��أ�����ˢ�½���

	// һ��25 �Σ� �Ǻ�������
	long long total = dt.totalMs;   // ��ֵ���������Է�����̸߳���
	if (total > 0)
	{
		double pos = (double)dt.pts / (double)total;
		int v = ui.playPos->maximum() * pos;
		ui.playPos->setValue(v);
	}
}

// ���ڳߴ�仯
void XPaly::resizeEvent(QResizeEvent *e)
{
	ui.playPos->move(50, this->height() - 100);
	ui.playPos->resize(this->width() - 100, ui.playPos->height());
	ui.openFile->move(100, this->height() - 150);
	ui.video->resize(this->size());
	ui.isPlay->move(ui.openFile->x() + ui.openFile->width() + 10, ui.openFile->y());
}

void XPaly::mouseDoubleClickEvent(QMouseEvent * e)
{
	if (isFullScreen())
		this->showNormal();
	else
	{
		this->showFullScreen();
	}
}
void XPaly::SetPause(bool isPause)
{
	if (isPause)
		ui.isPlay->setText(QString::fromLocal8Bit("�� ��"));
	else
		ui.isPlay->setText(QString::fromLocal8Bit("�� ͣ"));
}
XPaly::~XPaly()
{
	dt.Close();
}
