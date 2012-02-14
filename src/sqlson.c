#include "../src/g_types.h"
#include "../src/config.h"
#include "../src/engine.h"
#include "../src/tables.h"
#include "../src/console.h"
#include "../src/sqlson.h"
// A utility to translate triples into bson and back
//

int make_sqlson_type(int bson_type){
  int sqlson_type ='.';  // default type
  if(bson_type == BSON_ARRAY) sqlson_type =',';
  return sqlson_type | bson_type << 8;}

int  make_word_from_bytes(char * b) {
  int i;
     i =  *b++;
    i += *b++  << 8;
    i += *b++  << 16;
    i += *b++  << 24; 
    return i;}

char *  make_bytes_from_word(char * b,int i) {
    *b++ = i & 0xff;
    *b++ = (i >> 8) & 0xff;
    *b++ = (i >> 16) & 0xff;
    *b++ = i >> 24;
  return b;}

int make_bson_type(int sqlson_type){
  if(sqlson_type & 0xff00)
    return  sqlson_type >> 8;
  else return 1;}
typedef struct {  int rowcount; 
char * start; Triple t; int byte_count;
} CallBoxBson;
// Save a pointer to the parent byte count location
// Convert and fill n the element type right away
// and the parent key is valid, fill it in.
int make_bson_from_sqlson(Triple *tr,CallBoxBson * parent) {
  CallBoxBson child;
  char *bson_count_ptr;
  int bson_key_len; int bson_type;
  parent->byte_count=0;
  bson_count_ptr=0;
  bson_type = make_bson_type(parent->t.link);
  if((bson_type == BSON_OBJECT) ||(bson_type==BSON_BINARY)
    || (bson_type == BSON_ARRAY) ||(bson_type==BSON_STRING)) {
    bson_count_ptr = parent->start; 
    parent->start +=4;
    parent->byte_count+=4;
  }
  if(parent->t.link == ':') { 
    G_printf("Name\n"); }
   else {
  parent->start[0] = bson_type; 
  parent->start[1]=0;
  parent->byte_count +=2;  //type, zero terminator, and optional text
  parent->start +=2;
   }
  bson_key_len = machine_key_len((Code) tr->key);
  G_memcpy(parent->start, parent->t.key,bson_key_len);
  parent->start[bson_key_len]=0;
  parent->byte_count += bson_key_len+1;  //zero terminator, and optional text
  parent->start += bson_key_len+1;
  parent->rowcount=1;
  while(parent->rowcount < parent->t.pointer) {
    child = *parent;
    child.rowcount=0;
    triple(tr+pop_triple_operator,0);
    child.t = tr[pop_triple_data]; 
    parent->byte_count += make_bson_from_sqlson(tr,&child);
    parent->rowcount +=  child.rowcount;
    parent->start = child.start;
  }
  if(bson_count_ptr){
    bson_count_ptr =make_bytes_from_word(bson_count_ptr,parent->byte_count);
  }
  return parent->byte_count;
}
typedef char  *Bson;
typedef struct {  int byte_count; 
char  * empty; Bson b; int rowid;
} CallBoxSqlson;
int make_sqlson_element(Triple *tr,CallBoxSqlson * parent) {
  int i;
    int bson_type = parent->empty[0];
    //ps(parent->empty);
    parent->byte_count=1;parent->empty++;
    if(parent->empty[0]) { // have a name ?
      tr[append_triple_data].key = (char *) parent->empty+1;
      tr[append_triple_data].pointer = 1;
      tr[append_triple_data].link = ':';
      triple(tr+append_triple_operator,0);
      parent->rowid++;
      parent->empty += G_strlen(tr[append_triple_data].key)+1;
      parent->byte_count += G_strlen((char *) parent->empty+1) + 1;
    }  else { parent->empty++;parent->byte_count++;}
    tr[append_triple_data].link = make_sqlson_type(bson_type);
    tr[append_triple_data].pointer =1;
    tr[append_triple_data].key = (char *) parent->empty;
    triple(tr+append_triple_operator,0);
    i = G_strlen(tr[append_triple_data].key) + 1;
    parent->rowid++;
    parent->byte_count += i;
    parent->empty +=i;
    return(parent->byte_count);
}
//Make triple from Bson

int make_sqlson_from_bson(Triple *tr,CallBoxSqlson * parent) {
  CallBoxSqlson child;
  int char_count;
  // on entry we have a compound object
  char_count = make_word_from_bytes(parent->empty);
  if((parent->empty[0] != BSON_ARRAY) && (parent->empty[0] != BSON_OBJECT )) 
    G_printf("Bson Error\n");
  parent->byte_count =4;
  parent->empty +=4;
  parent->byte_count += make_sqlson_element(tr,parent);  // lead
  while(parent->byte_count < char_count) {
    child = *parent;
    child.rowid=0;
    if((parent->empty[0] == BSON_ARRAY) || (parent->empty[0] == BSON_OBJECT )) 
      parent->byte_count += make_sqlson_from_bson(tr,&child);
    else
      parent->byte_count += make_sqlson_element(tr,&child);
    parent->rowid += child.rowid;
    parent->empty = child.empty;
  }
  triple(tr+update_triple_operator,0);
  return parent->byte_count;
}
#define BSIZE 256
int Sqlson_to_Bson(Triple t[],char ** buff) {
  CallBoxBson call;
  G_memset(&call,0,sizeof(call));
  *buff = (char *) G_calloc(BSIZE);
  triple(t+pop_triple_operator,0);
  call.t = t[pop_triple_data];
  call.start = *buff;
  call.rowcount=0; 
  return make_bson_from_sqlson(t,&call);

}

int Bson_to_Sqlson(Triple t[],char * Bson) {
  CallBoxSqlson call;
  G_memset(&call,0,sizeof(call));
  call.empty = Bson;
 //ps(call.empty);
  return make_sqlson_from_bson(t,&call);  //use the null table on input
}
int Sqlson_to_Json(Triple t[],char **Bson) {
  // make lazy J
  // for each element in J
  // if new block, add ':'
  // make json element
  else
  // if add add key text link
}
  G_memset(&call,0,sizeof(call));
  call.empty = Bson;
 //ps(call.empty);
  return make_sqlson_from_bson(t,&call);  //use the null table on input
}