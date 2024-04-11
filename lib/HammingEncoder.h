#pragma once

#include "vector"
#include "cstdint"

uint64_t SetAmountAddingBits(uint64_t encoded_block_length);

void SetRealAddingBitsValue(std::vector<bool> &checked_block, uint64_t amount_adding_bits, uint64_t length);

std::vector<bool> SetEncodedBits(std::vector<bool> original, uint64_t encoded_block_length);

std::vector<bool> Encode(uint64_t length, uint64_t shift, std::vector<bool> encoded);