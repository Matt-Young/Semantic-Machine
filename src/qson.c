#include "../src/g_types.h"
#include "../src/config.h"
#include "../src/engine.h"
#include "../src/tables.h"
#include "../src/console.h"
#include "../src/qson.h"

// Qson bundled in and out
//
// extract the Qson
Triple * set_output_buff(Triple *t);
int get_qson_graph(Triple *t) {
  int rows,len,total; Triple v;int i;
  Triple *Qson;
  Code stmt = get_ready_stmt();
  rows = t[0].pointer; total = 0;
  Qson = (Triple *) G_malloc(rows*sizeof(Triple));
  set_output_buff(Qson);
  for(i=0;i<rows;i++) {
    Qson[i] = *t;
    len = machine_key_len(stmt); 
    Qson[i].key=(char *) G_malloc(len+4);
    *Qson[i].key = len;
    G_memcpy(Qson[i].key+4,t->key,len);
    t++;
    total += len;
    machine_step_fetch(&v,0);
  }
    return total + sizeof(Triple) * rows;
}

// extract the Qson
Triple * set_output_buff(Triple *t);
int put_qson_graph(TABLE *table,Triple *Qson) {
  int rows,len,total; int i;
  Code stmt = get_ready_stmt();
  Triple *t;
  t = Qson;
  rows = t[0].pointer; total = 0;
  set_output_buff(Qson);
  for(i=0;i<rows;i++) {
    t->key = Qson[i].key+4;;
    len = Qson[i].key[0];
  // machine_append_blob(&Qson[i],0);
   G_free(Qson[i].key+4);
  }
  G_free(Qson);
    return total + sizeof(Triple) * rows;
}
