#include <Ice/Ice.h>
#include <chat.h>
#include "src/chatI.h"
#include <memory>
#include <vector>

using namespace std;
using namespace Chat;

int main(int argc, char *argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try
    {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints("SERVER_CHAT",
                                                                             "default -p 6660");
        Ice::ObjectPtr object = new LobbyI;
        adapter->add(object, Ice::stringToIdentity("Lobby"));

        adapter->activate();
        ic->waitForShutdown();
    }
    catch (const Ice::Exception &e)
    {
        cerr << e << endl;
        status = 1;
    }
    catch (const char *msg)
    {
        cerr << msg << endl;
        status = 1;
    }
    if (ic)
    {
        try
        {
            ic->destroy();
        }
        catch (const Ice::Exception &e)
        {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}
