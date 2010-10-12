//============================================================================
// Name        : pacman.cpp
// Author      : Masaaki Honda
// Version     :
// Copyright   : 2010 Masaaki Honda
// Description : Pacman solver
//============================================================================

#include <stdio.h>
#include <string.h>

struct Character {
  char type;
  int x;
  int y;
  int prev_x;
  int prev_y;
};

struct Stage {
  int time;
  int time_limit;
  int width;
  int height;

  char* map;
  const char* initial_map;

  Character pacman;
  Character enemy[13];
  int num_enemy;
  int num_dot;

  int score;

  Stage(int t, int w, int h, char* m, const char* im)
      : time(0), time_limit(t), width(w), height(h), map(m), initial_map(im),
        num_enemy(0), score(0) {}

  void InitStage() {
    memcpy(map, initial_map, width * height);
    char* m = map;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x, ++m) {
        // Does nothing if it's a wall, dot or road.
        switch (*m) {
        case '.':
          num_dot++;
        case '#':
        case ' ':
          continue;
        default:
          break;
        }

        // Keeps a character position.
        Character* c = (*m == '@' ? &pacman : &enemy[num_enemy++]);
        c->x = x;
        c->y = y;
        c->prev_x = x;
        c->prev_y = y;
        c->type = *m;
        *m = ' ';
      }
    }
  }

  void CopyStage(const Stage* src) {
    char* m = map;
    memcpy(this, src, sizeof(Stage));

    map = m;
    memcpy(map, src->map, width * height);
  }

  char GetObject(int x, int y) {
    return map[x + y * width];
  }

  void SetObject(int x, int y, char c) {
    map[x + y * width] = c;
  }
  bool IsWall(int x, int y) {
    return GetObject(x, y) == '#';
  }

  void PrintStage() {
    char* m = map;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x, ++m) {
        char o = *m;

        // Checks character positions.
        for (int i = 0; i < 1 + num_enemy; ++i) {
          Character* c = (i == 0 ? &pacman : &enemy[i - 1]);
          if (x == c->x && y == c->y) {
            o = c->type;
            break;
          }
        }

        printf("%c", o);
      }
      printf("\n");
    }
  }

  // Moves enemy to down if it's empty, otherwise left, up or right if it's empty.
  void MoveEnemyDownLeftUpRight(int* x, int* y) {
    if (!IsWall(*x, *y + 1)) {
      // Moves to down.
      (*y)++;
    } else if (!IsWall(*x - 1, *y)){
      // Moves to left.
      (*x)--;
    } else if (!IsWall(*x, *y - 1)){
      // Moves to up.
      (*y)--;
    } else if (!IsWall(*x + 1, *y)){
      // Moves right.
      (*x)++;
    }
  }

  void MoveVHEnemy(char e_type, int* x, int* y) {
    int dx = pacman.x - *x;
    int dy = pacman.y - *y;

    // Nomarilizes dx, dy.
    if (dx != 0) dx = (dx > 0 ? 1 : -1);
    if (dy != 0) dy = (dy > 0 ? 1 : -1);

    // Checks the vertical direction.
    if (e_type == 'V' && dy != 0 && !IsWall(*x, *y + dy)) {
      *y += dy;
      return;
    }
    // Checks the horizontal direction.
    if (dx != 0 && !IsWall(*x + dx, *y)) {
      *x += dx;
      return;
    }
    // Checks the vertical direction.
    if (e_type == 'H' && dy != 0 && !IsWall(*x, *y + dy)) {
      *y += dy;
      return;
    }
    MoveEnemyDownLeftUpRight(x, y);
  }

  void MoveLREnemy(char e_type, int prev_x, int prev_y, int* x, int* y) {
    // The direction to the previous position
    int dx = prev_x - *x;
    int dy = prev_y - *y;
    // Rotation matrix for 'L' is:
    // |  0 -1 |
    // |  1  0 |
    // rotate: (dx, dy) <= (-dy, dx)
    //
    // Rotation matrix for 'R' is:
    // |  0  1 |
    // | -1  0 |
    // rotate: (dx, dy) <= (dy, -dx)
    //
    // ex) moving to the right (dx, dy) = (-1, 0)
    //    (current pos to previous pos direction)
    //     next (dx, dy) => (0, -1) => (1, 0) => (0,  1) for 'L'
    //                      (0,  1) => (1, 0) => (0, -1) for 'R'
    for (int i = 0; i < 3; ++i) {
      int pdx = dx;
      dx = (e_type == 'L' ? -dy : dy);
      dy = (e_type == 'L' ? pdx : -pdx);
      if (!IsWall(*x + dx, *y + dy)) {
        *x += dx;
        *y += dy;
        return;
      }
    }
  }

  void MoveEnemy() {
    for (int i = 0; i < num_enemy; ++i) {
      Character* c = &enemy[i];

      // Enemy's position
      int x = c->x;
      int y = c->y;

      // Initial move (time = 0)
      if (time == 0) {
        MoveEnemyDownLeftUpRight(&x, &y);
      } else {
        // Checks if intersection or not.
        int num_dir =
            (IsWall(x + 1, y) ? 0 : 1) +
            (IsWall(x - 1, y) ? 0 : 1) +
            (IsWall(x, y + 1) ? 0 : 1) +
            (IsWall(x, y - 1) ? 0 : 1);

        if (num_dir == 1) {
          // Moves back.
          x = c->prev_x;
          y = c->prev_y;
        } else {
          // Enemy type
          char e_type = c->type;

          if (num_dir == 2) {
            // Moves to a new position.
            // Looks for a possible direction like enemy 'L'.
            e_type = 'L';
          } else if(num_dir > 2) {
            // Enemy 'J' behaves as if 'L' at the 2N-th intersection,
            // 'R' at the (2N+1)-th intersection.
            if (e_type == 'J') {
              e_type = 'L';
              // Changes the type to 'j' for the next intersection.
              c->type = 'j';
            } else if(e_type == 'j') {
              e_type = 'R';
              // Changes the type to 'J' for the next intersection.
              c->type = 'J';
            }
          }
          if (e_type == 'V' || e_type == 'H') {
            MoveVHEnemy(e_type, &x, &y);
          } else {
            // Enemy type should be 'L' or 'R'
            MoveLREnemy(e_type, c->prev_x, c->prev_y, &x, &y);
          }
        }
      }
      // Updates the enemy position.
      c->prev_x = c->x;
      c->prev_y = c->y;
      c->x = x;
      c->y = y;
    }
  }

  // Returns true if Pacman is live. Otherwise returns false.
  bool IsPackmanLive() {
    for (int i = 0; i < num_enemy; ++i) {
      Character* c = &enemy[i];

      // Pacman dies if Pacman and an enemy are at the same position.
      if (c->x == pacman.x && c->y == pacman.y) {
        return false;
      }

      // Pacman dies if Pacman and an enemy positions were swapped.
      if (c->x == pacman.prev_x && c->y == pacman.prev_y &&
          pacman.x == c->prev_x && pacman.y == c->prev_y) {
        return false;
      }
    }
    return true;
  }

  bool CanPacmanMove(char command) {
    return
        (command == '.') ||
        (command == 'h' && !IsWall(pacman.x - 1, pacman.y)) ||
        (command == 'l' && !IsWall(pacman.x + 1, pacman.y)) ||
        (command == 'k' && !IsWall(pacman.x, pacman.y - 1)) ||
        (command == 'j' && !IsWall(pacman.x, pacman.y + 1));
  }

  void MovePackman(char command) {
    pacman.prev_x = pacman.x;
    pacman.prev_y = pacman.y;
    switch (command) {
    case 'h':
      pacman.x--;
      break;
    case 'l':
      pacman.x++;
      break;
    case 'k':
      pacman.y--;
      break;
    case 'j':
      pacman.y++;
      break;
    }
  }

  // Returns true if Pacman is live. Otherwise returns false.
  bool MovePacmanAndEnemy(char command) {
    MoveEnemy();

    if (!CanPacmanMove(command))
      return false;

    MovePackman(command);

    if (!IsPackmanLive())
      return false;

    // Picks a dot at Pacman.
    if (GetObject(pacman.x, pacman.y) == '.') {
      SetObject(pacman.x, pacman.y, ' ');
      score++;
    }

    time++;
    return true;
  }
};

struct Stage1 : public Stage {
  static const char initial_map_[];
  char map_[11 * 7];

  Stage1() : Stage(50, 11, 7, map_, initial_map_) {}
};
const char Stage1::initial_map_[] =
    "###########"
    "#.V..#..H.#"
    "#.##...##.#"
    "#L#..#..R.#"
    "#.#.###.#.#"
    "#....@....#"
    "###########";

struct Stage2 : public Stage {
  static const char initial_map_[];
  char map_[20 * 17];

  Stage2() : Stage(300, 20, 17, map_, initial_map_) {}
};
const char Stage2::initial_map_[] =
    "####################"
    "###.....L..........#"
    "###.##.##.##L##.##.#"
    "###.##.##.##.##.##.#"
    "#.L................#"
    "#.##.##.##.##.##.###"
    "#.##.##L##.##.##.###"
    "#.................L#"
    "#.#.#.#J####J#.#.#.#"
    "#L.................#"
    "###.##.##.##.##.##.#"
    "###.##.##R##.##.##.#"
    "#................R.#"
    "#.##.##.##.##R##.###"
    "#.##.##.##.##.##.###"
    "#@....R..........###"
    "####################";

struct Stage3 : public Stage {
  static const char initial_map_[];
  char map_[58 * 17];

  Stage3() : Stage(700, 58, 17, map_, initial_map_) {}
};
const char Stage3::initial_map_[] =
    "##########################################################"
    "#........................................................#"
    "#.###.#########.###############.########.###.#####.#####.#"
    "#.###.#########.###############.########.###.#####.#####.#"
    "#.....#########....J.............J.......###.............#"
    "#####.###.......#######.#######.########.###.#######.#####"
    "#####.###.#####J#######.#######.########.###.##   ##.#####"
    "#####.###L#####.##   ##L##   ##.##    ##.###.##   ##.#####"
    "#####.###..H###.##   ##.##   ##.########.###.#######J#####"
    "#####.#########.##   ##L##   ##.########.###.###V....#####"
    "#####.#########.#######.#######..........###.#######.#####"
    "#####.#########.#######.#######.########.###.#######.#####"
    "#.....................L.........########..........R......#"
    "#L####.##########.##.##########....##....#########.#####.#"
    "#.####.##########.##.##########.##.##.##.#########.#####.#"
    "#.................##............##..@.##...............R.#"
    "##########################################################";

int Solve_sub(Stage* stage, int stage_size, int time, int depth, int min_score, char* commands) {
  static const char command[] = {'h', 'j', 'k', 'l', '.'};

  Stage* curr_stage = (Stage*)((char*)stage + time * stage_size);
  Stage* next_stage = (Stage*)((char*)curr_stage + stage_size);

  int high_score = 0;

  if (curr_stage->score + curr_stage->time_limit - curr_stage->time <= min_score)
    return high_score;

  for (size_t i = 0; i < sizeof(command) / sizeof(command[0]); ++i) {
    if (!curr_stage->CanPacmanMove(command[i]))
      continue;

    commands[time] = command[i];
    next_stage->CopyStage(curr_stage);

    int score = curr_stage->score;
    if (next_stage->MovePacmanAndEnemy(command[i]) == true) {
      score = next_stage->score;
      if (score == next_stage->num_dot) {
        // Cleared!
        score += next_stage->time_limit - next_stage->time;
        commands[time + 1] = 0;
        printf("%s - %d\n", commands, score);
      } else if (next_stage->time < depth) {
        score = Solve_sub(stage, stage_size, time + 1, depth, min_score, commands);
      } else {
        commands[time + 1] = 0;
        printf("%s - %d\n", commands, score);
      }
    }
    if (score > high_score)
      high_score = score;
    if (score > min_score)
      min_score = score;
  }

  return high_score;
}

int Solve(int stage, int depth, char* commands) {
  int score = 0;
  if (stage == 1) {
    Stage1* stage = new Stage1[51];
    stage[0].InitStage();
    score = Solve_sub(stage, sizeof(Stage1), 0, depth, 40, commands);
    delete[] stage;
  } else if (stage == 2) {
    Stage2* stage = new Stage2[301];
    stage[0].InitStage();
    score = Solve_sub(stage, sizeof(Stage2), 0, depth, 253, commands);
    delete[] stage;

  }
  return score;
}

int main(int argc, char *argv[]) {

  Stage1 s1;
  Stage2 s2;
  Stage3 s3;
  s1.InitStage();
  s2.InitStage();
  s3.InitStage();

  printf("s1 p(%d,%d) num_enemy(%d)\n", s1.pacman.x, s1.pacman.y, s1.num_enemy);
  printf("s2 p(%d,%d) num_enemy(%d)\n", s2.pacman.x, s2.pacman.y, s2.num_enemy);
  printf("s3 p(%d,%d) num_enemy(%d)\n", s3.pacman.x, s3.pacman.y, s3.num_enemy);

  {
    char commands[1000];
    int score = Solve(1, 50, commands);
    printf("stage 1 high score = %d\n", score);
  }
  {
    char commands[1000];
    int score = Solve(2, 300, commands);
    printf("stage 1 high score = %d\n", score);
  }

  return 0;
}
