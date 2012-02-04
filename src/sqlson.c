// A utility to translate triples into bson
//
// Bson is not native sqlson, but close
// the effort here is to create the recursive
// bson byte counts from sqlson row counts
//
//
typedef struct {  int rowid; int * buffer; 
int * start; Triple t; int word_count;
} CallBoxBson;
int add_triple_descentCode stmt,(CallBoxBson * parent) {
  	CallBoxBson child;
	int *bson_count_ptr;
  int bson_key_len;
  // Save a pointer to the parent byte count location
  // Convert and fill n the element type right away
  // and the parent key is valid, fill it in.
  bson_count_ptr = parent->start; parent->start++;
  parent->start[0] = convert(parent->t.link); parent->start++;
  bson_key_len = machine_key_len(parent->stmt);
	memcpy(parent->start, parent->t.key,bson_key_len);
  parent->word_count = 0;
  parent->start += bson_key_len;
	while(parent->rowid < parent->t.pointer) {
		child = *parent;
		child.start = parent->buffer;
    machine_triple(ready.stmt,&child.t);
		add_triple_descent(&child);
		parent->rowid = child.rowid;
    parent->word_count += child.word_count;
	}

  typedef int Bson;
typedef struct {  int wordid; 
int * empty; Bson b; int row_count;
} CallBoxSqlson;
int add_element_descent(Code stmt,CallBoxSqlson * parent) {
  CallBoxSqlson child;
  Triple new_triple;
  parent->row_count++;
  new_triple.link = parent->start[0];
  new_triple.key = (char *) parent->start+1;
  new_triple.pointer =0;
  while(parent->wordid < parent->b) {
    child = *parent;
		child.start = parent->buffer;
    machine_triple(parent->stmt,&child.t);
		add_triple_descent(&child);
		parent->rowid = child.rowid;
    parent->word_count += child.word_count;
  }
  new_triple.pointer = parent->row_count;
  machine_insert_triple(ready.stmt,&new_triple);
}
#define BSIZE 256
int Sqlson_to_Bson(Code stmt,Table * t ) {
  CallBoxBson call;
  void * pointer = malloc(BSIZE); 
  ;  // uses remalloc when needed
   CallBoxBson call;
  memset(&call,0,sizeof(call));
  call.t = test[0];
  call.buffer = bson_buffer;
  call.start = bson_buffer;
}

int Bson_to_Sqlsone(Code stmt,int * Bson) {
  CallBoxBson call;
  memset(&call,0,sizeof(call));
  call.start = Bson;
  add_triple_descent(Code stmt,&call);  //use the null table on input
}