#include"noteParser.hpp"
#include"color.hpp"
#include"circle.hpp"

#include<new>

static bool IsStripeColor(HsvColor hsv) {
	return hsv.h >= 55 && hsv.h < 150 && hsv.s > 45 && hsv.s < 120
		|| hsv.h >= 40 && hsv.h < 55 && hsv.s > 45 && hsv.s < 175
		|| hsv.h >= 25 && hsv.h < 40 && hsv.s > 75 && hsv.s < 175
		|| hsv.h >= 10 && hsv.h < 25 && hsv.s > 100 && hsv.s < 175
		|| hsv.h < 10 && hsv.s > 100 && hsv.s < 140;
}

void noteParser::StripeSeeker(uint8_t const*frame0) {
	for (unsigned i0 = 0; i0 < n_channels; ++i0) {
		for (Circle&icc1 : circles[i0]) {
			if (icc1.c_type == '\0') {
				for (unsigned i1 = 0; i1 < n_trigono; ++i1) {
					int const icos = ICos(i1), isin = ISin(i1);
					int const ucos = icos >= 0 ? icos : -icos, usin = isin >= 0 ? isin : -isin;
					int const x_src = icc1.x + (int)(icc1.r + y_sigma * 3 / 4) * icos / 10000;
					int const y_src = icc1.y + (int)(icc1.r + y_sigma * 3 / 4) * isin / 10000;

					int xx_delta, yy_delta, x_num, y_num, x_den, y_den;
					unsigned n_valid = 0;

					if (ucos < usin) {
						xx_delta = (icc1.r * 2 - y_sigma / 4) * usin / 10000 * usin / 10000;
						yy_delta = (int)y_sigma * usin / 5000;
						y_num = isin >= 0 ? 1 : -1, y_den = 1;
						x_num = icos, x_den = usin;
					}
					else {
						xx_delta = 1;
						yy_delta = (int)y_sigma * ucos / 5000;
						x_num = icos >= 0 ? 1 : -1, x_den = 1;
						y_num = isin, y_den = ucos;
					}

					for (int xx = 0; xx < xx_delta; ++xx) {//radial step
						unsigned n_dark = 0, n_bright = 0;
						int yy_offset = (xx - xx_delta / 2) * x_den * y_den / 10000;
						yy_offset = (icc1.r - ISqrt(icc1.r * icc1.r - yy_offset * yy_offset)) * x_den * y_den / 10000;
						for (int yy = 0; yy < yy_delta; ++yy) {//axial step
							int const x = x_src + (xx - xx_delta / 2) * y_num / y_den + (yy - yy_offset) * x_num / x_den;
							int const y = y_src - (xx - xx_delta / 2) * x_num / x_den + (yy - yy_offset) * y_num / y_den;
							if (-y > 5 * (int)y_sigma) break;//out of screen
							HsvColor const hsv(*std::launder(reinterpret_cast<RgbColor const*>(frame0 + GetPixelIndex(i0, x, y))));
							if (IsStripeColor(hsv)) {
								if (hsv.l >= 0xA0) ++n_bright;
								else ++n_dark;
							}
							else break;
						}

						if (n_bright > 5 * n_dark) n_valid += n_bright;

						if (n_valid >= 2 * y_sigma) {
							icc1.c_type = isin >= 0 ? '+' : '-';
							goto LB_NEXT_NOTE;
						}
					}
				}
			}
		LB_NEXT_NOTE: continue;
		}
	}
}