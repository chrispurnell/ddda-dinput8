// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "stdafx.h"
#include "Cheats.h"

bool thirdSkillLevels[0x200][4] = {};
void thirdSkillLevelsInit(int partyId, std::vector<int> list)
{
	if (list.empty())
		return;
	if (find(list.begin(), list.end(), -1) != list.end())
		for (size_t i = 0; i < Hooks::ListItemEnchant.size() - 1; i++)
			thirdSkillLevels[Hooks::ListItemEnchant[i].first & 0x1FF][partyId] = true;
	else
		for (size_t i = 0; i < list.size(); i++)
			thirdSkillLevels[list[i] & 0x1FF][partyId] = true;
}

int __stdcall GetSkillTier(UINT16 skill, DWORD address)
{
	address -= (DWORD)GetBasePtr(0);
	if (address == 0xA7DFC)//player
		return thirdSkillLevels[skill & 0x1FF][0];
	if (address == 0xA85EC)//main pawn
		return thirdSkillLevels[skill & 0x1FF][1];
	if (address == 0xA9C4C)//pawn 1
		return thirdSkillLevels[skill & 0x1FF][2];
	if (address == 0xAB2AC)//pawn 2
		return thirdSkillLevels[skill & 0x1FF][3];
	return 0;
}

bool skillLevel;
LPBYTE pSkillLevel, oSkillLevel;
void __declspec(naked) HSkillLevel()
{
	asm("pusha");
	asm("push	%edi");
	asm("push	%esi");
	asm("call	%0" : : "m"(GetSkillTier));
	asm("cmp	$0,%eax");
	asm("popa");

	asm("jne	1f");
	asm("test	%eax,0x74(%edi,%ecx,4)");

	asm("\n1:");
	asm("setnz	%dl");
	asm("jmp	*%0" : : "m"(oSkillLevel));
}

int runType;
LPBYTE pRunType, oRunType;
void __declspec(naked) HRunType()
{
	asm("cmpl	$0,%0" : : "m"(runType));
	asm("je 	1f");
	asm("cmpl	$2,%0" : : "m"(runType));
	asm("je 	2f");
	asm("mov	$0x20,%eax");
	asm("jmp	*%0" : : "m"(oRunType));

	asm("\n1:");
	asm("and	$0x20,%eax");
	asm("jmp	*%0" : : "m"(oRunType));

	asm("\n2:");
	asm("or 	$0x20,%eax");
	asm("jmp	*%0" : : "m"(oRunType));
}

float mWeight;
LPBYTE pWeight, oWeight;
void __declspec(naked) HWeight()
{
	asm("movss	%0,%%xmm0" : : "m"(mWeight));
	asm("mulss	%xmm0,%xmm5");
	asm("jmp	*%0" : : "m"(oWeight));
}

bool realTime;
float mTimeInterval;
UINT rTimeInterval;
LPBYTE pTimeInterval, oTimeInterval;
void __declspec(naked) HTimeInterval()
{
	asm("cmpl	$0,8(%esp)");
	asm("je 	2f");
	asm("cmpb	$0,%0" : : "m"(realTime));
	asm("je 	1f");
	asm("mov	$0x11,%eax");
	asm("mov	%eax,8(%esp)");
	asm("jmp	2f");

	asm("\n1:");
	asm("fildl	8(%esp)");
	asm("fmuls	%0" : : "m"(mTimeInterval));
	asm("fistpl	%0" : : "m"(rTimeInterval));
	asm("mov	%0,%%eax" : : "m"(rTimeInterval));
	asm("mov	%eax,8(%esp)");

	asm("\n2:");
	asm("jmp	*%0" : : "m"(oTimeInterval));
}

BYTE *pAffinityLast;
LPVOID oAffinity;
enum AffinityMod { Disabled = -1, NoNegative, AllPositive, NoChange, InstantFriend = 850, InstantMax = 900 } iAffinityMod;
void __declspec(naked) HAffinity()
{
	asm("mov	%%esi,%0" : : "m"(pAffinityLast));
	asm("cmpl	%0,%1" : : "i"(Disabled), "m"(iAffinityMod));
	asm("jle	3f");
	asm("cmpl	%0,%1" : : "i"(AllPositive), "m"(iAffinityMod));
	asm("jle	1f");
	asm("cmpl	%0,%1" : : "i"(NoChange), "m"(iAffinityMod));
	asm("je 	2f");
	asm("movzwl	0x8B8(%esi),%eax");
	asm("cmp	%0,%%eax" : : "m"(iAffinityMod));
	asm("jae	1f");
	asm("sub	%0,%%eax" : : "m"(iAffinityMod));
	asm("neg	%eax");
	asm("mov	%eax,%ebp");
	asm("jmp	3f");

	asm("\n1:");
	asm("test	%ebp,%ebp");
	asm("jns	3f");
	asm("cmpl	%0,%1" : : "i"(NoNegative), "m"(iAffinityMod));
	asm("je 	2f");
	asm("neg	%ebp");
	asm("jmp	3f");

	asm("\n2:");
	asm("xor	%ebp,%ebp");

	asm("\n3:");
	asm("jmp	*%0" : : "m"(oAffinity));
}

bool augmentsActive[0x80][4] = {};
void __declspec(naked) HIsAugmentEquipped()
{
	asm("push	%ecx");
	asm("push	%edx");
	asm("mov	12(%esp),%ecx");
	asm("mov	16(%esp),%edx");

	asm("xor	%eax,%eax");
	asm("add	$0x258,%ecx");

	asm("\n1:");
	asm("cmp	%edx,(%ecx)");
	asm("je 	2f");
	asm("inc	%eax");
	asm("add	$4,%ecx");
	asm("cmp	$6,%eax");
	asm("jb 	1b");

	asm("xor	%al,%al");
	asm("mov	12(%esp),%ecx");
	asm("mov	8(%ecx),%ecx");
	asm("cmp	$-1,%ecx");
	asm("je 	3f");

	asm("lea	%0(%%ecx,%%edx,4),%%ecx" : : "m"(augmentsActive));
	asm("mov	(%ecx),%al");
	asm("jmp	3f");

	asm("\n2:");
	asm("mov	$1,%al");

	asm("\n3:");
	asm("pop	%edx");
	asm("pop	%ecx");
	asm("ret	$8");
}

struct augmentInfo
{
	LPVOID Zero;
	float Activated;
	float Deactivated;
	UINT32 Unknown1;
	UINT32 Unknown2;

	std::vector<float> getList() const { return{ Activated, Deactivated, static_cast<float>(Unknown1), static_cast<float>(Unknown2) }; }
};

bool augmentMods;
bool augmentModsEnabled[0x80] = {};
augmentInfo augmentModsValues[0x80] = {};
LPBYTE pAugmentMods1, oAugmentMods1Orig, oAugmentMods1Mod;
void __declspec(naked) HAugmentMods1()
{
	asm("push	%edx");
	asm("push	%ecx");
	asm("call	%0" : : "m"(HIsAugmentEquipped));
	asm("mov	8(%ecx),%ecx");
	asm("cmp	$-1,%ecx");
	asm("je 	1f");

	asm("mov	%0(%%edx),%%cl" : : "m"(augmentModsEnabled));
	asm("test	%cl,%cl");
	asm("jz 	1f");

	asm("imul	$20,%edx,%ecx");
	asm("lea	%0(%%ecx),%%ecx" : : "m"(augmentModsValues));
	asm("jmp	*%0" : : "m"(oAugmentMods1Mod));

	asm("\n1:");
	asm("jmp	*%0" : : "m"(oAugmentMods1Orig));
}

LPBYTE pAugmentMods2, oAugmentMods2;
void __declspec(naked) HAugmentMods2()
{
	asm("push	%ebx");
	asm("push	%edx");
	asm("call	%0" : : "m"(HIsAugmentEquipped));
	asm("test	%al,%al");
	asm("jnz	1f");
	asm("pop	%edi");
	asm("pop	%esi");
	asm("pop	%ebx");
	asm("ret	$8");

	asm("\n1:");
	asm("mov	8(%edx),%edx");
	asm("cmp	$-1,%edx");
	asm("je 	2f");

	asm("mov	%0(%%ebx),%%dl" : : "m"(augmentModsEnabled));
	asm("test	%dl,%dl");
	asm("jz 	2f");

	asm("imul	$20,%ebx,%esi");
	asm("lea	%0(%%esi),%%esi" : : "m"(augmentModsValues));

	asm("\n2:");
	asm("xor	%al,%al");
	asm("jmp	*%0" : : "m"(oAugmentMods2));
}

LPBYTE pAugmentMods3;
void __declspec(naked) HAugmentMods3()
{
	asm("push	%edx");
	asm("push	%ecx");
	asm("call	%0" : : "m"(HIsAugmentEquipped));
	asm("ret");
}

struct buffInfo
{
	BOOL Enabled;
	UINT32 Category;
	float Timer;
	float Param0;
	float Param1;

	std::vector<float> getList() const { return{ Timer, Param0, Param1 }; }
};

bool buffModsLog = false;
void __stdcall HBuffmodsDebug(int buffId) { LOG("%d\n", buffId); }

bool buffMods;
buffInfo buffModsValues[0x30] = {};
LPBYTE pBuffMods, oBuffMods;
void __declspec(naked) HBuffMods()
{
	asm("test	%eax,%eax");
	asm("jz 	2f");
	asm("mov	0x3DEC(%eax),%ebx");
	asm("test	%ebx,%ebx");
	asm("jz 	2f");
	asm("mov	8(%ebx),%ebx");
	asm("cmp	$0,%ebx");
	asm("jl 	2f");

	asm("cmpb	$0,%0" : : "m"(buffModsLog));
	asm("je 	1f");

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
	asm("call	%0" : : "m"(HBuffmodsDebug));

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
	asm("cmp	$0x30,%esi");
	asm("jae	2f");

	asm("imul	$20,%esi,%ebx");
	asm("lea	%0(%%ebx),%%ebx" : : "m"(buffModsValues));
	asm("cmpb	$1,(%ebx)");
	asm("jne	2f");

	asm("movss	0x08(%ebx),%xmm0");
	asm("movss	0x0C(%ebx),%xmm1");
	asm("movss	0x10(%ebx),%xmm2");

	asm("\n2:");
	asm("jmp	*%0" : : "m"(oBuffMods));
}

void renderCheatsSkillLevel(const char *label, float position, bool *check, int partyId, bool isHeader = false)
{
	ImGui::SameLine(position);
	if (isHeader)
		ImGui::Text(label);
	ImGui::SameLine(position + 50.0f);
	if (ImGui::Checkbox((string("##") + label).c_str(), check))
	{
		std::vector<int> list;
		for (size_t i = 0; i < Hooks::ListItemEnchant.size() - 1; i++)
		{
			int skillId = Hooks::ListItemEnchant[i].first;
			if (isHeader)
				thirdSkillLevels[skillId][partyId] = *check;
			if (thirdSkillLevels[skillId][partyId])
				list.push_back(skillId);
		}
		config.setInts("cheats", (string("thirdSkillLevel") + label).c_str(), list);
	}
}

void renderCheatsAugment(const char *label, float position, int partyId, int skillId)
{
	ImGui::SameLine(position);
	if (ImGui::Checkbox((string("##") + label).c_str(), augmentsActive[skillId] + partyId))
	{
		std::vector<int> list;
		for (size_t i = 1; i < Hooks::ListSkillsAugments.size(); i++)
			if (augmentsActive[Hooks::ListSkillsAugments[i].first][partyId])
				list.push_back(Hooks::ListSkillsAugments[i].first);
		config.setInts("augments", (string("augments") + label).c_str(), list);
	}
}

bool shareWeaponSkills, ignoreEquipVocation, ignoreSkillVocation;
std::vector<std::pair<int, LPCSTR>> runTypeMapEV = { { -1, "Disabled" },{ 0, "Town Animation" },{ 1, "Town Animation + Stamina" },{ 2, "Stamina" } };
void renderCheatsUI()
{
	static bool setSkillsOpened = false, setAugmentsOpened = false, setAugmentModsOpened = false, setBuffModsOpened = false;
	if (setSkillsOpened)
	{
		ImGui::PushID("Set 3rd level skills");
		ImGui::Begin("Set 3rd level skills", &setSkillsOpened, ImVec2(525, 400));
		static bool selectAll1 = false, selectAll2 = false, selectAll3 = false, selectAll4 = false;
		renderCheatsSkillLevel("Player", 200.0f + 75.0f * 0, &selectAll1, 0, true);
		renderCheatsSkillLevel("Pawn", 200.0f + 75.0f * 1, &selectAll2, 1, true);
		renderCheatsSkillLevel("Pawn1", 200.0f + 75.0f * 2, &selectAll3, 2, true);
		renderCheatsSkillLevel("Pawn2", 200.0f + 75.0f * 3, &selectAll4, 3, true);

		for (size_t i = 0; i < Hooks::ListItemEnchant.size() - 1; i++)
		{
			ImGui::PushID(i);
			ImGui::Text(Hooks::ListItemEnchant[i].second);
			int skillId = Hooks::ListItemEnchant[i].first;
			renderCheatsSkillLevel("Player", 200.0f + 75.0f * 0, thirdSkillLevels[skillId] + 0, 0);
			renderCheatsSkillLevel("Pawn", 200.0f + 75.0f * 1, thirdSkillLevels[skillId] + 1, 1);
			renderCheatsSkillLevel("Pawn1", 200.0f + 75.0f * 2, thirdSkillLevels[skillId] + 2, 2);
			renderCheatsSkillLevel("Pawn2", 200.0f + 75.0f * 3, thirdSkillLevels[skillId] + 3, 3);
			ImGui::PopID();
		}
		ImGui::End();
		ImGui::PopID();
	}

	if (setAugmentsOpened)
	{
		ImGui::PushID("Active augments");
		ImGui::Begin("Active augments", &setAugmentsOpened, ImVec2(375, 400));
		ImGui::TextUnformatted("Player", 150.0f + 50.0f * 0);
		ImGui::TextUnformatted("Pawn", 150.0f + 50.0f * 1);
		ImGui::TextUnformatted("Pawn1", 150.0f + 50.0f * 2);
		ImGui::TextUnformatted("Pawn2", 150.0f + 50.0f * 3);

		for (size_t i = 1; i < Hooks::ListSkillsAugments.size(); i++)
		{
			int skillId = Hooks::ListSkillsAugments[i].first;
			if (skillId <= 100 && skillId % 10 == 0)
				ImGui::Separator();
			ImGui::PushID(i);
			ImGui::TextUnformatted(Hooks::ListSkillsAugments[i].second);
			renderCheatsAugment("Player", 150.0f + 50.0f * 0, 0, skillId);
			renderCheatsAugment("Pawn", 150.0f + 50.0f * 1, 1, skillId);
			renderCheatsAugment("Pawn1", 150.0f + 50.0f * 2, 2, skillId);
			renderCheatsAugment("Pawn2", 150.0f + 50.0f * 3, 3, skillId);
			ImGui::PopID();
		}
		ImGui::End();
		ImGui::PopID();
	}

	if (setAugmentModsOpened)
	{
		ImGui::PushID("Augment mods");
		ImGui::Begin("Augment mods", &setAugmentModsOpened, ImVec2(625, 400));

		static int addMod = 0;
		ImGui::ComboEnum<int>("##add", &addMod, Hooks::ListSkillsAugments);
		ImGui::SameLine();
		if (ImGui::Button("Add") && addMod >= 0)
		{
			augmentInfo *augmentOriginal = GetWorldPtr<augmentInfo>({ 0x8C8, 0x70, addMod * 4 });
			if (augmentOriginal)
			{
				augmentModsEnabled[addMod] = true;
				augmentModsValues[addMod] = *augmentOriginal;
				config.setFloats("augments", std::to_string(addMod).c_str(), augmentModsValues[addMod].getList());
			}
		}

		ImGui::Separator();
		ImGui::TextUnformatted("Activated", 150.0f + 105.0f * 0);
		ImGui::TextUnformatted("Deactivated", 150.0f + 105.0f * 1);
		ImGui::TextUnformatted("Unknown1", 150.0f + 105.0f * 2);
		ImGui::TextUnformatted("Unknown2", 150.0f + 105.0f * 3);

		for (size_t i = 1; i < Hooks::ListSkillsAugments.size(); i++)
		{
			int skillId = Hooks::ListSkillsAugments[i].first;
			if (!augmentModsEnabled[skillId])
				continue;
			ImGui::PushID(i);
			ImGui::TextUnformatted(Hooks::ListSkillsAugments[i].second);

			bool changed = false;
			ImGui::PushItemWidth(100.0f);
			ImGui::SameLine(150.0f + 105.0f * 0);
			changed |= ImGui::InputFloatEx("##val1", &augmentModsValues[skillId].Activated, 0.1f, 0.0f);
			ImGui::SameLine(150.0f + 105.0f * 1);
			changed |= ImGui::InputFloatEx("##val2", &augmentModsValues[skillId].Deactivated, 0.1f, 0.0f);
			ImGui::SameLine(150.0f + 105.0f * 2);
			changed |= ImGui::InputScalar<UINT32>("##val3", &augmentModsValues[skillId].Unknown1, 0);
			ImGui::SameLine(150.0f + 105.0f * 3);
			changed |= ImGui::InputScalar<UINT32>("##val4", &augmentModsValues[skillId].Unknown2, 0);
			ImGui::PopItemWidth();

			ImGui::SameLine(150.0f + 105.0f * 4);
			if (ImGui::Button("Remove"))
			{
				config.removeKey("augments", std::to_string(skillId).c_str());
				augmentModsEnabled[skillId] = false;
			}

			if (changed)
				config.setFloats("augments", std::to_string(skillId).c_str(), augmentModsValues[skillId].getList());
			ImGui::PopID();
		}
		ImGui::End();
		ImGui::PopID();
	}

	if (setBuffModsOpened)
	{
		ImGui::PushID("Buff mods");
		ImGui::Begin("Buff mods", &setBuffModsOpened, ImVec2(600, 400));

		static int addMod = 0;
		ImGui::ComboEnum<int>("##add", &addMod, Hooks::ListStatus);
		ImGui::SameLine();
		if (ImGui::Button("Add"))
		{
			buffInfo *buffOriginal = GetWorldPtr<buffInfo>({ 0x99C, 0x2710, 0x70, addMod * 4 });
			if (buffOriginal)
			{
				buffModsValues[addMod].Enabled = TRUE;
				buffModsValues[addMod].Timer = buffOriginal->Timer;
				buffModsValues[addMod].Param0 = buffOriginal->Param0;
				buffModsValues[addMod].Param1 = buffOriginal->Param1;
				config.setFloats("buffs", std::to_string(addMod).c_str(), buffModsValues[addMod].getList());
			}
		}

		ImGui::Separator();
		ImGui::TextUnformatted("Timer", 150.0f + 105.0f * 0);
		ImGui::TextUnformatted("Param0", 150.0f + 105.0f * 1);
		ImGui::TextUnformatted("Param1", 150.0f + 105.0f * 2);

		for (size_t i = 0; i < Hooks::ListStatus.size(); i++)
		{
			int buffId = Hooks::ListStatus[i].first;
			if (!buffModsValues[buffId].Enabled)
				continue;
			ImGui::PushID(i);
			ImGui::TextUnformatted(Hooks::ListStatus[i].second);

			bool changed = false;
			ImGui::PushItemWidth(100.0f);
			ImGui::SameLine(150.0f + 105.0f * 0);
			changed |= ImGui::InputFloatEx("##val1", &buffModsValues[buffId].Timer, 1.0f, 0.0f);
			ImGui::SameLine(150.0f + 105.0f * 1);
			changed |= ImGui::InputFloatEx("##val2", &buffModsValues[buffId].Param0, 1.0f, 0.0f);
			ImGui::SameLine(150.0f + 105.0f * 2);
			changed |= ImGui::InputFloatEx("##val3", &buffModsValues[buffId].Param1, 1.0f, 0.0f);
			ImGui::PopItemWidth();

			ImGui::SameLine(150.0f + 105.0f * 3);
			if (ImGui::Button("Remove"))
			{
				config.removeKey("buffs", std::to_string(buffId).c_str());
				buffModsValues[buffId].Enabled = FALSE;
			}

			if (changed)
				config.setFloats("buffs", std::to_string(buffId).c_str(), buffModsValues[buffId].getList());
			ImGui::PopID();
		}

		ImGui::End();
		ImGui::PopID();
	}

	if (ImGui::CollapsingHeader("Cheats"))
	{
		if (ImGui::Checkbox("Share weapon skills", &shareWeaponSkills))
			config.setBool("cheats", "shareWeaponSkills", shareWeaponSkills);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		if (ImGui::Checkbox("Ignore equip vocation", &ignoreEquipVocation))
			config.setBool("cheats", "ignoreEquipVocation", ignoreEquipVocation);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		if (ImGui::Checkbox("Ignore skill vocation", &ignoreSkillVocation))
			config.setBool("cheats", "ignoreSkillVocation", ignoreSkillVocation);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		bool prevState = runType >= 0;
		if (ImGui::ComboEnum<int>("Outside run type", &runType, runTypeMapEV))
		{
			config.setInt("cheats", "runType", runType);
			if (prevState != runType >= 0)
				Hooks::SwitchHook("Cheat (runType)", pRunType, runType >= 0);
		}

		prevState = mWeight >= 0;
		if (ImGui::InputFloatEx("Weight multiplicator", &mWeight, 0.01f, -1.0f, 1.0f))
		{
			config.setFloat("cheats", "weightMultiplicator", mWeight);
			if (prevState != mWeight >= 0)
				Hooks::SwitchHook("Cheat (weight)", pWeight, mWeight >= 0);
		}

		prevState = mTimeInterval >= 0;
		if (ImGui::InputFloatEx("Time speed", &mTimeInterval, 0.1f, -1.0f, 60.0f))
		{
			config.setFloat("cheats", "timeInterval", mTimeInterval);
			realTime = mTimeInterval == 0;
			if (prevState != mTimeInterval >= 0)
				Hooks::SwitchHook("Cheat (timeInterval)", pTimeInterval, mTimeInterval >= 0);
		}

		if (ImGui::Checkbox("3rd level skills", &skillLevel))
		{
			config.setBool("cheats", "thirdSkillLevel", skillLevel);
			Hooks::SwitchHook("Cheat (thirdSkillLevel)", pSkillLevel, skillLevel);
		}
		ImGui::SameLine();
		if (ImGui::Button("Set##setSkillsOpened"))
			setSkillsOpened = true;

		if (ImGui::Checkbox("Augment mods", &augmentMods))
		{
			config.setBool("augments", "enabled", augmentMods);
			Hooks::SwitchHook("Cheat (augmentMods)", pAugmentMods1, augmentMods);
			Hooks::SwitchHook("Cheat (augmentMods)", pAugmentMods2, augmentMods);
			Hooks::SwitchHook("Cheat (augmentMods)", pAugmentMods3, augmentMods);
		}
		ImGui::SameLine();
		if (ImGui::Button("Active##setAugmentsOpened"))
			setAugmentsOpened = true;
		ImGui::SameLine();
		if (ImGui::Button("Mods##setAugmentModsOpened"))
			setAugmentModsOpened = true;

		if (ImGui::Checkbox("Buff mods", &buffMods))
		{
			config.setBool("buffs", "enabled", buffMods);
			Hooks::SwitchHook("Cheat (buffMods)", pBuffMods, buffMods);
		}
		ImGui::SameLine();
		if (ImGui::Button("Set##setBuffModsOpened"))
			setBuffModsOpened = true;
		ImGui::SameLine();
		ImGui::Checkbox("Log", &buffModsLog);

		ImGui::Separator();
		if (ImGui::TreeNode("Affinity mod"))
		{
			std::vector<std::pair<int, LPCSTR>> affinityModEV =
			{
				{ Disabled, "Disabled" },{ NoNegative, "No negative changes" },{ AllPositive, "All changes are positive" },
				{ NoChange, "No changes at all" },{ InstantFriend, "Instant friend (850)" },{ InstantMax, "Instant max (900)" }
			};

			if (ImGui::ComboEnum<int>("Mode", &iAffinityMod, affinityModEV))
				config.setInt("cheats", "affinityMod", iAffinityMod);

			if (pAffinityLast)
			{
				ImGui::InputScalar<UINT16>("Last", pAffinityLast + 0x8B8, 0, 1000);
				ImGui::InputScalar<UINT16>("Attitude", pAffinityLast + 0x8BA, 0, UINT16_MAX);
			}
			ImGui::TreePop();
		}
	}
}

void Hooks::Cheats()
{
	BYTE sigRun[] = { 0x8B, 0x42, 0x40,			//mov	eax, [edx+40h]
					0x53,						//push	ebx
					0x8B, 0x5C, 0x24, 0x08 };	//mov	ebx, [esp+4+arg_0]
	if (FindSignature("Cheat (runType)", sigRun, &pRunType))
	{
		runType = config.getInt("cheats", "runType", false);
		if (runType > 2 || runType < -1)
			runType = -1;
		CreateHook("Cheat (runType)", pRunType += 3, (LPVOID)HRunType, &oRunType, runType >= 0);
	}

	BYTE sigWeight[] = { 0xF3, 0x0F, 0x58, 0xAB, 0x4C, 0x02, 0x00, 0x00,	//addss		xmm5, dword ptr [ebx+24Ch]
						0x45 };												//inc		ebp
	if (FindSignature("Cheat (weight)", sigWeight, &pWeight))
	{
		mWeight = config.getFloat("cheats", "weightMultiplicator", -1);
		CreateHook("Cheat (weight)", pWeight, (LPVOID)HWeight, &oWeight, mWeight >= 0);
	}

	BYTE sigTime[] = { 0x8B, 0x44, 0x24, 0x08, 0x01, 0x86, 0x68, 0x87, 0x0B, 0x00 };
	if (FindSignature("Cheat (timeInterval)", sigTime, &pTimeInterval))
	{
		mTimeInterval = config.getFloat("cheats", "timeInterval", -1);
		realTime = mTimeInterval == 0;
		CreateHook("Cheat (timeInterval)", pTimeInterval, (LPVOID)HTimeInterval, &oTimeInterval, mTimeInterval >= 0);
	}

	BYTE sigSkill[] = { 0x85, 0x44, 0x8F, 0x74, 0x0F, 0x95, 0xC2 };
	if (FindSignature("Cheat (thirdSkillLevel)", sigSkill, &pSkillLevel))
	{
		skillLevel = config.getBool("cheats", "thirdSkillLevel", false);
		CreateHook("Cheat (thirdSkillLevel)", pSkillLevel, (LPVOID)HSkillLevel, &oSkillLevel, skillLevel);
		oSkillLevel += 7;
		thirdSkillLevelsInit(0, config.getInts("cheats", "thirdSkillLevelPlayer"));
		thirdSkillLevelsInit(1, config.getInts("cheats", "thirdSkillLevelPawn"));
		thirdSkillLevelsInit(2, config.getInts("cheats", "thirdSkillLevelPawn1"));
		thirdSkillLevelsInit(3, config.getInts("cheats", "thirdSkillLevelPawn2"));
	}

	BYTE *pOffset;
	BYTE sigAffinity[] = { 0x0F, 0xB7, 0x86, 0xB8, 0x08, 0x00, 0x00, 0x8B, 0xD8, 0x03, 0xC5 };
	if (FindSignature("Cheat (affinity)", sigAffinity, &pOffset))
	{
		iAffinityMod = (AffinityMod)config.getInt("cheats", "affinityMod", Disabled);
		CreateHook("Cheat (affinity)", pOffset, (LPVOID)HAffinity, &oAffinity);
	}

	BYTE sigAug1[] = { 0x33, 0xC0, 0x81, 0xC1, 0x58, 0x02, 0x00, 0x00,  0x39, 0x11, 0x74, 0xCC, 0x40, 0x83, 0xC1, 0x04, 0x83, 0xF8, 0x06, 0x72, 0xCC, 0x32, 0xC0, 0x8B, 0x0D };
	BYTE sigAug2[] = { 0x32, 0xC0, 0x33, 0xC9, 0x81, 0xC2, 0x58, 0x02, 0x00, 0x00, 0x39, 0x1A, 0x74 };
	BYTE sigAug3[] = { 0x8B, 0x89, 0xEC, 0x3D, 0x00, 0x00, 0x85, 0xC9, 0x74, 0xCC, 0x33, 0xC0 };
	if (FindSignature("Cheat (augmentMods)", sigAug1, &pAugmentMods1) &&
		FindSignature("Cheat (augmentMods)", sigAug2, &pAugmentMods2) &&
		FindSignature("Cheat (augmentMods)", sigAug3, &pAugmentMods3))
	{
		augmentMods = config.getBool("augments", "enabled", false);
		for (auto i : config.getInts("augments", "augmentsPlayer"))
			augmentsActive[i & 0x7F][0] = true;
		for (auto i : config.getInts("augments", "augmentsPawn"))
			augmentsActive[i & 0x7F][1] = true;
		for (auto i : config.getInts("augments", "augmentsPawn1"))
			augmentsActive[i & 0x7F][2] = true;
		for (auto i : config.getInts("augments", "augmentsPawn2"))
			augmentsActive[i & 0x7F][3] = true;
		for (auto augmentId : config.getSectionInts("augments"))
			for (size_t i = 1; i < ListSkillsAugments.size(); i++)
				if (ListSkillsAugments[i].first == augmentId)
				{
					auto values = config.getFloats("augments", std::to_string(augmentId).c_str());
					if (values.size() == 4)
					{
						augmentModsValues[augmentId].Activated = values[0];
						augmentModsValues[augmentId].Deactivated = values[1];
						augmentModsValues[augmentId].Unknown1 = static_cast<UINT32>(values[2]);
						augmentModsValues[augmentId].Unknown2 = static_cast<UINT32>(values[3]);
						augmentModsEnabled[augmentId] = true;
						break;
					}
				}

		CreateHook("Cheat (augmentMods)", pAugmentMods1, (LPVOID)HAugmentMods1, nullptr, augmentMods);
		oAugmentMods1Orig = pAugmentMods1 + sizeof sigAug1 - 2;
		oAugmentMods1Mod = oAugmentMods1Orig + 6 + 6 + 3 + 3;

		CreateHook("Cheat (augmentMods)", pAugmentMods2, (LPVOID)HAugmentMods2, nullptr, augmentMods);
		oAugmentMods2 = pAugmentMods2 + sizeof sigAug2 + pAugmentMods2[sizeof sigAug2] + 1;

		pAugmentMods3 += sizeof sigAug3 - 2;
		CreateHook("Cheat (augmentMods)", pAugmentMods3, (LPVOID)HAugmentMods3, nullptr, augmentMods);
	}

	BYTE sigBuff[] = { 0xF3, 0x0F, 0x10, 0x48, 0x0C, 0xF3, 0x0F, 0x10, 0x50, 0x10, 0x8B, 0x43, 0x74 };
	if (FindSignature("Cheat (buffMods)", sigBuff, &pBuffMods))
	{
		buffMods = config.getBool("buffs", "enabled", false);
		for (auto buffId : config.getSectionInts("buffs"))
			for (size_t i = 0; i < ListStatus.size(); i++)
				if (ListStatus[i].first == buffId)
				{
					auto values = config.getFloats("buffs", std::to_string(buffId).c_str());
					if (values.size() == 3)
					{
						buffModsValues[buffId].Enabled = TRUE;
						buffModsValues[buffId].Timer = values[0];
						buffModsValues[buffId].Param0 = values[1];
						buffModsValues[buffId].Param1 = values[2];
						break;
					}
				}

		CreateHook("Cheat (buffMods)", pBuffMods += sizeof sigBuff, (LPVOID)HBuffMods, &oBuffMods, buffMods);
	}

	if ((shareWeaponSkills = config.getBool("cheats", "shareWeaponSkills", false)))
	{
		BYTE sig1[] = { 0x0F,0x84,0x97,0x00,0x00,0x00,0x8B,0xCC,0xCC,0xCC,0x8B,0xCC,0x8B };
		BYTE sig2[] = { 0x75,0x06,0xC7,0x02,0xFF,0xFF,0xFF,0xFF,0x8B,0xCC,0xCC,0x83 };
		BYTE sig3[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x70,0x02 };
		BYTE sig4[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x74,0x02 };
		BYTE sig5[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x78,0x02 };
		BYTE sig6[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x7C,0x02 };
		BYTE sig7[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x80,0x02 };
		BYTE sig8[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x84,0x02 };

		if (FindSignature("Cheat (shareWeaponSkills1)", sig1, &pOffset))
			Set<BYTE>(pOffset, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

		if (FindSignature("Cheat (shareWeaponSkills2)", sig2, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills3)", sig3, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills4)", sig4, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills5)", sig5, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills6)", sig6, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills7)", sig7, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills8)", sig8, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
	}
	else
		LOG("Cheat (shareWeaponSkills): disabled\n");

	if ((ignoreEquipVocation = config.getBool("cheats", "ignoreEquipVocation", false)))
	{
		BYTE sig1[] = { 0x0F, 0x94, 0xC0, 0xC2, 0x0C, 0x00 };
		BYTE sig2[] = { 0x74, 0x0F, 0x3B, 0xC2, 0x74, 0x0E, 0x8B, 0x41, 0x04, 0x83 };
		BYTE sig3[] = { 0x3B, 0xC2, 0x75, 0x15, 0x8B, 0x4D, 0x10 };
		BYTE sig4[] = { 0x0F, 0x94, 0xC0, 0x84, 0xC0, 0x75, 0x09, 0x5F };
		BYTE sig5[] = { 0x0F, 0x94, 0xC0, 0x84, 0xC0, 0x74, 0x60 };
		BYTE sig6[] = { 0x0F, 0x95, 0xC1, 0x84, 0xC9, 0x74, 0x09, 0x83 };
		BYTE sig7[] = { 0x75, 0x02, 0xB3, 0x01, 0x8B, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xE8 };
		BYTE sig8[] = { 0x75, 0x17, 0x8B, 0x44, 0x24, 0x14 };

		if (FindSignature("Cheat (ignoreEquipVocation1)", sig1, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0xB0, 0x01 });
		if (FindSignature("Cheat (ignoreEquipVocation2)", sig2, &pOffset)) Set<BYTE>(pOffset + 4, { 0xEB });
		if (FindSignature("Cheat (ignoreEquipVocation3)", sig3, &pOffset)) Set<BYTE>(pOffset + 2, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreEquipVocation4)", sig4, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0xB0, 0x01 });
		if (FindSignature("Cheat (ignoreEquipVocation5)", sig5, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0xB0, 0x01 });
		if (FindSignature("Cheat (ignoreEquipVocation6)", sig6, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x30, 0xC9 });
		if (FindSignature("Cheat (ignoreEquipVocation7)", sig7, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreEquipVocation8)", sig8, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
	}
	else
		LOG("Cheat (ignoreEquipVocation): disabled\n");

	if ((ignoreSkillVocation = config.getBool("cheats", "ignoreSkillVocation", false)))
	{
		BYTE sig1[] = { 0x74, 0x2F, 0x8B, 0x47, 0x10 };
		BYTE sig2[] = { 0x74, 0x74, 0x8B, 0x44, 0x24, 0x1C };
		BYTE sig3[] = { 0x74, 0x24, 0x83, 0xBD, 0xCC, 0xCC, 0xCC, 0xCC, 0x05 };
		BYTE sig4[] = { 0x8B, 0x4A, 0x10, 0x49, 0x3B, 0xC1 };

		if (FindSignature("Cheat (ignoreSkillVocation1)", sig1, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreSkillVocation2)", sig2, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreSkillVocation3)", sig3, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreSkillVocation4)", sig4, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90, 0x8B, 0xC1 });
	}
	else
		LOG("Cheat (ignoreSkillVocation): disabled\n");

	InGameUIAdd(renderCheatsUI);
}
