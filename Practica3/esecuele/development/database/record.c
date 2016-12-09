#include "record.h"

struct record_ {
  void ** values;
  int ncols;
  long next;
};

record_t* record_create(void** values, int ncols, long next) {
  record_t* record;
  if(!values || ncols <= 0 || next < 0)
    return NULL;
  record = (record_t*)malloc(sizeof(record_t));
  if(!record)
    return NULL;
  
  printf("%d\n", 1);  
  record->values = values;
  printf("%d\n", 2);
  record->ncols = ncols;
  printf("%d\n", 3);
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
