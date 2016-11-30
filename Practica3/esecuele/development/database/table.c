#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "../type/type.h"

struct table_ {
  /* To be implemented */
  FILE * fichero;
  int ncols;
  long first_pos;
  long last_pos;
};

void table_create(char* path, int ncols, type_t* types) {
  /*
    fopen(path, "w")
    meter la cabecera con fwrite del ncols y de los tipos
    ej: fwrite(types, sizeof(type_t), n_cols, nobrefichero)
  */
  FILE* file;
  file = fopen(path, "w");
  if(!file)
    return;
  fwrite(n_cols, sizeof(int), 1, file);
  fwrite(types, sizeof(type_t), n_cols, file);
  fclose(file);
}

table_t* table_open(char* path) {
  /*
    Hacemos un malloc de table
    table->file = fopen
    leemos ncols table->ncols = fread
    leemos los tipos
      ¿¿¿¿¿¿ malloc de table->types ?????
      fread table->types
    table->first_pos = ftell
    table->last_pos = "fin archivo"
  */
  table_t* table;

  /*Creamos la tabla*/

  table = (table_t*)malloc(sizeof(table_t));
  if(!table)
    return NULL;

  table->fichero = fopen(path, "r");
  if(!table->fichero)
    return NULL;

  /*Obtenemos el número de columnas de la tabla*/
  fread(&(table->ncols), sizeof(int), 1, table->fichero);

  fseek(table->fichero, table->ncols*sizeof(type_t), SEEK_CUR);
  table_first_pos = ftell(table->fichero);
  /*FALTA EL FINAL Y DEVOLVER*/

}

void table_close(table_t* table) {
  /* To be implemented */
}

int table_ncols(table_t* table) {
  /* To be implemented */
}

type_t* table_types(table_t* table) {
  /* To be implemented */
}

long table_first_pos(table_t* table) {
  /* To be implemented */
}

long table_last_pos(table_t* table) {
  /* To be implemented */
}

record_t* table_read_record(table_t* table, long pos) {
  /* To be implemented */
  /*void** values*/
  /*Comprobar que la posicion que nos pasan no typeses la ultima (EOF)*/
  /*Hacemos fseek*/
  /*malloc de values = malloc(table->ncols*sizeof(void*))*/
  /*for(i, i ?< ncols, i++){
    fread(&size, sizeof(type_t), 1, table->file)
    values = malloc(size);
    fread(values[i], size, 1, tabla->file)
    next = ftell
    record_create(values, table->ncols, next);
  }*/
}

void table_insert_record(table_t* table, void** values) {
  /* To be implemented */
  /*Pensar nosotros*/
}
