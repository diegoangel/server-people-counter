#ifndef MEMCACHEDDRIVER_H
#define MEMCACHEDDRIVER_H

#include <libmemcached/memcached.h>
#include <iosfwd>
#include <time.h>
#include <stdlib.h>
/*#include <easylogging++.h>*/

namespace Movilgate
{
/**
 * \class MemcachedDriver
 * \brief Class to handle connections to memcached, store, get and delete data.
 * \copyright Movilgate
 */
class Memcached
{
    public:     
        /*!
         * \brief   Constructor
         *          Connect to a server 
         * \param   address
         * \param   port         
         */        
        Memcached(char* &address, uint32_t port);
        /*!
         * \brief   Destructor 
         */
        virtual ~Memcached(void);
        /*!
         * \brief Store a key and value and set the expiration time
         * \param key
         * \param data
         * \param expiration
         */
        bool Set(const std::string &key, const std::string &value, time_t expiration, uint32_t flags);
        /*! 
         * \brief
         * \param key
         */        
        bool Delete(const std::string &key);
        /*!
         * \brief
         * \param key
         * \return
         */   
        const std::string Get(const std::string &key, uint32_t flags);
    private:
        struct Memcache {
            memcached_st *memc, 
            memcached_return rc   
        };
  
};

}

#endif /* MEMCACHEDDRIVER_H */ 
