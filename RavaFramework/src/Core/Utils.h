#pragma once

namespace Rava {
static std::string GetPathWithoutFileName(const std::filesystem::path& path) {
  std::filesystem::path withoutFilename{std::filesystem::path(path.parent_path())};
  std::string pathWithoutFilename = withoutFilename.string();
  if (!pathWithoutFilename.empty()) {
    if (pathWithoutFilename.back() != '/') {
      pathWithoutFilename += '/';
    }
  }
  return pathWithoutFilename;
}
}