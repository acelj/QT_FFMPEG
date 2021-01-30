#include "XPaly.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QThread>
#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include "XAudioPlay.h"
#include "XAudioThread.h"
#include "XVideoThread.h"

using namespace std;

class TestThread : public QThread
{
public:
	XAudioThread at;
	XVideoThread vt;
	void Init()
	{
		char* url = "rtmp://58.200.131.2:1935/livetv/hunantv";   // ��������
		url = "D:\\02C++\\08FFMPEG\\src\\v720p.mp4";
		cout << "demux.Open = " << demux.Open(url) << endl;
// 		demux.Read();
// 		demux.Clear();
// 		demux.Close();
		//cout << "demux.Open = " << demux.Open(url) << endl;
		//cout << "demux.CopyVPara = " << demux.CopyVPara() << endl;
		//cout << "demux.CopyAPara = " << demux.CopyAPara() << endl;
		//cout << "seek=" << demux.Seek(0.9) << endl;

		// cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
		//cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << endl;

		//cout << "resample.Open = " << resample.Open(demux.CopyAPara()) << endl;

		//XAudioPlay::Get()->channels = demux.channels;
		//XAudioPlay::Get()->sampleRate = demux.sampleRate;
		//cout << "XAudioPlay::Get()->Open() = " << XAudioPlay::Get()->Open() << endl;
		cout << "at.Open = " << at.Open(demux.CopyAPara(), demux.sampleRate, demux.channels);
		vt.Open(demux.CopyVPara(), video, demux.width, demux.height);
		at.start();
		vt.start();
	}
	void run()
	{
		unsigned char *pcm = new unsigned char[1024 * 1024];
		for (;;)
		{
			AVPacket *pkt = demux.Read();
			if (demux.IsAudio(pkt))
			{
				at.Push(pkt);
				//adecode.Send(pkt);
				//AVFrame *frame = adecode.Recv();
				//// cout << "Resample:" << resample.Resample(frame, pcm) << " " << endl;
				//// ���Բ�������
				//int len = resample.Resample(frame, pcm);
				//cout << "Resample:" << len << " ";
				//while (len > 0)
				//{
				//	if (XAudioPlay::Get()->GetFree() >= len)
				//	{
				//		XAudioPlay::Get()->Write(pcm, len);
				//		break;
				//	}
				//	msleep(1);
				//}

				//cout << "Audio:" << frame << endl;
			}
			else
			{
				vt.Push(pkt);
				/*vdecode.Send(pkt);
				AVFrame *frame = vdecode.Recv();
				video->Repaint(frame);*/
				// msleep(40);
				//cout << "Video:" << frame << endl;
			}
			if (!pkt)
			{
				demux.Seek(0);
			//	break;
			}
		}
	}

	XDemux demux;
	//XDecode vdecode;
	//XDecode adecode;
	//XResample resample;
	XVideoWidget *video = 0;
};

// �½���Ŀʱ��ѡ������������Ŀ¼��ǰ����
/// 1. ����->����->���Ŀ¼					..\..\bin\win64
/// 2. ����->����->����Ŀ¼					..\..\bin\win64	
/// 3. ����->C/C++->����->���Ӱ���Ŀ¼		..\..\include
/// 4. ����->������->����->���ӿ�Ŀ¼		..\..\lib\win64
/// 5. qt������Գɿ���̨������->������->ϵͳ->��ϵͳ->console
/// 6. ������->������->����ѡ��->ӳ����а�ȫ�쳣�������  �޸ĳ� ��SAFESEH:NO

#include "XDemuxThread.h"
int main(int argc, char *argv[])
{

	//XDemux demux;
	//char* url = "rtmp://58.200.131.2:1935/livetv/hunantv";   // ��������
	//url = "D:\\02C++\\08FFMPEG\\src\\v720p.mp4";
	//cout << "demux.Open = " << demux.Open(url) << endl;
	//demux.Read();
	//demux.Clear();
	//demux.Close();
	//cout << "demux.Open = " << demux.Open(url) << endl;
	//cout << "demux.CopyVPara = " << demux.CopyVPara() << endl;
	//cout << "demux.CopyAPara = " << demux.CopyAPara() << endl;
	//cout << "seek=" << demux.Seek(0.9) << endl;

	////////////////////////////////////////////////////////////
	// �������
	//XDecode vdecode;
	//cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
	////vdecode.Clear();
	////vdecode.Close();
	//XDecode adecode;
	//cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << endl;


	//for (;;)
	//{
	//	AVPacket *pkt = demux.Read();
	//	if (demux.IsAudio(pkt))
	//	{
	//		adecode.Send(pkt);
	//		AVFrame *frame = adecode.Recv();
	//		//cout << "Audio:" << frame << endl;
	//	}
	//	else
	//	{
	//		vdecode.Send(pkt);
	//		AVFrame *frame = vdecode.Recv();
	//		//cout << "Video:" << frame << endl;
	//	}
	//	if (!pkt)break;
	//}

	//TestThread tt;
	

    QApplication a(argc, argv);
    XPaly w;
    w.show();

	// ��ʼ��gl����
	// w.ui.video->Init(tt.demux.width, tt.demux.height);
	//tt.video = w.ui.video;    // ���ڳ�ʼ����ɺ������߳�
	//tt.Init();
	//tt.start();
	//XDemuxThread dt;
	//char* url = "rtmp://116.213.200.53/tslsChannelLive/PCG0DuD/live";   // ��������
	//url = "D:\\02C++\\08FFMPEG\\src\\v720p.mp4";
	//dt.Open("D:\\02C++\\08FFMPEG\\src\\v720p.mp4", w.ui.video);
	//dt.Start();    // ��������д��Start����

    return a.exec();
}
