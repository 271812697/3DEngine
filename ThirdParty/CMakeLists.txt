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

if(NOT TARGET ovanalytics)
    set(ovanalytics_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvAnalytics)
    file(GLOB_RECURSE ovanalytics_impl CONFIGURE_DEPENDS  
    "${ovanalytics_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovanalytics_head CONFIGURE_DEPENDS  
    "${ovanalytics_SOURCE_DIR_}/*.h")
    source_group(TREE "${ovanalytics_SOURCE_DIR_}" FILES ${ovanalytics_impl} ${ovanalytics_head})
    add_library(ovanalytics STATIC ${ovanalytics_impl} ${ovanalytics_head})
    target_include_directories(ovanalytics PUBLIC $<BUILD_INTERFACE:${ovanalytics_SOURCE_DIR_}/include>)
    set_target_properties(ovanalytics PROPERTIES FOLDER ${third_party_folder}/ovanalytics)
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

if(NOT TARGET ovdebug)
    set(ovdebug_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvDebug)
    file(GLOB_RECURSE ovdebug_impl CONFIGURE_DEPENDS  
    "${ovdebug_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovdebug_head CONFIGURE_DEPENDS  
    "${ovdebug_SOURCE_DIR_}/*.h" "${ovdebug_SOURCE_DIR_}/*.inl")
    source_group(TREE "${ovdebug_SOURCE_DIR_}" FILES ${ovdebug_impl} ${ovdebug_head})
    add_library(ovdebug STATIC ${ovdebug_impl} ${ovdebug_head})
    target_link_libraries(ovdebug PUBLIC ovtools)
    target_include_directories(ovdebug PUBLIC $<BUILD_INTERFACE:${ovdebug_SOURCE_DIR_}/include>)
    set_target_properties(ovdebug PROPERTIES FOLDER ${third_party_folder}/ovdebug)
endif()

if(NOT TARGET ovrendering)
    set(ovrendering_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvRendering)
    file(GLOB_RECURSE ovrendering_impl CONFIGURE_DEPENDS  
    "${ovrendering_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovrendering_head CONFIGURE_DEPENDS  
    "${ovrendering_SOURCE_DIR_}/*.h" "${ovrendering_SOURCE_DIR_}/*.inl")
    source_group(TREE "${ovrendering_SOURCE_DIR_}" FILES ${ovrendering_impl} ${ovrendering_head})
    add_library(ovrendering STATIC ${ovrendering_impl} ${ovrendering_head})
    target_link_libraries(ovrendering PUBLIC ovtools)
    target_link_libraries(ovrendering PUBLIC assimp)
    target_link_libraries(ovrendering PUBLIC glad)
    target_link_libraries(ovrendering PUBLIC std_image)
    target_link_libraries(ovrendering PUBLIC ovdebug)
    target_link_libraries(ovrendering PUBLIC ovmath)
    target_link_libraries(ovrendering PUBLIC glfw)
    target_include_directories(ovrendering PUBLIC $<BUILD_INTERFACE:${ovrendering_SOURCE_DIR_}/include>)
    set_target_properties(ovrendering PROPERTIES FOLDER ${third_party_folder}/ovrendering)
endif()

if(NOT TARGET ovaudio)
    set(ovaudio_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvAudio)
    file(GLOB_RECURSE ovaudio_impl CONFIGURE_DEPENDS  
    "${ovaudio_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovaudio_head CONFIGURE_DEPENDS  
    "${ovaudio_SOURCE_DIR_}/*.h" "${ovaudio_SOURCE_DIR_}/*.inl")
    source_group(TREE "${ovaudio_SOURCE_DIR_}" FILES ${ovaudio_impl} ${ovaudio_head})
    
    add_library(ovaudio STATIC ${ovaudio_impl} ${ovaudio_head})
    target_link_directories(ovaudio PUBLIC	${CMAKE_CURRENT_SOURCE_DIR}/irrklang/lib)
    target_link_libraries(ovaudio PUBLIC ovtools)
    target_link_libraries(ovaudio PUBLIC ovdebug)
    target_link_libraries(ovaudio PUBLIC ovmath)
    target_link_libraries(ovaudio PUBLIC irrKlang.lib)
    target_include_directories(ovaudio PUBLIC $<BUILD_INTERFACE:${ovaudio_SOURCE_DIR_}/include>)
    target_include_directories(ovaudio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/irrklang/include>)
    set_target_properties(ovaudio PROPERTIES FOLDER ${third_party_folder}/ovaudio)
endif()
if(NOT TARGET ovphysics)
    set(ovphysics_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvPhysics)
    file(GLOB_RECURSE ovphysics_impl CONFIGURE_DEPENDS  
    "${ovphysics_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE ovphysics_head CONFIGURE_DEPENDS  
    "${ovphysics_SOURCE_DIR_}/*.h" "${ovphysics_SOURCE_DIR_}/*.inl")
    file(GLOB_RECURSE ovphysics_lib CONFIGURE_DEPENDS  
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet3/lib/Debug/*.lib")
    source_group(TREE "${ovphysics_SOURCE_DIR_}" FILES ${ovphysics_impl} ${ovphysics_head})
    
    add_library(ovphysics STATIC ${ovphysics_impl} ${ovphysics_head})
    #target_link_directories(ovphysics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/bullet3/Debug/lib)
    target_link_libraries(ovphysics PUBLIC ovtools)
    target_link_libraries(ovphysics PUBLIC ovdebug)
    target_link_libraries(ovphysics PUBLIC ovmath)
    target_link_libraries(ovphysics PUBLIC ${ovphysics_lib})
    target_include_directories(ovphysics PUBLIC $<BUILD_INTERFACE:${ovphysics_SOURCE_DIR_}/include>)
    target_include_directories(ovphysics PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bullet3/include>)
    set_target_properties(ovphysics PROPERTIES FOLDER ${third_party_folder}/ovphysics)
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

if(NOT TARGET ovcore)
    set(ovcore_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/OvCore)
    file(GLOB_RECURSE ovcore_impl CONFIGURE_DEPENDS  
    "${ovcore_SOURCE_DIR_}/*.cpp" "${ovcore_SOURCE_DIR_}/*.inl")
    file(GLOB_RECURSE ovcore_head CONFIGURE_DEPENDS  
    "${ovcore_SOURCE_DIR_}/*.h")
    source_group(TREE "${ovcore_SOURCE_DIR_}" FILES ${ovcore_impl} ${ovcore_head})
    add_library(ovcore STATIC ${ovcore_impl} ${ovcore_head})
    target_link_libraries(ovcore PUBLIC glfw)
    target_link_libraries(ovcore PUBLIC lua)
    target_link_libraries(ovcore PUBLIC ovtools)
    target_link_libraries(ovcore PUBLIC std_image)
    target_link_libraries(ovcore PUBLIC ovphysics)
    target_link_libraries(ovcore PUBLIC glad)
    target_link_libraries(ovcore PUBLIC ovanalytics)
    target_link_libraries(ovcore PUBLIC ovaudio)
    target_link_libraries(ovcore PUBLIC ovdebug)
    target_link_libraries(ovcore PUBLIC ovmath)
    target_link_libraries(ovcore PUBLIC ovrendering)
    target_link_libraries(ovcore PUBLIC ovtools)
    target_link_libraries(ovcore PUBLIC ovui)
    target_link_libraries(ovcore PUBLIC ovwindow)
    target_include_directories(ovcore PUBLIC $<BUILD_INTERFACE:${ovcore_SOURCE_DIR_}/include>)
    set_target_properties(ovcore PROPERTIES FOLDER ${third_party_folder}/ovcore)
    target_compile_options(ovcore PRIVATE /bigobj )
    #target_compile_options(ovcore PRIVATE /bigobj )
endif()

if(NOT TARGET lua)
set(lua_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/lua)
add_library(lua STATIC 
${lua_SOURCE_DIR_}/lapi.c
${lua_SOURCE_DIR_}/lauxlib.c
${lua_SOURCE_DIR_}/lbaselib.c
${lua_SOURCE_DIR_}/lcode.c
${lua_SOURCE_DIR_}/lcorolib.c
${lua_SOURCE_DIR_}/lctype.c
${lua_SOURCE_DIR_}/ldblib.c
${lua_SOURCE_DIR_}/ldebug.c
${lua_SOURCE_DIR_}/ldo.c
${lua_SOURCE_DIR_}/ldump.c
${lua_SOURCE_DIR_}/lfunc.c
${lua_SOURCE_DIR_}/lgc.c
${lua_SOURCE_DIR_}/linit.c
${lua_SOURCE_DIR_}/liolib.c
${lua_SOURCE_DIR_}/llex.c
${lua_SOURCE_DIR_}/lmathlib.c
${lua_SOURCE_DIR_}/lmem.c
${lua_SOURCE_DIR_}/loadlib.c
${lua_SOURCE_DIR_}/lobject.c
${lua_SOURCE_DIR_}/lopcodes.c
${lua_SOURCE_DIR_}/loslib.c
${lua_SOURCE_DIR_}/lparser.c
${lua_SOURCE_DIR_}/lstate.c
${lua_SOURCE_DIR_}/lstring.c
${lua_SOURCE_DIR_}/lstrlib.c
${lua_SOURCE_DIR_}/ltable.c
${lua_SOURCE_DIR_}/ltablib.c
#${lua_SOURCE_DIR_}/ltests.c
${lua_SOURCE_DIR_}/ltm.c
#${lua_SOURCE_DIR_}/lua.c
${lua_SOURCE_DIR_}/lundump.c
${lua_SOURCE_DIR_}/lutf8lib.c
${lua_SOURCE_DIR_}/lvm.c
${lua_SOURCE_DIR_}/lzio.c
#${lua_SOURCE_DIR_}/onelua.c
)
target_include_directories(lua PUBLIC ${lua_SOURCE_DIR_})
set_target_properties(lua PROPERTIES FOLDER ${third_party_folder}/lua)
endif()

if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
endif()

if(NOT TARGET glm_static)
    add_subdirectory(glm)
    set_target_properties(glm_static PROPERTIES FOLDER ${third_party_folder}/glm_static)
endif()

if(NOT TARGET assimp)
    set(ASSIMP_BUILD_ZLIB ON)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
    set(ASSIMP_BUILD_TESTS OFF)
    set(ASSIMP_INSTALL OFF)
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
    add_subdirectory(assimp)
    set_target_properties(assimp PROPERTIES FOLDER ${third_party_folder}/Assimp)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER ${third_party_folder}/Assimp)
    set_target_properties(zlibstatic PROPERTIES FOLDER ${third_party_folder}/Assimp)
    #set_target_properties(assimp_cmd PROPERTIES FOLDER ${third_party_folder}/Assimp)


endif()

if(NOT TARGET opengl)
    set(opengl_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/Opengl)
    file(GLOB_RECURSE opengl_impl CONFIGURE_DEPENDS  
    "${opengl_SOURCE_DIR_}/*.cpp")
    file(GLOB_RECURSE opengl_head CONFIGURE_DEPENDS  
    "${opengl_SOURCE_DIR_}/*.h")
    source_group(TREE "${opengl_SOURCE_DIR_}" FILES ${opengl_impl} ${opengl_head})
    add_library(opengl STATIC ${opengl_impl} ${opengl_head})
    target_link_libraries(opengl PUBLIC glfw)
    target_link_libraries(opengl PUBLIC glad)
    target_link_libraries(opengl PUBLIC spdlog)
    target_link_libraries(opengl PUBLIC std_image)
    target_link_libraries(opengl PUBLIC glm_static)
    target_link_libraries(opengl PUBLIC assimp)
    target_include_directories(opengl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
    set_target_properties(opengl PROPERTIES FOLDER ${third_party_folder}/opengl)
endif()




