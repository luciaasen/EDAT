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
  SQLINTEGER nousuario;
  SQLCHAR screen_name[512];
  SQLCHAR isbn[512];
  SQLCHAR fecha[512];

  /* CONNECT */
  /*CONECTO CON LA BASE DE DATOS*/
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  if(argc < 3){
    printf("Error en los parámetros de entrada:\n");
    printf("./compra <screen_name> <isbn> ... <isbn> para realizar una compra.\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
  time_t tiempo = time(0);
  struct tm *tlocal = localtime(&tiempo);
  strftime((char*)fecha,128,"%Y-%m-%d",tlocal);
  strcpy((char*)screen_name, argv[1]);

  SQLPrepare(stmt, (SQLCHAR*) "select nousuario from usuario where screen_name = ?", SQL_NTS);
  SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);

  if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
      printf("Error al comprobar el screen_name del usuario.\n");
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

  SQLBindCol(stmt, 1, SQL_C_SLONG, &nousuario, sizeof(nousuario), NULL);

  if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      printf("El usuario %s no existe\n", screen_name);
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
  SQLCloseCursor(stmt);
  SQLPrepare(stmt, (SQLCHAR*) "insert into compra (isbn, nousuario, metodo, fecha) values (?, ?, ccard, ?)", SQL_NTS);

  SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nousuario, 0, NULL);
  SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha, 0, NULL);

  /*Bucle con bind parameter de los isbn y sus execute*/

}
