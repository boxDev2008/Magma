#pragma once

const char HLSL_QUAD_FRAG[] = 
"struct PSInput"
"{"
"    float4 pos : SV_Position;"
"    float4 color : COLOR;"
"};"

"float4 PSMain(PSInput input) : SV_Target"
"{"
"    return float4(1.0, 1.0, 1.0, 1.0);"
"}\0";