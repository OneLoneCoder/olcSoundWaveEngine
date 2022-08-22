# require version 3.10 or higher
cmake_minimum_required(VERSION 3.10)

project(olcSoundWaveEngine)

option(USE_SDL2_MIXER "Force using SDL2_mixer as audio backend")

# Set C++ Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

######################################################################
# Directories

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# We need to specify the output for each configuration to make it work
# on Visual Studio solutions.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_PROFILE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_PROFILE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_PROFILE "${CMAKE_BINARY_DIR}/bin")

# set(SOURCE_CXX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_CXX_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/demos)
set(SOURCE_DATA_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/demos/assets)

# Source Files are Curated Here
file(
    GLOB_RECURSE _demoSourceFiles
    "${SOURCE_CXX_SRC_DIR}/*.cpp"
)

foreach(_demoSource IN LISTS _demoSourceFiles) #######################

get_filename_component(OutputExecutable ${_demoSource} NAME_WE)
add_executable(${OutputExecutable} ${_demoSource})

######################################################################
# MacOS: XCode
######################################################################
if(APPLE AND XCODE)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    # Carbon
    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    target_link_libraries(${OutputExecutable} ${CARBON_LIBRARY})

    # GLUT
    find_package(GLUT REQUIRED)
    target_link_libraries(${OutputExecutable} ${GLUT_LIBRARIES})

    # Threads
    find_package(Threads REQUIRED)
    target_link_libraries(${OutputExecutable} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})
    
    # SDL2_mixer
    set(USE_SDL2_MIXER ON)
            
    find_package(PNG REQUIRED)
    target_link_libraries(${OutputExecutable} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})
    
endif()

######################################################################
# Windows: MinGW
######################################################################
if(WIN32 AND MINGW)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    # GDI+
    set(GDIPLUS_LIBRARY gdiplus)
    target_link_libraries(${OutputExecutable} ${GDIPLUS_LIBRARY})
    
    # Shlwapi
    set(SHLWAPI_LIBRARY shlwapi)
    target_link_libraries(${OutputExecutable} ${SHLWAPI_LIBRARY})
    
    # Dwmapi
    set(DWMAPI_LIBRARY dwmapi)
    target_link_libraries(${OutputExecutable} ${DWMAPI_LIBRARY})

    if(NOT USE_SDL2_MIXER)
        
        # winmm
        set(WINMM_LIBRARY winmm)
        target_link_libraries(${OutputExecutable} ${WINMM_LIBRARY})

    endif() # NOT USE_SDL2_MIXER


    # stdc++fs
    target_link_libraries(${OutputExecutable} stdc++fs)

endif() # WIN32 AND MINGW

######################################################################
# Windows: Visual Studio / MSVC
######################################################################
if(WIN32 AND MSVC)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    # set the startup project to the target executable instead of ALL_BUILD
    set_property(
        DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY
        VS_STARTUP_PROJECT
        ${OutputExecutable}
    )
    
    # set working directory for Visual Studio Debugger
    set_target_properties(
        ${OutputExecutable} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    
    # GDI+
    set(GDIPLUS_LIBRARY gdiplus)
    target_link_libraries(${OutputExecutable} ${GDIPLUS_LIBRARY})
    
    # Shlwapi
    set(SHLWAPI_LIBRARY shlwapi)
    target_link_libraries(${OutputExecutable} ${SHLWAPI_LIBRARY})
    
    # Dwmapi
    set(DWMAPI_LIBRARY dwmapi)
    target_link_libraries(${OutputExecutable} ${DWMAPI_LIBRARY})

    if(NOT USE_SDL2_MIXER)
    
        # winmm
        set(WINMM_LIBRARY winmm)
        target_link_libraries(${OutputExecutable} ${WINMM_LIBRARY})

    endif()

endif() # WIN32 AND MSVC

######################################################################
# Linux: using anything?
######################################################################
if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    # X11
    find_package(X11 REQUIRED)
    target_link_libraries(${OutputExecutable} X11::X11)

    include_directories(${X11_INCLUDE_DIRS})

    # Threads
    find_package(Threads REQUIRED)
    target_link_libraries(${OutputExecutable} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})

    if(NOT USE_SDL2_MIXER)
        
        # ALSA
        find_package(ALSA REQUIRED)
        target_link_libraries(${OutputExecutable} ALSA::ALSA)
        include_directories(${ALSA_INCLUDE_DIRS})

    endif() # NOT USE_SDL2_MIXER

    find_package(PNG REQUIRED)
    target_link_libraries(${OutputExecutable} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})

    # stdc++fs
    target_link_libraries(${OutputExecutable} stdc++fs)

endif() # Linux

######################################################################
# Emscripten
######################################################################
if (EMSCRIPTEN)
    
    # Generate an HTML file
    set(CMAKE_EXECUTABLE_SUFFIX .html)

    # SDL2_mixer: build cache
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build sdl2_mixer)
    
    # libpng: build cache
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng)
    
    # zlib, for libpng: build cache
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build zlib)

    # require libpng
    find_package(PNG REQUIRED)
    target_link_libraries(${OutputExecutable} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})
    
    if(EXISTS "${SOURCE_DATA_DIR}" AND IS_DIRECTORY "${SOURCE_DATA_DIR}")
        target_link_options(
            ${OutputExecutable}
            PRIVATE
            -sALLOW_MEMORY_GROWTH=1
            -sMAX_WEBGL_VERSION=2
            -sMIN_WEBGL_VERSION=2
            -sUSE_LIBPNG=1
            -sUSE_SDL_MIXER=2          # thanks for the s, cstd
            -sLLD_REPORT_UNDEFINED
            --preload-file ${SOURCE_DATA_DIR}@assets)
    else()
        target_link_options(
            ${OutputExecutable}
            PRIVATE
            -sALLOW_MEMORY_GROWTH=1
            -sMAX_WEBGL_VERSION=2
            -sMIN_WEBGL_VERSION=2
            -sUSE_LIBPNG=1
            -sUSE_SDL_MIXER=2          # thanks for the s, cstd
            -sLLD_REPORT_UNDEFINED)
    endif()

endif() # Emscripten


if(USE_SDL2_MIXER AND NOT EMSCRIPTEN)
        
    # SDL2_mixer
    find_package(SDL2_mixer REQUIRED)
    target_link_libraries(${OutputExecutable} SDL2_mixer::SDL2_mixer)

    add_compile_definitions(SOUNDWAVE_USING_SDLMIXER=1)

endif() # USE_SDL2_MIXER


######################################################################
# Set include directory
######################################################################
if(IS_DIRECTORY ${SOURCE_CXX_INCLUDE_DIR})
    include_directories(${SOURCE_CXX_INCLUDE_DIR})
endif()

endforeach() #########################################################


######################################################################
# Copy assets/ directory target
######################################################################
set(DATA_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/assets)

file(GLOB_RECURSE src_data_files
    RELATIVE ${SOURCE_DATA_DIR}/ "${SOURCE_DATA_DIR}/*.*" "${SOURCE_DATA_DIR}/*")
foreach(fn ${src_data_files})
    add_custom_command(
        OUTPUT ${DATA_OUTPUT_DIR}/${fn}
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DATA_DIR}/${fn} ${DATA_OUTPUT_DIR}/${fn}
        MAIN_DEPENDENCY ${SOURCE_DATA_DIR}/${fn})
    list(APPEND out_data_files ${DATA_OUTPUT_DIR}/${fn})
endforeach()

add_custom_target(copy_data DEPENDS ${out_data_files})

# Copy Asset Files, if not Emscripten
if (NOT EMSCRIPTEN)
    foreach(_demoSource IN LISTS _demoSourceFiles) #######################
        get_filename_component(OutputExecutable ${_demoSource} NAME_WE)
        add_dependencies(${OutputExecutable} copy_data)
    endforeach()
endif()
