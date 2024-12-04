module hello_world;

import core.avio_core;

struct VertexInput {
    float3 position : POSITION;
    float4 color : COLOR;
}

struct VsToPs {
    float4 position : SV_Position;
    float4 color : COLOR;    
}

[require(sm_6_6)]
[shader("vertex")]
VsToPs vs_main(in VertexInput vertex) {
    VsToPs output;
    output.position = float4(vertex.position.xyz, 1.0f);
    output.color = vertex.color;
    return output;
}

[require(sm_6_6)]
[shader("pixel")]
float4 ps_main(in VsToPs input) : SV_Target0 {
    return input.color;
}