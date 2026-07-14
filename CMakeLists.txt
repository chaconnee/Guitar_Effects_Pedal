cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME Guitar_Effect)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

enable_language(C ASM)

add_executable(${CMAKE_PROJECT_NAME}
        APP/AudioPipeline.c
        APP/AudioPipeline.h
        APP/mode_ctrl.c
        APP/mode_ctrl.h
        APP/encoder.c
        APP/encoder.h
        APP/tft.c
        APP/tft.h
        APP/model/effect_chain.c
        APP/model/effect_chain.h
        APP/model/effect.h
        APP/effects/amp_sim/amp_sim.c
        APP/effects/amp_sim/amp_sim.h
        APP/effects/cab_sim/cab_sim.c
        APP/effects/cab_sim/cab_sim.h
        APP/effects/reverb/reverb.c
        APP/effects/reverb/reverb.h
        "APP/effects/cab_sim/ir_AC30 brilliant+bass AT4033a stalevel_dc.h"
)

add_subdirectory(cmake/stm32cubemx)

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    APP/
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE)

list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
list(REMOVE_ITEM CMAKE_CXX_IMPLICIT_LINK_LIBRARIES ob)

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
)

add_custom_command(TARGET Guitar_Effect POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:Guitar_Effect> ${CMAKE_CURRENT_BINARY_DIR}/Guitar_Effect.hex
        COMMENT "Generating HEX file"
)
