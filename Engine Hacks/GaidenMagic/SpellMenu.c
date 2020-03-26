
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
	int spell = GetNthUsableSpell(gActiveUnit,proc->commandIndex);
	SelectedSpell = spell;
	
	//UpdateMenuItemPanel(proc); // We're gonna rewrite and inline this instead.
	MenuItemPanelProc* menuItemPanel = (MenuItemPanelProc*)ProcFind(&gProc_MenuItemPanel);
	TextHandle* textHandle = &menuItemPanel->textHandle;
	TextHandle* textHandle2 = &menuItemPanel->textHandle2;
	TextHandle* textHandle3 = &menuItemPanel->textHandle3;
	int x = menuItemPanel->x;
	int y = menuItemPanel->y;
	
	Text_Clear(&menuItemPanel->textHandle);
	Text_Clear(&menuItemPanel->textHandle2);
	Text_Clear(&menuItemPanel->textHandle3);
	MakeUIWindowTileMap_BG0BG1(x,y,14,8,0);
	BattleGenerateUiStats(gActiveUnit,9); // 9 is using a Gaiden spell.
	
	Text_InsertString(textHandle,0x02,0,GetStringFromIndex(0x4F1)); // Affin.
	Text_InsertString(textHandle,0x32,0,GetStringFromIndex(gGaidenMagicHPCostText)); // HP Cost.
	Text_InsertString(textHandle2,0x02,0,GetStringFromIndex(0x4F3)); // Atk.
	Text_InsertString(textHandle3,0x02,0,GetStringFromIndex(0x4F4)); // Hit.
	Text_InsertString(textHandle2,0x32,0,GetStringFromIndex(0x501)); // Crit.
	Text_InsertString(textHandle3,0x32,0,GetStringFromIndex(0x4F5)); // Avoid.
	
	int CostColor = 2;
	if ( !HasSufficientHP(gActiveUnit,spell) ) { CostColor = 1; }
	Text_InsertNumberOr2Dashes(textHandle,0x54,CostColor,GetItemAwardedExp(spell));
	Text_InsertNumberOr2Dashes(textHandle2,0x24,2,gBattleActor.battleAttack);
	Text_InsertNumberOr2Dashes(textHandle3,0x24,2,gBattleActor.battleHitRate);
	Text_InsertNumberOr2Dashes(textHandle2,0x54,2,gBattleActor.battleCritRate);
	Text_InsertNumberOr2Dashes(textHandle3,0x54,2,gBattleActor.battleAvoidRate);
	
	Text_Display(textHandle,&gBg0MapBuffer[((y+1)<<5)+1+x]);
	Text_Display(textHandle2,&gBg0MapBuffer[((y+3)<<5)+1+x]);
	Text_Display(textHandle3,&gBg0MapBuffer[((y+5)<<5)+1+x]);
	
	//u16* bg0buffer = GetBgMapBuffer(0) + x + (y << 5);
	DrawIcon(GetBgMapBuffer(0) + x + (y << 5)+0x25,GetItemType(spell)+0x70,menuItemPanel->oam2base<<0xC);
	// 0x0202BE6A
	BmMapFill(gMapMovement,-1);
	BmMapFill(gMapRange,0);
	FillRangeMapByRangeMask(gActiveUnit,GetWeaponRangeMask(spell));
	DisplayMoveRangeGraphics(2);
	return 0;
}

int SpellOnUnhover(MenuProc* proc)
{
	HideMoveRangeGraphics();
	return 0;
}
