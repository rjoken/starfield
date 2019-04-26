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


int main(int argc, char *argv[])
{
  int delay = 1000/60;
  int close_requested = 0;
  srand(time(NULL));
  if(init() == 0)
  {
    SDL_Quit();
    return 1;
  }
  else
  {
    //no errors in initialisation
    while(close_requested == 0)
    {
      SDL_Event event;
      SDL_PollEvent(&event);

      close_requested = handleEvent(event);
      update();
      render();

      SDL_Delay(delay);
    }
  }
  return 1;
}
