#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "map.h"

int streql(const char *s1, const char *s2) {
  int s1_len = strlen(s1);
  int s2_len = strlen(s2);
  
  if (s1_len != s2_len) return 0;

  int res = 1;
  int i;
  for (i=0; i < s1_len; i++) {
    if (s1[i] != s2[i]) {
      res = 0;
      break;
    }
  }
  return res;
}

struct map_t* map_create() {
   struct map_t *m;
   m=(struct map_t *)malloc(sizeof(struct map_t));
   m->name=NULL;
   m->value=NULL;
   m->nxt=NULL;
   return m;
}


void map_set(struct map_t *m,const char *name,const char *value) {
   struct map_t *map;

   if(m->name==NULL) {
      m->name=(char *)malloc(strlen(name)+1);
      strcpy(m->name,name);
      m->value=(char *)malloc(strlen(value)+1);
      strcpy(m->value,value);
      m->nxt=NULL;
      return;
   }
   for(map=m; map!=NULL; map=map->nxt) {
      //if(!stricmp(name,map->name)) {
      if (streql(name,map->name)) {
         if(map->value!=NULL) {
            free(map->value);
            map->value=(char *)malloc(strlen(value)+1);
            strcpy(map->value,value);
            return;
         }
      }
      if(map->nxt==NULL) {
         map->nxt=(struct map_t *)malloc(sizeof(struct map_t));
         map=map->nxt;
         map->name=(char *)malloc(strlen(name)+1);
         strcpy(map->name,name);
         map->value=(char *)malloc(strlen(value)+1);
         strcpy(map->value,value);
         map->nxt=NULL;
         return;
      } 
   }
}

char* map_get(struct map_t* m,const char* name) {
   struct map_t* map;
   for(map=m; map!=NULL && map->name!=NULL; map=map->nxt) {
      //if(!stricmp(name,map->name)) {
      if(streql(name,map->name)) {
         return map->value;
      }
   }
   return NULL;
}
