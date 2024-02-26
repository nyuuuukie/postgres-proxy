
#include "Args.hpp"
#include "Globals.hpp"

int main(int ac, char **av) {
    (void)ac;

    if (Args::parse(av) < 0) {
        return 1;
    }

    // Server logger settings
    Log.setLogDir(Args::logdir);
    Log.setLevel(Args::loglvl);
    Log.fileLogs(true);

    // Query logger settings
    queryLog.setLogDir(Args::logdir);
    queryLog.setFilePrefix("query-");
    queryLog.fileLogs(true);
    queryLog.stdoutLogs(false);

    Globals::server.start();

    return 0;
}