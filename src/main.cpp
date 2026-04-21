#include <filesystem>
#include <fstream>
#include <logic.h>
#include <nlohmann/json.hpp>
#include <random>
#include <sdbus-c++/sdbus-c++.h>
#include <service.h>
#include <string>
#include <vector>

int main(int, char **) {

  // here I initialise the service with the name of connection
  sdbus::ServiceName serviceName{"com.system.sharing"};
  // and create a connection itself
  auto connection = sdbus::createSessionBusConnection(serviceName);

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
                                  &registerService,
                                  {}},
          sdbus::MethodVTableItem{sdbus::MethodName{"openFile"},
                                  sdbus::Signature{"s"},
                                  {},
                                  {},
                                  {},
                                  &openFile,
                                  {}},
          sdbus::MethodVTableItem{sdbus::MethodName{"openFileUsingService"},
                                  sdbus::Signature{"ss"},
                                  {},
                                  {},
                                  {},
                                  &openFileUsingService,
                                  {}})
      .forInterface(sharingInterface);

  connection->enterEventLoop();
}