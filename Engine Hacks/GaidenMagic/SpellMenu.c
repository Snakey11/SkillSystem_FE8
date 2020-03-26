
// This follows those weird menu usability return values. 1 = usable, 2 = grey, 3 = unsuable.
int SpellUsability(const struct MenuCommandDefinition* menuEntry, int index, int idk)
{
	int spell = SpellsGetter(gActiveUnit)[index]|0xFF00;
	if ( !spell ) { return 3; }
	// This option should be usable if the nth spell exists.
	if ( !CanCastSpellNow(gActiveUnit,spell) ) { return 3; }
	// Now, let's grey out the spell if we don't have the HP to cast it.
	return ( HasSufficientHP(gActiveUnit,spell) ? 1 : 2 );
	
}

int SpellDrawingRoutine(MenuProc* menu, MenuCommandProc* menuCommand)
{
	// extern void DrawItemMenuCommand(TextHandle* textHandle, u16 item, int canUse, u16* buffer);
	int spell = SpellsGetter(gActiveUnit)[menuCommand->commandDefinitionIndex] | 0xFF00; // Max uses.
	// At this point, the spell should be guranteed to exist. Let's check to see if we have the HP to cast the spell (and the weapon rank).
	int canUse = CanUnitUseWeapon(gActiveUnit,spell) && HasSufficientHP(gActiveUnit,spell);
	DrawItemMenuCommand(&menuCommand->text,spell,canUse,&gBg0MapBuffer[menuCommand->yDrawTile * 32 + menuCommand->xDrawTile]);
	EnableBgSyncByMask(1);
	return 0;
}

int MagicMenuBPress(void)
{
	FillBgMap(gBg2MapBuffer,0);
	EnableBgSyncByMask(4);
	Text_ResetTileAllocation();
	// Refer to the Skill System's repointed unit menu (even though it's not a table reee).
	StartMenu_AndDoSomethingCommands(&gMenu_UnitMenu,gGameState._unk1C.x - gGameState.cameraRealPos.x,1,16);
	HideMoveRangeGraphics();
	SelectedSpell = 0;
	UsingSpellMenu = 0;
	return 0x3B;
}

int SpellEffectRoutine(MenuProc* proc, MenuCommandProc* commandProc)
{
	if ( commandProc->availability == 2)
	{
		// Option is greyed out. Error R-text!
		MenuCallHelpBox(proc,gGaidenMagicSpellMenuErrorText);
		return 0x08;
	}
	else
	{
		// Actual effect. Call the target selection menu and shit.
		gActionData.itemSlotIndex = 0;
		ClearBG0BG1();
		MakeTargetListForWeapon(gActiveUnit,SelectedSpell|0xFF00);
		StartTargetSelection(&SpellTargetSelection);
		return 0x27;
	}
}

int SpellOnHover(MenuProc* proc)
{
	BmMapFill(gMapMovement,-1);
	BmMapFill(gMapRange,0);
	FillRangeMapByRangeMask(gActiveUnit,GetWeaponRangeMask(GetNthUsableSpell(gActiveUnit,proc->commandIndex)));
	DisplayMoveRangeGraphics(2);
	return 0;
}

int SpellOnUnhover(MenuProc* proc)
{
	HideMoveRangeGraphics();
	return 0;
}
