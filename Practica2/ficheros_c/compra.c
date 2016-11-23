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
  SQLHSTMT stmt2;
  SQLHSTMT stmt3;
  SQLHSTMT stmt4;
  SQLINTEGER nousuario;
  SQLCHAR screen_name[512];
  SQLCHAR isbn[512];
  SQLCHAR precio[512];
  SQLCHAR titulo[512];
  SQLCHAR fecha[512];
  char buffer[512];
  time_t tiempo;
  struct tm *tlocal;
  int i, j = 0;
  float total;

  /* CONNECT */
  /*CONECTO CON LA BASE DE DATOS*/
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*Comprobamos los parámetros de entrada*/

  if(argc < 3){
    printf("Error en los parámetros de entrada:\n");
    printf("./compra <screen_name> <isbn> ... <isbn> para realizar una compra.\n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  /*Obtenemos la fecha actual y el screen name del usuario*/
  tiempo = time(0);
  tlocal = localtime(&tiempo);
  strftime((char*)fecha,128,"%Y-%m-%d",tlocal);
  strcpy((char*)screen_name, argv[1]);

  /*Comprobamos si el usuario existe*/

  SQLPrepare(stmt, (SQLCHAR*) "select nousuario from usuario where screen_name = ? and borrado = false", SQL_NTS);
  SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);

  if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
      printf("Error al comprobar el screen_name del usuario.\n");
      fflush(stdout);
      SQLCloseCursor(stmt);
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
  }

  SQLBindCol(stmt, 1, SQL_C_SLONG, &nousuario, sizeof(nousuario), NULL);

  /*En caso de que exista el usuario guardamos su nousuario*/

  if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      printf("El usuario %s no existe.\n", screen_name);
      SQLCloseCursor(stmt);
      SQLFreeHandle(SQL_HANDLE_STMT, stmt);
      ret = odbc_disconnect(env, dbc);
      if (!SQL_SUCCEEDED(ret)) {
          return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
  }

  /*Creamos la nueva compra*/

  SQLCloseCursor(stmt);
  SQLPrepare(stmt, (SQLCHAR*) "insert into compra (isbn, nousuario, metodo, fecha) values (?, ?, 'card', ?)", SQL_NTS);

  /*Sólo podemos hacer bind de los parámetros fijos, la fecha y el numero de usuario*/

  SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nousuario, 0, NULL);
  SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha, 0, NULL);

  /*Preparamos la query que nos proporcionará el precio del libro si está en oferta*/
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt2);
  SQLPrepare(stmt2, (SQLCHAR*) "select e.titulonombre, e.precio*(0.9)*o.descuento, e.precio from afectaoferta as a, oferta as o, edicion as e where o.id = a.ofertaid and a.isbn = ? and o.fecha1 <= ? and o.fecha2 >= ? and e.isbn = a.isbn", SQL_NTS);

  /*Preparamos la query que nos devolverá el precio del libro si no está en oferta*/
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt3);
  SQLPrepare(stmt3, (SQLCHAR*) "select titulonombre, precio*(0.9) from edicion where isbn = ?", SQL_NTS);

  /*Además debemos comprobar si el ISBN se encuenta en nuestra base de datos*/
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt4);
  SQLPrepare(stmt4, (SQLCHAR*) "select isbn from edicion where isbn = ?", SQL_NTS);

  for(i = 2; i < argc ; i++){
    strcpy((char*)isbn, argv[i]);
    SQLBindParameter(stmt4, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, isbn, 0, NULL);
    if (!SQL_SUCCEEDED(ret = SQLExecute(stmt4))) {
        printf("Error al comprobar si el ISBN %s se encuenta en la base de datos.\n", isbn);
        SQLCloseCursor(stmt);
        SQLCloseCursor(stmt2);
        SQLCloseCursor(stmt3);
        SQLCloseCursor(stmt4);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt3);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt4);
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (!SQL_SUCCEEDED(ret = SQLFetch(stmt4))) {
      printf("\nEl ISBN %d no se encuentra en la base de datos.\n",i - 1 );
      SQLCloseCursor(stmt4);
    }
    else{
    SQLBindCol(stmt4, 1, SQL_C_CHAR, isbn, sizeof(isbn), NULL);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, isbn, 0, NULL);
    if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
        printf("Error al realizar la compra del libro cuyo ISBN es: %s.\n", isbn);
        fflush(stdout);
        SQLCloseCursor(stmt);
        SQLCloseCursor(stmt2);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt3);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt4);
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    /*Ahora obtenemos el precio final del libro*/
    SQLBindParameter(stmt2, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, isbn, 0, NULL);
    SQLBindParameter(stmt2, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha, 0, NULL);
    SQLBindParameter(stmt2, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha, 0, NULL);
    if (!SQL_SUCCEEDED(ret = SQLExecute(stmt2))) {
        printf("Error al obtener el precio del libro cuyo ISBN es: %s.\n", isbn);
        fflush(stdout);
        SQLCloseCursor(stmt);
        SQLCloseCursor(stmt2);
        SQLCloseCursor(stmt3);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt3);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt4);
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    SQLBindCol(stmt2, 1, SQL_C_CHAR, titulo, sizeof(titulo), NULL);
    SQLBindCol(stmt2, 2, SQL_C_CHAR, precio, sizeof(precio), NULL);
    j = 1;
    /*Si tenemos fallo en este fetch es que el libro no está en oferta*/
    if (!SQL_SUCCEEDED(ret = SQLFetch(stmt2))) {
    j = 0;
    SQLBindParameter(stmt3, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, isbn, 0, NULL);
      if (!SQL_SUCCEEDED(ret = SQLExecute(stmt3))) {
          printf("Error al obtener el precio del libro sin oferta cuyo ISBN es: %s.\n", isbn);
          fflush(stdout);
          SQLCloseCursor(stmt);
          SQLCloseCursor(stmt2);
          SQLCloseCursor(stmt3);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt3);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt4);
          ret = odbc_disconnect(env, dbc);
          if (!SQL_SUCCEEDED(ret)) {
              return EXIT_FAILURE;
          }
          return EXIT_SUCCESS;
      }
      SQLBindCol(stmt3, 1, SQL_C_CHAR, titulo, sizeof(titulo), NULL);
      SQLBindCol(stmt3, 2, SQL_C_CHAR, precio, sizeof(precio), NULL);
      if (!SQL_SUCCEEDED(ret = SQLFetch(stmt3))) {
          printf("No se ha podido obtener el precio del libro cuyo isbn es %s\n", isbn);
          SQLCloseCursor(stmt);
          SQLCloseCursor(stmt2);
          SQLCloseCursor(stmt3);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt3);
          SQLFreeHandle(SQL_HANDLE_STMT, stmt4);
          ret = odbc_disconnect(env, dbc);
          if (!SQL_SUCCEEDED(ret)) {
              return EXIT_FAILURE;
          }
          return EXIT_SUCCESS;
      }
      SQLCloseCursor(stmt3);
    }
    SQLCloseCursor(stmt2);
    printf("\nHa comprado: %s\nPrecio final de esta edición: %s\n", titulo, precio);
    if(j == 1)
      printf("Este producto se encuentra en oferta.\n");
    strcpy(buffer, (char*)precio);
    /*Cambio el formato del precio obtenido por uno con el que pueda operar*/
    for(j = 0; buffer[j] != '\0' ; j++){
      if(buffer[j] == ',')
        buffer[j] = '.';
    }
    total += atof(buffer);
    SQLCloseCursor(stmt4);
  }
}

  printf("\nPrecio final de la compra: %.2f€\n", total);

  SQLCloseCursor(stmt);
  SQLCloseCursor(stmt2);
  SQLCloseCursor(stmt3);
  SQLCloseCursor(stmt4);

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt3);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt4);
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;

}
