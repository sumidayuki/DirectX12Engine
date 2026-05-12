{
    "HLSLPath": "ScreenSpaceOverlay.hlsl",
    "VS_Entry" : "VSMain",
    "VS_ShaderModel" : "vs_5_1",
    "PS_Entry": "PSMain",
    "PS_ShaderModel" : "ps_5_1",
    "UseSkinning" : false,

    "InputLayout": 
    [
        { "Semantic": "POSITION", "Format": "R32G32_FLOAT" },
        { "Semantic": "TEXCOORD", "Format": "R32G32_FLOAT" }
    ],

    "PrimitiveTopology": "Triangle",

    "Rasterizer": 
    {
        "FillMode": "Solid",
        "CullMode": "None",
        "FrontCCW": false
    },

    "Depth": 
    {
        "Enable": false,
        "WriteMask": "Zero",
        "Func": "Less"
    },

    "Blend": 
    {
        "Enable": true,
        "Src": "SRC_ALPHA",
        "Dst": "INV_SRC_ALPHA",
        "Op": "ADD"
    },

    "RTVFormat": "R8G8B8A8_UNORM",
    "DSVFormat":  "D32_FLOAT"
}