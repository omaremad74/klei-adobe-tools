#pragma once

#include <fstream>
#include "logger.hpp"
#include "strings.hpp"

#include "libzip/lib/zip.h"

template<typename data_type>
static void read_bin_data(std::istream& stream, data_type& value) {
    if (!stream.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        throw std::runtime_error(STRINGS::UNABLE_READ_STREAM);
    }
}

template<typename data_type>
static void write_bin_data(std::ostream& stream, const data_type value) {
    if (!stream.write(reinterpret_cast<const char*>(&value), sizeof(value))) {
        throw std::runtime_error(STRINGS::UNABLE_WRITE_STREAM);
    }
}

static void read_bin_string(std::istream& stream, std::string& str, const int length) {
    char string_buffer[1 << 12];
    
    if (stream.read(string_buffer, length)) {
        str = string_buffer;
        str.resize(length);
    } else {
        throw std::runtime_error(STRINGS::UNABLE_READ_STREAM);
    }
}

static void write_bin_string(std::ostream& stream, const std::string& str, const size_t length) {
    if (!stream.write(str.data(), length)) {
        throw std::runtime_error(STRINGS::UNABLE_WRITE_STREAM);
    }
}

static void read_bin_kleistring(std::istream& stream, std::string& str) {
    uint32_t str_length;

    read_bin_data(stream, str_length);
    read_bin_string(stream, str, str_length);
}

static void write_bin_kleistring(std::ostream& stream, const std::string& str) {
    const size_t str_length = str.length();
    write_bin_data(stream, str_length);
    write_bin_string(stream, str, str_length);
}

struct ZipStream {
    char* buffer;
    std::istringstream stream;
};

// Do not forget to delete the buffer after you are done using the stream.
static ZipStream GetZipStreamFromFile(const std::filesystem::path& zippath, const std::string& animname) {
    int errorcode;
    zip* zip_handle = zip_open(zippath.string().c_str(), ZIP_RDONLY, &errorcode);
    if (zip_handle == nullptr) {
        zip_error_t error;
        zip_error_init_with_code(&error, errorcode);
        const char* error_str = zip_error_strerror(&error);
        zip_error_fini(&error);
        throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::ERROR_CODE, error_str));
    }

    const zip_int64_t idx = zip_name_locate(zip_handle, animname.c_str(), 0);
    if (idx == -1) {
        zip_close(zip_handle);
        throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::UNABLE_TO_LOCATE_FILE, animname, zippath.string()));
    }

    struct zip_stat file_info;

    if (zip_stat_index(zip_handle, idx, 0, &file_info) == -1) {
        const char* error = zip_strerror(zip_handle);
        zip_close(zip_handle);
        throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::UNABLE_TO_STAT_FILE, animname, zippath.string(), error));
    }

    zip_file* entry = zip_fopen_index( zip_handle, idx, ZIP_FL_UNCHANGED );
    if (entry == nullptr) {
        const char* error = zip_strerror(zip_handle);
        zip_close(zip_handle);
        throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::UNABLE_TO_OPEN_FILE, animname, zippath.string(), error)); 
    }

    char* buffer = new char[file_info.size];

    const zip_int64_t read_count = zip_fread(entry, buffer, file_info.size);

    if (read_count == -1) {
        const char* error = zip_file_strerror(entry);
        zip_fclose(entry);
        zip_close(zip_handle);
        throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::UNABLE_TO_READ_FILE, animname, zippath.string(), error)); 
    }

    zip_fclose(entry);
    zip_close(zip_handle);

    return { buffer, std::istringstream(std::string(buffer, size_t(read_count))) };
}

static void SaveFolderToZip(const std::filesystem::path& folderpath) {
    if (folderpath.has_extension())
        throw std::invalid_argument(std::format(STRINGS::ZIP_ERRORS::UNABLE_TO_ZIP_FOLDER, folderpath.string()));

    const std::string folderpath_str = folderpath.string();
    int errorcode;
    zip* zip_handle = zip_open((folderpath_str + ".zip").c_str(), ZIP_CREATE, &errorcode);
    if (zip_handle == nullptr) {
        zip_error_t error;
        zip_error_init_with_code(&error, errorcode);
        const char* error_str = zip_error_strerror(&error);
        zip_error_fini(&error);
        throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::ERROR_CODE, error_str));
    }

    for (const auto& dir_entry : std::filesystem::directory_iterator{folderpath}) {
        const std::filesystem::path dir_path = dir_entry.path();
        zip_source *source = zip_source_file(zip_handle, dir_path.string().c_str(), 0, 0);
        if (source == nullptr) {
            throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::UNABLE_TO_ADD_SOURCE, zip_strerror(zip_handle)));
        }

        const zip_int64_t idx = zip_file_add(zip_handle, dir_path.filename().string().c_str(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8);
        if (idx == -1) {
            zip_source_free(source);
            throw std::runtime_error(std::format(STRINGS::ZIP_ERRORS::UNABLE_TO_ADD_FILE, zip_strerror(zip_handle)));
        }
    }

    zip_close(zip_handle);
}