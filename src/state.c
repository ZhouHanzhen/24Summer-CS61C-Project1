#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.

  // allocate the memory for state
  game_state_t* state = malloc(sizeof(game_state_t));

  // set the rows number
  state->num_rows = 18;

  // create the board and allocate the memory for the board 
  state->board = malloc((state->num_rows) * sizeof(char*));
  for(int i = 0; i < (state->num_rows); i++) {
    state->board[i] = malloc(22 * sizeof(char));
  }
  char row1[22] = "####################\n";
  char row2[22] = "#                  #\n";
  
  strcpy(state->board[0], row1);
  strcpy(state->board[1], row2);
  
  for(int i = 2; i < 17; i++) {
    strcpy(state->board[i], state->board[1]);
  }
  strcpy(state->board[17], state->board[0]);

  // create the snakes
  state->num_snakes = 1;
  state->snakes = malloc(1 * sizeof(snake_t));
  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].head_row = 2;
  state->snakes[0].head_col = 4;
  state->snakes[0].live = true;

  // set the snake on the board
  state->board[2][2] = 'd';
  state->board[2][4] = 'D';
  state->board[2][3] = '>';
  // set the fruit on the board
  state->board[2][9] = '*';

  return state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  for(int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  for(int i = 0; i < state->num_rows; i++) {   
    fprintf(fp, "%s", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  switch(c)
  {
  case 'w':
  case 'a':
  case 's':
  case 'd':
    return true;
  default:
    return false;
  }
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  switch(c){
  case 'W':
  case 'A':
  case 'S':
  case 'D':
  case 'x':
    return true;
  default:
    return false;
  }
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  switch(c)
  {
  case 'w':
  case 'a':
  case 's':
  case 'd':
  case 'W':
  case 'A':
  case 'S':
  case 'D':
  case 'x':
  case '^':
  case '<':
  case 'v':
  case '>':
    return true;
  default:
    return false;
  }
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c){
  case '^':
    return 'w';
  case '<':
    return 'a';
  case 'v':
    return 's';
  case '>':
    return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch(c){
  case 'W':
    return '^';
  case 'A':
    return '<';
  case 'S':
    return 'v';
  case 'D':
    return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  switch(c){
  case 'v':
  case 's':
  case 'S':
    return cur_row + 1;
  case '^':
  case 'w':
  case 'W':
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  switch(c){
  case '>':
  case 'd':
  case 'D':
    return cur_col + 1;
  case '<':
  case 'a':
  case 'A':
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // get the cur_row and cur_col of the head of snake[snum] 
  snake_t snake = state->snakes[snum];
  unsigned int row = snake.head_row;
  unsigned int col = snake.head_col;

  // get the char of the head of snake[snum]
  char head = state->board[row][col];

  // get the next row and next col of the snake[snum]
  unsigned int n_row = get_next_row(row, head);
  unsigned int n_col = get_next_col(col, head);

  char square = state->board[n_row][n_col];

  return square;
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // get the cur_row and cur_col of the head of snake[snum] 
  snake_t snake = state->snakes[snum];
  unsigned int row = snake.head_row;
  unsigned int col = snake.head_col;

  // get the char of the head of snake[snum]
  char head = state->board[row][col];
  // head to body
  char body = head_to_body(head);
  set_board_at(state, row, col, body);

  // get the next row and next col of the snake[snum]'s head
  unsigned int n_row = get_next_row(row, head);
  unsigned int n_col = get_next_col(col, head);

  // update the snake's head
  state->snakes[snum].head_row = n_row;
  state->snakes[snum].head_col = n_col;

  // add a character where the snake is moving
  set_board_at(state, n_row, n_col, head);

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // get the cur_row and cur_col of the tail of snake[snum] 
  snake_t snake = state->snakes[snum];
  unsigned int row = snake.tail_row;
  unsigned int col = snake.tail_col;

  // get  the tail 
  char tail = get_board_at(state, row, col);
  // blank out the current tail
  set_board_at(state, row, col, ' ');

  // get the next row and next col of the snake[snum]'s tail
  unsigned int n_row = get_next_row(row, tail);
  unsigned int n_col = get_next_col(col, tail);
  // update the snake's tail
  state->snakes[snum].tail_row = n_row;
  state->snakes[snum].tail_col = n_col;

  // get the body and body to tail 
  char body = get_board_at(state, n_row, n_col);
  char n_tail = body_to_tail(body);
  set_board_at(state, n_row, n_col, n_tail);


  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for(unsigned int i = 0; i < state->num_snakes; i++) {
    // get the character in the cell the snake is moving into.
    char square = next_square(state, i);
    if(is_snake(square) || square == '#') {
      // the snake's head crashes into the body of a snake or a wall,
      // the snake dies and stops moving
      state->snakes[i].live = false;
      set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
    }else if(square == '*') {
      // the snake eats a fruit and grows by one unit 
      update_head(state, i);
      // genetate a new fruit
      add_food(state);
    }else {
      // the snake just move by one unit with no change
      update_head(state, i);
      update_tail(state, i);
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  unsigned int size = 1024;
  char *line = malloc(size * sizeof(char));
  // if there are any errors, or if EOF is reached, return NULL
  if((fgets(line, size, fp)) == NULL) {
    return NULL;
  }

  // reallocate the memory for str 
  // find the position of char '\n'
  char *ptr = strchr(line, '\n');
  // compute the new size of a line
  unsigned long new_size = ptr - line + 1 + 1;
  line = realloc(line, new_size);

  return line;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.

  // allocate memory for the state
  game_state_t *state = malloc(sizeof(game_state_t));
  state->num_rows = 0; 
  state->board = malloc(1024 * sizeof(char*));

  char *line = read_line(fp);
  while(line != NULL) {
    state->board[state->num_rows] = line;
    state->num_rows += 1;
    line = read_line(fp);
  }

  // state->board = realloc(state->board, state->num_rows);

  state->num_snakes = 0;
  state->snakes = NULL;

  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  char tail = get_board_at(state, state->snakes[snum].tail_row, state->snakes[snum].tail_col);
  unsigned int row = get_next_row(state->snakes[snum].tail_row, tail);
  unsigned int col = get_next_col(state->snakes[snum].tail_col, tail);
  char c = get_board_at(state, row, col);

  // find head 
  while(!(is_head(c))) {
    row = get_next_row(row, c);
    col = get_next_col(col, c);
    c = get_board_at(state, row, col);
  }

  // set head 
  state->snakes[snum].head_row = row;
  state->snakes[snum].head_col = col;

  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.

  // count snakes
  for(int i = 0 ; i < state->num_rows; i++) {
    for(int j = 0; j < strlen(state->board[i]); j++) {
      char c = state->board[i][j];
      // find tail
      if(is_tail(c)) {
        state->num_snakes += 1;
      }
    }
  }

  // allocate memory for snakes
  state->snakes = malloc((state->num_snakes) * sizeof(snake_t));

  // find tail and head's row and col
  unsigned int snum = 0;
  for(unsigned int i = 0 ; i < state->num_rows; i++) {
    for(unsigned int j = 0; j < strlen(state->board[i]); j++) {
      char c = state->board[i][j];
      // find tail
      if(is_tail(c)) {
        state->snakes[snum].tail_row = i;
        state->snakes[snum].tail_col = j;
        find_head(state, snum);
        state->snakes[snum].live = true;
        snum += 1;

      }
    }
  }

  return state;
}
