#include"noteParser.hpp"
#include"color.hpp"
#include"circle.hpp"

#include<new>

enum NoteColor { white, red, blue, yellow, green, black };

static NoteColor GetPixelColor(uint8_t const*pixel_in_frame) {
	unsigned const char thres_s_low = 64;
	unsigned const char thres_s_high = 128;
	unsigned const char thres_v_low = 20;
	unsigned const char thres_v_high = 100;
	unsigned const char thres_l = 100;
	HsvColor hsv(*std::launder(reinterpret_cast<RgbColor const*>(pixel_in_frame)));
	if (hsv.l < thres_l) return black;
	if (hsv.v > thres_v_high && hsv.s >= thres_s_low) {
		if (hsv.h >= 55 && hsv.h <= 90) return green;
		if (hsv.h >= 228 || hsv.h <= 12) return red;
		if (hsv.h >= 120 && hsv.h <= 160) return blue;
		if (hsv.h >= 18 && hsv.h <= 39) return yellow;
	}
	return white;
}

unsigned noteParser::CircleSeeker(uint8_t const*frame0) {
	for (unsigned i0 = 0; i0 < n_channels; ++i0) {
		int y0 = -1 - (int)y_sigma;
		NoteColor c_last;
		circles[i0].clear();
	label_retry:
		++y0;
		c_last = black;
		for (unsigned countColor[black] = {}; y0 < 4 * (int)y_sigma; ++y0) {
			NoteColor c = GetPixelColor(frame0 + GetPixelIndex(i0, 0, y0));
			if (c == black) {
				NoteColor c_note = black;
				if (c_last != white) goto label_retry;
				{
					unsigned const q = 8;
					unsigned ccn = 0;
					if (countColor[red] > 0) c_note = red, ccn = countColor[red];
					if (countColor[blue] > ccn * q) c_note = blue, ccn = countColor[blue];
					else if (ccn <= countColor[blue] * q)
						c_note = black, ccn = ccn < countColor[blue] ? countColor[blue] : ccn;
					if (countColor[green] > ccn * q) c_note = green, ccn = countColor[green];
					else if (ccn <= countColor[green] * q)
						c_note = black, ccn = ccn < countColor[green] ? countColor[green] : ccn;
					if (countColor[yellow] > ccn * q) c_note = yellow, ccn = countColor[yellow];
					else if (ccn <= countColor[yellow] * q) c_note = black;
				}
				if (c_note != black) {
					int const diroff[][2] = { {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1} };
					int y_circle_1 = y0 - 1;
					int x_up = 0, y_up = y_circle_1;
					unsigned n_up = 1, nn_up = 0;
					int x_down = 0, y_down = y_circle_1;
					unsigned n_down = 1, nn_down = 0;
					int x_left = 0, y_left = y_circle_1;
					unsigned n_left = 1, nn_left = 0;
					int x_right = 0, y_right = y_circle_1;
					unsigned n_right = 1, nn_right = 0;
					unsigned radius;
					int x_this, y_this;
					unsigned dir_this;
					char c_type;

					x_this = 0, y_this = y_circle_1, dir_this = 6U;
					for (unsigned i1 = 0;; ++i1) {
						if (i1 >= 17U * y_sigma) goto label_retry;
						for (int i = 0;; ++i) {
							NoteColor c_dir = GetPixelColor(frame0 + GetPixelIndex(i0, x_this + diroff[dir_this][0], y_this + diroff[dir_this][1]));
							if (c_dir == black) break;
							if (i >= 8) goto label_retry;
							dir_this = (dir_this + 7) % 8;
						}
						for (int i = 0;; ++i) {
							NoteColor c_dir = GetPixelColor(frame0 + GetPixelIndex(i0, x_this + diroff[dir_this][0], y_this + diroff[dir_this][1]));
							if (c_dir != black) break;
							if (i >= 8) goto label_retry;
							dir_this = (dir_this + 1) % 8;
						}
						x_this += diroff[dir_this][0], y_this += diroff[dir_this][1];
						if (x_this == 0 && y_this == y_circle_1) break;
						if ((x_this < 0 ? -x_this : x_this) > 6 * (int)y_sigma) goto label_retry;
						if (y_this > 9 * (int)y_sigma || -y_this > 6 * (int)y_sigma) goto label_retry;

						if (y_this > y_up) x_up = x_this, y_up = y_this, nn_up = n_up, n_up = 1;
						else if (y_this == y_up) x_up += x_this, ++n_up;
						else if (y_this + 1 == y_up) ++nn_up;
						if (y_this < y_down) x_down = x_this, y_down = y_this, nn_down = n_down, n_down = 1;
						else if (y_this == y_down) x_down += x_this, ++n_down;
						else if (y_this == y_down + 1) ++nn_down;
						if (x_this < x_left) x_left = x_this, y_left = y_this, nn_left = n_left, n_left = 1;
						else if (x_this == x_left) y_left += y_this, ++n_left;
						else if (x_this == x_left + 1) ++nn_left;
						if (x_this > x_right) x_right = x_this, y_right = y_this, nn_right = n_right, n_right = 1;
						else if (x_this == x_right) y_right += y_this, ++n_right;
						else if (x_this + 1 == x_right) ++nn_right;
					}

					x_up /= (int)n_up, x_down /= (int)n_down, y_left /= (int)n_left, y_right /= (int)n_right;
					nn_up += n_up, nn_down += n_down, nn_left += n_left, nn_right += n_right;

					if (nn_up + nn_down + nn_left + nn_right < 5U * 3U + 4U) goto label_retry;

					switch (c_note) {
					case red:
						c_type = '\0';
						radius = (x_right - x_left) / 2;
						break;
					case blue:
						c_type = 'L';
						radius = x_right - x_up;
						x_left = 2 * x_up - x_right;
						break;
					case green:
						c_type = 'U';
						radius = (x_right - x_left) / 2;
						y_up = (y_left + y_right + x_right - x_left) / 2;
						break;
					case yellow:
						c_type = 'R';
						radius = x_up - x_left;
						x_right = 2 * x_up - x_left;
						break;
					}

					if (radius > 3 * y_sigma || radius < y_sigma) goto label_retry;
					{
						int x_l, x_r, y_u;
						for (x_l = x_left; GetPixelColor(frame0 + GetPixelIndex(i0, x_l, y_left)) != c_note; ++x_l)
							if (x_l - x_left > (int)y_sigma) goto label_retry;
						for (x_r = x_right; GetPixelColor(frame0 + GetPixelIndex(i0, x_r, y_right)) != c_note; --x_r)
							if (x_right - x_r > (int)y_sigma) goto label_retry;
						for (y_u = y_up; GetPixelColor(frame0 + GetPixelIndex(i0, x_up, y_u)) != c_note; --y_u)
							if (y_up - y_u > (int)y_sigma) goto label_retry;
						for (int xy = 0; GetPixelColor(frame0 + GetPixelIndex(i0, (x_l + x_up) / 2 - xy, (y_left + y_u) / 2 + xy)) != c_note; ++xy)
							if (xy > (int)y_sigma / 2) goto label_retry;
						for (int xy = 0; GetPixelColor(frame0 + GetPixelIndex(i0, (x_r + x_up) / 2 + xy, (y_right + y_u) / 2 + xy)) != c_note; ++xy)
							if (xy > (int)y_sigma / 2) goto label_retry;
					}

					circles[i0].emplace_back(c_type, x_up, (y_left + y_right) / 2, radius);
				}
				goto label_retry;
			}
			else ++countColor[c];
			c_last = c;
		}
	}
	return 0;
}

unsigned noteParser::CenterCircleSeeker(uint8_t const*frame0) {
	for (int y = 2 * (int)y_sigma; y < 7 * (int)y_sigma; ) {
		unsigned r = (y_sigma * 703 - 11 * y) / 88;
		for (unsigned i1 = 0; i1 < (4 * n_trigono - 4); ++i1) {
			int const xc = (int)r * ICos(i1) / 10000;
			int const yc = y + (int)r * ISin(i1) / 10000;
			int const offset = GetCenterPixelIndex(xc, yc);
			if (HsvColor(*std::launder(reinterpret_cast<RgbColor const*>(frame0 + offset))).v < 0xF8)
				goto label_nexty;
		}
		circles[0].emplace_back('!', 0, (int)y, r);
		return 0;
	label_nexty: ++y;
	}
	return 0;
}