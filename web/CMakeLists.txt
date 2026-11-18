# CMake build for the WebAssembly (Emscripten) version of POV-Ray.
#
# Configure and build with the Emscripten toolchain wrapper:
#
#   emcmake cmake -S web -B build-web -DCMAKE_BUILD_TYPE=Release
#   cmake --build build-web -j
#
# or run `web/build.sh`. The resulting site is assembled in
# `build-web/site/` and can be served with `web/tools/serve.py`.

cmake_minimum_required(VERSION 3.16)
project(povray_web CXX C)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "This project must be configured with Emscripten (emcmake cmake ...)")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

get_filename_component(POV_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(WEB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(SITE_DIR "${CMAKE_BINARY_DIR}/site")

option(POVWEB_SIMD "Enable WebAssembly SIMD (supported by all current browsers)" ON)
set(POVWEB_BUILT_BY "POV-Ray Web build" CACHE STRING "Value reported as 'compiled by'")
set(POVWEB_SOURCE_URL "https://github.com/POV-Ray/povray" CACHE STRING
    "Repository the page footer links to for the source code of this build")

#-------------------------------------------------------------------------------
# Compiler settings shared by POV-Ray and the bundled libraries
#-------------------------------------------------------------------------------

set(POVWEB_COMMON_FLAGS
    -pthread
    -fwasm-exceptions
    -sSUPPORT_LONGJMP=wasm
)
if(POVWEB_SIMD)
    list(APPEND POVWEB_COMMON_FLAGS -msimd128)
endif()
add_compile_options(${POVWEB_COMMON_FLAGS} $<$<CONFIG:Release>:-O3>)

#-------------------------------------------------------------------------------
# Bundled third-party libraries (zlib, libpng, libjpeg)
#-------------------------------------------------------------------------------

set(LIB_ROOT "${POV_ROOT}/libraries")

set(ZLIB_SOURCE_NAMES adler32 compress crc32 deflate gzclose gzlib gzread gzwrite
    infback inffast inflate inftrees trees uncompr zutil)
list(TRANSFORM ZLIB_SOURCE_NAMES PREPEND "${LIB_ROOT}/zlib/")
list(TRANSFORM ZLIB_SOURCE_NAMES APPEND ".c")
add_library(povweb_zlib STATIC ${ZLIB_SOURCE_NAMES})
target_include_directories(povweb_zlib PUBLIC "${LIB_ROOT}/zlib")
target_compile_definitions(povweb_zlib PRIVATE HAVE_UNISTD_H HAVE_STDARG_H)
target_compile_options(povweb_zlib PRIVATE -Wno-deprecated-non-prototype -Wno-implicit-function-declaration)

# libpng needs a `pnglibconf.h`; use the prebuilt default configuration.
set(PNG_GENERATED_DIR "${CMAKE_BINARY_DIR}/libpng")
configure_file("${LIB_ROOT}/png/scripts/pnglibconf.h.prebuilt" "${PNG_GENERATED_DIR}/pnglibconf.h" COPYONLY)
set(PNG_SOURCE_NAMES png pngerror pngget pngmem pngpread pngread pngrio pngrtran
    pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil)
list(TRANSFORM PNG_SOURCE_NAMES PREPEND "${LIB_ROOT}/png/")
list(TRANSFORM PNG_SOURCE_NAMES APPEND ".c")
add_library(povweb_png STATIC ${PNG_SOURCE_NAMES})
target_include_directories(povweb_png PUBLIC "${LIB_ROOT}/png" "${PNG_GENERATED_DIR}")
target_link_libraries(povweb_png PUBLIC povweb_zlib)

set(JPEG_SOURCE_NAMES jaricom jcapimin jcapistd jcarith jccoefct jccolor jcdctmgr
    jchuff jcinit jcmainct jcmarker jcmaster jcomapi jcparam jcprepct jcsample
    jctrans jdapimin jdapistd jdarith jdatadst jdatasrc jdcoefct jdcolor jddctmgr
    jdhuff jdinput jdmainct jdmarker jdmaster jdmerge jdpostct jdsample jdtrans
    jerror jfdctflt jfdctfst jfdctint jidctflt jidctfst jidctint jquant1 jquant2
    jutils jmemmgr jmemnobs)
list(TRANSFORM JPEG_SOURCE_NAMES PREPEND "${LIB_ROOT}/jpeg/")
list(TRANSFORM JPEG_SOURCE_NAMES APPEND ".c")
add_library(povweb_jpeg STATIC ${JPEG_SOURCE_NAMES})
target_include_directories(povweb_jpeg PUBLIC "${LIB_ROOT}/jpeg")
target_compile_options(povweb_jpeg PRIVATE -Wno-shift-negative-value)

#-------------------------------------------------------------------------------
# POV-Ray sources
#-------------------------------------------------------------------------------

file(GLOB_RECURSE POV_CORE_SOURCES CONFIGURE_DEPENDS "${POV_ROOT}/source/*.cpp")

set(POV_SOURCES
    ${POV_CORE_SOURCES}
    # Unix platform layer, reused as-is.
    ${POV_ROOT}/platform/unix/syspovfilesystem.cpp
    ${POV_ROOT}/platform/unix/syspovtimer.cpp
    # Virtual front-end.
    ${POV_ROOT}/vfe/vfe.cpp
    ${POV_ROOT}/vfe/vfecontrol.cpp
    ${POV_ROOT}/vfe/vfedisplay.cpp
    ${POV_ROOT}/vfe/vfepovms.cpp
    ${POV_ROOT}/vfe/vfesession.cpp
    ${POV_ROOT}/vfe/unix/unixoptions.cpp
    ${POV_ROOT}/vfe/unix/vfeplatform.cpp
    # Web entry point.
    ${WEB_ROOT}/src/webconsole.cpp
)

add_executable(povray ${POV_SOURCES})
set_target_properties(povray PROPERTIES
    OUTPUT_NAME povray
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${SITE_DIR}"
)

target_include_directories(povray PRIVATE
    ${WEB_ROOT}/config
    ${POV_ROOT}/unix/povconfig
    ${POV_ROOT}/source
    ${POV_ROOT}/platform/unix
    ${POV_ROOT}/vfe
    ${POV_ROOT}/vfe/unix
    ${POV_ROOT}/unix
)
target_include_directories(povray SYSTEM PRIVATE
    ${POV_ROOT}/libraries/boost
)

target_compile_definitions(povray PRIVATE
    HAVE_CONFIG_H
    "BUILT_BY=\"${POVWEB_BUILT_BY}\""
    # Paths inside the Emscripten virtual file system.
    "POVLIBDIR=\"/povray\""
    "POVCONFDIR=\"/povray/etc\""
    "POVCONFDIR_BACKWARD=\"/povray/etc\""
    # Bundled Boost predates the current libc++: keep it from using
    # std::auto_ptr, and silence its deprecation message about global placeholders.
    BOOST_NO_AUTO_PTR
    BOOST_BIND_GLOBAL_PLACEHOLDERS
)

#-------------------------------------------------------------------------------
# POV-Ray compiler and linker settings
#-------------------------------------------------------------------------------

target_link_libraries(povray PRIVATE povweb_png povweb_jpeg povweb_zlib)

set_target_properties(povray PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

target_compile_options(povray PRIVATE
    -ffast-math
    # POV-Ray uses HUGE_VAL (infinity) as a sentinel, e.g. for unset camera
    # parameters; clang would otherwise fold such comparisons away.
    -fno-finite-math-only
    -Wno-multichar
    -Wno-write-strings
    -Wno-deprecated-declarations
    -Wno-deprecated-register
    -Wno-register
    -Wno-unused-command-line-argument
)

set(POVWEB_LINK_FLAGS
    ${POVWEB_COMMON_FLAGS}
    # Run main() on a pthread so that the thread hosting the module (a web
    # worker started by the front-end) stays free to service file system
    # and thread-creation requests from POV-Ray's own threads.
    -sPROXY_TO_PTHREAD=1
    "-sPTHREAD_POOL_SIZE=Math.min(navigator.hardwareConcurrency||4,32)+8"
    -sPTHREAD_POOL_SIZE_STRICT=0
    -sSTACK_SIZE=8MB
    -sDEFAULT_PTHREAD_STACK_SIZE=4MB
    -sALLOW_MEMORY_GROWTH=1
    -sINITIAL_MEMORY=256MB
    -sMAXIMUM_MEMORY=4GB
    -sMODULARIZE=1
    -sEXPORT_NAME=createPovray
    -sENVIRONMENT=web,worker
    -sINVOKE_RUN=0
    -sEXIT_RUNTIME=0
    -sFORCE_FILESYSTEM=1
    -sEXPORTED_RUNTIME_METHODS=callMain,FS,wasmMemory
    -sEXPORTED_FUNCTIONS=_main,_povweb_submit,_povweb_cancel,_povweb_framebuffer,_povweb_width,_povweb_height,_povweb_frame_serial,_povweb_current_frame,_povweb_total_frames
    # Standard include files and INI files, packaged as `povray.data`.
    # (`SHELL:` keeps CMake from de-duplicating the repeated flag.)
    "SHELL:--preload-file ${POV_ROOT}/distribution/include@/povray/include"
    "SHELL:--preload-file ${POV_ROOT}/distribution/ini@/povray/ini"
    "SHELL:--preload-file ${WEB_ROOT}/etc@/povray/etc"
    $<$<CONFIG:Release>:-O3>
)
target_link_options(povray PRIVATE ${POVWEB_LINK_FLAGS})

# Re-link when packaged data changes.
file(GLOB_RECURSE POVWEB_PACKAGED_FILES CONFIGURE_DEPENDS
    "${POV_ROOT}/distribution/include/*"
    "${POV_ROOT}/distribution/ini/*"
    "${WEB_ROOT}/etc/*"
)
set_property(TARGET povray APPEND PROPERTY LINK_DEPENDS ${POVWEB_PACKAGED_FILES})

#-------------------------------------------------------------------------------
# Assemble the static site
#-------------------------------------------------------------------------------

find_package(Python3 COMPONENTS Interpreter REQUIRED)

file(GLOB POVWEB_SHELL_FILES CONFIGURE_DEPENDS "${WEB_ROOT}/shell/*")
# index.html is written by stamp_version.py instead of being copied.
list(REMOVE_ITEM POVWEB_SHELL_FILES "${WEB_ROOT}/shell/index.html")

add_custom_target(site ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SITE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POVWEB_SHELL_FILES} "${SITE_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${POV_ROOT}/distribution/agpl-3.0.txt" "${SITE_DIR}/LICENSE.txt"
    COMMAND Python3::Interpreter -I "${WEB_ROOT}/tools/make_manifest.py"
            --scenes "${POV_ROOT}/distribution/scenes"
            --out "${SITE_DIR}"
    COMMAND Python3::Interpreter -I "${WEB_ROOT}/tools/stamp_version.py"
            --template "${WEB_ROOT}/shell/index.html"
            --site "${SITE_DIR}"
            --repo "${POV_ROOT}"
            --source-url "${POVWEB_SOURCE_URL}"
    DEPENDS povray ${POVWEB_SHELL_FILES} "${WEB_ROOT}/shell/index.html"
            "${WEB_ROOT}/tools/make_manifest.py" "${WEB_ROOT}/tools/stamp_version.py"
    COMMENT "Assembling web site in ${SITE_DIR}"
    VERBATIM
)
