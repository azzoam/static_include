#include <stdio.h>
#include <string.h>

typedef int Bool;
#define True 1
#define False 0

static Bool write_blob_to_file(char *in_filename, char *out_filename, char *data_name)
{
    if (!data_name) {
        printf("error:  data_name must be supplied to write_blob_to_file.\n");
        return False;
    }

    FILE *in_handle;
    if (in_filename) {
        /* NOTE: The read mode MUST be "rb" on Windows, or else a 0x1a byte triggers EOF.  Interesting. */
        in_handle = fopen(in_filename, "rb");
    } else {
        in_handle = stdin;
    }

    if (in_filename && (!in_handle)) {
        printf("error:  unable to open input file %s.\n", in_filename);
        return False;
    }

    FILE *out_handle;
    if (out_filename) {
        /* NOTE: The write mode MUST be "wb" on Windows. */
        out_handle = fopen(out_filename, "wb");
    } else {
        out_handle = stdout;
    }

    if (out_filename && (!out_handle)) {
        if (in_filename) {
            fclose(in_handle);
        }
        printf("error:  unable to open output file %s.\n", out_filename);
        return False;
    }

    fprintf(out_handle, 
            "/* Autogenerated by static_include v0.1\n"
            " * ================================================\n"
            " * Author: Alexander Azzo <azzoa@vcu.edu>\n"
            " * All rights reserved.\n"
            " */\n\n");
    fprintf(out_handle, "const unsigned char %s[] = {", data_name);
    unsigned char c;
    unsigned int count = 0;
    while (fread((void *)&c, 1, 1, in_handle)) {
        if (count % 8 == 0) {
            if (count % 16 == 0) {
                if (count != 0) {
                    fprintf(out_handle, ",");
                }
                fprintf(out_handle, "\n/* 0x%08x: */ ", count);
            } else {
                fprintf(out_handle, ",  ");
            }
        } else {
            fprintf(out_handle, ", ");
        }
        fprintf(out_handle, "0x%02x", c);
        count++;
    }

    fprintf(out_handle, "\n};\n");

    if (in_filename) {
        fclose(in_handle);
    }
    if (out_filename) {
        fclose(out_handle);
    }

    return True;
}

static void print_help(void)
{
    printf("static_include v0.1 - Bake binary files into c byte array.\n"
           "\n"
           "Usage: static_include -i [input file] -o [output file] -n [array name]\n");
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_help();
        return 0;
    }

    char *in_filename = 0;
    char *out_filename = 0;
    char *data_name = 0;
    int i = 1;

    while ((i + 1) < argc) {
        char *arg = argv[i];

        if (strcmp(arg, "-i") == 0) {
            in_filename = argv[i + 1];
            i++;

        } else if (strcmp(arg, "-o") == 0) {
            out_filename = argv[i + 1];
            i++;

        } else if (strcmp(arg, "-n") == 0) {
            data_name = argv[i + 1];
            i++;

        } else {
            print_help();
            return 0;
        }
        i++;
    }

    if (in_filename && data_name) {
        write_blob_to_file(in_filename, out_filename, data_name);
    } else {
        print_help();
    }

    return 0;
}


