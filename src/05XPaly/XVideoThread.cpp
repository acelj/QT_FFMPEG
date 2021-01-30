#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

//�򿪣����ܳɹ��������
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
	if (!para)return false;

	Clear();

	vmux.lock();
	synpts = 0; 
	//��ʼ����ʾ����
	this->call = call;
	if (call)
	{
		call->Init(width, height);
	}
	vmux.unlock();
	//�򿪽�����

	int re = true;
	if (!decode->Open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		re = false;
	}
	cout << "XAudioThread::Open :" << re << endl;
	return re;
}

// void XVideoThread::Push(AVPacket *pkt)
// {
// 	if (!pkt)return;
// 	//����
// 	while (!isExit)
// 	{
// 		mux.lock();
// 		if (packs.size() < maxList)
// 		{
// 			packs.push_back(pkt);
// 			mux.unlock();
// 			break;
// 		}
// 		mux.unlock();
// 		msleep(1);
// 	}
// }

void XVideoThread::SetPause(bool isPause)
{
	vmux.lock();
	this->isPause = isPause;

	vmux.unlock();

}

void XVideoThread::run()
{
	while (!isExit)
	{
		vmux.lock();

		if (this->isPause)
		{
			vmux.unlock();
			msleep(5);
			continue;
		}

// 		//û������
// 		if (packs.empty() || !decode)
// 		{
// 			mux.unlock();
// 			msleep(1);
// 			continue;
// 		}

		// ����Ƶͬ��
		if (synpts > 0 && synpts < decode->pts)   // ��Ƶ���ˣ�����Ҫ��  , û����Ƶ����Ҫ�ȴ�
		{
			vmux.unlock();
			msleep(1);
			continue;
		}

// 		AVPacket *pkt = packs.front();
// 		packs.pop_front();
		AVPacket *pkt = Pop();
		bool re = decode->Send(pkt);
		if (!re)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}
		//һ��send ���recv
		while (!isExit)
		{
			AVFrame * frame = decode->Recv();
			if (!frame) break;
			//��ʾ��Ƶ
			if (call)
			{
				call->Repaint(frame);
			}

		}
		vmux.unlock();
	}
}

// ����pts�� ������յ��Ľ�������pts >= seekpts    return true;    ������ʾ����
bool XVideoThread::RepaintPts(AVPacket *pkt, long long seekpts)
{
	vmux.lock();
	bool re = decode->Send(pkt);
	// av_packet_free(&pkt);
	if (!re)
	{
		vmux.unlock();
		return true;  // ��ʾ��������
	}
		AVFrame *frame = decode->Recv();
		if (!frame)
		{
			vmux.unlock();
			return false;
		}
	// �������λ�ã�����ʾ
	if (decode->pts >= seekpts)
	{
		// ���ڹؼ�֡�� ��Ҫ��ʾ��ȥ
		if(call)
			call->Repaint(frame);
		vmux.unlock();
		return true;
	}
	XFreeFrame(&frame);
	vmux.unlock();

	return false;
}

XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
// 	//�ȴ��߳��˳�
// 	isExit = true;
// 	wait();
}
