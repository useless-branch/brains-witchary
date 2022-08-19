cmake_minimum_required(VERSION 3.18)

if(TARGET brainsWitchery)
    return()
endif()

if(NOT CMAKE_CROSSCOMPILING)

    project(brainsWitchery VERSION 0.2.0)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    add_compile_options (-fdiagnostics-color=always)
    add_compile_options (-Wfatal-errors)
    add_compile_options(-march=native)
    add_compile_options(-O3)

    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        add_compile_options("/W4")
        message("Compiling with: Microsoft Compiler")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options("-Wall" "-Wextra" "-pedantic")
        message("Compiling with: GCC Compiler")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        #add_compile_options("-Weverything")
        add_compile_options(
                "-Wall"
                "-Wextra"
                "-pedantic"
                "-Wfatal-errors"
                "-Wno-c++98-compat"
                "-Wno-c++98-c++11-compat-binary-literal"
                "-Wno-c++98-compat-pedantic"
        )
        message("Compiling with: Clang Compiler")
    endif()



    find_package(fmt)


    add_executable(brainsWitchery
            src/main.cpp)

    target_link_libraries(brainsWitchery PRIVATE
        fmt::fmt-header-only
        )

    target_include_directories(brainsWitchery PRIVATE external/CRCpp/inc)

else()
    include(cmake/HostBuild.cmake)
    configure_host_build(brainsWitchery)
endif()

function(brainsWitcheryGenerate targetname)

    cmake_parse_arguments(
            PARSE_ARGV
            1
            PARSED_ARGS
            ""
            "INPUT_FILE;HEADER_SIZE"
            ""
    )

    if(PARSED_ARGS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "unknown argument ${PARSED_ARGS_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT PARSED_ARGS_INPUT_FILE)
        message(FATAL_ERROR "brainsWitchery needs INPUT_FILE")
    endif()

    if(NOT PARSED_ARGS_HEADER_SIZE)
        set(PARSED_ARGS_HEADER_SIZE 16)
    endif()

    set(output_directory_name ${CMAKE_CURRENT_BINARY_DIR}/brainsWitchery_generated/${PARSED_ARGS_OUTPUT_DIRECTORY})

    set(input_filename ${CMAKE_CURRENT_SOURCE_DIR}/${PARSED_ARGS_INPUT_FILE})

    set(output_filename ${targetname}.hpp)

    get_target_property(build_dir brainsWitchery BINARY_DIR)

    if(NOT CMAKE_CROSSCOMPILING)
        set(brainsWitchery_bin ${build_dir}/brainsWitchery)
    else()
        set(brainsWitchery_bin ${build_dir}/host_build/brainsWitchery)
    endif()

    set(
            command ${brainsWitchery_bin}
            --input=${input_filename}
            --output=${output_directory_name}/${output_filename}
            --generateHeader=true
            --name=${targetname}
            --headerSize=${PARSED_ARGS_HEADER_SIZE}
    )

    add_custom_command(
            COMMAND cmake -E make_directory ${output_directory_name}
            COMMAND ${command}
            DEPENDS brainsWitchery ${input_filename} ${brainsWitchery_bin}
            OUTPUT ${output_filename}
    )

    if(${CMAKE_VERSION} VERSION_LESS "3.19.0")
        set_source_files_properties(
                ${output_filename}
                PROPERTIES GENERATED TRUE
        )
        add_custom_target(
                ${targetname}_helper
                DEPENDS ${output_filename}
        )
        add_library(${targetname} INTERFACE)
        add_dependencies(${targetname} ${targetname}_helper)
    else()
        add_library(${targetname} INTERFACE ${output_filename})
    endif()

    target_include_directories(${targetname} INTERFACE ${output_directory_name})

endfunction()