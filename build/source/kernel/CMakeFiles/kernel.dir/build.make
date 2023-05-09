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
include source/kernel/CMakeFiles/kernel.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include source/kernel/CMakeFiles/kernel.dir/compiler_depend.make

# Include the progress variables for this target.
include source/kernel/CMakeFiles/kernel.dir/progress.make

# Include the compile flags for this target's objects.
include source/kernel/CMakeFiles/kernel.dir/flags.make

source/kernel/CMakeFiles/kernel.dir/init/start.S.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/init/start.S.o: ../source/kernel/init/start.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object source/kernel/CMakeFiles/kernel.dir/init/start.S.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/kernel.dir/init/start.S.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/init/start.S

source/kernel/CMakeFiles/kernel.dir/init/start.S.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing ASM source to CMakeFiles/kernel.dir/init/start.S.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/init/start.S > CMakeFiles/kernel.dir/init/start.S.i

source/kernel/CMakeFiles/kernel.dir/init/start.S.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling ASM source to assembly CMakeFiles/kernel.dir/init/start.S.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/init/start.S -o CMakeFiles/kernel.dir/init/start.S.s

source/kernel/CMakeFiles/kernel.dir/core/memory.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/core/memory.c.o: ../source/kernel/core/memory.c
source/kernel/CMakeFiles/kernel.dir/core/memory.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object source/kernel/CMakeFiles/kernel.dir/core/memory.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/core/memory.c.o -MF CMakeFiles/kernel.dir/core/memory.c.o.d -o CMakeFiles/kernel.dir/core/memory.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/core/memory.c

source/kernel/CMakeFiles/kernel.dir/core/memory.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/core/memory.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/core/memory.c > CMakeFiles/kernel.dir/core/memory.c.i

source/kernel/CMakeFiles/kernel.dir/core/memory.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/core/memory.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/core/memory.c -o CMakeFiles/kernel.dir/core/memory.c.s

source/kernel/CMakeFiles/kernel.dir/core/task.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/core/task.c.o: ../source/kernel/core/task.c
source/kernel/CMakeFiles/kernel.dir/core/task.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object source/kernel/CMakeFiles/kernel.dir/core/task.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/core/task.c.o -MF CMakeFiles/kernel.dir/core/task.c.o.d -o CMakeFiles/kernel.dir/core/task.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/core/task.c

source/kernel/CMakeFiles/kernel.dir/core/task.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/core/task.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/core/task.c > CMakeFiles/kernel.dir/core/task.c.i

source/kernel/CMakeFiles/kernel.dir/core/task.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/core/task.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/core/task.c -o CMakeFiles/kernel.dir/core/task.c.s

source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.o: ../source/kernel/cpu/gate.c
source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.o -MF CMakeFiles/kernel.dir/cpu/gate.c.o.d -o CMakeFiles/kernel.dir/cpu/gate.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/gate.c

source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/cpu/gate.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/gate.c > CMakeFiles/kernel.dir/cpu/gate.c.i

source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/cpu/gate.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/gate.c -o CMakeFiles/kernel.dir/cpu/gate.c.s

source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o: ../source/kernel/cpu/gdt.c
source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o -MF CMakeFiles/kernel.dir/cpu/gdt.c.o.d -o CMakeFiles/kernel.dir/cpu/gdt.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/gdt.c

source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/cpu/gdt.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/gdt.c > CMakeFiles/kernel.dir/cpu/gdt.c.i

source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/cpu/gdt.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/gdt.c -o CMakeFiles/kernel.dir/cpu/gdt.c.s

source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.o: ../source/kernel/cpu/idt.c
source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.o -MF CMakeFiles/kernel.dir/cpu/idt.c.o.d -o CMakeFiles/kernel.dir/cpu/idt.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/idt.c

source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/cpu/idt.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/idt.c > CMakeFiles/kernel.dir/cpu/idt.c.i

source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/cpu/idt.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/idt.c -o CMakeFiles/kernel.dir/cpu/idt.c.s

source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.o: ../source/kernel/cpu/mmu.c
source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.o -MF CMakeFiles/kernel.dir/cpu/mmu.c.o.d -o CMakeFiles/kernel.dir/cpu/mmu.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/mmu.c

source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/cpu/mmu.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/mmu.c > CMakeFiles/kernel.dir/cpu/mmu.c.i

source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/cpu/mmu.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/cpu/mmu.c -o CMakeFiles/kernel.dir/cpu/mmu.c.s

source/kernel/CMakeFiles/kernel.dir/dev/time.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/dev/time.c.o: ../source/kernel/dev/time.c
source/kernel/CMakeFiles/kernel.dir/dev/time.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object source/kernel/CMakeFiles/kernel.dir/dev/time.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/dev/time.c.o -MF CMakeFiles/kernel.dir/dev/time.c.o.d -o CMakeFiles/kernel.dir/dev/time.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/dev/time.c

source/kernel/CMakeFiles/kernel.dir/dev/time.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/dev/time.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/dev/time.c > CMakeFiles/kernel.dir/dev/time.c.i

source/kernel/CMakeFiles/kernel.dir/dev/time.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/dev/time.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/dev/time.c -o CMakeFiles/kernel.dir/dev/time.c.s

source/kernel/CMakeFiles/kernel.dir/init/first_task.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/init/first_task.c.o: ../source/kernel/init/first_task.c
source/kernel/CMakeFiles/kernel.dir/init/first_task.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object source/kernel/CMakeFiles/kernel.dir/init/first_task.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/init/first_task.c.o -MF CMakeFiles/kernel.dir/init/first_task.c.o.d -o CMakeFiles/kernel.dir/init/first_task.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/init/first_task.c

source/kernel/CMakeFiles/kernel.dir/init/first_task.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/init/first_task.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/init/first_task.c > CMakeFiles/kernel.dir/init/first_task.c.i

source/kernel/CMakeFiles/kernel.dir/init/first_task.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/init/first_task.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/init/first_task.c -o CMakeFiles/kernel.dir/init/first_task.c.s

source/kernel/CMakeFiles/kernel.dir/init/first_task_entry.S.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/init/first_task_entry.S.o: ../source/kernel/init/first_task_entry.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building ASM object source/kernel/CMakeFiles/kernel.dir/init/first_task_entry.S.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/kernel.dir/init/first_task_entry.S.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/init/first_task_entry.S

source/kernel/CMakeFiles/kernel.dir/init/first_task_entry.S.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing ASM source to CMakeFiles/kernel.dir/init/first_task_entry.S.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/init/first_task_entry.S > CMakeFiles/kernel.dir/init/first_task_entry.S.i

source/kernel/CMakeFiles/kernel.dir/init/first_task_entry.S.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling ASM source to assembly CMakeFiles/kernel.dir/init/first_task_entry.S.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/init/first_task_entry.S -o CMakeFiles/kernel.dir/init/first_task_entry.S.s

source/kernel/CMakeFiles/kernel.dir/init/init.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/init/init.c.o: ../source/kernel/init/init.c
source/kernel/CMakeFiles/kernel.dir/init/init.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object source/kernel/CMakeFiles/kernel.dir/init/init.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/init/init.c.o -MF CMakeFiles/kernel.dir/init/init.c.o.d -o CMakeFiles/kernel.dir/init/init.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/init/init.c

source/kernel/CMakeFiles/kernel.dir/init/init.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/init/init.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/init/init.c > CMakeFiles/kernel.dir/init/init.c.i

source/kernel/CMakeFiles/kernel.dir/init/init.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/init/init.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/init/init.c -o CMakeFiles/kernel.dir/init/init.c.s

source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.o: ../source/kernel/ipc/mutex.c
source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.o -MF CMakeFiles/kernel.dir/ipc/mutex.c.o.d -o CMakeFiles/kernel.dir/ipc/mutex.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/ipc/mutex.c

source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/ipc/mutex.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/ipc/mutex.c > CMakeFiles/kernel.dir/ipc/mutex.c.i

source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/ipc/mutex.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/ipc/mutex.c -o CMakeFiles/kernel.dir/ipc/mutex.c.s

source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.o: ../source/kernel/ipc/sem.c
source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.o -MF CMakeFiles/kernel.dir/ipc/sem.c.o.d -o CMakeFiles/kernel.dir/ipc/sem.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/ipc/sem.c

source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/ipc/sem.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/ipc/sem.c > CMakeFiles/kernel.dir/ipc/sem.c.i

source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/ipc/sem.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/ipc/sem.c -o CMakeFiles/kernel.dir/ipc/sem.c.s

source/kernel/CMakeFiles/kernel.dir/test/test.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/test/test.c.o: ../source/kernel/test/test.c
source/kernel/CMakeFiles/kernel.dir/test/test.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object source/kernel/CMakeFiles/kernel.dir/test/test.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/test/test.c.o -MF CMakeFiles/kernel.dir/test/test.c.o.d -o CMakeFiles/kernel.dir/test/test.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/test/test.c

source/kernel/CMakeFiles/kernel.dir/test/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/test/test.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/test/test.c > CMakeFiles/kernel.dir/test/test.c.i

source/kernel/CMakeFiles/kernel.dir/test/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/test/test.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/test/test.c -o CMakeFiles/kernel.dir/test/test.c.s

source/kernel/CMakeFiles/kernel.dir/tools/assert.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/tools/assert.c.o: ../source/kernel/tools/assert.c
source/kernel/CMakeFiles/kernel.dir/tools/assert.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object source/kernel/CMakeFiles/kernel.dir/tools/assert.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/tools/assert.c.o -MF CMakeFiles/kernel.dir/tools/assert.c.o.d -o CMakeFiles/kernel.dir/tools/assert.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/assert.c

source/kernel/CMakeFiles/kernel.dir/tools/assert.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/tools/assert.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/assert.c > CMakeFiles/kernel.dir/tools/assert.c.i

source/kernel/CMakeFiles/kernel.dir/tools/assert.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/tools/assert.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/assert.c -o CMakeFiles/kernel.dir/tools/assert.c.s

source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.o: ../source/kernel/tools/bitmap.c
source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building C object source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.o -MF CMakeFiles/kernel.dir/tools/bitmap.c.o.d -o CMakeFiles/kernel.dir/tools/bitmap.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/bitmap.c

source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/tools/bitmap.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/bitmap.c > CMakeFiles/kernel.dir/tools/bitmap.c.i

source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/tools/bitmap.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/bitmap.c -o CMakeFiles/kernel.dir/tools/bitmap.c.s

source/kernel/CMakeFiles/kernel.dir/tools/klib.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/tools/klib.c.o: ../source/kernel/tools/klib.c
source/kernel/CMakeFiles/kernel.dir/tools/klib.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building C object source/kernel/CMakeFiles/kernel.dir/tools/klib.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/tools/klib.c.o -MF CMakeFiles/kernel.dir/tools/klib.c.o.d -o CMakeFiles/kernel.dir/tools/klib.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/klib.c

source/kernel/CMakeFiles/kernel.dir/tools/klib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/tools/klib.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/klib.c > CMakeFiles/kernel.dir/tools/klib.c.i

source/kernel/CMakeFiles/kernel.dir/tools/klib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/tools/klib.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/klib.c -o CMakeFiles/kernel.dir/tools/klib.c.s

source/kernel/CMakeFiles/kernel.dir/tools/list.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/tools/list.c.o: ../source/kernel/tools/list.c
source/kernel/CMakeFiles/kernel.dir/tools/list.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building C object source/kernel/CMakeFiles/kernel.dir/tools/list.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/tools/list.c.o -MF CMakeFiles/kernel.dir/tools/list.c.o.d -o CMakeFiles/kernel.dir/tools/list.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/list.c

source/kernel/CMakeFiles/kernel.dir/tools/list.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/tools/list.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/list.c > CMakeFiles/kernel.dir/tools/list.c.i

source/kernel/CMakeFiles/kernel.dir/tools/list.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/tools/list.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/list.c -o CMakeFiles/kernel.dir/tools/list.c.s

source/kernel/CMakeFiles/kernel.dir/tools/log.c.o: source/kernel/CMakeFiles/kernel.dir/flags.make
source/kernel/CMakeFiles/kernel.dir/tools/log.c.o: ../source/kernel/tools/log.c
source/kernel/CMakeFiles/kernel.dir/tools/log.c.o: source/kernel/CMakeFiles/kernel.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Building C object source/kernel/CMakeFiles/kernel.dir/tools/log.c.o"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT source/kernel/CMakeFiles/kernel.dir/tools/log.c.o -MF CMakeFiles/kernel.dir/tools/log.c.o.d -o CMakeFiles/kernel.dir/tools/log.c.o -c /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/log.c

source/kernel/CMakeFiles/kernel.dir/tools/log.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/kernel.dir/tools/log.c.i"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/log.c > CMakeFiles/kernel.dir/tools/log.c.i

source/kernel/CMakeFiles/kernel.dir/tools/log.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/kernel.dir/tools/log.c.s"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kbpoyo/code/cpp/MyOs/source/kernel/tools/log.c -o CMakeFiles/kernel.dir/tools/log.c.s

# Object files for target kernel
kernel_OBJECTS = \
"CMakeFiles/kernel.dir/init/start.S.o" \
"CMakeFiles/kernel.dir/core/memory.c.o" \
"CMakeFiles/kernel.dir/core/task.c.o" \
"CMakeFiles/kernel.dir/cpu/gate.c.o" \
"CMakeFiles/kernel.dir/cpu/gdt.c.o" \
"CMakeFiles/kernel.dir/cpu/idt.c.o" \
"CMakeFiles/kernel.dir/cpu/mmu.c.o" \
"CMakeFiles/kernel.dir/dev/time.c.o" \
"CMakeFiles/kernel.dir/init/first_task.c.o" \
"CMakeFiles/kernel.dir/init/first_task_entry.S.o" \
"CMakeFiles/kernel.dir/init/init.c.o" \
"CMakeFiles/kernel.dir/ipc/mutex.c.o" \
"CMakeFiles/kernel.dir/ipc/sem.c.o" \
"CMakeFiles/kernel.dir/test/test.c.o" \
"CMakeFiles/kernel.dir/tools/assert.c.o" \
"CMakeFiles/kernel.dir/tools/bitmap.c.o" \
"CMakeFiles/kernel.dir/tools/klib.c.o" \
"CMakeFiles/kernel.dir/tools/list.c.o" \
"CMakeFiles/kernel.dir/tools/log.c.o"

# External object files for target kernel
kernel_EXTERNAL_OBJECTS =

source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/init/start.S.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/core/memory.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/core/task.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/dev/time.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/init/first_task.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/init/first_task_entry.S.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/init/init.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/test/test.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/tools/assert.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/tools/klib.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/tools/list.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/tools/log.c.o
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/build.make
source/kernel/kernel: source/kernel/CMakeFiles/kernel.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kbpoyo/code/cpp/MyOs/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_20) "Linking C executable kernel"
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kernel.dir/link.txt --verbose=$(VERBOSE)
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && objcopy -S kernel.elf /home/kbpoyo/code/cpp/MyOs/image/kernel.elf
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && objdump -x -d -S -m i386 /home/kbpoyo/code/cpp/MyOs/build/source/kernel/kernel.elf > kernel_dis.txt
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && readelf -a /home/kbpoyo/code/cpp/MyOs/build/source/kernel/kernel.elf > kernel_elf.txt

# Rule to build all files generated by this target.
source/kernel/CMakeFiles/kernel.dir/build: source/kernel/kernel
.PHONY : source/kernel/CMakeFiles/kernel.dir/build

source/kernel/CMakeFiles/kernel.dir/clean:
	cd /home/kbpoyo/code/cpp/MyOs/build/source/kernel && $(CMAKE_COMMAND) -P CMakeFiles/kernel.dir/cmake_clean.cmake
.PHONY : source/kernel/CMakeFiles/kernel.dir/clean

source/kernel/CMakeFiles/kernel.dir/depend:
	cd /home/kbpoyo/code/cpp/MyOs/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kbpoyo/code/cpp/MyOs /home/kbpoyo/code/cpp/MyOs/source/kernel /home/kbpoyo/code/cpp/MyOs/build /home/kbpoyo/code/cpp/MyOs/build/source/kernel /home/kbpoyo/code/cpp/MyOs/build/source/kernel/CMakeFiles/kernel.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : source/kernel/CMakeFiles/kernel.dir/depend

