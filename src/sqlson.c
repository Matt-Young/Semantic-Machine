// A utility to translate triples into bson
typedef struct { Code stmt; int rowid; int * buffer; 
int * start; Triple t; int word_count;
} CallBoxBson;
int add_triple_descent(CallBoxBson * parent) {
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
    machine_triple(parent->stmt,&child.t);
		add_triple_descent(&child);
		parent->rowid = child.rowid;
    parent->word_count += child.word_count;
	}