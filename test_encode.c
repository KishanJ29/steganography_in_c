#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
  if(check_operation_type(argv) == e_encode)
  {
  	printf("Selected encoding\n");
  	EncodeInfo structure_for_encoding;
  	if(read_and_validate_encode_args(argv, &structure_for_encoding) == e_success)
  	{
		printf("Read and Validate encode arguments is sucessfull\n");
		if(do_encoding(&structure_for_encoding) ==e_success)
			printf("Encoding is successful\n");
		else
			printf("Encoding is not successful\n");
	}
	else
	{
		if(do_encoding(&structure_for_encoding) ==e_success)
			printf("Encoding is successful\n");
		printf("Read and Validate encode arguments is NOT sucessfull\n");
	}
  }
    else if (check_operation_type(argv) == e_decode)
    {
        printf("Selected decoding\n");

        // Declare structure variables
        DecodeInfo decInfo;
        if (read_and_validate_decode_args(argv, &decInfo) == d_success)
        {
            printf("Read and validate decode arguments is a success\n");

            // Function call for do decoding
            if (do_decoding(&decInfo) == d_success)
            {
                printf("Decoding is successful\n");
            }
            else
            {
                printf("Decoding is NOT successful\n");
            }
        }  
        else
        {
            printf("Read and validate decode arguments is a failure\n");
        }
    } 
  else 
  	printf("Invalid argument");
	
}

OperationType check_operation_type(char *argv[])
{
  if(strcmp(argv[1], "-e") ==0)
  	return e_encode;
  else if (strcmp(argv[1], "-d") ==0)
  	return e_decode;
  else
  	return e_unsupported;
}	
