{
    "HLSLPath": "MeshRenderer.hlsl",
    "VS_Entry" : "VSMain",
    "VS_ShaderModel" : "vs_5_1",
    "PS_Entry": "PSMain",
    "PS_ShaderModel" : "ps_5_1",

    "InputLayout": 
    [
        { "Semantic": "POSITION", "Format": "R32G32B32_FLOAT" },
        { "Semantic": "NORMAL",   "Format": "R32G32B32_FLOAT" },
        { "Semantic": "TEXCOORD", "Format": "R32G32_FLOAT" },
        { "Semantic": "TANGENT",  "Format": "R32G32B32_FLOAT" }
    ],

    "PrimitiveTopology": "Triangle",

    "Rasterizer": 
    {
        "FillMode": "Solid",
        "CullMode": "Back",
        "FrontCCW": false
    },

    "Depth": 
    {
        "Enable": true,
        "WriteMask": "All",
        "Func": "Less"
    },

    "Blend": 
    {
        "Enable": false,
        "Src": "SRC_ALPHA",
        "Dst": "INV_SRC_ALPHA",
        "Op": "ADD"
    },

    "RTVFormat": "R8G8B8A8_UNORM",

    "DSVFormat":  "D32_FLOAT"
}