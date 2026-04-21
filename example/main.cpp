#include <dbussharing.hpp>
#include <filesystem>
#include <string>
#include <vector>

int main() {
  std::string name = "com.example.reader";
  std::vector<std::string> ext = {"epub", "pdf", "mobi", "fb2"};

  const auto onOpenFile = [](const std::string &path, const Request &req) {
    if (!std::filesystem::exists(path)) {
      req.sendErrorResponse("File doesn't exist");
    } else {
      req.sendSuccessResponse();
    }
  };

  SharingService service(name, ext, onOpenFile);

  service.start();

  return 0;
}
