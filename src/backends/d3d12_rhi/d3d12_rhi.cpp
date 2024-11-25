#include "d3d12_rhi.hpp"
#include "d3d12_surface.hpp"

namespace avio {

static bool d3d12_rhi_init(RHI* rhi, const infos::RHIInfo& info);
static void d3d12_rhi_shutdown(RHI* rhi);

RhiD3D12 g_rhi_d3d12{
    .base =
        {
            .init_impl = d3d12_rhi_init,
            .shutdown_impl = d3d12_rhi_shutdown,
        },
};

static RHI* get_rhi_d3d12() {
  return &g_rhi_d3d12.base;
}

bool d3d12_rhi_init(RHI* rhi, const infos::RHIInfo& info) {
  // Create dxgi factory
#ifndef NDEBUG
  UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#else
  UINT flags = 0;
#endif

  RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);
  HR_ASSERT(CreateDXGIFactory2(flags, IID_PPV_ARGS(&d3d12->dxgi_factory)));

  // Pick adapter
  HR_ASSERT(d3d12->dxgi_factory->EnumAdapterByGpuPreference(
      0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&d3d12->adapter)));
  AV_LOG(info, "DXGI factory created.");

  // Log adapter
  DXGI_ADAPTER_DESC adapter_desc;
  zero_mem(adapter_desc);

  HR_ASSERT(d3d12->adapter->GetDesc(&adapter_desc));

  char adapter_name[std::size(adapter_desc.Description)]{};
  wcstombs(adapter_name, adapter_desc.Description,
           std::size(adapter_desc.Description));
  AV_LOG(info, "DXGI adapter selected: {}", adapter_name);

// Create device
#ifdef AVIO_ENABLE_GPU_VALIDATION
  ID3D12Debug* debug{};
  HR_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
  debug->EnableDebugLayer();
  debug->Release();
#endif

  HR_ASSERT(D3D12CreateDevice(d3d12->adapter, D3D_FEATURE_LEVEL_12_0,
                              IID_PPV_ARGS(&d3d12->device)));

  // Create graphics queue
  D3D12_COMMAND_QUEUE_DESC graphics_queue_desc{};
  graphics_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  zero_mem(graphics_queue_desc);
  HR_ASSERT(d3d12->device->CreateCommandQueue(
      &graphics_queue_desc, IID_PPV_ARGS(&d3d12->graphics_queue)));
  AV_LOG(info, "D3D12 Graphics queue created");

  return true;
}

void d3d12_rhi_shutdown(RHI* rhi) {
  RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);

  // Release the graphics queue
  d3d12->graphics_queue->Release();

  // Release the device.
  // TODO: Need to clear the queue.
  d3d12->device->Release();

  // Release the adapter
  d3d12->adapter->Release();

  // Release factory (must go last)
  d3d12->dxgi_factory->Release();

  AV_LOG(info, "D3D12 Rhi terminated.");
}

}  // namespace avio

void avio::d3d12_rhi::init_global_rhi_pointers() {
  get_rhi = get_rhi_d3d12;

  // Surface pointers
  rhi_create_surface = d3d12_create_surface;
  rhi_destroy_surface = d3d12_destroy_surface;
}
