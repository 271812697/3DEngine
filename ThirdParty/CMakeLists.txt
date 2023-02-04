set(third_party_folder "ThirdParty")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


if(NOT TARGET imgui)
    set(imgui_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    file(GLOB imgui_sources CONFIGURE_DEPENDS  "${imgui_SOURCE_DIR_}/*.cpp")
    file(GLOB imgui_impl CONFIGURE_DEPENDS  
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.h"
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_opengl3.cpp" 
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_opengl3.h"
    "${imgui_SOURCE_DIR_}/backends/imgui_impl_opengl3_loader.h")
    add_library(imgui STATIC ${imgui_sources} ${imgui_impl})
    target_link_libraries(imgui PUBLIC glfw)
    target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${imgui_SOURCE_DIR_}>)
    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
endif()
if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()
if(NOT TARGET glad)
    set(glad_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/glad)
    file(GLOB_RECURSE glad_impl CONFIGURE_DEPENDS  
    "${glad_SOURCE_DIR_}/*.c")
    file(GLOB_RECURSE glad_head CONFIGURE_DEPENDS  
    "${glad_SOURCE_DIR_}/*.h")
    add_library(glad STATIC ${glad_impl} ${glad_head})
    target_include_directories(glad PUBLIC $<BUILD_INTERFACE:${glad_SOURCE_DIR_}>)
    set_target_properties(glad PROPERTIES FOLDER ${third_party_folder}/glad)
endif()
if(NOT TARGET ovui)
    set(ovui_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvUI)
    file(GLOB_RECURSE ovui_impl CONFIGURE_DEPENDS  
    "${ovui_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovui_head CONFIGURE_DEPENDS  
    "${ovui_SOURCE_DIR_}/*.h")
    source_group(TREE "${ovui_SOURCE_DIR_}" FILES ${ovui_impl} ${ovui_head})
    add_library(ovui STATIC ${ovui_impl} ${ovui_head})
    target_link_libraries(ovui PUBLIC imgui)
    target_link_libraries(ovui PUBLIC ovmath)
    target_link_libraries(ovui PUBLIC ovtools)
    target_include_directories(ovui PUBLIC $<BUILD_INTERFACE:${ovui_SOURCE_DIR_}/include>)
    set_target_properties(ovui PROPERTIES FOLDER ${third_party_folder}/ovui)
endif()

if(NOT TARGET ovmath)
    set(ovmath_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvMaths)
    file(GLOB_RECURSE ovmath_impl CONFIGURE_DEPENDS  
    "${ovmath_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovmath_head CONFIGURE_DEPENDS  
    "${ovmath_SOURCE_DIR_}/*.h")
    source_group(TREE "${ovmath_SOURCE_DIR_}" FILES ${ovmath_impl} ${ovmath_head})
    add_library(ovmath STATIC ${ovmath_impl} ${ovmath_head})
    target_include_directories(ovmath PUBLIC $<BUILD_INTERFACE:${ovmath_SOURCE_DIR_}/include>)
    set_target_properties(ovmath PROPERTIES FOLDER ${third_party_folder}/ovmath)
endif()

if(NOT TARGET ovtools)
    set(ovtools_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvTools)
    file(GLOB_RECURSE ovtools_impl CONFIGURE_DEPENDS  
    "${ovtools_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovtools_head CONFIGURE_DEPENDS  
    "${ovtools_SOURCE_DIR_}/*.h")
    source_group(TREE "${ovtools_SOURCE_DIR_}" FILES ${ovtools_impl} ${ovtools_head})
    add_library(ovtools STATIC ${ovtools_impl} ${ovtools_head})
    target_include_directories(ovtools PUBLIC $<BUILD_INTERFACE:${ovtools_SOURCE_DIR_}/include>)
    set_target_properties(ovtools PROPERTIES FOLDER ${third_party_folder}/ovtools)
endif()
if(NOT TARGET std_image)
    set(std_image_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/stb_Image)
    file(GLOB_RECURSE std_image_impl CONFIGURE_DEPENDS  
    "${std_image_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE std_image_head CONFIGURE_DEPENDS  
    "${std_image_SOURCE_DIR_}/*.h")
    source_group(TREE "${std_image_SOURCE_DIR_}" FILES ${std_image_head} ${std_image_impl})
    add_library(std_image STATIC ${std_image_head} ${std_image_impl})
    set_target_properties(std_image PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(std_image PUBLIC $<BUILD_INTERFACE:${std_image_SOURCE_DIR_}/include>)
    set_target_properties(std_image PROPERTIES FOLDER ${third_party_folder}/std_image)
endif()
if(NOT TARGET ovwindow)
    set(ovwindow_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvWindowing)
    file(GLOB_RECURSE ovwindow_impl CONFIGURE_DEPENDS  
    "${ovwindow_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovwindow_head CONFIGURE_DEPENDS  
    "${ovwindow_SOURCE_DIR_}/*.h")
    source_group(TREE "${ovwindow_SOURCE_DIR_}" FILES ${ovwindow_impl} ${ovwindow_head})
    add_library(ovwindow STATIC ${ovwindow_impl} ${ovwindow_head})
    target_link_libraries(ovwindow PUBLIC glfw)
    target_link_libraries(ovwindow PUBLIC ovtools)
    target_link_libraries(ovwindow PUBLIC std_image)
    target_include_directories(ovwindow PUBLIC $<BUILD_INTERFACE:${ovwindow_SOURCE_DIR_}/include>)
    set_target_properties(ovwindow PROPERTIES FOLDER ${third_party_folder}/ovwindow)
endif()




