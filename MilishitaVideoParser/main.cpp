#include"noteParser.hpp"
#include"avframes.hpp"

/*extern "C" {
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
#include<libswscale/swscale.h>
#include<libavutil/imgutils.h>
}*/

#include<iostream>
#include<fstream>
#include<iomanip>
#include<string>
#include<atomic>
#include<thread>

static std::string FilenameNoExt(char const*original_name) {
	unsigned i;
	for (i = 0; original_name[i]; i++);
	for (unsigned j = i; j;) {
		--j;
		if (original_name[j] == '\\' || original_name[j] == ':') break;
		else if (original_name[j] == '.') {
			i = j;
			break;
		}
	}
	return std::string(original_name, i);
}

static void SaveBmp(char const*filename, uint8_t const*frame, int64_t n_frame, int w, int h, int linesize) {
	int const filesize = 54 + 3 * w * h;
	char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0 };
	char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0 };
	memcpy(bmpfileheader + 2, &filesize, 4);
	memcpy(bmpinfoheader + 4, &w, 4);
	memcpy(bmpinfoheader + 8, &h, 4);
	std::ofstream f_o(FilenameNoExt(filename) + "_" + std::to_string(n_frame) + ".bmp", std::ios::binary);
	f_o.write(bmpfileheader, 14);
	f_o.write(bmpinfoheader, 40);
	for (int i2 = h; --i2 != -1;) for (int i1 = 0; i1 < w; ++i1) {
		f_o.put(frame[i1 * 3 + linesize * i2 + 2]);
		f_o.put(frame[i1 * 3 + linesize * i2 + 1]);
		f_o.put(frame[i1 * 3 + linesize * i2 + 0]);
	}
}

static void OutputTimestamp(std::ostream&o, int64_t time) {
	if (int64_t const hour = time / 3600000; hour > 0) {
		o << hour;
		o << ':';
	}
	o << std::setw(2) << time / 60000 % 60;
	o << ':';
	o << std::setw(2) << time / 1000 % 60;
	o << '.';
	o << std::setw(3) << time % 1000 << std::setw(0);
}

static int64_t n_frame_all;
static std::atomic_int64_t n_frame(-1);
#ifndef NDEBUG
static char const*filename;
static int width;
static int height;
static int linesize;
#endif

static void decode_single_frame(int64_t time, uint8_t data[], noteParser*np) {
#ifndef NDEBUG
	bool savefile = false;
	if (savefile) SaveBmp(filename, data, n_frame, width, height, linesize);
#endif
	np->Input(time, data);
	n_frame += 1;
}

static void report_decode_progress() {
	std::chrono::seconds constexpr one_second(1);
	for (int64_t n; std::this_thread::sleep_for(one_second), (n = n_frame) != -1;)
		std::cout << "frame: " << n << " of " << n_frame_all << std::endl;
}

int main(int argc, char **argv) {
	Avframes fs;
	noteParser np(0, 0, 0);
	for (int i = 1; i < argc; ++i) {
		fs.open(argv[i]);

		if (fs.ready()) {
			int const width = fs.width();
			int const height = fs.height();
			int const linesize = fs.linesize();

			np.SetSize(width, height, linesize);

#ifndef NDEBUG
			::filename = argv[i];
			::width = width;
			::height = height;
			::linesize = linesize;
#endif

			n_frame_all = fs.count();
			n_frame = 0;
			std::thread thread_report(report_decode_progress);
			fs.run(decode_single_frame, &np);
			n_frame = -1;
			np.InputFinal();

			if (fs.error() == 0) {
				std::ofstream f_o(FilenameNoExt(argv[i]) + ".ass");

				f_o << std::setfill('0');

				f_o << "[Script Info]" << std::endl;
				f_o << "Title: MLTD Notes" << std::endl;
				f_o << "ScriptType: v4.00+" << std::endl;
				f_o << "PlayResX: " << width << std::endl;
				f_o << "PlayResY: " << height << std::endl;
				f_o << std::endl << "[V4+ Styles]" << std::endl;
				f_o << "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding" << std::endl;
				f_o << "Style: Default,Arial,20,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,2,2,10,10,10,1" << std::endl;
				f_o << std::endl << "[Events]" << std::endl;
				f_o << "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text";

				for (int64_t time_pre = 0; noteParser::note const n_this : np.notesOutput) {
					if (!(n_this.time - time_pre <= noteParser::time_error_permitted && time_pre - n_this.time <= noteParser::time_error_permitted)) {
						f_o << std::endl;
						f_o << "Dialogue: 0,";

						OutputTimestamp(f_o, n_this.time);
						f_o << ',';
						OutputTimestamp(f_o, n_this.time + noteParser::time_error_permitted);

						f_o << ",Default,,0,0,0,,";

						time_pre = n_this.time;
					}

					f_o << n_this.n_channel + 1;
					if (n_this.c_type) f_o << n_this.c_type;
				}

				thread_report.join();
				std::cout << "Parsing of \"";
				std::cout << argv[i];
				std::cout << "\" succeeded";
				std::cout << std::endl;
				continue;
			}
			else thread_report.join();
		}

		std::cout << "Parsing of \"";
		std::cout << argv[i];
		std::cout << "\" failed with an error code of ";
		std::cout << fs.error();
		std::cout << ".";
		std::cout << std::endl;
	}
	return 0;
}