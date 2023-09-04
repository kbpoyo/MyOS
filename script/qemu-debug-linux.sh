# 适用于Linux
qemu-system-i386 -daemonize -m 128M -s -S -drive file=disk1.img,index=0,media=disk,format=raw -drive file=disk2.img,index=1,media=disk,format=raw -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation
#-drive参数指定磁盘，index参数 0，1 为primary bus 的master 和 slave盘
#2，3 为secondary bus 的master和slave盘
# 以下是您的qemu执行命令的参数的解释：
# - qemu-system-i386：使用qemu模拟一个32位的x86架构的系统¹。
# - -daemonize：让qemu以守护进程的方式运行，即在后台运行，不占用终端²。
# - -m 128M：为虚拟机分配128M的内存²。
# - -s：相当于-gdb tcp::1234，即在端口1234上开启gdbserver，等待gdb客户端连接²。
# - -S：让虚拟机在启动时暂停，等待gdb客户端连接后继续运行²。
# - -drive file=disk1.img,index=0,media=disk,format=raw：将disk1.img文件作为虚拟机的第一个（index=0）磁盘（media=disk），使用raw格式（format=raw）²。
# - -drive file=disk2.img,index=1,media=disk,format=raw：将disk2.img文件作为虚拟机的第二个（index=1）磁盘（media=disk），使用raw格式（format=raw）²。
# - -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation：设置qemu的调试选项，打印出以下事件的信息：
#     - pcall：当虚拟机执行特权指令时³。
#     - page：当虚拟机进行页表操作时³。
#     - mmu：当虚拟机进行内存管理单元（MMU）操作时³。
#     - cpu_reset：当虚拟机重置CPU时³。
#     - guest_errors：当虚拟机发生错误时³。
#     - trace:ps2_keyboard_set_translation：当虚拟机调用ps2_keyboard_set_translation函数时。
# 源: 与必应的对话， 2023/8/29
# (1) qemu常用参数选项说明_qemu 参数_Quard_D的博客-CSDN博客. https://blog.csdn.net/weixin_39871788/article/details/123250595.
# (2) Qemu的常用命令 - 知乎 - 知乎专栏. https://zhuanlan.zhihu.com/p/55691829.
# (3) QEMU. https://www.qemu.org/.
# qemu的磁盘还可以设置成哪些格式
# - raw：裸格式，占用空间较大，性能较好，但不支持虚拟机快照功能。
# - qcow2：写时复制（copy on write）格式，占用空间较小，支持快照，性能比raw稍差一些。
# - qcow：qcow2的前身，已经过时，不建议使用。
# - cow：写时复制（copy on write）格式，是qemu的早期格式，已经过时，不建议使用。
# - vdi：VirtualBox的磁盘格式，可以被qemu识别和转换。
# - vmdk：VMware的磁盘格式，可以被qemu识别和转换。
# - vpc：Virtual PC的磁盘格式，也称为vhd，可以被qemu识别和转换。
# - cloop：压缩循环（compressed loop）格式，主要用于Live CD镜像。
# 不同的磁盘格式有各自的优缺点和特性，您可以根据您的需求和场景选择合适的格式。如果您想了解更多关于qemu磁盘格式的信息，您可以参考这些网站¹²³。
# 源: 与必应的对话， 2023/8/29
# (1) qemu 磁盘格式cow、raw_weixin_33795093的博客-CSDN博客. https://blog.csdn.net/weixin_33795093/article/details/91911960.
# (2) 03-KVM虚拟磁盘格式及快照 - CSDN博客. https://blog.csdn.net/m0_46180357/article/details/108919515.
# (3) qemu磁盘格式raw和qcow2 – oVirt中文社区. https://www.cnovirt.com/archives/684.
# ps2_keyboard_set_translation函数是什么意思，这是一个很好的问题。根据我从网络上搜索到的信息，ps2_keyboard_set_translation函数是一个用于设置PS/2键盘的扫描码转换模式的函数¹。扫描码转换模式是指PS/2控制器将键盘发送的扫描码转换为不同的格式，以便操作系统识别和处理。PS/2控制器支持三种扫描码转换模式²：
# - Set 1：这是最早的扫描码格式，也称为XT模式，由IBM PC/XT使用。它使用一字节或两字节的扫描码表示每个按键的按下和释放。
# - Set 2：这是最常用的扫描码格式，也称为AT模式，由IBM PC/AT使用。它使用一字节或多字节的扫描码表示每个按键的按下和释放，但与Set 1不同，它使用E0或E1作为前缀字节来区分不同的按键。
# - Set 3：这是一种可编程的扫描码格式，由IBM PS/2使用。它使用一字节或两字节的扫描码表示每个按键的按下和释放，但与Set 1和Set 2不同，它不使用前缀字节，而是使用不同的扫描码值来区分不同的按键。
# ps2_keyboard_set_translation函数的作用是让PS/2控制器根据参数设置扫描码转换模式。参数可以是以下三个值之一¹：
# - 0：关闭扫描码转换，即保持键盘发送的原始扫描码格式。
# - 1：打开Set 1模式的扫描码转换，即将键盘发送的任何扫描码格式转换为Set 1模式。
# - 2：打开Set 2模式的扫描码转换，即将键盘发送的任何扫描码格式转换为Set 2模式。
# 希望这些信息对您有所帮助。如果您还有其他问题，欢迎继续与我聊天。😊
# 源: 与必应的对话， 2023/8/29
# (1) 键盘和鼠标类驱动程序的配置 - Windows drivers | Microsoft Learn. https://learn.microsoft.com/zh-cn/windows-hardware/drivers/hid/keyboard-and-mouse-class-drivers.
# (2) PS/2 Keyboard - OSDev Wiki. https://wiki.osdev.org/PS/2_Keyboard.
# (3) keyboard常用函数及基础用法 - 知乎 - 知乎专栏. https://zhuanlan.zhihu.com/p/625824340.