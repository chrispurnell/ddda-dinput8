#include "imgui.h"
#include "imgui_impl_dx9.h"
#include <d3dx9.h>

#ifdef __GNUC__
typedef D3DXMATRIX D3DXMATRIXA16 __attribute__((aligned(16)));
#endif

// Data
static INT64                    g_Time = 0;
static INT64                    g_TicksPerSecond = 0;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static LPDIRECT3DVERTEXBUFFER9  g_pVB = nullptr;
static LPDIRECT3DINDEXBUFFER9   g_pIB = nullptr;
static LPDIRECT3DTEXTURE9       g_FontTexture = nullptr;
static LPDIRECT3DSTATEBLOCK9    g_State;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DCOLOR    col;
	D3DXVECTOR2 uv;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

static void ImGui_ImplDX9_RenderDrawLists(ImDrawData* draw_data)
{
	// Create and grow buffers if needed
	if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	{
		if (g_pVB) { g_pVB->Release(); g_pVB = nullptr; }
		g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
		if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, nullptr) < 0)
			return;
	}
	if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
	{
		if (g_pIB) { g_pIB->Release(); g_pIB = nullptr; }
		g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
		if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, nullptr) < 0)
			return;
	}

	// Copy and convert all vertices into a single contiguous buffer
	CUSTOMVERTEX* vtx_dst;
	ImDrawIdx* idx_dst;
	if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
		return;
	if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
		return;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_src = &cmd_list->VtxBuffer[0];
		for (int i = 0; i < cmd_list->VtxBuffer.size(); i++)
		{
			vtx_dst->pos.x = vtx_src->pos.x;
			vtx_dst->pos.y = vtx_src->pos.y;
			vtx_dst->pos.z = 0.0f;
			vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000) >> 16) | ((vtx_src->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
			vtx_dst->uv.x = vtx_src->uv.x;
			vtx_dst->uv.y = vtx_src->uv.y;
			vtx_dst++;
			vtx_src++;
		}
		memcpy(idx_dst, &cmd_list->IdxBuffer[0], cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx));
		idx_dst += cmd_list->IdxBuffer.size();
	}
	g_pVB->Unlock();
	g_pIB->Unlock();

	g_State->Capture();
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetIndices(g_pIB);
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	// Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing
	g_pd3dDevice->SetPixelShader(nullptr);
	g_pd3dDevice->SetVertexShader(nullptr);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// Setup orthographic projection matrix
	D3DXMATRIXA16 mat;
	D3DXMatrixIdentity(&mat);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat);
	D3DXMatrixOrthoOffCenterLH(&mat, 0.5f, ImGui::GetIO().DisplaySize.x + 0.5f, ImGui::GetIO().DisplaySize.y + 0.5f, 0.5f, -1.0f, +1.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);

	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				g_pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE9)pcmd->TextureId);
				g_pd3dDevice->SetScissorRect(&r);
				g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)cmd_list->VtxBuffer.size(), idx_offset, pcmd->ElemCount / 3);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.size();
	}
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	g_State->Apply();
}

IMGUI_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}

bool ImGui_ImplDX9_Init(IDirect3DDevice9* device)
{
	g_pd3dDevice = device;

	if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
		return false;
	if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
		return false;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = ImGui_ImplDX9_RenderDrawLists;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	return true;
}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

	// Upload texture to graphics system
	g_FontTexture = nullptr;
	if (D3DXCreateTexture(g_pd3dDevice, width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &g_FontTexture) < 0)
		return false;
	D3DLOCKED_RECT tex_locked_rect;
	if (g_FontTexture->LockRect(0, &tex_locked_rect, nullptr, 0) != D3D_OK)
		return false;
	for (int y = 0; y < height; y++)
		memcpy((unsigned char *)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
	g_FontTexture->UnlockRect(0);

	// Store our identifier
	io.Fonts->TexID = (void *)g_FontTexture;

	return true;
}

bool ImGui_ImplDX9_CreateDeviceObjects()
{
	if (!g_pd3dDevice)
		return false;
	D3DVIEWPORT9 viewPort;
	g_pd3dDevice->GetViewport(&viewPort);
	ImGui::GetIO().DisplaySize = ImVec2((float)viewPort.Width, (float)viewPort.Height);
	if (!ImGui_ImplDX9_CreateFontsTexture())
		return false;
	if (!g_State)
		g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &g_State);
	return true;
}

void ImGui_ImplDX9_InvalidateDeviceObjects()
{
	if (!g_pd3dDevice)
		return;
	D3DVIEWPORT9 viewPort;
	g_pd3dDevice->GetViewport(&viewPort);
	ImGui::GetIO().DisplaySize = ImVec2((float)viewPort.Width, (float)viewPort.Height);
	if (g_pVB)
	{
		g_pVB->Release();
		g_pVB = nullptr;
	}
	if (g_pIB)
	{
		g_pIB->Release();
		g_pIB = nullptr;
	}
	if (LPDIRECT3DTEXTURE9 tex = (LPDIRECT3DTEXTURE9)ImGui::GetIO().Fonts->TexID)
	{
		tex->Release();
		ImGui::GetIO().Fonts->TexID = nullptr;
	}
	g_FontTexture = nullptr;
	if (g_State)
	{
		g_State->Release();
		g_State = nullptr;
	}
}

void ImGui_ImplDX9_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplDX9_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup time step
	INT64 current_time;
	QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
	g_Time = current_time;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;

	// Start the frame
	ImGui::NewFrame();
}
