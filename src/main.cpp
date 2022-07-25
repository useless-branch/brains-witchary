#include <fmt/format.h>
#include <CLI/CLI.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <span>

int main(int argc, char const* const* argv){
    CLI::App app{"brainsWitchery binary switcher for IMU firmware"};
    
    std::string inputFileName;
    std::string outputFileName{"a.out"};
    std::size_t headerSize{16};

    app.add_option("-i,--input", inputFileName, "Input binary filename")->required()->check(CLI::ExistingFile);
    app.add_option("-o,--output", outputFileName, "Output binary filename");
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

    std::span<std::byte> firmwareSpan{std::as_writable_bytes(std::span{inputBuffer})};
    for(std::size_t i{0}; i < headerSize; ++i){
        outputFile << std::to_integer<char>(firmwareSpan[i]);
    }
    firmwareSpan = firmwareSpan.subspan(headerSize);

    static constexpr std::size_t bytesToFlip{4};
    for(std::size_t i{0}; i < firmwareSpan.size_bytes(); ++i){
        std::size_t countDown{bytesToFlip-1};
        for(std::size_t countUp{0}; countUp < bytesToFlip; ++countUp){
            outputFile << std::to_integer<char>(firmwareSpan[countDown]);
            --countDown;
        }
        firmwareSpan = firmwareSpan.subspan(bytesToFlip);
    }


    return 0;
}

