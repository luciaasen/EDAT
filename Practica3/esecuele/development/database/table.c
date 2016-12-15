#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "../type/type.h"

struct table_ {
  FILE * fichero;
  int ncols;
  long first_pos;
  long last_pos;
  type_t * types;
};

void table_create(char* path, int ncols, type_t* types) {
  FILE* file;
  file = fopen(path, "w");
  if(!file)
    return;
  fwrite(&ncols, sizeof(int), 1, file);
  fwrite(types, sizeof(type_t), ncols, file);
  fclose(file);
}

table_t* table_open(char* path) {
  table_t* table;
  /*Creamos la tabla*/
  table = (table_t*)malloc(sizeof(table_t));
  if(!table)
    return NULL;

  table->fichero = fopen(path, "r+");
  if(!table->fichero)
    return NULL;

  /*Obtenemos el número de columnas de la tabla*/
  fread(&(table->ncols), sizeof(int), 1, table->fichero);

	/*Reservamos para el array de tipo de columnas*/
	table->types = (type_t *)malloc(table->ncols * sizeof(type_t));
	if(!table->types) table_close(table);

	/*Leemos los tipos de las columnas y nos situamos al final de la cabecera*/
  fread(table->types, sizeof(type_t), table->ncols, table->fichero);
  table->first_pos = ftell(table->fichero);
  fseek(table->fichero, 0, SEEK_END);
  table->last_pos =  ftell(table->fichero);
  return table;

}

void table_close(table_t* table) {
  if(table){
  	if(table->types) free (table->types);
  	if(table->fichero) fclose(table->fichero);
		free(table);
	}
}

int table_ncols(table_t* table) {
  if(!table) return -1;
  return table->ncols;
}

type_t* table_types(table_t* table) {
  if(!table) return NULL;
  return table->types;
}

long table_first_pos(table_t* table) {
  if(!table) return -1;
  return table->first_pos;
}

long table_last_pos(table_t* table) {
  if(!table) return -1;
  return table->last_pos;
}

record_t* table_read_record(table_t* table, long pos) {
  void** values;
  record_t *rec;
  int i, size, ncols;
  long next;

  if(!table || table->last_pos == pos) return NULL;

  ncols = table_ncols(table);
  values = (void **)malloc((table->ncols)* sizeof(void *));
  fseek(table->fichero, pos, SEEK_SET);
  for(i = 0; i < table->ncols; i++){
  /*Leemos tamaño y reservamos*/
    fread(&size, sizeof(size_t), 1, table->fichero);
  	values[i] = malloc(size);
	/*Leemos valor*/
	 fread(values[i], size, 1, table->fichero);
  }
  /*Guardamos la posicion del siguiente*/
  next = ftell(table->fichero);

  rec = record_create(values, ncols, next);

  return rec;

}

void table_insert_record(table_t* table, void** values) {
    int i;
    unsigned int tam;
    if(!table || !values) return ;
    /*Hay que incrementar el last pos*/
    fseek(table->fichero, table->last_pos, SEEK_SET);
    for(i = 0 ; i < table->ncols ; i++){
      tam = value_length(table->types[i], values[i]);
      fwrite(&tam, sizeof(size_t), 1, table->fichero);
      fwrite(values[i], value_length(table->types[i], values[i]), 1, table->fichero);
    }
    table->last_pos = ftell(table->fichero);

}
