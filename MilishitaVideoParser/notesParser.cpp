#include "noteParser.hpp"

unsigned noteParser::SetSize(unsigned new_width, unsigned new_height) {
	unsigned width, height;
	unsigned x_offset, y_offset;
	unsigned y_focus;

	if (new_width * 9 >= new_height * 16)
		width = (height = new_height) * 16 / 9;
	else height = (width = new_width) * 9 / 16;

	x_offset = new_width - width;
	y_offset = new_height - height;

	y_sigma = height * y_sigma_permy / 10000U;
	y_range = 2 * y_sigma + 2;

	y_focus = (height * y_focus_permy + y_offset * 5000U) / 10000U;
	y_bottom = y_focus + y_sigma + 1;
	y_focus = (width * y_2pfocus_permy + x_offset * 5000U) / 10000U;
	y_2pbottom = y_focus + y_sigma + 1;

	x_focus[0] = (width * x1_focus_permy + x_offset * 5000U) / 10000U;
	x_focus[1] = (width * x2_focus_permy + x_offset * 5000U) / 10000U;
	x_focus[2] = (width * x3_focus_permy + x_offset * 5000U) / 10000U;
	x_focus[3] = (width * x4_focus_permy + x_offset * 5000U) / 10000U;
	x_focus[4] = (width * x5_focus_permy + x_offset * 5000U) / 10000U;
	x_focus[5] = (width * x6_focus_permy + x_offset * 5000U) / 10000U;

	x_2pfocus[0] = (height * x1_2pfocus_permy + y_offset * 5000U) / 10000U;
	x_2pfocus[1] = (height * x2_2pfocus_permy + y_offset * 5000U) / 10000U;

	x_2mfocus[0] = (width * x1_2mfocus_permy + x_offset * 5000U) / 10000U;
	x_2mfocus[1] = (width * x2_2mfocus_permy + x_offset * 5000U) / 10000U;

	x_4mfocus[0] = (width * x1_4mfocus_permy + x_offset * 5000U) / 10000U;
	x_4mfocus[1] = (width * x2_4mfocus_permy + x_offset * 5000U) / 10000U;
	x_4mfocus[2] = (width * x3_4mfocus_permy + x_offset * 5000U) / 10000U;
	x_4mfocus[3] = (width * x4_4mfocus_permy + x_offset * 5000U) / 10000U;

	return 0;
}

unsigned noteParser::Input(int64_t time, int linesize, uint8_t const*frame0) {
	//unsigned i0 = 4; {
	for (unsigned i0 = 6; --i0 != 0U - 1U;) {
		auto n_this = notesSus[i0].begin();

		unsigned y = y_range;
		while (--y > 0) {
			unsigned countColor[mixed + 1] = {};
			Color c_this = GetPixel(frame0, linesize, i0, y);
			if (c_this != black) {
				++countColor[c_this];
				while (--y > 0 && (c_this = GetPixel(frame0, linesize, i0, y)) != black) {
					++countColor[c_this];
				}

				if (n_this == notesSus[i0].end() || y > n_this->pos_Front) {
					n_this = notesSus[i0].emplace(n_this);

					n_this->pos_Front = y;
					n_this->t_FrontEnter = time;
					if (y == 0) n_this->t_FrontExit = time;
					for (unsigned i1 = mixed + 1; --i1 != 0U - 1U;)
						n_this->countColor[i1] = countColor[i1];

					if (n_this == notesSus[i0].begin()) {
						++n_this;
						if (n_this != notesSus[i0].end()) n_this->t_BackEnter = time;
					}
					else {
						n_this->t_BackEnter = time;
						++n_this;
					}
				}
				else {
					if (n_this == notesSus[i0].end()) break;
					if (y == 0 && n_this->pos_Front > 0)
						n_this->t_FrontExit = time_pre;
					n_this->pos_Front = y;
					for (unsigned i1 = mixed + 1; --i1 != 0U - 1U;)
						n_this->countColor[i1] += countColor[i1];
					++n_this;
				}

				if (y == 0) break;
			}
		}

		if (n_this == notesSus[i0].begin() && n_this != notesSus[i0].end())
			n_this->t_BackEnter = time_pre;

		while (n_this != notesSus[i0].end()) {
			Color c_final = DecideColor(n_this->countColor);
			if (c_final == white) {
			}
			else if (c_final == mixed) {
				auto n_next = n_this;
				(--n_next)->isAfterIris = true;

				if (n_this->pRedBefore) *n_this->pRedBefore = '+';
			}
			else {
				if (n_this->pos_Front > 0) n_this->t_FrontExit = time_pre;
				n_this->t_BackExit = time_pre;

				char suffix_this;
				if (c_final == green) suffix_this = 'U';
				else if (c_final == blue) suffix_this = 'L';
				else if (c_final == yellow) suffix_this = 'R';
				else suffix_this = '\0';

				notes.push_back({
					(n_this->t_FrontEnter + n_this->t_FrontExit + n_this->t_BackEnter + n_this->t_BackExit) / 4,
					i0,
					suffix_this,
				});

				if (c_final == red) {
					if (n_this->isAfterIris) notes.back().c_type = '-';
					else {
						auto n_next = n_this;
						(--n_next)->pRedBefore = &notes.back().c_type;
					}
				}
			}
			n_this = notesSus[i0].erase(n_this);
		}
	}
	time_pre = time;
	return 0;
}

noteParser::HsvColor noteParser::RgbToHsv(RgbColor rgb)
{
	HsvColor hsv;
	unsigned char rgbMin, rgbMax;

	rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
	rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

	hsv.v = rgbMax;
	if (hsv.v == 0)
	{
		hsv.h = 0;
		hsv.s = 0;
		return hsv;
	}

	hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
	if (hsv.s == 0)
	{
		hsv.h = 0;
		return hsv;
	}

	if (rgbMax == rgb.r)
		hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
	else if (rgbMax == rgb.g)
		hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
	else
		hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

	return hsv;
}

noteParser::Color noteParser::RecogColor(HsvColor hsv) {
	unsigned const char thres_s_low = 64;
	unsigned const char thres_s_high = 128;
	unsigned const char thres_v_low = 20;
	unsigned const char thres_v_high = 100;
	if (hsv.v < thres_v_low) return black;
	if (hsv.v > thres_v_high && hsv.s >= thres_s_low) {
		if (hsv.h >= 55 && hsv.h <= 90) return green;
	}
	if (hsv.v > thres_v_high && hsv.s >= thres_s_high) {
		if (hsv.h >= 235 || hsv.h <= 12) return red;
		if (hsv.h >= 130 && hsv.h <= 160) return blue;
		if (hsv.h >= 18 && hsv.h <= 39) return yellow;
		return mixed;
	}
	return white;
}

/*noteParser::Color noteParser::GetPixel(uint8_t const*frame0, unsigned linesize, unsigned x, unsigned y)
{
	switch (mode_channel) {
	case mode_2p:
		if (x < 2) return RecogColor(RgbToHsv(RgbColor{
			frame0[(y_2pbottom - y) * 3 + x_2pfocus[x] * linesize],
			frame0[(y_2pbottom - y) * 3 + x_2pfocus[x] * linesize + 1],
			frame0[(y_2pbottom - y) * 3 + x_2pfocus[x] * linesize + 2],
		}));
		else return black;
	case mode_2m:
		if (x < 2) return RecogColor(RgbToHsv(RgbColor{
			frame0[x_2mfocus[x] * 3 + (y_bottom - y) * linesize],
			frame0[x_2mfocus[x] * 3 + (y_bottom - y) * linesize + 1],
			frame0[x_2mfocus[x] * 3 + (y_bottom - y) * linesize + 2],
			}));
		else return black;
	case mode_4m:
		if (x < 4) return RecogColor(RgbToHsv(RgbColor{
			frame0[x_4mfocus[x] * 3 + (y_bottom - y) * linesize],
			frame0[x_4mfocus[x] * 3 + (y_bottom - y) * linesize + 1],
			frame0[x_4mfocus[x] * 3 + (y_bottom - y) * linesize + 2],
			}));
		else return black;
	default:
		return RecogColor(RgbToHsv(RgbColor{
			frame0[x_focus[x] * 3 + (y_bottom - y) * linesize],
			frame0[x_focus[x] * 3 + (y_bottom - y) * linesize + 1],
			frame0[x_focus[x] * 3 + (y_bottom - y) * linesize + 2],
		}));
	}
}*/

noteParser::Color noteParser::DecideColor(unsigned const countColor[mixed + 1])
{
	unsigned count_saluted =
		countColor[red] * 4 +
		countColor[green] * 16 +
		countColor[blue] * 8 +
		countColor[yellow] * 8 +
		countColor[mixed];
	if (count_saluted < countColor[white]) return white;

	count_saluted =
		countColor[red] +
		countColor[green] +
		countColor[blue] +
		countColor[yellow] +
		countColor[mixed];
	if (count_saluted * 7 < countColor[red] * 8) return red;
	if (count_saluted * 7 < countColor[green] * 8) return green;
	if (count_saluted * 7 < countColor[blue] * 8) return blue;
	if (count_saluted * 7 < countColor[yellow] * 8) return yellow;
	return mixed;
}