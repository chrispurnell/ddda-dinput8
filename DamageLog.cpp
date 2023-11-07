#include "stdafx.h"
#include "DamageLog.h"
#include "dinput8.h"
#include "Hotkeys.h"
#include "InGameUI.h"
#include "ImGui/imgui_internal.h"

namespace
{

CRITICAL_SECTION damageLogSync;
std::vector<UINT32> damageLogBuffer;
void __stdcall HDamageLogStore(UINT32 id)
{
	EnterCriticalSection(&damageLogSync);
	damageLogBuffer.push_back(id);
	LeaveCriticalSection(&damageLogSync);
}

LPBYTE pDamageLog, oDamageLog;
void __declspec(naked) HDamageLog()
{
	asm("pusha");
	asm("push	%edx");
	asm("call	%0" : : "m"(HDamageLogStore));
	asm("popa");
	asm("jmp	*%0" : : "m"(oDamageLog));
}

bool damageLog = false;
ImVec2 damageLogPosition, damageLogSize;
ImVec4 damageLogForeground, damageLogBackground;
ImGuiWindowFlags damageLogFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;

int unique[113] = {
 4175, 4176, 4177, 4178, 4179, 4180, 4181, 4182, 4183, 4207,
 4208, 4209, 4210, 4211, 4212, 4239, 4240, 4276, 4277, 4279,
 4280, 4308, 4309, 4310, 4311, 4340, 4341, 4342, 4343, 4344,
 4367, 4368, 4453, 4454, 4455, 4456, 4457, 4421, 4422, 4423,
 4424, 4490, 4491, 4492, 4522, 4523, 4524, 4525, 4526, 4554,
 4555, 4556, 4557, 4586, 4587, 4588, 4589, 4590, 4591, 4592,
 4593, 4594, 4618, 4619, 4620, 4621, 4623, 4682, 4683, 4714,
 4715, 4716, 4717, 4718, 4719, 4720, 4721, 4842, 4843, 4223,
 4884, 4885, 4886, 4887, 4901, 4902, 4903, 4943, 4944, 4946,
 4945, 4947, 4970, 4971, 4972, 4463, 4634, 4635, 4636, 4637,
 4638, 4639, 4694, 4695, 4696, 4756, 4757, 4758, 4759, 4997,
 4998, 4999, 5000
};

void renderDamageLog(bool getsInput)
{
	if (!damageLog)
		return;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, damageLogBackground);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
	ImGui::SetNextWindowPos(damageLogPosition, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(damageLogSize, ImGuiSetCond_Once);
	if (ImGui::Begin("DamageLog", nullptr, getsInput ? damageLogFlags : damageLogFlags | ImGuiWindowFlags_NoResize))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, damageLogForeground);
		ImGuiListClipper clipper(damageLogBuffer.size(), ImGui::GetTextLineHeightWithSpacing());

		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			UINT32 n = damageLogBuffer[i];
			INT32 id = -1;
			if (n < 113) id = unique[n];
			ImGui::Text("%u: %d", n, id);
		}

		clipper.End();
		ImGui::SetScrollHere();
		ImGui::PopStyleColor();
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void DamageLogSwitch()
{
	config.setBool("inGameUI", "damageLog", damageLog = !damageLog);
	Hooks::SwitchHook("DamageLog", pDamageLog, damageLog);
}

void renderDamageLogUI()
{
	if (ImGui::CollapsingHeader("Damage Log"))
	{
		if (ImGui::Checkbox("Enabled", &damageLog))
		{
			config.setBool("inGameUI", "damageLog", damageLog);
			Hooks::SwitchHook("DamageLog", pDamageLog, damageLog);
		}

		if (ImGui::ColorEdit4("Foreground", (float*)&damageLogForeground))
			config.setUInt("inGameUI", "damageLogForeground", ImGui::ColorConvertFloat4ToU32(damageLogForeground), true);

		if (ImGui::ColorEdit4("Background", (float*)&damageLogBackground))
			config.setUInt("inGameUI", "damageLogBackground", ImGui::ColorConvertFloat4ToU32(damageLogBackground), true);

		if (ImGui::Button("Save Position/Size"))
		{
			ImGuiWindow *window = nullptr;
			ImGuiState *state = (ImGuiState*)ImGui::GetInternalState();
			for (ImGuiWindow *w : state->Windows)
				if (strcmp(w->Name, "DamageLog") == 0)
					window = w;
			if (window)
			{
				config.setInts("inGameUI", "damageLogPosition", { (int)window->Pos.x, (int)window->Pos.y });
				config.setInts("inGameUI", "damageLogSize", { (int)window->Size.x, (int)window->Size.y });
			}
		}

		if (ImGui::Button("Clear"))
		{
			EnterCriticalSection(&damageLogSync);
			damageLogBuffer.clear();
			LeaveCriticalSection(&damageLogSync);
		}
	}
}

}

void Hooks::DamageLog()
{
	InitializeCriticalSection(&damageLogSync);
	HotkeysAdd("keyDamageLog", 'P', DamageLogSwitch);

	damageLog = config.getBool("inGameUI", "damageLog", false);
	ImU32 foreground = config.getUInt("inGameUI", "damageLogForeground", ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)));
	ImU32 background = config.getUInt("inGameUI", "damageLogBackground", ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 0.1f)));
	auto position = config.getInts("inGameUI", "damageLogPosition");
	auto size = config.getInts("inGameUI", "damageLogSize");
	damageLogForeground = ImGui::ColorConvertU32ToFloat4(foreground);
	damageLogBackground = ImGui::ColorConvertU32ToFloat4(background);
	damageLogPosition = position.size() == 2 ? ImVec2((float)position[0], (float)position[1]) : ImVec2(500, 0);
	damageLogSize = size.size() == 2 ? ImVec2((float)size[0], (float)size[1]) : ImVec2(200, 400);

	BYTE sig[] = { 0x80, 0x7f, 0x2c, 0x00, 0x0f, 0x85, 0x16, 0x01, 0x00, 0x00, 0x83, 0xfa, 0x74 };
	if (FindSignature("DamageLog", sig, &pDamageLog))
	{
		CreateHook("DamageLog", pDamageLog, (LPVOID)HDamageLog, &oDamageLog, damageLog);
	}

	InGameUIAdd(renderDamageLogUI);
	InGameUIAddWindow(renderDamageLog);
}
