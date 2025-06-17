#ifndef SOLVER_H
#define SOLVER_H

bool isSafe(int board[9][9], size_t L, size_t C, int num);

bool fill_board(int board[9][9], size_t L, size_t C);

void solver(const char *input_file);

#endif
