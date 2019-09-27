#ifndef FRACTAL_FILE_READER_STACK_H
#define FRACTAL_FILE_READER_STACK_H

#include <string>
#include <fstream>

#include "../general.h"

/**
 * Load a known-size file which fit on stack.
 *
 * Create `size` bytes buffer, and fill it with file data located at `path`.
 * If file is larger than `size`, remaining bytes will be dropped.
 * If file is smaller than `size`, remaining bytes will be 0x0.
 *
 * This class is a generic which allow buffer to live on stack, easily.
 *
 * This is especially useful and easy to use for a gameboy emulator:
 * - Boot loader is 256KB
 * - Game are max 2MB
 *
 * Once a file is loaded, it is not editable.
 */
template <const size_t size>
class FileReaderStack
{
public:
    // A non-mutable view of the data
    const uint8 * const data = mutable_data;

    /**
     * If opening `path` fail, the constructor will throw.
     *
     * @param path path to the file to read and fill in `data`
     */
    explicit FileReaderStack(const std::string &path)
    {
        std::ifstream fileStream(path, std::ios::binary);
        if (!fileStream.is_open())
        {
            throw std::invalid_argument("Can not open file at given path");
        }

        fileStream.read(reinterpret_cast<int8*>(mutable_data), size);
    }

    FileReaderStack& operator=(const FileReaderStack&) = delete;

private:
    uint8 mutable_data[size] {};
};


#endif //FRACTAL_FILE_READER_STACK_H
