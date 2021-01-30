#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}
#include "XDecode.h"
using namespace std;
void XDemuxThread::SetPause(bool isPause)
{
	mux.lock();
	this->isPause = isPause;    // ���Լ���ͣ ��Ҫ��at�� vt ��ͣ
	if (at) at->SetPause(isPause);
	if (vt) vt->SetPause(isPause);
	mux.unlock();
}


void XDemuxThread::run()
{
	while (!isExit)
	{
		mux.lock();
		if (isPause)
		{
			mux.unlock();
			msleep(5);
			continue;

		}
		if (!demux)
		{
			mux.unlock();
			msleep(1);
			continue;
		}

		if (vt && at)    // �����������Ƶͬ��
		{
			pts = at->pts;
			vt->synpts = at->pts;
		}

		AVPacket *pkt = demux->Read();
		if (!pkt)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//�ж���������Ƶ
		if (demux->IsAudio(pkt))
		{
			if (at)at->Push(pkt);  // yuv rgb ��Ƶ��ʽ����û����Ƶ��
		}
		else //��Ƶ
		{
			if (vt)vt->Push(pkt);
		}

		mux.unlock();
		msleep(1);    //��ȡ�ٶ�̫�죬���²��ɹ�   ===============================
	}
}


bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
	if (url == 0 || url[0] == '\0')   //�ж�url�Ƿ�Ϊ��
		return false;

	mux.lock();
	//if (!demux) demux = new XDemux();
	//if (!vt) vt = new XVideoThread();
	//if (!at) at = new XAudioThread();

	//�򿪽��װ
	bool re = demux->Open(url);
	if (!re)
	{
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}
	//����Ƶ�������ʹ����߳�
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		re = false;
		cout << "vt->Open failed!" << endl;
	}
	//����Ƶ�������ʹ����߳�
	if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels))
	{
		re = false;
		cout << "at->Open failed!" << endl;
	}
	totalMs = demux->totalMs;
	mux.unlock();
	cout << "XDemuxThread::Open " << re << endl;
	return re;
}
//�ر��߳�������Դ
void XDemuxThread::Close()
{
	isExit = true;
	wait();				// �ȴ���ǰ�̵߳��˳�
	if (vt) vt->Close();
	if (at) at->Close();
	mux.lock();
	delete vt;
	delete at;
	vt = NULL;
	at = NULL;
	mux.unlock();
}

void XDemuxThread::Clear()
{
	mux.lock();
	if(demux) demux->Clear();
	if (vt) vt->Clear();
	if (at) at->Clear();
	mux.unlock();
}

//void XDemuxThread::Seek(double pos)
//{
//	Clear();
//	mux.lock();
//	if (demux)
//		demux->Seek(pos);
//	mux.unlock();
//}

void XDemuxThread::Seek(double pos)
{
	// ������
	Clear();
	
	mux.lock();
	bool status = this->isPause;
	mux.unlock();
	// ��ͣ
	SetPause(true);
	mux.lock();
	if(demux)
		demux->Seek(pos);    // ������Ҫ��ջ���
	// ʵ��Ҫ��ʾ��λ��pos
	long long seekPos = pos * demux->totalMs;
	while (!isExit)
	{
// 		AVPacket *pkt = demux->Read();
// 		if (!pkt) break;
// 		if (pkt->stream_index == demux->audioStream)
// 		{
// 			// ����Ƶ���ݾͶ���
// 			av_packet_free(&pkt);
// 			continue;
// 		}
		AVPacket *pkt = demux->ReadVideo();
		if (!pkt) break;
		// ������뵽seekPts
		if (vt->RepaintPts(pkt, seekPos))
		{

			break;
		}

		//bool re = vt->decode->Send(pkt);
		//// av_packet_free(&pkt);
		//if (!re) break;  // ʧ��
		//AVFrame *frame = vt->decode->Recv();
		//if (!frame) continue;
		//// �������λ�ã�����ʾ
		//if (frame->pts >= seekPos)
		//{
		//	this->pts = frame->pts;
		//	// ���ڹؼ�֡�� ��Ҫ��ʾ��ȥ

		//	vt->call->Repaint(frame);
		//	break;
		//}
		//av_frame_free(&frame);
	}
	mux.unlock();

	// ���� seek �Ƿ���ͣ״̬
	if (!status)
		SetPause(false);
}

//���������߳�
void XDemuxThread::Start()
{
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	if (!at) at = new XAudioThread();
	//������ǰ�߳�
	QThread::start();
	if (vt)vt->start();
	if (at)at->start();
	mux.unlock();
}
XDemuxThread::XDemuxThread()
{
}


XDemuxThread::~XDemuxThread()
{
	isExit = true;
	wait();
}
