add_executable(main MACOSX_BUNDLE main.cpp yspng.h yspng.cpp ysglfontdata.h ysglfontdata.c)
target_link_libraries(main fssimplewindow yssimplesound)

message(STATUS "Copying media files")
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DEST_DIR ${CMAKE_BINARY_DIR}/)
file(COPY ${SOURCE_DIR}/media DESTINATION ${DEST_DIR})