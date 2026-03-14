set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_PROCESSOR  aarch64)

# Toolchain paths
set(TOOLCHAIN_PREFIX /home/mrdark/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-)

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_LINKER       ${TOOLCHAIN_PREFIX}ld)

# Bare-metal
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
