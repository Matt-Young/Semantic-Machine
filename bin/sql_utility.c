  char buff[100];
  const char * query = "select 'hello Sqlite3';";
  const char * cmd = "sqlite3  test.db  < name > result";
  f = fopen("name", "w+");
  F = *f;

  if(f==0)	exit(1);
  fwrite(query, strlen(query), 1, f);
  fclose(f);
  system(cmd);
  f = fopen("result", "r");
  if(f==0)	exit(2);

  while (fgets(buff, sizeof(buff)-1, f) != NULL) 
    printf("%s", buff);
  fclose(f);