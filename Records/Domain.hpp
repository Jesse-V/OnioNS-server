
#ifndef DOMAIN
#define DOMAIN

#include "Record.hpp"

class Domain
{
    public:
        const int DIFFICULTY = 3; //1/2^x chance of success, so order of magnitude
        //currently takes 256 MB. 1.3 secs on Core 2 Q9000, 0.7 secs on i7-2600k

        //difficulty at 17:
            //4 days on Q9000 @ 1 CPU, 256MB RAM
            //1 day on Q9000 @ 4 CPUs, 1GB RAM
            //1 day on i7-2600k @ 1 CPU, 256MB RAM
            //6 hours on i7-2600k @ 8 CPUs, 2GB RAM

        const uint8_t IN_SIZE = 8;

        void mineRegistration(RSA*);
        void recursiveMining(uint8_t*, uint8_t, RSA* key, uint8_t*, uint8_t*);
};

#endif
