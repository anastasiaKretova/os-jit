#include <iostream>
#include <cstring>
#include <sys/mman.h>

/* useful function:
bool equal(int x) {
    const int value = 2;
    return x == value;
};
 */

unsigned char code[] = {  0x55,
                          0x48, 0x89, 0xe5,
                          0x89, 0x7d, 0xec,
                          0xc7, 0x45, 0xfc,
             /*value*/    0x02, 0x00, 0x00, 0x00,
                          0x8b, 0x45, 0xfc,
                          0x39, 0x45, 0xec,
                          0x0f, 0x94, 0xc0,
                          0x5d,
                          0xc3};

const size_t size_of_code = sizeof(code);
const size_t value = 10;

void* ptr = nullptr;

void help() {
    std::cout << "Function has one argument v.\n";
    std::cout << "It checks if x is equal to v(=2 by default).\n";
    std::cout << "If you want to execute function with argument x, enter \"execute x\".\n";
    std::cout << "If you want to modify v with value new, enter \"modify new\".\n";
    std::cout << "Enter \"exit\" to exit\n";
}

void modify(int new_value) {
    if (mprotect(ptr, size_of_code, PROT_READ | PROT_WRITE) == -1) {
        std::cerr << "Can't change priviliges: " << strerror(errno) << "\n";
        return;
    }
    std::cout << (int)((unsigned char *)ptr)[value] << '\n';
    for (int i = 0; i < 4; ++i) {
        ((unsigned char *) ptr)[value + i] = static_cast<unsigned char>(new_value);
        new_value >>= 8;
    }
    std::cout << (int)((unsigned char *)ptr)[value] << '\n';
}

void execute(int x) {
    if (mprotect(ptr, size_of_code, PROT_READ | PROT_EXEC) == -1) {
        std::cerr << "Can't change priviliges: " << strerror(errno) << "\n";
        return;
    }
    std::cout << ((bool(*)(int))ptr)(x) << "\n";
}

int main() {
    ptr = mmap(nullptr, size_of_code, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    std::memcpy(ptr, code, size_of_code);

    std::string cmd;
    while(true) {
        std::cin >> cmd;
        if (cmd == "exit")
            break;
        if (cmd == "help") {
            help();
        } else if (cmd == "modify") {
            int new_value;
            std::cin >> new_value;
            modify(new_value);
        } else if (cmd == "execute") {
            int x;
            std::cin >> x;
            execute(x);
        } else {
            std::cout << "Illegal command name. Enter \"help\" for help. \n";
        }
    }

    return 0;
}