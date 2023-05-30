import gdb

gdb.execute('target remote :1234')

gdb.execute('symbol-file ../build/source/boot/boot.elf')

# 设置临时断点
gdb.execute("tbreak *0x7c00")

# 继续执行
gdb.execute("continue")
