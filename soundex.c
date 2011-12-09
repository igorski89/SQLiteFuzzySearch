#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
// #include "map.h"

// static struct map_t *soundex_cache_map = NULL;

/*
** function to determine soundex value
** soundex(str) => string
*/
static void soundex(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  
  // if (soundex_cache_map == NULL) {
  //   soundex_cache_map = map_create();
  // }
  
  const char *str = (const char*)sqlite3_value_text(argv[0]);
  const char *in = str;
  
  // char *cache_res = map_get(soundex_cache_map, str);
  // if (cache_res != NULL) {
  //   // printf("found in cache:%s => %s\n",str,cache_res);
  //   sqlite3_result_text(context, cache_res, 4, SQLITE_TRANSIENT);
  //   return;
  // }
  
  static int code[] =
    {  0,1,2,3,0,1,2,0,0,2,2,4,5,5,0,1,2,6,2,3,0,1,0,2,0,2 };
    /* a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z */
    

  char ch;
  int last;
  int count;

  char key[5];
  /* Set up default key, complete with trailing '0's */
  strcpy(key, "Z000");

  /* Advance to the first letter.  If none present, 
    return default key */
  while (*in != '\0'  &&  !isalpha(*in))
    ++in;
  if (*in == '\0') {
    sqlite3_result_text(context, key, 4, SQLITE_TRANSIENT);
    return;
  }
    

  /* Pull out the first letter, uppercase it, and 
    set up for main loop */
  key[0] = toupper(*in);
  last = code[key[0] - 'A'];
  ++in;

  /* Scan rest of string, stop at end of string or 
    when the key is full */
  for (count = 1;  count < 4  &&  *in != '\0';  ++in) {
    /* If non-alpha, ignore the character altogether */
    if (isalpha(*in)) {
       ch = tolower(*in);
       /* Fold together adjacent letters sharing the same code */
       if (last != code[ch - 'a']) {
          last = code[ch - 'a'];
          /* Ignore code==0 letters except as separators */
          if (last != 0)
             key[count++] = '0' + last;
       }
    }
  }
  
  // map_set(soundex_cache_map,str,key);
  
  sqlite3_result_text(context, key, 4, SQLITE_TRANSIENT);  
}

/* SQLite invokes this routine once when it loads the extension.
** Create new functions, collating sequences, and virtual table
** modules here.  This is usually the only exported symbol in
** the shared library.
*/
int sqlite3_extension_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi)
  sqlite3_create_function(db, "soundex", 1, SQLITE_ANY, NULL, soundex, NULL, NULL);
  
  return 0;
}
