
#include "chatI.h"

void
Chat::LobbyI::_cpp_register(const UserPrx& user,
                            const ::std::string& password,
                            const Ice::Current& current)
{
    if (std::find(users.begin(), users.end(), user) != users.end()) {
        throw UserExists();
    }
    this->users.push_back(user);
    this->passwords.push_back(password);
    user->changeStatus(Chat::UserStatus::Online);
    std::cout << "User \'" << user->getName() << "\' has been registered." << std::endl;
}

::std::string
Chat::LobbyI::login(const UserPrx& user,
                    const ::std::string& password,
                    const Ice::Current& current)
{
    auto it = std::find(users.begin(), users.end(), user);
    int index = std::distance(users.begin(), it);
    if (it == users.end() || this->passwords[index] != password) {
        throw AccessDenied();
    }
    user->changeStatus(Chat::UserStatus::Online);
    std::cout << "User \'" << user->getName() << "\' has been logged in." << std::endl;

    return user->getName();
}

void
Chat::LobbyI::logout(const UserPrx& user,
                     const Ice::Current& current)
{
    if(user->getStatus() == Chat::UserStatus::Online)
        user->changeStatus(Chat::UserStatus::Offline);
    else
        throw AccessDenied();

    std::cout << "User \'" << user->getName() << "\' has been logged out." << std::endl;
}

::Chat::Rooms
Chat::LobbyI::getRooms(const Ice::Current& current)
{
    return rooms;
}

::Chat::Users
Chat::LobbyI::getUsers(const Ice::Current& current)
{
    return users;
}

::Chat::RoomPrx
Chat::LobbyI::createRoom(const ::std::string& name,
                         const Ice::Current& current)
{
    for(const auto& room : rooms)
    {
        if(name == room->getName())
            throw RoomExists();
    }

    if(roomFactories.size() == 0)
        throw AccessDenied();
    
    RoomFactoryPrx roomFactoryPrx = roomFactories[0];
    RoomPrx roomPrx = roomFactoryPrx->createRoom(name);
    rooms.push_back(roomPrx);
    return roomPrx;
}

::Chat::RoomPrx
Chat::LobbyI::findRoom(const ::std::string& name,
                       const Ice::Current& current)
{
    for (const auto& room : rooms)
    {
        if (room->getName() == name)
        {
            return room;
        }
    }
    return nullptr;
}

void
Chat::LobbyI::sendPrivateMessage(const UserPrx& fromUser,
                                 const ::std::string& toUser,
                                 const ::std::string& msg,
                                 const Ice::Current& current)
{
    for (const auto& user : users) {
        if (user->getName() == toUser && user->getStatus() == Chat::UserStatus::Online) {
            user->receivePrivateMessage(fromUser, msg);
        }
    }
}

void
Chat::LobbyI::registerRoomFactory(const RoomFactoryPrx& roomFactory,
                                  const Ice::Current& current)
{
    for (const auto& roomf : roomFactories)
    {
        if (roomf == roomFactory)
            throw RoomFactoryExists();
    }
    roomFactories.push_back(roomFactory);
}

void
Chat::LobbyI::unregisterRoomFactory(const RoomFactoryPrx& roomFactory,
                                    const Ice::Current& current)
{
    for (const auto& roomf : roomFactories)
    {
        if (roomf == roomFactory) {
            roomFactories.erase(std::remove(roomFactories.begin(), roomFactories.end(), roomf), roomFactories.end());
            return;
        }
    }
    throw NoSuchRoomFactory();
}

::std::string
Chat::UserI::getName(const Ice::Current& current)
{
    return this->name;
}

::Chat::UserStatus
Chat::UserI::getStatus(const Ice::Current& current)
{
    return this->userStatus;
}

void
Chat::UserI::changeStatus(UserStatus userStatus,
                          const Ice::Current& current)
{
    this->userStatus=userStatus;
}

void
Chat::UserI::receivePrivateMessage(const UserPrx& fromUser,
                                   const ::std::string& message,
                                   const Ice::Current& current)
{
    std::cout << "PRIVATE: " << fromUser->getName() << ": " << message << ::std::endl;
}

void
Chat::UserI::receiveMessage(const RoomPrx& fromRoom,
                            const UserPrx& fromUser,
                            const ::std::string& message,
                            const Ice::Current& current)
{
    std::cout << fromUser->getName() << ": " << message << ::std::endl;
}

::std::string
Chat::RoomI::getName(const Ice::Current& current)
{
    return this->name;
}

::Chat::Users
Chat::RoomI::getUsers(const Ice::Current& current)
{
    return this->users;
}

void
Chat::RoomI::join(const UserPrx& user,
                  const Ice::Current& current)
{
    users.push_back(user);
}

void
Chat::RoomI::leave(const UserPrx& user,
                   const Ice::Current& current)
{
    users.erase(std::find(users.begin(), users.end(), user));
}

void
Chat::RoomI::sendMessage(const UserPrx& fromUser,
                         const ::std::string& message,
                         const Ice::Current& current)
{
    std::cout << "FUNCTION SEND MESSAGE" << std::endl;
    for (auto user : users) {
        std::cout << "USER " << user->getName() << std::endl;
        if (user == fromUser || user->getStatus() != Chat::UserStatus::Online) {
            continue;
        }
        user->receiveMessage(nullptr, fromUser, message);
    }
}

double
Chat::RoomFactoryI::getServerLoad(const Ice::Current& current)
{
    return 0.0;
}

::Chat::RoomPrx
Chat::RoomFactoryI::createRoom(const ::std::string& name,
                               const Ice::Current& current)
{
    RoomPtr roomPtr = new RoomI(name);
    RoomPrx roomPrx = RoomPrx::uncheckedCast(current.adapter->add(roomPtr, Ice::stringToIdentity (name)));
    return roomPrx;
}
