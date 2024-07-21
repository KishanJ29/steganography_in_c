#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include <string.h> // for string comparisions of args
#include <stdio.h>
/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], DecodeInfo *decoInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decoInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(DecodeInfo *decoInfo);

/* check capacity */
Status check_capacity(DecodeInfo *decoInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status decode_magic_string( char *magic_string, DecodeInfo *decoInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn_size(int size, DecodeInfo *decoInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn( char *file_extn, DecodeInfo *decoInfo);

/* Encode secret file size */
Status decode_secret_file_size(int file_size, DecodeInfo *decoInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decoInfo);

/* Encode function, which does the real encoding */
Status decode_data_to_image(char *data, int size, DecodeInfo *decoInfo);

/* Encode a byte into LSB of image data array */
Status decode_byte_to_lsb(char data, char *image_buffer);

/*  Encode an integer to the LSB*/
Status decode_size_to_lsb(int size,char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(DecodeInfo *enchInfo);

#endif
