add_library(hamarc
        HammingEncoder.h
        HammingEncoder.cpp
        HammingDecryptor.h
        HammingDecryptor.cpp
        Archive.h
        Archive.cpp
        ArchiveWriter.h
        ArchiveWriter.cpp
        ArchiveReader.h
        ArchiveReader.cpp)

target_include_directories(hamarc PUBLIC "${PROJECT_SOURCE_DIR}/include")
