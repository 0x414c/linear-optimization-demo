# CMAKE generated file: DO NOT EDIT!
# Generated by "MSYS Makefiles" Generator, CMake Version 3.3

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
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
CMAKE_COMMAND = /C/msys64/mingw64/bin/cmake.exe

# The command to remove a file.
RM = /C/msys64/mingw64/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build

# Include any dependencies generated for this target.
include CMakeFiles/format.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/format.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/format.dir/flags.make

CMakeFiles/format.dir/format.cc.obj: CMakeFiles/format.dir/flags.make
CMakeFiles/format.dir/format.cc.obj: ../format.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/format.dir/format.cc.obj"
	/C/msys64/mingw64/bin/g++.exe   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/format.dir/format.cc.obj -c /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/format.cc

CMakeFiles/format.dir/format.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format.dir/format.cc.i"
	/C/msys64/mingw64/bin/g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -E /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/format.cc > CMakeFiles/format.dir/format.cc.i

CMakeFiles/format.dir/format.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format.dir/format.cc.s"
	/C/msys64/mingw64/bin/g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -S /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/format.cc -o CMakeFiles/format.dir/format.cc.s

CMakeFiles/format.dir/format.cc.obj.requires:

.PHONY : CMakeFiles/format.dir/format.cc.obj.requires

CMakeFiles/format.dir/format.cc.obj.provides: CMakeFiles/format.dir/format.cc.obj.requires
	$(MAKE) -f CMakeFiles/format.dir/build.make CMakeFiles/format.dir/format.cc.obj.provides.build
.PHONY : CMakeFiles/format.dir/format.cc.obj.provides

CMakeFiles/format.dir/format.cc.obj.provides.build: CMakeFiles/format.dir/format.cc.obj


CMakeFiles/format.dir/posix.cc.obj: CMakeFiles/format.dir/flags.make
CMakeFiles/format.dir/posix.cc.obj: ../posix.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/format.dir/posix.cc.obj"
	/C/msys64/mingw64/bin/g++.exe   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/format.dir/posix.cc.obj -c /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/posix.cc

CMakeFiles/format.dir/posix.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format.dir/posix.cc.i"
	/C/msys64/mingw64/bin/g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -E /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/posix.cc > CMakeFiles/format.dir/posix.cc.i

CMakeFiles/format.dir/posix.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format.dir/posix.cc.s"
	/C/msys64/mingw64/bin/g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -S /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/posix.cc -o CMakeFiles/format.dir/posix.cc.s

CMakeFiles/format.dir/posix.cc.obj.requires:

.PHONY : CMakeFiles/format.dir/posix.cc.obj.requires

CMakeFiles/format.dir/posix.cc.obj.provides: CMakeFiles/format.dir/posix.cc.obj.requires
	$(MAKE) -f CMakeFiles/format.dir/build.make CMakeFiles/format.dir/posix.cc.obj.provides.build
.PHONY : CMakeFiles/format.dir/posix.cc.obj.provides

CMakeFiles/format.dir/posix.cc.obj.provides.build: CMakeFiles/format.dir/posix.cc.obj


# Object files for target format
format_OBJECTS = \
"CMakeFiles/format.dir/format.cc.obj" \
"CMakeFiles/format.dir/posix.cc.obj"

# External object files for target format
format_EXTERNAL_OBJECTS =

libformat.a: CMakeFiles/format.dir/format.cc.obj
libformat.a: CMakeFiles/format.dir/posix.cc.obj
libformat.a: CMakeFiles/format.dir/build.make
libformat.a: CMakeFiles/format.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libformat.a"
	$(CMAKE_COMMAND) -P CMakeFiles/format.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/format.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/format.dir/build: libformat.a

.PHONY : CMakeFiles/format.dir/build

CMakeFiles/format.dir/requires: CMakeFiles/format.dir/format.cc.obj.requires
CMakeFiles/format.dir/requires: CMakeFiles/format.dir/posix.cc.obj.requires

.PHONY : CMakeFiles/format.dir/requires

CMakeFiles/format.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/format.dir/cmake_clean.cmake
.PHONY : CMakeFiles/format.dir/clean

CMakeFiles/format.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MSYS Makefiles" /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/CMakeFiles/format.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/format.dir/depend
