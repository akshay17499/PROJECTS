#include"decode.h"
#include"types.h"
#include<string.h>
#include"common.h"

void get_help()
{
    printf("\n\e[0;31mINFO : PLEASE PASS VALID ARGUMENTS\n\e[0m\n");
    printf("\e[0;31mINFO : Decoding - Minimum 3 arguments.\n\e[0m\n");
    printf("\e[0;36mINFO : ./a.exe -d <encoded_file>.bmp  <name>\n \n\e[0m");
    
}

Status read_and_validate_decode_args( Decode *decoinfo , char*argv[],int argc )
{
    if(argc>2)
    {
    if(strstr(*(argv+2),".bmp")!=NULL)
    {
       decoinfo->image_file_name = *(argv+2);
        decoinfo->secret_file=malloc(20);
       if(argc>3)
       {
        if(strstr(*(argv+3),".")!=NULL||argc>3)   //this argc
        {
           strcpy(decoinfo->secret_file,*(argv+3));
            sleep(1);
             printf("\e[0;36mINFO : file name entered for data collection\n \n\e[0m");
             return e_success; //this line
            
        }
        else{
           
           get_help();
           return e_failure;
            
        }
       }
       else{
            strcpy(decoinfo->secret_file,"output");
             sleep(1);
            printf("\e[0;36mINFO : default file was choosen\n \n\e[0m");
           
         return e_success;
       }
    }
    else
    {
        get_help();
       return e_failure;
    }
    }
    else{
        get_help();
       return e_failure;
    }
}

Status openfile(Decode*decoinfo)
{
    decoinfo->fptr = fopen(decoinfo->image_file_name,"rb");
    if(decoinfo->fptr==NULL){
       printf("\n\e[0;31mINFO : Failed to open file\n\e[0m\n");
        return e_failure;
    }
    return e_success;
}

char* char_decode(char*arr,int size)
{
    char c= c&0;
    int temp=0;
     char *d_arr = malloc((size/8+1)*sizeof(char));
    for(int i = 0;i<size/8;i++)
    {
        for(int j=0;j<8;j++)
        {
          c =  (arr[j+temp]&1)<<(7-j)|c;
        }
        d_arr[i]=c;
        temp +=8;
        c=c&0;
    }
    d_arr[size/8]='\0';
    return d_arr;
}


Status check_for_magic_string(FILE*fptr)
{
    fseek(fptr,54,SEEK_SET);
    char arr[16];
    fread(arr,1,16,fptr);
    char* temp = char_decode(arr,16);
    if(strcmp(temp,MAGIC_STRING)==0)
    return e_success;
    return e_failure;
}

int int_decode(FILE*fptr)
{
    char arr1[32];
    fread(arr1,sizeof(char),32,fptr);
    int num=0;
    for(int i=0;i<32;i++)
    {
       num = ((arr1[i]&1)<<(31-i))|num;
    }
    return num;
}

Status find_the_message(FILE*fptr,Decode *decoinfo)
{
   int file_ext_size =  int_decode(fptr);
   if(file_ext_size>1)
   {
       printf("\e[0;36mINFO : file extention size decoded successfully\n \n\e[0m");
       sleep(1);
   }
   else{
    printf("\n\e[0;31mINFO : failed to decode file extension size\n\e[0m\n");
   
    return e_failure;
   }
   char find_file_ext_arr[file_ext_size*8] ; 
   fread(find_file_ext_arr,1,file_ext_size*8,fptr);
   char* file_ext = NULL;
    file_ext = char_decode(find_file_ext_arr,file_ext_size*8);
    if(file_ext!=NULL)
    {
        printf("\e[0;36mINFO : file extention  decoded successfully\n \n\e[0m");
        sleep(1);
    }
    else{
        printf("\n\e[0;31mINFO : failed to decode file extension\n\e[0m\n");
          return e_failure;
    }
   strcat(decoinfo->secret_file,file_ext);
   
   decoinfo->fptr_secret_file = fopen(decoinfo->secret_file,"w");
   if(decoinfo->fptr_secret_file==NULL)
   {
      printf("\n\e[0;31mINFO : failed to create secret file\n\e[0m\n");
      return e_failure;
   }
    printf("\e[0;36mINFO : Successfully created secret file\n \n\e[0m");
    sleep(1);
   int length_data = int_decode(fptr);
   if(length_data>1)
   {
       printf("\e[0;36mINFO : file size decoded successfully\n \n\e[0m");
       sleep(1);
   }
   else{
    printf("\n\e[0;31mINFO : failed to decode file size\n\e[0m\n");
   
    return e_failure;
   }
   char data[8*length_data] ;
   fread(data,1,8*length_data,fptr);
   char* data_ptr = char_decode(data,8*length_data);
   if(fprintf(decoinfo->fptr_secret_file,"%s",data_ptr)==length_data)
   {
    return e_success;
   }
   return e_failure;
}

Status do_decoding(Decode * decoinfo ,char*argv[],int argc)
{
    if(read_and_validate_decode_args( decoinfo , argv,argc)==e_success)
    {
        sleep(1);
        printf("\e[0;36mINFO : file check successful\n \n\e[0m");
       sleep(1);
        
        if(openfile(decoinfo)==e_success)
        {
            printf("\e[0;36mINFO : file opened successfully\n \n\e[0m");
            sleep(1);

            if(check_for_magic_string(decoinfo->fptr)==e_success)
            {
                printf("\e[0;36mINFO : magic string found\n \n\e[0m");
               sleep(1);
                
               if(find_the_message(decoinfo->fptr,decoinfo)==e_success)
               {
                printf("\e[0;36mINFO : messege has been decoded and stored in the file \e[0m""%s\n\n",decoinfo->secret_file);
               sleep(1);
               }
               else{
                printf("\n\e[0;31mINFO : failed to decode message\n\e[0m\n");
                return e_failure;
               }
            }
            else{
                printf("\n\e[0;31mINFO : magic string not found\n\e[0m\n");
                return e_success;
            }
        }
        else{
            
            return e_failure;
        }

    }
    else{
       
        return e_failure;
    }
}
