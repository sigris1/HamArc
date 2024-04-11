#include "iostream"
#include "valarray"
#include "HammingEncoder.h"

using namespace std;

uint64_t SetAmountAddingBits(uint64_t encoded_block_length){
    uint64_t current_degree = 0;
    while (encoded_block_length > 0) {
        ++current_degree;
        encoded_block_length /= 2;
    }
    return current_degree;
}

void SetRealAddingBitsValue(std::vector<bool> &checked_block, uint64_t amount_adding_bits, uint64_t length){
    for (uint64_t i = 0; i < amount_adding_bits; ++i){
        uint64_t position = pow(2, i) - 1;
        uint64_t safety_position = position;
        bool temp = false;
        while (position < length){
            for (uint64_t j = 0; j < pow(2, i); ++j){
                if (position + j < length){
                    temp ^= checked_block[position + j];
                }
            }
            position += pow(2, i + 1);
        }
        checked_block[safety_position] = temp;
    }
}

std::vector<bool> SetEncodedBits(std::vector<bool> original, uint64_t encoded_block_length){
    uint64_t amount_adding_bits = SetAmountAddingBits(encoded_block_length);
    std::vector<bool> final_block;
    uint64_t adding_index = 0;
    uint64_t encoded_index = 0;
    for (uint64_t i = 0; i < amount_adding_bits + encoded_block_length; ++i){
        if (i == pow(2, adding_index) - 1){
            final_block.push_back(false);
            ++adding_index;
        } else {
            final_block.push_back(original[encoded_index]);
            encoded_index++;
        }
    }
    SetRealAddingBitsValue(final_block, amount_adding_bits, encoded_block_length + amount_adding_bits);
    return final_block;
}

std::vector<bool> Encode(uint64_t length, uint64_t shift, std::vector<bool> encoded){
    uint64_t shift_length = SetAmountAddingBits(shift);
    uint64_t block_count = length / shift;
    uint64_t block_length = shift + shift_length;
    std::vector<bool> now;
    for (uint64_t i = 0; i < block_count; ++i){
        std::vector<bool> current_now;
        for (uint64_t j = 0; j < shift; ++j){
            current_now.push_back(encoded[i * shift + j]);
        }
        std::vector<bool> temp_now = SetEncodedBits(current_now, shift);
        for (uint64_t j = 0; j < block_length; ++j){
            now.push_back(temp_now[j]);
        }
    }
    if (length % shift != 0){
        std::vector<bool> current_now;
        for (uint64_t i = length - (length % shift); i < length; ++i){
            current_now.push_back(encoded[i]);
        }
        uint64_t remain = length % shift;
        for (uint64_t j = current_now.size(); j < remain; ++j){
            current_now.push_back(false);
        }
        std::vector<bool> temp_now = SetEncodedBits(current_now, shift);
        for (uint64_t j = 0; j < block_length; ++j){
            now.push_back(temp_now[j]);
        }
    }
    return now;
}