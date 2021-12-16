#include <stdio.h>
#include <errno.h>

#include "wav_player/WAVHeader.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Incorrect number of input arguments. "
               "The program requires exactly 1 argument: the path to the input file.\n");
        return 1;
    }

    char* input_file_path = argv[1];
    FILE* input_file;
    input_file = fopen(input_file_path, "rb");
    if (input_file == NULL) {
        perror("Can't open the input file");
        return errno;
    }

    WAVHeader* wav_header = ReadWAVHeader(input_file);
    if (wav_header == NULL) {
        printf("Failed to read WAV header from file.\n");
        return 1;
    }

    {
        enum WAVHeaderValidationCode validation_code = ValidateWAVHeader(wav_header);
        if (validation_code != OK) {
            printf("Wrong format of WAV header file. Validation code: %d\n", (int)validation_code);
            free(wav_header);
            return 1;
        }
    }

    printf("WAV header is correct\n");

    if (wav_header->num_channels != 1) {
        printf("Input WAV-file has more than 1 channel.\n"
               "This plugin supports only Mono WAV-files.\n");
        return 1;
    }

    if (wav_header->audio_format != 1) {
        printf("Input WAV-file is compressed ('audio_format'=1 in WAV header).\n"
               "This plugin supports only uncompressed WAV-files.\n");
        return 1;
    }

    if (wav_header->bits_per_sample != 16) {
        printf("Input WAV-file isn't 16-bit per sample.\n"
               "This plugin supports only 16-bit WAV-files.\n");
        return 1;
    }

    printf("frequency %zu\n", wav_header->sample_rate);

    free(wav_header);
    return 0;
}
