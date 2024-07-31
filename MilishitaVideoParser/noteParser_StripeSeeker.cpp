#include"noteParser.hpp"
#include"color.hpp"
#include"circle.hpp"

static bool IsStripeColor(HsvColor const&hsv) {
	return
		hsv.h >= 55 && hsv.h < 150 && hsv.s > 45 && hsv.s < 120 ||
		hsv.h >= 40 && hsv.h < 55 && hsv.s > 45 && hsv.s < 175 ||
		hsv.h >= 25 && hsv.h < 40 && hsv.s > 75 && hsv.s < 175 ||
		hsv.h >= 10 && hsv.h < 25 && hsv.s > 100 && hsv.s < 175 ||
		hsv.h < 10 && hsv.s > 100 && hsv.s < 140;
}

unsigned noteParser::StripeSeeker(uint8_t const*frame0) {
	bool leftempty = true, rightempty = true;

	for (unsigned i0 = 0; i0 < n_channels; ++i0) {
		if (i0 < n_channels / 2) leftempty = leftempty && !circles[i0].empty();
		else rightempty = rightempty && !circles[i0].empty();

		for (Circle&icc1 : circles[i0]) {
			if (icc1.c_type == '\0') {
				for (unsigned i1 = 0; i1 < (4 * n_trigono - 4); ++i1) {
					int icos = ICos(i1), isin = ISin(i1);
					int x_step = icos >= 0 ? 1 : -1;
					int y_step = isin >= 0 ? 1 : -1;
					int xx_delta, yy_delta, x_num, y_num, x_den, y_den;
					int x_src = icc1.x + (int)(icc1.r + y_sigma * 3 / 4) * icos / 10000;
					int y_src = icc1.y + (int)(icc1.r + y_sigma * 3 / 4) * isin / 10000;
					unsigned n_valid = 0;

					icos *= x_step, isin *= y_step;
					if (icos < isin) {
						xx_delta = (icc1.r * 2 - y_sigma / 4) * isin / 10000 * isin / 10000;
						yy_delta = (int)y_sigma * isin / 5000;
						y_num = 1, y_den = 1;
						x_num = icos, x_den = isin;
					}
					else {
						xx_delta = 1;
						yy_delta = (int)y_sigma * icos / 5000;
						x_num = 1, x_den = 1;
						y_num = isin, y_den = icos;
					}

					for (int xx = 0; xx < xx_delta; ++xx) {
						unsigned n_dark = 0, n_bright = 0;
						int yy_offset = (xx - xx_delta / 2) * x_den * y_den / 10000;
						yy_offset = (icc1.r - ISqrt(icc1.r * icc1.r - yy_offset * yy_offset)) * x_den * y_den / 10000;
						for (int yy = 0; yy < yy_delta; ++yy) {
							int x = x_src
								+ ((xx - xx_delta / 2) * y_num + y_den / 2) / y_den * y_step
								+ ((yy - yy_offset) * x_num + x_den / 2) / x_den * x_step;
							int y = y_src
								- ((xx - xx_delta / 2) * x_num + x_den / 2) / x_den * x_step
								+ ((yy - yy_offset) * y_num + y_den / 2) / y_den * y_step;
							HsvColor hsv;
							if (-y > 5 * (int)y_sigma) break;
							if (IsStripeColor(hsv = RgbToHsv(*(RgbColor*)(frame0 + GetPixelIndex(i0, x, y))))) {
								if (hsv.l >= 0xA0) ++n_bright;
								else ++n_dark;
							}
							else break;
						}

						if (n_bright > 5 * n_dark) n_valid += n_bright;

						if (n_valid >= 2 * y_sigma) {
							icc1.c_type = y_step >= 0 ? '+' : '-';
							goto label_nextnote;
						}
					}
				}
			}
		label_nextnote: (void)0;
		}
	}

	return 0;
}