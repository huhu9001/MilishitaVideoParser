#pragma once

#include<cstdint>
#include<list>

class noteParser {
public:
	typedef struct {
		int64_t time;
		unsigned n_channel;
		char c_type;
	} note;

	noteParser(unsigned width, unsigned height): notesOutput(notes), time_pre(0) {
		SetSize(width, height);
	}

	unsigned SetSize(unsigned new_width, unsigned new_height);
	unsigned Input(int64_t time, int linesize, uint8_t const*frame0);

	std::list<note> const& notesOutput;
protected:
	static const unsigned y_sigma_permy = 254U;

	static const unsigned y_focus_permy = 8333U;
	static const unsigned y_2pfocus_permy = 7136U;

	static const unsigned
		x1_focus_permy = 1961U,
		x2_focus_permy = 3181U,
		x3_focus_permy = 4410U,
		x4_focus_permy = 5630U,
		x5_focus_permy = 6860U,
		x6_focus_permy = 8079U;
	static const unsigned
		x1_2pfocus_permy = 7618U,
		x2_2pfocus_permy = 2397U;
	static const unsigned
		x1_2mfocus_permy = 3155U,
		x2_2mfocus_permy = 6862U;
	static const unsigned
		x1_4mfocus_permy = 1930U,
		x2_4mfocus_permy = 3973U,
		x3_4mfocus_permy = 6016U,
		x4_4mfocus_permy = 8059U;

	enum Color { none, red, blue, yellow, green, black, white, mixed };

	unsigned y_range, y_sigma;

	unsigned x_focus[6], y_bottom;
	unsigned x_2pfocus[6], y_2pbottom;
	unsigned x_2mfocus[6], y_2mbottom;
	unsigned x_4mfocus[6], y_4mbottom;

	class noteSus {
	public:
		noteSus(): pRedBefore(nullptr), isAfterIris(false) {}

		int64_t t_FrontEnter, t_FrontExit, t_BackEnter, t_BackExit;
		unsigned pos_Front;
		unsigned countColor[mixed + 1];
		char *pRedBefore;
		bool isAfterIris;
	};

	std::list<note> notes;
	std::list<noteSus> notesSus[6];

	int64_t time_pre;

	typedef struct RgbColor {
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} RgbColor;
	typedef struct HsvColor {
		unsigned char h;
		unsigned char s;
		unsigned char v;
	} HsvColor;
	static HsvColor RgbToHsv(RgbColor rgb);
	static Color RecogColor(HsvColor hsv);
	Color GetPixel(uint8_t const*frame0, unsigned linesize, unsigned x, unsigned y);
	static Color DecideColor(unsigned const countColor[mixed + 1]);
};