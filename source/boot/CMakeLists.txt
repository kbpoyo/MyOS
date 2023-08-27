project(boot LANGUAGES C)

# 使用自定义的链接器,
# -Ttext=0x7c00  告诉连接器该程序加载到内存的段的地址, 不加也可以，因为bios会自动将第一扇区的内容加载到0x7c00，
# --section-start=<sectionname>=<org> 以-Ttext(必须有-T参数提前指定标准段的地址-Tbss=org，-Tdata=org，-Ttext=org)
# 指定的地址为参考，指定程序中boot_end段的相对地址并输出到编译的文件中,
# 如此处boot_end段表示以0x7c00为起始地址的512字节的最后两个字节
set(CMAKE_EXE_LINKER_FLAGS "-m elf_i386 -Ttext=0x7c00 --section-start=boot_end=0x7dfe")
set(CMAKE_C_LINK_EXECUTABLE "${LINKER_TOOL} <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} -o ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf")

# 将所有的汇编、C文件加入工程
# 注意保证start.asm在最前头，这样对应的程序会放在bin文件开头，从而在加载到0x7c00时能在开头处
file(GLOB C_LIST "*.c" "*.h")
add_executable(${PROJECT_NAME} start.S ${C_LIST})
# bin文件生成，写入到image目录下
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${OBJCOPY_TOOL} -O binary ${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/image/${PROJECT_NAME}.bin
                   COMMAND ${OBJDUMP_TOOL} -x -d -S -m i8086 ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_dis.txt
                   COMMAND ${READELF_TOOL} -a ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_elf.txt
)

# objcopy: 将目标文件进行格式转换到另一种类型的文件 
# 参数：-O指定文件类型 

# objdump: 显示目标文件的信息
# 参数： -x 显示所有program头和section头信息
#        -d 显示代码段的反汇编信息
#        -D 显示所有段的反汇编信息
#        -S 将源代码和反汇编信息一起显示
#       -m 指定机器的目标架构

# readelf: 显示elf文件的信息
# 参数 -a 显示全部信息

#ld: 链接器
#参数： -m <file_type>指定输出的可执行文件的类型，即可执行文件的指令架构
#       -T <file.lds>指定链接脚本
#       -Ttext = addr(-Tbss=addr，-Tdata=addr, -Trodata=addr), 指定段的起始地址   
#       --section-start=<sectionname>=addr 指定名字为sectionname的段的起始地址