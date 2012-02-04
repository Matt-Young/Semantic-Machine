
int Sqlson_to_Bson(Code stmt,Table * t );  // uses remalloc when needed
int Bsonr_to_Sqlsone(Code stmt, int * Bson);  // I assume bson is word aligned.

//When a bson arrives, it is collected and send to the scratch table
// When a quer result in an output, the sqlson is sent back to originator
