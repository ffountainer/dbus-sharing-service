#include <iostream>
#include <sdbus-c++/sdbus-c++.h>


int main(int, char**){

    // here I initialise the service with the name of connection
    sdbus::ServiceName serviceName{"com.system.sharing"};
    // and create a connection itself
    auto connection = sdbus::createBusConnection(serviceName);


}
