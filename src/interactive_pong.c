// Copyright 2022 mablever eboniflo thuzadca

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define X_PLACE 80
#define Y_PLACE 25
#define X_PLACE_START 0
#define Y_PLACE_START 0
#define X_START_BALL 40
#define Y_START_BALL 12
#define Y_START_PLAYERS 12
#define SCORE_WIN 21
#define SCORE_START 0
#define X_FIRST_PLAYER 5
#define X_SECOND_PLAYER 75
#define X_GRID 40

static struct termios stored_settings;
void draw_scoreboard(int first_score, int second_score);
void new_round(int *x_ball, int *y_ball, int *dy_ball, int players_place[2]);
void draw_game_place(const int player_place[2], int x_ball, int y_ball);
void set_keypress(void);

int main() {
  char code;
  fd_set rfds;
  struct timeval tv;

  set_keypress();
  printf("Input Enter if want to start the game pong");
  scanf("%c", &code);
  if (code == '\n') {
    int players_place[2] = {Y_START_PLAYERS, Y_START_PLAYERS};
    int dx_ball = 1, dy_ball = 0;
    int x_ball = X_START_BALL, y_ball = Y_START_BALL;
    int players_score[2] = {SCORE_START, SCORE_START};

    while ((players_score[0] != SCORE_WIN && players_score[1] != SCORE_WIN)) {
      printf("\e[1;1H\e[2J");
      draw_scoreboard(players_score[0], players_score[1]);
      draw_game_place(players_place, x_ball, y_ball);
      FD_ZERO(&rfds);
      FD_SET(0, &rfds);

      tv.tv_sec = 0;
      tv.tv_usec = 0;
      int retval = select(2, &rfds, NULL, NULL, &tv);
      if (retval) {
        code = getc(stdin);
      } else {
        code = ' ';
      }
      usleep(60000);

      if ((code == 'a' || code == 'A') && (players_place[0] - 1 > 0))
        players_place[0] -= 1;
      else if ((code == 'z' || code == 'Z') && (players_place[0] + 1 < 24))
        players_place[0] += 1;

      else if ((code == 'k' || code == 'K') && (players_place[1] - 1 > 0))
        players_place[1] -= 1;
      else if ((code == 'm' || code == 'M') && (players_place[1] + 1 < 24))
        players_place[1] += 1;
      x_ball += dx_ball;
      y_ball += dy_ball;

      if (code == 'q' || code == 'Q')
        exit(0);

      if (y_ball == -1 || y_ball == Y_PLACE) {
        dy_ball = -dy_ball;
        y_ball += dy_ball;
      }

      if (x_ball == X_FIRST_PLAYER) {
        dx_ball = -dx_ball;
        x_ball += dx_ball;
        if (y_ball == players_place[0]) {
          dy_ball = 0;
        } else if (y_ball == (players_place[0] - 1)) {
          dy_ball = -1;
        } else if (y_ball == (players_place[0] + 1)) {
          dy_ball = 1;
        } else {
          dx_ball = 1;
          new_round(&x_ball, &y_ball, &dy_ball, players_place);
          players_score[1]++;
        }
      }

      if (x_ball == X_SECOND_PLAYER) {
        dx_ball = -dx_ball;
        x_ball += dx_ball;
        if (y_ball == players_place[1]) {
          dy_ball = 0;
        } else if (y_ball == (players_place[1] - 1)) {
          dy_ball = -1;
        } else if (y_ball == (players_place[1] + 1)) {
          dy_ball = 1;
        } else {
          dx_ball = -1;
          new_round(&x_ball, &y_ball, &dy_ball, players_place);
          players_score[0]++;
        }
      }
    }

    printf("\e[1;1H\e[2J");
    if (players_score[0] == SCORE_WIN) {
      printf("First player win!");
    } else {
      printf("Second player win!");
    }
  }
  return 0;
}

void draw_game_place(const int players_place[2], int x_ball, int y_ball) {
  int i = Y_PLACE_START, j = X_PLACE_START;
  while (i < Y_PLACE) {
    while (j < X_PLACE) {
      if (((i == players_place[0] - 1) || (i == players_place[0]) ||
           (i == players_place[0] + 1)) &&
          j == X_FIRST_PLAYER) {
        printf("H");
      } else if (((i == (players_place[1] - 1)) || (i == players_place[1]) ||
                  (i == (players_place[1] + 1))) &&
                 j == X_SECOND_PLAYER) {
        printf("H");
      } else if (j == x_ball && i == y_ball) {
        printf("o");
      } else if (j == X_GRID) {
        printf("|");
      } else {
        printf(" ");
      }
      j++;
    }
    printf("\n");
    j = X_PLACE_START;
    i++;
  }
  return;
}

void new_round(int *x_ball, int *y_ball, int *dy_ball, int players_place[2]) {
  *x_ball = X_START_BALL;
  *y_ball = Y_START_BALL;
  *dy_ball = 0;
  players_place[0] = Y_START_PLAYERS;
  players_place[1] = Y_START_PLAYERS;
  return;
}

void draw_scoreboard(int first_score, int second_score) {
  int j = X_PLACE_START;
  while (j < X_PLACE / 2 - 3) {
    printf(" ");
    j++;
  }

  if (first_score < 10) {
    printf("0%d", first_score);
  } else {
    printf("%d", first_score);
  }

  printf(" : ");
  if (second_score < 10) {
    printf("0%d", second_score);
  } else {
    printf("%d", second_score);
  }

  printf("\n\n");
}

void set_keypress(void) {
  struct termios new_settings;

  tcgetattr(0, &stored_settings);

  new_settings = stored_settings;

  new_settings.c_lflag &= (~ICANON);
  new_settings.c_lflag &= (~ECHO);
  new_settings.c_cc[VTIME] = 0;
  new_settings.c_cc[VMIN] = 1;

  tcsetattr(0, TCSANOW, &new_settings);
  return;
}
