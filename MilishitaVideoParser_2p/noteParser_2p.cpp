#include"..\\MilishitaVideoParser\\noteParser.hpp"

unsigned const noteParser::n_channels = 2U;

unsigned noteParser::GetPixelIndex(unsigned n_channel, int x_offset, int y_offset) const {
	return (y_2pfocus - y_offset) * 3 + (x_2pfocus[n_channel] - x_offset) * linesize;
}

unsigned noteParser::GetCenterPixelIndex(int x_offset, int y_offset) const {
	return (y_2pfocus - y_offset) * 3 + (x_2pcenter - x_offset) * linesize;
}