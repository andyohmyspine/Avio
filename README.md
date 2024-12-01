# Avio Rendering Engine

An easy to use yet (not yet) powerful rendering engine.
Basically created as a sandbox for learning d3d12 and vulkan. 

Supports both D3D12 and Vulkan.
Currently only windows.

See examples/sandbox for a WIP example.

## Example usage
To initialize the engine use ```avio::init_engine```

```c++
avio::Engine engine;
avio::init_engine(engine, {});  // This will select the default rendering api for the platform
```

You can also specify which render api to use 
```c++
avio::init_engine(engine, {.render_api = avio::RenderAPI::d3d12});
```

Don't forget to destroy it once you're done with it:
```c++
avio::shutdown_engine(engine);
```

See ```examples/sandbox/main.cpp``` for example.