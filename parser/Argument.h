#pragma once

#include "string"

using var = std::variant<int, bool, std::string>;


namespace ArgumentParser {

    class Argument {
        friend class ArgParser;
    public:
        explicit Argument(std::string argument_name);

        Argument(char n, std::string argument_name);

        Argument(char n, std::string argument_name, std::string logic);

        Argument& Default(var definition);

        Argument& Positional();

        Argument& StoreValue(int& stored);

        Argument& StoreValue(std::string& stored);

        Argument& StoreValue(bool& stored);

        Argument& StoreValues(std::vector<int>& stored);

        Argument& StoreValues(std::vector<std::string>& stored);

        Argument& MultiValue();

        Argument& MultiValue(size_t count);

        std::string argument_long_name_ = " ";
        std::vector<std::string>* multi_string_stored_;
    private:
        char argument_short_name_ = ' ';
        std::string logic_ = " ";
        char argument_type_ = ' ';
        var definition_;
        std::vector<var> multi_definition_;
        bool is_default_ = false;
        bool is_positional_ = false;
        bool is_unary_stored_ = false;
        bool is_multi_stored_ = false;
        bool is_multi_ = false;
        uint64_t min_args_count_ = 0;
        var* stored_;
        std::vector<int>* multi_int_stored_;
    };
}