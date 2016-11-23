/*Grupo 1201, Lucia Asencio y David Garcia*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <time.h>
#include "odbc.h"

int main(int argc, char** argv){

  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret;
  SQLHSTMT stmt;
  SQLCHAR titulo[512];
  int n, i;
  
   /*CONECTO CON LA BASE DE DATOS*/
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  if(argc < 2){
    printf("Error en los parámetros de entrada:\n");
    printf("./best_seller <n>\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }
  
	/*Consulta cuyo resultado guardamos en stmt y de donde extraeremos la info*/
	SQLPrepare(stmt, (SQLCHAR*) "select t.titulonombre as titulo, count(*) as count from compra as c inner join (select titulonombre, isbn from edicion) as t on t.isbn = c.isbn group by t.titulonombre	order by count desc", SQL_NTS);
	
	if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
      printf("Error al buscar los bestsellers.\n");
      fflush(stdout);
      SQLCloseCursor(stmt);
      /* free up statement handle */
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);
      /* DISCONNECT */
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
  }
  
  n = atoi(argv[1]);
  SQLBindCol(stmt, 1, SQL_C_CHAR, titulo, sizeof(titulo), NULL);
  for(i = 0; i < n; i++){
  	if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      printf("No hay suficientes ventas como para tener %d bestsellers\n", n);
      SQLCloseCursor(stmt);
      /* free up statement handle */
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);

      /* DISCONNECT */
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
  	}else{
  		printf("%dº: %s\n", i+1, titulo);
  	}
  }
  
  /*Fin del programa*/
  SQLCloseCursor(stmt);
      /* free up statement handle */
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);

      /* DISCONNECT */
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
}
  
  
  
  
  

