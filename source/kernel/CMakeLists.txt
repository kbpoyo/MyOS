project(kernel LANGUAGES C)

# 使用自定义的链接器,将内核加载到内存中0x100000处，即1M处
# set(CMAKE_EXE_LINKER_FLAGS "-m elf_i386 -Ttext=0x100000")
set(CMAKE_EXE_LINKER_FLAGS "-m elf_i386 -T ${PROJECT_SOURCE_DIR}/kernel.lds") #用脚本文件的方式加载
set(CMAKE_C_LINK_EXECUTABLE "${LINKER_TOOL} <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} -o ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf")

# 将所有的汇编、C文件加入工程
# 注意保证start.asm在最前头，这样对应的程序会放在bin文件开头，从而在加载到0x7c00时能在开头处
file(GLOB_RECURSE C_LIST "*.c" "*.h")
add_executable(${PROJECT_NAME} init/start.S ${C_LIST})
# bin文件生成，写入到image目录下， i386方式对32位程序进行反汇编, .elf只是为了配合脚本以及之后的使用，本质上还是纯二进制文件
# 当将各个段在内存中分隔加载时，二进制文件bin会特别大且没有文件的权限信息，所以直接用elf文件, -S参数对elf文件进行瘦身(不包含符号表)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${OBJCOPY_TOOL}  -S#[[-O binary]] ${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/image/${PROJECT_NAME}.elf
                   COMMAND ${OBJDUMP_TOOL} -x -d -S -m i386 ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_dis.txt
                   COMMAND ${READELF_TOOL} -a ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_elf.txt
)