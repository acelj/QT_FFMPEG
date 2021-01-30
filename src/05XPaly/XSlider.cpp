#include "XSlider.h"

XSlider::XSlider(QWidget *parent)
	: QSlider(parent)
{
}

XSlider::~XSlider()
{
}

void XSlider::mousePressEvent(QMouseEvent * e)
{
	double pos = (double)e->pos().x() / (double)width();
	setValue(pos * this->maximum());
	//ԭ���¼�����
	QSlider::mousePressEvent(e);	// ������ܹ����������������صĵ�������ǵ�������沿��û�з�Ӧ������û����
	//QSlider::sliderReleased();	// �����ֻ�ܴ������¼�������κ�λ�ö��У����ǲ��ܴ������¼���

}
