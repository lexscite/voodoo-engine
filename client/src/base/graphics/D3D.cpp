#include "D3D.h"
#include "..\Engine.h"

D3D::D3D()
{
	swap_chain_ = nullptr;
	device_ = nullptr;
	device_context_ = nullptr;
	render_target_view_ = nullptr;
	depth_stencil_buffer_ = nullptr;
	depth_stencil_state_ = nullptr;
	depth_stencil_view_ = nullptr;
	rasterizer_state_ = nullptr;
}

D3D* D3D::singleton_;

D3D* D3D::GetSingleton()
{
	if (singleton_ == 0)
		singleton_ = new D3D;

	return singleton_;
}

bool D3D::Init(UINT screen_width, UINT screen_height, bool vsync_enabled, bool fullscreen_enabled, float screen_depth, float screen_near)
{
	HRESULT result = S_OK;

	vsync_enabled_ = vsync_enabled;
	fullscreen_enabled_ = fullscreen_enabled;

	IDXGIFactory* factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		std::cerr << "Failed to create DXGIFactory" << std::endl;
		return false;
	}

	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		std::cerr << "Failed to enum adapters" << std::endl;
		return false;
	}

	IDXGIOutput* adapter_output;
	result = adapter->EnumOutputs(0, &adapter_output);
	if (FAILED(result))
	{
		std::cerr << "Failed to enum adapter outputs" << std::endl;
		return false;
	}

	UINT num_modes;
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);
	if (FAILED(result))
	{
		std::cerr << "Failed to get the number of modes" << std::endl;
		return false;
	}

	DXGI_MODE_DESC* display_mode_list;
	display_mode_list = new DXGI_MODE_DESC[num_modes];
	if (!display_mode_list)
	{
		std::cerr << "Failed to create mode list" << std::endl;
		return false;
	}

	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_mode_list);
	if (FAILED(result))
	{
		std::cerr << "Failed to fill display mode list struct" << std::endl;
		return false;
	}

	UINT numerator, denominator;
	for (UINT i = 0; i<num_modes; i++)
	{
		if (display_mode_list[i].Width == (unsigned int)screen_width)
		{
			if (display_mode_list[i].Height == (unsigned int)screen_height)
			{
				numerator = display_mode_list[i].RefreshRate.Numerator;
				denominator = display_mode_list[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC adapter_desc;
	result = adapter->GetDesc(&adapter_desc);
	if (FAILED(result))
	{
		std::cerr << "Failed to get adapter desc" << std::endl;
		return false;
	}

	adapter_memory_ = (UINT)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	size_t stringLength;
	int error = wcstombs_s(&stringLength, adapter_desc_, 128, adapter_desc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	safe_delete_arr(display_mode_list);
	safe_release(adapter_output);
	safe_release(adapter);
	safe_release(factory);

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.OutputWindow = Engine::GetSingleton()->GetHwnd();
	swap_chain_desc.Windowed = !fullscreen_enabled_;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = screen_width;
	swap_chain_desc.BufferDesc.Height = screen_height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = vsync_enabled_ ? numerator : 0;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = vsync_enabled_ ? denominator : 1;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;

	D3D_FEATURE_LEVEL feature_level[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	UINT creation_flags = 0;
#if defined(_DEBUG)
	creation_flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	for (int i = 0; i < ARRAYSIZE(feature_level); i++)
	{
		result = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_REFERENCE,
			NULL,
			creation_flags,
			&feature_level[i],
			6,
			D3D11_SDK_VERSION,
			&swap_chain_desc,
			&swap_chain_,
			&device_,
			&feature_level_,
			&device_context_
		);

		if (SUCCEEDED(result))
			break;
	}
	if (FAILED(result))
	{
		std::cerr << "Failed to create DirectX device and swap chain" << std::endl;
		return false;
	}

	ID3D11Texture2D* back_buffer;
	result = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	if (FAILED(result))
	{
		std::cerr << "Failed to get the back buffer pointer" << std::endl;
		return false;
	}

	result = device_->CreateRenderTargetView(back_buffer, NULL, &render_target_view_);
	if (FAILED(result))
	{
		std::cerr << "Failed to create render target view" << std::endl;
		return false;
	}

	safe_release(back_buffer);

	D3D11_TEXTURE2D_DESC depth_buffer_desc;
	ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));
	depth_buffer_desc.Width = screen_width;
	depth_buffer_desc.Height = screen_height;
	depth_buffer_desc.MipLevels = 1;
	depth_buffer_desc.ArraySize = 1;
	depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc.Count = 1;
	depth_buffer_desc.SampleDesc.Quality = 0;
	depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_buffer_desc.CPUAccessFlags = 0;
	depth_buffer_desc.MiscFlags = 0;

	result = device_->CreateTexture2D(&depth_buffer_desc, NULL, &depth_stencil_buffer_);
	if (FAILED(result))
	{
		std::cerr << "Failed to create back buffer texture" << std::endl;
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	result = device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_);
	if (FAILED(result))
	{
		std::cerr << "Failed to create depth stencil state" << std::endl;
		return false;
	}

	device_context_->OMSetDepthStencilState(depth_stencil_state_, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	result = device_->CreateDepthStencilView(depth_stencil_buffer_, &depth_stencil_view_desc, &depth_stencil_view_);
	if (FAILED(result))
	{
		std::cerr << "Failed to create depth stencil view" << result << std::endl;
		return false;
	}

	device_context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);

	D3D11_RASTERIZER_DESC rasterizer_desc;
	rasterizer_desc.AntialiasedLineEnable = false;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.FrontCounterClockwise = false;
	rasterizer_desc.MultisampleEnable = false;
	rasterizer_desc.ScissorEnable = false;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;

	result = device_->CreateRasterizerState(&rasterizer_desc, &rasterizer_state_);
	if (FAILED(result))
	{
		std::cerr << "Failed to create rasterizer state" << std::endl;
		return false;
	}

	device_context_->RSSetState(rasterizer_state_);

	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)screen_width;
	viewport.Height = (FLOAT)screen_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	device_context_->RSSetViewports(1, &viewport);

	float fov = 3.141592654f / 4.0f;
	float screen_aspect = (float)(screen_width / screen_height);
	projection_matrix_ = XMMatrixPerspectiveFovLH(fov, screen_aspect, screen_near, screen_depth);
	world_matrix_ = XMMatrixIdentity();
	ortho_matrix_ = XMMatrixOrthographicLH((float)screen_width, (float)screen_height, screen_near, screen_depth);

	device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void D3D::Release()
{
	if (swap_chain_)
		swap_chain_->SetFullscreenState(false, NULL);

	if (rasterizer_state_)
		safe_release(rasterizer_state_);

	if (depth_stencil_view_)
		safe_release(depth_stencil_view_);

	if (depth_stencil_state_)
		safe_release(depth_stencil_state_);

	if (depth_stencil_buffer_)
		safe_release(depth_stencil_buffer_);

	if (render_target_view_)
		safe_release(render_target_view_);

	if (device_context_)
		safe_release(device_context_);

	if (device_)
		safe_release(device_);

	if (swap_chain_)
		safe_release(swap_chain_);
}

void D3D::BeginScene(Color* color)
{
	float clear_color[4] = { color->r, color->g, color->b, color->a };
	device_context_->ClearRenderTargetView(render_target_view_, clear_color);
	device_context_->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	if (vsync_enabled_)
		swap_chain_->Present(1, 0);
	else
		swap_chain_->Present(0, 0);
}

ID3D11Device* D3D::GetDevice()
{
	return device_;
}

ID3D11DeviceContext* D3D::GetDeviceContext()
{
	return device_context_;
}

void D3D::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = projection_matrix_;
}


void D3D::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = world_matrix_;
}


void D3D::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = ortho_matrix_;
}