# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

source/kernel/CMakeFiles/kernel.dir/core/memory.c.o: ../source/kernel/core/memory.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/core/memory.h \
  ../source/common/types.h \
  ../source/kernel/include/tools/bitmap.h \
  ../source/kernel/include/ipc/mutex.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/core/task.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/common/boot_info.h \
  ../source/common/types.h \
  ../source/kernel/include/tools/log.h

source/kernel/CMakeFiles/kernel.dir/core/task.c.o: ../source/kernel/core/task.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/core/task.h \
  ../source/common/types.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/tools/klib.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h \
  ../source/kernel/include/tools/log.h \
  ../source/common/cpu_instr.h \
  ../source/common/types.h \
  ../source/kernel/include/cpu/gdt.h \
  ../source/kernel/include/os_cfg.h \
  ../source/kernel/include/cpu/idt.h

source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o: ../source/kernel/cpu/gdt.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/cpu/gdt.h \
  ../source/common/types.h \
  ../source/common/cpu_instr.h \
  ../source/common/types.h \
  ../source/kernel/include/ipc/mutex.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/core/task.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/kernel/include/os_cfg.h

source/kernel/CMakeFiles/kernel.dir/cpu/idt.c.o: ../source/kernel/cpu/idt.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/cpu/idt.h \
  ../source/common/types.h \
  ../source/common/exc_frame.h \
  ../source/common/types.h \
  ../source/common/cpu_instr.h \
  ../source/kernel/include/os_cfg.h \
  ../source/kernel/include/tools/log.h

source/kernel/CMakeFiles/kernel.dir/dev/time.c.o: ../source/kernel/dev/time.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/dev/time.h \
  ../source/common/types.h \
  ../source/common/cpu_instr.h \
  ../source/common/types.h \
  ../source/common/exc_frame.h \
  ../source/kernel/include/os_cfg.h \
  ../source/kernel/include/cpu/idt.h \
  ../source/kernel/include/core/task.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h

source/kernel/CMakeFiles/kernel.dir/init/init.c.o: ../source/kernel/init/init.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/init/init.h \
  ../source/common/boot_info.h \
  ../source/common/types.h \
  ../source/common/cpu_instr.h \
  ../source/kernel/include/cpu/gdt.h \
  ../source/common/types.h \
  ../source/kernel/include/cpu/idt.h \
  ../source/kernel/include/tools/log.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/dev/time.h \
  ../source/kernel/include/core/task.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/os_cfg.h \
  ../source/kernel/include/test/test.h \
  ../source/kernel/include/tools/klib.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h \
  ../source/kernel/include/ipc/sem.h \
  ../source/kernel/include/core/memory.h \
  ../source/kernel/include/tools/bitmap.h \
  ../source/kernel/include/ipc/mutex.h

source/kernel/CMakeFiles/kernel.dir/ipc/mutex.c.o: ../source/kernel/ipc/mutex.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/ipc/mutex.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/core/task.h \
  ../source/common/types.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/kernel/include/cpu/idt.h

source/kernel/CMakeFiles/kernel.dir/ipc/sem.c.o: ../source/kernel/ipc/sem.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/ipc/sem.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/core/task.h \
  ../source/common/types.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/kernel/include/cpu/idt.h

source/kernel/CMakeFiles/kernel.dir/test/test.c.o: ../source/kernel/test/test.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/tools/log.h \
  ../source/common/types.h

source/kernel/CMakeFiles/kernel.dir/tools/assert.c.o: ../source/kernel/tools/assert.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/tools/assert.h \
  ../source/common/cpu_instr.h \
  ../source/common/types.h \
  ../source/kernel/include/tools/log.h

source/kernel/CMakeFiles/kernel.dir/tools/bitmap.c.o: ../source/kernel/tools/bitmap.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/tools/bitmap.h \
  ../source/common/types.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/tools/klib.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h

source/kernel/CMakeFiles/kernel.dir/tools/klib.c.o: ../source/kernel/tools/klib.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/tools/klib.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h \
  ../source/common/types.h

source/kernel/CMakeFiles/kernel.dir/tools/list.c.o: ../source/kernel/tools/list.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h

source/kernel/CMakeFiles/kernel.dir/tools/log.c.o: ../source/kernel/tools/log.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/tools/log.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h \
  ../source/common/types.h \
  ../source/common/cpu_instr.h \
  ../source/common/types.h \
  ../source/kernel/include/tools/klib.h \
  ../source/kernel/include/cpu/idt.h \
  ../source/kernel/include/ipc/mutex.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/core/task.h \
  ../source/kernel/include/cpu/tss.h


../source/kernel/tools/log.c:

../source/kernel/tools/bitmap.c:

../source/kernel/tools/assert.c:

../source/kernel/test/test.c:

../source/kernel/ipc/sem.c:

../source/kernel/include/core/memory.h:

../source/common/boot_info.h:

../source/kernel/include/cpu/tss.h:

../source/kernel/include/test/test.h:

../source/kernel/include/core/task.h:

/usr/include/stdc-predef.h:

../source/kernel/tools/list.c:

../source/kernel/include/tools/log.h:

../source/kernel/include/tools/klib.h:

../source/kernel/include/ipc/sem.h:

../source/kernel/cpu/gdt.c:

../source/kernel/include/ipc/mutex.h:

../source/kernel/core/task.c:

../source/kernel/init/init.c:

../source/kernel/include/tools/bitmap.h:

../source/kernel/cpu/idt.c:

../source/kernel/core/memory.c:

/usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h:

../source/kernel/include/tools/assert.h:

../source/common/types.h:

../source/kernel/include/cpu/gdt.h:

../source/common/cpu_instr.h:

../source/kernel/include/os_cfg.h:

../source/kernel/include/cpu/idt.h:

../source/common/exc_frame.h:

../source/kernel/include/tools/list.h:

../source/kernel/dev/time.c:

../source/kernel/tools/klib.c:

../source/kernel/include/dev/time.h:

../source/kernel/init/init.h:

../source/kernel/ipc/mutex.c:
