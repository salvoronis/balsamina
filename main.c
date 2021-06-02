#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>
#include <core.h>
#include <ui.h>

void disassembly_args(int, char*[]);

int main(int argc, char *argv[]) {
    disassembly_args(argc, argv);
}

void disassembly_args(int argc, char * argv[]) {
    const char * shortFlags = "s:c:";
    const struct option longFlags[] = {
            {"server", required_argument, NULL, 's'},
            {"client", required_argument, NULL, 'c'},
    };
    int32_t rez;
    int32_t longId;
    int client_fd;
    while ((rez = getopt_long(argc, argv, shortFlags, longFlags, &longId)) != -1) {
        switch (rez) {
            case 's':
                server(atoi(optarg));
                break;
            case 'c':
                client(atoi(optarg));
                initialize_ui();
                break;
            default:
                break;
        }
    }
}