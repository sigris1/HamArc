#include "valarray"
#include "HammingDecryptor.h"
#include "HammingEncoder.h"

std::vector<bool> Decrypt(uint64_t length, uint64_t shift, std::vector<bool> now){
    std::vector<bool> returned;
    uint32_t block = SetAmountAddingBits(shift);
    uint32_t pos = 0;
    uint32_t current_pos = 0;
    for (uint64_t j = 0; j < length; ++j){
        if (pos == pow(2, block - 1) - 1){
            pos = 0;
        }
        bool check = false;
        for (uint64_t k = 0; k < block; ++k){
            if (pos == pow(2, k) - 1){
                pos++;
                check = true;
                break;
            }
        }
        if(check){
            continue;
        }
        returned.push_back(now[j]);
        pos++;
        current_pos++;
    }
    return returned;
}