
#include <stddef.h>
#include "FE-Clib-master/include/gbafe.h"

typedef struct SpellList SpellList;
typedef struct MenuItemPanelProc MenuItemPanelProc;

struct SpellList
{
	u8 level;
	u8 spell;
};

struct MenuItemPanelProc
{
	PROC_HEADER;
	u8 pad1[3]; // 0x29.
	Unit* unit; // 0x2C.
	u8 x; // 0x30.
	u8 y; // 0x31.
	u8 oam2base, pad2; // 0x32, 0x33.
	TextHandle textHandle, textHandle2, textHandle3; // 0x34, 0x3C 0x44.
};

extern SpellList* SpellListTable[0xFF]; // Entirely different from vanilla Spell Association Table. Just indexed by character ID, points to a ROM spell list.
extern const MenuDefinition SpellSelectMenuDefs;
extern u8 SpellsBuffer[10]; // 0x202B6D0. Undocced RAM. Just a temporary buffer it seems.
extern u8 UsingSpellMenu; // 0x0203F082. Boolean for using spell menu.
extern u8 SelectedSpell; // 0x0203F080.
extern u16 gGaidenMagicHPCostText;
extern u16 gGaidenMagicUMErrorText;
extern u16 gGaidenMagicSpellMenuErrorText;

extern void MakeTargetListForWeapon(Unit* unit, int item); // 0x080251B4.
extern void SetFaceBlinkControlById(int faceId, int idk2); // 0x088006458. faceId is a guess.
extern void ForceMenuItemPanel(MenuProc* menu, Unit* unit, int x, int y); // 0x0801E684. idk about the x and y parameters. Just a guess.
extern u32 GetUnitRangeMask(Unit* unit, u8 val); // 0x80171E8.
extern u32 GetWeaponRangeMask(int item); // 0x080170D4.
extern MenuProc* StartMenu_AndDoSomethingCommands(const MenuDefinition*, int xScreen, int xLeft, int xRight); // 0x804F64C.
extern void DrawItemMenuCommand(TextHandle* text, u16 item, int canUse, u16* buffer); // 0x08016848.
extern int AttackUMEffect(MenuProc* proc, MenuCommandProc* commandProc); // 0x08022B30.
extern void UpdateMenuItemPanel(MenuProc* proc); // 0x0801E748.
extern void BattleGenerateUiStats(Unit* unit, int slot); // 0x802A400.
extern void DrawItemRText(int xTile, int yTile, int item); // 0x08088E60.

extern MenuDefinition gMenu_UnitMenu; // 0x0859D1F0.
extern TargetSelectionDefinition SpellTargetSelection; // 0x0859D3F8.
extern ProcInstruction gProc_MenuItemPanel; // 0x0859AE88.

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
static int GetUnitRangeMaskForSpells(Unit* unit);
static int GetNthUsableSpell(Unit* unit, int n);

int SpellUsability(const struct MenuCommandDefinition* menuEntry, int index, int idk);
int SpellDrawingRoutine(MenuProc* menu, MenuCommandProc* menuCommand);
int MagicMenuBPress(void);
int SpellEffectRoutine(MenuProc* proc, MenuCommandProc* menuCommand);
int SpellOnHover(MenuProc* proc);
int SpellOnUnhover(MenuProc* proc);
void NewExitBattleForecase(Proc* proc);

#include "UnitMenu.c"
#include "SpellSystem.c"
#include "SpellMenu.c"
