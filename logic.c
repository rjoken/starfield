#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "structures.h"
#include "logic.h"
#include "pellets.h"

extern object player;
extern node *head_pellet;

extern SDL_Renderer *renderer;
extern SDL_Texture *pellet_texture;

extern int pellet_count;

extern int up;
extern int down;
extern int left;
extern int right;
extern int pause;

gamestate currentGamestate;
int deaths = 0;
char deathString[10]; //max 2 digits for deaths
int speed = 10;
int spawn_density = 10;
int pausepressed = 0;
int frames;

//per-frame event handler
int handleEvent(SDL_Event e)
{
  switch(e.type)
  {
    case SDL_QUIT:
      SDL_Quit();
      return 1;
    case SDL_KEYDOWN:
      switch(e.key.keysym.scancode)
      {
        case SDL_SCANCODE_UP:
          up = 1;
          break;
        case SDL_SCANCODE_DOWN:
          down = 1;
          break;
        case SDL_SCANCODE_LEFT:
          left = 1;
          break;
        case SDL_SCANCODE_RIGHT:
          right = 1;
          break;
        case SDL_SCANCODE_P:
          if(pausepressed == 0)
          {
            pausepressed = 1;
            pause = !pause;
          }
          break;
      }
      break;
    case SDL_KEYUP:
    switch(e.key.keysym.scancode)
    {
      case SDL_SCANCODE_UP:
        up = 0;
        break;
      case SDL_SCANCODE_DOWN:
        down = 0;
        break;
      case SDL_SCANCODE_LEFT:
        left = 0;
        break;
      case SDL_SCANCODE_RIGHT:
        right = 0;
        break;
      case SDL_SCANCODE_P:
        pausepressed = 0;
        break;
    }
    break;
  }
  return 0;
}

void update(void)
{
  frames++;
  //printf("%d\n", frames);
  sprintf(deathString, "DEATHS: %d", deaths % 100);
  //collision detection
  int xpos;
  int ypos;
  switch(currentGamestate)
  {
    case INGAME:
      xpos = player.hitbox.x;
      ypos = player.hitbox.y;
      //cannot be up and down, left and right at the same time
      player.xvel = player.yvel = 0;
      if(up && !down)
      {
        player.yvel = -player.speed;
      }
      if(down && !up)
      {
        player.yvel = player.speed;
      }
      if(left && !right)
      {
        player.xvel = -player.speed;
      }
      if(right && !left)
      {
        player.xvel = player.speed;
      }

      xpos += player.xvel;
      ypos += player.yvel;

      //reset player to boundary on wall collision
      if(xpos <= 0)
      {
        xpos = 0;
      }
      if(xpos >= WINDOW_WIDTH - player.hitbox.w)
      {
        xpos = WINDOW_WIDTH - player.hitbox.w;
      }
      if(ypos <= 0)
      {
        ypos = 0;
      }
      if(ypos >= WINDOW_HEIGHT - player.hitbox.h)
      {
        ypos = WINDOW_HEIGHT - player.hitbox.h;
      }

      player.hitbox.x = xpos;
      player.hitbox.y = ypos;

      //spawn new pellets
      if((frames % spawn_density) == 0)
      {
        srand(frames);
        SDL_Rect position;
        position.x = rand() % WINDOW_WIDTH;
        position.y = 0;
        position.w = 16;
        position.h = 16;
        object pellet = {speed, 0, 0, position, pellet_texture};
        addPellet(head_pellet, pellet);
        printf("added\n");
      }
      //pellet gravity
      updatePelletPos(head_pellet);
      checkCollisionWithY(head_pellet);
      
      checkCollisionWithPlayer(head_pellet, player);

      //check pause key
      if(pause)
      {
        currentGamestate = PAUSED;
      }
      return;
    case PAUSED:
      //unpause on pause key
      if(!pause)
      {
        currentGamestate = INGAME;
      }
      return;
  }
}

void killPlayer(void)
{
  printf("death\n");
  player.hitbox.x = (WINDOW_WIDTH - player.hitbox.w) / 2;
  player.hitbox.y = (WINDOW_HEIGHT - player.hitbox.h) * 0.9;
  deaths++;
}
