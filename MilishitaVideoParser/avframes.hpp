#ifndef AVFRAMES_HPP
#define AVFRAMES_HPP

extern "C" {
#include"../../ffmpeg/FFmpeg-master/libavcodec/avcodec.h"
#include"../../ffmpeg/FFmpeg-master/libavformat/avformat.h"
#include"../../ffmpeg/FFmpeg-master/libswscale/swscale.h"
#include"../../ffmpeg/FFmpeg-master/libavutil/imgutils.h"
}

#include<future>
#include<atomic>
#include<iostream>

class Avframes {
public:
	Avframes() :
		error_code(0),
		prepared(false),
		pkt(av_packet_alloc()),
		frame(av_frame_alloc()),
		frame_rgb(av_frame_alloc()),
		fnum(0),
		fden(0),
		m_width(0),
		m_height(0),
		m_linesize(0),
		m_count(0) {}
	Avframes(Avframes const&) = delete;
	Avframes(Avframes&&) = delete;
	~Avframes() {
		close();
		av_frame_free(&frame_rgb);
		av_frame_free(&frame);
		av_packet_free(&pkt);
	}

	int error() const noexcept { return error_code; }

	void open(char const*filename) {
		error_code = 0;
		if (pkt == nullptr || frame == nullptr || frame_rgb == nullptr) {
			std::cerr << "av_packet_alloc/av_frame_alloc failed." << std::endl;
			return;
		}

		AVFormatContext*ctx_format = nullptr;
		if (error_code = avformat_open_input(&ctx_format, filename, nullptr, nullptr)) {
			std::cerr << "avformat_open_input failed (can not open file)." << std::endl;
			return;
		}
		if (int const err = avformat_find_stream_info(ctx_format, nullptr); err < 0) {
			error_code = err;
			std::cerr << "avformat_find_stream_info failed." << std::endl;
			avformat_close_input(&ctx_format);
			return;
		}

		int stream_idx;
		AVStream*vid_stream;
		for (int i = 0;; ++i) {
			if (i >= (int)ctx_format->nb_streams) {
				std::cerr << "(This file is not a video)." << std::endl;
				avformat_close_input(&ctx_format);
				return;
			}
			if (ctx_format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				stream_idx = i;
				vid_stream = ctx_format->streams[i];
				break;
			}
		}

		fnum = vid_stream->time_base.num * 1000;
		fden = vid_stream->time_base.den;
		m_count = vid_stream->nb_frames;

		AVCodecContext*ctx_codec;
		AVCodec*codec;
		if (!(codec = avcodec_find_decoder(vid_stream->codecpar->codec_id))) {
			std::cerr << "avcodec_find_decoder failed (unsupported format)." << std::endl;
			avformat_close_input(&ctx_format);
			return;
		}
		if (!(ctx_codec = avcodec_alloc_context3(codec))) {
			std::cerr << "avcodec_alloc_context3 failed." << std::endl;
			avformat_close_input(&ctx_format);
			return;
		}
		if (int const err = avcodec_parameters_to_context(ctx_codec, vid_stream->codecpar); err < 0) {
			error_code = err;
			std::cerr << "avcodec_parameters_to_context failed." << std::endl;
			avcodec_free_context(&ctx_codec);
			avformat_close_input(&ctx_format);
			return;
		}
		if (error_code = avcodec_open2(ctx_codec, codec, nullptr)) {
			std::cerr << "avcodec_open2 failed." << std::endl;
			avcodec_free_context(&ctx_codec);
			avformat_close_input(&ctx_format);
			return;
		}

		m_width = ctx_codec->width;
		m_height = ctx_codec->height;

		SwsContext *ctx_sws;
		if (!(ctx_sws = sws_getContext(
			ctx_codec->width,
			ctx_codec->height,
			ctx_codec->pix_fmt,
			ctx_codec->width,
			ctx_codec->height,
			AV_PIX_FMT_RGB24,
			SWS_BICUBIC, nullptr, nullptr, nullptr))) {
			std::cerr << "sws_getContext failed." << std::endl;
			avcodec_free_context(&ctx_codec);
			avformat_close_input(&ctx_format);
			return;
		}

		if (int const err = av_image_alloc(
			frame_rgb->data,
			frame_rgb->linesize,
			ctx_codec->width,
			ctx_codec->height,
			AV_PIX_FMT_RGB24,
			32); err < 0) {
			error_code = err;
			std::cerr << "av_image_alloc failed." << std::endl;
			sws_freeContext(ctx_sws);
			avcodec_free_context(&ctx_codec);
			avformat_close_input(&ctx_format);
			return;
		}

		prepared = true;

		m_linesize = frame_rgb->linesize[0];

		init_temp.ctx_format = ctx_format;

		init_temp.stream_idx = stream_idx;
		init_temp.vid_stream = vid_stream;

		init_temp.ctx_codec = ctx_codec;
		init_temp.codec = codec;

		init_temp.ctx_sws = ctx_sws;
	}
	void close() noexcept {
		if (!prepared) return;
		av_freep(frame_rgb->data);
		sws_freeContext(init_temp.ctx_sws);
		avcodec_free_context(&init_temp.ctx_codec);
		avformat_close_input(&init_temp.ctx_format);
	}

	template<typename Runnable, typename...Args> void run(Runnable func, Args...args) {
		if (!prepared) return;
		prepare_frames(
			init_temp.ctx_format,
			init_temp.stream_idx,
			init_temp.vid_stream,
			init_temp.ctx_codec,
			init_temp.codec,
			init_temp.ctx_sws,
			func,
			args...);
	}

	int width() const noexcept { return m_width; }
	int height() const noexcept { return m_height; }
	int linesize() const noexcept { return m_linesize; }
	int64_t count() const noexcept { return m_count; }

	struct Frame {
		int64_t time;
		uint8_t*data;
	};
private:
	int error_code;
	bool prepared;
	
	AVPacket*pkt;
	AVFrame*frame;
	AVFrame*frame_rgb;

	int fnum;
	int fden;
	int m_width;
	int m_height;
	int m_linesize;
	int64_t m_count;

	struct {
		AVFormatContext*ctx_format;

		int stream_idx;
		AVStream*vid_stream;

		AVCodecContext*ctx_codec;
		AVCodec*codec;

		SwsContext *ctx_sws;
	} init_temp;
	
	template<typename Runnable, typename...Args> void prepare_frames(
		AVFormatContext*ctx_format,
		int stream_idx,
		AVStream*vid_stream,
		AVCodecContext*ctx_codec,
		AVCodec*codec,
		SwsContext *ctx_sws,
		Runnable func,
		Args...args) {
		while (!av_read_frame(ctx_format, pkt)) {
			if (pkt->stream_index == stream_idx) {
				if (int const err = avcodec_send_packet(ctx_codec, pkt); err) {
					if (err != AVERROR_EOF) {
						error_code = err;
						std::cerr << "avcodec_send_packet failed." << std::endl;
					}
					av_packet_unref(pkt);
					goto LB_RETIRING;
				}
				for (;;) {
					if (int const err = avcodec_receive_frame(ctx_codec, frame); err) {
						if (err == AVERROR(EAGAIN)) break;
						else {
							if (err != AVERROR_EOF) {
								error_code = err;
								std::cerr << "avcodec_send_packet failed." << std::endl;
							}
							av_packet_unref(pkt);
							goto LB_RETIRING;
						}
					}

					frame_rgb->pts = frame->pts;
					sws_scale(
						ctx_sws,
						frame->data,
						frame->linesize,
						0,
						frame->height,
						frame_rgb->data,
						frame_rgb->linesize);

					func(frame->pts * fnum / fden, frame_rgb->data[0], args...);
				}
			}
			av_packet_unref(pkt);
		}

	LB_RETIRING:
		av_freep(frame_rgb->data);
		sws_freeContext(ctx_sws);
		avcodec_free_context(&ctx_codec);
		avformat_close_input(&ctx_format);

		prepared = false;
	}
};

#endif