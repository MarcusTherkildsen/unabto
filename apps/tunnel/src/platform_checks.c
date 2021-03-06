#include "platform_checks.h"
#include <unabto/unabto_env_base.h>
#include <unabto/unabto_logging.h>

bool test_if_lo_exists();

bool platform_checks() {
    bool result = true;
#ifndef WIN32
    result &= test_if_lo_exists();
#endif
    return result;
}

#ifndef WIN32

#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>

bool test_if_lo_exists() {
    bool foundLoopback = false;

    struct ifaddrs *addrs,*tmp;

    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp)
    {
        const char* lo = "lo";
        if (tmp->ifa_addr) {
            if (strncmp(tmp->ifa_name, lo, strlen(lo)) == 0) {
                if (! (tmp->ifa_flags & (IFF_UP))) {
                    NABTO_LOG_FATAL(("Loopback interface exists but it's not up"));
                } else if (! (tmp->ifa_flags & (IFF_UP))) {
                    NABTO_LOG_FATAL(("Loopback interface exists but it's not running"));
                } else {
                    foundLoopback = true;
                }
            }
        }

        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs);

    if (!foundLoopback) {
        NABTO_LOG_FATAL(("No loopback interface found, searched for an interface named lo. This is required if the tunnel should be able to connect to services on localhost."));
    }
    return foundLoopback;
}
#endif
