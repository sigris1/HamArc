#include "fstream"
#include "iostream"
#include "valarray"
#include "ArchiveWriter.h"
#include "Archive.h"
#include "ArchiveReader.cpp"

void Write(ArgumentParser::ArgParser& parser, const std::string& file_path, std::ofstream& file, EncodedParameters& parameters){
    std::string now = CreateArchivePath(parser);
    uint64_t adding_bits = SetAmountAddingBits(parameters.shift);
    std::ifstream file1(file_path, std::ios::binary);
    std::vector<uint8_t> temp_array;
    char temp;
    while (file1 >> std::noskipws >> temp){
        temp_array.push_back(int(temp));
    }
    for (uint64_t i = 0; i < temp_array.size() / (parameters.final_encoded_length / kBitsInBite); ++i){
        bool cur[parameters.final_encoded_length];
        for (uint64_t j = 0; j < parameters.final_encoded_length; j++){
            cur[j] = false;
        }
        for (uint8_t j = 0; j < (parameters.final_encoded_length / kBitsInBite); ++j){
            CreateUnaryBinaryVersion(int(temp_array[i * (parameters.final_encoded_length / kBitsInBite) + j]) , cur, (j + 1) * kBitsInBite - 1);
        }

        bool current[parameters.final_coded_length];
        for (uint8_t j = 0; j < kBitsInBite; j++){
            std::vector<bool> pos;
            for (uint64_t  k = 0; k < (parameters.final_encoded_length / kBitsInBite); k++){
                pos.push_back(cur[(parameters.final_encoded_length / kBitsInBite) * j + k]);
            }
            std::vector<bool> def = Encode((parameters.final_encoded_length / kBitsInBite), parameters.shift, pos);
            for (uint64_t k = 0; k < (parameters.shift + adding_bits); ++k){
                current[j * (parameters.shift + adding_bits) + k] = def[k];
            }
        }
        for (uint64_t j = 0; j < (parameters.shift + adding_bits); j++){
            uint8_t number = 0;
            for (uint8_t k = 0; k < kBitsInBite; k++){
                if(current[kBitsInBite * j + k]){
                    number += pow(2, kMaxBlockIndex - k);
                }
            }
            file << number;
        }
    }
    if (temp_array.size() % (parameters.final_encoded_length / kBitsInBite) != 0){
        bool cur[parameters.final_encoded_length];
        for (uint64_t j = 0; j < parameters.final_encoded_length; j++){
            cur[j] = false;
        }
        for (uint64_t j = 0; j < (parameters.final_encoded_length / kBitsInBite); ++j){
            CreateUnaryBinaryVersion(int(temp_array[(temp_array.size() / (parameters.final_encoded_length / kBitsInBite)) * (parameters.final_encoded_length / kBitsInBite) + j]) , cur, (j + 1) * kBitsInBite - 1);
        }
        bool current[parameters.final_coded_length];
        for (uint64_t j = 0; j < kBitsInBite; j++){
            std::vector<bool> pos;
            for (uint64_t  k = 0; k < (parameters.final_encoded_length / kBitsInBite); k++){
                pos.push_back(cur[(parameters.final_encoded_length / kBitsInBite) * j + k]);
            }
            std::vector<bool> def = Encode((parameters.final_encoded_length / kBitsInBite), parameters.shift, pos);
            for (uint64_t k = 0; k < (parameters.shift + adding_bits); ++k){
                current[j * (parameters.shift + adding_bits) + k] = def[k];
            }
        }
        for (uint64_t j = 0; j < (parameters.shift + adding_bits); j++){
            uint8_t number = 0;
            for (uint64_t k = 0; k < kBitsInBite; k++){
                if(current[kBitsInBite * j + k]){
                    number += pow(2, kMaxBlockIndex - k);
                }
            }
            file << number;
        }
    }
}