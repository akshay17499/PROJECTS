#ifndef DECODE_H
#define DECODE_H
#include<stdlib.h>
#include<stdio.h>
#include"types.h"

typedef struct decode
{
    FILE *fptr;
    char * image_file_name;
    FILE* fptr_secret_file;
    char* secret_file;
    char* user_given_secret_file_name;

}Decode;


Status do_decoding(Decode * decoinfo , char*argv[] ,int argc);

Status read_and_validate_decode_args( Decode *decoinfo , char*argv[] ,int argc);

Status openfile(Decode*decoinfo);

char* char_decode(char*arr,int size);

Status check_for_magic_string(FILE*fptr);

int int_decode(FILE*fptr);

Status find_the_message(FILE*fptr,Decode *decoinfo);

#endif
