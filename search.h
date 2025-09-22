#ifndef _SEARCH_H_
#define _SEARCH_H_  

#include <memory>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "rule.h"

#define SIMNUM 300
#define EXPLORE (0.5f)

struct Node {
  int8_t result;
  int16_t action;
  int32_t parent;
  int32_t child[BDSIZE];
  int32_t visit[BDSIZE];
  float_t prob[BDSIZE];
  float_t value[BDSIZE];
};
typedef struct Node Tree[SIMNUM + 1];

struct SearchOutput {
  int8_t result;
  int16_t action;
  float_t value;
};

typedef std::unique_ptr<tflite::Interpreter> model_t;

template <typename T>
static void rotate90(T x[BDLEN][BDLEN]);

template <typename T>
static void rotate180(T x[BDLEN][BDLEN]);

template <typename T>
static void rotate270(T x[BDLEN][BDLEN]);

void predict(const board_t board, int8_t player, model_t& model);

static int16_t backpropagate(Tree tree, int32_t now, float_t value);

static int16_t get_action(Tree tree, int32_t now, int32_t visit);

static void expand(
  Tree tree, 
  model_t& model,
  board_t board,
  int16_t action,
  int32_t now, 
  int32_t idx, 
  int16_t turn,
  int16_t step,
  float_t& value);


static void simulate(
  board_t board, 
  Tree tree, 
  int16_t turn, 
  int32_t visit,
  int32_t& now,
  int16_t& action,
  int16_t& step);

SearchOutput search_move(const board_t board0, model_t& model, int16_t turn);



#endif // _SEARCH_H_