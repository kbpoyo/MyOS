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
  ../source/kernel/include/tools/log.h \
  ../source/kernel/include/tools/klib.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h \
  ../source/kernel/include/cpu/mmu.h \
  ../source/common/cpu_instr.h

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
  ../source/kernel/include/cpu/idt.h \
  ../source/kernel/include/core/memory.h \
  ../source/kernel/include/tools/bitmap.h \
  ../source/kernel/include/ipc/mutex.h \
  ../source/common/boot_info.h \
  ../source/kernel/include/cpu/mmu.h \
  ../source/kernel/include/cpu/syscall.h \
  ../source/common/elf.h \
  ../source/kernel/include/fs/fs.h

source/kernel/CMakeFiles/kernel.dir/cpu/gate.c.o: ../source/kernel/cpu/gate.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/cpu/gate.h \
  ../source/common/types.h

source/kernel/CMakeFiles/kernel.dir/cpu/gdt.c.o: ../source/kernel/cpu/gdt.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/cpu/gdt.h \
  ../source/common/types.h \
  ../source/kernel/include/cpu/gate.h \
  ../source/kernel/include/cpu/syscall.h \
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
  ../source/kernel/include/tools/log.h \
  ../source/kernel/include/cpu/gate.h

source/kernel/CMakeFiles/kernel.dir/cpu/mmu.c.o: ../source/kernel/cpu/mmu.c \
  /usr/include/stdc-predef.h

source/kernel/CMakeFiles/kernel.dir/cpu/syscall.c.o: ../source/kernel/cpu/syscall.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/cpu/syscall.h \
  ../source/common/types.h \
  ../source/kernel/include/core/memory.h \
  ../source/kernel/include/tools/bitmap.h \
  ../source/kernel/include/ipc/mutex.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/kernel/include/core/task.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/common/boot_info.h \
  ../source/common/types.h \
  ../source/kernel/include/tools/log.h \
  ../source/kernel/include/fs/fs.h

source/kernel/CMakeFiles/kernel.dir/dev/console.c.o: ../source/kernel/dev/console.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/dev/console.h \
  ../source/common/types.h

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

source/kernel/CMakeFiles/kernel.dir/fs/fs.c.o: ../source/kernel/fs/fs.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/fs/fs.h \
  ../source/kernel/include/tools/klib.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h \
  ../source/common/types.h \
  ../source/common/cpu_instr.h \
  ../source/common/types.h \
  ../source/common/boot_info.h \
  ../source/kernel/include/tools/log.h

source/kernel/CMakeFiles/kernel.dir/init/first_task.c.o: ../source/kernel/init/first_task.c \
  /usr/include/stdc-predef.h \
  ../source/kernel/include/tools/log.h \
  ../source/kernel/include/core/task.h \
  ../source/common/types.h \
  ../source/kernel/include/cpu/tss.h \
  ../source/kernel/include/tools/list.h \
  ../source/kernel/include/tools/assert.h \
  ../source/applib/lib_syscall.h \
  ../source/kernel/include/cpu/syscall.h \
  ../source/kernel/include/os_cfg.h \
  ../source/newlib/i686-elf/include/sys/stat.h \
  ../source/newlib/i686-elf/include/_ansi.h \
  ../source/newlib/i686-elf/include/newlib.h \
  ../source/newlib/i686-elf/include/_newlib_version.h \
  ../source/newlib/i686-elf/include/sys/config.h \
  ../source/newlib/i686-elf/include/machine/ieeefp.h \
  ../source/newlib/i686-elf/include/sys/features.h \
  ../source/newlib/i686-elf/include/time.h \
  ../source/newlib/i686-elf/include/_ansi.h \
  ../source/newlib/i686-elf/include/sys/cdefs.h \
  ../source/newlib/i686-elf/include/machine/_default_types.h \
  /usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h \
  ../source/newlib/i686-elf/include/sys/reent.h \
  ../source/newlib/i686-elf/include/sys/_types.h \
  ../source/newlib/i686-elf/include/machine/_types.h \
  ../source/newlib/i686-elf/include/sys/lock.h \
  ../source/newlib/i686-elf/include/machine/time.h \
  ../source/newlib/i686-elf/include/sys/types.h \
  ../source/newlib/i686-elf/include/sys/_stdint.h \
  ../source/newlib/i686-elf/include/machine/endian.h \
  ../source/newlib/i686-elf/include/machine/_endian.h \
  ../source/newlib/i686-elf/include/sys/select.h \
  ../source/newlib/i686-elf/include/sys/_sigset.h \
  ../source/newlib/i686-elf/include/sys/_timeval.h \
  ../source/newlib/i686-elf/include/sys/timespec.h \
  ../source/newlib/i686-elf/include/sys/_timespec.h \
  ../source/newlib/i686-elf/include/sys/_pthreadtypes.h \
  ../source/newlib/i686-elf/include/sys/sched.h \
  ../source/newlib/i686-elf/include/machine/types.h \
  ../source/newlib/i686-elf/include/sys/_locale.h

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
  ../source/kernel/include/ipc/mutex.h \
  ../source/kernel/include/dev/console.h

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

../source/kernel/tools/klib.c:

../source/kernel/test/test.c:

../source/kernel/ipc/sem.c:

../source/kernel/ipc/mutex.c:

../source/kernel/include/ipc/sem.h:

../source/newlib/i686-elf/include/sys/_locale.h:

../source/newlib/i686-elf/include/sys/sched.h:

../source/newlib/i686-elf/include/sys/_pthreadtypes.h:

../source/newlib/i686-elf/include/sys/_timespec.h:

../source/kernel/cpu/mmu.c:

../source/common/elf.h:

../source/kernel/tools/list.c:

../source/common/exc_frame.h:

../source/kernel/include/cpu/idt.h:

../source/newlib/i686-elf/include/sys/reent.h:

../source/newlib/i686-elf/include/machine/types.h:

../source/common/cpu_instr.h:

../source/kernel/include/cpu/gdt.h:

../source/kernel/include/tools/assert.h:

../source/common/types.h:

/usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h:

../source/newlib/i686-elf/include/time.h:

../source/kernel/cpu/idt.c:

../source/kernel/include/tools/log.h:

../source/kernel/tools/assert.c:

../source/newlib/i686-elf/include/newlib.h:

../source/kernel/include/tools/bitmap.h:

../source/newlib/i686-elf/include/sys/_stdint.h:

../source/kernel/include/cpu/gate.h:

../source/kernel/include/os_cfg.h:

../source/kernel/fs/fs.c:

../source/newlib/i686-elf/include/_ansi.h:

../source/newlib/i686-elf/include/sys/_types.h:

../source/kernel/tools/bitmap.c:

../source/kernel/core/task.c:

../source/kernel/include/cpu/syscall.h:

../source/kernel/include/core/memory.h:

/usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h:

../source/kernel/include/cpu/mmu.h:

../source/kernel/include/fs/fs.h:

../source/kernel/include/ipc/mutex.h:

../source/applib/lib_syscall.h:

/usr/include/stdc-predef.h:

../source/kernel/include/core/task.h:

../source/kernel/include/dev/console.h:

../source/newlib/i686-elf/include/sys/select.h:

../source/kernel/include/cpu/tss.h:

../source/newlib/i686-elf/include/sys/timespec.h:

../source/newlib/i686-elf/include/machine/_endian.h:

../source/common/boot_info.h:

../source/kernel/cpu/gdt.c:

../source/kernel/include/tools/klib.h:

../source/newlib/i686-elf/include/sys/cdefs.h:

../source/kernel/init/init.c:

../source/kernel/dev/console.c:

../source/newlib/i686-elf/include/machine/time.h:

../source/newlib/i686-elf/include/sys/_sigset.h:

../source/kernel/cpu/syscall.c:

../source/kernel/include/test/test.h:

../source/kernel/include/dev/time.h:

../source/kernel/init/first_task.c:

../source/kernel/dev/time.c:

../source/newlib/i686-elf/include/machine/ieeefp.h:

../source/newlib/i686-elf/include/sys/_timeval.h:

../source/kernel/core/memory.c:

../source/newlib/i686-elf/include/sys/stat.h:

../source/newlib/i686-elf/include/_newlib_version.h:

../source/kernel/cpu/gate.c:

../source/newlib/i686-elf/include/sys/config.h:

../source/newlib/i686-elf/include/sys/features.h:

../source/newlib/i686-elf/include/machine/_default_types.h:

../source/newlib/i686-elf/include/machine/_types.h:

../source/kernel/init/init.h:

../source/newlib/i686-elf/include/sys/types.h:

../source/newlib/i686-elf/include/sys/lock.h:

../source/kernel/include/tools/list.h:

../source/newlib/i686-elf/include/machine/endian.h:
