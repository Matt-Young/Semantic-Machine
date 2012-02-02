#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/g_types.h"
#include "../src/machine.h"
int main_engine(int argc, char * argv[]);
int engine_init();
int main(int argc, char *argv[]) {
  int i;
  for(i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-V") == 0) {
      printf("You are using %s.\n",VERSION);
      exit(0);
    } else if((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
      printf("Usage: graphs\n");
      printf("\n");
      printf("Please see https://github.com/Matt-Young/Semantic-Machine/wiki .\n");
      exit(0);
    } 
  }
          engine_init();
          main_engine(argc, argv);
  }