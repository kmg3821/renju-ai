// cl test.cpp .\mongoose\mongoose.c /Zi /EHsc /nologo /std:c++20 /I .\mongoose

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"

#include "search.h"

#include <iostream>
#include <memory>
#include <math.h>
#include <iomanip>

void print_board(board_t board) {
  const char idx[] = "123456789ABCDEF";
  std::cout << "  1 2 3 4 5 6 7 8 9 A B C D E F" << std::endl;
  for(int i = 0; i < BDLEN; ++i) {
    std::cout << idx[i] << ' ';
    for(int j = 0; j < BDLEN; ++j) {
      int8_t val = board[i + 1][j + 1];
      if(val == BLACK) std::cout << "O ";
      else if(val == WHITE) std::cout << "X ";
      else std::cout << "* ";
    }
    std::cout << std::endl;
  }
}

int main() {
  auto model = tflite::FlatBufferModel::BuildFromFile("renju.tflite");
  if (!model) {
    std::cerr << "Failed to load model\n";
    return 1;
  }

  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*model, resolver);
  std::unique_ptr<tflite::Interpreter> interpreter;
  builder(&interpreter);

  if (!interpreter) {
    std::cerr << "Failed to build interpreter\n";
    return 1;
  }

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    std::cerr << "Failed to allocate tensors\n";
    return 1;
  }

  board_t board;
  init_board(board);

  char first;
  while(true) {
    std::cout << "You first? (y/n): ";
    std::cin >> first;
    if(first == 'y' || first == 'n') break;
  }

  int16_t turn = 0;
  int8_t result = 0;
  while(result == 0 && turn < BDSIZE) {
    print_board(board);
    int8_t player = (turn & 1) == 0 ? BLACK : WHITE;
    if(first == 'y') {
      int row, col;
      while(true) {
        std::cout << "row: ";
        std::cin >> std::hex >> row;
        std::cout << "col: ";
        std::cin >> std::hex >> col;
        if(row < 1 || row > BDLEN || col < 1 || col > BDLEN) {
          continue;
        }
        if(board[row][col] != EMPTY) {
          continue;
        }
        break;
      }
      board[row][col] = player;
      pos_t pos = {(int8_t)row, (int8_t)col};
      if(player == BLACK) {
        result = is_black_terminated(board, pos);
      }
      else {
        result = is_white_terminated(board, pos);
      }
    }
    else {
      SearchOutput out = search_move(board, interpreter, turn);
      int16_t action = out.action;
      float_t value = out.value;
      result = out.result;
      pos_t pos = {(int8_t)(action / BDLEN + 1), (int8_t)(action % BDLEN + 1)};
      board[pos[0]][pos[1]] = player;
      std::cout << "AI move:" << action << std::endl;
      std::cout << "AI win: " << (value + 1.0f)/2.0f*100.0f << "%" << std::endl;
    }

    first = (first == 'y') ? 'n' : 'y';
    turn++;
    std::cout << "===================================" << std::endl;
  }
  print_board(board);

  return 0;
}
