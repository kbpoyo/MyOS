/**
 * @file keyboard.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief 键盘设备
 * @version 0.1
 * @date 2023-06-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dev/keyboard.h"
#include "cpu/idt.h"
#include "common/exc_frame.h"
#include "tools/log.h"
#include "common/cpu_instr.h"
#include "tools/klib.h"
#include "dev/tty.h"
#include "core/task.h"
/**
 * 键盘映射表，分3类
 * normal是没有shift键按下，或者没有numlock按下时默认的键值
 * func是按下shift或者numlock按下时的键值
 * esc是以esc开头的的键值
 */
static const key_map_t map_table[256] = {
        [0x2] = {'1', '!'},
        [0x3] = {'2', '@'},
        [0x4] = {'3', '#'},
        [0x5] = {'4', '$'},
        [0x6] = {'5', '%'},
        [0x7] = {'6', '^'},
        [0x08] = {'7', '&'},
        [0x09] = {'8', '*' },
        [0x0A] = {'9', '('},
        [0x0B] = {'0', ')'},
        [0x0C] = {'-', '_'},
        [0x0D] = {'=', '+'},
        [0x0E] = {0x7f, 0x7f},  //退格键
        [0x0F] = {'\t', '\t'},
        [0x10] = {'q', 'Q'},
        [0x11] = {'w', 'W'},
        [0x12] = {'e', 'E'},
        [0x13] = {'r', 'R'},
        [0x14] = {'t', 'T'},
        [0x15] = {'y', 'Y'},
        [0x16] = {'u', 'U'},
        [0x17] = {'i', 'I'},
        [0x18] = {'o', 'O'},
        [0x19] = {'p', 'P'},
        [0x1A] = {'[', '{'},
        [0x1B] = {']', '}'},
        [0x1C] = {'\n', '\n'},
        [0x1E] = {'a', 'A'},
        [0x1F] = {'s', 'B'},
        [0x20] = {'d',  'D'},
        [0x21] = {'f', 'F'},
        [0x22] = {'g', 'G'},
        [0x23] = {'h', 'H'},
        [0x24] = {'j', 'J'},
        [0x25] = {'k', 'K'},
        [0x26] = {'l', 'L'},
        [0x27] = {';', ':'},
        [0x28] = {'\'', '"'},
        [0x29] = {'`', '~'},
        [0x2B] = {'\\', '|'},
        [0x2C] = {'z', 'Z'},
        [0x2D] = {'x', 'X'},
        [0x2E] = {'c', 'C'},
        [0x2F] = {'v', 'V'},
        [0x30] = {'b', 'B'},
        [0x31] = {'n', 'N'},
        [0x32] = {'m', 'M'},
        [0x33] = {',', '<'},
        [0x34] = {'.', '>'},
        [0x35] = {'/', '?'},
        [0x39] = {' ', ' '},
};

//记录键盘状态
static kbd_state_t kbd_state;


/**
 * @brief 判断按键是按下还是弹起
 * 
 * @param key_code 
 * @return int 
 */
static inline int is_make_code(uint8_t key_code) {
    return !(key_code & 0x80);  //key_code的第7位为1则为按下，为0则为弹起
}

/**
 * @brief 获取真正的键值码，用来映射键值
 * 
 * @param key_code 
 * @return int 
 */
static inline int get_key(uint8_t key_code) {
    return key_code & 0x7f; //key_code的低7位为键值码
}


/**
 * @brief 对F1~F8功能键进行处理
 * 
 * @param key 
 */
static void do_fx_key(char key) {
    int index = key - KEY_F1;
    //在ctr键按下的情况下才进行处理
    if (kbd_state.lctrl_press || kbd_state.rctrl_press) {
        tty_select(index);
    }
}

/**
 * @brief 处理键盘按键按下时得到的原码
 *  
 * @param key_code 键按下时得到的原码
 */
static void do_normal_key(uint8_t key_code) {
    char key = get_key(key_code);
    int is_make = is_make_code(key_code);

    //处理对应按键
    switch (key) {
        case KEY_RSHIFT:
            kbd_state.rshift_press = is_make ? 1 : 0;
            break;
        
        case KEY_LSHIFT:
            kbd_state.lshift_press = is_make ? 1 : 0;
            break;  
        case KEY_CAPS:
            if (is_make) {
                kbd_state.caps_lock = ~kbd_state.caps_lock;
            }
            break;
        
        case KEY_ALT:
            kbd_state.lalt_press = is_make ? 1 : 0;
            break;
        case KEY_CTRL:
            kbd_state.lctrl_press = is_make ? 1 : 0;
            break;
        
        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
        case KEY_F6:
        case KEY_F7:
        case KEY_F8:
            do_fx_key(key);
            break;
        case KEY_F9:
            break;
        case KEY_F10:
            break;
        case KEY_F11:
            break;
        case KEY_F12:
            break;

        default:    //处理可被映射的非功能键
            if (is_make) {
                    if (kbd_state.rshift_press || kbd_state.lshift_press) {
                        //shift功能键已被按下，获取该键的对应值
                        key = map_table[key].func;
                    } else {
                        key = map_table[key].normal;
                    }


                    //判断是否是进程退出指令
                    if (kbd_state.lctrl_press) {
                        if (key == 'c' || key == 'z') {
                            task_t *curr = task_current();
                            if (curr->state == TASK_RUNNING 
                                && kernel_strncmp(curr->name, "empty_task", 10) != 0) {
                                sys_exit(-9);
                            }
                            return;
                        }
                    }

                    if (kbd_state.caps_lock) {  //当前状态下大写锁定已开启
                        if ((key >= 'A' && key <= 'Z')) {
                            key = key - 'A' + 'a';
                        } else if (key >= 'a' && key <= 'z') {
                            key = key -'a' + 'A';
                        }
                    }

        
                tty_in(key);
            }
            break;
    }
}


/**
 * @brief 处理0xE0开头的键值码，即EBGIN_E0状态下接收到的键值码
 * 
 * @param key_code 
 */
static void do_e0_key(uint8_t key_code) {
    char key = get_key(key_code);
    int is_make = is_make_code(key_code);

    switch (key) {
    case KEY_CTRL:
        kbd_state.rctrl_press = is_make ? 1 : 0;
        break;
    case KEY_ALT:
        kbd_state.ralt_press = is_make ? 1 : 0;
        break;
    default:
        break;
    }
}

/**
 * @brief 键盘初始化
 * 
 */
void kbd_init(void) {
    static uint8_t is_inited = 0;
    if (!is_inited)  {
        idt_install(IRQ1_KEYBOARD, (idt_handler_t)exception_handler_kbd);
        idt_enable(IRQ1_KEYBOARD);
        kernel_memset(&kbd_state, 0, sizeof(kbd_state));
        is_inited = 1;
    }
}

/**
 * @brief 键盘中断处理程序
 * 
 */
void do_handler_kbd(exception_frame_t *frame) {
    //原始键值码的状态枚举
    static enum {
        NORMAL, //普通键值码，只有单个字节
        BEGIN_E0, //E0开始的键值码，占2个或4个字节
        BEGIN_E1, //E1开始的键值码，占6个字节
    }recv_state = NORMAL;

    //1.获取键盘的读取状态
    uint32_t status = inb(KBD_PORT_STAT);
    if (!(status & KBD_STAT_RECV_READY)) {  //端口没有准备好的数据，直接结束中断
        pic_send_eoi(IRQ1_KEYBOARD);
        return;
    }

    //2.端口数据已准备好，读取数据端口
    uint8_t key_code = inb(KBD_PORT_DATA);

    //3.因为ICW4的EOI位为0，所以要手动发送EOI即中断结束信号
    pic_send_eoi(IRQ1_KEYBOARD);

    //4.根据接受的键值原码的第一个字节，判断当前接收的原码的状态
    if (key_code == KEY_E0) {   //第一个字节为0xE0,代表还有后续字节
        recv_state = BEGIN_E0;
    } else if (key_code == KEY_E1) {    //第一个字节为0xE1，代表还有后续字节
        recv_state = BEGIN_E1;
    } else {
        switch (recv_state) {
            case NORMAL: //读取的单字节原码
                do_normal_key(key_code);
                break;
            case BEGIN_E0://读取的2个或4个字节原码
                do_e0_key(key_code);
                recv_state = NORMAL;
                break;
            case BEGIN_E1://TODO: 读取的6字节原码, pause 键，暂不处理
                recv_state = NORMAL;
                break;
            default :   
                break;
        }
    }

}