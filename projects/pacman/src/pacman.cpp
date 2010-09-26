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
  int width;
  int height;

  char* map;
  const char* initial_map;

  Character pacman;
  Character enemy[13];
  int num_enemy;

  Stage(int t, int w, int h, char* m, const char* im)
      : time(t), width(w), height(h), map(m), initial_map(im), num_enemy(0) {}

  void InitStage() {
    memcpy(map, initial_map, width * height);
    char* m = map;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x, ++m) {
        // Does nothing if it's a wall / power ball / road
        switch (*m) {
          case '#':
          case '.':
          case ' ':
            continue;
          default:
            break;
        }

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

  void PrintStage() {
    char* m = map;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x, ++m) {
        char o = *m;

        // Checks character positions
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

  void MoveEnemy() {

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

  s1.PrintStage();
  return 0;
}
