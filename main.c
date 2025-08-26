#include "App_runner.h"
#include "ota_version.h"

/**
 * @brief
 * @param 	 argc 	参数个数
 * @param 	 argv 	参数列表
 * @return 	 int
 */
int main( int argc , char const* argv[] ) {

    if (argc == 1) {
        log_info( "请传入 app|ota|daemon, 来启动不同的程序" );
    } else if (strcmp( argv[1] , "app" ) == 0) {

        App_runner_run();

    } else if (strcmp( argv[1] , "ota" ) == 0) {

        log_info( "otaota" );
        ota_version_check();

    } else if (strcmp( argv[1] , "daemon" ) == 0) {

        log_info( "daemondaemon" );

    }

    return 0;
}
