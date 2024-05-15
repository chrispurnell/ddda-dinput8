// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "stdafx.h"
#include "Misc.h"
#include "dinput8.h"
#include "InGameUI.h"

namespace
{

LPBYTE pCharCustomization, oCharCustomization;
void __declspec(naked) HCharCustomization()
{
	asm("cmpl	$10,0x284(%ebx)");
	asm("jne	1f");
	asm("movl	$2,0x284(%ebx)");

	asm("\n1:");
	asm("jmp	*%0" : : "m"(oCharCustomization));
}

const float plus = 89.9f, minus = -89.9f;
LPBYTE pExtendVerticalCam1, pExtendVerticalCam2, pExtendVerticalBow1, pExtendVerticalBow2;
LPVOID oExtendVerticalCam1, oExtendVerticalCam2, oExtendVerticalBow1, oExtendVerticalBow2;
void __declspec(naked) HExtendVerticalCam1()
{
	asm("movss	%0,%%xmm0" : : "m"(plus));
	asm("jmp	*%0" : : "m"(oExtendVerticalCam1));
}

void __declspec(naked) HExtendVerticalCam2()
{
	asm("movss	%0,%%xmm0" : : "m"(minus));
	asm("jmp	*%0" : : "m"(oExtendVerticalCam2));
}

void __declspec(naked) HExtendVerticalBow1()
{
	asm("movss	%0,%%xmm0" : : "m"(plus));
	asm("jmp	*%0" : : "m"(oExtendVerticalBow1));
}

void __declspec(naked) HExtendVerticalBow2()
{
	asm("movss	%0,%%xmm0" : : "m"(minus));
	asm("jmp	*%0" : : "m"(oExtendVerticalBow2));
}

LPBYTE pAutoCamV, pAutoCamH, oAutoCamV, oAutoCamH;
void __declspec(naked) HAutoCamV()
{
	asm("cmp	$0,%esi");
	asm("jmp	*%0" : : "m"(oAutoCamV));
}

void __declspec(naked) HAutoCamH()
{
	asm("cmp	$0,%edx");
	asm("jmp	*%0" : : "m"(oAutoCamH));
}

LPVOID oWeather;
void __declspec(naked) HWeather()
{
	asm("cmpb	$0,0xB33A8(%ecx)");
	asm("je 	1f");
	asm("cmpw	$0xDC,0x34(%ecx)");
	asm("jmp	*%0" : : "m"(oWeather));

	asm("\n1:");
	asm("cmpw	$0xE6,0x34(%ecx)");
	asm("je 	2f");
	asm("cmp	%ecx,%ecx");
	asm("jmp	*%0" : : "m"(oWeather));

	asm("\n2:");
	asm("cmp	$0,%ecx");
	asm("jmp	*%0" : : "m"(oWeather));
}

bool jumpMod;
float jumpMods[6] = { 0.0f, 19.00f, 13.95f, -2.00f, 0.30f, 4.25f };
float jumpModsRun[6] = { 0.0f, 15.25f, 22.90f, -1.25f, 0.30f, 4.25f };
LPBYTE pJumpMod1, pJumpMod2, oJumpMod1, oJumpMod2;
void __declspec(naked) HJumpMod1()
{
	asm("mov	%ecx,%edx");
	asm("jmp	*%0" : : "m"(oJumpMod1));
}

void __declspec(naked) HJumpMod2()
{
	asm("mov	0x3DEC(%edi),%ecx");
	asm("mov	8(%ecx),%ecx");
	asm("test	%ecx,%ecx");
	asm("jnz	3f");

	asm("cmp	$9,%edx");
	asm("jl 	1f");
	asm("cmp	$18,%edx");
	asm("jl		2f");
	asm("jmp	3f");

	asm("\n1:");
	asm("lea	%0,%%esi" : : "m"(jumpMods));
	asm("jmp	3f");

	asm("\n2:");
	asm("lea	%0,%%esi" : : "m"(jumpModsRun));

	asm("\n3:");
	asm("jmp	*%0" : : "m"(oJumpMod2));
}

float fallHeight;
LPBYTE pFallHeight, oFallHeight;
void __declspec(naked) HFallHeight()
{
	asm("mov	0x3DEC(%esi),%ecx");
	asm("test	%ecx,%ecx");
	asm("jz 	1f");

	asm("mov	8(%ecx),%ecx");
	asm("cmp	$-1,%ecx");
	asm("je 	1f");

	asm("fstp	%st(0)");
	asm("flds	%0" : : "m"(fallHeight));

	asm("\n1:");
	asm("jmp	*%0" : : "m"(oFallHeight));
}

bool movementSpeedDebug = false;
void __stdcall HMovementSpeedDebug(LPVOID animId) { LOG("%08X\n", (UINT)animId); }

float movementSpeed;
LPBYTE pMovementSpeed, oMovementSpeed;
void __declspec(naked) HMovementSpeed()
{
	asm("mov	0x3DEC(%edi),%esi");
	asm("mov	8(%esi),%esi");
	asm("cmp	$0,%esi");
	asm("jl 	3f");

	asm("mov	0x2DD4(%edi),%esi");
	asm("cmp	$0,%esi");
	asm("jle	3f");

	asm("cmpb	$1,%0" : : "m"(movementSpeedDebug));
	asm("jne	1f");

	asm("push	%esi");
	asm("mov	0x3DEC(%edi),%esi");
	asm("mov	8(%esi),%esi");
	asm("cmp	$0,%esi");
	asm("pop	%esi");
	asm("jne	1f");

	asm("pusha");
	asm("sub	$0x80,%esp");
	asm("movdqu	%xmm0,(%esp)");
	asm("movdqu	%xmm1,0x10(%esp)");
	asm("movdqu	%xmm2,0x20(%esp)");
	asm("movdqu	%xmm3,0x30(%esp)");
	asm("movdqu	%xmm4,0x40(%esp)");
	asm("movdqu	%xmm5,0x50(%esp)");
	asm("movdqu	%xmm6,0x60(%esp)");
	asm("movdqu	%xmm7,0x70(%esp)");

	asm("push	%esi");
	asm("call	%0" : : "m"(HMovementSpeedDebug));

	asm("movdqu	0x70(%esp),%xmm7");
	asm("movdqu	0x60(%esp),%xmm6");
	asm("movdqu	0x50(%esp),%xmm5");
	asm("movdqu	0x40(%esp),%xmm4");
	asm("movdqu	0x30(%esp),%xmm3");
	asm("movdqu	0x20(%esp),%xmm2");
	asm("movdqu	0x10(%esp),%xmm1");
	asm("movdqu	(%esp),%xmm0");
	asm("add	$0x80,%esp");
	asm("popa");

	asm("\n1:");
	asm("cmp	$0x7D,%esi");//sprint
	asm("je 	2f");
	asm("cmp	$0x05,%esi");//sprint
	asm("je 	2f");
	asm("cmp	$0x02,%esi");//run
	asm("je 	2f");
	asm("cmp	$0x01,%esi");//walk
	asm("je 	2f");
	asm("jmp	3f");

	asm("\n2:");
	asm("mulss	%0,%%xmm0" : : "m"(movementSpeed));

	asm("\n3:");
	asm("jmp	*%0" : : "m"(oMovementSpeed));
}

float gatheringSpeed;
bool charCustomization, extendVerticalCam, disableAutoCam;
void renderMiscUI()
{
	if (ImGui::CollapsingHeader("Main"))
	{
		static bool borderlessFullscreen = config.getBool("main", "borderlessFullscreen", false);
		if (ImGui::Checkbox("Borderless fullscreen", &borderlessFullscreen))
			config.setBool("main", "borderlessFullscreen", borderlessFullscreen);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		if (ImGui::Checkbox("Char customization", &charCustomization))
		{
			config.setBool("main", "charCustomization", charCustomization);
			Hooks::SwitchHook("CharCustomization", pCharCustomization, charCustomization);
		}

		if (ImGui::Checkbox("Camera - extend vertical", &extendVerticalCam))
		{
			config.setBool("main", "extendVerticalCam", extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCam1", pExtendVerticalCam1, extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCam2", pExtendVerticalCam2, extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCamBow1", pExtendVerticalBow1, extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCamBow2", pExtendVerticalBow2, extendVerticalCam);
		}

		if (ImGui::Checkbox("Camera - disable autocorrection", &disableAutoCam))
		{
			config.setBool("main", "disableAutoCam", disableAutoCam);
			Hooks::SwitchHook("DisableAutoCamV", pAutoCamV, disableAutoCam);
			Hooks::SwitchHook("DisableAutoCamH", pAutoCamH, disableAutoCam);
		}

		pair<int, const char*> weather[]{ { 0, "Clear sky" }, { 1, "Cloudy" }, { 2, "Foggy" }, { 3, "Vulcanic" } };
		ImGui::RadioButtons(GetBasePtr(0xB8780), weather);
		ImGui::Checkbox("Weather - post game", GetBasePtr<bool>(0xB33A8));

		ImGui::PushItemWidth(150.0f);
		if (ImGui::InputFloatEx("Gathering/Mining speed", &gatheringSpeed, 0.1f, 1.0f, 100.0f, 1))
			config.setFloat("main", "gatheringSpeed", gatheringSpeed);

		bool prevState = fallHeight < 0;
		if (ImGui::InputFloatEx("Fall height", &fallHeight, 10.0f, -FLT_MAX, 0.0f))
		{
			config.setFloat("main", "fallHeight", fallHeight);
			if (prevState != (fallHeight < 0))
				Hooks::SwitchHook("FallHeight", pFallHeight, fallHeight < 0);
		}

		prevState = movementSpeed >= 0;
		if (ImGui::InputFloatEx("Movement speed", &movementSpeed, 0.1f, -1.0f))
		{
			config.setFloat("main", "movementSpeed", movementSpeed);
			if (prevState != (movementSpeed >= 0))
				Hooks::SwitchHook("MovementSpeed", pMovementSpeed, movementSpeed >= 0);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Checkbox("Log", &movementSpeedDebug);

		if (ImGui::TreeNode("Jump mod"))
		{
			if (ImGui::Checkbox("Enabled", &jumpMod))
			{
				config.setBool("main", "jumpMod", jumpMod);
				Hooks::SwitchHook("JumpMod", pJumpMod1, jumpMod);
				Hooks::SwitchHook("JumpMod", pJumpMod2, jumpMod);
			}

			ImGui::TextUnformatted("Walking Jump");
			ImGui::TextUnformatted("Running Jump", 190.0f);
			ImGui::PushItemWidth(150.0f);
			static const char *labels1[5] = { "##Height", "##Length", "##Gravity", "##Damping", "##MoveSpeed" };
			static const char *labels2[5] = { "Height", "Length", "Gravity", "Damping", "MoveSpeed" };
			for (int i = 0; i < 5; i++)
			{
				if (ImGui::InputFloatEx(labels1[i], jumpMods + 1 + i, 0.1f))
					config.setFloats("main", "jumpMods", vector<float>(jumpMods + 1, jumpMods + 6));
				ImGui::SameLine();
				if (ImGui::InputFloatEx(labels2[i], jumpModsRun + 1 + i, 0.1f))
					config.setFloats("main", "jumpModsRun", vector<float>(jumpModsRun + 1, jumpModsRun + 6));
			}
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
	}
}

}

void Hooks::Misc()
{
	BYTE sigChar[] = { 0x83, 0xBB, 0x84, 0x02, 0x00, 0x00, 0x0B };	//cmp	dword ptr [ebx+284h], 0Bh
	if (FindSignature("CharCustomization", sigChar, &pCharCustomization))
	{
		charCustomization = config.getBool("main", "charCustomization", false);
		CreateHook("CharCustomization", pCharCustomization, (LPVOID)HCharCustomization, &oCharCustomization, charCustomization);
	}

	BYTE sigCam[] = { 0xF3, 0x0F, 0x10, 0x80, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [eax+8Ch]
					0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
					0xF3, 0x0F, 0x10, 0x11 };							//movss	xmm2, dword ptr [ecx]

	BYTE sigBow[] = { 0xF3, 0x0F, 0x10, 0x81, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [ecx+8Ch]
					0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
					0xF3, 0x0F, 0x10, 0x17 };							//movss	xmm2, dword ptr [edi]

	if (FindSignature("ExtendVerticalCam", sigCam, &pExtendVerticalCam1) &&
		FindSignature("ExtendVerticalCamBow", sigBow, &pExtendVerticalBow1))
	{
		pExtendVerticalCam1 += 8;
		pExtendVerticalCam2 = pExtendVerticalCam1 + 0x21;
		pExtendVerticalBow1 += 8;
		pExtendVerticalBow2 = pExtendVerticalBow1 + 0x27;

		extendVerticalCam = config.getBool("main", "extendVerticalCam", false);
		CreateHook("ExtendVerticalCam1", pExtendVerticalCam1, (LPVOID)HExtendVerticalCam1, &oExtendVerticalCam1, extendVerticalCam);
		CreateHook("ExtendVerticalCam2", pExtendVerticalCam2, (LPVOID)HExtendVerticalCam2, &oExtendVerticalCam2, extendVerticalCam);
		CreateHook("ExtendVerticalCamBow1", pExtendVerticalBow1, (LPVOID)HExtendVerticalBow1, &oExtendVerticalBow1, extendVerticalCam);
		CreateHook("ExtendVerticalCamBow2", pExtendVerticalBow2, (LPVOID)HExtendVerticalBow2, &oExtendVerticalBow2, extendVerticalCam);
	}

	BYTE sigV[] = { 0x80, 0xBE, 0xF0, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [esi+2F0h], 0
					0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
	BYTE sigH[] = { 0x80, 0xBA, 0xF1, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [edx+2F1h], 0
					0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
	if (FindSignature("DisableAutoCamV", sigV, &pAutoCamV) &&
		FindSignature("DisableAutoCamH", sigH, &pAutoCamH))
	{
		disableAutoCam = config.getBool("main", "disableAutoCam", false);
		CreateHook("DisableAutoCamV", pAutoCamV, (LPVOID)HAutoCamV, nullptr, disableAutoCam);
		CreateHook("DisableAutoCamH", pAutoCamH, (LPVOID)HAutoCamH, nullptr, disableAutoCam);
		oAutoCamV = pAutoCamV + 7;
		oAutoCamH = pAutoCamH + 7;
	}

	BYTE *pOffset;
	BYTE sigWeather[] = { 0x80, 0xB9, 0xA8, 0x33, 0x0B, 0x00, 0x00, 0x74, 0x15 };
	if (FindSignature("Weather", sigWeather, &pOffset))
	{
		CreateHook("Weather", pOffset, (LPVOID)HWeather, nullptr);
		oWeather = pOffset + 7;
	}

	gatheringSpeed = config.getFloat("main", "gatheringSpeed", 1.0f);
	BYTE sigGathering[] = { 0xBA, 0x68, 0x00, 0x00, 0x00, 0x8B, 0xCE, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x84, 0xC0, 0x74 };
	if (FindSignature("Gathering", sigGathering, &pOffset))
	{
		pOffset += pOffset[sizeof sigGathering] + sizeof sigGathering + 5;
		Set<float*>((float**)pOffset, { &gatheringSpeed });
		for (int i = 0; i < 3; i++)
		{
			if (!Find("Gathering", pOffset, pOffset + 0x10000, sigGathering, &pOffset))
				break;
			pOffset += pOffset[sizeof sigGathering] + sizeof sigGathering + 5;
			Set<float*>((float**)pOffset, { &gatheringSpeed });
		}
	}

	fallHeight = config.getFloat("main", "fallHeight", -1000.0f);
	BYTE sigFall[] = { 0xD9, 0x5C, 0x24, 0x0C, 0x80, 0xBE, 0xD0, 0x1E, 0x00, 0x00, 0x00 };
	if (FindSignature("FallHeight", sigFall, &pFallHeight))
		CreateHook("FallHeight", pFallHeight, (LPVOID)HFallHeight, &oFallHeight, fallHeight < 0);

	movementSpeed = config.getFloat("main", "movementSpeed", -1.0f);
	BYTE sigMovement[] = { 0xF3, 0x0F, 0x11, 0x87, 0xE4, 0x0E, 0x00, 0x00, 0x5E, 0x0F, 0x28, 0xC1 };
	if (FindSignature("MovementSpeed", sigMovement, &pMovementSpeed))
		CreateHook("MovementSpeed", pMovementSpeed, (LPVOID)HMovementSpeed, &oMovementSpeed, movementSpeed >= 0);

	BYTE sigJump[] = { 0x8B, 0x40, 0x70, 0x56, 0x8B, 0x34, 0x88, 0x8B, 0x16, 0x8B, 0x42, 0x10 };
	if (FindSignature("JumpMod", sigJump, &pJumpMod1))
	{
		pJumpMod1 += sizeof sigJump;
		BYTE sigJump2[] = { 0xD9, 0x46, 0x04, 0xD9, 0x9F, 0xD8, 0x38, 0x00, 0x00 };
		if (Find("JumpMod", pJumpMod1, pJumpMod1 + 0x100, sigJump2, &pJumpMod2))
		{
			jumpMod = config.getBool("main", "jumpMod", false);
			auto mods = config.getFloats("main", "jumpMods");
			if (mods.size() == 5)
				copy(mods.begin(), mods.end(), jumpMods + 1);
			mods = config.getFloats("main", "jumpModsRun");
			if (mods.size() == 5)
				copy(mods.begin(), mods.end(), jumpModsRun + 1);
			CreateHook("JumpMod", pJumpMod1, (LPVOID)HJumpMod1, &oJumpMod1, jumpMod);
			CreateHook("JumpMod", pJumpMod2, (LPVOID)HJumpMod2, &oJumpMod2, jumpMod);
		}
	}

	InGameUIAdd(renderMiscUI);
}
