#include "string"
#include "ArgParser.h"
#include "iostream"

const uint64_t kCommandCharsShift = 2;
const uint64_t kShortFunctionValue = 3;
const uint64_t kStartAnalysisPosition = 0;

bool IsNumeric(const std::string& str, int q) {
    for (int i = q; i < str.length(); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

uint64_t GetPosition(std::string args) {
    uint64_t position = kCommandCharsShift;
    while (args[position] != '=') {
        ++position;
        if (position == args.size()) {
            break;
        }
    }
    return position;
}

namespace ArgumentParser {
    ArgParser::ArgParser(std::string parser_name)
            : name_{std::move(parser_name)}
    {}

    bool ArgParser::Parse(const std::vector<std::string>& arguments) {
        if (arguments.size() == 1 && args_.empty()){
            return true;
        }
        for (auto & arg : args_){
            if (arg.is_default_ || arg.is_positional_){
                commands_.emplace_back(arg);
            }
        }
        if (arguments.size() == 1 && !args_.empty()) {
            for (auto & arg : args_){
                if (arg.is_default_ ){
                    continue;
                } else {
                    return false;
                }
            }
        }
        for (uint64_t i = 1; i < arguments.size(); ++i){
            if(arguments[i][0] == '-'){
                if(arguments[i].size() == 1){
                    if (args_.empty() || args_.back().argument_type_ != 's'){
                        return false;
                    } else if (commands_.back().argument_type_ == 's'){
                        std::string minus =  "-";
                        if (commands_.back().is_positional_){
                            if (commands_.back().is_multi_){
                                if (commands_.back().is_multi_stored_){
                                    commands_.back().multi_string_stored_->emplace_back(minus);
                                }
                                commands_.back().multi_definition_.emplace_back(minus);
                            } else {
                                if (commands_.back().is_unary_stored_){
                                    *commands_.back().stored_ = minus;
                                }
                                commands_.back().definition_ = minus;
                            }
                        } else {
                            return false;
                        }
                    }
                } else if (arguments[i].size() > 1){
                    if (arguments[i][1] == '-'){
                        std::string now_value;
                        std::string now_parameter;
                        uint64_t position = GetPosition(arguments[i]);
                        if (position == arguments[i].size()){
                            now_parameter = arguments[i].substr(kCommandCharsShift, position - kCommandCharsShift);
                            if (is_help_) {
                                if (now_parameter == long_help_calling_) {
                                    std::cout << help_command_.str();
                                    return true;
                                }
                            }
                            bool checked = false;
                            for (auto & arg : args_){
                                if (arg.argument_long_name_ == now_parameter && (arg.is_positional_ || arg.is_default_ || arg.argument_type_ == 'b')){
                                    checked = true;
                                    commands_.emplace_back(arg);
                                    if (commands_.back().argument_type_ == 'b'){
                                        commands_.back().definition_ = true;
                                    }
                                    if (commands_.back().is_unary_stored_){
                                        *(bool*)commands_.back().stored_ = true;
                                    }
                                    break;
                                }
                            }
                            if (!checked){
                                return false;
                            } else {
                                continue;
                            }
                        } else if (position < arguments[i].size()){
                            now_parameter = arguments[i].substr(kCommandCharsShift, position - kCommandCharsShift);
                            now_value = arguments[i].substr(position + 1);
                            bool checked_multi = false;
                            for (auto & arg: commands_){
                                if (arg.argument_long_name_ == now_parameter) {
                                    checked_multi = true;
                                    if (arg.is_multi_){
                                        if (arg.argument_type_ == 's'){
                                            arg.multi_definition_.emplace_back(now_value);
                                            if (arg.is_multi_stored_) {
                                                arg.multi_string_stored_->emplace_back(now_value);
                                            }
                                        }
                                        if (commands_.back().argument_type_ == 'i'){
                                            if (IsNumeric(now_value, kStartAnalysisPosition)) {
                                                arg.multi_definition_.emplace_back(std::stoi(now_value));
                                                if (arg.is_multi_stored_) {
                                                    arg.multi_int_stored_->emplace_back(std::stoi(now_value));
                                                }
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            if (checked_multi){
                                continue;
                            }
                            bool checked = false;
                            for (auto & arg : args_){
                                if (arg.argument_long_name_ == now_parameter){
                                    checked = true;
                                    commands_.emplace_back(arg);
                                    if (commands_.back().argument_type_ == 's'){
                                        commands_.back().definition_ = now_value;
                                        if (commands_.back().is_multi_) {
                                            commands_.back().multi_definition_.emplace_back(now_value);
                                            if (commands_.back().is_multi_stored_) {
                                                commands_.back().multi_string_stored_->emplace_back(now_value);
                                            }
                                        }
                                        if (commands_.back().is_unary_stored_){
                                            *(std::string*)commands_.back().stored_ = now_value;
                                        }
                                    }
                                    if (commands_.back().argument_type_ == 'i'){
                                        var int_mew = std::stoi(now_value);
                                        if (IsNumeric(now_value, kStartAnalysisPosition)) {
                                            commands_.back().definition_ = std::stoi(now_value);
                                            if (commands_.back().is_multi_) {
                                                commands_.back().multi_definition_.emplace_back(std::stoi(now_value));
                                                if (commands_.back().is_multi_stored_) {
                                                    commands_.back().multi_int_stored_->emplace_back(std::stoi(now_value));
                                                }
                                            }
                                            if (commands_.back().is_unary_stored_){
                                                *(int*)commands_.back().stored_ = (std::stoi(now_value));
                                            }

                                        }
                                    }
                                    break;
                                }
                            }
                            if (!checked){
                                return false;
                            } else {
                                continue;
                            }
                        }
                    } else {
                        uint64_t position = GetPosition(arguments[i]);
                        if (position == arguments[i].size()) {
                            bool checked = false;
                            for (auto &arg: args_) {
                                if (arg.argument_long_name_ == arguments[i] && (arg.is_positional_ || arg.is_default_)) {
                                    commands_.emplace_back(arg);
                                    checked = true;
                                    break;
                                }
                            }
                            if (checked) {
                                continue;
                            }
                            uint64_t corrected = 1;
                            for (uint64_t j = 1; j < arguments[i].size(); ++j) {
                                for (auto &arg: args_) {
                                    if (arg.argument_type_ == 'b' && arg.argument_short_name_ == arguments[i][j]) {
                                        corrected++;
                                        commands_.emplace_back(arg);
                                        commands_.back().definition_ = true;
                                        if (commands_.back().is_unary_stored_) {
                                            *(bool*)commands_.back().stored_ = true;
                                        }
                                        break;
                                    }
                                }
                            }
                            if (corrected == arguments[i].size()){
                                continue;
                            } else {
                                return false;
                            }
                        } else if (arguments[i][2] != '='){
                            return false;
                        } else {
                            char now_parameter = arguments[i][1];
                            std::string now_value = arguments[i].substr(kShortFunctionValue);
                            bool checked = false;
                            for (auto & arg : args_){
                                if (arg.argument_short_name_ == now_parameter){
                                    checked = true;
                                    commands_.emplace_back(arg);
                                    if (commands_.back().argument_type_ == 's'){
                                        commands_.back().definition_ = now_value;
                                        if (commands_.back().is_multi_){
                                            commands_.back().multi_definition_.emplace_back(now_value);
                                            if (commands_.back().is_multi_stored_){
                                                commands_.back().multi_string_stored_->emplace_back(now_value);
                                            }
                                        }
                                    }
                                    if (commands_.back().argument_type_ == 'i'){
                                        var int_mew = std::stoi(now_value);
                                        if (IsNumeric(now_value, kStartAnalysisPosition)) {
                                            commands_.back().definition_ = std::stoi(now_value);
                                            if (commands_.back().is_multi_) {
                                                commands_.back().multi_definition_.emplace_back(std::stoi(now_value));
                                                if (commands_.back().is_multi_stored_) {
                                                    commands_.back().multi_int_stored_->emplace_back(std::stoi(now_value));
                                                }
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            if (!checked){
                                return false;
                            } else {
                                continue;
                            }
                        }
                    }
                }
            } else {
                if (commands_.back().is_positional_){
                    std::string mew = arguments[i];
                    var mewmew = mew;
                    if (commands_.back().argument_type_ == 's'){
                        if (commands_.back().is_multi_){
                            commands_.back().multi_definition_.emplace_back(arguments[i]);
                            if (commands_.back().is_multi_stored_){
                                commands_.back().multi_string_stored_->emplace_back(arguments[i]);
                            }
                        }
                    }
                    if (commands_.back().argument_type_ == 'i'){
                        if (IsNumeric(arguments[i], kStartAnalysisPosition)) {
                            var int_mew = std::stoi(mew);
                            if (commands_.back().is_multi_) {
                                commands_.back().multi_definition_.emplace_back(std::stoi(arguments[i]));
                                if (commands_.back().is_multi_stored_) {
                                    commands_.back().multi_int_stored_->emplace_back(std::stoi(arguments[i]));
                                }
                            }
                        }
                    }
                } else {
                    return false;
                }
            }
        }
        for (auto & arg : commands_){
            if (arg.is_multi_stored_ && arg.min_args_count_ > arg.multi_definition_.size()){
                return false;
            }
        }
        return true;
    }

    Argument &ArgParser::AddStringArgument(const std::string &string) {
        Argument now = Argument(string);
        now.argument_type_ = 's';
        args_.emplace_back(now);
        return args_.back();
    }

    Argument &ArgParser::AddStringArgument(char s, const std::string &string) {
        Argument now = Argument(s,string);
        now.argument_type_ = 's';
        args_.emplace_back(now);
        return args_.back();
    }

    Argument &ArgParser::AddStringArgument(char s, const std::string &string, const std::string &logic) {
        Argument now = Argument(s,string, logic);
        now.argument_type_ = 's';
        args_.emplace_back(now);
        return args_.back();
    }

    std::string ArgParser::GetStringValue(const std::string &string) {
        for (const auto &arg: commands_) {
            if (arg.argument_long_name_ == string) {
                return (std::string&)(arg.definition_);
            }
        }
        return "";
    }

    std::string ArgParser::GetStringValue(const std::string &string, uint64_t i) {
        for (const auto &arg: commands_) {
            if (arg.argument_long_name_ == string) {
                if(i < arg.multi_definition_.size()) {
                    return (std::string &)(arg.multi_definition_[i]);
                }
            }
        }
        return "";
    }

    Argument& ArgParser::AddIntArgument(const std::string &string) {
        Argument now = Argument(string);
        now.argument_type_ = 'i';
        args_.emplace_back(now);
        return args_.back();
    }

    Argument& ArgParser::AddIntArgument(char s, const std::string &string) {
        Argument now = Argument(s,string);
        now.argument_type_ = 'i';
        args_.emplace_back(now);
        return args_.back();
    }

    Argument& ArgParser::AddIntArgument(char s, const std::string &string, const std::string &logic) {
        Argument now = Argument(s,string, logic);
        now.argument_type_ = 'i';
        args_.emplace_back(now);
        return args_.back();
    }

    uint64_t ArgParser::GetIntValue(const std::string &string) {
        for (const auto &arg: commands_) {
            if (arg.argument_long_name_ == string) {
                return (int &)(arg.definition_);
            }
        }
        return 0;
    }

    uint64_t ArgParser::GetIntValue(const std::string &string, uint64_t i) {
        for (const auto &arg: commands_) {
            if (arg.argument_long_name_ == string) {
                if(i < arg.multi_definition_.size()) {
                    return (int &)(arg.multi_definition_[i]);
                }
            }
        }
        return 0;
    }

    Argument& ArgParser::AddFlag(const std::string &string) {
        Argument now = Argument(string);
        now.argument_type_ = 'b';
        args_.emplace_back(now);
        return args_.back();
    }

    Argument& ArgParser::AddFlag(char s, const std::string &string) {
        Argument now = Argument(s,string);
        now.argument_type_ = 'b';
        args_.emplace_back(now);
        return args_.back();
    }

    Argument& ArgParser::AddFlag(char s, const std::string &string, const std::string& logic) {
        Argument now = Argument(s,string, logic);
        now.argument_type_ = 'b';
        args_.emplace_back(now);
        return args_.back();
    }

    bool ArgParser::GetFlag(const std::string &string) {
        for (const auto &arg: commands_) {
            if (arg.argument_long_name_ == string) {
                return (bool &)(arg.definition_);
            }
        }
        return false;
    }

    void ArgParser::AddHelp(char h, const std::string &string, const std::string &Description) {
        is_help_ = true;
        short_help_calling_ = h;
        long_help_calling_ = string;
        for (auto & arg : args_) {
            help_command_ << arg.argument_short_name_;
            help_command_ << " ";
            help_command_ << arg.argument_long_name_;
            help_command_ << " ";
            help_command_ << arg.logic_;
            help_command_ << "\n";
        }
    }

    bool ArgParser::Help() const {
        return is_help_;
    }
}