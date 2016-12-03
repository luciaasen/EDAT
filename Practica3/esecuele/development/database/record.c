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

  return record_t;
}

void* record_get(record_t* record, int n) {
  return record->values[n];
}

long record_next(record_t* record) {
  return record->next;
}

void record_free(record_t* record) {
  int i;
  for(i = 0; i < ncols ; i++){
    free(record->values[i]);
  }

  free(record);
}
