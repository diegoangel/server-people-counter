#include <MemcachedDriver.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <exception>

using namespace Movilgate; 

/*INITIALIZE_EASYLOGGINGPP*/

// g++ -std=gnu++11 -Wall -Iinclude/ -lmemcached src/MemcachedDriver.cpp -o build/Memcached.o

/**
 * \brief Constructor 
 */
Memcached::Memcached(const std::string &hostname, const uint32_t port)
{
    client = new memcache::Memcache("localhost:11211");

    if (client)
    {
        std::cout << "jojojo" << std::endl;
    }
}
/**
 * \brief Destructor 
 */
Memcached::~Memcached()
{
    std::cout << "destructor" << std::endl;
}

bool Memcached::Set(const std::string &key, const std::string &value, time_t expiration, uint32_t flags)
{
    std::vector<char> val(value.begin(), value.end());


        if(client->set(key, val, expiration, flags)) {
            std::cout << "key stored successfully" << std::endl;
        } else {
            std::cerr << "couldn't store key" << std::endl;
        } 

    return true;   
}

bool Memcached::Delete(const std::string &key) 
{
    return true;
}

const std::string Memcached::Get(const std::string &key, uint32_t flags)
{
    std::vector<char> retval;
    return std::string("hoa");
}

int main(int argc, char const *argv[])
{
    using namespace Movilgate;

    Memcached m("localhost", 11211);

    if (m.client)
    {
        const std::string key = "A";
        const std::string value = "Tu vieja";
        m.Set(key, value, 0, 0);
        std::cout << "conectado!" << std::endl;
        std::cout << m.Get("A", 0) << std::endl;
    }
    m.~Memcached();
    return 0;
}
    