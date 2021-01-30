#pragma once

///�������ʾ��Ƶ
struct AVPacket;
struct AVCodecParameters;
class XDecode;
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"
class XVideoThread :public XDecodeThread
{
public:
	// ����pts�� ������յ��Ľ�������pts >= seekpts    return true;    ������ʾ����
	virtual bool RepaintPts(AVPacket *pkt, long long seekpts);
	//�򿪣����ܳɹ��������
	virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);
/*	virtual void Push(AVPacket *pkt);*/
	void run();

	XVideoThread();
	virtual ~XVideoThread();
// 	//������
// 	int maxList = 1000;
// 	bool isExit = false;
	// ͬ��ʱ�����ⲿ����
	long long synpts = 0;    // ͬ����pts

	void SetPause(bool isPause);
	bool isPause = false;
	
protected:
/*	std::list <AVPacket *> packs;*/
	std::mutex vmux;
/*	XDecode *decode = 0;*/
	IVideoCall *call = 0;


};

