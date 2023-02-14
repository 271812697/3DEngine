set(TARGET_NAME PathTrace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Add source files ----
file(GLOB_RECURSE HEADER_FILES "*.h" "*.hpp")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE SHADER_FILES "*.glsl")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES})
# ---- Create library ----
add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES})


set(OIDN_LIBRARIES "OpenImageDenoise")
target_link_directories(${TARGET_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/oidn/lib
)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "PathTrace")


# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC opengl32.lib)
target_link_libraries(${TARGET_NAME} PUBLIC ovui)
target_link_libraries(${TARGET_NAME} PUBLIC ovwindow)
target_link_libraries(${TARGET_NAME} PUBLIC opengl)
target_link_libraries(${TARGET_NAME} PUBLIC ${OIDN_LIBRARIES})


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/oidn/lib/OpenImageDenoise.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/oidn/lib/tbb.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})










