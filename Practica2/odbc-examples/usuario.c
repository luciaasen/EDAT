#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#include <time.h>


int main(int argc, char** argv) {

  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret;
  SQLCHAR full_name[512];
  SQLCHAR screen_name[512];
  SQLCHAR fecha[512];
  SQLINTEGER nousuario;
  time_t tiempo;
  struct tm *tlocal;

  /*CONECTO CON LA BASE DE DATOS*/
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  /*Reservo memoria para el contenedor*/
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*Introduzco en él la primera query que realizaremos*/
  SQLPrepare(stmt, (SQLCHAR*) "select nousuario from usuario where screen_name = ? and borrado = false", SQL_NTS);

  /*Comprobamos si los argumentos del main son los correctos para el funcionamiento del programa*/
  if (argc < 3){
    printf("Error en los parámetros de entrada:\n");
    printf("./usuario + <screen_name> <full_name> (entre comillas) para insertar usuario.\n");
    printf("./usuario - <screen_name> para borrar usuario.\n");
    /*Liberamos y desconectamos*/
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  strcpy((char*)screen_name, argv[2]);
    /*Si la opción es añadir usuario:*/
    if(argv[1][0] == '+'){
        /*Comprobamos que nos hayan proporcionado screen_name y full_name*/
        if(argc < 4){
          printf("Error en los parámetros de entrada:\n");
          printf("./usuario + <screen_name> <full_name> (entre comillas) para insertar usuario.\n");
          SQLFreeHandle(SQL_HANDLE_STMT, stmt);
          ret = odbc_disconnect(env, dbc);
          if (!SQL_SUCCEEDED(ret)) {
              return EXIT_FAILURE;
          }
          return EXIT_SUCCESS;
        }

        strcpy((char*)full_name, argv[3]);
        /*Obtenemos la fecha actual para añadirla al perfil del usuario*/
        tiempo = time(0);
        tlocal = localtime(&tiempo);
        strftime((char*)fecha,128,"%Y-%m-%d",tlocal);

        /*Añadimos el screen_name solicitado a nuestra query*/
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
        /*Ejecutamos la query*/
        if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
            printf("Error al comprobar si existe el usuario solicitado.\n");
            SQLCloseCursor(stmt);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            ret = odbc_disconnect(env, dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
        /*Asignamos que el resultado del fetch se realice en la variable nousuario*/
        SQLBindCol(stmt, 0, SQL_C_SBIGINT, &nousuario, sizeof(nousuario), NULL);
        /*Si el fetch se realiza sin errores quiere decir que el usuario ya está contenido en la BD*/
        if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("El usuario %s ya existe, escoja otro screen name.\n", screen_name);
            SQLCloseCursor(stmt);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            ret = odbc_disconnect(env, dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
        SQLCloseCursor(stmt);
        /*Ahora comprobamos si existe un usuario con mismo screen y full name borrado en la BD*/
        SQLPrepare(stmt, (SQLCHAR*) "select nousuario from usuario where borrado = true and screen_name = ? and full_name = ?", SQL_NTS);
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, full_name, 0, NULL);
        if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
            printf("Error al comprobar si el usuario solicitado estaba borrado.\n");
            SQLCloseCursor(stmt);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            ret = odbc_disconnect(env, dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
        /*En caso afirmativo simplemente reactivamos ese usuario*/
        if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            SQLCloseCursor(stmt);
            SQLPrepare(stmt, (SQLCHAR*) "update usuario set borrado = false where screen_name = ?", SQL_NTS);
            if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
                printf("Error al reactivar el usuario solicitado.\n");
                SQLCloseCursor(stmt);
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                ret = odbc_disconnect(env, dbc);
                if (!SQL_SUCCEEDED(ret)) {
                    return EXIT_FAILURE;
                }
                return EXIT_SUCCESS;
            }
            printf("Usuario %s reactivado con éxito.\n", screen_name);
            SQLCloseCursor(stmt);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            ret = odbc_disconnect(env, dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
              }
              return EXIT_SUCCESS;
        }
        /*Si no, creamos un nuevo usuario*/
        /*Obtenemos el nousuario más grande de la BD + 1 para asignárselo al nuevo usuario*/
        SQLCloseCursor(stmt);
        SQLPrepare(stmt, (SQLCHAR*) "select nousuario + 1 from usuario order by nousuario desc", SQL_NTS);
          if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
              printf("Error 1 al insertar usuario.\n");
              SQLCloseCursor(stmt);
              SQLFreeHandle(SQL_HANDLE_STMT, stmt);
              ret = odbc_disconnect(env, dbc);
              if (!SQL_SUCCEEDED(ret)) {
                  return EXIT_FAILURE;
              }
              return EXIT_SUCCESS;
          }
          SQLBindCol(stmt, 1, SQL_C_SLONG, &nousuario, sizeof(nousuario), NULL);
          /*Si no hay ninguno en la base de datos le asignamos el valor 1*/
          if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            nousuario = 1;
          }
          SQLCloseCursor(stmt);
          /*Insetamos el nuevo usuario*/
          SQLPrepare(stmt, (SQLCHAR*) "insert into usuario (nousuario, ccard, screen_name, full_name, fecha) values (?, 0000000000000000, ?, ?, ?)", SQL_NTS);
          SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nousuario, 0, NULL);
          SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
          SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, full_name, 0, NULL);
          SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, fecha, 0, NULL);
          if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
              printf("Error 3 al insertar usuario.\n");
              SQLCloseCursor(stmt);
              SQLFreeHandle(SQL_HANDLE_STMT, stmt);
              ret = odbc_disconnect(env, dbc);
              if (!SQL_SUCCEEDED(ret)) {
                  return EXIT_FAILURE;
              }
              return EXIT_SUCCESS;
          }
          printf("Usuario %s añadido con éxito.\n", screen_name);
          SQLCloseCursor(stmt);
        }
      /*Si la opción borrar usuario:*/
      else if(argv[1][0] == '-'){
        /*Comprobamos si el usuario está en la BD (sin borrar)*/
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
        if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
            printf("Error al comprobar si existe el usuario solicitado.\n");
            SQLCloseCursor(stmt);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            ret = odbc_disconnect(env, dbc);
            if (!SQL_SUCCEEDED(ret)) {
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
        SQLBindCol(stmt, 1, SQL_INTEGER, &nousuario, sizeof(nousuario), NULL);
        /*Si el fetch se ejecuta es que el usuario existe*/
        if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            SQLCloseCursor(stmt);
            /*Cambiamos el valor de borrado a true*/
            SQLPrepare(stmt, (SQLCHAR*) "update usuario set borrado = true where screen_name = ?", SQL_NTS);
            SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, screen_name, 0, NULL);
            if (!SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
                printf("Error al borrar el usuario solicitado.\n");
                SQLCloseCursor(stmt);
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                ret = odbc_disconnect(env, dbc);
                if (!SQL_SUCCEEDED(ret)) {
                    return EXIT_FAILURE;
                }
                return EXIT_SUCCESS;
            }
            SQLCloseCursor(stmt);
            printf("Usuario %s borrado con éxito\n", screen_name);
            fflush(stdout);
        }
        /*En caso contrario el usuario no existe, por lo que no puede ser borrado*/
        else{
          SQLCloseCursor(stmt);
          printf("El usuario introducido no se encuentra en la base de datos.\n");
          fflush(stdout);
        }
      }
      /*En caso de que la opción no haya sido añadir (+) o borrar (-)*/
      else {
        printf("Error en los parámetros de entrada:\n");
        printf("./usuario + <screen_name> <full_name> (entre comillas) para insertar usuario.\n");
        printf("./usuario - <screen_name> para borrar usuario.\n");
        fflush(stdout);
        /* free up statement handle */
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        /* DISCONNECT */
        ret = odbc_disconnect(env, dbc);
        if (!SQL_SUCCEEDED(ret)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;

      }

  /*Liberamos y desconectamos la base de datos*/

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
