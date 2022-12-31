SECTION MBR vstart=0x7c00
  ; 将各个段寄存器都初始化为0x7c00相当于平坦模式，各个段都
  mov ax,cs ; 被加载到0x7c00代码段即为该地址
  mov ds,ax ;数据段
  mov es,ax ;es fs gs 都是附加段，gs这里作者应该没用到就没初始化
  mov ss,ax ;栈段
  mov fs,ax
  mov sp,0x7c00 ; 栈顶指针

  ; ah 是功能号，相当于函数参数， 表示将屏幕上卷
  ; al 为要上卷的行数，为0即全部，这里就是上卷全部相当于清屏
  mov ax,0x600
  mov bx,0x700
  mov cx,0
  mov dx,0x184f

  ; 调用bios中断
  int 0x10

  mov ah,3
  mov bh,0

  int 0x10

  mov ax,message
  mov bp,ax

  mov cx,5
  mov ax,0x1301
  mov bx,0x2

  int 0x10

  jmp $

  message db "1 MBR"
  times 510-($-$$) db 0
  db 0x55,0xaa
