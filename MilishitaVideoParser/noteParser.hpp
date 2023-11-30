#pragma once
#include<cstdint>
#include<list>

class noteParser {
public:
	static int const time_error_permitted = 30U;

	typedef struct {
		int64_t time;
		unsigned n_channel;
		char c_type;
	} note;

	noteParser(unsigned width, unsigned height, int new_linesize): notesOutput(notes) {
		SetSize(width, height, new_linesize);
	}

	unsigned SetSize(unsigned new_width, unsigned new_height, int new_linesize);
	unsigned Input(int64_t time, uint8_t const*frame0);
	unsigned InputFinal();

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

	static const unsigned n_channels;

	unsigned y_sigma;

	unsigned x_center, x_2pcenter;
	unsigned x_focus[6];
	unsigned x_2pfocus[6];
	unsigned x_2mfocus[6];
	unsigned x_4mfocus[6];
	unsigned y_focus, y_2pfocus;

	int linesize;

	std::list<note> notes;

	unsigned GetPixelIndex(unsigned n_channel, int x_offset, int y_offset) const;
	unsigned GetCenterPixelIndex(int x_offset, int y_offset) const;

	typedef struct Circle {
		char c_type;
		int x, y;
		unsigned r;
	} Circle;
	std::list<Circle> circles[6];
	unsigned CircleSeeker(uint8_t const*frame0);
	unsigned StripeSeeker(uint8_t const*frame0);
	unsigned CenterCircleSeeker(uint8_t const*frame0);
	
	std::list<char> c_notes[6];
	std::list<std::list<int64_t>> t_notes[6];
	std::list<std::list<int>> y_notes[6];
	unsigned TimeNotes(int64_t time);
};