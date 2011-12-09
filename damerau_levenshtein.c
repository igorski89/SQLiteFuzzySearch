#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

#include <stdlib.h>
#include <string.h>
#include <limits.h>

// helpers
int minimum(int a,int b,int c);

/*
  function to determine damerau-levenshtein distance
  damerau_levenshtein(src,dts,max_distance) => bool
*/
static void 
damerau_levenshtein(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
    
  const char *s    = (const char *)sqlite3_value_text(argv[0]);
  const char *t    = (const char *)sqlite3_value_text(argv[1]);
  int max_distance = sqlite3_value_int(argv[2]);
  
  //Step 1
  int n = strlen(s); 
  int m = strlen(t);
  if ((n == 0) || (m == 0)) {
    sqlite3_result_int(context, 0);
    return;
  }
  
  if (abs(n - m) > max_distance) {
    sqlite3_result_int(context, 0);
    return;
  }
  
  int *d = malloc(sizeof(int)*(m+1)*(n+1));
  m++;
  n++;
  
  //Step 2
  int k;
  for(k=0;k<n;k++) d[k]=k;
  for(k=0;k<m;k++) d[k*n]=k;
  
  //Step 3 and 4	
  int i, j;
  for(i=1; i<n; i++) {
    for(j=1; j<m; j++) {
      int cost;
      
      //Step 5
      //cost = s[i-1] != t[j-1];
      if (s[i-1] == t[j-1]) {
        cost = 0;
      }  
      else {
        cost = 1;
      }
        
      //Step 6			 
      d[j*n+i] = minimum(d[(j-1)*n+i]+1, d[j*n+i-1]+1, d[(j-1)*n+i-1]+cost);
      
      //Step 7 – only difference from pure Levenshtein - transposition
      if ( (i > 1) && (j > 1) && (s[i-1] == t[j-2]) && (s[i-2] == t[j-1]) ) {
        d[j*n+i] = minimum(INT_MAX, 
                           d[j*n+i], 
                           d[(j-2)*n+(i-2)] + cost);
      }
    }
  }
  
  int distance = d[n*m-1];
  free(d);
  
  sqlite3_result_int(context, distance <= max_distance);
}


inline int minimum(int a, int b, int c) {
  int min = a;
  if (b < min) min=b;
  if (c < min) min=c;
  return min;
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
  sqlite3_create_function(db, "damerau_levenshtein", 3, SQLITE_ANY, 0, damerau_levenshtein, NULL, NULL);
  
  return 0;
}