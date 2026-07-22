cmake_minimum_required(VERSION 3.18.1)

if (NOT DEFINED MODULE_NAME)
    set(MODULE_NAME "Stardust_PG3D") # Pastikan nama modul Anda benar
endif ()

project(${MODULE_NAME})

set(CMAKE_CXX_STANDARD 20)

set(LINKER_FLAGS "-ffixed-x18 -Wl,--hash-style=both")
set(C_FLAGS "-Werror=format -fdata-sections -ffunction-sections")
set(CXX_FLAGS "${CXX_FLAGS} -fno-exceptions -fno-rtti")

set(C_FLAGS "${C_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,-exclude-libs,ALL -Wl,--gc-sections -Wl,--strip-all")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS} ${CXX_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")

include_directories(KittyMemory)
include_directories(Include)
include_directories(Includes)
include_directories(ImGui)
include_directories(ImGui/backends)

add_library(${MODULE_NAME}
        SHARED
        ImGui/imgui.cpp
        ImGui/imgui_draw.cpp
        ImGui/imgui_demo.cpp
        ImGui/imgui_widgets.cpp
        ImGui/imgui_tables.cpp
        ImGui/backends/imgui_impl_opengl3.cpp
        ImGui/backends/imgui_impl_android.cpp
        KittyMemory/KittyArm64.cpp
        KittyMemory/KittyMemory.cpp
        KittyMemory/KittyScanner.cpp
        KittyMemory/KittyUtils.cpp
        KittyMemory/MemoryBackup.cpp
        KittyMemory/MemoryPatch.cpp
        main.cpp
        hook.cpp
        )

set(DOBBY_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libdobby.a)
set(CURL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libcurl.a)

target_link_libraries(
        ${MODULE_NAME} log
        ${DOBBY_LIB}
        ${CURL_LIB}
        GLESv3
        GLESv2
        android
        EGL  )

add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-all --remove-section=.comment "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${MODULE_NAME}.so")
