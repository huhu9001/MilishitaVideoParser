#include"noteParser.hpp"

void noteParser::SetSize(unsigned new_width, unsigned new_height, int new_linesize) {
	unsigned width, height;
	unsigned x_offset, y_offset;

	x_center = new_width / 2;
	x_2pcenter = new_height / 2;

	if (new_width * 9 >= new_height * 16)
		width = (height = new_height) * 16 / 9;
	else height = (width = new_width) * 9 / 16;

	x_offset = new_width - width;
	y_offset = new_height - height;

	y_sigma = height * y_sigma_permy / 10000U;

	if constexpr (dfct == dfcts::D2S)
		y_2pfocus = (width * y_2pfocus_permy + x_offset * 5000U) / 10000U;
	else y_focus = (height * y_focus_permy + y_offset * 5000U) / 10000U;

	if constexpr (dfct == dfcts::D2M) {
		x_2mfocus[0] = (width * x1_2mfocus_permy + x_offset * 5000U) / 10000U;
		x_2mfocus[1] = (width * x2_2mfocus_permy + x_offset * 5000U) / 10000U;
	}
	else if constexpr (dfct == dfcts::D2S) {
		x_2pfocus[0] = (height * x1_2pfocus_permy + y_offset * 5000U) / 10000U;
		x_2pfocus[1] = (height * x2_2pfocus_permy + y_offset * 5000U) / 10000U;
	}
	else if constexpr (dfct == dfcts::D4M) {
		x_4mfocus[0] = (width * x1_4mfocus_permy + x_offset * 5000U) / 10000U;
		x_4mfocus[1] = (width * x2_4mfocus_permy + x_offset * 5000U) / 10000U;
		x_4mfocus[2] = (width * x3_4mfocus_permy + x_offset * 5000U) / 10000U;
		x_4mfocus[3] = (width * x4_4mfocus_permy + x_offset * 5000U) / 10000U;
	}
	else {
		x_focus[0] = (width * x1_focus_permy + x_offset * 5000U) / 10000U;
		x_focus[1] = (width * x2_focus_permy + x_offset * 5000U) / 10000U;
		x_focus[2] = (width * x3_focus_permy + x_offset * 5000U) / 10000U;
		x_focus[3] = (width * x4_focus_permy + x_offset * 5000U) / 10000U;
		x_focus[4] = (width * x5_focus_permy + x_offset * 5000U) / 10000U;
		x_focus[5] = (width * x6_focus_permy + x_offset * 5000U) / 10000U;
	}

	linesize = new_linesize;

	notes.clear();
}

void noteParser::InputFinal() {
	for (unsigned i = 0; i < n_channels; ++i) circles[i].clear();
	TimeNotes(0);
	/* TODO: Left-right hand? */
}