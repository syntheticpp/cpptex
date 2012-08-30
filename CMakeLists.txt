
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


set(EXECUTABLE_OUTPUT_PATH   ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH      ${CMAKE_BINARY_DIR}/bin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(CMAKE_VERBOSE_MAKEFILE 1)


project(cpptex)


macro(prepend prefix outvar)
    set(${outvar})
    foreach(it ${ARGN})
        list(APPEND ${outvar} ${prefix}${it})
        set_source_files_properties(${prefix}${it} PROPERTIES LANGUAGE CXX)
    endforeach()
endmacro()


set(src
        align.c
        alloc.c
        arith.c
        box.c
        boxlist.c
        cells.c
        chr.c
        cmd.c
        cond.c
        def.c
        dvi.c
        error.c
        eval.c
        expand.c
        fileio.c
        hyph.c
        math.c
        mathlist.c
        mlst-hlst.c
        pack.c
        page.c
        par.c
        print.c
        scan.c
        str.c
        sym.c
        tex.c
        texext.c
        tfm.c
        tok.c
        toklist.c
        )

        
prepend(src/ src ${src})


file(GLOB headers src/*.h)


include_directories(src)


if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()


add_executable(cpptex ${src} ${headers})


