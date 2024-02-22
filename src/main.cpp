
#include "Args.hpp"
#include "Time.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Globals.hpp"

int main(int ac, char **av) {
    (void)ac;

    Args::parse(av);

    Log.setLogDir(Args::logdir);
    Log.fileLogs(true);

    queryLog.setLogDir(Args::logdir);
    queryLog.setPrefix("query-");
    queryLog.fileLogs(true);

    Globals::server.start();

    return 0;
}