#include "application/application.h"

using namespace mkr;

int main() {
    application::instance().init();
    application::instance().run();
    application::instance().exit();

    return 0;
}