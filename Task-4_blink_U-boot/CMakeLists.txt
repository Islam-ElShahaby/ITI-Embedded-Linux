cmake_minimum_required(VERSION 3.16)
project(blinky ASM C)

# Compiler flags
set(CMAKE_C_FLAGS   "-Wall -O2 -ffreestanding -nostdlib -nostartfiles -mcpu=cortex-a53")
set(CMAKE_ASM_FLAGS "-mcpu=cortex-a53")

# Sources
set(SOURCES
    startup.s
    main.c
)

# Linker script
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

# ELF target
add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -nostdlib
    -T ${LINKER_SCRIPT}
)

# Re-link when the linker script changes
set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    LINK_DEPENDS ${LINKER_SCRIPT}
)

# Binary image (.img)
set(IMG_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.img)

add_custom_command(
    TARGET  ${PROJECT_NAME}.elf
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${IMG_OUTPUT}
    COMMENT "Built ${PROJECT_NAME}.img — copy to SD boot partition, then in U-Boot:\n     fatload mmc 0:1 0x80000 ${PROJECT_NAME}.img\n     go 0x80000"
)
