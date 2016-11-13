#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


int main(void) {

  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLHSTMT stmt2;
  SQLHSTMT stmt3;
  SQLHSTMT stmt4;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR full_name[512];
  SQLCHAR screen_name[512];
  char usuario[100];
  SQLINTEGER nousuario;
  SQLINTEGER y;
  SQLCHAR sign;

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
  /*Pedimos información al usuario*/

  printf("Añadir o borrar usuario: \n");
  fflush(stdout);
  while (scanf("%s %c %s", usuario, &sign, screen_name) != EOF) {

      if(strcmp(usuario, "usuario") != 0){
        printf("INPUT FAILURE - Syntax must be: \nusuario + <screen_name> <full_name> to create a new user\nusuario + <screen_name> to delete an user \n");
        fflush(stdout);
        printf("Añadir o borrar usuario: \n");
        fflush(stdout);
      }

      else if(sign == '+'){
        scanf(" ");
        /*Guardamos el nombre completo del usuario en una cadena*/
        fgets((char*)full_name, 512, stdin);
        /*Añadimos el screen_name solicitado a nuestra query*/
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
        /*Ejecutamos la query*/
        SQLExecute(stmt);
        /*Asignamos el valor de a primera columna a la variable nousuario*/
        SQLBindCol(stmt, 0, SQL_C_SBIGINT, &nousuario, sizeof(nousuario), NULL);
        if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("%d", ret); /*Aquí por alguna razon ret es 0*/
            printf("El usuario %s ya existe\n", screen_name);
            SQLCloseCursor(stmt);
            printf("Añadir o borrar usuario: \n");
            fflush(stdout);
        }

        else{
          SQLExecute(stmt4);
          SQLBindCol(stmt4, 1, SQL_C_SLONG, &y, sizeof(y), NULL);
          if (!SQL_SUCCEEDED(ret = SQLFetch(stmt4))) {
              printf("ERROR EN EL FETCH - NO SE PUDO OBTENER NUEVO ID DE USUARIO\n");
              fflush(stdout);
              SQLCloseCursor(stmt4);
          }
          else{
            SQLBindParameter(stmt2, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &y, 0, NULL);
            SQLBindParameter(stmt2, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
            SQLBindParameter(stmt2, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, full_name, 0, NULL);
            SQLExecute(stmt2);
            printf("Usuario añadido con éxito.\n");
            fflush(stdout);
            SQLCloseCursor(stmt);
            SQLCloseCursor(stmt4);
            SQLCloseCursor(stmt2);
            printf("Añadir o borrar usuario: \n");
            fflush(stdout);
        }
        }
      }

      else if(sign == '-'){
        fflush(stdout);
        /*Añadimos el screen_name solicitado a nuestra query*/
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
        /*Ejecutamos la query*/
        SQLExecute(stmt);
        /*Asignamos el valor de a primera columna a la variable nousuario*/
        SQLBindCol(stmt, 1, SQL_INTEGER, &nousuario, sizeof(nousuario), NULL);
        if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            SQLBindParameter(stmt3, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
            SQLExecute(stmt3);
            SQLCloseCursor(stmt);
            SQLCloseCursor(stmt3);
            printf("Usuario %s borrado con éxito\n", screen_name);
            fflush(stdout);
            printf("Añadir o borrar usuario: \n");
            fflush(stdout);
        }
        else{
          SQLCloseCursor(stmt);
          printf("El usuario introducido no se encuentra en la base de datos.\n");
          fflush(stdout);
          printf("Añadir o borrar usuario: \n");
          fflush(stdout);
        }
      }

      else {
        printf("INPUT FAILURE - Incorrect instruction syntax:\n<usuario +> to add a new user\n<usuario -> to delete an user\n");
        fflush(stdout);
        printf("Añadir o borrar usuario: \n");
        fflush(stdout);
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
