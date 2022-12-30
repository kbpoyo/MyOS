#include <iostream>
#include <thread>



int main() {

    std::thread thread_1([](){
        int i = 0;
        while (i++ < 1000) {
            std::cout << "进程PID = " << std::this_thread::get_id() <<std::endl;
            sleep(1);
        }
    });

    thread_1.detach();

    std::thread thread_2([](){
        int i = 0;
        while (i++ < 1000) {
            std::cout << "PID = " << std::this_thread::get_id() <<std::endl;
            sleep(1);
        }
    });

    thread_2.join();
    
    return 0;
}
