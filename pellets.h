//void initPellets(object *head, SDL_Texture *texture);
void drawPellets(node *head);
void addPellet(node *head, object new);
void popPellet(node **head);
void destroyPellet(node **head, int index);
void checkCollisionWithPlayer(node *head, object collider);
void checkCollisionWithY(node *head);
void updatePelletPos(node *head);
