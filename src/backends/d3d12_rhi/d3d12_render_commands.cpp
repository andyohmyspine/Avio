#include "d3d12_render_commands.hpp"
#include "d3d12_rhi.hpp"

namespace avio::dx12 {
  struct RhiCmdD3D12 {
    RhiD3D12* d3d12;
    ID3D12GraphicsCommandList4* cmd;
  };

  inline RhiCmdD3D12 get_cmd(RHI* rhi) {
    RhiD3D12* d3d12 = cast_rhi<RhiD3D12>(rhi);
    return {d3d12, get_current_command_list(d3d12)};
  }

  // -------------------------------------------------------------------------------------
  void d3d12_cmd_begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    auto [d3d12, cmd] = get_cmd(rhi);
    auto d3d12_sc = cast_rhi<D3D12Swapchain>(swapchain);

    ID3D12Resource* image = d3d12_current_swapchain_image(d3d12_sc)->image;
    auto transition =
        CD3DX12_RESOURCE_BARRIER::Transition(image, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    cmd->ResourceBarrier(1, &transition);

    auto rtv = d3d12_current_swapchain_rtv(d3d12_sc);
    cmd->OMSetRenderTargets(1, &rtv->descriptor_handle.cpu_handle, FALSE, nullptr);

    FLOAT clear_color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    cmd->ClearRenderTargetView(rtv->descriptor_handle.cpu_handle, clear_color, 0, nullptr);
  }

  // -------------------------------------------------------------------------------------
  void d3d12_cmd_end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    auto [d3d12, cmd] = get_cmd(rhi);
    auto d3d12_sc = cast_rhi<D3D12Swapchain>(swapchain);

    ID3D12Resource* image = d3d12_sc->swapchain_images[d3d12_sc->current_back_buffer_index].image;

    auto transition =
        CD3DX12_RESOURCE_BARRIER::Transition(image, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    cmd->ResourceBarrier(1, &transition);
  }

  // -------------------------------------------------------------------------------------
  void detail::init_cmd_pointers() {
    rhi_cmd_begin_draw_to_swapchain = d3d12_cmd_begin_draw_to_swapchain;
    rhi_cmd_end_draw_to_swapchain = d3d12_cmd_end_draw_to_swapchain;
  }
}  // namespace avio::dx12