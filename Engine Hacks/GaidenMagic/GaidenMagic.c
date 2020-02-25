
#include <stddef.h>
#include "FE-Clib-master/include/gbafe.h"

typedef struct SpellList SpellList;

struct SpellList
{
	u8 level;
	u8 spell;
};

extern SpellList* SpellListTable[0xFF]; // Entirely different from vanilla Spell Association Table. Just indexed by character ID, points to a ROM spell list.
extern const MenuDefinition SpellSelectMenuDefs;
extern u8 SpellsBuffer[10]; // 0x202B6D0. Undocced RAM. Just a temporary buffer it seems.
extern u8 UsingSpellMenu; // 0x0203F082. Boolean for using spell menu.
extern u8 SelectedSpell; // 0x0203F080.
extern u16 gGaidenMagicUMErrorText;
extern u16 gGaidenMagicSpellMenuErrorText;

extern void MakeTargetListForWeapon(Unit* unit, int item); // 0x080251B4.
extern void SetFaceBlinkControlById(int faceId, int idk2); // 0x088006458. faceId is a guess.
extern void ForceMenuItemPanel(MenuProc* menu, Unit* unit, int x, int y); // 0x0801E684. idk about the x and y parameters. Just a guess.
extern u32 GetUnitRangeMask(Unit* unit, u8 val); // 0x80171E8.
extern u32 GetWeaponRangeMask(int item); // 0x080170D4.
extern MenuProc* StartMenu_AndDoSomethingCommands(const MenuDefinition*, int xScreen, int xLeft, int xRight); // 0x804F64C.
extern void DrawItemMenuCommand(TextHandle* text, u16 item, int canUse, u16* buffer); // 0x08016848.

extern MenuDefinition gMenu_UnitMenu; // 0x0859D1F0.

int GaidenMagicUMUsability(void);
int GaidenMagicUMEffect(MenuProc* proc, MenuCommandProc* commandProc);
int GaidenMagicUMHover(MenuProc* proc);
int GaidenMagicUMUnhover(MenuProc* proc);

u8* SpellsGetter(Unit* unit);
int NewGetUnitEquippedWeapon(Unit* unit);
int NewGetUnitEquippedWeaponSlot(Unit* unit);
u32 NewGetUnitRangeMask(Unit* unit, int slot);
static int HasSufficientHP(Unit* unit, int spell);
static int CanCastSpellNow(Unit* unit, int spell);

int SpellUsability(const struct MenuCommandDefinition* menuEntry, int index, int idk);
int SpellDrawingRoutine(MenuProc* menu, MenuCommandProc* menuCommand);
int MagicMenuBPress(void);
int SpellEffectRoutine(MenuProc* proc, MenuCommandProc* menuCommand);
int SpellOnHover(MenuProc* proc);
int SpellOnUnhover(MenuProc* proc);

#include "UnitMenu.c"
#include "SpellSystem.c"
#include "SpellMenu.c"
