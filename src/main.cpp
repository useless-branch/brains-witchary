#include <fmt/format.h>
#include <CLI/CLI.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>

int main(int argc, char const* const* argv){
    CLI::App app{"brainsWitchery -- a binary switcher for firmware binaries"};
    
    std::string inputFileName;
    std::string outputFileName{"a.out"};
    std::size_t headerSize{16};

    app.add_option("-i,--input", inputFileName, "Input binary filename")->required()->check(CLI::ExistingFile);
    app.add_option("-o,--output", outputFileName, "Output binary filename (default a.out)");
    app.add_option("-s,--headerSize", headerSize, "Size of Header in binary (default 16 byte)");
    
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

    static constexpr auto bytesToSwap{4};
    for(std::size_t i{headerSize}; i < inputBuffer.size(); i+=bytesToSwap){
        auto iterBegin{inputBuffer.begin()+i};
        auto iterEnd{inputBuffer.begin()+i+bytesToSwap};
        std::reverse(iterBegin, iterEnd);
    }    

    for(auto& ch : inputBuffer){
        outputFile << std::to_integer<char>(ch);
    }

    return 0;
}

