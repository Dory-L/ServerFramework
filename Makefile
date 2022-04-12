# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/lly/projectfile/dory/workspace/CMakeFiles /home/lly/projectfile/dory/workspace/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/lly/projectfile/dory/workspace/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named test

# Build rule for target.
test: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test
.PHONY : test

# fast build rule for target.
test/fast:
	$(MAKE) -f CMakeFiles/test.dir/build.make CMakeFiles/test.dir/build
.PHONY : test/fast

#=============================================================================
# Target rules for targets named dory

# Build rule for target.
dory: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 dory
.PHONY : dory

# fast build rule for target.
dory/fast:
	$(MAKE) -f CMakeFiles/dory.dir/build.make CMakeFiles/dory.dir/build
.PHONY : dory/fast

dory/log.o: dory/log.cpp.o

.PHONY : dory/log.o

# target to build an object file
dory/log.cpp.o:
	$(MAKE) -f CMakeFiles/dory.dir/build.make CMakeFiles/dory.dir/dory/log.cpp.o
.PHONY : dory/log.cpp.o

dory/log.i: dory/log.cpp.i

.PHONY : dory/log.i

# target to preprocess a source file
dory/log.cpp.i:
	$(MAKE) -f CMakeFiles/dory.dir/build.make CMakeFiles/dory.dir/dory/log.cpp.i
.PHONY : dory/log.cpp.i

dory/log.s: dory/log.cpp.s

.PHONY : dory/log.s

# target to generate assembly for a file
dory/log.cpp.s:
	$(MAKE) -f CMakeFiles/dory.dir/build.make CMakeFiles/dory.dir/dory/log.cpp.s
.PHONY : dory/log.cpp.s

dory/util.o: dory/util.cpp.o

.PHONY : dory/util.o

# target to build an object file
dory/util.cpp.o:
	$(MAKE) -f CMakeFiles/dory.dir/build.make CMakeFiles/dory.dir/dory/util.cpp.o
.PHONY : dory/util.cpp.o

dory/util.i: dory/util.cpp.i

.PHONY : dory/util.i

# target to preprocess a source file
dory/util.cpp.i:
	$(MAKE) -f CMakeFiles/dory.dir/build.make CMakeFiles/dory.dir/dory/util.cpp.i
.PHONY : dory/util.cpp.i

dory/util.s: dory/util.cpp.s

.PHONY : dory/util.s

# target to generate assembly for a file
dory/util.cpp.s:
	$(MAKE) -f CMakeFiles/dory.dir/build.make CMakeFiles/dory.dir/dory/util.cpp.s
.PHONY : dory/util.cpp.s

tests/test.o: tests/test.cpp.o

.PHONY : tests/test.o

# target to build an object file
tests/test.cpp.o:
	$(MAKE) -f CMakeFiles/test.dir/build.make CMakeFiles/test.dir/tests/test.cpp.o
.PHONY : tests/test.cpp.o

tests/test.i: tests/test.cpp.i

.PHONY : tests/test.i

# target to preprocess a source file
tests/test.cpp.i:
	$(MAKE) -f CMakeFiles/test.dir/build.make CMakeFiles/test.dir/tests/test.cpp.i
.PHONY : tests/test.cpp.i

tests/test.s: tests/test.cpp.s

.PHONY : tests/test.s

# target to generate assembly for a file
tests/test.cpp.s:
	$(MAKE) -f CMakeFiles/test.dir/build.make CMakeFiles/test.dir/tests/test.cpp.s
.PHONY : tests/test.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... test"
	@echo "... dory"
	@echo "... dory/log.o"
	@echo "... dory/log.i"
	@echo "... dory/log.s"
	@echo "... dory/util.o"
	@echo "... dory/util.i"
	@echo "... dory/util.s"
	@echo "... tests/test.o"
	@echo "... tests/test.i"
	@echo "... tests/test.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

