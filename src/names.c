#include "g_types.h"
#include "console.h"
// Just a quick utility to stoe name value pairs
#define MAXNAMES 100
NameValue g_names[MAXNAMES];
int  g_name_count;
int add_name_value(NameValue pairs[]) {
	int i=0;
	char *  p;
	NameValue a;
	while(pairs[i].name != 0) {
		a = pairs[i];
		p =  (char * )G_malloc(G_strlen(a.name));
		G_strcpy((char *) p,a.name);
		g_names[g_name_count].name = p;
		g_names[g_name_count].value = a.value;
		g_name_count++;
		i++;
	}
	return 0;
}
int init_name_value() {
	g_name_count=0;
	G_memset(g_names,0,sizeof(g_names));
	G_printf("Init names %d\n",sizeof(g_names));
	return 0;
}
Pointer  find_name_value(char * name) { 
	int i;
	for(i=0; g_names[i].name;i++) 
		if(!G_strcmp(g_names[i].name,name))
			return(g_names[i].value);
	return 0;
}

 void print_name_value() { 
	int i;
	for(i=0; g_names[i].name;i++) 
		G_printf("%s %d %d \n",g_names[i].name,g_names[i].value,i);
	}