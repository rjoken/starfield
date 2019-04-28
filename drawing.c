#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "structures.h"
#include "drawing.h"
#include "logic.h"
#include "objects.h"

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Surface *player_surface;

SDL_Texture *pellet_texture;
SDL_Surface *pellet_surface;

SDL_Texture *score_texture;
SDL_Surface *score_surface;

SDL_Texture *life_texture;
SDL_Surface *life_surface;

TTF_Font *fixedsys;

object player;
node *head_pellet;
node *head_score;
node *head_life;

bool up;
bool down;
bool left;
bool right;
bool pause;
bool enter;

bool menu_buttons[2];
char gameover_score[19];

const SDL_Color color_black = {0, 0, 0};
const SDL_Color color_white = {255, 255, 255};
const SDL_Color color_red = {255, 0, 0};
const SDL_Color color_green = {0, 255, 0};
const SDL_Color color_blue = {0, 0, 255};
const SDL_Color color_cyan = {0, 255, 255};
const SDL_Color color_grey = {128, 128, 128};

extern gamestate current_gamestate;
extern bool pause_pressed;
extern int score;
extern char score_string[13];
extern char lives_string[11];
extern char level_string[3];
extern char object_count[12];

void quit(char *msg)
{
  printf("%s", msg);
  destroyAll(head_pellet);
  destroyAll(head_score);
  destroyAll(head_life);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  exit(0);
}

int init(void)
{
  if(SDL_Init(SDL_INIT_VIDEO) != 0) //returns 0 on failure
  {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return 0;
  }
  
  //Uint32 render_flags = SDL_RENDERER_ACCELERATED;

  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

  if(!window) //window returns 0, failed to create
  {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    return 0;
  }

  if(!renderer)
  {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    return 0;
  }
  
  SDL_SetWindowTitle(window, "Starfield");

  TTF_Init();
  fixedsys = TTF_OpenFont("resources/fnt/fixedsys.ttf", 16);

  if(!fixedsys)
  {
    quit("Error loading font.\n");
  }

  SDL_SetRenderDrawColor(renderer, color_black.r, color_black.g, color_black.b, color_black.a);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  current_gamestate = MENU;

  up = false;
  down = false;
  left = false;
  right = false;
  pause = false;
  enter = false;
  menu_buttons[0] = true;
  menu_buttons[1] = false;

  render();

  return 1;
}

void initGame(void)
{
  //load player image
  player_surface = IMG_Load("resources/img/player.png");
  //load pellet image
  pellet_surface = IMG_Load("resources/img/pellet.png");
  //load score image
  score_surface = IMG_Load("resources/img/score.png");
  //load life image
  life_surface = IMG_Load("resources/img/life.png");
  //check surfaces loaded correctly
  if(!player_surface)
  {
    quit("Error creating player surface\n");
  }
  if(!pellet_surface)
  {
    quit("Error creating pellet surface\n");
  }
  if(!score_surface)
  {
    quit("Error creating score surface\n");
  }
  if(!life_surface)
  {
    quit("Error creating life surface\n");
  }

  //create textures from surfaces
  player.texture = SDL_CreateTextureFromSurface(renderer, player_surface);
  pellet_texture = SDL_CreateTextureFromSurface(renderer, pellet_surface);
  score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
  life_texture = SDL_CreateTextureFromSurface(renderer, life_surface);

  //free all surfaces after textures are made
  SDL_FreeSurface(player_surface);
  SDL_FreeSurface(pellet_surface);
  SDL_FreeSurface(score_surface);
  SDL_FreeSurface(life_surface);

  if(!player.texture)
  {
    quit("Error creating player texture");
  }
  if(!pellet_texture)
  {
    quit("Error creating pellet texture");
  }
  if(!score_texture)
  {
    quit("Error creating score texture");
  }
  if(!life_texture)
  {
    quit("Error creating life texture");
  }

  //get width and height of player texture and store information in hitbox rect
  SDL_QueryTexture(player.texture, NULL, NULL, &player.hitbox.w, &player.hitbox.h);
  //place player in centre-bottom of screen
  player.hitbox.x = (WINDOW_WIDTH - player.hitbox.w) / 2;
  player.hitbox.y = (WINDOW_HEIGHT - player.hitbox.h) * 0.9;
  player.speed = 10;

  //create objects
  createObjects(&head_pellet, pellet_texture);
  createObjects(&head_score, score_texture);
  createObjects(&head_life, life_texture);

  current_gamestate = INGAME;
}

void render(void)
{
  SDL_RenderClear(renderer);
  switch(current_gamestate)
  {
    case MENU:
      drawString("STARFIELD", fixedsys, color_white, WINDOW_WIDTH/2 - (48*4), 8, 48*8, 72);
      if(menu_buttons[0] && !menu_buttons[1])
      {
        drawString("PLAY", fixedsys, color_cyan, WINDOW_WIDTH/2 - (24*2), WINDOW_HEIGHT/2 - 24, 24*4, 48);
        drawString("QUIT", fixedsys, color_grey, WINDOW_WIDTH/2 - (24*2), WINDOW_HEIGHT/2 + 24, 24*4, 48);
      }
      else if(!menu_buttons[0] && menu_buttons[1])
      {
        drawString("PLAY", fixedsys, color_grey, WINDOW_WIDTH/2 - (24*2), WINDOW_HEIGHT/2 - 24, 24*4, 48);
        drawString("QUIT", fixedsys, color_cyan, WINDOW_WIDTH/2 - (24*2), WINDOW_HEIGHT/2 + 24, 24*4, 48);
      }
      break;
    case GAMEOVER:

      sprintf(gameover_score, "YOUR SCORE: %06d", score % 1000000);
      drawString("GAME OVER", fixedsys, color_red, WINDOW_WIDTH/2 - (48*4.5), 9, 48*9, 72);
      drawString(gameover_score, fixedsys, color_white, WINDOW_WIDTH/2 - (24*9.5), WINDOW_HEIGHT * 0.4, 24*18, 48);
      if(menu_buttons[0] && !menu_buttons[1])
      {
        drawString("RETRY", fixedsys, color_cyan, WINDOW_WIDTH/2 - (24*2.5), WINDOW_HEIGHT/2 - 24, 24*5, 48);
        drawString("QUIT", fixedsys, color_grey, WINDOW_WIDTH/2 - (24*2), WINDOW_HEIGHT/2 + 24, 24*4, 48);
      }
      else if(!menu_buttons[0] && menu_buttons[1])
      {
        drawString("RETRY", fixedsys, color_grey, WINDOW_WIDTH/2 - (24*2.5), WINDOW_HEIGHT/2 - 24, 24*5, 48);
        drawString("QUIT", fixedsys, color_cyan, WINDOW_WIDTH/2 - (24*2), WINDOW_HEIGHT/2 + 24, 24*4, 48);
      }
      break;
    case INGAME:
      drawObject(player);
      drawObjects(head_pellet);
      drawObjects(head_score);
      drawObjects(head_life);
      drawString(score_string, fixedsys, color_white, 2, 2, 24*13, 48);
      drawString(lives_string, fixedsys, color_white, WINDOW_WIDTH - 24*10, 2, 24*10, 48);
      drawString(level_string, fixedsys, color_white, WINDOW_WIDTH - 24*3, 50, 24*3, 48);
      drawString(object_count, fixedsys, color_white, 2, WINDOW_HEIGHT - 48, 24*11, 48);
      break;
    case PAUSED:
      drawObject(player);
      drawObjects(head_pellet);
      drawObjects(head_score);
      drawObjects(head_life);
      drawString(score_string, fixedsys, color_grey, 2, 2, 24*13, 48);
      drawString(lives_string, fixedsys, color_grey, WINDOW_WIDTH - 24*10, 2, 24*10, 48);
      drawString(level_string, fixedsys, color_grey, WINDOW_WIDTH - 24*3, 50, 24*3, 48);
      drawString("PAUSED", fixedsys, color_white, WINDOW_WIDTH/2 - (24*3), WINDOW_HEIGHT/2 - 24, 24*6, 48);
      break;
  }
  SDL_RenderPresent(renderer);
  return;
}

void drawObject(object o)
{
  SDL_RenderCopy(renderer, o.texture, NULL, &o.hitbox);
}

void drawString(char *s, TTF_Font *f, SDL_Color c, int x, int y, int w, int h)
{
  SDL_Surface *text_surface = TTF_RenderText_Solid(f, s, c);
  SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  SDL_FreeSurface(text_surface);
  SDL_Rect container = {.x = x, .y = y, .w = w, .h = h};

  SDL_RenderCopy(renderer, text_texture, NULL, &container);
}
