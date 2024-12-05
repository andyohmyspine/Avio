#pragma once

#include "core.hpp"
#include "d3d12_common.hpp"

#include <list>

namespace avio::dx12 {
  struct RhiD3D12;

  struct D3D12DescriptorHandle {
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
    bool shader_visible;
    uint64_t num_descriptors;
    uint32_t descriptor_increment_size;
  };

  struct D3D12DescriptorPool {
    // TODO: Base will be here.
    
    D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
    ID3D12DescriptorHeap* heap;

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;

    uint64_t num_descriptors_total = 0;
    uint64_t allocated_descriptors = 0;
    uint32_t descriptor_increment_size = 0;
    bool is_shader_visible = false;

    std::list<D3D12DescriptorHandle> handle_free_list;
  };


  D3D12DescriptorPool d3d12_create_descriptor_pool(RhiD3D12* rhi, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint64_t num_descriptors, bool shader_visible);
  void d3d12_destroy_descriptor_pool(RhiD3D12* rhi, D3D12DescriptorPool* pool);

  D3D12DescriptorHandle d3d12_allocate_descriptors(D3D12DescriptorPool* pool, uint64_t num_descriptors);
  void d3d12_destroy_descriptors(D3D12DescriptorPool* pool, D3D12DescriptorHandle* handle);
}