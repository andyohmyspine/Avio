#include "d3d12_rhi.hpp"
#include "d3d12_render_commands.hpp"
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

  static void d3d12_create_command_block(RhiD3D12* d3d12);
  static void d3d12_create_sync(RhiD3D12* d3d12);

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

    d3d12_create_command_block(d3d12);
    d3d12_create_sync(d3d12);

    return true;
  }

  void d3d12_rhi_shutdown(RHI* rhi) {
    RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);

    d3d12_flush_command_queue(d3d12);

    for (uint8_t index = 0; index < RHI_NUM_FRAMES_IN_FLIGHT; ++index) {
      d3d12->command_allocators[index]->Release();
      d3d12->command_lists[index]->Release();
    }

    d3d12->fence->Release();

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

  void d3d12_create_command_block(RhiD3D12* d3d12) {
    for (uint8_t index = 0; index < RHI_NUM_FRAMES_IN_FLIGHT; ++index) {
      HR_ASSERT(d3d12->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                      IID_PPV_ARGS(&d3d12->command_allocators[index])));

      HR_ASSERT(d3d12->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d12->command_allocators[index],
                                                 nullptr, IID_PPV_ARGS(&d3d12->command_lists[index])));

      // Close all command lists except the first one, because we need it straight away.
      HR_ASSERT(d3d12->command_lists[index]->Close());
    }
  }

  void d3d12_create_sync(RhiD3D12* d3d12) {
    HR_ASSERT(d3d12->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12->fence)));
  }

  static void signal_frame_in_flight(RhiD3D12* d3d12) {
    d3d12->fence_frame_values[d3d12->base.current_frame_in_flight] = ++d3d12->fence_value;
    HR_ASSERT(d3d12->graphics_queue->Signal(d3d12->fence, d3d12->fence_value));
  }

  static void d3d12_rhi_submit_frame(RHI* rhi) {
    auto d3d12 = cast_rhi<RhiD3D12>(rhi);
    // Close the primary cmd
    HR_ASSERT(d3d12->command_lists[rhi->current_frame_in_flight]->Close());

    ID3D12CommandList* cmds[] = {d3d12->command_lists[rhi->current_frame_in_flight]};
    d3d12->graphics_queue->ExecuteCommandLists(_countof(cmds), cmds);
    signal_frame_in_flight(d3d12);

    rhi->current_frame_in_flight = (rhi->current_frame_in_flight + 1) % RHI_NUM_FRAMES_IN_FLIGHT;
  }

  void d3d12_flush_command_queue(RhiD3D12* d3d12) {
    if (HANDLE event_handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS)) {
      d3d12->fence_value++;

      HR_ASSERT(d3d12->graphics_queue->Signal(d3d12->fence, d3d12->fence_value));
      HR_ASSERT(d3d12->fence->SetEventOnCompletion(d3d12->fence_value, event_handle));

      WaitForSingleObject(event_handle, INFINITE);
      CloseHandle(event_handle);
    }
  }

  ID3D12GraphicsCommandList4* get_current_command_list(RhiD3D12* rhi) {
    return rhi->command_lists[rhi->base.current_frame_in_flight];
  }

  static void d3d12_begin_frame(RHI* rhi) {
    AV_ASSERT_MSG(!rhi->has_began_frame, "Failed to begin the frame. Did you forget to end the frame?");
    rhi->has_began_frame = true;

    auto d3d12 = cast_rhi<RhiD3D12>(rhi);

    // Synchronize next frame in flight
    const auto fence_frame_value = d3d12->fence_frame_values[rhi->current_frame_in_flight];
    if (fence_frame_value != 0 && d3d12->fence->GetCompletedValue() < fence_frame_value) {
      if (HANDLE event_handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS)) {
        HR_ASSERT(d3d12->fence->SetEventOnCompletion(fence_frame_value, event_handle));
        WaitForSingleObject(event_handle, INFINITE);
        CloseHandle(event_handle);
      }
    }

    // Begin new frame recording
    HR_ASSERT(d3d12->command_allocators[rhi->current_frame_in_flight]->Reset());
    HR_ASSERT(d3d12->command_lists[rhi->current_frame_in_flight]->Reset(
        d3d12->command_allocators[rhi->current_frame_in_flight], nullptr));
  }

  static void d3d12_end_frame(RHI* rhi) {
    AV_ASSERT_MSG(rhi->has_began_frame, "Failed to end the frame. Did you forget to begin the frame?");
    rhi->has_began_frame = false;
  }

  void init_global_rhi_pointers() {
    get_rhi = get_rhi_d3d12;
    rhi_submit_frame = d3d12_rhi_submit_frame;
    rhi_begin_frame = d3d12_begin_frame;
    rhi_end_frame = d3d12_end_frame;

    // Surface pointers
    rhi_create_surface = d3d12_create_surface;
    rhi_destroy_surface = d3d12_destroy_surface;

    // Swapchain pointers
    rhi_create_swapchain = d3d12_create_swapchain;
    rhi_destroy_swapchain = d3d12_destroy_swapchain;
    rhi_present_swapchain = d3d12_present_swapchain;

    detail::init_cmd_pointers();
  }

}  // namespace avio::dx12
