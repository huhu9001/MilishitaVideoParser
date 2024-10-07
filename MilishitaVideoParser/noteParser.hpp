#ifndef NOTEPARSER_HPP
#define NOTEPARSER_HPP

#include<cstdint>
#include<vector>

class noteParser {
public:
	static int constexpr time_error_permitted = 30U;

	typedef struct {
		int64_t time;
		unsigned n_channel;
		char c_type;
	} note;

	noteParser(unsigned width, unsigned height, int new_linesize): notesOutput(notes) {
		SetSize(width, height, new_linesize);
		notes.reserve(0x800);
	}

	unsigned SetSize(unsigned new_width, unsigned new_height, int new_linesize);
	unsigned Input(int64_t time, uint8_t const*frame0);
	unsigned InputFinal();

	std::vector<note> const& notesOutput;
protected:
	enum dfcts { DMM, D2M, D2S, D4M } static constexpr dfct = DIFFICULTY_NAME;
	static constexpr unsigned n_channels =
		dfct == dfcts::D2M ? 2U :
		dfct == dfcts::D2S ? 2U :
		dfct == dfcts::D4M ? 4U : 6U;

	static constexpr unsigned y_sigma_permy = 254U;

	static constexpr unsigned y_focus_permy = 8333U;
	static constexpr unsigned y_2pfocus_permy = 7136U;

	static constexpr unsigned
		x1_focus_permy = 1961U,
		x2_focus_permy = 3181U,
		x3_focus_permy = 4410U,
		x4_focus_permy = 5630U,
		x5_focus_permy = 6860U,
		x6_focus_permy = 8079U;
	static constexpr unsigned
		x1_2pfocus_permy = 7618U,
		x2_2pfocus_permy = 2397U;
	static constexpr unsigned
		x1_2mfocus_permy = 3155U,
		x2_2mfocus_permy = 6862U;
	static constexpr unsigned
		x1_4mfocus_permy = 1930U,
		x2_4mfocus_permy = 3973U,
		x3_4mfocus_permy = 6016U,
		x4_4mfocus_permy = 8059U;

	unsigned y_sigma;

	unsigned x_center, x_2pcenter;
	unsigned x_focus[6];
	unsigned x_2pfocus[2];
	unsigned x_2mfocus[2];
	unsigned x_4mfocus[4];
	unsigned y_focus, y_2pfocus;

	int linesize;

	std::vector<note> notes;

	unsigned GetPixelIndex(unsigned n_channel, int x_offset, int y_offset) const;
	unsigned GetCenterPixelIndex(int x_offset, int y_offset) const;

	struct Circle {
		char c_type;
		int x, y;
		unsigned r;
	};
	std::vector<Circle> circles[n_channels];
	unsigned CircleSeeker(uint8_t const*frame0);
	unsigned StripeSeeker(uint8_t const*frame0);
	unsigned CenterCircleSeeker(uint8_t const*frame0);
	
	std::vector<char> c_notes[n_channels];
	std::vector<std::vector<int64_t>> t_notes[n_channels];
	std::vector<std::vector<int>> y_notes[n_channels];
	unsigned TimeNotes(int64_t time);
};

#endif