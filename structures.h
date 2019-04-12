#define WINDOW_WIDTH (800)
#define WINDOW_HEIGHT (600)

typedef struct game_object
{
  int speed;
  float xvel;
  float yvel;
  SDL_Rect hitbox;
  SDL_Texture *texture;
} object;

typedef enum gamestate
{
  INGAME,
  MENU,
  GAMEOVER,
  PAUSED
} gamestate;
