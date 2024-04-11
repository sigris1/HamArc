#include "Argument.h"
#include "ArgParser.h"
#include "utility"

using var = std::variant<int, bool, std::string>;

namespace ArgumentParser {
    Argument::Argument(std::string argument_name)
            : argument_long_name_(std::move(argument_name))
    {}

    Argument::Argument(char n, std::string argument_name)
            : argument_short_name_(n), argument_long_name_(std::move(argument_name))
    {}

    Argument::Argument(char n, std::string argument_name, std::string logic)
            : argument_short_name_(n), argument_long_name_(std::move(argument_name)), logic_(std::move(logic))
    {}

    Argument& Argument::Default(var definition) {
        is_default_ = true;
        definition_ = std::move(definition);
        return *this;
    }

    Argument& Argument::Positional() {
        is_positional_ = true;
        return *this;
    }

    Argument& Argument::MultiValue() {
        is_multi_ = true;
        return *this;
    }

    Argument& Argument::MultiValue(size_t count) {
        is_multi_ = true;
        min_args_count_ = count;
        return *this;
    }

    Argument& Argument::StoreValue(std::string& stored) {
        is_unary_stored_ = true;
        std::string* mew = &stored;
        stored_ = (var*)mew;
        return *this;
    }

    Argument& Argument::StoreValue(int& stored) {
        is_unary_stored_ = true;
        int* mew = &stored;
        stored_ = (var*)mew;
        return *this;
    }

    Argument& Argument::StoreValue(bool& stored) {
        is_unary_stored_ = true;
        bool* mew = &stored;
        stored_ = (var*)mew;
        return *this;
    }

    Argument& Argument::StoreValues(std::vector<int>& stored) {
        is_multi_stored_ = true;
        multi_int_stored_ = &stored;
        return *this;
    }

    Argument& Argument::StoreValues(std::vector<std::string>& stored) {
        is_multi_stored_ = true;
        multi_string_stored_ = &stored;
        return *this;
    }
}