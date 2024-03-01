
#include "Args.hpp"
#include "Globals.hpp"

int main(int ac, char** av) {
    (void)ac;

    if (Args::parse(av) < 0) {
        return 1;
    }

    // Server logger settings
    Log.setLogDir(Args::logdir);
    Log.setLevel(Args::loglvl);
    Log.fileLogs(true);
    // Log.stdoutLogs(false);

    // Query logger settings
    queryLog.setLogDir(Args::logdir);
    queryLog.setFilePrefix("query-");
    queryLog.fileLogs(true);
    queryLog.stdoutLogs(false);

    Globals::server = new Server();
    if (Globals::server == nullptr) {
        Log.crit() << "Couldn't allocate server" << Log.endl;
    } else {
        Globals::server->start();
        delete Globals::server;
    }

    return 0;
}