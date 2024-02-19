
#include "Args.hpp"
#include "Time.hpp"
#include "Log.hpp"
#include "Server.hpp"

// static void
// sigint_handler(int) {
//     Log.info() << "Server is stopping..." << Log.endl;
//     serv->stop();
// }

int main(int ac, char **av) {
    (void)ac;

    Args::parse(av);

    Log << "the first " << "thing logged" << Log.endl;
    Log.info() << "the first " << "thing logged" << Log.endl;

    Server *serv = new Server();

    return 0;
}