
#include "Args.hpp"
#include "Time.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "Globals.hpp"

int main(int ac, char **av) {
    (void)ac;

    Args::parse(av);

    // Log << "the first " << "thing logged" << Log.endl;
    // Log.info() << "the first " << "thing logged" << Log.endl;

    Log.setLogDir(Args::logdir);
    Log.fileLogs(true);
    Globals::server.start();

    return 0;
}