#include <iostream>
#include "FastEngine/network/C_socket.hpp"
#include "FastEngine/extra/extra_function.hpp"

int main()
{
    if (!fge::net::Socket::initSocket())
    {
        std::cout << "Can't init socket" << std::endl;
        return 1;
    }

    nlohmann::json config;
    if (!fge::LoadJsonFromFile("./config.json", config))
    {
        std::cout << "Can't load config" << std::endl;
        return 1;
    }

    auto ipv4_port = config["ipv4_port"].get<fge::net::Port>();
    auto ipv6_port = config["ipv6_port"].get<fge::net::Port>();
    fge::net::IpAddress ipv6_ip = config["ipv6_ip"].get<std::string>();

    //Listen for incoming connections on port 60000
    fge::net::SocketListenerTcp listener;
    if (listener.listen(ipv4_port, fge::net::IpAddress::Ipv4Any) != fge::net::Socket::ERR_DONE)
    {
        std::cout << "Can't listen on port " << ipv4_port << std::endl;
        return 1;
    }

    fge::net::SocketTcp tunnel;
    if (tunnel.connect(ipv6_ip, ipv6_port, 2000) != fge::net::Socket::ERR_DONE)
    {
        std::cout << "Can't connect to tunnel" << std::endl;
        return 1;
    }

    fge::net::SocketTcp client;

    bool running = true;
    while (running)
    {
        if (!client.isValid())
        {
            if (listener.select(true, 200) == fge::net::Socket::ERR_DONE)
            {
                if (listener.accept(client) != fge::net::Socket::ERR_DONE)
                {
                    std::cout << "Can't accept client" << std::endl;
                    return 1;
                }
                std::cout << "Client connected" << std::endl;
                std::cout << "Remote address: " << client.getRemoteAddress().toString().value_or("ERR") << std::endl;
                std::cout << "Remote port: " << client.getRemotePort() << std::endl;
            }
        }
        else
        {
            static std::vector<uint8_t> data(1024, 0);
            std::size_t received = 0;
            if (client.receive(data.data(), data.size(), received, 200) == fge::net::Socket::ERR_DONE)
            {
                if (received == 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    client.close();
                }
                else
                {
                    std::cout << "Received: " << received << " bytes" << std::endl;
                    if (tunnel.send(data.data(), received) != fge::net::Socket::ERR_DONE)
                    {
                        std::cout << "Can't send to tunnel" << std::endl;
                    }
                }
            }
        }
    }

    return 0;
}