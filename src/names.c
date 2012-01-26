#include "g_types.h"
#include "names.h"
#include "console.h"
// Just a quick utility to store name value pairs
// All names here should be constant

Trio g_names[LocalSymbolCount];
int  g_name_count;

int add_trio(char * name,int type,Pointer value) {
g_names[g_name_count].name = name;
g_names[g_name_count].type = type;
g_names[g_name_count].value = value;
g_name_count++;
return g_name_count;
}
int add_trios(Trio trios[]) {
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
Trio  * find_trio(char * name) { 
	int i;
	for(i=0;i < g_name_count;i++) 
		if(!G_strcmp(g_names[i].name,name))
			return(&g_names[i]);
	return 0;
}
Pointer  find_trio_value(char * name) {
	Trio * trio = find_trio(name);
	if(trio) return trio->value; else return 0;
}
 void print_trios() { 
	int i;
	for(i=0; g_names[i].name;i++) 
		G_printf("%3d: |%16s|%4d|%10d| \n",
		i,g_names[i].name,g_names[i].type,g_names[i].value,i);
	}