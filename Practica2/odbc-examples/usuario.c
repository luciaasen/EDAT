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
  /*Pedimos información al usuario*/

  if (argc < 3){
    printf("Error en los parámetros de entrada:\n");
    printf("./usuario + <screen_name> <full_name> (entre comillas) para insertar usuario.\n");
    printf("./usuario - <screen_name> para borrar usuario.\n");

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

  strcpy((char*)screen_name, argv[2]);

    if(argv[1][0] == '+'){
        if(argc < 4){
          printf("Error en los parámetros de entrada:\n");
          printf("./usuario + <screen_name> <full_name> (entre comillas) para insertar usuario.\n");

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

        strcpy((char*)full_name, argv[3]);

        /*Añadimos el screen_name solicitado a nuestra query*/
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
        /*Ejecutamos la query*/
        if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
            printf("Error al comprobar si existe el usuario solicitado.\n");
            fflush(stdout);
            SQLCloseCursor(stmt);
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
        /*Asignamos el valor de la primera columna a la variable nousuario*/
        SQLBindCol(stmt, 0, SQL_C_SBIGINT, &nousuario, sizeof(nousuario), NULL);

        if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("El usuario %s ya existe\n", screen_name);
            SQLCloseCursor(stmt);
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
        else{
          if (!SQL_SUCCEEDED(ret = SQLExecute(stmt4))) {
              printf("Error 1 al insertar usuario.\n");
              fflush(stdout);
              SQLCloseCursor(stmt4);
              SQLCloseCursor(stmt);
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
          SQLBindCol(stmt4, 1, SQL_C_SLONG, &y, sizeof(y), NULL);
          if (!SQL_SUCCEEDED(ret = SQLFetch(stmt4))) {
              printf("Error 2 al insertar usuario.\n");
              fflush(stdout);
              SQLCloseCursor(stmt4);
              SQLCloseCursor(stmt);
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
          else{
            SQLBindParameter(stmt2, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &y, 0, NULL);
            SQLBindParameter(stmt2, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
            SQLBindParameter(stmt2, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, full_name, 0, NULL);
            if (!SQL_SUCCEEDED(ret = SQLExecute(stmt2))) {
                printf("Error 3 al insertar usuario.\n");
                fflush(stdout);
                SQLCloseCursor(stmt4);
                SQLCloseCursor(stmt);
                SQLCloseCursor(stmt2);
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
            printf("Usuario %s añadido con éxito.\n", screen_name);
            fflush(stdout);
            SQLCloseCursor(stmt);
            SQLCloseCursor(stmt4);
            SQLCloseCursor(stmt2);
          }
        }
      }

      else if(argv[1][0] == '-'){
        /*Añadimos el screen_name solicitado a nuestra query*/
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
        /*Ejecutamos la query*/
        if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
            printf("Error al comprobar si existe el usuario solicitado.\n");
            fflush(stdout);
            SQLCloseCursor(stmt);
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
        /*Asignamos el valor de a primera columna a la variable nousuario*/
        SQLBindCol(stmt, 1, SQL_INTEGER, &nousuario, sizeof(nousuario), NULL);
        if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            SQLBindParameter(stmt3, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
            if (!SQL_SUCCEEDED(ret = SQLExecute(stmt3))) {
                printf("Error al borrar el usuario solicitado.\n");
                fflush(stdout);
                SQLCloseCursor(stmt);
                SQLCloseCursor(stmt);
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
            SQLCloseCursor(stmt);
            SQLCloseCursor(stmt3);
            printf("Usuario %s borrado con éxito\n", screen_name);
            fflush(stdout);
        }
        else{
          SQLCloseCursor(stmt);
          printf("El usuario introducido no se encuentra en la base de datos.\n");
          fflush(stdout);
        }
      }

      else {
        printf("Error en los parámetros de entrada:\n");
        printf("./usuario + <screen_name> <full_name> (entre comillas) para insertar usuario.\n");
        printf("./usuario - <screen_name> para borrar usuario.\n");
        fflush(stdout);
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
