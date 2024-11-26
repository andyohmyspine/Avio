#include "d3d12_rhi.hpp"
#include "d3d12_surface.hpp"

namespace avio::dx12 {

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

#ifdef AVIO_ENABLE_GPU_VALIDATION
  static void d3d12_message_callback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity,
                                     D3D12_MESSAGE_ID messsage_id, LPCSTR description, void* context) {
    switch (severity) {
      case D3D12_MESSAGE_SEVERITY_INFO:
        AV_LOG(info, "D3D12 Validation: {}", description);
        break;
      case D3D12_MESSAGE_SEVERITY_MESSAGE:
        AV_LOG(trace, "D3D12 Validation: {}", description);
        break;
      case D3D12_MESSAGE_SEVERITY_WARNING:
        AV_LOG(warn, "D3D12 Validation: {}", description);
        break;
      case D3D12_MESSAGE_SEVERITY_ERROR:
        AV_LOG(error, "D3D12 Validation: {}", description);
        break;
      case D3D12_MESSAGE_SEVERITY_CORRUPTION:
        AV_LOG(critical, "D3D12 Validation: {}", description);
        break;
      default:
        break;
    }
  }
#endif

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
    HR_ASSERT(d3d12->dxgi_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                              IID_PPV_ARGS(&d3d12->adapter)));
    AV_LOG(info, "DXGI factory created.");

    // Log adapter
    DXGI_ADAPTER_DESC adapter_desc;
    zero_mem(adapter_desc);

    HR_ASSERT(d3d12->adapter->GetDesc(&adapter_desc));

    char adapter_name[std::size(adapter_desc.Description)]{};
    wcstombs(adapter_name, adapter_desc.Description, std::size(adapter_desc.Description));
    AV_LOG(info, "DXGI adapter selected: {}", adapter_name);

// Create device
#ifdef AVIO_ENABLE_GPU_VALIDATION
    ID3D12Debug* debug{};
    HR_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
    debug->EnableDebugLayer();
    debug->Release();
#endif

    HR_ASSERT(D3D12CreateDevice(d3d12->adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3d12->device)));

#ifdef AVIO_ENABLE_GPU_VALIDATION
    HR_ASSERT(d3d12->device->QueryInterface(&d3d12->info_queue));
    d3d12->info_queue->RegisterMessageCallback(&d3d12_message_callback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr,
                                               nullptr);
#endif

    // Create graphics queue
    D3D12_COMMAND_QUEUE_DESC graphics_queue_desc{};
    graphics_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    zero_mem(graphics_queue_desc);
    HR_ASSERT(d3d12->device->CreateCommandQueue(&graphics_queue_desc, IID_PPV_ARGS(&d3d12->graphics_queue)));
    AV_LOG(info, "D3D12 Graphics queue created");

    return true;
  }

  void d3d12_rhi_shutdown(RHI* rhi) {
    RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);

    // Release the graphics queue
    d3d12->graphics_queue->Release();

#ifdef AVIO_ENABLE_GPU_VALIDATION
    d3d12->info_queue->Release();
#endif
    // Release the info queue

    // Release the device.
    // TODO: Need to clear the queue.
    d3d12->device->Release();

    // Release the adapter
    d3d12->adapter->Release();

    // Release factory (must go last)
    d3d12->dxgi_factory->Release();

    AV_LOG(info, "D3D12 Rhi terminated.");
  }

  static void d3d12_rhi_submit_frame(RHI* rhi) {}

  void init_global_rhi_pointers() {
    get_rhi = get_rhi_d3d12;
    rhi_submit_frame = d3d12_rhi_submit_frame;

    // Surface pointers
    rhi_create_surface = d3d12_create_surface;
    rhi_destroy_surface = d3d12_destroy_surface;

    // Swapchain pointers
    rhi_create_swapchain = d3d12_create_swapchain;
    rhi_destroy_swapchain = d3d12_destroy_swapchain;
    rhi_present_swapchain = d3d12_present_swapchain;
  }

}  // namespace avio::dx12
