/* test Ping connection */
#include <stdio.h>
#include <sql.h>
#include <sqlext.h> /* SQL_ATTR_ODBC_VERSION, SQL_OV_ODBC3*/

/*
SQLHENV - environment handle // manejador de entorno
This is the first handle you will need as everything else is effectively in the environment. 
Once you have an environment handle you can define the version of ODBC you require, 
enable connection pooling and allocate connection handles with SQLSetEnvAttr and SQLAllocHandle.

SQLHDBC - manejador de conexion
Tu necesitas un manejador de conexion para cada fuente de datos que se va a conectar.
Como los SQLHENV los SQLHDBC tienen atributos los cual puedes recibir y establecer con 
SQLSetConnectAttr and SQLGetConnectAttr
*/
#define ERRMSG_LEN      200
SQLINTEGER CheckError ( SQLRETURN       rc,
                SQLSMALLINT     handleType,
            SQLHANDLE       handle,
            SQLCHAR*        errmsg)
{
    SQLRETURN       retcode = SQL_SUCCESS;
    SQLSMALLINT     errNum = 1;
    SQLCHAR     sqlState[6];
    SQLINTEGER      nativeError;
    SQLCHAR     errMsg[ERRMSG_LEN];
    SQLSMALLINT     textLengthPtr;
      
    if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO))
    {
        while (retcode != SQL_NO_DATA)
        {
            retcode = SQLGetDiagRec (handleType, handle, errNum, sqlState, 
                        &nativeError, errMsg, ERRMSG_LEN, 
                        &textLengthPtr);
            if (retcode == SQL_INVALID_HANDLE)
            {
                fprintf (stderr, "SQL_INVALID_HANDLE!\n");
                return 1;
            }
            if (( retcode == SQL_SUCCESS) || 
                ( retcode == SQL_SUCCESS_WITH_INFO))
                fprintf (stderr, "ERROR: %d:  %s : %s \n", 
                                 (int)nativeError, sqlState, errMsg);
                errNum++;
        }
        fprintf (stderr, "%s\n", errmsg);
        return 1; 
    }
    else
    return 0;
}
int main() 
{
	SQLHENV env;
	SQLHDBC dbc;
	long res;

	/* For sentences SQL*/
	SQLHSTMT stmt;
	SQLRETURN sql_return;
	
	/*
     *  Find something out about the driver.
     */
	SQLCHAR dbms_name[256], dbms_ver[256], database_name[256], data_source_name[256], 
     			driver_name[256],
     			driver_hstmt[256],
     			driver_odbc_ver[256],
     			insert_statement[256],
     			max_statement_len[256],
     			max_tables_in_select[256],
     			sql_subqueries[256],
     			sql_user_name[256],
     			sql_max_columns_in_select[256];


	/* Allocate an environment handle */
	SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	/* We want ODBC 3 support */
 	SQLSetEnvAttr( env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
 	/* Allocate a connection handle */
 	SQLAllocHandle( SQL_HANDLE_DBC, env, &dbc);
 	res = SQLConnect( dbc, (SQLCHAR*) "test", SQL_NTS,
                               (SQLCHAR*) "root", SQL_NTS,
                               (SQLCHAR*) "!root+123+abc+ABC!", SQL_NTS);

 	if (SQL_SUCCEEDED(res)) {
    	printf("Connected\n\n");
 		sql_return = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
 		if (CheckError (sql_return, SQL_HANDLE_DBC, dbc, (SQLCHAR *) "SQLAllocHandle failed\n"))
	    return ;

 		sql_return = SQLPrepare(stmt, (SQLCHAR *) "SELECT id, name FROM jobeet.jobeet_category;", SQL_NTS);
 		if (CheckError (sql_return, SQL_HANDLE_STMT, stmt, (SQLCHAR *) "SQLPrepare failed\n"))
    	return ;
		
 		sql_return = SQLExecute(stmt);
 		if (CheckError (sql_return, SQL_HANDLE_STMT, stmt, (SQLCHAR *) "SQLExecute failed\n"))
    	return ;

 		/**
         * Bind a column to a variable
         */
        SQLCHAR         sName[15+1];
        SQLLEN          lsname = 0;

     	
		SQLGetInfo(dbc, SQL_MAX_TABLES_IN_SELECT, (SQLPOINTER)max_tables_in_select, sizeof(max_tables_in_select), NULL);
    	printf("SQL_MAX_TABLES_IN_SELECT: %s\n", max_tables_in_select);
	
		SQLGetInfo(dbc, SQL_SUBQUERIES, (SQLPOINTER)sql_subqueries, sizeof(sql_subqueries), NULL);
    	printf("SQL_SUBQUERIES: %s\n", sql_subqueries);
		
		SQLGetInfo(dbc, SQL_USER_NAME, (SQLPOINTER)sql_user_name, sizeof(sql_user_name), NULL);
    	printf("SQL_USER_NAME: %s\n", sql_user_name);

		SQLGetInfo(dbc, SQL_MAX_COLUMNS_IN_SELECT, (SQLPOINTER)sql_max_columns_in_select, sizeof(sql_max_columns_in_select), NULL);
    	printf("SQL_MAX_COLUMNS_IN_SELECT: %s\n", sql_max_columns_in_select);

        sql_return = SQLBindCol(stmt, 2, SQL_C_CHAR, &sName, sizeof(sName), &lsname);
        while ((sql_return = SQLFetch(stmt)) != SQL_NO_DATA_FOUND) 
    		fprintf (stdout, "name = %s\n", sName);


    	


     	SQLUINTEGER getdata_support;
     	SQLUSMALLINT max_concur_act;
     	SQLSMALLINT string_len;

     	/* use %ld for print long unsigned int */
     	printf("SQL_DBMS_NAME : %i  sizeof %ld  \n", SQL_DBMS_NAME, sizeof(dbms_name));
     	printf("SQL_DBMS_VER  : %i sizeof %ld \n", SQL_DBMS_VER, sizeof(dbms_ver));
     	printf("SQL_GETDATA_EXTENSIONS  : %i sizeof %ld \n", SQL_GETDATA_EXTENSIONS, sizeof(getdata_support));

    	printf("\n** Find something out about the driver **\n");
     	SQLGetInfo(dbc, SQL_DBMS_NAME, (SQLPOINTER)dbms_name, sizeof(dbms_name), NULL);
     	printf("DBMS Name: %s\n", dbms_name);
     	
     	SQLGetInfo(dbc, SQL_DBMS_VER, (SQLPOINTER)dbms_ver, sizeof(dbms_ver), NULL);
    	printf("DBMS Version: %s\n", dbms_ver);
     	
     	SQLGetInfo(dbc, SQL_DATABASE_NAME, (SQLPOINTER)database_name, sizeof(database_name), NULL);
    	printf("Database name: %s\n", database_name);
     	

     	SQLGetInfo(dbc, SQL_GETDATA_EXTENSIONS, (SQLPOINTER)&getdata_support, 0, 0);
     	SQLGetInfo(dbc, SQL_MAX_CONCURRENT_ACTIVITIES, &max_concur_act, 0, 0);

     	if (max_concur_act == 0) {
     		printf("SQL_MAX_CONCURRENT_ACTIVITIES - no limit or undefined\n");
     	} else {
     		printf("SQL_MAX_CONCURRENT_ACTIVITIES = %u\n", max_concur_act);
     	}
     	if (getdata_support & SQL_GD_ANY_ORDER) {
     		// Las columnas se pueden recuperar en cualquier orden
      		printf("SQLGetData - columns can be retrieved in any order\n");
     	} else {
     		// Las columnas se deben recuperar en orden
      		printf("SQLGetData - columns must be retrieved in order\n");
    	}
		if (getdata_support & SQL_GD_ANY_COLUMN) {
			printf("SQLGetData - can retrieve columns before last bound one\n");
		} else {
			printf("SQLGetData - columns must be retrieved after last bound one\n");
		}


     	SQLGetInfo(dbc, SQL_DATA_SOURCE_NAME, (SQLPOINTER)data_source_name, sizeof(data_source_name), NULL);
    	printf("Data source name: %s\n", data_source_name);
     	
     	SQLGetInfo(dbc, SQL_DRIVER_NAME, (SQLPOINTER)driver_name, sizeof(driver_name), NULL);
    	printf("Driver name: %s\n", driver_name);

     	SQLGetInfo(dbc, SQL_DRIVER_HSTMT, (SQLPOINTER)driver_hstmt, sizeof(driver_hstmt), NULL);
    	printf("Driver HSTMT: %s\n", driver_hstmt);
		
		SQLGetInfo(dbc, SQL_DRIVER_ODBC_VER, (SQLPOINTER)driver_odbc_ver, sizeof(driver_odbc_ver), NULL);
    	printf("Driver ODBC VER: %s\n", driver_odbc_ver);
		
		SQLGetInfo(dbc, SQL_INSERT_STATEMENT, (SQLPOINTER)insert_statement, sizeof(insert_statement), NULL);
    	printf("INSERT STATEMENT: %s\n", insert_statement);
		
		SQLGetInfo(dbc, SQL_MAX_STATEMENT_LEN, (SQLPOINTER)max_statement_len, sizeof(max_statement_len), NULL);
    	printf("ISQL_MAX_STATEMENT_LEN: %s\n", max_statement_len);
		

	    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    	SQLFreeStmt (stmt, SQL_CLOSE);
	} else {
		printf("NO Connected\n");
 
	}
	/*%ld print long int in res */
 	//printf("RES: %ld \n", res);
 	SQLDisconnect( dbc );
    SQLFreeHandle( SQL_HANDLE_DBC, dbc );
    SQLFreeHandle( SQL_HANDLE_ENV, env );
 
    printf("\n");
    return 0;
 
}