#include "noteParser.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

extern "C" {
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
#include<libswscale/swscale.h>
#include<libavutil/imgutils.h>
}

int decode(char const*filename) {
	char *filename_output;
	char const*const filename_default = "E:\\Print\\bandicam 2022-01-20 20-15-37-140.mp4";

	AVFormatContext *ctx_format;

	int stream_idx;
	AVStream *vid_stream;

	AVCodecContext* ctx_codec;
	AVCodec* codec;

	AVPacket* pkt = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	AVFrame* frame_rgb = av_frame_alloc();

	SwsContext *ctx_sws;

	ctx_format = nullptr;
	if (filename == nullptr)  filename = filename_default;
	if (avformat_open_input(&ctx_format, filename, nullptr, nullptr)) {
		std::cout << "Can not open file." << std::endl;
		return -1;
	}
	if (avformat_find_stream_info(ctx_format, nullptr)) {
		std::cout << "Unsupported Format." << std::endl;
		return -1;
	}

	{
		unsigned i;
		for (i = 0; filename[i]; i++);
		filename_output = new char[i + 5];
		for (i = 0; filename[i]; i++) filename_output[i] = filename[i];
		filename_output[i++] = '.';
		filename_output[i++] = 'a';
		filename_output[i++] = 's';
		filename_output[i++] = 's';
		filename_output[i] = '\0';
	}

	vid_stream = nullptr;
	for (int i = 0; i < (int)ctx_format->nb_streams; i++)
		if (ctx_format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			stream_idx = i;
			vid_stream = ctx_format->streams[i];
			break;
		}
	if (!vid_stream) {
		std::cout << "Can not find a video stream." << std::endl;
		return -1;
	}

	if (!(codec = avcodec_find_decoder(vid_stream->codecpar->codec_id))) {
		std::cout << "No codec for this format." << std::endl;
		return -1;
	}
	ctx_codec = avcodec_alloc_context3(codec);
	if (avcodec_parameters_to_context(ctx_codec, vid_stream->codecpar) < 0) {
		std::cout << "avcodec_parameters_to_context error." << std::endl;
		return -1;
	}
	if (avcodec_open2(ctx_codec, codec, nullptr)) {
		std::cout << "Codec initialization failure." << std::endl;
		return -1;
	}

	ctx_sws = sws_getContext(
		ctx_codec->width,
		ctx_codec->height,
		ctx_codec->pix_fmt,
		ctx_codec->width,
		ctx_codec->height,
		AV_PIX_FMT_RGB24,
		SWS_BICUBIC, nullptr, nullptr, nullptr);

	av_image_alloc(frame_rgb->data, frame_rgb->linesize, ctx_codec->width, ctx_codec->height, AV_PIX_FMT_RGB24, 32);

	{
		noteParser np(ctx_codec->width, ctx_codec->height);
		unsigned n_frame = 0U - 1U;

		while (av_read_frame(ctx_format, pkt) >= 0) {
			if (pkt->stream_index == stream_idx) {
				int ret = avcodec_send_packet(ctx_codec, pkt);
				if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					std::cout << "avcodec_send_packet: " << ret << std::endl;
					break;
				}
				while (ret >= 0) {
					ret = avcodec_receive_frame(ctx_codec, frame);
					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
						//std::cout << "avcodec_receive_frame: " << ret << std::endl;
						break;
					}

					sws_scale(
						ctx_sws,
						frame->data,
						frame->linesize,
						0,
						frame->height,
						frame_rgb->data,
						frame_rgb->linesize);

					{
						++n_frame;
						if (n_frame % 1000 == 0) std::cout << "frame: " << n_frame << std::endl;
						//if (n_frame >= 2730 && n_frame <= 2764)
						np.Input(
							frame->pts * ctx_codec->framerate.den / ctx_codec->framerate.num,
							frame_rgb->linesize[0],
							frame_rgb->data[0]);
					}
				}
			}
			av_packet_unref(pkt);
		}

		{
			std::ofstream f_o;
			f_o.open(filename_output);

			f_o << std::setfill('0');

			f_o << "[Script Info]" << std::endl;
			f_o << "Title: MLTD Notes" << std::endl;
			f_o << "ScriptType: v4.00+" << std::endl;
			f_o << "PlayResX: " << ctx_codec->width << std::endl;
			f_o << "PlayResY: " << ctx_codec->height << std::endl;
			f_o << std::endl << "[V4+ Styles]" << std::endl;
			f_o << "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding" << std::endl;
			f_o << "Style: Default,Arial,20,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,2,2,10,10,10,1" << std::endl;
			f_o << std::endl << "[Events]" << std::endl;
			f_o << "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text";

			int64_t time_pre = 0;
			for (auto n_this = np.notesOutput.begin(); n_this != np.notesOutput.end(); ++n_this) {
				if (!(n_this->time - time_pre <= 15 && time_pre - n_this->time <= 15)) {
					f_o << std::endl;
					f_o << "Dialogue: 0,";
					f_o << n_this->time / 3600000;
					f_o << ':';
					f_o << n_this->time / 60000 % 60;
					f_o << ':';
					f_o << n_this->time / 1000 % 60;
					f_o << '.';
					f_o << std::setw(3) << n_this->time % 1000 << std::setw(0);
					f_o << ',';
					f_o << n_this->time / 3600000;
					f_o << ':';
					f_o << n_this->time / 60000 % 60;
					f_o << ':';
					f_o << n_this->time / 1000 % 60;
					f_o << '.';
					f_o << std::setw(3) << n_this->time % 1000 << std::setw(0);
					f_o << ",Default,,0,0,0,,";

					time_pre = n_this->time;
				}

				f_o << n_this->n_channel + 1;
				if (n_this->c_type) f_o << n_this->c_type;
			}

			f_o.close();
		}
	}

	avformat_close_input(&ctx_format);
	av_packet_unref(pkt);
	avcodec_free_context(&ctx_codec);
	avformat_free_context(ctx_format);

	av_frame_free(&frame);
	av_freep(frame_rgb->data);
	av_frame_free(&frame_rgb);

	delete[] filename_output;

	return 0;
}

int main(int argc, char **argv)
{
	return decode(argv[1]);
}