#include <stdio.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	for(int i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
  
	printf("\n");
	return 0;
}

int main(int argc, char **argv)
{
	sqlite3 *db;	//using database
	char *szErrMsg = 0;
	int nResult;
	
	if(argc != 3)
	{
		fprintf(stderr, "USAGE: %s DATABASE SQL-STATEMENT\n", argv[0]);
		return(1);
	}
	
	nResult = sqlite3_open(argv[1], &db);
	if(nResult)
	{
		fprintf(stderr, "ERROR: Can't open database %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	
	nResult = sqlite3_exec(db, argv[2], callback, 0, &szErrMsg);
	if(nResult)
	{
		fprintf(stderr, "ERROR: SQL error %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}
	
	sqlite3_close(db);
	return 0;
}