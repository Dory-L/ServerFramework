# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lly/projectfile/dory/workspace

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lly/projectfile/dory/workspace

# Include any dependencies generated for this target.
include CMakeFiles/dory.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/dory.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dory.dir/flags.make

CMakeFiles/dory.dir/dory/log.cpp.o: CMakeFiles/dory.dir/flags.make
CMakeFiles/dory.dir/dory/log.cpp.o: dory/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lly/projectfile/dory/workspace/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/dory.dir/dory/log.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"dory/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dory.dir/dory/log.cpp.o -c /home/lly/projectfile/dory/workspace/dory/log.cpp

CMakeFiles/dory.dir/dory/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dory.dir/dory/log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lly/projectfile/dory/workspace/dory/log.cpp > CMakeFiles/dory.dir/dory/log.cpp.i

CMakeFiles/dory.dir/dory/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dory.dir/dory/log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lly/projectfile/dory/workspace/dory/log.cpp -o CMakeFiles/dory.dir/dory/log.cpp.s

CMakeFiles/dory.dir/dory/util.cpp.o: CMakeFiles/dory.dir/flags.make
CMakeFiles/dory.dir/dory/util.cpp.o: dory/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lly/projectfile/dory/workspace/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/dory.dir/dory/util.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"dory/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dory.dir/dory/util.cpp.o -c /home/lly/projectfile/dory/workspace/dory/util.cpp

CMakeFiles/dory.dir/dory/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dory.dir/dory/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lly/projectfile/dory/workspace/dory/util.cpp > CMakeFiles/dory.dir/dory/util.cpp.i

CMakeFiles/dory.dir/dory/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dory.dir/dory/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lly/projectfile/dory/workspace/dory/util.cpp -o CMakeFiles/dory.dir/dory/util.cpp.s

CMakeFiles/dory.dir/dory/config.cpp.o: CMakeFiles/dory.dir/flags.make
CMakeFiles/dory.dir/dory/config.cpp.o: dory/config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lly/projectfile/dory/workspace/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/dory.dir/dory/config.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"dory/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dory.dir/dory/config.cpp.o -c /home/lly/projectfile/dory/workspace/dory/config.cpp

CMakeFiles/dory.dir/dory/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dory.dir/dory/config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lly/projectfile/dory/workspace/dory/config.cpp > CMakeFiles/dory.dir/dory/config.cpp.i

CMakeFiles/dory.dir/dory/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dory.dir/dory/config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lly/projectfile/dory/workspace/dory/config.cpp -o CMakeFiles/dory.dir/dory/config.cpp.s

CMakeFiles/dory.dir/dory/Thread.cpp.o: CMakeFiles/dory.dir/flags.make
CMakeFiles/dory.dir/dory/Thread.cpp.o: dory/Thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lly/projectfile/dory/workspace/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/dory.dir/dory/Thread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"dory/Thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dory.dir/dory/Thread.cpp.o -c /home/lly/projectfile/dory/workspace/dory/Thread.cpp

CMakeFiles/dory.dir/dory/Thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dory.dir/dory/Thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/Thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lly/projectfile/dory/workspace/dory/Thread.cpp > CMakeFiles/dory.dir/dory/Thread.cpp.i

CMakeFiles/dory.dir/dory/Thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dory.dir/dory/Thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"dory/Thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lly/projectfile/dory/workspace/dory/Thread.cpp -o CMakeFiles/dory.dir/dory/Thread.cpp.s

# Object files for target dory
dory_OBJECTS = \
"CMakeFiles/dory.dir/dory/log.cpp.o" \
"CMakeFiles/dory.dir/dory/util.cpp.o" \
"CMakeFiles/dory.dir/dory/config.cpp.o" \
"CMakeFiles/dory.dir/dory/Thread.cpp.o"

# External object files for target dory
dory_EXTERNAL_OBJECTS =

lib/libdory.so: CMakeFiles/dory.dir/dory/log.cpp.o
lib/libdory.so: CMakeFiles/dory.dir/dory/util.cpp.o
lib/libdory.so: CMakeFiles/dory.dir/dory/config.cpp.o
lib/libdory.so: CMakeFiles/dory.dir/dory/Thread.cpp.o
lib/libdory.so: CMakeFiles/dory.dir/build.make
lib/libdory.so: CMakeFiles/dory.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lly/projectfile/dory/workspace/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX shared library lib/libdory.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dory.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dory.dir/build: lib/libdory.so

.PHONY : CMakeFiles/dory.dir/build

CMakeFiles/dory.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dory.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dory.dir/clean

CMakeFiles/dory.dir/depend:
	cd /home/lly/projectfile/dory/workspace && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lly/projectfile/dory/workspace /home/lly/projectfile/dory/workspace /home/lly/projectfile/dory/workspace /home/lly/projectfile/dory/workspace /home/lly/projectfile/dory/workspace/CMakeFiles/dory.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dory.dir/depend

