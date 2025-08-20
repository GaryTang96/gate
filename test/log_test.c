#include "log.h"


int main (void) {

    log_set_level( LOG_INFO );
    log_error("error");
    log_warn("warn");
    log_info("info");
    log_debug("debug");


    return 0;
}
