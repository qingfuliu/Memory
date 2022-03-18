aux_source_directory(./ TEST_SRC)

include_directories(${PROJECT_SOURCE_DIR}/include)

foreach(loopVar ${TEST_SRC})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" FILE_NAME ${loopVar})

    add_executable(${FILE_NAME} ${loopVar})

    target_link_libraries(${FILE_NAME} 
                                    MEMORYPOOL)

    add_dependencies(${FILE_NAME} MEMORYPOOL)
endforeach()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)