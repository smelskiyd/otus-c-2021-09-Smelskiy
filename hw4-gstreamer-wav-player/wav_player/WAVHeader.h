//
// Created by Danya Smelskiy on 11.12.2021.
//

#ifndef OTUSHW_WAVHEADER_H
#define OTUSHW_WAVHEADER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Little-endian RIFF marker
const uint32_t RIFF_MARK = 0x46464952;
// Little-endian WAVE marker
const uint32_t WAVE_MARK = 0x45564157;
// Little-endian FMT marker
const uint32_t FMT_MARK = 0x20746d66;
// Little-endian DATA marker
const uint32_t DATA_MARK = 0x61746164;

/*
 * The structure which describes WAV-file header
 */
#pragma pack(push, 1)
typedef struct {
    // WAV-format file starts with RIFF-header:
    // It contains symbols "RIFF" in ASCII encoding
    // (RIFF_MARK in little-endian representation)
    uint32_t chunk_id;

    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    uint32_t chunk_size;

    // It contains symbols "WAVE"
    // (WAVE_MARK in little-endian representation)
    uint32_t format;

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:

    // It contains symbols "fmt "
    // (FMT_MARK in little-endian representation)
    uint32_t subchunk1_id;

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    uint32_t subchunk1_size;

    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    uint16_t audio_format;

    // The number of channels: 1 for Mono, 2 for Stereo, etc
    uint16_t num_channels;

    // Sampling frequency. 8000 Hz, 44100 Hz etc
    uint32_t sample_rate;

    // sample_rate * num_channels * (bits_per_sample / 8)
    uint32_t byte_rate;

    // num_channels * (bits_per_sample / 8)
    // Количество байт для одного сэмпла, включая все каналы.
    uint16_t block_align;

    // Так называемая "глубиная" или точность звучания. 8-bit, 16-bit etc
    uint16_t bits_per_sample;

    // Подцепочка "data" содержит аудио-данные и их размер.

    // It contains symbols "data"
    // (DATA_MARK in little-endian representation)
    uint32_t subchunk2_id;

    // num_samples * num_channels * (bits_per_sample / 8)
    // Количество байт в области данных.
    uint32_t subchunk2_size;

    // Далее следуют непосредственно Wav данные.
} WAVHeader;
#pragma pack(pop)


WAVHeader* ReadWAVHeader(FILE* file) {
    if (file == NULL) {
        return NULL;
    }

    WAVHeader* wav_header = (WAVHeader*)malloc(sizeof(WAVHeader));
    if (fread(wav_header, sizeof(*wav_header), 1, file) != 1) {
        free(wav_header);
        return NULL;
    }

    return wav_header;
}

enum WAVHeaderValidationCode {
    OK,
    WrongChunkID,
    WrongFormat,
    WrongFMTMark,
    WrongDataMark,
};

enum WAVHeaderValidationCode ValidateWAVHeader(WAVHeader* wav_header) {
    if (wav_header->chunk_id != RIFF_MARK) {
        return WrongChunkID;
    }
    if (wav_header->format != WAVE_MARK) {
        return WrongFormat;
    }
    if (wav_header->subchunk1_id != FMT_MARK) {
        return WrongFMTMark;
    }
    if (wav_header->subchunk2_id != DATA_MARK) {
        return WrongDataMark;
    }

    return OK;
}

#endif //OTUSHW_WAVHEADER_H
