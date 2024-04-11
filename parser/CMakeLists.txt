add_library(argparser ArgParser.h
        ArgParser.cpp
        Argument.h
        Argument.cpp)

target_include_directories(argparser PUBLIC "${PROJECT_SOURCE_DIR}/include")