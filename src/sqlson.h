
int  Sqlson_to_Bson(Triple t[],char * *Bson);  // uses remalloc when needed, should return bufer
int Bson_to_Sqlson(Triple t[], char * Bson);  // I assume bson is word aligned.

//When a bson arrives, it is collected and send to the scratch table
// When a quer result in an output, the sqlson is sent back to originator
