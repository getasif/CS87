# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/shwang1/cs87/CUDA

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/shwang1/cs87/CUDA

# Include any dependencies generated for this target.
include CMakeFiles/set_gpu.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/set_gpu.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/set_gpu.dir/flags.make

./set_gpu_generated_set_gpu.cu.o: CMakeFiles/set_gpu_generated_set_gpu.cu.o.cmake
./set_gpu_generated_set_gpu.cu.o: set_gpu.cu
	$(CMAKE_COMMAND) -E cmake_progress_report /home/shwang1/cs87/CUDA/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Building NVCC (Device) object ./set_gpu_generated_set_gpu.cu.o"
	/usr/bin/cmake -E make_directory /home/shwang1/cs87/CUDA/.
	/usr/bin/cmake -D verbose:BOOL=$(VERBOSE) -D build_configuration:STRING= -D generated_file:STRING=/home/shwang1/cs87/CUDA/./set_gpu_generated_set_gpu.cu.o -D generated_cubin_file:STRING=/home/shwang1/cs87/CUDA/./set_gpu_generated_set_gpu.cu.o.cubin.txt -P /home/shwang1/cs87/CUDA/CMakeFiles/set_gpu_generated_set_gpu.cu.o.cmake

# Object files for target set_gpu
set_gpu_OBJECTS =

# External object files for target set_gpu
set_gpu_EXTERNAL_OBJECTS = \
"/home/shwang1/cs87/CUDA/./set_gpu_generated_set_gpu.cu.o"

set_gpu: /usr/local/lib/libcudart.so
set_gpu: /usr/lib/libcuda.so
set_gpu: libbook.a
set_gpu: libtimers.a
set_gpu: /usr/local/lib/libcudart.so
set_gpu: /usr/lib/libcuda.so
set_gpu: CMakeFiles/set_gpu.dir/build.make
set_gpu: ./set_gpu_generated_set_gpu.cu.o
set_gpu: CMakeFiles/set_gpu.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable set_gpu"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/set_gpu.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/set_gpu.dir/build: set_gpu
.PHONY : CMakeFiles/set_gpu.dir/build

CMakeFiles/set_gpu.dir/requires:
.PHONY : CMakeFiles/set_gpu.dir/requires

CMakeFiles/set_gpu.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/set_gpu.dir/cmake_clean.cmake
.PHONY : CMakeFiles/set_gpu.dir/clean

CMakeFiles/set_gpu.dir/depend: ./set_gpu_generated_set_gpu.cu.o
	cd /home/shwang1/cs87/CUDA && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/shwang1/cs87/CUDA /home/shwang1/cs87/CUDA /home/shwang1/cs87/CUDA /home/shwang1/cs87/CUDA /home/shwang1/cs87/CUDA/CMakeFiles/set_gpu.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/set_gpu.dir/depend
