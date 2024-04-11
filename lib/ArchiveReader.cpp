#include "iostream"
#include "fstream"
#include "vector"
#include "ArchiveReader.h"
#include "Archive.h"
#include "HammingEncoder.h"

void CreateUnaryBinaryVersion(uint8_t value, bool binary_version[], uint8_t start_index){
    while (value > 0) {
        if (value % 2 == 1) {
            binary_version[start_index] = true;
        } else {
            binary_version[start_index] = false;
        }
        value /= 2;
        start_index--;
    }
}

void Read(ArgumentParser::ArgParser& parser, const std::string& unarchived_path, EncodedParameters& parameters){
    uint64_t adding_bits = SetAmountAddingBits(parameters.shift);
    std::string path = CreateArchivePath(parser);
    std::ifstream file2(path, std::ios::binary);
    std::ofstream file3(unarchived_path, std::ios::binary);
    std::vector<char> now;
    uint8_t temp;
    while (file2 >> std::noskipws >> temp){
        now.push_back(temp);
    }
    for (uint64_t i = 0; i < now.size() / (parameters.shift + adding_bits); ++i){
        bool cur[parameters.final_coded_length];
        for (uint64_t j = 0; j < parameters.final_coded_length; j++){
            cur[j] = 0;
        }
        for (uint64_t j = 0; j < (parameters.shift + adding_bits); ++j){
            CreateUnaryBinaryVersion(int(now[i * (parameters.shift + adding_bits) + j]), cur, (j + 1) * kBitsInBite - 1);
        }
        bool current[parameters.final_encoded_length];
        uint32_t pos = 0;
        uint32_t curpos = 0;
        for (uint64_t j = 0; j < parameters.final_coded_length; ++j){
            if (pos == pow(2, adding_bits) - 1){
                pos = 0;
            }
            bool check = false;
            for (uint64_t k = 0; k < adding_bits; ++k){
                if (pos == pow(2, k) - 1){
                    pos++;
                    check = true;
                    break;
                }
            }
            if(check){
                continue;
            }
            current[curpos] = cur[j];
            pos++;
            curpos++;
        }
        for (uint64_t j = 0; j < parameters.final_encoded_length / kBitsInBite; j++){
            uint8_t bite = 0;
            for (uint8_t k = 0; k < kBitsInBite; k++) {
                if (current[kBitsInBite * j + k]) {
                    bite += pow(2, kMaxBlockIndex - k);
                }
            }
            file3 << char(bite);
        }
    }
    if (now.size() % (parameters.shift + adding_bits) != 0){
        bool cur[parameters.final_coded_length];
        for (uint64_t j = 0; j < parameters.final_coded_length; j++){
            cur[j] = 0;
        }
        for (uint64_t j = 0; j < now.size() % (parameters.shift + adding_bits); ++j){
            CreateUnaryBinaryVersion(int(now[(now.size() /(parameters.shift + adding_bits)) * (parameters.shift + adding_bits) + j]), cur, (j + 1) * kBitsInBite - 1);
        }
        bool current[parameters.final_encoded_length];
        uint32_t pos = 0;
        uint32_t curpos = 0;
        for (uint64_t j = 0; j < parameters.final_coded_length; ++j){
            if (pos == pow(2, adding_bits) - 1){
                pos = 0;
            }
            bool check = false;
            for (uint64_t k = 0; k < adding_bits; ++k){
                if (pos == pow(2, k) - 1){
                    pos++;
                    check = true;
                    break;
                }
            }
            if(check){
                continue;
            }
            current[curpos] = cur[j];
            pos++;
            curpos++;
        }
        for (uint64_t j = 0; j < parameters.final_encoded_length / kBitsInBite; j++){
            uint8_t bite = 0;
            for (uint8_t k = 0; k < kBitsInBite; k++) {
                if (current[kBitsInBite * j + k]) {
                    bite += pow(2, kMaxBlockIndex - k);
                }
            }
            file3 << char(bite);
        }
    }
}