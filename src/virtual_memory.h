#ifndef FRACTAL_VIRTUAL_MEMORY_H
#define FRACTAL_VIRTUAL_MEMORY_H

#include <cstddef>
#include <memory>

#include "general.h"
#include "file_reader.h"

/**
 * MMU
 */
class VirtualMemory
{
public:
    explicit VirtualMemory(std::unique_ptr<FileReader<256>> biosRom) : biosRom(std::move(biosRom))
    {

    }

private:
    std::unique_ptr<FileReader<256>> biosRom;
};


#endif //FRACTAL_VIRTUAL_MEMORY_H
