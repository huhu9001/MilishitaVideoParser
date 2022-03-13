#include "noteParser.hpp"

noteParser::Color noteParser::GetPixel(uint8_t const*frame0, unsigned linesize, unsigned x, unsigned y) {
	return RecogColor(RgbToHsv(RgbColor{
			frame0[x_focus[x] * 3 + (y_bottom - y) * linesize],
			frame0[x_focus[x] * 3 + (y_bottom - y) * linesize + 1],
			frame0[x_focus[x] * 3 + (y_bottom - y) * linesize + 2],
	}));
}