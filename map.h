struct map_t {
   struct map_t *nxt;
   char *name;
   char *value;
};

struct map_t* map_create();
void  map_set(struct map_t* m, const char* name, const char* value);
char* map_get(struct map_t* m, const char* name);