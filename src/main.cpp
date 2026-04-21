#include <iostream>
#include <sdbus-c++/sdbus-c++.h>
#include <sharing_service.hpp>

int main(int, char **) {

  // here I initialise the service with the name of connection
  sdbus::ServiceName serviceName{"com.system.sharing"};
  // and create a connection itself
  std::unique_ptr<sdbus::IConnection> connection =
      sdbus::createSessionBusConnection();
  connection->requestName(serviceName);

  std::cout << "Connected to session bus\n";

  sdbus::ObjectPath objectPath{"/"};

  // pointer to the object
  auto sharingSystem = sdbus::createObject(*connection, std::move(objectPath));

  sdbus::InterfaceName sharingInterface{"com.system.Sharing"};

  sharingSystem
      ->addVTable(
          sdbus::MethodVTableItem{sdbus::MethodName{"registerService"},
                                  sdbus::Signature{"sas"},
                                  {},
                                  {},
                                  {},
                                  &service::registerService,
                                  {}},
          sdbus::MethodVTableItem{sdbus::MethodName{"openFile"},
                                  sdbus::Signature{"s"},
                                  {},
                                  {},
                                  {},
                                  &service::openFile,
                                  {}},
          sdbus::MethodVTableItem{sdbus::MethodName{"openFileUsingService"},
                                  sdbus::Signature{"ss"},
                                  {},
                                  {},
                                  {},
                                  &service::openFileUsingService,
                                  {}})
      .forInterface(sharingInterface);

  connection->enterEventLoop();
}