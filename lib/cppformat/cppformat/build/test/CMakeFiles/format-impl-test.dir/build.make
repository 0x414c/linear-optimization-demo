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
include test/CMakeFiles/format-impl-test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/format-impl-test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/format-impl-test.dir/flags.make

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj: test/CMakeFiles/format-impl-test.dir/flags.make
test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj: ../test/format-impl-test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj"
	cd /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test && /C/msys64/mingw64/bin/g++.exe   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj -c /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/test/format-impl-test.cc

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/format-impl-test.dir/format-impl-test.cc.i"
	cd /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test && /C/msys64/mingw64/bin/g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -E /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/test/format-impl-test.cc > CMakeFiles/format-impl-test.dir/format-impl-test.cc.i

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/format-impl-test.dir/format-impl-test.cc.s"
	cd /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test && /C/msys64/mingw64/bin/g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -S /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/test/format-impl-test.cc -o CMakeFiles/format-impl-test.dir/format-impl-test.cc.s

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.requires:

.PHONY : test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.requires

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.provides: test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.requires
	$(MAKE) -f test/CMakeFiles/format-impl-test.dir/build.make test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.provides.build
.PHONY : test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.provides

test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.provides.build: test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj


# Object files for target format-impl-test
format__impl__test_OBJECTS = \
"CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj"

# External object files for target format-impl-test
format__impl__test_EXTERNAL_OBJECTS =

bin/format-impl-test.exe: test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj
bin/format-impl-test.exe: test/CMakeFiles/format-impl-test.dir/build.make
bin/format-impl-test.exe: test/libtest-main.a
bin/format-impl-test.exe: libformat.a
bin/format-impl-test.exe: libgmock.a
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/format-impl-test.exe"
	cd /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test && /C/msys64/mingw64/bin/cmake.exe -E remove -f CMakeFiles/format-impl-test.dir/objects.a
	cd /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test && /C/msys64/mingw64/bin/ar.exe cr CMakeFiles/format-impl-test.dir/objects.a $(format__impl__test_OBJECTS) $(format__impl__test_EXTERNAL_OBJECTS)
	cd /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test && /C/msys64/mingw64/bin/g++.exe  -O3 -DNDEBUG   -Wl,--whole-archive CMakeFiles/format-impl-test.dir/objects.a -Wl,--no-whole-archive  -o ../bin/format-impl-test.exe -Wl,--major-image-version,0,--minor-image-version,0  libtest-main.a ../libformat.a ../libgmock.a -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 

# Rule to build all files generated by this target.
test/CMakeFiles/format-impl-test.dir/build: bin/format-impl-test.exe

.PHONY : test/CMakeFiles/format-impl-test.dir/build

test/CMakeFiles/format-impl-test.dir/requires: test/CMakeFiles/format-impl-test.dir/format-impl-test.cc.obj.requires

.PHONY : test/CMakeFiles/format-impl-test.dir/requires

test/CMakeFiles/format-impl-test.dir/clean:
	cd /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test && $(CMAKE_COMMAND) -P CMakeFiles/format-impl-test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/format-impl-test.dir/clean

test/CMakeFiles/format-impl-test.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MSYS Makefiles" /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/test /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test /C/Users/Alex/Dropbox/Development/Qt/LinearOptimization/lib/cppformat/cppformat/build/test/CMakeFiles/format-impl-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/format-impl-test.dir/depend

