#include "pch.h"
#include "Game.h"
#include "vertex.h"
#include <fstream>


ID3D11Buffer* g_d3dConstantBuffers[NumConstantBuffers];

// Demo parameters
XMMATRIX g_WorldMatrix = DirectX::XMMatrixIdentity();
XMMATRIX g_ViewMatrix = DirectX::XMMatrixIdentity();
XMMATRIX g_ProjectionMatrix = DirectX::XMMatrixIdentity();
std::vector<Vertex> vertices;


bool operator ==(Vertex a, Vertex b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

// this function loads a file into an :vector<byte>
std::vector<byte>* LoadShaderFile(std::string File)
{
    std::vector<byte>* FileData =  nullptr;

    // open the file
    std::ifstream VertexFile(File, std::ios::in | std::ios::binary | std::ios::ate);

    // if open was successful
    if (VertexFile.is_open())
    {
        // find the length of the file
        int Length = (int)VertexFile.tellg();

        // collect the file data
        FileData = new std::vector<byte>(Length);
        VertexFile.seekg(0, std::ios::beg);
        VertexFile.read(reinterpret_cast<char*>(FileData->data()), Length);
        VertexFile.close();
    }

    return FileData;
}


void CGame::Initialize()
{
    // Define temporary pointers to a device and a device context
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;

    // Create the device and device context objects
    D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        device.put(),
        nullptr,
        context.put());

    // Convert the pointers from the DirectX 11 versions to the DirectX 11.1 versions
    m_d3dDevice = device.as<ID3D11Device3>();
    m_d3dContext = context.as<ID3D11DeviceContext3>();

    
    com_ptr<IDXGIDevice1> dxgiDevice;
    dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();
    com_ptr<IDXGIAdapter> dxgiAdapter;
    winrt::check_hresult(
        dxgiDevice->GetAdapter(dxgiAdapter.put())
    );
    winrt::com_ptr<IDXGIFactory2> dxgiFactory;
    winrt::check_hresult(
        dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
    );

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
    swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.

    CoreWindow window = CoreWindow::GetForCurrentThread();

    winrt::check_hresult(
        dxgiFactory->CreateSwapChainForCoreWindow(
            m_d3dDevice.get(),
            winrt::get_unknown(window),
            &swapChainDesc,
            nullptr,
            m_swapChain.put()
        )
    );

    // Create a render target view of the swap chain back buffer.
    winrt::com_ptr<ID3D11Texture2D> backBuffer = winrt::capture<ID3D11Texture2D>(m_swapChain, &IDXGISwapChain1::GetBuffer, 0);

    winrt::check_hresult(
        m_d3dDevice->CreateRenderTargetView(
            backBuffer.get(),
            nullptr,
            m_d3dRenderTargetView.put()
        )
    );

    // set the viewport
    D3D11_VIEWPORT viewport = { 0 };

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = window.Bounds().Width;
    viewport.Height = window.Bounds().Height;


    m_d3dContext->RSSetViewports(1, &viewport);
    // initialize graphics and the pipeline
    InitGraphics();
    InitPipeline();
}

// this function performs updates to the state of the game
void CGame::Update()
{

    XMVECTOR eyePosition = XMVectorSet(0, 0, heightZ, 1);
    XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
    XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
    g_ViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
    m_d3dContext->UpdateSubresource(g_d3dConstantBuffers[CB_Frame], 0, nullptr, &g_ViewMatrix, 0, 0);

    XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);

    g_WorldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
    m_d3dContext->UpdateSubresource(g_d3dConstantBuffers[CB_Object], 0, nullptr, &g_WorldMatrix, 0, 0);
}

// this function renders a single frame of 3D graphics
void CGame::Render()
{
    // set our new render target object as the active render target
    ID3D11RenderTargetView* const targets[1] = { m_d3dRenderTargetView.get() };
    m_d3dContext->OMSetRenderTargets(1, targets, nullptr);
    const float clearColor[4] = { 0.0f, 0.5f, 0.8f, 1.0f };
    m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView.get(), clearColor);

    // set the vertex buffer
    uint32_t stride{ sizeof(Vertex) };
    uint32_t offset{ 0 };

    ID3D11Buffer* vertexBuffer{ vertexbuffer.get() };
    m_d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    m_d3dContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_d3dContext->VSSetConstantBuffers(0, 3, g_d3dConstantBuffers);

    m_d3dContext->Draw(vertices.size(), 0);

    // switch the back buffer and the front buffer
    m_swapChain->Present(1, 0);
}

void CGame::InitGraphics()
{   
    std::vector<Normal> normals;
    std::vector<Triangle> objectTriangles;

    getGeometryInput("3dmodel.stl", objectTriangles);
    getVerticesForIntersectionTriangles(objectTriangles, vertices);

    D3D11_BUFFER_DESC bd = { 0 };
    //bd.ByteWidth = sizeof(VertexPosColor) * ARRAYSIZE(OurVertices);
    bd.ByteWidth = sizeof(Vertex) * vertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA srd = {/*OurVertices*/ vertices.data(), 0, 0};

    m_d3dDevice->CreateBuffer(&bd, &srd, vertexbuffer.put());
}

void CGame::InitPipeline()
{
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Appliation]);
    m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Frame]);
    m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Object]);


    // load the shader files
    std::vector<byte>*  VSFile = LoadShaderFile("VertexShader.cso");
    std::vector<byte>*  PSFile = LoadShaderFile("PixelShader.cso");

    // create the shader objects
    m_d3dDevice->CreateVertexShader(VSFile->data(), VSFile->size(), nullptr, vertexshader.put());
    m_d3dDevice->CreatePixelShader(PSFile->data(), PSFile->size(), nullptr, pixelshader.put());

    // set the shader objects as the active shaders
    m_d3dContext->VSSetShader(vertexshader.get(), nullptr, 0);
    m_d3dContext->PSSetShader(pixelshader.get(), nullptr, 0);
    // initialize input layout
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // create and set the input layout
    m_d3dDevice->CreateInputLayout(ied, ARRAYSIZE(ied), VSFile->data(), VSFile->size(), inputlayout.put());
    // initialize input layout

    // Setup the projection matrix.
    CoreWindow window = CoreWindow::GetForCurrentThread();

    // Compute the exact client dimensions.
    // This is required for a correct projection matrix.
    float clientWidth = window.Bounds().Width;
    float clientHeight = window.Bounds().Height;

    g_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 1000.0f);
    m_d3dContext->UpdateSubresource(g_d3dConstantBuffers[CB_Appliation], 0, nullptr, &g_ProjectionMatrix, 0, 0);

    m_d3dContext->IASetInputLayout(inputlayout.get());

}

