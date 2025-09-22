
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

#include "search.h"

template <typename T>
static void rotate90(T x[BDLEN][BDLEN]) {
  for(int i = 0; i < BDLEN/2; ++i) {
    for(int j = 0; j < BDLEN - 2*i - 1; ++j) {
      T tmp = x[i+j][BDLEN-1-i];
      x[i+j][BDLEN-1-i] = x[i][i+j];
      x[i][i+j] = x[BDLEN-1-(i+j)][i];
      x[BDLEN-1-(i+j)][i] = x[BDLEN-1-i][BDLEN-1-(i+j)];
      x[BDLEN-1-i][BDLEN-1-(i+j)] = tmp;
    }
  }
}

template <typename T>
static void rotate180(T x[BDLEN][BDLEN]) {
  for(int i = 0; i < BDLEN/2; ++i) {
    for(int j = 0; j < BDLEN - 2*i - 1; ++j) {
      T tmp = x[i+j][BDLEN-1-i];
      x[i+j][BDLEN-1-i] = x[BDLEN-1-(i+j)][i];
      x[BDLEN-1-(i+j)][i] = tmp;
      tmp = x[i][i+j];
      x[i][i+j] = x[BDLEN-1-i][BDLEN-1-(i+j)];
      x[BDLEN-1-i][BDLEN-1-(i+j)] = tmp;
    }
  }
}

template <typename T>
static void rotate270(T x[BDLEN][BDLEN]) {
  for(int i = 0; i < BDLEN/2; ++i) {
    for(int j = 0; j < BDLEN - 2*i - 1; ++j) {
      T tmp = x[i+j][BDLEN-1-i];
      x[i+j][BDLEN-1-i] = x[BDLEN-1-i][BDLEN-1-(i+j)];
      x[BDLEN-1-i][BDLEN-1-(i+j)] = x[BDLEN-1-(i+j)][i];
      x[BDLEN-1-(i+j)][i] = x[i][i+j];
      x[i][i+j] = tmp;
    }
  }
}

void predict(const board_t board, int8_t player, model_t& model) {
    
  int8_t rotated[BDLEN][BDLEN];
  for(int i = 0; i < BDLEN; ++i) {
    memcpy(rotated[i], &board[i+1][1], BDLEN * sizeof(int8_t));
  }
    
  int rn = rand() % 4;
  switch (rn) {
    case 1: rotate90<int8_t>(rotated); break;
    case 2: rotate180<int8_t>(rotated); break;
    case 3: rotate270<int8_t>(rotated); break;
    default: break;
  }

  float_t* input = model->typed_input_tensor<float_t>(0);
  float_t (*input_3d)[BDLEN][3] = (float_t (*)[BDLEN][3])input;
  for(int i = 0; i < BDLEN; ++i) {
    for(int j = 0; j < BDLEN; ++j) {
      input_3d[i][j][0] = (rotated[i][j] == 1) ? 1.0f : 0.0f;
      input_3d[i][j][1] = (rotated[i][j] == 2) ? 1.0f : 0.0f;
      input_3d[i][j][2] = (float_t)(player - 1);
    }
  }

  model->Invoke();
  float_t* policy = model->typed_output_tensor<float_t>(0);
  float_t (*policy_2d)[BDLEN] = (float_t (*)[BDLEN])policy;
  float_t value = *model->typed_output_tensor<float_t>(1);
  

  float_t max_val = *std::max_element(policy, policy + BDSIZE);
  float_t exp_sum = 0.0f;
  for(int i = 0; i < BDLEN; ++i) {
    for(int j = 0; j < BDLEN; ++j) {
      if(rotated[i][j] == EMPTY) {
        float_t tmp = exp(policy_2d[i][j] - max_val);
        exp_sum += tmp;
        policy_2d[i][j] = tmp;
      }
      else {
        policy_2d[i][j] = 0.0f;
      }
    }
  }
  for(int i = 0; i < BDSIZE; ++i) {
    policy[i] /= exp_sum;
  }

  switch (rn) {
    case 1: rotate270<float_t>(policy_2d); break;
    case 2: rotate180<float_t>(policy_2d); break;
    case 3: rotate90<float_t>(policy_2d); break;
    default: break;
  }
}

static int16_t backpropagate(Tree tree, int32_t now, float_t value) {
  int16_t action;
  while(now != 0) {
    uint32_t parent = tree[now].parent;
    action = tree[now].action;
    tree[parent].visit[action] += 1;
    tree[parent].value[action] += value;
    now = parent;
    value = -value;
  }
  return action;
}

static void expand(
  Tree tree, 
  model_t& model,
  board_t board,
  int16_t action,
  int32_t now, 
  int32_t idx, 
  int16_t turn,
  int16_t step,
  float_t& value) {
  
  if(now != idx) {
    tree[now].child[action] = idx;
    tree[idx].action = action;
    tree[idx].parent = now;

    int8_t player = ((step + turn) & 1) == 0 ? BLACK : WHITE;
    pos_t pos = {(int8_t)(action / BDLEN + 1), (int8_t)(action % BDLEN + 1)};
    int8_t result;
    switch(player) {
      case WHITE: result = is_black_terminated(board, pos); break;
      case BLACK: result = is_white_terminated(board, pos); break;
    }
    if(result == CONT && (step + turn) == BDSIZE) {
      result = DRAW;
    }
    tree[idx].result = result;  
  }
  switch (tree[idx].result) {
    case CONT: {
      int8_t player = ((step + turn) & 1) == 0 ? BLACK : WHITE;
      predict(board, player, model);
      memcpy(tree[idx].prob, model->typed_output_tensor<float_t>(0), BDSIZE * sizeof(float_t));
      value = -(*(model->typed_output_tensor<float_t>(1)));
      break;
    }
    case WIN: value = 1.0f; break;
    case LOSE: value = -1.0f; break;
    case DRAW: value = 0.0f; break;
  }
}

static int16_t get_action(Tree tree, int32_t now, int32_t visit) {
  float_t value[BDSIZE];
  for(int i = 0; i < BDSIZE; ++i) {
    if(tree[now].prob[i] == 0.0f) {
      value[i] = -INFINITY;
      continue;
    }
    int32_t visit = tree[now].visit[i];
    if(visit == 0) {
      value[i] = 0.0f;
    }
    else {
      value[i] = tree[now].value[i] / visit;
    }
  }
  float_t max_val = -INFINITY;
  int16_t action = -1;
  for(int i = 0; i < BDSIZE; ++i) {
    float_t ucb = EXPLORE * tree[now].prob[i] * sqrt(visit) / (float_t)(1 + tree[now].visit[i]);
    float_t tmp = value[i] + ucb;
    if(max_val < tmp) {
      max_val = tmp;
      action = i;
    }
  }
  return action;
}

static void simulate(
  board_t board, 
  Tree tree, 
  int16_t turn, 
  int32_t visit,
  int32_t& now,
  int16_t& action,
  int16_t& step) {

  while(tree[now].result == CONT) {
    action = get_action(tree, now, visit);
    int8_t player = ((step + turn) & 1) == 0 ? BLACK : WHITE;
    board[action / BDLEN + 1][action % BDLEN + 1] = player;
    step++;

    visit = tree[now].visit[action];
    int32_t child = tree[now].child[action];
    if(child == 0) break;
    now = child;
  }
}

SearchOutput search_move(const board_t board0, model_t& model, int16_t turn) {
  auto tree = new Tree;
  memset(tree, 0, sizeof(Tree));
  int8_t player = (turn & 1) == 0 ? BLACK : WHITE;
  predict(board0, player, model);
  memcpy(tree[0].prob, model->typed_output_tensor<float_t>(0), BDSIZE * sizeof(float_t));
  board_t board;
  
  int32_t max_visit[2] = {0,0};
  int16_t max_action[2];
  for(int32_t idx = 1; idx <= SIMNUM; ++idx) {
    int16_t action = 0;
    int32_t now = 0;
    int16_t step = 0;
    float_t value = 0.0f;
    memcpy(board, board0, (BDLEN + 2) * (BDLEN + 2) * sizeof(int8_t));
    
    simulate(board, tree, turn, idx, now, action, step);
    int32_t leaf = idx;
    if(tree[now].result != CONT) leaf = now;
    expand(tree, model, board, action, now, leaf, turn, step, value);

    int16_t tmp = backpropagate(tree, leaf, value);
    int32_t visit = tree[0].visit[tmp];
    if(max_visit[1] < visit) {
      if(max_visit[0] < visit) {
        max_visit[1] = max_visit[0];
        max_action[1] = max_action[0];
        max_visit[0] = visit;
        max_action[0] = tmp;
      }
      else {
        max_visit[1] = visit;
        max_action[1] = tmp;
      }
    }
    if(max_visit[0] - max_visit[1] > SIMNUM - idx) break;
  }
  
  SearchOutput ret = {
    tree[tree[0].child[max_action[0]]].result, 
    max_action[0], 
    tree[0].value[max_action[0]] / max_visit[0]};
  delete[] tree;
  return ret;
}