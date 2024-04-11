#pragma once

#include "string"
#include "../parser/ArgParser.h"

const uint64_t kBitsInBite = 8;
const uint64_t kMaxBlockIndex = 7;

struct EncodedParameters{
    uint64_t shift;
    uint64_t final_encoded_length;
    uint64_t final_coded_length;
};

std::string CreateArchivePath(ArgumentParser::ArgParser& parser);

std::string CreateFilesFromArchivePath(ArgumentParser::ArgParser& parser);

void CreateArchive(ArgumentParser::ArgParser& parser, EncodedParameters& parameters);

void AppendFile(ArgumentParser::ArgParser& parser, std::string path, EncodedParameters& parameters);

void DeleteFile(ArgumentParser::ArgParser& parser, EncodedParameters& parameters);

void MergeArchives(ArgumentParser::ArgParser& parser);

void ExtractFiles(ArgumentParser::ArgParser& parser, const std::string& path, EncodedParameters& parameters);

void PrintNames(ArgumentParser::ArgParser& parser, const std::string& path, EncodedParameters& parameters);