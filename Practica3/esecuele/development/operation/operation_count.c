#include "operation.h"

/* COUNT OPERATION: operation whose result is the number of results of the source operation */

typedef struct {
    operation_t* suboperation;
    int counter;
} operation_count_args_t;

void
operation_count_reset(void* vargs) {
    operation_count_args_t* args = vargs;
    args->counter = 0;
    operation_reset(args->suboperation);
}

int operation_count_next(void* vargs) {
    /* Dudo de si es simplemente hacer next de la suboperación, creo que sí */
    int ret;
    operation_count_args_t* args = vargs;
    operation_t* suboperation = args->suboperation;
    /*Está bien y es necesario este bucle?*/
    /*while(ret = operation_next(suboperation));*/
    /*Creo que eso nos saltaría directamente al final*/
    /*O simplemente hay que hacer...*/
    ret = operation_next(suboperation);
    /*Tiene mejor pinta*/
    return ret;
}

void* operation_count_get(int col, void* vargs) {
    operation_count_args_t* args = vargs;
    args->counter = 0;
    for (args->counter = 0 ; operation_next(args->suboperation) ; args->counter++);
    return &(args->counter);
}

void operation_count_close(void* vargs) {
    operation_count_args_t* args = vargs;

    operation_close(args->suboperation);
    free(args);
}

operation_t*
operation_count_create(operation_t* suboperation) {
      operation_t* operation;
      operation_count_args_t* args;

      args = malloc(sizeof(operation_count_args_t));
      args->suboperation = suboperation;

      operation = malloc(sizeof(operation_t));
      operation->args = args;
      operation->reset = operation_count_reset;
      operation->next = operation_count_next;
      operation->get = operation_count_get;
      operation->close = operation_count_close;
      /*DUDA IMPORTANTE A MIRAR*/
      operation->ncols = 1;
      operation->types = malloc(sizeof(type_t));
      operation->types = /*????*/

      return operation;
}
