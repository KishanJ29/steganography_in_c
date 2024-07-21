#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
// /* Function Definitions */

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
Status open_files(DecodeInfo *decoinfo)
{
    // Src Image file
    
    // printf("\nthe value is give here \n%s",decoinfo->src_image_fname);
    decoinfo->fptr_src_image = fopen(decoinfo->src_image_fname, "r");
    // Do Error handling
    if (decoinfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file Source File %s\n", decoinfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    decoinfo->fptr_secret = fopen(decoinfo->secret_fname, "r");
    // Do Error handling
    if (decoinfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decoinfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    decoinfo->fptr_stego_image = fopen(decoinfo->stego_image_fname, "w");
    // Do Error handling
    if (decoinfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decoinfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[],DecodeInfo *decoinfo)
{
   if(strcmp(strstr(argv[2],"."),".bmp")==0)
   {
      // printf("\nCHECK THERE %s\n",argv[2]);
      decoinfo -> src_image_fname = argv[2];
     }
     else
      return e_failure;
      
   if(strcmp(strstr(argv[3],"."),".txt")==0)
   {
    //  printf("\nCHECK THERE secret %s\n",argv[3]);
     decoinfo -> secret_fname = argv[3];
     }
     else
      return e_failure;
      
   if(argv[4] != NULL)
      decoinfo -> stego_image_fname = argv[4];
    else
      decoinfo -> stego_image_fname = "stego.bmp";

    // printf("\nCHECK THERE source %s\n",decoinfo -> src_image_fname);
    // printf("\nCHECK THERE source %s\n",decoinfo -> secret_fname);
    // printf("\nCHECK THERE source %s\n",decoinfo -> stego_image_fname);


  return e_success;    
 }
Status check_capacity(DecodeInfo *decoinfo)
{
	decoinfo -> image_capacity =	get_image_size_for_bmp(decoinfo -> fptr_src_image); //sizepf beautiful.bmp
	decoinfo -> size_secret_file = get_file_size(decoinfo -> fptr_secret); //size of secret.txct
  // printf("\nImage cap %lu",decoinfo -> image_capacity);
  // printf("\n Secrete cap %lu",decoinfo -> size_secret_file);
	if(decoinfo -> image_capacity> (54 +16 +32 + 32 + (decoinfo -> size_secret_file*8)))
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
Status encode_magic_string( char *magic_string, DecodeInfo *decoinfo)
{
  encode_data_to_image(magic_string, strlen(magic_string),decoinfo);
  return e_success;
}

Status encode_data_to_image(char *data, int size, DecodeInfo *decoinfo)
{
  for(int i =0;i<size;i++){
    fread(decoinfo -> image_data,8,1, decoinfo -> fptr_src_image); //read 8 bytes of data from .bmp file
    encode_byte_to_lsb(data[i], decoinfo -> image_data); //hide 1 byte in 8 byte
    fwrite(decoinfo -> image_data , 8, 1, decoinfo -> fptr_stego_image); // write the encoded 8 bytes
    }

}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
  for(int i = 0; i< 8 ; i++)
  {
    image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7-i)) & 1); //clear the lsb of image byte and replace with data bit and subsequnetly substitue
  }
}

Status encode_secret_file_extn_size(int size, DecodeInfo *decoinfo)
{
  // encode a integer -> 32 bytes
  char str[32];
  fread(str,32,1,decoinfo -> fptr_src_image);
  encode_size_to_lsb(size,str);
  fwrite(str,32,1,decoinfo -> fptr_stego_image);
  return e_success;
}
Status encode_size_to_lsb(int size, char *image_buffer)
{
  for(int i=0;i <32;i++){
    image_buffer[i] = (image_buffer[i] & 0xFE) | ((size >> (31-i))&1);
  }
}

Status encode_secret_file_extn(char *file_extn, DecodeInfo *decoinfo)
{
  encode_data_to_image(file_extn,strlen(file_extn), decoinfo);
  return e_success;
}
Status encode_secret_file_size(int file_size, DecodeInfo *decoinfo)
{
  char str[32];
  fread(str,32,1,decoinfo -> fptr_src_image);
  encode_size_to_lsb(file_size,str);
  fwrite(str,32,1,decoinfo -> fptr_stego_image);
  return e_success;

}
Status encode_secret_file_data(DecodeInfo *decoinfo)
{
  fseek(decoinfo -> fptr_secret,0,SEEK_SET);
  char str[decoinfo ->size_secret_file];
  fread(str,decoinfo ->size_secret_file,1,decoinfo -> fptr_secret);
  encode_data_to_image(str,decoinfo -> size_secret_file,decoinfo);
  return e_success;
}

Status copy_remaining_img_data(DecodeInfo *decoinfo)
{
  char ch;
  while(fread(&ch, 1,1,decoinfo -> fptr_src_image) > 0 )
  {
    fwrite(&ch, 1,1, decoinfo -> fptr_stego_image);
    return e_success;
  }
}

Status do_decoding(DecodeInfo *decoinfo)
{
	//rest of all func calls will be made here
	if (open_files(decoinfo) == e_success)
	{
		printf("Successfully openned all files \n");
		if( check_capacity(decoinfo) == e_success)
		{
			printf("capcity check done\n");
      if(copy_bmp_header(decoinfo ->fptr_src_image, decoinfo ->fptr_stego_image)==e_success)
      {
        printf("Copy bmp header is a sucess\n");
        if(encode_magic_string(MAGIC_STRING,decoinfo) == e_success)
        {
          printf("Magic String Encodded SUccessfully\n");
          strcpy(decoinfo -> extn_secret_file, strstr(decoinfo -> secret_fname, "."));
          if(encode_secret_file_extn_size(strlen(decoinfo -> extn_secret_file),decoinfo) == e_success)
          {
            printf("Encoding secret file extn size is a success\n");
            if(encode_secret_file_extn(decoinfo-> extn_secret_file,decoinfo)==e_success)
            {
              printf("Encoded secret file extn successfuly\n");
              if(encode_secret_file_size(decoinfo -> size_secret_file, decoinfo) == e_success)
              {
                printf("Encoded secret file size successufully\n");
                if(encode_secret_file_data(decoinfo)==e_success)
                {
                  printf("Encoded secret file data successfully\n");
                  if(copy_remaining_img_data(decoinfo) ==e_success)
                  {
                    printf("Copied remaining Image bytes successfully\n");
                  }
                  else
                  {
                    printf("Copied remaining Image bytes FAILED");
                  }
                }
                else{
                  printf("Encoded secret file data FAILED\n");
                }
              }
              else{
                printf("ENcode secret file size NOT Success\n");
                return e_failure;
              }
            }
            else
            {
              printf("Encoding secret file exten Failed\n");
            }
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
