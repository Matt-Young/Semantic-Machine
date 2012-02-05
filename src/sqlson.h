
int *  Sqlson_to_Bson(Triple t[]);  // uses remalloc when needed, should return bufer
int Bson_to_Sqlsone(Triple t[], int * Bson);  // I assume bson is word aligned.

//When a bson arrives, it is collected and send to the scratch table
// When a quer result in an output, the sqlson is sent back to originator
