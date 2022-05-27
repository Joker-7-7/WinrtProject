#pragma once

//using namespace Microsoft::WRL;

using namespace winrt;
using namespace Windows::UI::Core;
using namespace DirectX;


enum ConstanBuffer
{
    CB_Appliation,
    CB_Frame,
    CB_Object,
    NumConstantBuffers
};

// a struct to represent a single vertex
struct VertexPosColor
{
    XMFLOAT3 Position;
    XMFLOAT3 Color;
};


class CGame
{
private:
    winrt::com_ptr<ID3D11Device3> m_d3dDevice;                 // the device interface
    winrt::com_ptr<ID3D11DeviceContext3> m_d3dContext;         // the device context interface
    winrt::com_ptr<IDXGISwapChain1>       m_swapChain;
    winrt::com_ptr<ID3D11RenderTargetView>  m_d3dRenderTargetView;  // the render target interface
    winrt::com_ptr<ID3D11RasterizerState> m_d3dRasterizer;

    winrt::com_ptr<ID3D11Buffer> vertexbufferForModel;              // the vertex buffer interface
    winrt::com_ptr<ID3D11Buffer> vertexbufferForPlaneFirst;
    winrt::com_ptr<ID3D11Buffer> vertexbufferForPlaneSecond;
    winrt::com_ptr<ID3D11VertexShader> vertexshader;        // the vertex shader interface
    winrt::com_ptr<ID3D11PixelShader> pixelshader;          // the pixel shader interface
    winrt::com_ptr<ID3D11InputLayout> inputlayout;          // the input layout interface


    winrt::com_ptr<ID3D11Buffer>                m_constantBufferChangeOnResize;
    winrt::com_ptr<ID3D11Buffer>                m_constantBufferChangesEveryFrame;
    winrt::com_ptr<ID3D11Buffer>                m_constantBufferChangesEveryPrim;

public:
    float angle = 0.0;
    float deltaTime = 0.05;
    float heightZ = -50;
    float deltaZ = 10;

    void Initialize();
    void Update();
    void Render();
    void Uninitialize();

    void InitGraphics();
    void InitPipeline();
};