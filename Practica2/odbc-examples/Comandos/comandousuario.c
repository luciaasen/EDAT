#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


int main(int argc, char** argv) {

  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLHSTMT stmt2;
  SQLHSTMT stmt3;
  SQLHSTMT stmt4;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR full_name[512];
  SQLCHAR screen_name[512];
  SQLINTEGER nousuario;
  SQLINTEGER y;

  /* CONNECT */
  /*CONECTO CON LA BASE DE DATOS*/
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt2);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt3);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt4);


  SQLPrepare(stmt, (SQLCHAR*) "select nousuario from usuario where screen_name = ? and borrado = false", SQL_NTS);
  SQLPrepare(stmt2, (SQLCHAR*) "insert into usuario (nousuario, ccard, screen_name, full_name, fecha) values (?, 0000000000000000, ?, ?, '2016-11-13')", SQL_NTS);
  SQLPrepare(stmt3, (SQLCHAR*) "update usuario set borrado = true where screen_name = ?", SQL_NTS);
  SQLPrepare(stmt4, (SQLCHAR*) "select nousuario + 1 from usuario order by nousuario desc", SQL_NTS);

  if (argc != 3) {
    fprintf(stderr, "Error en los parametros de entrada:\n\n");
    fprintf(stderr, "%s + <screen_name> <full_name> para crear nuevo usuario.\n", argv[0]);
    fprintf(stderr, "%s - <screen_name> para eliminar un usuario.\n", argv[0]);
    return EXIT_FAILURE;
}

  /*Añadimos el screen_name a una cadena SQL*/

  strcpy((char*)screen_name, argv[2]);

  if(argv[1][0] == '+'){
    /*Guardamos el nombre completo del usuario en una cadena*/
    strcpy((char*)full_name, argv[3]);
    /*Añadimos el screen_name solicitado a nuestra query*/
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
    /*Ejecutamos la query*/
    if (SQL_SUCCEEDED(ret = SQLExecute(stmt)) == 0) {
        printf("Error al comprobar si el usuario ya existe.\n");
        fflush(stdout);
        SQLCloseCursor(stmt);
        return EXIT_FAILURE;
    }
    /*Asignamos el valor de la primera columna a la variable nousuario*/
    SQLBindCol(stmt, 0, SQL_C_SBIGINT, &nousuario, sizeof(nousuario), NULL);
    /*Si el fetch se realiza correctamente es que el usuario ya existe */
    if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("El usuario %s ya existe\n", screen_name);
        SQLCloseCursor(stmt);
        printf("Añadir o borrar usuario: \n");
        fflush(stdout);
    }
    else{
      /*Ejecutamos la query que nos proporciona el nuevo nousuario*/
      if (SQL_SUCCEEDED(ret = SQLExecute(stmt4)) == 0) {
          printf("Error 1: al generar id del nuevo usuario.\n");
          fflush(stdout);
          SQLCloseCursor(stmt4);
          SQLCloseCursor(stmt);
          return EXIT_FAILURE;
      }
      /*Hacemos fecth de ella en la tabla*/
      SQLBindCol(stmt4, 1, SQL_C_SLONG, &y, sizeof(y), NULL);
      if (!SQL_SUCCEEDED(ret = SQLFetch(stmt4))) {
          printf("Error 2: al generar id del nuevo usuario.\n");
          fflush(stdout);
          SQLCloseCursor(stmt4);
          SQLCloseCursor(stmt);
          return EXIT_FAILURE;
      }
      /*Configuramos la query que añade el usuario y la ejecutamos*/
      SQLBindParameter(stmt2, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &y, 0, NULL);
      SQLBindParameter(stmt2, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
      SQLBindParameter(stmt2, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, full_name, 0, NULL);
      if (SQL_SUCCEEDED(ret = SQLExecute(stmt2)) == 0) {
          printf("Error al insertar el nuevo usuario.\n");
          fflush(stdout);
          SQLCloseCursor(stmt4);
          SQLCloseCursor(stmt2);
          SQLCloseCursor(stmt);
          return EXIT_FAILURE;
      }
      printf("Usuario añadido con éxito.\n");
      fflush(stdout);
      SQLCloseCursor(stmt);
      SQLCloseCursor(stmt4);
      SQLCloseCursor(stmt2);
    }
  }

  else if(argv[1][0] == '-'){
    fflush(stdout);
    /*Añadimos el screen_name solicitado a nuestra query*/
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
    /*Ejecutamos la query*/
    /*Ejecutamos la query*/
    if (SQL_SUCCEEDED(ret = SQLExecute(stmt)) == 0) {
        printf("Error al comprobar si el usuario deseado existe.\n");
        fflush(stdout);
        SQLCloseCursor(stmt);
        return EXIT_FAILURE;
    }
    /*Asignamos el valor de la primera columna a la variable nousuario*/
    SQLBindCol(stmt, 1, SQL_INTEGER, &nousuario, sizeof(nousuario), NULL);
    if (SQL_SUCCEEDED(ret = SQLFetch(stmt)) == 0) {
      SQLCloseCursor(stmt);
      printf("El usuario introducido no se encuentra en la base de datos.\n");
      fflush(stdout);
      return EXIT_FAILURE;
    }
    /*Actualizamos la query que borra el usuario deseado*/
    SQLBindParameter(stmt3, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
    if (SQL_SUCCEEDED(ret = SQLExecute(stmt3)) == 0) {
        printf("Error al borrar el usuario seleccionado.\n");
        fflush(stdout);
        SQLCloseCursor(stmt);
        SQLCloseCursor(stmt3);
        return EXIT_FAILURE;
    }
    SQLCloseCursor(stmt);
    SQLCloseCursor(stmt3);
    printf("Usuario %s borrado con éxito\n", screen_name);
    fflush(stdout);
  }

  else {
    fprintf(stderr, "Error en los parametros de entrada:\n\n");
    fprintf(stderr, "%s + <screen_name> <full_name> para crear nuevo usuario.\n", argv[0]);
    fprintf(stderr, "%s - <screen_name> para eliminar un usuario.\n", argv[0]);
    fflush(stdout);
    return EXIT_FAILURE;
  }
}

  /* free up statement handle */
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt3);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt4);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
