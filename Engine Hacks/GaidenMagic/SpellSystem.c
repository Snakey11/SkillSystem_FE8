
u8* SpellsGetter(Unit* unit) // Returns a pointer to a list of spells this character can currently use.
{
	for ( int i = 0 ; i < 10 ; i++ ) { SpellsBuffer[i] = 0; } // Clear the spell buffer.
	int level = unit->level;
	if ( UNIT_ATTRIBUTES(unit) & CA_PROMOTED ) { level += 80; } // Treat promoted as top bit set.
	u8* currBuffer = SpellsBuffer;
	SpellList* ROMList = SpellListTable[unit->pCharacterData->number];
	if ( ROMList )
	{
		// ROMList is a non-null pointer.
		for ( int i = 0 ; ROMList[i].level ; i++ )
		{
			if ( level >= ROMList[i].level )
			{
				// Valid spell found!
				*currBuffer = ROMList[i].spell;
				currBuffer++;
			}
		}
	}
	// Whether or not there were any matching spells (or if the list even existed), we need to terminate the list.
	*currBuffer = 0;
	return SpellsBuffer;
}

int NewGetUnitEquippedWeapon(Unit* unit) // Autohook to 0x08016B28.
{
	if ( unit == gActiveUnit && UsingSpellMenu && CanUnitUseWeapon(unit,SelectedSpell) ) { return SelectedSpell; }
	// Vanilla behavior. Loop through inventory to see if any items are currently usable as weapons.
	for ( int i = 0 ; i < 5 ; i++ )
	{
		if ( CanUnitUseWeapon(unit,unit->items[i]) ) { return unit->items[i]; }
	}
	return 0;
}

int NewGetUnitEquippedWeaponSlot(Unit* unit) // Autohook to 0x08016B58.
{
	if ( CanUnitUseWeapon(unit,SelectedSpell) ) { return 9; }
	for ( int i = 0 ; i < 5 ; i++ )
	{
		if ( CanUnitUseWeapon(unit,unit->items[i]) ) { return i; }
	}
	return -1;
}

u32 NewGetUnitRangeMask(Unit* unit, int slot) // Autohook to 0x080171E8. It seems slot is the inventory slot we're looking at. -1 is undefined (like on the map).
// This function may be quite different from how it was expressed in Gaiden Magic v1.0, but I think this should work fine.
{
	if ( slot < 0 || slot > 5)
	{
		// No inventory slot defined. Loop through the unit's inventory.
		u32 mask = 0;
		if ( UsingSpellMenu )
		{
			mask |= GetUnitRangeMaskForSpells(unit);
		}
		for ( int i = 0 ; i < 5 ; i++ )
		{
			if ( CanUnitUseWeapon(unit,unit->items[i]) )
			{
				mask |= GetWeaponRangeMask(unit->items[i]);
			}
		}
		return mask;
	}
	else
	{
		// We already have a slot defined. Check to see if they're using Gaiden magic.
		if ( UsingSpellMenu )
		{
			u8* spells = SpellsGetter(unit);
			int spell = spells[SelectedSpell];
			return ( spell ? GetWeaponRangeMask(spell) : GetWeaponRangeMask(unit->items[slot]) );
		}
		else
		{
			// Vanilla behavior.
			return GetWeaponRangeMask(unit->items[slot]);
		}
	}
}

static int HasSufficientHP(Unit* unit, int spell)
{
	// WeaponEXP granted in item data is also the HP cost of the spell.
	return unit->curHP > GetItemData(spell)->weaponExp;
}

// This function is going to check if we should be able to use this spell NOW. If this is an attack spell, are we in range, etc.
// This does NOT check for HP cost.
static int CanCastSpellNow(Unit* unit, int spell)
{
	// This function should do a bit of miscellaneous conditional stuff.
		if ( !CanUnitUseWeaponNow(gActiveUnit,spell) ) { return 0; }
		
		// Next, we can initialize a "dummy" target list and check if it's empty. If not, then there's a valid target we can attack.
		MakeTargetListForWeapon(gActiveUnit,spell);
		if ( GetTargetListSize() == 0 ) { return 0; }
		return 1;
}

static int GetUnitRangeMaskForSpells(Unit* unit)
{
	u32 mask = 0;
	u8* spells = SpellsGetter(unit);
	// Orr the range mask with all usable spells.
	for ( int i = 0 ; spells[i] ; i++ )
	{
		if ( CanUnitUseWeapon(unit,spells[i]) && HasSufficientHP(unit,spells[i]) )
		{
			mask |= GetWeaponRangeMask(spells[i]);
		}
	}
	return mask;
}

// This should loop through spells that are usable NOW. Basically, a spell should be considered usable if it appears in the spell menu.
// Don't check for HP because those are greyed out with error R-text.
static int GetNthUsableSpell(Unit* unit, int n)
{
	u8* spells = SpellsGetter(unit);
	int k = -1;
	for ( int i = 0 ; spells[i] ; i++ )
	{
		if ( CanCastSpellNow(unit,spells[i]) )
		{
			k++;
			if ( k == n ) { return spells[i]; }
		}
	}
	return -1;
}
