#ifndef TYPES_H
#define TYPES_H
#include<unistd.h>
/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure,
    e_failure_b,
    e_failure_t
} Status;            //if u are collecting this enum variable wen need to use data type Status for return type

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
