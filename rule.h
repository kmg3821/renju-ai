#ifndef _OMOK_H_
#define _OMOK_H_

#include <cstring>
#include <memory>

#define BDLEN 15
#define BDSIZE (BDLEN*BDLEN)

#define CONT 0
#define WIN  1
#define LOSE 2
#define DRAW 3

#define EMPTY   0 // 0b0000
#define BLACK   1 // 0b0001
#define WHITE   2 // 0b0010
#define INVALID 4 // 0b0100
#define BORDER  4 // 0b0100
#define BIT     1 // 0b0001

typedef int8_t board_t[BDLEN + 2][BDLEN + 2];
typedef int8_t pos_t[2];
typedef int8_t dir_t[2];


static int8_t is_inside(const int8_t row, const int8_t col);

static int8_t is_inside_board(const int8_t row, const int8_t col);

static int8_t get_three_pattern_cnt(
	const board_t board,
	const pos_t pos,
	const int8_t offset[4][8][2],
	const int8_t pattern[8],
	const int8_t index[3]);

	
	static int8_t get_four_pattern_cnt(
		const board_t board, 
		const pos_t pos,
		const int8_t offset[4][7][2],
		const int8_t pattern[7],
		const int8_t index[4]);

static int8_t get_three(board_t board, const pos_t pos);
		
static int8_t get_four(board_t board, const pos_t pos);

static int8_t is_valid_move(board_t board, const pos_t pos);

static void get_black_three_four(board_t board, const pos_t pos, int8_t& three, int8_t& four);

static int8_t get_side_cnt(const board_t board, const pos_t pos, const dir_t dir, const int8_t player);

static void get_five_overline(const board_t board, const pos_t pos, const int8_t player, int8_t& five, int8_t& overline);

int8_t is_black_terminated(board_t board, const pos_t pos);

int8_t is_white_terminated(board_t board, const pos_t pos);

void init_board(board_t board);

#endif // _OMOK_H_
