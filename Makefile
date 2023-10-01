CC=i686-w64-mingw32-gcc
CXX=i686-w64-mingw32-g++
AR=i686-w64-mingw32-ar
OPT=-O2 -fno-math-errno -ffunction-sections
CFLAGS=$(OPT)
CXXFLAGS=-std=c++11 $(OPT)
LDFLAGS=-s $(OPT) -Wl,--gc-sections,--kill-at
LIBS=$(IG_LIB) $(MH_LIB) -ld3dx9_43

LIB=dinput8.dll
OFILES=DamageLog.o InGameUI.o ItemEditor.o Misc.o Cheats.o d3d9.o Hotkeys.o InGameClock.o dinput8.o iniConfig.o PlayerStats.o Portcrystals.o SaveBackup.o Server.o WeaponSets.o Lists.o
CFILES=DamageLog.cpp InGameUI.cpp ItemEditor.cpp Misc.cpp Cheats.cpp d3d9.cpp Hotkeys.cpp InGameClock.cpp dinput8.cpp iniConfig.cpp PlayerStats.cpp Portcrystals.cpp SaveBackup.cpp Server.cpp WeaponSets.cpp Lists.cpp

MH_LIB=MinHook/MinHook.a
MH_OFILES=MinHook/hook.o MinHook/trampoline.o MinHook/buffer.o MinHook/hde32.o
MH_CFILES=MinHook/hook.c MinHook/trampoline.c MinHook/buffer.c MinHook/hde32.c

IG_LIB=ImGui/ImGui.a
IG_OFILES=ImGui/imgui.o ImGui/imgui_draw.o ImGui/imgui_impl_dx9.o 
IG_CFILES=ImGui/imgui.cpp ImGui/imgui_draw.cpp ImGui/imgui_impl_dx9.cpp

$(LIB): $(OFILES) $(IG_LIB) $(MH_LIB) 
	$(CXX) $(LDFLAGS) -shared -o $(LIB) $(OFILES) $(LIBS)

$(MH_LIB): $(MH_OFILES)
	$(AR) qcs $(MH_LIB) $(MH_OFILES)

$(IG_LIB): $(IG_OFILES)
	$(AR) qcs $(IG_LIB) $(IG_OFILES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< 

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 
