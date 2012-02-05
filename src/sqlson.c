#include "../src/all.h"
#include "../src/sqlson.h"
// A utility to translate triples into bson
//
// Bson is not native sqlson, but close
// the effort here is to create the recursive
// bson byte counts from sqlson row counts
//
//Make Bson from triple
int make_bson_type(int link);
int make_sqlson_type(int link);
typedef struct {  int rowid; 
int * start; Triple t; int word_count;
} CallBoxBson;
int make_bson_from_sqlson(Triple *tr,CallBoxBson * parent) {
  	CallBoxBson child;
	int *bson_count_ptr;
  int bson_key_len;
  // Save a pointer to the parent byte count location
  // Convert and fill n the element type right away
  // and the parent key is valid, fill it in.
  bson_count_ptr = parent->start; parent->start++;
  parent->start[0] = make_bson_type(parent->t.link); parent->start++;
  bson_key_len = machine_key_len((Code) tr->key);
	G_memcpy(parent->start, parent->t.key,bson_key_len);
  parent->word_count = 0;
  parent->start += bson_key_len;
	while(parent->rowid < parent->t.pointer) {
		child = *parent;
		child.start = parent->start;
    triple(tr+pop_triple_operator,0);
    child.t = tr[pop_triple_data];
		make_bson_from_sqlson(tr,&child);
		parent->rowid = child.rowid;
    parent->word_count += child.word_count;
	}
  *bson_count_ptr=parent->word_count;
  return 0;
}
//Make triple from Bson
typedef int Bson;
typedef struct {  int wordid; 
int * empty; Bson b; int rowid;
} CallBoxSqlson;
int make_sqlson_from_bson(Triple *tr,CallBoxSqlson * parent) {
  CallBoxSqlson child;
  Triple new_triple;
  int rowid =   parent->rowid;  // save parent row for a pointer update
    // fill in the part we know
  tr[append_triple_data].link = parent->empty[0];
  tr[append_triple_data].key = (char *) parent->empty+1;
  tr[append_triple_data].pointer =0;
 triple(tr+append_triple_operator,0);

  while(parent->wordid < parent->b) {
    child = *parent;
		child.empty = parent->empty;
		make_sqlson_from_bson(tr,&child);
		parent->rowid = child.rowid;
    parent->wordid += child.wordid;
  }
  new_triple.pointer = rowid;
  triple(tr+update_triple_operator,0);
  return 0;
}
#define BSIZE 256
int * Sqlson_to_Bson(Triple t[]) {
  CallBoxBson call;
  int * Bson;
  G_memset(&call,0,sizeof(call));
  Bson= (int *) G_malloc(BSIZE);
  call.t = t[0];
  call.start =Bson;
  make_bson_from_sqlson(t,&call);
  return Bson;
}

int Bson_to_Sqlsone(Triple t[],int * Bson) {
  CallBoxSqlson call;
  G_memset(&call,0,sizeof(call));
  call.empty = Bson;
  make_sqlson_from_bson(t,&call);  //use the null table on input
  return 0;
}