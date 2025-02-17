#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "iphlpapi.lib" )

#include <stdio.h>
#include <stdlib.h>


int main()
{
    WSADATA d;

    if( WSAStartup(MAKEWORD(2, 2), &d) )
    {
        fprintf(stderr, "Failed to intialize.\n");
        return -1;
    }

    DWORD asize = 20000;
    PIP_ADAPTER_ADDRESSES adapters;
    do {
        adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);

        if( !adapters )
        {
            fprintf(stderr, "Couldn't allocate %ld bytes for adapters.\n", asize);
            WSACleanup();
            exit(-1);
        }

        int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapters, &asize);
        if( r == ERROR_BUFFER_OVERFLOW )
        {
            printf("Warning: GETAdaptersAddress wants %ld bytes.\n", asize);
            printf("Allocating requried bytes in next iteration ... \n");
            free(adapters);
        }
        else if( r == ERROR_SUCCESS )
        {
            break;
        }
        else
        {
            fprintf(stderr, "Error from GetAdaptersAddresses: %d\n", r);
            free(adapters);
            WSACleanup();
            exit(-1);
        }
    } while( !adapters );

    PIP_ADAPTER_ADDRESSES adapter = adapters;
    while( adapter )
    {
        printf("\nAdapter name: %S\n", adapter->FriendlyName);

        PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
        while( address )
        {
            printf("\t%s",
                  address->Address.lpSockaddr->sa_family == AF_INET ? "IPv4" : "IPv6");

            char ap[100];
            getnameinfo(address->Address.lpSockaddr,
                        address->Address.iSockaddrLength,
                        ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf("\t%s\n", ap);

            address = address->Next;
        }

        adapter = adapter->Next;
    }

    free(adapters);
    WSACleanup();
    return 0;
}
