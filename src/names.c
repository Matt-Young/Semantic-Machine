#include "./include/g_types.h"
#include "./include/names.h"
#include "./include/console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Just a quick utility to store name value pairs
// All names here should be constant
// This is shared memory
Trio g_names[LocalSymbolCount];
int  g_name_count;
extern int new_name_count;
extern int del_name_count;
Trio * add_trio( char * name,int type,Pointer value) {
g_names[g_name_count].name = name;
g_names[g_name_count].type = type;
g_names[g_name_count].value = value;
g_name_count++;
return &g_names[g_name_count];
}
int add_trios( Trio trios[]) {
	int i=0;
	while(trios[i].name != 0) {
		add_trio(trios[i].name,trios[i].type,trios[i].value);
		i++;
	}
	return 0;
}
int init_trios() {
	g_name_count=0;
	G_memset(g_names,0,sizeof(g_names));
	G_printf("Init names %d\n",sizeof(g_names));
  del_name_count=0;
  new_name_count=0;
	return 0;
}

 void print_trios() { 
	int i;
  printf("\n");
	for(i=0; g_names[i].name;i++) 
		printf("%3d: |%16s|%4d|%10d| \n",
		i,g_names[i].name,g_names[i].type,g_names[i].value,i);
	}

// when we make names we make buffs

char * new_string(const char * key) {
	int size = strlen(key)+1;
	char * p = (char *) malloc(size);
	strncpy(p,key,size);
	new_name_count++;
	return(p);
}
void del_string(const char * key) { 
	if(del_name_count < new_name_count) 
	{del_name_count++; G_free( (void *) key);}
}
typedef int (*cmp0)(const void*, const void*);
typedef int (*cmp1)(const char*, const char*);

int cmp2 (char * c1,Trio * c2) {
  int i;
  i = strcmp(c1,c2->name);
  return i;}
int cmp3 (Trio * c1,Trio * c2) {
  int i;
  i = strcmp(c1->name,c2->name);
  return i;}
Trio * get_name(char * key) {
  Trio * t;
   t = (Trio *) bsearch((void *) key, (void *) g_names, g_name_count, sizeof(Trio), (cmp0) cmp2);
   if(t == 0) 
    return add_trio(key,G_TYPE_INTEGER,0); 
   else return t;
}
Trio * find_name(char * key) {
  Trio * t;
  t =  (Trio *) 
    bsearch((void *) key, (void *) g_names, g_name_count, sizeof(Trio), (cmp0) cmp2);
  return t;
}

void sort_names() {
 qsort( g_names, g_name_count, sizeof(Trio),(cmp0) cmp3);
}
const char *zzz = "zzzz";
int flush_users() {
  int i;int count=0;
  for(i=0;i < g_name_count;i++) 
    if(g_names[i].type == G_TYPE_USER) {
      del_string( g_names[i].name);
      count++;
      g_names[i].type = G_TYPE_NONE;
      g_names[i].name = (char *) zzz;
    }
  g_name_count -= count;
  return count;
}
Pointer  find_trio_value(char * name) {
	Trio * trio = find_name(name);
	if(trio) return trio->value; else return 0;
}
