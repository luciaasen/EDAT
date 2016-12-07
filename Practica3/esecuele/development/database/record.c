#include "record.h"

struct record_ {
  void ** values;
  int ncols;
  long next;
};

record_t* record_create(void** values, int ncols, long next) {
  record_t* record;

  record = (record_t*)malloc(sizeof(record));
  if(!record)
    return NULL;

  record->values = values;
  record->ncols = ncols;
  record->next = next;

  return record;
}

void* record_get(record_t* record, int n) {
  return record->values[n];
}

long record_next(record_t* record) {
  return record->next;
}

void record_free(record_t* record) {
  int i;
  if(!record) return;
  if(!record->values){
  	free(record);
  	return;
  }
  for(i = 0; i < record->ncols ; i++){
    if(record->values[i])
        free(record->values[i]);
  }
  
  free(record->values);  
  free(record);
}
