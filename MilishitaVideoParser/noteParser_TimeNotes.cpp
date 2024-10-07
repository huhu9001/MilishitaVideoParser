#include"noteParser.hpp"

static bool IsSameNoteType(char c1, char c2) {
	return c1 == c2 || (c1 == '\0' || c1 == '+' || c1 == '-') && (c2 == '\0' || c2 == '+' || c2 == '-');
}

static unsigned char IsSame(std::vector<int64_t> const&t1, std::vector<int> const&y1, int64_t t2, int y2) {
	int64_t x, y, xx, yy, xy;
	if (t1.size() == 0 || t1.size() != y1.size()) return 0;

	x = (t1.front() - t2) + (t1.back() - t2);
	y = (y1.front() - y2) + (y1.back() - y2);
	xx = (t1.front() - t2) * (t1.front() - t2) + (t1.back() - t2) * (t1.back() - t2);
	yy = (y1.front() - y2) * (y1.front() - y2) + (y1.back() - y2) * (y1.back() - y2);
	xy = (t1.front() - t2) * (y1.front() - y2) + (t1.back() - t2) * (y1.back() - y2);

	if (xx * 3 == x * x) return yy * 3 == y * y ? '\xFF' : '\0';
	if (yy * 3 == y * y) return '\0';
	return (unsigned char)((xy * 3 - x * y) * (xy * 3 - x * y) * 0xFF / (xx * 3 - x * x) / (yy * 3 - y * y));
}

static int64_t CalcTime(std::vector<int64_t> const&t_note, std::vector<int> const&y_note) {
	std::vector<int64_t>::const_iterator i1;
	std::vector<int>::const_iterator i2;
	int64_t t_av = 0;
	int64_t y_av = 0;
	int64_t num = 0;
	int64_t den = 0;
	int64_t t0 = t_note.front(), y0 = y_note.front();

	if (t_note.size() == 0 || t_note.size() != y_note.size()) return 0;
	
	for (i1 = t_note.begin(), i2 = y_note.begin(); i1 != t_note.end(); ++i1, ++i2) {
		t_av += (*i1 - t0), y_av += *i2;
	}
	
	for(i1 = t_note.begin(), i2 = y_note.begin(); i1 != t_note.end(); ++i1, ++i2) {
		num += ((*i1 - t0) * (int64_t)t_note.size() - t_av) * (*i2 * (int64_t)y_note.size() - y_av);
		den += (*i2 * (int64_t)y_note.size() - y_av) * (*i2 * (int64_t)y_note.size() - y_av);
	}

	if (den != 0) return t0 + (t_av * den - y_av * num) / (den * (int64_t)y_note.size());
	else return 0;
}

unsigned noteParser::TimeNotes(int64_t time) {
	for (unsigned i0 = 0; i0 < n_channels; ++i0) {
		std::vector<char>::iterator icn1 = c_notes[i0].begin();
		std::vector<std::vector<int64_t>>::iterator it1 = t_notes[i0].begin();
		std::vector<std::vector<int>>::iterator iy1 = y_notes[i0].begin();
		std::vector<Circle>::iterator icc1 = circles[i0].begin(), icc2;
		int64_t time_note;

	label_loopstart:
		while (icn1 != c_notes[i0].end()) {
			int y = iy1->back();
			while (icc1 != circles[i0].end() && y >= icc1->y) ++icc1;
			icc2 = icc1;
			while (icc2 != circles[i0].begin()) {
				--icc2;
				if (IsSameNoteType(*icn1, icc2->c_type)) {
					if (y == icc2->y) {
						//video freeze
						++icn1, ++it1, ++iy1;
						goto label_loopstart;
					}
					else if (IsSame(*it1, *iy1, time, icc2->y) > (unsigned char)'\xC0') {
						if (*icn1 == '\0') *icn1 = icc2->c_type;
						it1->push_back(time);
						iy1->push_back(icc2->y);
						goto label_loopstart;
					}
				}
			}

			time_note = CalcTime(*it1, *iy1);
			if (time_note) {
				std::vector<note>::iterator in1 = notes.end(), in2;
			label_loopstart2:
				if (in1 != notes.begin()) {
					--in1;
					if (in1->time - time_note > time_error_permitted) goto label_loopstart2;
					in2 = in1;
				label_loopstart3:
					if (time_note - in1->time <= time_error_permitted) {
						if (in1->n_channel == i0 && IsSameNoteType(in1->c_type, *icn1))
							in1->time = (in1->time + time_note) / 2;
						else if (in1 != notes.begin()) {
							--in1;
							goto label_loopstart3;
						}
						else notes.emplace(++in2, time_note, i0, *icn1);
					}
					else notes.emplace(++in2, time_note, i0, *icn1);
				}
				else notes.emplace(in1, time_note, i0, *icn1);
			}
			icn1 = c_notes[i0].erase(icn1);
			it1 = t_notes[i0].erase(it1);
			iy1 = y_notes[i0].erase(iy1);
		}

		for (; icc1 != circles[i0].end(); ++icc1) {
			c_notes[i0].push_back(icc1->c_type);
			t_notes[i0].emplace_back();
			t_notes[i0].back().push_back(time);
			y_notes[i0].emplace_back();
			y_notes[i0].back().push_back(icc1->y);
		}
	}
	return 0;
}