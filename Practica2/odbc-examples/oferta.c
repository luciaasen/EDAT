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
  SQLHSTMT stmt2;
  SQLDOUBLE descuento;
  SQLCHAR fecha2[512];
  SQLCHAR fecha1[512];
  SQLCHAR isbn[512];
  SQLINTEGER ofertaid;
  int i, counter;

  /*CONECTO CON LA BASE DE DATOS*/
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*Comprobamos los parámetros de entrada*/
  if(argc < 4){
    printf("Error en los parámetros de entrada:\n");
    printf("./oferta <descuento> <de> <a> <isbn> ... <isbn> para insertar nueva oferta.\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

  /*Guardamos los datos que nos pasa el usuario en el formato que deseamos*/

  descuento = (double)atoi(argv[1]);
  descuento = (1 - descuento/100);
  strcpy((char*)fecha1, argv[2]);
  strcpy((char*)fecha2, argv[3]);

  /*Obtenemos el id de la nueva oferta sumando 1 al id más grande de la base de datos*/

  SQLPrepare(stmt, (SQLCHAR*) "select id + 1 from oferta order by id desc", SQL_NTS);

  if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
      printf("Error al obtener el nuevo id de oferta.\n");
      fflush(stdout);
      SQLCloseCursor(stmt);
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
  }

  SQLBindCol(stmt, 1, SQL_C_SLONG, &ofertaid, sizeof(ofertaid), NULL);

  /*Si no había ofertas previamente le asignamos el valor 1 a ofertaid*/

  if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    ofertaid = 1;
  }

  SQLCloseCursor(stmt);

  /*Insertamos la nueva oferta*/
  SQLPrepare(stmt, (SQLCHAR*) "insert into oferta (descuento, id, fecha1, fecha2) values (?, ?, ?, ?)", SQL_NTS);

  SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &descuento, 0, NULL);
  SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &ofertaid, 0, NULL);
  SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha1, 0, NULL);
  SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha2, 0, NULL);

  if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
      printf("Error al insertar la oferta.\n");
      fflush(stdout);
      SQLCloseCursor(stmt);
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
  }

  SQLCloseCursor(stmt);

  /*Comprobamos si nos han pasado algún ISBN para añadir a la oferta*/
  if (argc >= 4){

  /*En tal caso debemos actualizar la tabla de la relación edición - oferta (afectaoferta)*/
  SQLPrepare(stmt, (SQLCHAR*) "insert into afectaoferta(ofertaid, isbn) values (?, ?)", SQL_NTS);

  /*Además debemos comprobar si el ISBN se encuenta en nuestra base de datos*/
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt2);
  SQLPrepare(stmt2, (SQLCHAR*) "select isbn from edicion where isbn = ?", SQL_NTS);

  /*Con este bucle relacionamos cada isbn con la oferta que acabamos de crear si existe*/
  for (i = 4, counter = 0 ; i < argc; i++){
    strcpy((char*)isbn, argv[i]);
    SQLBindParameter(stmt2, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, isbn, 0, NULL);
    if (!SQL_SUCCEEDED(ret = SQLExecute(stmt2))) {
        printf("Error al comprobar el ISBN %s\n", isbn);
        SQLCloseCursor(stmt);
        SQLCloseCursor(stmt2);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    SQLBindCol(stmt, 1, SQL_C_CHAR, isbn, sizeof(isbn), NULL);
    if (!SQL_SUCCEEDED(ret = SQLFetch(stmt2))) {
      printf("El ISBN %d no se encuentra en la base de datos.\n",i -3 );
    }
    else{
      SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &ofertaid, 0, NULL);
      SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, isbn, 0, NULL);
      if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
          printf("Error al asignar el isbn %d a la oferta.\n", i - 3);
          SQLCloseCursor(stmt);
          SQLCloseCursor(stmt2);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
          ret = odbc_disconnect(env, dbc);
          if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
          }
          return EXIT_SUCCESS;
      }
      counter++;
    }
    SQLCloseCursor(stmt);
    SQLCloseCursor(stmt2);
  }
}
else{
  /*Si no hay más argumentos se crea una oferta que no afecta a ningún ISBN*/
  printf("Oferta añadida con éxito.\n");
  printf("No se añadió ningun ISBN a la oferta.\n");
}

  printf("Oferta añadida con éxito.\n");
  printf("Se añadieron %d ISBN a la oferta.\n", counter);

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
