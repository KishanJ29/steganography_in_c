#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
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
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

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
    
    // printf("\nthe value is give here \n%s",encInfo->src_image_fname);
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file Source File %s\n", encInfo->src_image_fname);

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
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
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
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
   if(strcmp(strstr(argv[2],"."),".bmp")==0)
   {
      // printf("\nCHECK THERE %s\n",argv[2]);
      encInfo -> src_image_fname = argv[2];
     }
     else
      return e_failure;
      
   if(strcmp(strstr(argv[3],"."),".txt")==0)
   {
    //  printf("\nCHECK THERE secret %s\n",argv[3]);
     encInfo -> secret_fname = argv[3];
     }
     else
      return e_failure;
      
   if(argv[4] != NULL)
      encInfo -> stego_image_fname = argv[4];
    else
      encInfo -> stego_image_fname = "stego.bmp";

    // printf("\nCHECK THERE source %s\n",encInfo -> src_image_fname);
    // printf("\nCHECK THERE source %s\n",encInfo -> secret_fname);
    // printf("\nCHECK THERE source %s\n",encInfo -> stego_image_fname);


  return e_success;    
 }
 
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo -> image_capacity =	get_image_size_for_bmp(encInfo -> fptr_src_image); //sizepf beautiful.bmp
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret); //size of secret.txct
  // printf("\nImage cap %lu",encInfo -> image_capacity);
  // printf("\n Secrete cap %lu",encInfo -> size_secret_file);
	if(encInfo -> image_capacity> (54 +16 +32 + 32 + (encInfo -> size_secret_file*8)))
		return e_success;
	else
		return e_failure;		
}

uint get_file_size(FILE *fptr)
{
	fseek(fptr, 0 , SEEK_END);
	return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
  char str[54];
  fseek(fptr_src_image,0,SEEK_SET);
  fread(str,54,1,fptr_src_image);
  fwrite(str,54,1,fptr_dest_image);
  return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
  encode_data_to_image(magic_string, strlen(magic_string),encInfo);
  return e_success;
}

Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
  for(int i =0;i<size;i++){
    fread(encInfo -> image_data,8,1, encInfo -> fptr_src_image); //read 8 bytes of data from .bmp file
    encode_byte_to_lsb(data[i], encInfo -> image_data); //hide 1 byte in 8 byte
    fwrite(encInfo -> image_data , 8, 1, encInfo -> fptr_stego_image); // write the encoded 8 bytes
    }

}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
  for(int i = 0; i< 8 ; i++)
  {
    image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7-i)) & 1); //clear the lsb of image byte and replace with data bit and subsequnetly substitue
  }
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
  // encode a integer -> 32 bytes
  char str[32];
  fread(str,32,1,encInfo -> fptr_src_image);
  encode_size_to_lsb(size,str);
  fwrite(str,32,1,encInfo);
  return e_success;
}
Status encode_secret_to_lsb(int size, char *image_buffer)
{
  for(int i=0;i <32;i++){
    image_buffer[i] = (image_buffer[i] & 0xFE) | ((size >> (31-i))&1);
  }
}
Status do_encoding(EncodeInfo *encInfo)
{
	//rest of all func calls will be made here
	if (open_files(encInfo) == e_success)
	{
		printf("Successfully openned all files \n");
		if( check_capacity(encInfo) == e_success)
		{
			printf("capcity check done\n");
      if(copy_bmp_header(encInfo ->fptr_src_image, encInfo ->fptr_stego_image)==e_success)
      {
        printf("Copy bmp header is a sucess\n");
        if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
        {
          printf("Magic String Encodded SUccessfully\n");
          strcpy(encInfo -> extn_secret_file, strstr(encInfo -> secret_fname, "."));
          if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file),encInfo) == e_success)
          {
            printf("Encoding secret file extn size is a success\n");
          }
          else{
            printf("Encoding secret file extn size failed\n");
            return e_failure;
          }
        }
        else{
          printf("Magic String NOT Encoded Successfully\n");
          return e_failure;
        }
      }
      else{
        printf("Copy bmp header is a failure\n");
        return e_failure;

      }
		}
		else
		{
			printf("capcity check failed \n");
			return e_failure;
		}
	}
	else 
	{
		printf("Successfully not openned all files \n");
		return e_failure;
	}
	return e_success;
}
