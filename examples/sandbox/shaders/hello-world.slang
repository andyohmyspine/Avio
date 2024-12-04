module hello_world;
import avio_shader;
import core.avio_core;

StructuredBuffer<float> buffer0;
StructuredBuffer<float> buffer1;
RWStructuredBuffer<float> result;

[shader("compute")]
[numthreads(1,1,1)]
func computeMain(uint3 thread_id: SV_DispatchThreadID)
{
    uint index = thread_id.x;
    result[index] = avio::add_two_numbers(buffer0[index], buffer1[index]);
}