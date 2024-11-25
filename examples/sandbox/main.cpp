#include "engine.hpp"

int main() {
  avio::Engine engine;

  try {
    AV_ASSERT_MSG(avio::init_engine(engine, {}),
                  "Failed to initialize engine!");
  } catch (const avio::Error& error) {
    AV_LOG(critical, "Exception caught: {}", error.what());
    return 1;
  }

  return 0;
}