#include "application/application.h"

int main(int _argc, char* _argv[]) {
    mkr::application::instance().run();
    return 0;
}