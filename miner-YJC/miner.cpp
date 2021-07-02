#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#pragma GCC optimize("Ofast")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4.1,sse4.2,avx,popcnt,tune=native")
#define N 256
#define M 128
#define K 64
#define skill_cnt 40

typedef unsigned long long u64_t;
typedef unsigned char u8_t;
#include "config.h"
u8_t freq[16], skill[skill_cnt], name_base[M], val[N];
u8_t p, q, s, name[LEN + 10];
int st[52], V;
int q_len;
inline u8_t gen() {
	q += val[++p];
	std::swap(val[p], val[q]);
	u8_t u = val[(val[p] + val[q]) & 255];
	q += val[++p];
	std::swap(val[p], val[q]);
	return (u << 8 | val[(val[p] + val[q]) & 255]) % skill_cnt;
}

#define median(x, y, z) std::max(std::min(x, y), std::min(std::max(x, y), z))
#define LIM 80
#define WK(x) val[i + x] = val[i + x] * 181 + 160;
#define a name_base
#define bw_threshold 612

template <int len, bool skip>
int load_name(int *arr) {
	q_len = -1;
	memcpy(val, val_base, sizeof val);
	for (int _ = 0; _ < 2; _++)
		for (int i = s = 0, j = 0; i < N; i++, j++) {
			s += name[j];
			s += val[i];
			std::swap(val[i], val[s]);
			if (j == len - 1) j = -1;
		}
	for (int i = 0; i < LIM; i += 8) {
		WK(0) WK(1) WK(2) WK(3) WK(4) WK(5) WK(6) WK(7)
	}
	for (int i = 0; i < LIM && q_len < 30; i++)
		if (val[i] >= 89 && val[i] < 217) name_base[++q_len] = val[i] & 63;

	if (q_len < 30) {
		for (int i = LIM; i < N; i += 8) {
			WK(0) WK(1) WK(2) WK(3) WK(4) WK(5) WK(6) WK(7)
		}
		for (int i = LIM; i < N && q_len < 30; i++)
			if (val[i] >= 89 && val[i] < 217) name_base[++q_len] = val[i] & 63;
	}

	V = 252;
	V += median(a[10], a[11], a[12]);
	V += median(a[13], a[14], a[15]);
	V += median(a[16], a[17], a[18]);
	V += median(a[19], a[20], a[21]);
	V += median(a[22], a[23], a[24]);
	V += median(a[25], a[26], a[27]);
	V += median(a[28], a[29], a[30]);
	if (skip && V < bw_threshold - 132) return 0;
	std::sort(a, a + 10);
	arr[0] = 154 + a[3] + a[4] + a[5] + a[6];
	V += (unsigned)arr[0] / 3;
	if (skip && V < bw_threshold) return 0;
	arr[1] = median(a[10], a[11], a[12]) + 36;
	arr[2] = median(a[13], a[14], a[15]) + 36;
	arr[3] = median(a[16], a[17], a[18]) + 36;
	arr[4] = median(a[19], a[20], a[21]) + 36;
	arr[5] = median(a[22], a[23], a[24]) + 36;
	arr[6] = median(a[25], a[26], a[27]) + 36;
	arr[7] = median(a[28], a[29], a[30]) + 36;
#undef a
	return 1;
}

void calc_skills() {
	q_len = -1;
	memcpy(val, val_base, sizeof val);
	for (int _ = 0; _ < 2; _++)
		for (int i = s = 0, j = 0; i < N; i++, j++) {
			s += name[j];
			s += val[i];
			std::swap(val[i], val[s]);
			if (j == LEN - 1) j = -1;
		}
	for (int i = 0; i < N; i++)
		if (val[i] * 181 + 199 & 128) name_base[++q_len] = val[i] * 53 & 63 ^ 32;

	u8_t *a = name_base + K;
	for (int i = 0; i < skill_cnt; i++) skill[i] = i;
	p = q = s = 0;
	for (int _ = 0; _ < 2; _++)
		for (int i = 0; i < skill_cnt; i++) {
			s = (s + gen() + skill[i]) % skill_cnt;
			std::swap(skill[i], skill[s]);
		}
	int last = -1;
	for (int i = 0, j = 0; j < 16; i += 4, j++) {
		u8_t p = std::min(std::min(a[i], a[i + 1]), std::min(a[i + 2], a[i + 3]));
		if (p > 10) {
			freq[j] = p - 10;
			if (skill[j] < 25) last = j;
		} else
			freq[j] = 0;
	}
	if (last != -1) freq[last] <<= 1;
	if (freq[14] && last != 14)
		freq[14] += std::min(std::min(name_base[60], name_base[61]), freq[14]);
	if (freq[15] && last != 15)
		freq[15] += std::min(std::min(name_base[62], name_base[63]), freq[15]);
}

#include "model-QP.h"

int model_input[1052];
long long l, r;
char fname[N];
int main(int argc, char **argv) {
	sscanf(argv[1], "%lld", &l);
	sscanf(argv[2], "%lld", &r);
	memset(name + 1, ' ', LEN - 1);
	strcpy((char *)name + LEN, "?shadow");
	strcpy((char *)name + 1, argv[3]);
	name[1 + strlen((char *)name + 1)] = ' ';
#ifdef VERBOSE
	clock_t start = clock();
	for (u64_t i = l, j = 1, k = 0; i < r; i++, j++) {
#else
	for (u64_t i = l, j = 1; i < r; i++, j++) {
#endif
		name[LEN - 7] = charset[(i >> 36) & 63];
		name[LEN - 6] = charset[(i >> 30) & 63];
		name[LEN - 5] = charset[(i >> 24) & 63];
		name[LEN - 4] = charset[(i >> 18) & 63];
		name[LEN - 3] = charset[(i >> 12) & 63];
		name[LEN - 2] = charset[(i >> 6) & 63];
		name[LEN - 1] = charset[(i >> 0) & 63];
		if (load_name<LEN, true>(st)) {
			bool yes = false;
			int cnt = 0;

			if (V >= 772) goto yes;
			calc_skills();

			memset(st + 8, 0, 35 * sizeof(int));
			for (int i = 0; i < 16; i++)
				if (freq[i]) st[skill[i] + 8] = freq[i];

			if (st[24 + 8] >= 50) {	 // 幻术
				load_name<LEN + 7, false>(st + 43);
				st[51] = 31 + (std::min(std::min(name_base[64], name_base[65]),
										std::min(name_base[66], name_base[67])) >>
							   1);

				for (int i = 0; i < 52; i++) model_input[cnt++] = st[i];
				for (int i = 0; i < 43; i++)
					for (int j = i; j < 43; j++) model_input[cnt++] = st[i] * st[j];
				for (int i = 0; i < 9; i++) model_input[cnt++] = st[i + 43] * st[24 + 8];
				for (int i = 0; i < 9; i++)
					for (int j = i; j < 9; j++)
						model_input[cnt++] = st[i + 43] * st[j + 43];

				yes |= QP::predict_shadow(model_input) >= 5600;
			} else {
				for (int i = 0; i < 9; i++) st[i + 43] = 0;

				for (int i = 0; i < 43; i++) model_input[cnt++] = st[i];
				for (int i = 0; i < 43; i++)
					for (int j = i; j < 43; j++) model_input[cnt++] = st[i] * st[j];

				yes |= QP::predict(model_input) >= 5500;
			}

			if (yes) {
			yes:;
				for (int i = 1; i < LEN; i++) putchar(name[i]);
				puts("@" TEAM);
				fflush(stdout);
			}
		}
		if (j == 2e6) {
			j -= 2e6;
#ifdef VERBOSE
			k += 2e6;
			clock_t end = clock();
			double tm = 1. * (end - start) / CLOCKS_PER_SEC;
			int time_left = (r - i) * tm / k;
			fprintf(stderr,
					"count: %llu, time: %fs, speed: %f/d, time left: %02d:%02d:%02d.\n",
					k, tm, k / tm * 86400, time_left / 3600, time_left / 60 % 60,
					time_left % 60);
#else
			fputs("\n", stderr);
#endif
		}
	}
	return 0;
}