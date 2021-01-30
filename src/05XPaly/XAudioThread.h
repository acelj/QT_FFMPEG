#pragma once
#include <QThread>
#include <mutex>
#include <list>
struct AVCodecParameters;
class XDecode;
class XAudioPlay;
class XResample;
/*class AVPacket;*/
#include "XDecodeThread.h"
class XAudioThread :public XDecodeThread
{
public:
	// ��ǰ��Ƶ���ŵ�pts
	long long pts = 0;
	//�򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters *para, int sampleRate, int channels);   //����Ƶ��ص�ҵ��������

	//ֹͣ�̣߳�������Դ
	virtual void Close();
	virtual void Clear();

/*	virtual void Push(AVPacket *pkt);*/
	void run();
	XAudioThread();
	virtual ~XAudioThread();

	void SetPause(bool isPause);
	bool isPause = false;

// 	//������
// 	int maxList = 100;          // 44100 ����ÿ��44֡���ң�100֡��Ż���2���һ��
// 	bool isExit = false;
protected:

/*	std::list <AVPacket *> packs;   // ����������*/
	std::mutex amux;
/*	XDecode *decode = 0;*/
	XAudioPlay *ap = 0;
	XResample *res = 0;

};

