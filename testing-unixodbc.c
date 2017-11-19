#include <stdlib.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

SQLHENV			 V_OD_Env;			// Handle ODBC environment
long			 V_OD_erg;			// result of functions
SQLHDBC			 V_OD_hdbc;			// Handle connection

char			 V_OD_stat[10];		// Status SQL
SQLINTEGER		 V_OD_err,V_OD_rowanz,V_OD_id;
SQLSMALLINT		 V_OD_mlen,V_OD_colanz;
char             V_OD_msg[200],V_OD_buffer[200];


int main(int argc,char *argv[])
{
	// 1. allocate Environment handle and register version 
	V_OD_erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&V_OD_Env);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error AllocHandle\n");
		exit(0);
	}
	V_OD_erg=SQLSetEnvAttr(V_OD_Env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error SetEnv\n");
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		exit(0);
	}
	// 2. allocate connection handle, set timeout
	V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, V_OD_Env, &V_OD_hdbc); 
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error AllocHDB %ld\n",V_OD_erg);
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		exit(0);
	}
	SQLSetConnectAttr(V_OD_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
	// 3. Connect to the datasource "web" 
	V_OD_erg = SQLConnect(V_OD_hdbc, (SQLCHAR*) "test", SQL_NTS,
                                     (SQLCHAR*) "root", SQL_NTS,
                                     (SQLCHAR*) "!root+123+abc+ABC!", SQL_NTS);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		printf("Error SQLConnect %ld\n",V_OD_erg);
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, 
		              V_OD_stat, &V_OD_err,V_OD_msg,100,&V_OD_mlen);
		printf("%s (%d)\n",V_OD_msg,V_OD_err);
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		exit(0);
	}
  SQLHSTMT V_OD_hstmt;
	printf("Connected !\n");
	V_OD_erg=SQLAllocHandle(SQL_HANDLE_STMT, V_OD_hdbc, &V_OD_hstmt);
	if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
	{
		printf("Fehler im AllocStatement %ld\n",V_OD_erg);
		SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
		printf("%s (%d)\n",V_OD_msg,V_OD_err);
        SQLDisconnect(V_OD_hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
		exit(0);
	}
    // SQLBindCol(V_OD_hstmt,1,SQL_C_CHAR, &V_OD_buffer,150,&V_OD_err);
    // SQLBindCol(V_OD_hstmt,2,SQL_C_ULONG,&V_OD_id,150,&V_OD_err);
	
    V_OD_erg=SQLExecDirect(V_OD_hstmt,"SELECT * FROM jobeet_category",SQL_NTS);   
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
       printf("Error in Select %ld\n",V_OD_erg);
       SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
       printf("%s (%d)\n",V_OD_msg,V_OD_err);
       SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
       SQLDisconnect(V_OD_hdbc);
       SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
       SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
       exit(0);
    }
    V_OD_erg=SQLNumResultCols(V_OD_hstmt,&V_OD_colanz);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
        SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
        SQLDisconnect(V_OD_hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
        exit(0);
    }
    printf("Number of Columns %d\n",V_OD_colanz);
    // V_OD_erg=SQLRowCount(V_OD_hstmt,&V_OD_rowanz);
    // if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    // {
    //   printf("Number ofRowCount %ld\n",V_OD_erg);
    //   SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
    //   SQLDisconnect(V_OD_hdbc);
    //   SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
    //   SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
    //   exit(0);
    // }
    printf("Number of Rows %d\n",V_OD_rowanz);
    V_OD_erg=SQLFetch(V_OD_hstmt);  
    while(V_OD_erg != SQL_NO_DATA)
    {
     // printf("Result: %s\n", V_OD_buffer);
     // printf("Result: %d %s\n",V_OD_id,V_OD_buffer);
     V_OD_erg=SQLFetch(V_OD_hstmt);  
    }  ;
    SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
    SQLDisconnect(V_OD_hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
    return(0);
}
  