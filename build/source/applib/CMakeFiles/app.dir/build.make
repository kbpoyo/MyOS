# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kbpoyo/code/cpp/MyOs

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kbpoyo/code/cpp/MyOs/build

# Include any dependencies generated for this target.
include source/applib/CMakeFiles/app.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include source/applib/CMakeFiles/app.dir/compiler_depend.make

# Include the progress variables for this target.
include source/applib/CMakeFiles/app.dir/progress.make

# Include the compile flags for this target's objects.
include source/applib/CMakeFiles/app.dir/flags.make

source/applib/CMakeFiles/app.dir/crt0.S.o: source/applib/CMakeFiles/app.dir/flags.make
source/applib/CMakeFiles/app.dir/crt0.S.o: ../source/applib/crt0.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object source/applib/CMakeFiles/app.dir/crt0.S.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/app.dir/crt0.S.o -c /home/kbpoyo/code/cpp/MyOs/source/applib/crt0.S

source/applib/CMakeFiles/app.dir/crt0.S.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing ASM source to CMakeFiles/app.dir/crt0.S.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/applib/crt0.S > CMakeFiles/app.dir/crt0.S.i

source/applib/CMakeFiles/app.dir/crt0.S.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling ASM source to assembly CMakeFiles/app.dir/crt0.S.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/applib/crt0.S -o CMakeFiles/app.dir/crt0.S.s

source/applib/CMakeFiles/app.dir/cstart.c.o: source/applib/CMakeFiles/app.dir/flags.make
source/applib/CMakeFiles/app.dir/cstart.c.o: ../source/applib/cstart.c
source/applib/CMakeFiles/app.dir/cstart.c.o: source/applib/CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object source/applib/CMakeFiles/app.dir/cstart.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/applib/CMakeFiles/app.dir/cstart.c.o -MF CMakeFiles/app.dir/cstart.c.o.d -o CMakeFiles/app.dir/cstart.c.o -c /home/kbpoyo/code/cpp/MyOs/source/applib/cstart.c

source/applib/CMakeFiles/app.dir/cstart.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/app.dir/cstart.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/applib/cstart.c > CMakeFiles/app.dir/cstart.c.i

source/applib/CMakeFiles/app.dir/cstart.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/app.dir/cstart.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/applib/cstart.c -o CMakeFiles/app.dir/cstart.c.s

# Object files for target app
app_OBJECTS = \
"CMakeFiles/app.dir/crt0.S.o" \
"CMakeFiles/app.dir/cstart.c.o"

# External object files for target app
app_EXTERNAL_OBJECTS =

source/applib/libapp.a: source/applib/CMakeFiles/app.dir/crt0.S.o
source/applib/libapp.a: source/applib/CMakeFiles/app.dir/cstart.c.o
source/applib/libapp.a: source/applib/CMakeFiles/app.dir/build.make
source/applib/libapp.a: source/applib/CMakeFiles/app.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libapp.a"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && $(CMAKE_COMMAND) -P CMakeFiles/app.dir/cmake_clean_target.cmake
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/app.dir/link.txt --verbose=$(VERBOSE)
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && objdump -x -d -S -m i386 /home/kbpoyo/code/cpp/MyOs/build/source/applib/libapp.a > app_dis.txt
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && readelf -a /home/kbpoyo/code/cpp/MyOs/build/source/applib/libapp.a > app_elf.txt

# Rule to build all files generated by this target.
source/applib/CMakeFiles/app.dir/build: source/applib/libapp.a
.PHONY : source/applib/CMakeFiles/app.dir/build

source/applib/CMakeFiles/app.dir/clean:
	cd /home/kbpoyo/code/cpp/MyOs/build/source/applib && $(CMAKE_COMMAND) -P CMakeFiles/app.dir/cmake_clean.cmake
.PHONY : source/applib/CMakeFiles/app.dir/clean

source/applib/CMakeFiles/app.dir/depend:
	cd /home/kbpoyo/code/cpp/MyOs/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kbpoyo/code/cpp/MyOs /home/kbpoyo/code/cpp/MyOs/source/applib /home/kbpoyo/code/cpp/MyOs/build /home/kbpoyo/code/cpp/MyOs/build/source/applib /home/kbpoyo/code/cpp/MyOs/build/source/applib/CMakeFiles/app.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : source/applib/CMakeFiles/app.dir/depend

