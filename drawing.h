int init(void);
void initGame(void);
void render(void);
void drawObject(object o);
void drawString(char *s, TTF_Font *f, SDL_Color c, int x, int y, int w, int h);
void quit(char *msg);

extern const SDL_Color color_black;
extern const SDL_Color color_white;
extern const SDL_Color color_red;
extern const SDL_Color color_green;
extern const SDL_Color color_blue;
extern const SDL_Color color_cyan;
extern const SDL_Color color_grey;
