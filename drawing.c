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

SDL_Window *window;
SDL_Renderer *renderer;

//load multiple surfaces in future
SDL_Surface *surface;

TTF_Font *fixedsys;
SDL_Color cWhite;
SDL_Color cBlack;

object player;

int up;
int down;
int left;
int right;
int pause;

extern gamestate currentGamestate;
extern int pausepressed;

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
    fprintf(stderr, "Error loading font 1.");
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    return 0;
  }

  SDL_SetRenderDrawColor(renderer, cBlack.r, cBlack.g, cBlack.b, cBlack.a);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  //load player image from slot 0
  surface = IMG_Load("resources/img/player.png");
  //check all surfaces loaded correctly
  if(!surface)
  {
    printf("Error creating surface\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
  }

  //create textures from surfaces
  player.texture = SDL_CreateTextureFromSurface(renderer, surface);

  //free all surfaces after textures are made
  SDL_FreeSurface(surface);

  if(!player.texture)
  {
    printf("Error creating player texture");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
  }

  //get width and height of player texture and store information in hitbox rect
  SDL_QueryTexture(player.texture, NULL, NULL, &player.hitbox.w, &player.hitbox.h);
  //place player in centre of screen
  player.hitbox.x = (WINDOW_WIDTH - player.hitbox.w) / 2;
  player.hitbox.y = (WINDOW_HEIGHT - player.hitbox.h) / 2;
  player.speed = 20;

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
      draw_object(player);
      draw_string("Gamestate: ingame", fixedsys, cWhite, 10, 10, 100, 16);
      printf("%d\n", pausepressed);
      break;
    case PAUSED:
      draw_object(player);
      draw_string("Gamestate: paused", fixedsys, cWhite, 10, 10, 100, 16);
      printf("%d\n", pausepressed);
      break;
  }
  SDL_RenderPresent(renderer);
  return;
}

void draw_object(object o)
{
  SDL_RenderCopy(renderer, o.texture, NULL, &o.hitbox);
}

void draw_string(char *s, TTF_Font *f, SDL_Color c, int x, int y, int w, int h)
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
