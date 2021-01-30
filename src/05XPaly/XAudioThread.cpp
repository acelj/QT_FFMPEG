#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <iostream>
using namespace std;

/// �ڼ̳��еĻ��������
// void XAudioThread::Push(AVPacket *pkt)
// {
// 	if (!pkt)return;
// 	//��������Ϊ����ᾭ������
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

void XAudioThread::Clear()
{
	XDecodeThread::Clear();
	mux.lock();
	if (ap) ap->Clear();
	mux.unlock();
}

//ֹͣ�̣߳�������Դ
void XAudioThread::Close()
{
	XDecodeThread::Close();
	if (res)
	{
		res->Close();
		amux.lock();
		delete res;
		res = NULL;
		amux.unlock();
	}
	if (ap)
	{
		ap->Close();
		amux.lock();
		ap = NULL;
		amux.unlock();
	}
}

bool XAudioThread::Open(AVCodecParameters *para, int sampleRate, int channels)
{
	if (!para)return false;
	Clear();
	amux.lock();
	pts = 0;
	bool re = true;
	if (!res->Open(para, false))
	{
		cout << "XResample open failed!" << endl;
		re = false;
	}
	ap->sampleRate = sampleRate;
	ap->channels = channels;
	if (!ap->Open())
	{
		re = false;
		cout << "XAudioPlay open failed!" << endl;
	}
	if (!decode->Open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		re = false;
	}
	amux.unlock();
	cout << "XAudioThread::Open :" << re << endl;
	return re;
}

void XAudioThread::SetPause(bool isPause)
{
	//amux.lock();    //����Ƶ�������isPause ֮�� ����Ҫ�������ȥ��
	this->isPause = isPause;
	// ������Ҫ��ap ����һ��
	if (ap)
		ap->SetPause(isPause);
	//amux.unlock();
}
void XAudioThread::run()  // ����û�� ����
{
	unsigned char *pcm = new unsigned char[1024 * 1024 * 100];
	while (!isExit)
	{
		amux.lock();
		if (this->isPause)
		{
			amux.unlock();
			msleep(5);
			continue;
		}
// 		//û������
// 		if (packs.empty() || !decode || !res || !ap)
// 		{
// 			mux.unlock();
// 			msleep(1);
// 			continue;
// 		}

// 		AVPacket *pkt = packs.front();
// 		packs.pop_front();   // β�����룬 ͷ��ȡ���� ͷ��ɾ��
		AVPacket *pkt = Pop();   // ��������
		bool re = decode->Send(pkt);
		if (!re)
		{
			amux.unlock();
			msleep(1);
			continue;
		}
		//һ��send ���recv
		while (!isExit)
		{
			AVFrame * frame = decode->Recv();
			if (!frame) break;
			
			// ��ȥ������Ϊ���ŵ�ʱ��
			pts = decode->pts - ap->Get()->GetNoPlayMs();
			//cout << "audio pts = " << pts << endl;
			
			//�ز��� 
			int size = res->Resample(frame, pcm);
			//������Ƶ
			while (!isExit)
			{
				if (size <= 0)break;
				//����δ���꣬�ռ䲻��
				if (ap->GetFree() < size || isPause)
				{
					msleep(1);
					continue;
				}
				ap->Write(pcm, size);
				break;
			}
		}
		amux.unlock();
	}
	delete pcm;
}

XAudioThread::XAudioThread()
{
	if (!res) res = new XResample();
	if (!ap) ap = XAudioPlay::Get();
}


XAudioThread::~XAudioThread()
{
	//�ȴ��߳��˳�
	isExit = true;
	wait();
}
