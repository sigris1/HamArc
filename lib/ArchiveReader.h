#pragma once

#include "../parser/ArgParser.h"
#include "Archive.h"

void Read(ArgumentParser::ArgParser& parser, const std::string& unarchived_path, EncodedParameters& parameters);