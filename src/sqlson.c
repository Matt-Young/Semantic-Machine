#include "../src/g_types.h"
#include "../src/machine.h"
#include "../src/engine.h"
#include "../src/console.h"
#include "../src/sqlson.h"
// A utility to translate triples into bson
//
// Bson is not native sqlson, but close
// the effort here is to create the recursive
// bson byte counts from sqlson row counts
//
//Make Bson from triple

#define BSON_ARRAY 4
#define BSON_OBJECT 3
int make_sqlson_type(int bson_type){
  int sqlson_type ='.';  // default type
  if(bson_type == BSON_ARRAY) sqlson_type =',';
  return sqlson_type | bson_type << 8;}
// Save a pointer to the parent byte count location
// Convert and fill n the element type right away
// and the parent key is valid, fill it in.
int make_bson_type(int sqlson_type){ return  sqlson_type >> 8;}
typedef struct {  int rowid; 
char * start; Triple t; int byte_count;
} CallBoxBson;
int make_bson_from_sqlson(Triple *tr,CallBoxBson * parent) {
  CallBoxBson child;
  char *bson_count_ptr;
  int bson_key_len; int bson_type;
  bson_type = make_bson_type(parent->t.link);
   if((bson_type == BSON_OBJECT) || (bson_type == BSON_ARRAY)) {
    bson_count_ptr = parent->start; 
    parent->start +=4;
  }
  parent->start[0] = bson_type; parent->start++;
  bson_key_len = machine_key_len((Code) tr->key);
  G_memcpy(parent->start, parent->t.key,bson_key_len);
  parent->start[bson_key_len]=0;
  parent->byte_count = bson_key_len+2;  //type, zero terminator, and optional text
  parent->start += parent->byte_count;
  parent->rowid++;
  while(parent->rowid < parent->t.pointer) {
    child = *parent;
    child.start = parent->start;
    triple(tr+pop_triple_operator,0);
    child.t = tr[pop_triple_data];
    parent->byte_count += make_bson_from_sqlson(tr,&child);
    parent->rowid = child.t.pointer;
  }
  if((bson_type == BSON_OBJECT) || (bson_type == BSON_ARRAY)){
    *bson_count_ptr++ = parent->byte_count >> 24;
    *bson_count_ptr++ = (parent->byte_count >> 16) & 0xff;
    *bson_count_ptr++ = (parent->byte_count >> 8) & 0xff;
    *bson_count_ptr++ = parent->byte_count & 0xff;
  }
  return parent->byte_count;
}
#define SerialInt(a) ((a[0] << 8) + a[1])
//Make triple from Bson
typedef char * Bson;
typedef struct {  int byteid; 
char  * empty; Bson b; int rowid;
} CallBoxSqlson;
int make_sqlson_from_bson(Triple *tr,CallBoxSqlson * parent) {
  CallBoxSqlson child;
  Triple new_triple;
  int bson_type;int block_count;char * start;
  int rowid =   parent->rowid;  // save parent row for a pointer update

  // fill in the part we know
  if((parent->empty[0] == BSON_ARRAY) || (parent->empty[0] == BSON_OBJECT )) {
    block_count = (int) *((int *) parent->empty); 
    parent->empty += 4;
    while(parent->byteid < block_count) {
      child = *parent;
      child.empty = parent->empty;
      parent->byteid += make_sqlson_from_bson(tr,&child);
      parent->rowid = child.rowid;
    }
  }
  else {
    start = parent->empty;
    bson_type = parent->empty[0];
    if((char *) parent->empty[1]) { // have a name
      tr[append_triple_data].key = (char *) parent->empty+1;
      tr[append_triple_data].pointer = 1;
      tr[append_triple_data].link = ':';
      triple(tr+append_triple_operator,0);
      parent->rowid++;
      parent->empty += G_strlen(tr[append_triple_data].key)+1;
    }
    tr[append_triple_data].link = make_sqlson_type(bson_type);
    tr[append_triple_data].pointer =1;
    triple(tr+append_triple_operator,0);
    parent->byteid=G_strlen((char *) parent->empty+1);
    new_triple.pointer = rowid;
    triple(tr+update_triple_operator,0);
    parent->byteid += G_strlen((char *) parent->empty+1) + 1;
    block_count =  parent->empty-start;
    parent->rowid++;
  }
  return block_count;
}
#define BSIZE 256
int Sqlson_to_Bson(Triple t[],char ** buff) {
  CallBoxBson call;
  G_memset(&call,0,sizeof(call));
  *buff = (char *) G_calloc(BSIZE,1);
  triple(t+pop_triple_operator,0);
  call.t = t[pop_triple_data];
  call.start = *buff;
  return make_bson_from_sqlson(t,&call);

}

int Bson_to_Sqlson(Triple t[],char * Bson) {
  CallBoxSqlson call;
  G_memset(&call,0,sizeof(call));
  call.empty = Bson;
  return make_sqlson_from_bson(t,&call);  //use the null table on input
}