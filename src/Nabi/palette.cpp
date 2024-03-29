#include "pch.h"
#include "palette.h"

template < int nBPP >
std::vector<gtl::color_bgra_t> GetGrayScalePalette() {
	constexpr size_t s = 1 << nBPP;
	static_assert (gtl::IsValueOneOf(s, 2, 16, 256));

	std::vector<gtl::color_bgra_t> p(s);
	for (int i = 0; i < p.size(); i++) {
		auto v = (uint8_t)(i * 255 / (p.size() - 1));
		p[i] = gtl::color_bgra_t{v, v, v, 255};
	}
	return p;
}

std::vector<gtl::color_bgra_t> const s_palette_1bit_bw {
	gtl::color_bgra_t{   0,   0,   0},	// 0
	gtl::color_bgra_t{ 255, 255, 255},	// 1
};

std::vector<gtl::color_bgra_t> const s_palette_4bit_grayscale = GetGrayScalePalette<4>();
std::vector<gtl::color_bgra_t> const s_palette_4bit_color {
	gtl::color_bgra_t{ 255, 255, 255},	// 0
	gtl::color_bgra_t{   0,   0,   0},	// 1
	gtl::color_bgra_t{ 120, 250,  70},	// 2
	gtl::color_bgra_t{  80,  50, 230},	// 3
	gtl::color_bgra_t{  80, 180, 250},	// 4
	gtl::color_bgra_t{ 225, 110,  10},	// 5
	gtl::color_bgra_t{ 240,   0,   0},	// 6
	gtl::color_bgra_t{ 170,  50, 240},	// 7
	gtl::color_bgra_t{ 240,   0,  50},	// 8
	gtl::color_bgra_t{ 220,   0,   0},	// 9
	gtl::color_bgra_t{ 200,   0,   0},	// 10
	gtl::color_bgra_t{ 170,   0,   0},	// 11
	gtl::color_bgra_t{ 150,   0,   0},	// 12
	gtl::color_bgra_t{ 130,   0,   0},	// 13
	gtl::color_bgra_t{ 100,   0,   0},	// 14
	gtl::color_bgra_t{ 200, 100, 100},	// 15
};

std::vector<gtl::color_bgra_t> const s_palette_8bit_grayscale = GetGrayScalePalette<8>();
std::vector<gtl::color_bgra_t> const s_palette_8bit_color {
	gtl::color_bgra_t{ 0x00, 0x00, 0x00, },	// 0
	gtl::color_bgra_t{ 0x0b, 0x0b, 0x0b, },	// 1
	gtl::color_bgra_t{ 0x22, 0x22, 0x22, },	// 2
	gtl::color_bgra_t{ 0x44, 0x44, 0x44, },	// 3
	gtl::color_bgra_t{ 0x55, 0x55, 0x55, },	// 4
	gtl::color_bgra_t{ 0x77, 0x77, 0x77, },	// 5
	gtl::color_bgra_t{ 0x88, 0x88, 0x88, },	// 6
	gtl::color_bgra_t{ 0xaa, 0xaa, 0xaa, },	// 7
	gtl::color_bgra_t{ 0xbb, 0xbb, 0xbb, },	// 8
	gtl::color_bgra_t{ 0xdd, 0xdd, 0xdd, },	// 9
	gtl::color_bgra_t{ 0xee, 0xee, 0xee, },	// 10
	gtl::color_bgra_t{ 0x0b, 0x00, 0x00, },	// 11
	gtl::color_bgra_t{ 0x22, 0x00, 0x00, },	// 12
	gtl::color_bgra_t{ 0x44, 0x00, 0x00, },	// 13
	gtl::color_bgra_t{ 0x55, 0x00, 0x00, },	// 14
	gtl::color_bgra_t{ 0x77, 0x00, 0x00, },	// 15
	gtl::color_bgra_t{ 0x88, 0x00, 0x00, },	// 16
	gtl::color_bgra_t{ 0xaa, 0x00, 0x00, },	// 17
	gtl::color_bgra_t{ 0xbb, 0x00, 0x00, },	// 18
	gtl::color_bgra_t{ 0xdd, 0x00, 0x00, },	// 19
	gtl::color_bgra_t{ 0xee, 0x00, 0x00, },	// 20
	gtl::color_bgra_t{ 0x00, 0x0b, 0x00, },	// 21
	gtl::color_bgra_t{ 0x00, 0x22, 0x00, },	// 22
	gtl::color_bgra_t{ 0x00, 0x44, 0x00, },	// 23
	gtl::color_bgra_t{ 0x00, 0x55, 0x00, },	// 24
	gtl::color_bgra_t{ 0x00, 0x77, 0x00, },	// 25
	gtl::color_bgra_t{ 0x00, 0x88, 0x00, },	// 26
	gtl::color_bgra_t{ 0x00, 0xaa, 0x00, },	// 27
	gtl::color_bgra_t{ 0x00, 0xbb, 0x00, },	// 28
	gtl::color_bgra_t{ 0x00, 0xdd, 0x00, },	// 29
	gtl::color_bgra_t{ 0x00, 0xee, 0x00, },	// 30
	gtl::color_bgra_t{ 0x00, 0x00, 0x0b, },	// 31
	gtl::color_bgra_t{ 0x00, 0x00, 0x22, },	// 32
	gtl::color_bgra_t{ 0x00, 0x00, 0x44, },	// 33
	gtl::color_bgra_t{ 0x00, 0x00, 0x55, },	// 34
	gtl::color_bgra_t{ 0x00, 0x00, 0x77, },	// 35
	gtl::color_bgra_t{ 0x00, 0x00, 0x88, },	// 36
	gtl::color_bgra_t{ 0x00, 0x00, 0xaa, },	// 37
	gtl::color_bgra_t{ 0x00, 0x00, 0xbb, },	// 38
	gtl::color_bgra_t{ 0x00, 0x00, 0xdd, },	// 39
	gtl::color_bgra_t{ 0x00, 0x00, 0xee, },	// 40
	gtl::color_bgra_t{ 0x33, 0x00, 0x00, },	// 41
	gtl::color_bgra_t{ 0x66, 0x00, 0x00, },	// 42
	gtl::color_bgra_t{ 0x99, 0x00, 0x00, },	// 43
	gtl::color_bgra_t{ 0xcc, 0x00, 0x00, },	// 44
	gtl::color_bgra_t{ 0xff, 0x00, 0x00, },	// 45
	gtl::color_bgra_t{ 0x00, 0x33, 0x00, },	// 46
	gtl::color_bgra_t{ 0x33, 0x33, 0x00, },	// 47
	gtl::color_bgra_t{ 0x66, 0x33, 0x00, },	// 48
	gtl::color_bgra_t{ 0x99, 0x33, 0x00, },	// 49
	gtl::color_bgra_t{ 0xcc, 0x33, 0x00, },	// 50
	gtl::color_bgra_t{ 0xff, 0x33, 0x00, },	// 51
	gtl::color_bgra_t{ 0x00, 0x66, 0x00, },	// 52
	gtl::color_bgra_t{ 0x33, 0x66, 0x00, },	// 53
	gtl::color_bgra_t{ 0x66, 0x66, 0x00, },	// 54
	gtl::color_bgra_t{ 0x99, 0x66, 0x00, },	// 55
	gtl::color_bgra_t{ 0xcc, 0x66, 0x00, },	// 56
	gtl::color_bgra_t{ 0xff, 0x66, 0x00, },	// 57
	gtl::color_bgra_t{ 0x00, 0x99, 0x00, },	// 58
	gtl::color_bgra_t{ 0x33, 0x99, 0x00, },	// 59
	gtl::color_bgra_t{ 0x66, 0x99, 0x00, },	// 60
	gtl::color_bgra_t{ 0x99, 0x99, 0x00, },	// 61
	gtl::color_bgra_t{ 0xcc, 0x99, 0x00, },	// 62
	gtl::color_bgra_t{ 0xff, 0x99, 0x00, },	// 63
	gtl::color_bgra_t{ 0x00, 0xcc, 0x00, },	// 64
	gtl::color_bgra_t{ 0x33, 0xcc, 0x00, },	// 65
	gtl::color_bgra_t{ 0x66, 0xcc, 0x00, },	// 66
	gtl::color_bgra_t{ 0x99, 0xcc, 0x00, },	// 67
	gtl::color_bgra_t{ 0xcc, 0xcc, 0x00, },	// 68
	gtl::color_bgra_t{ 0xff, 0xcc, 0x00, },	// 69
	gtl::color_bgra_t{ 0x00, 0xff, 0x00, },	// 70
	gtl::color_bgra_t{ 0x33, 0xff, 0x00, },	// 71
	gtl::color_bgra_t{ 0x66, 0xff, 0x00, },	// 72
	gtl::color_bgra_t{ 0x99, 0xff, 0x00, },	// 73
	gtl::color_bgra_t{ 0xcc, 0xff, 0x00, },	// 74
	gtl::color_bgra_t{ 0xff, 0xff, 0x00, },	// 75
	gtl::color_bgra_t{ 0x00, 0x00, 0x33, },	// 76
	gtl::color_bgra_t{ 0x33, 0x00, 0x33, },	// 77
	gtl::color_bgra_t{ 0x66, 0x00, 0x33, },	// 78
	gtl::color_bgra_t{ 0x99, 0x00, 0x33, },	// 79
	gtl::color_bgra_t{ 0xcc, 0x00, 0x33, },	// 80
	gtl::color_bgra_t{ 0xff, 0x00, 0x33, },	// 81
	gtl::color_bgra_t{ 0x00, 0x33, 0x33, },	// 82
	gtl::color_bgra_t{ 0x33, 0x33, 0x33, },	// 83
	gtl::color_bgra_t{ 0x66, 0x33, 0x33, },	// 84
	gtl::color_bgra_t{ 0x99, 0x33, 0x33, },	// 85
	gtl::color_bgra_t{ 0xcc, 0x33, 0x33, },	// 86
	gtl::color_bgra_t{ 0xff, 0x33, 0x33, },	// 87
	gtl::color_bgra_t{ 0x00, 0x66, 0x33, },	// 88
	gtl::color_bgra_t{ 0x33, 0x66, 0x33, },	// 89
	gtl::color_bgra_t{ 0x66, 0x66, 0x33, },	// 90
	gtl::color_bgra_t{ 0x99, 0x66, 0x33, },	// 91
	gtl::color_bgra_t{ 0xcc, 0x66, 0x33, },	// 92
	gtl::color_bgra_t{ 0xff, 0x66, 0x33, },	// 93
	gtl::color_bgra_t{ 0x00, 0x99, 0x33, },	// 94
	gtl::color_bgra_t{ 0x33, 0x99, 0x33, },	// 95
	gtl::color_bgra_t{ 0x66, 0x99, 0x33, },	// 96
	gtl::color_bgra_t{ 0x99, 0x99, 0x33, },	// 97
	gtl::color_bgra_t{ 0xcc, 0x99, 0x33, },	// 98
	gtl::color_bgra_t{ 0xff, 0x99, 0x33, },	// 99
	gtl::color_bgra_t{ 0x00, 0xcc, 0x33, },	// 100
	gtl::color_bgra_t{ 0x33, 0xcc, 0x33, },	// 101
	gtl::color_bgra_t{ 0x66, 0xcc, 0x33, },	// 102
	gtl::color_bgra_t{ 0x99, 0xcc, 0x33, },	// 103
	gtl::color_bgra_t{ 0xcc, 0xcc, 0x33, },	// 104
	gtl::color_bgra_t{ 0xff, 0xcc, 0x33, },	// 105
	gtl::color_bgra_t{ 0x00, 0xff, 0x33, },	// 106
	gtl::color_bgra_t{ 0x33, 0xff, 0x33, },	// 107
	gtl::color_bgra_t{ 0x66, 0xff, 0x33, },	// 108
	gtl::color_bgra_t{ 0x99, 0xff, 0x33, },	// 109
	gtl::color_bgra_t{ 0xcc, 0xff, 0x33, },	// 110
	gtl::color_bgra_t{ 0xff, 0xff, 0x33, },	// 111
	gtl::color_bgra_t{ 0x00, 0x00, 0x66, },	// 112
	gtl::color_bgra_t{ 0x33, 0x00, 0x66, },	// 113
	gtl::color_bgra_t{ 0x66, 0x00, 0x66, },	// 114
	gtl::color_bgra_t{ 0x99, 0x00, 0x66, },	// 115
	gtl::color_bgra_t{ 0xcc, 0x00, 0x66, },	// 116
	gtl::color_bgra_t{ 0xff, 0x00, 0x66, },	// 117
	gtl::color_bgra_t{ 0x00, 0x33, 0x66, },	// 118
	gtl::color_bgra_t{ 0x33, 0x33, 0x66, },	// 119
	gtl::color_bgra_t{ 0x66, 0x33, 0x66, },	// 120
	gtl::color_bgra_t{ 0x99, 0x33, 0x66, },	// 121
	gtl::color_bgra_t{ 0xcc, 0x33, 0x66, },	// 122
	gtl::color_bgra_t{ 0xff, 0x33, 0x66, },	// 123
	gtl::color_bgra_t{ 0x00, 0x66, 0x66, },	// 124
	gtl::color_bgra_t{ 0x33, 0x66, 0x66, },	// 125
	gtl::color_bgra_t{ 0x66, 0x66, 0x66, },	// 126
	gtl::color_bgra_t{ 0x99, 0x66, 0x66, },	// 127
	gtl::color_bgra_t{ 0xcc, 0x66, 0x66, },	// 128
	gtl::color_bgra_t{ 0xff, 0x66, 0x66, },	// 129
	gtl::color_bgra_t{ 0x00, 0x99, 0x66, },	// 130
	gtl::color_bgra_t{ 0x33, 0x99, 0x66, },	// 131
	gtl::color_bgra_t{ 0x66, 0x99, 0x66, },	// 132
	gtl::color_bgra_t{ 0x99, 0x99, 0x66, },	// 133
	gtl::color_bgra_t{ 0xcc, 0x99, 0x66, },	// 134
	gtl::color_bgra_t{ 0xff, 0x99, 0x66, },	// 135
	gtl::color_bgra_t{ 0x00, 0xcc, 0x66, },	// 136
	gtl::color_bgra_t{ 0x33, 0xcc, 0x66, },	// 137
	gtl::color_bgra_t{ 0x66, 0xcc, 0x66, },	// 138
	gtl::color_bgra_t{ 0x99, 0xcc, 0x66, },	// 139
	gtl::color_bgra_t{ 0xcc, 0xcc, 0x66, },	// 140
	gtl::color_bgra_t{ 0xff, 0xcc, 0x66, },	// 141
	gtl::color_bgra_t{ 0x00, 0xff, 0x66, },	// 142
	gtl::color_bgra_t{ 0x33, 0xff, 0x66, },	// 143
	gtl::color_bgra_t{ 0x66, 0xff, 0x66, },	// 144
	gtl::color_bgra_t{ 0x99, 0xff, 0x66, },	// 145
	gtl::color_bgra_t{ 0xcc, 0xff, 0x66, },	// 146
	gtl::color_bgra_t{ 0xff, 0xff, 0x66, },	// 147
	gtl::color_bgra_t{ 0x00, 0x00, 0x99, },	// 148
	gtl::color_bgra_t{ 0x33, 0x00, 0x99, },	// 149
	gtl::color_bgra_t{ 0x66, 0x00, 0x99, },	// 150
	gtl::color_bgra_t{ 0x99, 0x00, 0x99, },	// 151
	gtl::color_bgra_t{ 0xcc, 0x00, 0x99, },	// 152
	gtl::color_bgra_t{ 0xff, 0x00, 0x99, },	// 153
	gtl::color_bgra_t{ 0x00, 0x33, 0x99, },	// 154
	gtl::color_bgra_t{ 0x33, 0x33, 0x99, },	// 155
	gtl::color_bgra_t{ 0x66, 0x33, 0x99, },	// 156
	gtl::color_bgra_t{ 0x99, 0x33, 0x99, },	// 157
	gtl::color_bgra_t{ 0xcc, 0x33, 0x99, },	// 158
	gtl::color_bgra_t{ 0xff, 0x33, 0x99, },	// 159
	gtl::color_bgra_t{ 0x00, 0x66, 0x99, },	// 160
	gtl::color_bgra_t{ 0x33, 0x66, 0x99, },	// 161
	gtl::color_bgra_t{ 0x66, 0x66, 0x99, },	// 162
	gtl::color_bgra_t{ 0x99, 0x66, 0x99, },	// 163
	gtl::color_bgra_t{ 0xcc, 0x66, 0x99, },	// 164
	gtl::color_bgra_t{ 0xff, 0x66, 0x99, },	// 165
	gtl::color_bgra_t{ 0x00, 0x99, 0x99, },	// 166
	gtl::color_bgra_t{ 0x33, 0x99, 0x99, },	// 167
	gtl::color_bgra_t{ 0x66, 0x99, 0x99, },	// 168
	gtl::color_bgra_t{ 0x99, 0x99, 0x99, },	// 169
	gtl::color_bgra_t{ 0xcc, 0x99, 0x99, },	// 170
	gtl::color_bgra_t{ 0xff, 0x99, 0x99, },	// 171
	gtl::color_bgra_t{ 0x00, 0xcc, 0x99, },	// 172
	gtl::color_bgra_t{ 0x33, 0xcc, 0x99, },	// 173
	gtl::color_bgra_t{ 0x66, 0xcc, 0x99, },	// 174
	gtl::color_bgra_t{ 0x99, 0xcc, 0x99, },	// 175
	gtl::color_bgra_t{ 0xcc, 0xcc, 0x99, },	// 176
	gtl::color_bgra_t{ 0xff, 0xcc, 0x99, },	// 177
	gtl::color_bgra_t{ 0x00, 0xff, 0x99, },	// 178
	gtl::color_bgra_t{ 0x33, 0xff, 0x99, },	// 179
	gtl::color_bgra_t{ 0x66, 0xff, 0x99, },	// 180
	gtl::color_bgra_t{ 0x99, 0xff, 0x99, },	// 181
	gtl::color_bgra_t{ 0xcc, 0xff, 0x99, },	// 182
	gtl::color_bgra_t{ 0xff, 0xff, 0x99, },	// 183
	gtl::color_bgra_t{ 0x00, 0x00, 0xcc, },	// 184
	gtl::color_bgra_t{ 0x33, 0x00, 0xcc, },	// 185
	gtl::color_bgra_t{ 0x66, 0x00, 0xcc, },	// 186
	gtl::color_bgra_t{ 0x99, 0x00, 0xcc, },	// 187
	gtl::color_bgra_t{ 0xcc, 0x00, 0xcc, },	// 188
	gtl::color_bgra_t{ 0xff, 0x00, 0xcc, },	// 189
	gtl::color_bgra_t{ 0x00, 0x33, 0xcc, },	// 190
	gtl::color_bgra_t{ 0x33, 0x33, 0xcc, },	// 191
	gtl::color_bgra_t{ 0x66, 0x33, 0xcc, },	// 192
	gtl::color_bgra_t{ 0x99, 0x33, 0xcc, },	// 193
	gtl::color_bgra_t{ 0xcc, 0x33, 0xcc, },	// 194
	gtl::color_bgra_t{ 0xff, 0x33, 0xcc, },	// 195
	gtl::color_bgra_t{ 0x00, 0x66, 0xcc, },	// 196
	gtl::color_bgra_t{ 0x33, 0x66, 0xcc, },	// 197
	gtl::color_bgra_t{ 0x66, 0x66, 0xcc, },	// 198
	gtl::color_bgra_t{ 0x99, 0x66, 0xcc, },	// 199
	gtl::color_bgra_t{ 0xcc, 0x66, 0xcc, },	// 200
	gtl::color_bgra_t{ 0xff, 0x66, 0xcc, },	// 201
	gtl::color_bgra_t{ 0x00, 0x99, 0xcc, },	// 202
	gtl::color_bgra_t{ 0x33, 0x99, 0xcc, },	// 203
	gtl::color_bgra_t{ 0x66, 0x99, 0xcc, },	// 204
	gtl::color_bgra_t{ 0x99, 0x99, 0xcc, },	// 205
	gtl::color_bgra_t{ 0xcc, 0x99, 0xcc, },	// 206
	gtl::color_bgra_t{ 0xff, 0x99, 0xcc, },	// 207
	gtl::color_bgra_t{ 0x00, 0xcc, 0xcc, },	// 208
	gtl::color_bgra_t{ 0x33, 0xcc, 0xcc, },	// 209
	gtl::color_bgra_t{ 0x66, 0xcc, 0xcc, },	// 210
	gtl::color_bgra_t{ 0x99, 0xcc, 0xcc, },	// 211
	gtl::color_bgra_t{ 0xcc, 0xcc, 0xcc, },	// 212
	gtl::color_bgra_t{ 0xff, 0xcc, 0xcc, },	// 213
	gtl::color_bgra_t{ 0x00, 0xff, 0xcc, },	// 214
	gtl::color_bgra_t{ 0x33, 0xff, 0xcc, },	// 215
	gtl::color_bgra_t{ 0x66, 0xff, 0xcc, },	// 216
	gtl::color_bgra_t{ 0x99, 0xff, 0xcc, },	// 217
	gtl::color_bgra_t{ 0xcc, 0xff, 0xcc, },	// 218
	gtl::color_bgra_t{ 0xff, 0xff, 0xcc, },	// 219
	gtl::color_bgra_t{ 0x00, 0x00, 0xff, },	// 220
	gtl::color_bgra_t{ 0x33, 0x00, 0xff, },	// 221
	gtl::color_bgra_t{ 0x66, 0x00, 0xff, },	// 222
	gtl::color_bgra_t{ 0x99, 0x00, 0xff, },	// 223
	gtl::color_bgra_t{ 0xcc, 0x00, 0xff, },	// 224
	gtl::color_bgra_t{ 0xff, 0x00, 0xff, },	// 225
	gtl::color_bgra_t{ 0x00, 0x33, 0xff, },	// 226
	gtl::color_bgra_t{ 0x33, 0x33, 0xff, },	// 227
	gtl::color_bgra_t{ 0x66, 0x33, 0xff, },	// 228
	gtl::color_bgra_t{ 0x99, 0x33, 0xff, },	// 229
	gtl::color_bgra_t{ 0xcc, 0x33, 0xff, },	// 230
	gtl::color_bgra_t{ 0xff, 0x33, 0xff, },	// 231
	gtl::color_bgra_t{ 0x00, 0x66, 0xff, },	// 232
	gtl::color_bgra_t{ 0x33, 0x66, 0xff, },	// 233
	gtl::color_bgra_t{ 0x66, 0x66, 0xff, },	// 234
	gtl::color_bgra_t{ 0x99, 0x66, 0xff, },	// 235
	gtl::color_bgra_t{ 0xcc, 0x66, 0xff, },	// 236
	gtl::color_bgra_t{ 0xff, 0x66, 0xff, },	// 237
	gtl::color_bgra_t{ 0x00, 0x99, 0xff, },	// 238
	gtl::color_bgra_t{ 0x33, 0x99, 0xff, },	// 239
	gtl::color_bgra_t{ 0x66, 0x99, 0xff, },	// 240
	gtl::color_bgra_t{ 0x99, 0x99, 0xff, },	// 241
	gtl::color_bgra_t{ 0xcc, 0x99, 0xff, },	// 242
	gtl::color_bgra_t{ 0xff, 0x99, 0xff, },	// 243
	gtl::color_bgra_t{ 0x00, 0xcc, 0xff, },	// 244
	gtl::color_bgra_t{ 0x33, 0xcc, 0xff, },	// 245
	gtl::color_bgra_t{ 0x66, 0xcc, 0xff, },	// 246
	gtl::color_bgra_t{ 0x99, 0xcc, 0xff, },	// 247
	gtl::color_bgra_t{ 0xcc, 0xcc, 0xff, },	// 248
	gtl::color_bgra_t{ 0xff, 0xcc, 0xff, },	// 249
	gtl::color_bgra_t{ 0x00, 0xff, 0xff, },	// 250
	gtl::color_bgra_t{ 0x33, 0xff, 0xff, },	// 251
	gtl::color_bgra_t{ 0x66, 0xff, 0xff, },	// 252
	gtl::color_bgra_t{ 0x99, 0xff, 0xff, },	// 253
	gtl::color_bgra_t{ 0xcc, 0xff, 0xff, },	// 254
	gtl::color_bgra_t{ 0xff, 0xff, 0xff, },	// 255
};

