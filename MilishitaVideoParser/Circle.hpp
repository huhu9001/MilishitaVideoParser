#pragma once
unsigned const trigono[] = {
	10000,
	9877,
	9510,
	8910,
	8090,
	7071,
	5878,
	4540,
	3090,
	1564,
	0,
};
unsigned const n_trigono = sizeof(trigono)/sizeof(unsigned);

int ICos(unsigned theta);
int ISin(unsigned theta);
unsigned ISqrt(unsigned i);