#ifndef SHARING_SERVICE_HPP
#define SHARING_SERVICE_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <sdbus-c++/sdbus-c++.h>

class Request;

class SharingService {
private:
  std::string name{};
  std::vector<std::string> extensions{};
  std::unique_ptr<sdbus::IConnection> connection;
  std::function<void(const std::string &, const Request &)> onOpenFile;
  std::unique_ptr<sdbus::IObject> processingService;

public:
  SharingService(
      std::string service, std::vector<std::string> ext,
      std::function<void(const std::string &, const Request &)> handler);

  void start();
};

class Request {
private:
  sdbus::MethodCall call;

public:
  explicit Request(sdbus::MethodCall methodCall);

  void sendErrorResponse(const std::string &response);
  void sendSuccessResponse();
};

#endif