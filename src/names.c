#include "g_types.h"
#include "console.h"
// Just a quick utility to store name value pairs
// All names here should be constant
#define MAXNAMES 100
NameTypeValue g_names[MAXNAMES];
int  g_name_count;

int add_trio(char * name,int type,Pointer value) {
char * ch;
ch =  (char * )G_malloc(G_strlen(name));
G_strcpy((char *) ch,name);
g_names[g_name_count].name = ch;
g_names[g_name_count].value = value;
g_name_count++;
return g_name_count;
}
int add_trios(NameTypeValue trios[]) {
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
	return 0;
}
Pointer  find_trio(char * name) { 
	int i;
	for(i=0; g_names[i].name;i++) 
		if(!G_strcmp(g_names[i].name,name))
			return(g_names[i].value);
	return 0;
}

 void print_trios() { 
	int i;
	for(i=0; g_names[i].name;i++) 
		G_printf("%s %d %d \n",g_names[i].name,g_names[i].value,i);
	}