#pragma once

#include "cstdint"
#include "vector"

std::vector<bool> Decrypt(uint64_t length, uint64_t shift, std::vector<bool> now);