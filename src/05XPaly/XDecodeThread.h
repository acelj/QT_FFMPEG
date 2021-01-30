#pragma once
/// �����XAudioThread ��XVideoThread �Ĺ�������
///�������ʾ��Ƶ
struct AVPacket;
class XDecode;
#include <list>
#include <mutex>
#include <QThread>
class XDecodeThread :public QThread
{
public:
	XDecodeThread();
	virtual ~XDecodeThread();
	virtual void Push(AVPacket *pkt);

	//�������
	virtual void Clear();

	//������Դ��ֹͣ�߳�
	virtual void Close();

	//ȡ��һ֡���ݣ�����ջ�����û�з���NULL
	virtual AVPacket *Pop();
	//������
	int maxList = 100;
	bool isExit = false;
	
protected:
	std::list <AVPacket *> packs;
	std::mutex mux;
	XDecode *decode = 0;
};

