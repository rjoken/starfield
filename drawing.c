#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "structures.h"
#include "drawing.h"
#include "logic.h"
#include "pellets.h"

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Surface *player_surface;
SDL_Surface *pellet_surface;

SDL_Texture *pellet_texture;

TTF_Font *fixedsys;
SDL_Color cWhite;
SDL_Color cBlack;

object player;
node *head_pellet;

int up;
int down;
int left;
int right;
int pause;

extern gamestate currentGamestate;
extern int pausepressed;
extern char deathString[10];

void crash(char *msg)
{
  printf("%s", msg);
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

  window = SDL_CreateWindow("Starfield",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  if(!window) //window returns 0, failed to create
  {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    return 0;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED;
  renderer = SDL_CreateRenderer(window, -1, render_flags);

  if(!renderer)
  {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    return 0;
  }

  TTF_Init();
  fixedsys = TTF_OpenFont("resources/fnt/fixedsys.ttf", 16);
  cWhite.r = 255;
  cWhite.g = 255;
  cWhite.b = 255;
  cWhite.a = 0;

  if(!fixedsys)
  {
    crash("Error loading font.\n");
  }

  SDL_SetRenderDrawColor(renderer, cBlack.r, cBlack.g, cBlack.b, cBlack.a);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  //load player image
  player_surface = IMG_Load("resources/img/player.png");
  //load pellet image
  pellet_surface = IMG_Load("resources/img/pellet.png");
  //check surfaces loaded correctly
  if(!player_surface)
  {
    crash("Error creating player surface\n");
  }
  if(!pellet_surface)
  {
    crash("Error creating player surface\n");
  }

  //create textures from surfaces
  player.texture = SDL_CreateTextureFromSurface(renderer, player_surface);
  pellet_texture = SDL_CreateTextureFromSurface(renderer, pellet_surface);

  //free all surfaces after textures are made
  SDL_FreeSurface(player_surface);
  SDL_FreeSurface(pellet_surface);

  if(!player.texture)
  {
    crash("Error creating player texture");
  }
  if(!pellet_texture)
  {
    crash("Error creating pellet texture");
  }

  //get width and height of player texture and store information in hitbox rect
  SDL_QueryTexture(player.texture, NULL, NULL, &player.hitbox.w, &player.hitbox.h);
  //place player in centre-bottom of screen
  player.hitbox.x = (WINDOW_WIDTH - player.hitbox.w) / 2;
  player.hitbox.y = (WINDOW_HEIGHT - player.hitbox.h) * 0.9;
  player.speed = 10;

  //create pellets
  srand(time(NULL));
  head_pellet = malloc(sizeof(node));
  SDL_Rect hitbox = {.x = rand() % WINDOW_WIDTH, .y = 0, .w = 16, .h = 16};
  object new_obj = {10, 0.0f, 0.0f, hitbox, pellet_texture};
  if(head_pellet == NULL)
  {
    crash("Error: could not initialise pellet list\n");
  }

  head_pellet->obj = new_obj;
  head_pellet->next = NULL;

  currentGamestate = INGAME;

  up = 0;
  down = 0;
  left = 0;
  right = 0;
  pause = 0;

  render();

  return 1;
}

void render(void)
{
  SDL_RenderClear(renderer);
  switch(currentGamestate)
  {
    case INGAME:
      drawObject(player);
      drawPellets(head_pellet);
      drawString(deathString, fixedsys, cWhite, 2, 2, 160, 32);
      break;
    case PAUSED:
      drawObject(player);
      drawPellets(head_pellet);
      drawString("PAUSED", fixedsys, cWhite, WINDOW_WIDTH/2 - 48, WINDOW_HEIGHT/2 - 8, 96, 32);
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
  SDL_Surface *textSurface = TTF_RenderText_Solid(f, s, c);
  SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_FreeSurface(textSurface);
  SDL_Rect container;
  container.x = x;
  container.y = y;
  container.w = w;
  container.h = h;

  SDL_RenderCopy(renderer, textTexture, NULL, &container);
}
