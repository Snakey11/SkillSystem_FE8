
/* Intended behavior:
	Return 1 if there is any spell that is usable with enough HP to cast it with a valid target.
	Return 2 if there is at least 1 usable spell, but there isn't enough HP to cast any usable spells.
	Return 3 if there are no spells with valid targets. */
int GaidenMagicUMUsability(void) // It's kinda weird that usability is void, but the other UM functions get the menu proc passed in...
{
	u8* spellList = SpellsGetter(gActiveUnit); // This is a 0-terminated list of spells this character has learned.
	u8* validList = gGenericBuffer; // Let's build a list of valid spells.
	for ( int i = 0 ; spellList[i] ; i++ )
	{
		int spell = spellList[i];
		// There was a check here related to is the unit moving, but I think that was unnecessary?
		
		// First, let's check to see they have the weapon rank.
		if ( gActiveUnit->ranks[gItemData[spell].weaponType] < gItemData[spell].weaponRank ) { continue; }
		
		// Next, let's ensure that this spell is marked as a "weapon."
		if ( !(gItemData[spell].attributes & IA_WEAPON) ) { continue; }
		
		// This function should do a bit of miscellaneous conditional stuff.
		if ( !CanUnitUseWeaponNow(gActiveUnit,spell) ) { continue; }
		
		// Next, we can initialize a "dummy" target list and check if it's empty. If not, then there's a valid target we can attack.
		MakeTargetListForWeapon(gActiveUnit,spell);
		if ( GetTargetListSize() == 0 ) { continue; }
		
		*validList = spell;
		validList++;
	}
	*validList = 0;
	// At this point, validList has a 0-terminated list of spells that we can use NOW (aside from HP costs).
	validList = gGenericBuffer;
	if ( !*validList ) { return 3; } // Return unusable if there are no valid spells.
	for ( int i = 0 ; validList[i] ; i++ )
	{
		// Now let's loop through all valid spells and see if there are ANY that we can cast given our current HP.
		if ( HasSufficientHP(gActiveUnit,validList[i]) ) { return 1; } // We've found a spell we can cast! Return usable.
	}
	return 2; // There were valid spells, but we don't have enough HP to cast any of them. Return greyed out.
}

int GaidenMagicUMEffect(MenuProc* proc, MenuCommandProc* commandProc)
{
	if ( commandProc->availability == 2 )
	{
		// Option is greyed out. Error R-text!
		MenuCallHelpBox(proc,gGaidenMagicUMErrorText);
		return 0x08;
	}	
	else
	{
		_ResetIconGraphics();
		UsingSpellMenu = 1;
		LoadIconPalettes(4);
		MenuProc* menu = StartMenu(&SpellSelectMenuDefs);
		// We're going to load a face now. I'm going to leave out the hardcoded check for the phantom (for now at least).
		StartFace(0,GetUnitPortraitId(gActiveUnit),0xB0,0xC,2);
		SetFaceBlinkControlById(0,5);
		ForceMenuItemPanel(menu,gActiveUnit,15,11);
		return 0x17;
	}
}

int GaidenMagicUMHover(MenuProc* proc)
{
	UsingSpellMenu = 1;
	BmMapFill(gMapMovement,-1);
	BmMapFill(gMapRange,0);
	FillRangeMapByRangeMask(gActiveUnit,GetUnitRangeMask(gActiveUnit,-1));
	DisplayMoveRangeGraphics(3);
	return 0;
}

int GaidenMagicUMUnhover(MenuProc* proc)
{
	UsingSpellMenu = 0;
	HideMoveRangeGraphics();
	return 0;
}
