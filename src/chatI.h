#ifndef __chatI_h__
#define __chatI_h__

#include <Ice/Ice.h>
#include <chat.h>
#include <memory>
#include <map>

namespace Chat
{

class LobbyI : public virtual Lobby
{
private:
    Rooms rooms;
    Users users;
    std::vector<std::string> passwords;
    std::vector<RoomFactoryPrx> roomFactories;
public:
    virtual void _cpp_register(const UserPrx&,
                               const ::std::string&,
                               const Ice::Current&) override;

    virtual ::std::string login(const UserPrx&,
                                const ::std::string&,
                                const Ice::Current&) override;

    virtual void logout(const UserPrx&, const Ice::Current&) override;

    virtual Rooms getRooms(const Ice::Current&) override;

    virtual Users getUsers(const Ice::Current&) override;

    virtual RoomPrx createRoom(const ::std::string&,
                                                  const Ice::Current&) override;

    virtual RoomPrx findRoom(const ::std::string&,
                                                const Ice::Current&) override;

    virtual void sendPrivateMessage(const UserPrx&,
                                    const ::std::string&,
                                    const ::std::string&,
                                    const Ice::Current&) override;

    virtual void registerRoomFactory(const RoomFactoryPrx&,
                                     const Ice::Current&) override;

    virtual void unregisterRoomFactory(const RoomFactoryPrx&,
                                       const Ice::Current&) override;
};

class UserI : public virtual User
{
private:
    std::string name;
    std::string password;
    UserStatus userStatus = UserStatus::Offline;
public:
    UserI(std::string name) : name(name) {}

    virtual ::std::string getName(const Ice::Current&) override;

    virtual UserStatus getStatus(const Ice::Current&) override;

    virtual void changeStatus(UserStatus,
                              const Ice::Current&) override;

    virtual void receivePrivateMessage(const UserPrx&,
                                       const ::std::string&,
                                       const Ice::Current&) override;

    virtual void receiveMessage(const RoomPrx&,
                                const UserPrx&,
                                const ::std::string&,
                                const Ice::Current&) override;
};

class RoomI : public virtual Room
{
private:
    ::std::string name;
    Users users;
public:
    RoomI(std::string name) : name(name) {}

    virtual ::std::string getName(const Ice::Current&) override;

    virtual Users getUsers(const Ice::Current&) override;

    virtual void join(const UserPrx&,
                      const Ice::Current&) override;

    virtual void leave(const UserPrx&,
                       const Ice::Current&) override;

    virtual void sendMessage(const UserPrx&,
                             const ::std::string&,
                             const Ice::Current&) override;
};

class RoomFactoryI : public virtual RoomFactory
{
public:

    virtual double getServerLoad(const Ice::Current&) override;

    virtual RoomPrx createRoom(const ::std::string&,
                                                  const Ice::Current&) override;
};

}

#endif
