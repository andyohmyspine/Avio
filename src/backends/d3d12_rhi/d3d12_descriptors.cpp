#include "d3d12_descriptors.hpp"
#include "d3d12_rhi.hpp"

namespace avio::dx12 {
  static inline constexpr string_view to_string(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
    switch (heap_type) {
      case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: {
        return "CBV_SRV_UAV";
      } break;
      case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: {
        return "SAMPLER";
      } break;
      case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: {
        return "RTV";
      } break;
      case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: {
        return "DSV";
      } break;
      default:
        return {};
    }

    return {};
  }

  D3D12DescriptorPool d3d12_create_descriptor_pool(RhiD3D12* rhi, D3D12_DESCRIPTOR_HEAP_TYPE heap_type,
                                                   uint64_t num_descriptors, bool shader_visible) {

    D3D12DescriptorPool out_pool{
        .heap_type = heap_type,
        .num_descriptors_total = num_descriptors,
        .is_shader_visible = shader_visible,
    };

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc{
        .Type = heap_type,
        .NumDescriptors = static_cast<UINT>(num_descriptors),
        .Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0,
    };

    HR_ASSERT(rhi->device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&out_pool.heap)));
    out_pool.cpu_handle = out_pool.heap->GetCPUDescriptorHandleForHeapStart();

    if (shader_visible) {
      out_pool.gpu_handle = out_pool.heap->GetGPUDescriptorHandleForHeapStart();
    }

    out_pool.descriptor_increment_size = rhi->device->GetDescriptorHandleIncrementSize(heap_type);
    AV_LOG(info, "D3D12 {} {} descriptor pool created with {} descriptors.", to_string(heap_type),
           shader_visible ? "shader-visible" : "non-shader-visible", num_descriptors);
    return out_pool;
  }
  void d3d12_destroy_descriptor_pool(RhiD3D12* rhi, D3D12DescriptorPool* pool) {
    if (pool && pool->heap) {
      pool->heap->Release();
    }

    zero_mem(pool);
  }
  D3D12DescriptorHandle d3d12_allocate_descriptors(D3D12DescriptorPool* pool, uint64_t num_descriptors) {
    D3D12DescriptorHandle out_handle{};
    
    // TODO: Handle free list

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle(pool->cpu_handle, (INT)pool->allocated_descriptors, pool->descriptor_increment_size);
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpu_handle(pool->gpu_handle);
    if(pool->is_shader_visible) {
      gpu_handle.Offset((int)pool->allocated_descriptors, pool->descriptor_increment_size);
    }

    out_handle.cpu_handle = cpu_handle;
    out_handle.gpu_handle = gpu_handle;
    out_handle.shader_visible = pool->is_shader_visible;
    out_handle.descriptor_increment_size = pool->descriptor_increment_size;
    out_handle.num_descriptors = num_descriptors;

    pool->allocated_descriptors += num_descriptors;
    return out_handle;
  }
}  // namespace avio::dx12