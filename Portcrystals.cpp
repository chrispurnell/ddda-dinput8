#include "stdafx.h"
#include "Portcrystals.h"
#include "dinput8.h"
#include "InGameUI.h"

namespace
{

LPVOID oMapClosest, oMapCursor;
float pMapClosest[3] = { 0 }, pMapCursor[3] = { 0 };
void __declspec(naked) HMapClosest()
{
	asm("mov	(%eax),%edx");
	asm("mov	%%edx,%0" : : "m"(pMapClosest));
	asm("mov	4(%eax),%edx");
	asm("mov	%%edx,%0+4" : : "m"(pMapClosest));
	asm("mov	8(%eax),%edx");
	asm("mov	%%edx,%0+8" : : "m"(pMapClosest));
	asm("jmp	*%0" : : "m"(oMapClosest));
}

void __declspec(naked) HMapCursor()
{
	asm("mov	0x8D0(%esi),%edx");
	asm("mov	%%edx,%0" : : "m"(pMapCursor));
	asm("mov	0x8D4(%esi),%edx");
	asm("mov	%%edx,%0+4" : : "m"(pMapCursor));
	asm("mov	0x8D8(%esi),%edx");
	asm("mov	%%edx,%0+8" : : "m"(pMapCursor));
	asm("jmp	*%0" : : "m"(oMapCursor));
}

void renderPortcrystalsPos(const char *label, float *pPosition)
{
	float v[3] = { pPosition[0], pPosition[2], pPosition[1] };
	ImGui::InputFloat3(label, v, 0, ImGuiInputTextFlags_ReadOnly);
}

void renderPortcrystalsUI()
{
	if (ImGui::CollapsingHeader("Portcrystals"))
	{
		renderPortcrystalsPos("Cursor", pMapCursor);
		renderPortcrystalsPos("Closest", pMapClosest);
		ImGui::Separator();

		for (int i = 0; i < 10; i++)
		{
			ImGui::PushID(i);
			float *pPosition = GetBasePtr<float>(0xBE3A0 + 16 * i);
			renderPortcrystalsPos("##p", pPosition);
			ImGui::SameLine();
			if (ImGui::Button("Cursor"))
			{
				pPosition[0] = pMapCursor[0];
				pPosition[1] = pMapCursor[1];
				pPosition[2] = pMapCursor[2];
			}
			ImGui::SameLine();
			if (ImGui::Button("Closest"))
			{
				pPosition[0] = pMapClosest[0];
				pPosition[1] = pMapClosest[1];
				pPosition[2] = pMapClosest[2];
			}
			ImGui::PopID();
		}
	}
}

}

void Hooks::Portcrystals()
{
	BYTE *pOffset;
	BYTE sig1[] = { 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x86, 0x94, 0x02, 0x00, 0x00, 0x83, 0xF8, 0x41, 0x7C };
	if (FindSignature("Portcrystals", sig1, &pOffset))
		CreateHook("Portcrystals", pOffset + 5, (LPVOID)HMapClosest, &oMapClosest);

	BYTE sig2[] = { 0xD9, 0x86, 0xD0, 0x08, 0x00, 0x00, 0x57 };
	if (FindSignature("Portcrystals", sig2, &pOffset))
		CreateHook("Portcrystals", pOffset, (LPVOID)HMapCursor, &oMapCursor);

	InGameUIAdd(renderPortcrystalsUI);
}
