
#include "qdemuxer.hpp"

#include "ffmpeg.hpp"
#include "ffplayer_p.hpp"

Q_DECLARE_OPAQUE_POINTER(AVPacket*);

QDemuxer::QDemuxer(FFPlayer* _parent)
	: parent(_parent)
{
	connect(this, SIGNAL(start()), this, SLOT(slot_start()));
	qRegisterMetaType<AVPacket*>("AVPacket*");
}

void QDemuxer::stop()
{
	m_stop = true;
}


void QDemuxer::slot_start()
{
	// 开始解码
	QTimer::singleShot(0, this, SLOT(read_one_frame()));

	pkt.pts = 0;
}

void QDemuxer::read_one_frame()
{
	if (m_stop)
		return;

	auto avformat_ctx = parent->d_ptr->avformat_ctx.get();

	av_init_packet(&pkt);
	if (av_read_frame(avformat_ctx, &pkt) ==0)
	{
		QTimer::singleShot(0, this, SLOT(read_one_frame()));
		frame_readed(&pkt);
	}
	else
	{
		// EOF
	}

	av_free_packet(&pkt);
}