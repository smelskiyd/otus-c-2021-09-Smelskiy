#include <stdio.h>

#include "PKZip.h"

int main(int argc, char** argv) {
    // Validate input arguments
    if (argc != 2) {
        printf("Incorrect number of input arguments.\n");
        return 1;
    }

    char* file_path = argv[1];
    printf("The file path is %s\n", file_path);

    // Open the file
    FILE* input_file;
    input_file = fopen(file_path, "rb");
    if (input_file == NULL) {
        printf("Can't open the input file. Probably the path is wrong.\n");
        return 2;
    }

    // Determine the file size
    if (fseek(input_file, 0, SEEK_END) != 0) {
        printf("Can't fseek\n");
        return 3;
    }
    long file_size = ftell(input_file);
    printf("File size = %ld\n", file_size);

    // Check is there the end of the central directory record
    // If there are no ECDR then the file isn't PKZip
    // Otherwise it's PKZIP
    long start_of_the_ecdr = -1;
    for (long i = 0; i <= UINT16_MAX; ++i) {
        if (fseek(input_file, file_size - i - (long)sizeof(struct EndOfCentralDirectoryRecord), SEEK_SET) != 0) {
            continue;
        }

        struct EndOfCentralDirectoryRecord ecdr;
        if (fread(&ecdr, sizeof(ecdr), 1, input_file) != 1) {
            continue;
        }

        if (ecdr.signature == kECDRSignature) {
            start_of_the_ecdr = file_size - (long)sizeof(struct EndOfCentralDirectoryRecord) - i;
            break;
        }
    }

    if (start_of_the_ecdr == -1) {
        printf("Can't find the end of central directory record. The file isn't PKZip. \n");
        return 0;
    }

    printf("File has the end of central directory record. The file is PKZip.\n");

    // Print all file names in archive
    int it = 0;
    for (long i = 0; i < file_size; ++i) {
        fseek(input_file, i, SEEK_SET);
        struct CentralDirectoryFileHeader file_header;

        if (fread(&file_header, sizeof(file_header), 1, input_file) != 1) {
            continue;
        }

        if (file_header.signature == kCentralFileHeaderSignature) {
            char buffer[file_header.filenameLength + 1];
            fseek(input_file, i + (long)sizeof(struct CentralDirectoryFileHeader), SEEK_SET);
            fread(buffer, file_header.filenameLength, 1, input_file);
            buffer[file_header.filenameLength] = '\0';

            printf("File #%d: %s\n", ++it, buffer);
        }
    }

    return 0;
}
