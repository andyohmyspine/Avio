#include "d3d12_swapchain.hpp"
#include "d3d12_rhi.hpp"

namespace avio::dx12 {

  static void acquire_swapchain_images(RhiD3D12* d3d12, RhiSwapchain* swapchain);

  RhiSwapchain* d3d12_create_swapchain(RHI* rhi, const infos::RhiSwapchainInfo& info) {
    RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);
    D3D12Swapchain* out_swapchain = d3d12->swapchains.allocate();
    out_swapchain->base.info = info;

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = 0;
    desc.Height = 0;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Stereo = FALSE;
    desc.SampleDesc = {1, 0};
    desc.BufferCount = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = info.override_image_count > 0 ? info.override_image_count : RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags = info.allow_vsync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    out_swapchain->image_count = desc.BufferCount;

    IDXGISwapChain1* swap_chain = {};
    HR_ASSERT(d3d12->dxgi_factory->CreateSwapChainForHwnd(d3d12->graphics_queue, info.surface->info.hwnd, &desc,
                                                          nullptr, nullptr, &swap_chain));

    HR_ASSERT(swap_chain->QueryInterface(&out_swapchain->swapchain));
    swap_chain->Release();

    acquire_swapchain_images(d3d12, &out_swapchain->base);

    return &out_swapchain->base;
  }

  void acquire_swapchain_images(RhiD3D12* d3d12, RhiSwapchain* swapchain) {
    D3D12Swapchain* d3d12_sc = cast_rhi<D3D12Swapchain>(swapchain);
    const bool has_more_than_default_images = d3d12_sc->image_count > RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT;

    if (has_more_than_default_images) {
      d3d12_sc->swapchain_images.set_is_vector(d3d12_sc->image_count);
      d3d12_sc->swapchain_image_views.set_is_vector(d3d12_sc->image_count);
    } else {
      d3d12_sc->swapchain_images.set_is_array();
      d3d12_sc->swapchain_image_views.set_is_array();
    }

    // Get the desc so we can get the width and height of the image
    DXGI_SWAP_CHAIN_DESC1 desc{};
    HR_ASSERT(d3d12_sc->swapchain->GetDesc1(&desc));
    for (uint32_t index = 0; index < d3d12_sc->image_count; ++index) {
      D3D12Image out_image = {
          .base{
              .type = ImageType::image_2d,
              .width = desc.Width,
              .height = desc.Height,
              .format = PixelFormat::window_output,
          },
          .native_format = desc.Format,
      };
      d3d12_sc->width = out_image.base.width;
      d3d12_sc->height = out_image.base.height;

      HR_ASSERT(d3d12_sc->swapchain->GetBuffer(index, IID_PPV_ARGS(&out_image.image)));
      d3d12_sc->swapchain_images[index] = out_image;

      D3D12DescriptorHandle rtv_handle = d3d12_allocate_descriptors(&d3d12->rtv_descriptor_pool, 1);
      d3d12->device->CreateRenderTargetView(out_image.image, nullptr, rtv_handle.cpu_handle);
      d3d12_sc->swapchain_image_views[index] = D3D12ImageView{
          .base =
              {
                  .image = &d3d12_sc->swapchain_images[index].base,
                  .type = ImageViewType::color,
              },
          .heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
          .descriptor_handle = rtv_handle,
      };
    }
  }

  void d3d12_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);
    d3d12_flush_command_queue(d3d12);

    D3D12Swapchain* d3d12_sc = (D3D12Swapchain*)swapchain;
    for (uint32_t index = 0; index < d3d12_sc->image_count; ++index) {
      d3d12_sc->swapchain_images[index].image->Release();
    }

    d3d12_sc->swapchain->Release();
    d3d12->swapchains.deallocate(d3d12_sc);
  }

  void d3d12_present_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    D3D12Swapchain* d3d12_sc = (D3D12Swapchain*)swapchain;

    const UINT swap_interval = swapchain->info.allow_vsync ? 1 : 0;
    const UINT flags = swapchain->info.allow_vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING;
    HR_ASSERT(d3d12_sc->swapchain->Present(swap_interval, flags));

    d3d12_sc->current_back_buffer_index = d3d12_sc->swapchain->GetCurrentBackBufferIndex();

    RECT window_rect{};
    GetClientRect(d3d12_sc->base.info.surface->info.hwnd, &window_rect);

    // Check if we need to resize
    const uint32_t width = (uint32_t)window_rect.right - (uint32_t)window_rect.left;
    const uint32_t height = (uint32_t)window_rect.bottom - (uint32_t)window_rect.top;
    if (width != d3d12_sc->width || height != d3d12_sc->height) {

      // Resize the swapchain
      d3d12_flush_command_queue(cast_rhi<RhiD3D12>(rhi));

      // First destroy the swapchain
      infos::RhiSwapchainInfo sc_info = d3d12_sc->base.info;
      d3d12_destroy_swapchain(rhi, swapchain);

      // Then recreate the swapchain
      RhiSwapchain* new_sc = d3d12_create_swapchain(rhi, sc_info);

      // Just copy a new swapchain to the old place.
      *d3d12_sc = std::move(*(D3D12Swapchain*)new_sc);
    }
  }

}  // namespace avio::dx12