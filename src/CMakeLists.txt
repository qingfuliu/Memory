aux_source_directory(. MEMORYPOOL_SRC)

message("--------------------------${MEMORYPOOL_SRC}------------------------------")
include_directories(../include)

add_library(MEMORYPOOL SHARED ${MEMORYPOOL_SRC})


set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)