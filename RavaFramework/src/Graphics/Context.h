#pragma once

namespace Rava {
class Context {
public:
  static Unique<Context> Instance;

public:
  static bool Create();

protected:
  bool _initialized = false;
};
}  // namespace Rava