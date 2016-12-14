
#include "operation.h"

/* UNION OPERATION: concatenation of the results of two operations with the same columns */

typedef struct {
    operation_t* operation1;
	operation_t* operation2;
	int flag;
} operation_union_args_t;

void
operation_union_reset(void* vargs) {
	operation_union_args_t* args = vargs;
	args->flag = 0;
	operation_reset(args->operation2);
    operation_reset(args->operation1);
}

int operation_union_next(void* vargs) {
	int ret;
	operation_union_args_t* args = vargs;
	operation_t* operation1 = args->operation1;
	operation_t* operation2 = args->operation2;

	if(args->flag == 0){
		ret = operation_next(args->operation1);
		if(!ret){
			args->flag = 1;
		}
		else
			return ret;
	}

	ret = operation_next(args->operation2);
	return ret;
}

void* operation_union_get(int col, void* vargs) {
	void* value;
    operation_union_args_t* args = vargs;

	if(args->flag == 0){
    	value = operation_get(col, args->operation1);
	}
	else{
		value = operation_get(col, args->operation2);
	}
    return value;
}

void operation_union_close(void* vargs) {
	operation_union_args_t* args = vargs;

    operation_close(args->operation1);
    operation_close(args->operation2);
    free(args);
}

operation_t*
operation_union_create(operation_t* operation1, operation_t* operation2) {
    operation_t* operation;
	operation_union_args_t* args;

	args = malloc(sizeof(operation_union_args_t));
	args->operation1 = operation1;
	args->operation2 = operation2;
	args->flag = 0;

	operation = malloc(sizeof(operation_t));
	operation->args = args;
	operation->reset = operation_union_reset;
	operation->next = operation_union_next;
    operation->get = operation_union_get;
    operation->close = operation_union_close;
	/*Suponemos que las columnas son las mismas en las dos tablas*/
	operation->ncols = operation1->ncols;
	operation->types = malloc(operation->ncols*sizeof(type_t));
	memcpy(operation->types, operation1->types, operation->ncols * sizeof(type_t));

	return operation;
}
