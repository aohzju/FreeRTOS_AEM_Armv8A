cmake_minimum_required(VERSION 3.15)

#Cross compiling toolchain:
set(CMAKE_TOOLCHAIN_FILE "toolchain.armclang" CACHE PATH "")

#top project:
project(freertosDemo VERSION 1.0
			LANGUAGES C CXX ASM)

set(appName freertosDemo)

#the top app target:
add_executable(${appName}
				../../Source/tasks.c
				../../Source/timers.c
				../../Source/queue.c
				../../Source/event_groups.c
				../../Source/list.c
				../../Source/stream_buffer.c
				../../Source/croutine.c
				../../Source/portable/ARMClang/Armv8-A_aarch64/port.c
				../../Source/portable/ARMClang/Armv8-A_aarch64/portASM.S
				../../Source/portable/MemMang/heap_4.c
				bsp/basehw.S
				bsp/c_ex_handlers.c
				bsp/gicv3.c
				bsp/reset.S
				bsp/uart.c
				bsp/vectors.S
				main.c
				app/demo1.c
				)

set_source_files_properties( ../../Source/portable/ARMClang/Armv8-A_aarch64/portASM.S PROPERTIES COMPILE_OPTIONS --target=aarch64-arm-none-eabi)
set_source_files_properties(bsp/reset.S   PROPERTIES COMPILE_OPTIONS --target=aarch64-arm-none-eabi)
set_source_files_properties(bsp/vectors.S PROPERTIES COMPILE_OPTIONS --target=aarch64-arm-none-eabi)
set_source_files_properties(bsp/basehw.S PROPERTIES COMPILE_OPTIONS --target=aarch64-arm-none-eabi)

target_include_directories(${appName} PRIVATE
							${CMAKE_CURRENT_BINARY_DIR}
							../../Source/include
							../../Source/portable/ARMClang/Armv8-A_aarch64
							bsp
							./)

target_compile_options(${appName} PRIVATE -g -O0)
target_link_options(${appName} PRIVATE --scatter=${CMAKE_CURRENT_SOURCE_DIR}/AEMv8A.scat --entry reset_handler --map)
set_target_properties(${appName} PROPERTIES SUFFIX .axf)

