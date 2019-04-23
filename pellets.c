#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "structures.h"
#include "drawing.h"
#include "logic.h"
#include "pellets.h"

void drawPellets(node *head)
{
  //draws all pellets in the list to the screen
  node *current = head;
  while(current->next != NULL)
  {
    drawObject(current->obj);
    current = current->next;
  }
}

void addPellet(node *head, object new)
{
  //adds a new pellet to the list
  node *current = head;
  while(current->next != NULL)
  {
    current = current->next;
  }
  current->next = malloc(sizeof(node));

  current->next->obj = new;
  current->next->next = NULL;
}

void popPellet(node **head)
{
  printf("pop\n");
  //removes the first pellet from the list
  node *next_node = NULL;
  if(*head == NULL)
  {
    return;
  }

  next_node = (*head)->next;
  free(*head);
  *head = next_node;
}

void destroyPellet(node **head, int index)
{
  //destroys a pellet based on index
  node *current = *head;
  node *temp = NULL;

  if(index == 0)
  {
    /*
    temp = (*head)->next;
    free(*head);
    *head = temp;*/
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
  printf("destroyed %d\n", index);
}

void checkCollisionWithPlayer(node *head, object collider)
{
  //NOT IMPLEMENTED

  node *current = head;
  object node_obj;
  for(int i = 0; current->next != NULL; i++)
  {
    node_obj = current->obj;
    if((SDL_HasIntersection(&node_obj.hitbox, &collider.hitbox)) == SDL_TRUE)
    {
      printf("intersect with player\n");
      destroyPellet(&head, i);
      killPlayer();
      return;
    }
    current = current->next;
  }
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
      destroyPellet(&head, i);
    }
    current = current->next;
  }

}

void updatePelletPos(node *head)
{
  node *current = head;
  object node_obj;
  while(current != NULL)
  {
    object *current_obj = &current->obj;
    node_obj = current->obj;
    node_obj.yvel = node_obj.speed;
    node_obj.hitbox.x += node_obj.xvel;
    node_obj.hitbox.y += node_obj.yvel;
    *current_obj = node_obj;
    current = current->next;
  }
}
