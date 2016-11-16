#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int main(int argc, char **argv) {

  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret;
  SQLHSTMT stmt;
  SQLFLOAT descuento; /*posible error*/
  SQLCHAR fecha2[512];
  SQLCHAR fecha1[512];
  SQLCHAR isbn[512];
  SQLINTEGER ofertaid;
  int i;

  /* CONNECT */
  /*CONECTO CON LA BASE DE DATOS*/
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  if(argc < 4){
    printf("Error en los parámetros de entrada:\n");
    printf("./oferta <descuento> <de> <a> <isbn> ... <isbn> para insertar nueva oferta.\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

  descuento = 1 - (atoi(argv[1])/100);
  strcpy((char*)fecha1, argv[2]);
  strcpy((char*)fecha2, argv[3]);

  /*Obtenemos el id de la nueva oferta*/

  SQLPrepare(stmt, (SQLCHAR*) "select id + 1 from oferta order by id desc", SQL_NTS);

  if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
      printf("Error al obtener el nuevo id de oferta.\n");
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

  SQLBindCol(stmt, 1, SQL_C_SLONG, &ofertaid, sizeof(ofertaid), NULL);

  if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))){ /*No hay ofertas*/
    ofertaid = 1;
    printf("CACA %d", ofertaid);
  }

  SQLCloseCursor(stmt);

  SQLPrepare(stmt, (SQLCHAR*) "insert into oferta (descuento, id, fecha1, fecha2) values (?, ?, ?, ?)", SQL_NTS);

  SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, &descuento, 0, NULL);
  SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &ofertaid, 0, NULL);
  SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha1, 0, NULL);
  SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha2, 0, NULL);

  if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
      printf("Error al insertar la oferta.\n");
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

  SQLCloseCursor(stmt);

  for (i = 4 ; i <= argc - 4; i++){
    strcpy((char*)isbn, argv[i]);
    SQLPrepare(stmt, (SQLCHAR*) "insert into afectaoferta values (?, ?)", SQL_NTS);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, isbn, 0, NULL);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &ofertaid, 0, NULL);
    if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
        printf("Error al asignar el isbn %d a la oferta.\n", i - 3);
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
    SQLCloseCursor(stmt);
  }

  /* free up statement handle */
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
