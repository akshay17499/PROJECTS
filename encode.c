#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include"common.h"
int total_count=0;
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // sleep(1);
    //printf("\e[0;36mwidth = %u \n\e[0m",width);
    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   // sleep(2);
    //printf("\e[0;36m\nheight = %u\n\e[0m",height);
    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

OperationType check_operation_type(char *argv)
{
    //printf("%s",argv);
    if(strcmp(argv,"-e")==0)
    {
       
        return e_encode;
    }
    else if(strcmp(argv,"-d")==0)
    {
       // printf("decoding operation has been selected \n");
        return e_decode;
    }
    else{
        printf("invalid operation \n");
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //printf("%s",argv[2]);
    if(strstr(argv[2],".bmp")!=NULL&&strstr(argv[3],".")!=NULL)
    {
         encInfo->src_image_fname=argv[2];
         encInfo->secret_fname=argv[3];
         //printf("%s\n",encInfo->src_image_fname);
         return e_success;
        
    }
    else{
        if(argv[2]==NULL||strstr(argv[2],".bmp")==NULL)
        return e_failure_b;
        else if(strstr(argv[3],".")==NULL)
        return e_failure_t;
        else
        return e_failure;
    }
}

uint get_file_size(FILE *fptr)
{
   // int count=0;
    fseek(fptr,0,SEEK_END);                                      //insted u can use fseek(seek end) then use ftell
    //printf("count = %d\n",count);               or open in append mode and use ftell
    return ftell(fptr);

}



Status check_capacity(EncodeInfo *encInfo,char*argv[])
{
  // dothr the remaning its not complete
  int len =strlen(strstr(argv[3],"."));
  //printf("%d",len);
   
   int size_i = get_image_size_for_bmp(encInfo->fptr_src_image);
   int size_f = get_file_size(encInfo->fptr_secret);
   if(size_i>54+size_f*8+strlen(MAGIC_STRING)+32+len*8+32)
   return e_success;
   else
   e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];
    fseek(fptr_src_image,0,SEEK_SET);
    //printf("sorce->%d\n",ftell(fptr_src_image));
   if(fread(arr,sizeof(char),54,fptr_src_image)==54)
   {
      if(fwrite(arr,sizeof(char),54,fptr_dest_image)==54)
      {
        total_count=54;
        return e_success;
      }
   }
   else
   return e_failure;
  // printf("f = %d",1);
}

Status encode_byte_to_lsb(char data, char *image_buffer,int left,int right)
{
     int shift=7; 
     for(int i=left;i<right;i++)
     {
        //printf("image_buffer[%d] = %d\n",i,image_buffer[i]);
        image_buffer[i] = (image_buffer[i]>>1)<<1;
        image_buffer[i] = ((data&1<<shift)>>shift)|image_buffer[i];
        shift--;
         //printf("%d ",image_buffer[i]);
     }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char arr[16];
    //fseek(encInfo->fptr_src_image,54,SEEK_SET);
    fread(arr,sizeof(char),16,encInfo->fptr_src_image);
    int i=0;
    int size=0;
    while(*(magic_string+i))
    {
        char data=*(magic_string+i);
        encode_byte_to_lsb(data,arr,size,size+8);
        size +=8;
        i++;
    }
    if(fwrite(arr,sizeof(char),16,encInfo->fptr_stego_image)==16)
    {
        total_count += 16;
       return e_success;
    }
    else
    return e_failure;

}

Status encode_secret_file_extn_length(int len, EncodeInfo *encInfo)
{
     char arr[32]={0};
     //printf("%d\n",ftell(encInfo->fptr_src_image));
     fread(arr,sizeof(char),32,encInfo->fptr_src_image);
     //printf("%d\n",ftell(encInfo->fptr_src_image));
     //printf("\n");
     for(int i=0;i<32;i++)
     {
       // printf("arr[%d] = %d\n",i,arr[i]);
       arr[i] = ((arr[i]>>1)<<1);
       arr[i] = ((len&(1<<(31-i)))>>(31-i))|arr[i];
      // printf("%d ",arr[i]);
     }
   //
  // printf("\nftell->%d\n",ftell(encInfo->fptr_stego_image));
   if(fwrite(arr,1,32,encInfo->fptr_stego_image)==32){
    //printf("%d\n",ftell(encInfo->fptr_stego_image));
   total_count+=32;
   return e_success;
   }
}

Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo)
{
    int n = strlen(file_extn);
    //printf("length of extention %d\n",n);
    char arr[n*8];
    //printf("%d\n",ftell(encInfo->fptr_src_image));
    fread(arr,sizeof(char),n*8,encInfo->fptr_src_image);
    // printf("%d\n",ftell(encInfo->fptr_src_image));
    int temp=0;
    int num=0;
    for(int i=0;i<n;i++)
    {
        
        for(int j=0;j<=7;j++)
        {
           // printf("arr[%d] = %d\n",j+temp,arr[j+temp]);
           arr[j+temp] = ((arr[j+temp]>>1)<<1);
           //printf("%d\n",(((file_extn[i]&(1<<(7-j)))>>(7-j))));
           arr[j+temp] = ((file_extn[i]&1<<(7-j))>>(7-j))|arr[j+temp];   
          //printf("%d ",arr[j+temp]);
        }
        temp += 8;
        //printf("%c\n",file_extn[i]);
    }
   
    if(fwrite(arr,sizeof(char),n*8,encInfo->fptr_stego_image))
    {
        total_count +=8*n;
        return e_success;
    }
    return e_failure;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char arr[32]={0};

     //printf("\n\n%d\n\n",file_size);
     fread(arr,sizeof(char),32,encInfo->fptr_src_image);
     for(int i=0;i<32;i++)
     {
       // printf("arr[%d] = %d\n",i,arr[i]);
       arr[i] = ((arr[i]>>1)<<1);
       //printf("%d ",((file_size&(1<<(31-i)))>>(31-i)));
       arr[i] = ((file_size&(1<<(31-i)))>>(31-i))|arr[i];
       //printf("arr[%d](after) = %d\n",i,arr[i]);
     }
   
   if(fwrite(arr,sizeof(char),32,encInfo->fptr_stego_image)==32){
   total_count+=32;
   return e_success;
   }
   return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    
    char ch;
    char arr[8];
    int count=0;
    int count_out = 0;
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    while((ch=fgetc(encInfo->fptr_secret))!=EOF)
    {   
       
       
         // printf("\n%c\n",ch);
        fread(arr,sizeof(char),8,encInfo->fptr_src_image);
        //printf("\n%c\n",ch);
        int i=0;
        count_out++;
        while(i<8)
        {
           arr[i] =  ((arr[i]>>1)<<1); 
           arr[i] = ((ch&1<<(7-i))>>(7-i))|arr[i];
           i++;
        }
        fwrite(arr,sizeof(char),8,encInfo->fptr_stego_image);
       
    }
    if(count_out==count/8)
    {   
        return e_success;
    }

}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch [1000];
     int size;
    while((size=fread(&ch,sizeof(char),1000,fptr_src))>0)
    {
        fwrite(&ch,sizeof(char),size,fptr_dest);
    }
   while(getc(fptr_src)== EOF)
   {
    return e_success;
   }
   return e_failure;
}

Status do_encoding(EncodeInfo *encInfo,char*argv[])
{
    if(open_files(encInfo)==e_success)
    {
        sleep(1);
         printf("\e[0;36mINFO : all files opened succesfully\n \n\e[0m");
       sleep(1);
        if(check_capacity(encInfo,argv)==e_success)
        { 
            printf("\e[0;36mINFO : capacity checking succesful\n \n\e[0m");
           sleep(1);
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
            { 
                printf("\e[0;36mINFO : bmp header copied successfully\n \n\e[0m");
                sleep(1);
                char magic_string[]=MAGIC_STRING;
               if(encode_magic_string(magic_string,encInfo)==e_success)
               {
                 printf("\e[0;36mINFO : magic string entered sucessfully\n \n\e[0m");
                  sleep(1);
                   char * file_extn = strstr(encInfo->secret_fname,".");
                   int len = strlen(file_extn);
                   //printf("\ntotal_count-> %d\n",total_count);
                  if(encode_secret_file_extn_length(len, encInfo)==e_success)
                  { 
                    printf("\e[0;36mINFO : encoding of secret file extention length successful\n \n\e[0m");
                    sleep(1);
                     if(encode_secret_file_extn(file_extn , encInfo)==e_success)
                     { 
                        printf("\e[0;36mINFO : encoding of secret file extention successful\n \n\e[0m");
                        sleep(1);
                          encInfo->fptr_secret = fopen(encInfo->secret_fname,"r");
                          int file_size=0;
                          fseek(encInfo->fptr_secret,0,SEEK_END);
                             file_size = ftell(encInfo->fptr_secret);
                          if(encode_secret_file_size(file_size,encInfo)==e_success)
                          { 
                            printf("\e[0;36mINFO : encoding of secret file size successful\n \n\e[0m");                          
                                 sleep(1);
                              if(encode_secret_file_data(encInfo)==e_success)
                              { 
                                printf("\e[0;36mINFO : secret message has been encoded\n \e[0m");                                printf("\n");
                                sleep(1);
                               if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
                                {
                                    printf("\e[0;36mINFO : remaning image data has been copied to : \e[0m %s\n\n",encInfo->stego_image_fname);
                                   sleep(1);
                                   return e_success;
                                }
                                else{
                                     printf("\e[0;31mINFO : remaning image data has been coping failed\n\e[0m\n");
                                    return e_failure;
                                }
                              }
                              else{
                                 printf("\e[0;31mINFO : secret message failed to encoded\n\e[0m\n");
                                return e_failure;
                              }
                          }
                          else{
                             printf("\e[0;31mINFO : encoding of secret file size failed\n\e[0m\n");
                            return e_failure;
                          }
                     }
                     else{
                        printf("\e[0;31mINFO : encoding of secret file extention failed\n\e[0m\n");
                        return e_failure;
                     }
                  }
                  else{
                    printf("\e[0;31mINFO : encoding of secret file extention length failed\n\e[0m\n");
                    return e_failure;
                  }
               }
               else{
                printf("\e[0;31mINFO : failed to enter magic string\n\e[0m\n");
                return e_failure;
               }
            }
            else{
                printf("\e[0;31mINFO : failed to copy bmp header\n\e[0m\n");
                return e_failure;
            }
        }
        else
        {
            printf("\e[0;31mINFO : bmp file doesnt have required size\n\e[0m\n");
            return e_failure;
        }
    }
    else{
        printf("\e[0;31mINFO : failed to open all files\n\e[0m\n");
       return e_failure;
    }
}
