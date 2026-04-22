#include <dbussharing.hpp>
#include <functional>
#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>

SharingService::SharingService(
    std::string service, std::vector<std::string> ext,
    std::function<void(const std::string &, const Request &)> handler)
    : name{service}, extensions{ext}, onOpenFile{handler} {}

void SharingService::start() {
  sdbus::ServiceName service{name};
  connection = sdbus::createSessionBusConnection();
  connection->requestName(service);

  sdbus::ObjectPath objectPath{"/"};
  processingService = sdbus::createObject(*connection, objectPath);

  processingService
      ->addVTable(sdbus::MethodVTableItem{sdbus::MethodName{"openFile"},
                                          sdbus::Signature{"s"},
                                          {},
                                          {},
                                          {},
                                          [this](sdbus::MethodCall call) {
                                            std::string path;
                                            call >> path;

                                            Request req(std::move(call));
                                            onOpenFile(path, req);
                                          },
                                          {}})
      .forInterface(name);

  auto sharingProxy = sdbus::createProxy(
      *connection, sdbus::ServiceName{"com.system.sharing"}, objectPath);
  sdbus::InterfaceName interface{"com.system.Sharing"};
  auto registerMethod = sharingProxy->createMethodCall(
      interface, sdbus::MemberName("registerService"));

  registerMethod << name;
  registerMethod << extensions;
  sharingProxy->callMethod(registerMethod);

  connection->enterEventLoop();
};

Request::Request(sdbus::MethodCall methodCall) : call(std::move(methodCall)) {}

void Request::sendErrorResponse(const std::string &response) const {
  throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"}, response);
}

void Request::sendSuccessResponse() const {
  auto reply = call.createReply();
  reply.send();
}