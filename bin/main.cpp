#include "iostream"
#include "parser/ArgParser.h"
#include "lib/Archive.h"

const uint64_t kDefaultBlockLength = 4;

void SetEncodedParameters(EncodedParameters& parameters, ArgumentParser::ArgParser& parser){
    uint64_t shift = parser.GetIntValue("hemming");
    if (shift == 1){
        parameters.shift = 1;
        parameters.final_encoded_length = 8;
        parameters.final_coded_length = 24;
    } else if (shift == 2){
        parameters.shift = 2;
        parameters.final_encoded_length = 16;
        parameters.final_coded_length = 40;
    } else if (shift == 3){
        parameters.shift = 4;
        parameters.final_encoded_length = 32;
        parameters.final_coded_length = 56;
    } else if (shift == 4){
        parameters.shift = 8;
        parameters.final_encoded_length = 16;
        parameters.final_coded_length = 24;
    } else if (shift == 5){
        parameters.shift = 16;
        parameters.final_encoded_length = 128;
        parameters.final_coded_length = 168;
    }
}

std::vector<std::string> GetArguments(std::vector<std::string>& arguments, int argc, char* argv[]){
    for (int i = 1; i < argc; ++i){
        arguments.emplace_back(argv[i]);
    }
    return arguments;
}

void SetArgs(ArgumentParser::ArgParser& parser, const std::vector<std::string>& arguments){
    parser.AddStringArgument('F', "from_archive-location", "Files from archive place in system").Default("../from_arch.txt");
    parser.AddIntArgument('n', "number", "Number of archive").Default(0);
    parser.AddStringArgument('R', "archive-location", "Archive place in system");
    for (const auto & argument : arguments) {
        if (argument == "-c" || argument == "--create="){
            parser.AddFlag('c', "create");
        }
        if (argument == "-f" || argument == "--file="){
            parser.AddStringArgument('f', "file").MultiValue().Positional();
        }
        if (argument == "-l" || argument == "--list"){
            parser.AddFlag('l', "list");
        }
        if (argument == "-x" || argument == "--extract"){
            parser.AddStringArgument('x', "extract").Default("all").MultiValue().Positional();
        }
        if (argument == "-a" || argument == "--append"){
            parser.AddStringArgument('a', "append");
        }
        if (argument == "-d" || argument == "--delete"){
            parser.AddStringArgument('d', "delete");
        }
        if (argument == "-A" || argument == "--concatenate") {
            parser.AddStringArgument('A', "concatenate").MultiValue();
        }
        if (argument == "-h" || argument == "--hemming") {
            parser.AddIntArgument('h', "hemming", "Block size when encoding can be from 1 to 5 length").Default((int)kDefaultBlockLength);
        }
        if (argument == "-n" || argument == "--number") {
            parser.AddIntArgument('n', "number", "Number of archive").Default(0);
        }
        if (argument == "-R" || argument == "--archive-location") {
            parser.AddStringArgument('R', "archive-location", "Archive place in system");
        }
        if (argument == "-F" || argument == "--from_archive-location") {
            parser.AddStringArgument('F', "from_archive-location", "Files from archive place in system").Default("from_haf_location");
        }
    }
}

void DoCommand(ArgumentParser::ArgParser& parser, EncodedParameters& parameters){
    for (uint64_t i = 0; i < parser.commands_.size(); ++i){
        if (parser.commands_[i].argument_long_name_ == "file"){
            CreateArchive(parser, parameters);
        }
        if (parser.commands_[i].argument_long_name_ == "list"){
            PrintNames(parser, parser.GetStringValue("archive-location"), parameters);
        }
        if (parser.commands_[i].argument_long_name_ == "extract"){
            ExtractFiles(parser, parser.GetStringValue("archive-location"), parameters);
        }
        if (parser.commands_[i].argument_long_name_ == "append"){
            AppendFile(parser, parser.GetStringValue("append"), parameters);
        }
        if (parser.commands_[i].argument_long_name_ == "delete"){
            DeleteFile(parser, parameters);
        }
        if (parser.commands_[i].argument_long_name_ == "concatenate"){
            MergeArchives(parser);
        }
    }
}

int main(int argc, char* argv[]){
    std::vector<std::string> arguments;
    arguments = GetArguments(arguments, argc, argv);
    ArgumentParser::ArgParser parser("my_parser");
    SetArgs(parser, arguments);
    parser.Parse(arguments);
    EncodedParameters parameters{};
    parser.AddIntArgument('h', "hemming", "Block size when encoding").Default(3);
    SetEncodedParameters(parameters, parser);
    DoCommand(parser, parameters);
    return 0;
}