//
// Created by Danya Smelskiy on 31.10.2021.
//

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "CP1251.h"
#include "koi8.h"
#include "iso8859-5.h"
#include "utf8.h"

int GetNumberOfBits(int c) {
    int res = 0;
    while (c) {
        c >>= 1;
        ++res;
    }
    return res ? res : 1;
}

void PrintUTF8CodeToFile(FILE* output_file, int utf8) {
    const int len = GetNumberOfBits(utf8);
    const int number_of_bytes = (len + 7) / 8;

    for (int i = number_of_bytes - 1; i >= 0; --i) {
        int cur_byte = (utf8 >> (i * 8));  // get the highest byte
        fputc((char)cur_byte, output_file);
        utf8 ^= (cur_byte << (i * 8));  // remove the highest byte
    }
}

int ConvertFromCP1251(FILE* input_file, FILE* output_file) {
    InitCP1251Encoding();

    PrintUTF8CodeToFile(output_file, kUTF8BOM);

    int c;
    while ((c = getc(input_file)) != EOF) {
        int uni_c = ConvertCP1251ToUnicode(c);
        int utf8_c = ConvertUnicodeToUTF8(uni_c);
        if (utf8_c == -1) {
            return -1;
        }
        PrintUTF8CodeToFile(output_file, utf8_c);
    }

    return 0;
}

int ConvertFromKOI8(FILE* input_file, FILE* output_file) {
    InitISO8859Encoding();

    PrintUTF8CodeToFile(output_file, kUTF8BOM);

    int c;
    while ((c = getc(input_file)) != EOF) {
        int uni_c = ConvertKOI8ToUnicode(c);
        int utf8_c = ConvertUnicodeToUTF8(uni_c);
        if (utf8_c == -1) {
            return -1;
        }
        PrintUTF8CodeToFile(output_file, utf8_c);
    }

    return 0;
}

int ConvertFromISO8859(FILE* input_file, FILE* output_file) {
    PrintUTF8CodeToFile(output_file, kUTF8BOM);

    InitISO8859Encoding();

    int c;
    while ((c = getc(input_file)) != EOF) {
        int uni_c = ConvertISO8859ToUnicode(c);
        int utf8_c = ConvertUnicodeToUTF8(uni_c);
        if (utf8_c == -1) {
            return -1;
        }
        PrintUTF8CodeToFile(output_file, utf8_c);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Incorrect number of input arguments. "
               "Program requires exactly 3 arguments:\n"
               "\t - the path to the input file;\n"
               "\t - the input encoding;\n"
               "\t - the path to the output file;\n");
        return 1;
    }

    const char* const input_path = argv[1];
    printf("The input file path: %s\n", input_path);

    FILE* input_file;
    input_file = fopen(input_path, "rb");
    if (input_path == NULL) {
        perror("Can't open the input file");
        return errno;
    }

    const char* const encoding = argv[2];
    printf("The input file encoding: %s\n", encoding);

    const char* const output_path = argv[3];

    FILE* output_file;
    output_file = fopen(output_path, "wb");
    if (output_file == NULL) {
        perror("Can't create or open the output file");
        return errno;
    }

    int successful = 1;
    if (strcmp(encoding, "cp-1251") == 0) {
        if (ConvertFromCP1251(input_file, output_file)) {
            successful = 0;
        }
    } else if (strcmp(encoding, "koi8-r") == 0) {
        if (ConvertFromKOI8(input_file, output_file)) {
            successful = 0;
        }
    } else if (strcmp(encoding, "iso-8859-5") == 0) {
        if (ConvertFromISO8859(input_file, output_file)) {
            successful = 0;
        }
    } else {
        printf("Incorrect encoding. "
               "You can only select one of the following encodings: cp-1251, koi8-r, iso8859-5\n");
        return 1;
    }

    fclose(output_file);

    if (successful) {
        printf("The file is successfully converted to the %s encoding and saved to the file: %s\n",
               encoding, output_path);
    } else {
        printf("Failed to convert the file.\n");
        return 1;
    }

    return 0;
}
