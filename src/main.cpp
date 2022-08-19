#include <fmt/format.h>
#include <CLI/CLI.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>

#include "CRC.h"

int main(int argc, char const* const* argv){
    CLI::App app{"brainsWitchery -- a binary switcher for firmware binaries"};
    
    std::string inputFileName;
    std::string outputFileName{"a.out"};
    std::string arrayName{"firmware"};

    std::size_t headerSize{16};
    bool generateHeader{true};

    app.add_option("-i,--input", inputFileName, "Input binary filename")->required()->check(CLI::ExistingFile);
    app.add_option("-o,--output", outputFileName, "Output binary filename (default a.out)");
    app.add_option("-s,--headerSize", headerSize, "Size of Header in binary (default 16 byte)");
    app.add_option("-p,--generateHeader", generateHeader, "Generate in output format C++ Header (default true)");
    app.add_option("-n,--name", arrayName, "Name of the C++ Array (default \"firmware\")");
    CLI11_PARSE(app, argc, argv);

    fmt::print("Opening input binary...");
    std::ifstream inputFile{inputFileName, std::ios::binary | std::ios::ate};
    if(!inputFile.is_open())
    {
        fmt::print("Failed to open File! Abort.");
        return -1;
    }
    fmt::print("OK\n");
    fmt::print("Reading binary...");
    std::size_t fileSize = inputFile.tellg();
    inputFile.seekg(0);
    std::vector<std::byte> inputBuffer;
    inputBuffer.resize(fileSize);
    inputFile.read(reinterpret_cast<char*>(&inputBuffer[0]), fileSize);
    fmt::print("FileSize: {} ", fileSize);
    fmt::print("OK\n");

    fmt::print("Opening output binary...");
    std::ofstream outputFile{outputFileName, std::ios::binary | std::ios::trunc};
    if(!outputFile.is_open())
    {
        fmt::print("Failed to open File! Abort.");
        return -1;
    }
    fmt::print("OK\n");

    std::uint32_t crcHash{0};
    std::memcpy(&crcHash, &inputBuffer[4], sizeof(crcHash));


    inputBuffer.erase(inputBuffer.begin(), inputBuffer.begin()+headerSize);

    static constexpr auto bytesToSwap{4};
    auto iterBegin{inputBuffer.begin()};
    while(iterBegin != inputBuffer.end()){
        std::reverse(iterBegin, iterBegin+bytesToSwap);
        iterBegin = iterBegin + bytesToSwap;
    }

    if(generateHeader)
    {
        outputFile << fmt::format("#pragma once"
                                  "\n\n#include <array>"
                                  "\n#include <cstdint>"
                                  "\n\nstatic constexpr std::array<std::byte, {}> {} {{", inputBuffer.size(), arrayName);
        std::size_t count{0};
        for(auto& ch : inputBuffer){
            if(count % 10 == 0){
                outputFile << fmt::format("\n  ");
            }
            outputFile << fmt::format("std::byte{{0x{:02x}}}, ", ch);
            ++count;
        }
        outputFile << fmt::format("\n}};");
    }
    else{
        for(auto& ch : inputBuffer){
            outputFile << std::to_integer<char>(ch);
        }
    }


    outputFile << fmt::format("\n\n static constexpr std::uint32_t {}_CRC{{0x{:x}}};\n", arrayName, crcHash);

    return 0;
}

