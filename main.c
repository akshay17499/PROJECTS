#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include"decode.h"

int main(int argc,char*argv[])
{
    EncodeInfo encInfo;
    Decode decoinfo;
    if(argc<2)
    {
      get_help_main();
      return 0;
    }
    if(check_operation_type(argv[1])==e_encode)
    {
         printf("\e[0;32m            OPERATION TYPE ENCODING       \n\e[0m\n");
         sleep(1);
        if(argc>3)
        {
          
         if(read_and_validate_encode_args(argv,&encInfo)==e_success)
         {
            if(argc==5&&(strstr(argv[4],".bmp")!=NULL))
            {
                strcpy(encInfo.stego_image_fname,argv[4]);
            }
            else if (argc<5)
            {
                strcpy(encInfo.stego_image_fname,"stego.bmp");
            }
            else{
              get_help_main();
              return 0;
            }
           printf("\e[0;36mINFO : source & text file validation successful\n \n\e[0m");
          
              if(do_encoding(&encInfo,argv)==e_success)
              {
                printf("\e[0;33m        INFO : ENCODING PROCESS WAS SUCCESSFUL  \n\e[0m\n");
              }
              else{
                  printf("\e[0;31mINFO : ENCODING FAILED\n\e[0m\n");
              }
           
          }
         else if(read_and_validate_encode_args(argv,&encInfo)==e_failure_t){
          printf("\e[0;31mINFO : text file validation failed\n\e[0m\n");
           return 0;  
         }
         else if(read_and_validate_encode_args(argv,&encInfo)==e_failure_b){
            printf("\e[0;31mINFO : bmb file validation failed\n\e[0m\n");
            return 0;
         }
         else{
           printf("\e[0;31mINFO : both text and bmp file validation failed\n\e[0m\n");
         }
        }
      else{

       get_help_main();
      }  
    }
    else if(check_operation_type(argv[1])==e_decode)
    {
      printf("\e[0;32m            OPERATION TYPE DECODING       \n\e[0m\n");
        if(do_decoding(&decoinfo,argv,argc)==e_failure)
        {
          printf("\e[0;31mINFO : DECODING FAILED\n\e[0m\n");
          return 0;
        }
        
        else{
           printf("\e[0;33m        INFO : DECODING PROCESS WAS SUCCESSFUL  \n\e[0m\n");
        }
    }
    
}

void get_help_main ()
{
    printf("\n\e[0;31mINFO : PLEASE PASS VALID ARGUMENTS\n\e[0m\n");
    printf("\e[0;31mINFO : Encoding - Minimum 4 arguments.\n\e[0m\n");
    printf("\e[0;36mINFO : ./a.exe -e <sorce_file>.bmp  <secret_file> \n \n\e[0m");
    printf("\e[0;31mINFO : Decoding - Minimum 3 arguments.\n\e[0m\n");
     printf("\e[0;34mINFO : ./a.exe -d <Encoded_file>.bmp  name \n \n\e[0m");
    
}
