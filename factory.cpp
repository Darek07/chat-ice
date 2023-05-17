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
        Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints("FACTORY", "default");

        Ice::ObjectPtr roomFactory = new RoomFactoryI();
        Ice::ObjectPrx userPrxBase = adapter->addWithUUID(roomFactory);
        adapter->activate();
        RoomFactoryPrx roomFactoryPrx = RoomFactoryPrx::checkedCast(userPrxBase);

        Ice::ObjectPrx base = ic->stringToProxy("Lobby:default -p 6660");

        LobbyPrx lobbyPrx = LobbyPrx::checkedCast(base);
        if (!lobbyPrx)
        {
            throw "Invalid server proxy";
        }
        lobbyPrx->registerRoomFactory(roomFactoryPrx);
        while (1)
        {
            int i = 0;
            cin >> i;
            if (i == 66)
            {
                lobbyPrx->unregisterRoomFactory(roomFactoryPrx);
                break;
            }
        }
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
