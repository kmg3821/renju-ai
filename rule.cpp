#include "rule.h"


static int8_t is_inside(const int8_t row, const int8_t col) {
	return 0 <= row && row <= (BDLEN+1) && 0 <= col && col <= (BDLEN+1);
}

static int8_t is_inside_board(const int8_t row, const int8_t col) {
	return 1 <= row && row <= BDLEN && 1 <= col && col <= BDLEN;
}

static int8_t get_three_pattern_cnt(
	const board_t board,
	const pos_t pos,
	const int8_t offset[4][8][2],
	const int8_t pattern[8],
	const int8_t index[3]) {

	int8_t cnt = 0;
	for (int8_t i = 0; i < 3; ++i) { // offset
		int8_t row = pos[0] + offset[index[i]][0][0];
		int8_t col = pos[1] + offset[index[i]][0][1];
		if (!is_inside(row, col)) continue;
		if (pattern[0] != (board[row][col] & BIT)) continue;
		for (int8_t j = 1; j <= 6; ++j) {
			row = pos[0] + offset[index[i]][j][0];
			col = pos[1] + offset[index[i]][j][1];
			if (is_inside(row, col) && pattern[j] != board[row][col]) goto ESC;
		}
		row = pos[0] + offset[index[i]][7][0];
		col = pos[1] + offset[index[i]][7][1];
		if (!is_inside(row, col)) continue;
		if (pattern[7] != (board[row][col] & BIT)) continue;
		cnt++;
	ESC: continue;
	}
	return cnt;
}


static int8_t get_four_pattern_cnt(
	const board_t board,
	const pos_t pos,
	const int8_t offset[4][7][2],
	const int8_t pattern[7],
	const int8_t index[4]) {

		int8_t cnt = 0;
		for (int8_t i = 0; i < 4; ++i) { // offset
		int8_t row = pos[0] + offset[index[i]][0][0];
		int8_t col = pos[1] + offset[index[i]][0][1];
		if (!is_inside(row, col)) continue;
		if (pattern[0] != (board[row][col] & BIT)) continue;
		for (int8_t j = 1; j <= 5; ++j) {
			row = pos[0] + offset[index[i]][j][0];
			col = pos[1] + offset[index[i]][j][1];
			if (is_inside(row, col) && pattern[j] != board[row][col]) goto ESC;
		}
		row = pos[0] + offset[index[i]][6][0];
		col = pos[1] + offset[index[i]][6][1];
		if (!is_inside(row, col)) continue;
		if (pattern[6] != (board[row][col] & BIT)) continue;
		cnt++;
		ESC: continue;
	}
	return cnt;
}

static int8_t get_three(board_t board, const pos_t pos) {
	const int8_t offset[4][4][8][2] = {
		{
			{{0,-2},{0,-1},{0,0},{0,1},{0,2},{0,3},{0,4},{0,5}},
			{{0,-3},{0,-2},{0,-1},{0,0},{0,1},{0,2},{0,3},{0,4}},
			{{0,-4},{0,-3},{0,-2},{0,-1},{0,0},{0,1},{0,2},{0,3}},
			{{0,-5},{0,-4},{0,-3},{0,-2},{0,-1},{0,0},{0,1},{0,2}}
		},
		{
			{{-2,0},{-1,0},{0,0},{1,0},{2,0},{3,0},{4,0},{5,0}},
			{{-3,0},{-2,0},{-1,0},{0,0},{1,0},{2,0},{3,0},{4,0}},
			{{-4,0},{-3,0},{-2,0},{-1,0},{0,0},{1,0},{2,0},{3,0}},
			{{-5,0},{-4,0},{-3,0},{-2,0},{-1,0},{0,0},{1,0},{2,0}}
		},
		{
			{{-2,-2},{-1,-1},{0,0},{1,1},{2,2},{3,3},{4,4},{5,5}},
			{{-3,-3},{-2,-2},{-1,-1},{0,0},{1,1},{2,2},{3,3},{4,4}},
			{{-4,-4},{-3,-3},{-2,-2},{-1,-1},{0,0},{1,1},{2,2},{3,3}},
			{{-5,-5},{-4,-4},{-3,-3},{-2,-2},{-1,-1},{0,0},{1,1},{2,2}},
		},
		{
			{{-2,2},{-1,1},{0,0},{1,-1},{2,-2},{3,-3},{4,-4},{5,-5}},
			{{-3,3},{-2,2},{-1,1},{0,0},{1,-1},{2,-2},{3,-3},{4,-4}},
			{{-4,4},{-3,3},{-2,2},{-1,1},{0,0},{1,-1},{2,-2},{3,-3}},
			{{-5,5},{-4,4},{-3,3},{-2,2},{-1,1},{0,0},{1,-1},{2,-2}},
		}
	};
	const int8_t pattern[4][8] = {
		{0,0,1,1,1,0,0,0}, //NOBBBOON
		{0,0,1,1,0,1,0,0}, //NOBBOBON
		{0,0,1,0,1,1,0,0}, //NOBOBBON
		{0,0,0,1,1,1,0,0}  //NOOBBBON
	};
	const int8_t index[4][3] = {
		{0,1,2},
		{0,1,3},
		{0,2,3},
		{1,2,3},
	};

	board[pos[0]][pos[1]] = BLACK;
	int8_t cnt = 0;
	for (int8_t i = 0; i < 4; ++i) { // direction
		int8_t val[4];
		for (int8_t j = 0; j < 4; ++j) { // pattern
			val[j] = get_three_pattern_cnt(board, pos, offset[i], pattern[j], index[j]);
		}
		cnt += val[1] + val[2] + (val[0] + val[3] + 1) / 2;
	}
	board[pos[0]][pos[1]] = EMPTY;
	return cnt;
}

static int8_t get_four(board_t board, const pos_t pos) {
	const int8_t offset[4][5][7][2] = {
		{
			{{0,-1},{0,0},{0,1},{0,2},{0,3},{0,4},{0,5}},
			{{0,-2},{0,-1},{0,0},{0,1},{0,2},{0,3},{0,4}},
			{{0,-3},{0,-2},{0,-1},{0,0},{0,1},{0,2},{0,3}},
			{{0,-4},{0,-3},{0,-2},{0,-1},{0,0},{0,1},{0,2}},
			{{0,-5},{0,-4},{0,-3},{0,-2},{0,-1},{0,0},{0,1}}
		},
		{
			{{-1,0},{0,0},{1,0},{2,0},{3,0},{4,0},{5,0}},
			{{-2,0},{-1,0},{0,0},{1,0},{2,0},{3,0},{4,0}},
			{{-3,0},{-2,0},{-1,0},{0,0},{1,0},{2,0},{3,0}},
			{{-4,0},{-3,0},{-2,0},{-1,0},{0,0},{1,0},{2,0}},
			{{-5,0},{-4,0},{-3,0},{-2,0},{-1,0},{0,0},{1,0}}
		},
		{
			{{-1,-1},{0,0},{1,1},{2,2},{3,3},{4,4},{5,5}},
			{{-2,-2},{-1,-1},{0,0},{1,1},{2,2},{3,3},{4,4}},
			{{-3,-3},{-2,-2},{-1,-1},{0,0},{1,1},{2,2},{3,3}},
			{{-4,-4},{-3,-3},{-2,-2},{-1,-1},{0,0},{1,1},{2,2}},
			{{-5,-5},{-4,-4},{-3,-3},{-2,-2},{-1,-1},{0,0},{1,1}}
		},
		{
			{{-1,1},{0,0},{1,-1},{2,-2},{3,-3},{4,-4},{5,-5}},
			{{-2,2},{-1,1},{0,0},{1,-1},{2,-2},{3,-3},{4,-4}},
			{{-3,3},{-2,2},{-1,1},{0,0},{1,-1},{2,-2},{3,-3}},
			{{-4,4},{-3,3},{-2,2},{-1,1},{0,0},{1,-1},{2,-2}},
			{{-5,5},{-4,4},{-3,3},{-2,2},{-1,1},{0,0},{1,-1}}
		}
	};
	const int8_t pattern[5][7] = {
		{0,1,1,1,1,0,0}, //NBBBBON
		{0,1,1,1,0,1,0}, //NBBBOBN
		{0,1,1,0,1,1,0}, //NBBOBBN
		{0,1,0,1,1,1,0}, //NBOBBBN
		{0,0,1,1,1,1,0}  //NOBBBBN
	};
	const int8_t index[5][4] = {
		{0,1,2,3},
		{0,1,2,4},
		{0,1,3,4},
		{0,2,3,4},
		{1,2,3,4}
	};

	board[pos[0]][pos[1]] = BLACK;
	int8_t cnt = 0;
	for (int8_t i = 0; i < 4; ++i) { // direction
		int8_t val[5];
		for (int8_t j = 0; j < 5; ++j) { // pattern
			val[j] = get_four_pattern_cnt(board, pos, offset[i], pattern[j], index[j]);
		}
		cnt += val[1] + val[2] + val[3] + (val[0] + val[4] + 1) / 2;
	}
	board[pos[0]][pos[1]] = EMPTY;
	return cnt;
}

static int8_t is_valid_move(board_t board, const pos_t pos) {
	if (!is_inside_board(pos[0], pos[1])) return -1;
	if (board[pos[0]][pos[1]] != 0) return -1;
	
	const int8_t three = get_three(board, pos);
	const int8_t four = get_four(board, pos);
	return three < 2 && four < 2;
}

static void get_black_three_four(board_t board, const pos_t pos, int8_t& three, int8_t& four) {
	const int8_t offset[32][2] = {
		{0,-1},{0,-2},{0,-3},{0,-4},
		{0,1},{0,2},{0,3},{0,4},
		{-1,0},{-2,0},{-3,0},{-4,0},
		{1,0},{2,0},{3,0},{4,0},
		{-1,-1},{-2,-2},{-3,-3},{-4,-4},
		{1,1},{2,2},{3,3},{4,4},
		{-1,1},{-2,2},{-3,3},{-4,4},
		{1,-1},{2,-2},{3,-3},{4,-4}
	};

	int8_t mark[BDSIZE][2];
	int8_t ep = 0;
	for (int8_t i = 0; i < 32; ++i) {
		int8_t p[2];
		p[0] = pos[0] + offset[i][0]; 
		p[1] = pos[1] + offset[i][1];

		if (is_valid_move(board, p) == 0) {
			mark[ep][0] = p[0];
			mark[ep][1] = p[1];
			ep++;
		}
	}

	for (int8_t i = 0; i < ep; ++i) {
		board[mark[i][0]][mark[i][1]] = INVALID;
	}

	three = get_three(board, pos);
	four = get_four(board, pos);
	for (int8_t i = 0; i < ep; ++i) {
		board[mark[i][0]][mark[i][1]] = EMPTY;
	}
}

static int8_t get_side_cnt(const board_t board, const pos_t pos, const dir_t dir, const int8_t player) {
	const int8_t sgn[2] = { -1,1 };
	int8_t cnt = 1;

	for (int8_t i = 0; i < 2; ++i) {
		int8_t row = pos[0] + sgn[i] * dir[0];
		int8_t col = pos[1] + sgn[i] * dir[1];
		while (is_inside_board(row, col) && board[row][col] == player) {
			row += sgn[i] * dir[0];
			col += sgn[i] * dir[1];
			cnt++;
		}
	}
	return cnt;
}

static void get_five_overline(const board_t board, const pos_t pos, const int8_t player, int8_t& five, int8_t& overline) {
	const dir_t dir[4] = {{1,0}, {1,1}, {0,1}, {-1,1}};

	five = 0;
	overline = 0;
	for (int8_t i = 0; i < 4; ++i) {
		const int8_t cnt = get_side_cnt(board, pos, dir[i], player);
		five += (cnt == 5);
		overline += (cnt > 5);
	}
}

int8_t is_black_terminated(board_t board, const pos_t pos) {
	int8_t res[2];
	int8_t flag = CONT;

	int8_t five, overline;
	get_five_overline(board, pos, BLACK, five, overline);
	if (five > 0) flag = WIN;
	else if (overline > 0) flag = LOSE;
	else {
		int8_t three, four;
		get_black_three_four(board, pos, three, four);
		if (three >= 2 || four >= 2) flag = LOSE;
	}

	board[pos[0]][pos[1]] = BLACK;
	return flag;
}

int8_t is_white_terminated(board_t board, const pos_t pos) {
  int8_t res[2];
  int8_t flag = CONT;

  int8_t five, overline;
  get_five_overline(board, pos, WHITE, five, overline);
  if (five > 0 || overline > 0) flag = WIN;

  board[pos[0]][pos[1]] = WHITE;
  return flag;
}

void init_board(board_t board) {
	memset(board, 0, (BDLEN + 2) * (BDLEN + 2) * sizeof(int8_t));
	for (int8_t i = 0; i < BDLEN + 2; ++i) {
		board[0][i] = BORDER;
		board[BDLEN + 1][i] = BORDER;
	}
	for (int8_t i = 0; i < BDLEN + 2; ++i) {
		board[i][0] = BORDER;
		board[i][BDLEN + 1] = BORDER;
	}
}


