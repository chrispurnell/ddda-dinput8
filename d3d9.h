#pragma once
#include <d3d9.h>

namespace Hooks
{
	bool D3D9(void(*onInit)(LPDIRECT3DDEVICE9), void(*onLostDevice)(), void(*onResetDevice)(), void(*onEndScene)());
}

class fIDirect3DDevice9 : public IDirect3DDevice9
{
public:
	LPDIRECT3DDEVICE9 pD3DDevice;

	virtual ~fIDirect3DDevice9() { }
	fIDirect3DDevice9() : fIDirect3DDevice9(nullptr) {	}
	fIDirect3DDevice9(LPDIRECT3DDEVICE9 pD3DDevice) { this->pD3DDevice = pD3DDevice; }

	HRESULT __stdcall QueryInterface(const IID& riid, void** ppvObj) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;
	HRESULT __stdcall TestCooperativeLevel() override;
	UINT __stdcall GetAvailableTextureMem() override;
	HRESULT __stdcall EvictManagedResources() override;
	HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9) override;
	HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps) override;
	HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) override;
	HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) override;
	HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) override;
	void __stdcall SetCursorPosition(int X, int Y, DWORD Flags) override;
	BOOL __stdcall ShowCursor(BOOL bShow) override;
	HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) override;
	HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) override;
	UINT __stdcall GetNumberOfSwapChains() override;
	HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) override;
	HRESULT __stdcall Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) override;
	HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) override;
	HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) override;
	HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) override;
	void __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp) override;
	void __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) override;
	HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;
	HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) override;
	HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) override;
	HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) override;
	HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) override;
	HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
	HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
	HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestPoint) override;
	HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) override;
	HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) override;
	HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) override;
	HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) override;
	HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color) override;
	HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
	HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) override;
	HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) override;
	HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) override;
	HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) override;
	HRESULT __stdcall BeginScene() override;
	HRESULT __stdcall EndScene() override;
	HRESULT __stdcall Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) override;
	HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) override;
	HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) override;
	HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) override;
	HRESULT __stdcall SetViewport(const D3DVIEWPORT9* pViewport) override;
	HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) override;
	HRESULT __stdcall SetMaterial(const D3DMATERIAL9* pMaterial) override;
	HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) override;
	HRESULT __stdcall SetLight(DWORD Index, const D3DLIGHT9*) override;
	HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9*) override;
	HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) override;
	HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) override;
	HRESULT __stdcall SetClipPlane(DWORD Index, const float* pPlane) override;
	HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) override;
	HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override;
	HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) override;
	HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) override;
	HRESULT __stdcall BeginStateBlock() override;
	HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) override;
	HRESULT __stdcall SetClipStatus(const D3DCLIPSTATUS9* pClipStatus) override;
	HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) override;
	HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) override;
	HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) override;
	HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) override;
	HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) override;
	HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) override;
	HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) override;
	HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) override;
	HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) override;
	HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) override;
	HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) override;
	HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) override;
	HRESULT __stdcall SetScissorRect(const RECT* pRect) override;
	HRESULT __stdcall GetScissorRect(RECT* pRect) override;
	HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) override;
	BOOL __stdcall GetSoftwareVertexProcessing() override;
	HRESULT __stdcall SetNPatchMode(float nSegments) override;
	float __stdcall GetNPatchMode() override;
	HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;
	HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) override;
	HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
	HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
	HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) override;
	HRESULT __stdcall CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) override;
	HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) override;
	HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) override;
	HRESULT __stdcall SetFVF(DWORD FVF) override;
	HRESULT __stdcall GetFVF(DWORD* pFVF) override;
	HRESULT __stdcall CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader) override;
	HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) override;
	HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) override;
	HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) override;
	HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) override;
	HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) override;
	HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) override;
	HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) override;
	HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) override;
	HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) override;
	HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride) override;
	HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Setting) override;
	HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) override;
	HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) override;
	HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) override;
	HRESULT __stdcall CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader) override;
	HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) override;
	HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) override;
	HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) override;
	HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) override;
	HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) override;
	HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) override;
	HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) override;
	HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) override;
	HRESULT __stdcall DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo) override;
	HRESULT __stdcall DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo) override;
	HRESULT __stdcall DeletePatch(UINT Handle) override;
	HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) override;
};
