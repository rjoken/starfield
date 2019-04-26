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

void createObjects(node **head, SDL_Texture *texture)
{
  *head = malloc(sizeof(node));
  SDL_Rect hitbox = {.x = rand() % WINDOW_WIDTH, .y = 0, .w = 32, .h = 32};
  object new_obj = {10, 0.0f, 0.0f, hitbox, texture};
  if(*head == NULL)
  {
    quit("Error: could not initialise object list\n");
  }

  (*head)->obj = new_obj;
  (*head)->next = NULL;
}

void drawObjects(node *head)
{
  //draws all objects in the list to the screen
  node *current = head;
  while(current->next != NULL)
  {
    drawObject(current->obj);
    current = current->next;
  }
}

void addObject(node *head, object new)
{
  //adds a new object to the list
  node *current = head;
  while(current->next != NULL)
  {
    current = current->next;
  }
  current->next = malloc(sizeof(node));

  current->next->obj = new;
  current->next->next = NULL;
}

void destroyobject(node **head, int index)
{
  //destroys a pellet based on index
  node *current = *head;
  node *temp = NULL;

  if(index == 0)
  {
    return;
  }

  for(int i = 0; i < index-1; i++)
  {
    if(current->next == NULL)
    {
      printf("Error: failed to destroy pellet %d\n", index);
      return;
    }
    current = current->next;
  }

  temp = current->next;
  current->next = temp->next;
  free(temp);
}

bool checkCollisionWithPlayer(node *head, object collider)
{
  node *current = head;
  object node_obj;
  for(int i = 0; current->next != NULL; i++)
  {
    node_obj = current->obj;
    if((SDL_HasIntersection(&node_obj.hitbox, &collider.hitbox)) == SDL_TRUE)
    {
      printf("intersect with player\n");
      destroyobject(&head, i);
      return true;
    }
    current = current->next;
  }
  return false;
}

void checkCollisionWithY(node *head)
{
  node *current = head;
  object node_obj;
  for(int i = 0; current->next != NULL; i++)
  {
    node_obj = current->obj;
    if(node_obj.hitbox.y > WINDOW_HEIGHT)
    {
      destroyobject(&head, i);
    }
    current = current->next;
  }

}

void updateObjectPos(node *head)
{
  node *current = head;
  object node_obj;
  while(current != NULL)
  {
    object *current_obj = &current->obj;
    node_obj = current->obj;
    node_obj.yvel = node_obj.speed;
    node_obj.hitbox.y += node_obj.yvel;
    *current_obj = node_obj;
    current = current->next;
  }
}

void destroyAll(node *head)
{
  if(head == NULL)
  {
    return;
  }
  node *current = head;
  while(current->next != NULL)
  {
    node *next = current->next;
    free(current);
    current = next;
  }
}
