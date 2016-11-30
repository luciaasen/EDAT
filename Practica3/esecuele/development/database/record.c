#include "record.h"

struct record_ {
  /* To be implemented */
  /*
    void ** values;
    int ncols
    long next

  */
};

record_t* record_create(void** values, int ncols, long next) {
  /* To be implemented */
  /*
    malloc record
    values
    ncols
    next
    (igual que en table)
  */
}

void* record_get(record_t* record, int n) {
  /* To be implemented */
  /*
    devolver record->values[n]
  */
}

long record_next(record_t* record) {
  /* To be implemented */
}

void record_free(record_t* record) {
  /* To be implemented */
  /*Libera toda la memoria*/
  /*record values y de record*/
}
