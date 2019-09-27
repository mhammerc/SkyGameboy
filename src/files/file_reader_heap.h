#ifndef FRACTAL_FILE_READER_HEAP_H
#define FRACTAL_FILE_READER_HEAP_H

#include <string>
#include <fstream>
#include <memory>

#include "../general.h"

/**
 * Open and read an unknown-size or big-size file, making it available to be read.
 */
class FileReaderHeap
{
public:
    // On top to initialize it first
    const size_t fileSize;

private:
    std::unique_ptr<uint8> mutable_data;

public:
    // A non-mutable view of the data
    const uint8 * const data;

    /**
     * If opening `filename` fail, the constructor will throw.
     *
     * @param filename path to the file to read and fill in `data`
     */
    explicit FileReaderHeap(const std::string &filename):
    fileSize(filesize(filename)), mutable_data(new uint8[fileSize]), data(mutable_data.get())
    {
        std::ifstream fileStream(filename , std::ifstream::binary);
        if (!fileStream.is_open())
        {
            throw std::invalid_argument("Can not open file at given path");
        }

        fileStream.read(reinterpret_cast<int8*>(mutable_data.get()), fileSize);
    }

    FileReaderHeap& operator=(const FileReaderHeap&) = delete;
    FileReaderHeap(FileReaderHeap&) = delete;

private:
    static std::ifstream::pos_type filesize(const std::string &filename)
    {
        std::ifstream fileStream(filename, std::ifstream::ate | std::ifstream::binary);
        if (!fileStream.is_open())
        {
            throw std::invalid_argument("Can not open file at given path");
        }

        return fileStream.tellg();
    }
};

#endif
