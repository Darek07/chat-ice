#include <Ice/Ice.h>
#include <chat.h>
#include "src/chatI.h"
#include <limits>

using namespace Chat;
using namespace std;

string menu =
    "1 - login\n"
    "2 - log out\n"
    "3 - display status\n"
    "4 - display rooms\n"
    "5 - display all users\n"
    "6 - create room\n"
    "7 - join room\n"
    "8 - leave room\n"
    "9 - display room users\n"
    "10 - send message in room\n"
    "11 - send private message\n"
    "12 - print menu\n"
    "66 - exit";

int main(int argc, char *argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try
    {
        ic = Ice::initialize(argc, argv);
        // Ice::ObjectPrx base = ic->stringToProxy("Lobby:default -p 6661");
        Ice::ObjectPrx base = ic->stringToProxy("Lobby:default -h 10.0.2.15 -p 6660");
        const auto lobbyPrx = LobbyPrx::checkedCast(base);
        if (!lobbyPrx)
        {
            throw "Invalid server proxy";
        }
        const auto adapter = ic->createObjectAdapterWithEndpoints("CLIENT", "default");

        string name = "";
        cout << "Enter name: ";
        cin >> name;

        Ice::ObjectPtr objectUser = new UserI(name);
        Ice::ObjectPrx userPrxBase = adapter->addWithUUID(objectUser);
        adapter->activate();

        const auto userPrx = UserPrx::checkedCast(userPrxBase);

        string password = "";
        cout << "Enter password: ";
        cin >> password;

        lobbyPrx->_cpp_register(userPrx, password);

        RoomPrx roomPrx = nullptr;
        cout << "Menu:\n" << menu << endl;
        while (1)
        {
            if (roomPrx)
            {
                cout << "-----------------------------------" << endl;
                cout << "You have entered room: " + roomPrx->getName() << endl;
                cout << "-----------------------------------" << endl;
            }

            int option = 0;
            cin >> option;
            switch (option)
            {
            case 1:
            {
                try
                {
                    password = "";
                    cout << "Enter password: ";
                    cin >> password;
                    lobbyPrx->login(userPrx, password);
                }
                catch (AccessDenied)
                {
                    cout << "Cannot log in. Access Denied" << endl;
                }
                break;
            }
            case 2:
            {
                try
                {
                    lobbyPrx->logout(userPrx);
                }
                catch (AccessDenied)
                {
                    cout << "Cannot log out. Access Denied" << endl;
                }
                break;
            }
            case 3:
            {
                cout << "Status: ";
                if (userPrx->getStatus() == Chat::Online)
                    cout << "ONLINE";
                else if (userPrx->getStatus() == Chat::Offline)
                    cout << "OFFLINE";
                else
                    cout << "BE BACK";
                cout << endl;
                break;
            }
            case 4:
            {
                const auto rooms = lobbyPrx->getRooms();
                cout << "Rooms: " << endl;
                for (const auto &room : rooms)
                {
                    cout << "* " << room->getName() << endl << "\tUsers: " << endl;
                    const auto users = room->getUsers();
                    for (const auto &user : users)
                    {
                        if (user->getStatus() == Chat::Online)
                        {
                            cout << "\t- " << user->getName() << endl;
                        }
                    }
                }
                break;
            }
            case 5:
            {
                const auto users = lobbyPrx->getUsers();
                cout << "Online users:" << endl;
                for (const auto &user : users)
                {
                    if (user->getStatus() == Chat::Online)
                        cout << user->getName() << endl;
                }
                break;
            }
            case 6:
            {
                try
                {
                    string roomName = "";
                    cout << "Enter room name: ";
                    cin >> roomName;
                    const auto roomPrx1 = lobbyPrx->createRoom(roomName);
                    cout << "Room created" << endl;;
                }
                catch (AccessDenied)
                {
                    cout << "Cannot create room. Access Denied" << endl;
                }
                catch (RoomExists)
                {
                    cout << "Room already exists" << endl;
                }
                break;
            }
            case 7:
            {
                if (!roomPrx)
                {
                    try
                    {
                        string roomName = "";
                        cout << "Enter room name: ";
                        cin >> roomName;
                        roomPrx = lobbyPrx->findRoom(roomName);
                        cout << "Joined room" << endl;
                        roomPrx->join(userPrx);
                    }
                    catch (NoSuchRoom)
                    {
                        cout << "No such room" << endl;
                    }
                }
                else
                    cout << "You are already in the room" << endl;
                break;
            }
            case 8:
            {
                if (roomPrx)
                {
                    roomPrx->leave(userPrx);
                    cout << "Left room" << endl;
                    roomPrx = nullptr;
                }
                else
                    cout << "You are not in any room" << endl;
                break;
            }
            case 9:
            {
                if (roomPrx)
                {
                    const auto users = roomPrx->getUsers();
                    cout << "Users in room:" << endl;
                    for (const auto &user : users)
                    {
                        cout << user->getName() << endl;
                    }
                }
                else
                    cout << "You are not in any room" << endl;
                break;
            }
            case 10:
            {
                if (roomPrx)
                {
                    string input;
                    cout << "Message:";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(std::cin, input);
                    roomPrx->sendMessage(userPrx, input);
                }
                else
                    cout << "You are not in any room" << endl;
                break;
            }
            case 11:
            {
                string name = "";
                cout << "Enter user name: ";
                cin >> name;
                string input;
                cout << "Message:";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(std::cin, input);
                lobbyPrx->sendPrivateMessage(userPrx, name, input);
                break;
            }
            case 66:
            {
                userPrx->changeStatus(Chat::Offline);
                if (roomPrx)
                {
                    roomPrx->leave(userPrx);
                    roomPrx = nullptr;
                }
                break;
            }
            default:
            {
                cout << "Wrong option. Menu:\n" << menu << endl;
            }
            }
            if (option == 66)
                break;
        }
    }
    catch (const Ice::Exception &ex)
    {
        cerr << ex << endl;
        status = 1;
    }
    catch (const char *msg)
    {
        cerr << msg << endl;
        status = 1;
    }
    if (ic)
        ic->destroy();
    return status;
}
