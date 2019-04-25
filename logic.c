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
#include "pellets.h"

extern object player;
extern node *head_pellet;
extern node *head_score;
extern node *head_life;

extern SDL_Renderer *renderer;
extern SDL_Texture *pellet_texture;
extern SDL_Texture *score_texture;
extern SDL_Texture *life_texture;

extern bool up;
extern bool down;
extern bool left;
extern bool right;
extern bool pause;
extern bool enter;
extern bool menu_buttons[2];

gamestate current_gamestate;

int object_speed = 10;
int spawn_density = 10;

//these bools act as chokers for menu buttons to prevent 60fps toggling
//without these, menu navigation would be near impossible
bool pause_pressed = false;
bool up_pressed = false;
bool down_pressed = false;
int frames;
int score = 0;
int lives = 5;
char score_string[14]; //max 6 digits for score
char lives_string[11]; //max 3 digits for lives

//per-frame event handler
int handleEvent(SDL_Event e)
{
  switch(e.type)
  {
    case SDL_QUIT:
      quit("Close button pressed.\n");
      return 1;
    case SDL_KEYDOWN:
      switch(e.key.keysym.scancode)
      {
        case SDL_SCANCODE_UP:
          if((current_gamestate == MENU) || (current_gamestate == GAMEOVER))
          {
            if(!up_pressed)
            {
              up_pressed = true;
              menu_buttons[0] = !menu_buttons[0];
              menu_buttons[1] = !menu_buttons[1];
            }
          }
          else
          {
            up = true;
          }
          break;
        case SDL_SCANCODE_DOWN:
          if((current_gamestate == MENU) || (current_gamestate == GAMEOVER))
          {
            if(!down_pressed)
            {
              down_pressed = true;
              menu_buttons[0] = !menu_buttons[0];
              menu_buttons[1] = !menu_buttons[1];
            }
          }
          else
          {
            down = true;
          }
          break;
        case SDL_SCANCODE_LEFT:
          left = true;
          break;
        case SDL_SCANCODE_RIGHT:
          right = true;
          break;
        case SDL_SCANCODE_P:
          if(!pause_pressed)
          {
            pause_pressed = 1;
            pause = !pause;
          }
          break;
        case SDL_SCANCODE_RETURN:
          enter = true;
          break;
      }
      break;
    case SDL_KEYUP:
    switch(e.key.keysym.scancode)
    {
      case SDL_SCANCODE_UP:
        up = false;
        if(up_pressed)
        {
          up_pressed = false;
        }
        break;
      case SDL_SCANCODE_DOWN:
        down = false;
        if(down_pressed)
        {
          down_pressed = false;
        }
        break;
      case SDL_SCANCODE_LEFT:
        left = false;
        break;
      case SDL_SCANCODE_RIGHT:
        right = false;
        break;
      case SDL_SCANCODE_P:
        pause_pressed = false;
        break;
      case SDL_SCANCODE_RETURN:
        enter = false;
        break;
    }
    break;
  }
  return 0;
}

void update(void)
{
  switch(current_gamestate)
  {
    case INGAME:
      frames++;
      sprintf(score_string, "SCORE: %06d", score % 1000000); //maximum 6 digits for score
      sprintf(lives_string, "LIVES: %03d", lives % 1000);
      if(frames % 60 == 0)
      {
        //increment score every second
        score+=1;
      }
      if((score > 999) && ((score % 1000) == 0))
      {
        //spawn density increases every 1000 score
        if(spawn_density-1 > 0)
        {
          spawn_density--;
        }
      }
      if((score > 4999) && ((score % 5000) == 0))
      {
        object_speed++;
      }

      //collision detection
      int xpos;
      xpos = player.hitbox.x;
      //cannot be up and down, left and right at the same time
      if(left && !right)
      {
        player.xvel = -player.speed;
      }
      if(right && !left)
      {
        player.xvel = player.speed;
      }
      if(!right && !left)
      {
        player.xvel = 0;
      }

      xpos += player.xvel;

      //reset player to boundary on wall collision
      if(xpos <= 0)
      {
        xpos = 0;
      }
      if(xpos >= WINDOW_WIDTH - player.hitbox.w)
      {
        xpos = WINDOW_WIDTH - player.hitbox.w;
      }

      player.hitbox.x = xpos;

      //spawn new life pickups
      if((frames % (spawn_density * 5)) == 0)
      {
        float xpos = rand() % WINDOW_WIDTH;
        SDL_Rect position = {xpos, 0, 32, 32};
        object life_obj = {object_speed, 0.0f, 0.0f, position, life_texture};
        addObject(head_life, life_obj);
      }

      //spawn new score pickups
      else if((frames % (spawn_density * 2)) == 0)
      {
        float xpos = rand() % WINDOW_WIDTH;
        SDL_Rect position = {xpos, 0, 32, 32};
        object score_obj = {object_speed, 0.0f, 0.0f, position, score_texture};
        addObject(head_score, score_obj);
      }

      //spawn new pellets
      else if((frames % spawn_density) == 0)
      {
        float xpos = rand() % WINDOW_WIDTH;
        SDL_Rect position = {xpos, 0, 32, 32};
        object pellet = {object_speed, 0.0f, 0.0f, position, pellet_texture};
        addObject(head_pellet, pellet);
      }

      //object gravity
      updateObjectPos(head_pellet);
      updateObjectPos(head_score);
      updateObjectPos(head_life);
      checkCollisionWithY(head_pellet);
      checkCollisionWithY(head_score);
      checkCollisionWithY(head_life);

      if(checkCollisionWithPlayer(head_pellet, player))
      {
        if(lives-1 < 0)
        {
          printf("death\n");
          killPlayer();
          current_gamestate = GAMEOVER;
        }
        else
        {
          killPlayer();
          lives--;
        }
      }

      if(checkCollisionWithPlayer(head_score, player))
      {
        score+=50;
      }

      if(checkCollisionWithPlayer(head_life, player))
      {
        lives++;
      }

      //check pause key
      if(pause)
      {
        current_gamestate = PAUSED;
      }
      return;
    case PAUSED:
      //unpause on pause key
      if(!pause)
      {
        current_gamestate = INGAME;
      }
      return;
    case MENU:
      if(enter)
      {
        if(menu_buttons[0])
        {
          initGame();
        }
        if(menu_buttons[1])
        {
          quit("Game quit.\n");
        }
      }
      return;
    case GAMEOVER:
      if(enter)
      {
        if(menu_buttons[0])
        {
          destroyAll(head_pellet);
          destroyAll(head_score);
          destroyAll(head_life);
          createObjects(&head_pellet, pellet_texture);
          createObjects(&head_score, score_texture);
          createObjects(&head_life, life_texture);
          score = 0;
          lives = 5;
          object_speed = 10;
          spawn_density = 10;
          current_gamestate = INGAME;
        }
        else
        {
          quit("Game quit.\n");
        }
      }
  }
}

void killPlayer(void)
{
  player.hitbox.x = (WINDOW_WIDTH - player.hitbox.w) / 2;
  player.hitbox.y = (WINDOW_HEIGHT - player.hitbox.h) * 0.9;
}
