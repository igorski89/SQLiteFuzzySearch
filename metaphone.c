#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
// #include "map.h"

// static struct map_t *soundex_cache_map = NULL;

#define TRUE  (1)
#define FALSE (0)
#define NULLCHAR (char *) 0

static char *VOWELS="AEIOU";
static char *FRONTV="EIY";   /* special cases for letters in FRONT of these */
static char *VARSON="CSPTG"; /* variable sound--those modified by adding an "h"    */
static char *DOUBLE=".";     /* let these double letters through */

static char *excpPAIR="AGKPW"; /* exceptions "ae-", "gn-", "kn-", "pn-", "wr-" */
static char *nextLTR ="ENNNR";

/*
** function to determine metaphone value
** metaphone(str) => string
*/
static void metaphone(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  // if (soundex_cache_map == NULL) {
  //   soundex_cache_map = map_create();
  // }
  
  char *chrptr, *chrptr1;
  
  const char *name = (const char*)sqlite3_value_text(argv[0]);
  
  // char *cache_res = map_get(soundex_cache_map, name);
  // if (cache_res != NULL) {
  //   // printf("found in cache:%s => %s\n",name,cache_res);
  //   sqlite3_result_text(context, cache_res, strlen(cache_res), SQLITE_TRANSIENT);
  //   return;
  // }
  
  int  ii, jj, silent, hard, Lng, lastChr;

  char curLtr, prevLtr, nextLtr, nextLtr2, nextLtr3;

  int vowelAfter, vowelBefore, frontvAfter;

  char wname[60];
  char *ename=wname;

  jj = 0;
  for (ii=0; name[ii] != '\0'; ii++) {
      if ( isalpha(name[ii]) ) {
          ename[jj] = toupper(name[ii]);
          jj++;
      }
  }
  ename[jj] = '\0';
  
  if (strlen(ename) == 0) {
    sqlite3_result_text(context, NULL, 0, SQLITE_TRANSIENT);
    return;
  }
    
  
  /* if ae, gn, kn, pn, wr then drop the first letter */
  if ( (chrptr=strchr(excpPAIR,ename[0]) ) != NULLCHAR ) {
      chrptr1 = nextLTR + (chrptr-excpPAIR);
      if ( *chrptr1 == ename[1] )  strcpy(ename,&ename[1]);
  }
  /* change x to s */
  if  (ename[0] == 'X') ename[0] = 'S';
  /* get rid of the "h" in "wh" */
  if ( strncmp(ename,"WH",2) == 0 ) strcpy(&ename[1], &ename[2]);
  
  Lng = strlen(ename);
  lastChr = Lng -1;   /* index to last character in string makes code easier*/
  
  /* Remove an S from the end of the string */
  if ( ename[lastChr] == 'S' ) {
      ename[lastChr] = '\0';
      Lng = strlen(ename);
      lastChr = Lng -1;
  }
  
  int metalen = Lng;
  char *metaph = malloc(sizeof(char)*metalen);
  metaph[0] = '\0';
  // char *metaph = NULL;
  
  for (ii=0; ( (strlen(metaph) < metalen) && (ii < Lng) ); ii++) {
    // printf("%d: metaph = %s\n",ii,metaph);
    curLtr = ename[ii];
    
    vowelBefore = FALSE;  prevLtr = ' ';
    if (ii > 0) {
        prevLtr = ename[ii-1];
        if ( strchr(VOWELS,prevLtr) != NULLCHAR ) vowelBefore = TRUE;
    }
    /* if first letter is a vowel KEEP it */
    if (ii == 0 && (strchr(VOWELS,curLtr) != NULLCHAR) ) {
        strncat(metaph,&curLtr,1);
        continue;
    }
    
    vowelAfter = FALSE;   frontvAfter = FALSE;   nextLtr = ' ';
    if ( ii < lastChr ) {
        nextLtr = ename[ii+1];
        if ( strchr(VOWELS,nextLtr) != NULLCHAR ) vowelAfter = TRUE;
        if ( strchr(FRONTV,nextLtr) != NULLCHAR ) frontvAfter = TRUE;
    }
    /* skip double letters except ones in list */
    if (curLtr == nextLtr && (strchr(DOUBLE,nextLtr) == NULLCHAR) ) continue;
    
    nextLtr2 = ' ';
    if (ii < (lastChr-1) ) nextLtr2 = ename[ii+2];
    
    nextLtr3 = ' ';
    if (ii < (lastChr-2) ) nextLtr3 = ename[ii+3];
    
    switch (curLtr) {
            
      case 'B': 
        silent = FALSE;
        if (ii == lastChr && prevLtr == 'M') silent = TRUE;
        if (! silent) strncat(metaph,&curLtr,1);
        break;
          
          /*silent -sci-,-sce-,-scy-;  sci-, etc OK*/
      case 'C': if (! (ii > 1 && prevLtr == 'S' && frontvAfter) )          
        if ( ii > 0 && nextLtr == 'I' && nextLtr2 == 'A' )
          strncat(metaph,"X",1);
        else
          if (frontvAfter)
            strncat(metaph,"S",1);
          else
            if (ii > 1 && prevLtr == 'S' && nextLtr == 'H')
              strncat(metaph,"K",1);
            else
              if (nextLtr == 'H')
                if (ii == 0 && (strchr(VOWELS,nextLtr2) == NULLCHAR) )
                  strncat(metaph,"K",1);
                else
                  strncat(metaph,"X",1);
                else
                  if (prevLtr == 'C')
                    strncat(metaph,"C",1);
                  else
                    strncat(metaph,"K",1);
        break;
          
      case 'D': if (nextLtr == 'G' && (strchr(FRONTV,nextLtr2) != NULLCHAR))
        strncat(metaph,"J",1);
      else
        strncat(metaph,"T",1);
        break;
          
      case 'G': silent=FALSE;
        /* SILENT -gh- except for -gh and no vowel after h */
        if ( (ii < (lastChr-1) && nextLtr == 'H')
            && (strchr(VOWELS,nextLtr2) == NULLCHAR) )
          silent=TRUE;
        
        if ( (ii == (lastChr-3) )
            && nextLtr == 'N' && nextLtr2 == 'E' && nextLtr3 == 'D')
          silent=TRUE;
        else
          if ( (ii == (lastChr-1)) && nextLtr == 'N') silent=TRUE;
        
        if (prevLtr == 'D' && frontvAfter) silent=TRUE;
        
        if (prevLtr == 'G')
          hard=TRUE;
        else
          hard=FALSE;
        
        if (!silent)
          if (frontvAfter && (! hard) )
            strncat(metaph,"J",1);
          else
            strncat(metaph,"K",1);
        break;
          
      case 'H': silent = FALSE;
        if ( strchr(VARSON,prevLtr) != NULLCHAR ) silent = TRUE;
        
        if ( vowelBefore && !vowelAfter) silent = TRUE;
        
        if (!silent) strncat(metaph,&curLtr,1);
        break;
          
      case 'F': case 'J': case 'L': case 'M': case 'N': case 'R': 
        strncat(metaph,&curLtr,1); 
        break;
          
      case 'K': if (prevLtr != 'C') strncat(metaph,&curLtr,1);
          break;
          
      case 'P': 
        if (nextLtr == 'H')
          strncat(metaph,"F",1);
        else
            strncat(metaph,"P",1);
        break;
          
      case 'Q': strncat(metaph,"K",1);
          break;
          
      case 'S': 
        if (ii > 1 && nextLtr == 'I'&& ( nextLtr2 == 'O' || nextLtr2 == 'A') )
          strncat(metaph,"X",1);
        else
          if (nextLtr == 'H')
              strncat(metaph,"X",1);
          else
              strncat(metaph,"S",1);
        break;
          
      case 'T': 
        if (ii > 1 && nextLtr == 'I' && ( nextLtr2 == 'O' || nextLtr2 == 'A') )
            strncat(metaph,"X",1);
        else
          if (nextLtr == 'H')         /* The=0, Tho=T, Withrow=0 */
            if (ii > 0 || (strchr(VOWELS,nextLtr2) != NULLCHAR) )
              strncat(metaph,"0",1);
            else
              strncat(metaph,"T",1);
            else
              if (! (ii < (lastChr-2) && nextLtr == 'C' && nextLtr2 == 'H'))
                strncat(metaph,"T",1);
        break;
          
      case 'V': strncat(metaph,"F",1);
        break;
          
      case 'W': case 'Y': if (ii < lastChr && vowelAfter) strncat(metaph,&curLtr,1); 
        break;
          
      case 'X': strncat(metaph,"KS",2);
        break;
          
      case 'Z': strncat(metaph,"S",1);
        break;
    } 
  }

  // map_set(soundex_cache_map,name,metaph);
  
  sqlite3_result_text(context, metaph, strlen(metaph), SQLITE_TRANSIENT);  
}

/* SQLite invokes this routine once when it loads the extension.
** Create new functions, collating sequences, and virtual table
** modules here. This is usually the only exported symbol in
** the shared library.
*/
int sqlite3_extension_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi)
  sqlite3_create_function(db, "metaphone", 1, SQLITE_ANY, NULL, metaphone, NULL, NULL);
  
  return 0;
}