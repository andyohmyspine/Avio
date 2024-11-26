#include "d3d12_swapchain.hpp"
#include "d3d12_rhi.hpp"

namespace avio::dx12 {
  
  static void acquire_swapchain_images(RhiD3D12* d3d12, RhiSwapchain* swapchain);

  RhiSwapchain* d3d12_create_swapchain(RHI* rhi,
                                       const infos::RhiSwapchainInfo& info) {
    RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);
    D3D12Swapchain* out_swapchain = d3d12->swapchains.allocate();
    out_swapchain->base.info = info;

    DXGI_SWAP_CHAIN_DESC1 desc {};
    desc.Width = 0;
    desc.Height = 0;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Stereo = FALSE;
    desc.SampleDesc = { 1, 0 };
    desc.BufferCount = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = info.override_image_count > 0 ? info.override_image_count : RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags = info.allow_vsync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    out_swapchain->image_count = desc.BufferCount;

    IDXGISwapChain1* swap_chain = {};
    HR_ASSERT(d3d12->dxgi_factory->CreateSwapChainForHwnd(
      d3d12->graphics_queue,
      info.surface->info.hwnd,
      &desc,
      nullptr,
      nullptr,
      &swap_chain
    ));

    HR_ASSERT(swap_chain->QueryInterface(&out_swapchain->swapchain));
    swap_chain->Release();

    acquire_swapchain_images(d3d12, &out_swapchain->base);

    return &out_swapchain->base;
  }

  void acquire_swapchain_images(RhiD3D12* d3d12, RhiSwapchain* swapchain) {
    D3D12Swapchain* d3d12_sc = cast_rhi<D3D12Swapchain>(swapchain);
    const bool has_more_than_default_images = d3d12_sc->image_count > RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT;

    if(has_more_than_default_images) {
      d3d12_sc->swapchain_images.set_is_vector();
      d3d12_sc->swapchain_images.get_vector().resize(d3d12_sc->image_count);
    } else {
      d3d12_sc->swapchain_images.set_is_array();
    }

    for(uint32_t index = 0; index < d3d12_sc->image_count; ++index) {
      HR_ASSERT(d3d12_sc->swapchain->GetBuffer(index, IID_PPV_ARGS(&d3d12_sc->swapchain_images[index])));
    }
  }
  
  void d3d12_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);
    d3d12_flush_command_queue(d3d12);

    D3D12Swapchain* d3d12_sc = (D3D12Swapchain*)swapchain;
    d3d12_sc->swapchain->Release();
    d3d12->swapchains.deallocate(d3d12_sc);
  }

  void d3d12_present_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    D3D12Swapchain* d3d12_sc = (D3D12Swapchain*)swapchain;
    
    const UINT swap_interval = swapchain->info.allow_vsync ? 1 : 0;
    const UINT flags = swapchain->info.allow_vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING;
    HR_ASSERT(d3d12_sc->swapchain->Present(swap_interval, flags));
    d3d12_sc->current_back_buffer_index = d3d12_sc->swapchain->GetCurrentBackBufferIndex();
  }

}  // namespace avio::dx12