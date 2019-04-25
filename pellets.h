//void initPellets(object *head, SDL_Texture *texture);
void drawObjects(node *head);
void addObject(node *head, object new);
void popPellet(node **head);
void destroyobject(node **head, int index);
bool checkCollisionWithPlayer(node *head, object collider);
void checkCollisionWithY(node *head);
void updateObjectPos(node *head);
void destroyAll(node *head);
