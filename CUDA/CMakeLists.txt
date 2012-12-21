cmake_minimum_required(VERSION 2.8.0)

set(CMAKE_CXX_FLAGS "-g -Wall")

# Note set CUDA_BUILD_EMULATION to OFF to enable Device mode compilation.
set(CUDA_SDK /usr/local/src/NVIDIA_CUDA_SDK)
set(CUDA_INC ${CUDA_SDK}/common/inc/)

find_package(CUDA QUIET REQUIRED) #Any version OK

if (CUDA_FOUND)
   message("CUDA found")
else()
   message("CUDA not found")
endif()

cuda_include_directories(${CUDA_INC} ${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${CUDA_SDK}/lib/)

set(HDRS book.h)

#CUDA!
cuda_add_library(timers timer.h timer.cu)
cuda_add_library(book book.h book.cu)

set(EASYPROGS hello_world simple_kernel
	simple_kernel_params simple_device_call enum_gpu set_gpu
	add_loop_gpu add_loop_long maxval)

foreach(PROG ${EASYPROGS})
	cuda_add_executable(${PROG} ${PROG}.cu ${HDRS} )
	target_link_libraries(${PROG} book timers)
endforeach(PROG)

add_executable(add_loop_cpu add_loop_cpu.cpp)

# Add a special target to clean nvcc generated files.
cuda_build_clean_target()
