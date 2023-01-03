# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/cmake-build-debug"

# Utility rule file for update_mappings.

# Include any custom commands dependencies for this target.
include 3rdParty/glfw/src/CMakeFiles/update_mappings.dir/compiler_depend.make

# Include the progress variables for this target.
include 3rdParty/glfw/src/CMakeFiles/update_mappings.dir/progress.make

3rdParty/glfw/src/CMakeFiles/update_mappings:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir="/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Updating gamepad mappings from upstream repository"
	cd "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/3rdParty/glfw/src" && /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -P "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/3rdParty/glfw/CMake/GenerateMappings.cmake" mappings.h.in mappings.h

update_mappings: 3rdParty/glfw/src/CMakeFiles/update_mappings
update_mappings: 3rdParty/glfw/src/CMakeFiles/update_mappings.dir/build.make
.PHONY : update_mappings

# Rule to build all files generated by this target.
3rdParty/glfw/src/CMakeFiles/update_mappings.dir/build: update_mappings
.PHONY : 3rdParty/glfw/src/CMakeFiles/update_mappings.dir/build

3rdParty/glfw/src/CMakeFiles/update_mappings.dir/clean:
	cd "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/cmake-build-debug/3rdParty/glfw/src" && $(CMAKE_COMMAND) -P CMakeFiles/update_mappings.dir/cmake_clean.cmake
.PHONY : 3rdParty/glfw/src/CMakeFiles/update_mappings.dir/clean

3rdParty/glfw/src/CMakeFiles/update_mappings.dir/depend:
	cd "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project" "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/3rdParty/glfw/src" "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/cmake-build-debug" "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/cmake-build-debug/3rdParty/glfw/src" "/Users/lucas/Documents/ULB/MA2/INFO-H502 Virtual reality/INFO-H502 VR Project/cmake-build-debug/3rdParty/glfw/src/CMakeFiles/update_mappings.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : 3rdParty/glfw/src/CMakeFiles/update_mappings.dir/depend
