#pragma once

#include "sstream"
#include "Argument.h"
#include "variant"
#include "vector"

using var = std::variant<int, bool, std::string>;

namespace ArgumentParser {
    class ArgParser {
    public:
        explicit ArgParser(std::string parser_name);
        bool Parse(const std::vector<std::string> &arguments);

        Argument& AddStringArgument(const std::string &string);
        Argument& AddStringArgument(char s, const std::string &string);
        Argument& AddStringArgument(char s, const std::string &string, const std::string &logic);
        std::string GetStringValue(const std::string &string);
        std::string GetStringValue(const std::string &string, uint64_t i);

        Argument& AddIntArgument(const std::string &string);
        Argument& AddIntArgument(char s, const std::string &string);
        Argument& AddIntArgument(char s, const std::string &string, const std::string &logic);
        uint64_t GetIntValue(const std::string &string);
        uint64_t GetIntValue(const std::string &string, uint64_t i);

        Argument& AddFlag(const std::string &string);
        Argument& AddFlag(char s, const std::string &string);
        Argument& AddFlag(char s, const std::string &string, const std::string &logic);
        bool GetFlag(const std::string &string);

        void AddHelp(char h, const std::string &string, const std::string &Description);
        bool Help() const;

        std::vector<Argument> commands_;
    private:
        std::vector<Argument> args_;
        std::string name_;
        char short_help_calling_ = ' ';
        std::string long_help_calling_;
        std::stringstream help_command_;
        bool is_help_ = false;

    };
}