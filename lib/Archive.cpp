#include "cstring"
#include "fstream"
#include "iostream"
#include "Archive.h"
#include "HammingEncoder.h"
#include "ArchiveWriter.cpp"

const uint64_t kSufficientLength = 100;

std::string CreateArchivePath(ArgumentParser::ArgParser& parser){
    char name[kSufficientLength];
    name[0] = '"';
    std::string str = "archive";
    char num[str.length()];
    for (uint64_t i = 0; i < str.length(); i++) {
        num[i] = str[i];
    }
    std::string location = parser.GetStringValue("archive-location");
    char path[location.length()];
    for (uint64_t i = 0; i < location.length() + 1; ++i){
        path[i] = location[i];
    }
    strcpy(name, path);
    strcat(name, num);
    strcat(name, ".haf");
    strcat(name, reinterpret_cast<const char *>(""));
    return name;
}

void CreateArchive(ArgumentParser::ArgParser& parser, EncodedParameters& parameters){
    std::string path = CreateArchivePath(parser);
    std::ofstream archive(path, std::ios::binary);
    for (uint64_t i = 0; i < parser.commands_.size(); ++i){
        if (parser.commands_[i].argument_long_name_  == "file"){
            for (uint64_t j = 0; j < parser.commands_[i].multi_string_stored_->size(); ++j){
                AppendFile(parser, parser.GetStringValue("file", j), parameters);
            }
        }

    }
}

std::string CreateFilesFromArchivePath(ArgumentParser::ArgParser& parser){
    char name[kSufficientLength];
    name[0] = '"';
    std::string location = parser.GetStringValue("from_archive-location");
    char path[location.length()];
    for (uint64_t i = 0; i < location.length() + 1; ++i){
        path[i] = location[i];
    }
    strcpy(name, path);
    strcat(name, reinterpret_cast<const char *>(""));
    return name;
}

void PushCorrection(std::ofstream& archive, std::vector<bool> correct){
    for (uint64_t i = 0; i < correct.size() / kBitsInBite; ++i){
        uint8_t number = 0;
        for (uint8_t k = 0; k < kBitsInBite; k++) {
            if (correct[kBitsInBite * i + k]) {
                number += pow(2, kMaxBlockIndex - k);
            }
        }
        archive << number;
    }
    if (correct.size() % kBitsInBite != 0){
        uint8_t number = 0;
        for (uint64_t k = 0; k < correct.size() % kBitsInBite; k++){
            if(correct[kBitsInBite * (correct.size() / kBitsInBite) + k]){
                number += pow(2, kMaxBlockIndex - k);
            }
        }
        archive << number;
    }
}

void AppendFile(ArgumentParser::ArgParser& parser, std::string path, EncodedParameters& parameters){
    std::string archive = CreateArchivePath(parser);
    std::ofstream file(archive, std::ios::binary|std::ios::app);
    std::vector<bool> temp_name;
    std::vector<bool> correction_char;
    correction_char.push_back(false);
    for (uint64_t i = 0; i < 5; ++i){
        correction_char.push_back(true);
    }
    correction_char.push_back(false);
    correction_char.push_back(false);
    std::vector<bool> correct = Encode(correction_char.size(), parameters.shift, correction_char);
    while(correct.size() < parameters.final_coded_length){
        correct.push_back(false);
    }
    for (uint64_t i = 0; i < 8 * path.length(); ++i){
        temp_name.push_back(false);
    }
    for (uint64_t i = 0; i < path.length(); ++i){
        uint32_t temp = int(path[i]);
        uint64_t k = kMaxBlockIndex;
        while(temp > 0){
            temp_name[i * kBitsInBite + k] = (temp % 2);
            temp /= 2;
            k--;
        }
    }
    std::vector<bool> full_name = Encode(temp_name.size(), parameters.shift, temp_name);
    while (full_name.size() % parameters.final_coded_length != 0 ){
        full_name.push_back(false);
    }
    for (uint64_t i = 0; i < full_name.size() / kBitsInBite; ++i){
        uint8_t number = 0;
        for (uint8_t k = 0; k < kBitsInBite; k++){
            if(full_name[kBitsInBite * i + k]){
                number += pow(2, kMaxBlockIndex - k);
            }
        }
        file << number;
    }
    if (full_name.size() % kBitsInBite != 0){
        uint8_t number = 0;
        for (uint64_t k = 0; k < full_name.size() % parameters.shift; k++){
            if(full_name[parameters.shift * (full_name.size() / parameters.shift) + k]){
                number += pow(2, kMaxBlockIndex - k);
            }
        }
        file << number;
    }
    PushCorrection(file, correct);
    std::string now = path;
    std::ifstream appended(now, std::ios::binary);
    std::vector<uint8_t> temp_array;
    char temp;
    while (appended >> std::noskipws >> temp){
        temp_array.push_back(int(temp));
    }
    std::cout << temp_array.size() << "\n";
    uint64_t length = (temp_array.size() / parameters.shift) * (parameters.shift + SetAmountAddingBits(parameters.shift));
    if (temp_array.size() % parameters.shift != 0) {
        length++;
    }
    uint64_t cur = log2(length);
    bool binary_version[cur + 1];
    for (uint64_t i = 0; i < cur + 1; ++i){
        binary_version[i] = false;
    }
    CreateUnaryBinaryVersion(length, binary_version, cur);
    std::vector<bool> file_length;
    for (uint64_t i = 0; i < cur + 1; ++i){
        file_length.push_back(binary_version[i]);
    }
    std::vector<bool> file_len = Encode(cur + 1, parameters.shift, file_length);
    while (file_len.size() < parameters.final_coded_length){
        file_len.push_back(false);
    }
    PushCorrection(file, file_len);
    PushCorrection(file, correct);
    Write(parser, path, file, parameters);
    PushCorrection(file, correct);
}

void DeleteFile(ArgumentParser::ArgParser& parser, EncodedParameters& parameters){
    std::string deleted = parser.GetStringValue("delete");
    std::vector<char> deleted_in_vector;
    for (uint64_t i = 0; i < deleted.length(); ++i){
        deleted_in_vector.push_back(deleted[i]);
    }
    std::string archive = CreateArchivePath(parser);
    std::vector<std::string> list;
    std::fstream file(archive, std::ios::in|std::ios::out|std::ios::binary);
    Read(parser, "../temp.txt", parameters);
    std::ofstream temp("../temp.txt", std::ios::binary);
    uint64_t pos = 3;
    std::vector<char> temp_array;
    char readed;
    while (file >> std::noskipws >> readed){
        if (pos == 3 && readed != '|') {
            temp_array.push_back(char(readed));
        } else if (pos == 3 && readed == '|'){
            pos--;
            if (deleted_in_vector != temp_array){
                char position[temp_array.size()];
                for (uint64_t j = 0; j < temp_array.size(); ++j){
                    position[j] = temp_array[j];
                }
                Write(parser, position, temp, parameters);
                temp_array.clear();
            } else {
                while (true){
                    file >> std::noskipws >> readed;
                    if (readed == '|'){
                        pos--;
                        if (pos == 1){
                            pos = 3;
                            break;
                        }
                    }
                }
            }
        } else if (pos == 2 && readed != '|') {
            temp_array.push_back(char(readed));
        } else if (pos == 2 && readed == '|'){
            char position[temp_array.size()];
            for (uint64_t j = 0; j < temp_array.size(); ++j){
                position[j] = temp_array[j];
            }
            Write(parser, position, temp, parameters);
            temp_array.clear();
        } else if (pos == 1 && readed != '|'){
            temp_array.push_back(char(readed));
        } else {
            char position[temp_array.size()];
            for (uint64_t j = 0; j < temp_array.size(); ++j){
                position[j] = temp_array[j];
            }
            Write(parser, position, temp, parameters);
            temp_array.clear();
            pos = 3;
        }
    }
}

void MergeArchives(ArgumentParser::ArgParser& parser){
    std::string first_archive = parser.GetStringValue("concatenate", 0);
    std::string second_archive = parser.GetStringValue("concatenate", 1);
    std::ofstream first(first_archive, std::ios::binary|std::ios::app);
    std::ifstream second(second_archive, std::ios::binary);
    char temp;
    while (second >> std::noskipws >> temp){
        first << temp;
    }
}

void ExtractFiles(ArgumentParser::ArgParser& parser, const std::string& cur_path, EncodedParameters& parameters){
    std::vector<std::string> extracted;
    extracted.emplace_back("all");
    std::string archive = CreateArchivePath(parser);
    std::vector<std::string> list;
    std::ifstream file(archive, std::ios::binary);
    Read(parser, cur_path, parameters);
    std::fstream temp("../temp.txt", std::ios::out|std::ios::in);
    uint64_t pos = 3;
    std::vector<char> temp_array;
    char readed;
    bool checked = false;
    std::string current_path = parser.GetStringValue("from_archive-location");
    std::ofstream extracteded(current_path, std::ios::in|std::ios::app);
    while (temp >> std::noskipws >> readed){
        if (pos == 3 && readed != '|') {
            temp_array.push_back(readed);
        } else if (pos == 3 && readed == '|'){
            if (extracted[0] == "all"){
                checked = true;
            }
            char position[temp_array.size()];
            for (uint64_t i = 0; i < temp_array.size(); ++i){
                position[i] = temp_array[i];
            }
            for (uint64_t i = 0; i < parser.commands_.size(); ++i){
                if (parser.commands_[i].argument_long_name_  == "extract"){
                    for (uint64_t j = 0; j < parser.commands_[i].multi_string_stored_->size(); ++j){
                        if (position == parser.GetStringValue("extract", j)){
                            checked = true;
                        }
                    }
                }

            }
            pos--;
            temp_array.clear();
        } else if (pos == 2 && readed == '|'){
            for (uint64_t i = 0; i < parameters.final_coded_length / kBitsInBite - 1; ++i){
                temp >> std::noskipws >> readed;
            }
            pos--;
        } else if (pos == 1 && readed != '|'){
            if (checked) {
                extracteded << readed;
            }
        } else if (pos == 1 && readed == '|'){
            pos = 3;
            checked = false;
        }
    }
}

void PrintNames(ArgumentParser::ArgParser& parser, const std::string& cur_path, EncodedParameters& parameters) {
    std::string archive = CreateArchivePath(parser);
    std::ifstream file(archive, std::ios::binary);
    Read(parser, cur_path, parameters);
    std::fstream temp("../temp.txt", std::ios::out|std::ios::in);
    uint64_t pos = 3;
    char readed;
    while (temp >> std::noskipws >> readed){
        if (pos == 3 && readed != '|') {
            std::cout << readed;
        } else if (pos == 3 && readed == '|'){
            pos--;
            std::cout << "\n";
        } else if ( pos == 2 && readed == '|'){
            pos--;
        } else if (pos == 1 && readed == '|'){
            pos = 3;
        }
    }
}