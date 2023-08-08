# 适用于Linux
qemu-system-i386 -daemonize -m 128M -s -S -drive file=disk1.img,index=0,media=disk,format=raw -drive file=disk2.img,index=1,media=disk,format=raw -d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation
#-drive参数指定磁盘，index参数 0，1 为primary bus 的master 和 slave盘
#2，3 为secondary bus 的master和slave盘
