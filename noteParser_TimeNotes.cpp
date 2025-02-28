#include"noteParser.hpp"

static bool IsSameNoteType(char c1, char c2) {
	return c1 == c2
		|| (c1 == '\0' || c1 == '+' || c1 == '-')
		&& (c2 == '\0' || c2 == '+' || c2 == '-');
}

//Collinearity of (t1.front(), y1.front()), (t1.back(), y1.back()), (t2, y2)
static bool IsSame(
	std::vector<int64_t>const&t1,
	std::vector<int>const&y1,
	int64_t t2,
	int y2)
{
	int64_t const
		dt_f = t2 - t1.front(), dy_f = y2 - y1.front(),
		dt_b = t2 - t1.back(), dy_b = y2 - y1.back();

	if (dt_f == 0 && dy_f == 0) return dt_b == 0 && dy_b == 0;
	else if (dt_b == 0 && dy_b == 0) return false;

	int64_t const sin = dt_f * dy_b - dy_f * dt_b;
	int64_t const result = 0x100 * sin * sin
		/ (dt_f * dt_f + dy_f * dy_f)
		/ (dt_b * dt_b + dy_b * dy_b);

	return result < 64;
}

//Simple linear regression
//returns 0 on failure
static int64_t CalcTime(std::vector<int64_t>const&t_note, std::vector<int>const&y_note) {
	int64_t const n = t_note.size();
	if (n != y_note.size()) return 0;
	int64_t const t0 = t_note.front();
	
	int64_t t_av = 0;
	int64_t y_av = 0;
	int64_t num = 0;
	int64_t den = 0;

	for (int i = 0; i < n; ++i)
		t_av += (t_note[i] - t0), y_av += y_note[i];
	for (int i = 0; i < n; ++i) {
		num += ((t_note[i] - t0) * n - t_av) * (y_note[i] * n - y_av);
		den += (y_note[i] * n - y_av) * (y_note[i] * n - y_av);
	}

	if (den == 0) return 0;
	return t0 + (t_av * den - y_av * num) / (den * n);
}

void noteParser::TimeNotes(int64_t time) {
	for (unsigned i0 = 0; i0 < n_channels; ++i0) {
		auto icc1 = circles[i0].cbegin();

		auto it1 = t_notes[i0].begin();
		auto iy1 = y_notes[i0].begin();
		for (auto icn1 = c_notes[i0].begin(); icn1 != c_notes[i0].end();) {
			int y = iy1->back();
			//find the last circle with y <= y of this note
			while (icc1 != circles[i0].end() && icc1->y <= y) ++icc1;
			for (auto icc2 = icc1; icc2 != circles[i0].begin();) {
				--icc2;
				if (IsSameNoteType(*icn1, icc2->c_type)) {
					if (y == icc2->y) {
						//video frame loss, invalid data
						++icn1, ++it1, ++iy1;
						goto LB_NEXT_NOTE;
					}
					else if (IsSame(*it1, *iy1, time, icc2->y)) {
						//identify this note with this circle, add data
						if (*icn1 == '\0') *icn1 = icc2->c_type;
						it1->push_back(time);
						iy1->push_back(icc2->y);
						++icn1, ++it1, ++iy1;
						goto LB_NEXT_NOTE;
					}
				}
			}

			//this note passed away, ouput to result
			if (int64_t const time_note = CalcTime(*it1, *iy1); time_note != 0) {
				auto in1 = notes.end();
				do {// find last existing output note within (-infi, t + error]
					if (in1 == notes.begin()) {
						notes.emplace(in1, time_note, i0, *icn1);
						goto LB_POP_AND_NEXT_NOTE;
					}
					--in1;
				}
				while (in1->time - time_note > time_error_permitted);

				for (auto in2 = in1; time_note - in2->time <= time_error_permitted; --in2) {
					//now within [t - error, t + error]
					if (in2->n_channel == i0 && IsSameNoteType(in2->c_type, *icn1)) {
						//same type found, merge
						in2->time = (in2->time + time_note) / 2;
						goto LB_POP_AND_NEXT_NOTE;
					}
					//same type not found, insert
					if (in2 == notes.begin()) break;
				}
				notes.emplace(++in1, time_note, i0, *icn1);
			}
		LB_POP_AND_NEXT_NOTE:
			icn1 = c_notes[i0].erase(icn1);
			it1 = t_notes[i0].erase(it1);
			iy1 = y_notes[i0].erase(iy1);
		LB_NEXT_NOTE: continue;
		}

		//new notes
		for (; icc1 != circles[i0].end(); ++icc1) {
			c_notes[i0].push_back(icc1->c_type);
			t_notes[i0].emplace_back(1, time);
			y_notes[i0].emplace_back(1, icc1->y);
		}
	}
}