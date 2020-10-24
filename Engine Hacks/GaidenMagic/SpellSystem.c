
u8* SpellsGetter(Unit* unit, int type) // Returns a pointer to a list of spells this character can currently use. Type: 0 = All, 1 = black magic, 2 = white magic.
{
	return SpellsGetterForLevel(unit,-1,type);
}

u8* SpellsGetterForLevel(Unit* unit, int level, int type)  // Same as SpellsGetter but filters for a specific level.
{
	// Treat level = -1 as any level equal to or below the unit's current level.
	for ( int i = 0 ; i < 10 ; i++ ) { SpellsBuffer[i] = 0; } // Clear the spell buffer.
	int unitLevel = unit->level;
	if ( UNIT_ATTRIBUTES(unit) & CA_PROMOTED ) { unitLevel += 80; } // Treat promoted as top bit set.
	u8* currBuffer = SpellsBuffer;
	SpellList* ROMList = SpellListTable[unit->pCharacterData->number];
	if ( ROMList )
	{
		// ROMList is a non-null pointer.
		for ( int i = 0 ; ROMList[i].level ; i++ )
		{
			if ( (level == -1 && unitLevel >= ROMList[i].level) || (level == ROMList[i].level) )
			{
				if ( type == -1 || type == GetSpellType(ROMList[i].spell) )
				{
					// Valid spell found!
					*currBuffer = ROMList[i].spell;
					currBuffer++;
				}
			}
		}
	}
	// Whether or not there were any matching spells (or if the list even existed), we need to terminate the list.
	*currBuffer = 0;
	return SpellsBuffer;
}

/* Intended behavior:
	First, check if we're using the spell menu.
		If so, return the selected spell. Otherwise, vanilla.
	If battle hasn't started yet, vanilla behavior. Return the first usable weapon in inventory.
	If the battle has started...
		If attacking, we already know we're not trying to use a Gaiden spell, so just vanilla.
		If defending, check if there is a usable weapon in inventory we can counter with.
			If so, vanilla. If not, return the first gaiden spell whether it's usable or not.
*/
int NewGetUnitEquippedWeapon(Unit* unit) // Autohook to 0x08016B28.
{
	int vanillaEquipped = GetVanillaEquipped(unit);
	if ( gChapterData.currentPhase == ( unit->index & 0xC0 ) )
	{
		return ( UsingSpellMenu ? SelectedSpell|0xFF00 : vanillaEquipped );
	}
	else
	{
		// Well, all the logic is in NewGetUnitEquippedWeaponSlot. Why not get the slot then return that item, checking for case 9 (Gaiden magic)?
		if ( GetUnitEquippedWeaponSlot(unit) == 9 )
		{
			// We're not using the spell menu, but we're still using Gaiden magic. We must be trying to counter with it.
			int spell = SpellsGetter(unit,-1)[0];
			return ( spell ? spell|0xFF00 : 0 );
		}
		else { return vanillaEquipped; }
	}
}

int NewGetUnitEquippedWeaponSlot(Unit* unit) // Autohook to 0x08016B58.
{
	if ( UsingSpellMenu && CanUnitUseWeapon(unit,SelectedSpell) ) { return 9; } // If we're using the spell menu, return using Gaiden magic.
	// This function appears only to be called in simulated and real battes (and on the stat screen)?
	if ( (gBattleStats.config & (BATTLE_CONFIG_REAL|BATTLE_CONFIG_SIMULATE)) && unit->index == gBattleTarget.unit.index )
	{
		int spell = SpellsGetter(unit,-1)[0];
		// If we're here this is a real or simulated batte, and we're the defender. See if we can use what would be the equipped weapon.
		for ( int i = 0 ; i < 5 ; i++ )
		{
			if ( CanUnitUseWeapon(unit,unit->items[i]) )
			{
				// This would be the equipped weapon. If we can't counter with this, then we should return the first Gaiden spell.
				if ( !gCan_Attack_Target(unit->items[i],gBattleStats.range,unit) )
				{
					return ( spell ? 9 : i );
				}
				else { return i; }
			}
		}
		return ( spell ? 9 : -1 );
	}
	else
	{
		for ( int i = 0 ; i < 5 ; i++ )
		{
			if ( CanUnitUseWeapon(unit,unit->items[i]) ) { return i; }
		}
	}
	return -1;
}

// We need to change this FE8-Item Range Fix function to include checking Gaiden magic.
	// Slot is the inventory slot to check. -1 is check all slots, and 9 can represent just for Gaiden magic. Why don't we let -2 mean only loop through items in inventory?
// Wow this function actually is supposed to return a value in r1.
	// Return the range bitfield (range mask) in r0 and the min/max halfword in r1.
	// The min/max halfword only really appears to apply to special ranges not even including seige tomes?
	// Jesus christ this return scheme aaaaaaaaa. This is a way to "trick" the compiler into returning into r1.
long long Return_Range_Bitfield(Unit* unit, int slot, int(*usability)(Unit* unit, int item))
{
	long long current = 0;
	if ( slot == -1 || slot == -2 )
	{
		// Loop through all items.
		for ( int i = 0 ; i < 5 && unit->items[i] ; i++ )
		{
			if ( usability(unit,unit->items[i]) )
			{
				current = IncorporateNewRange(current,gGet_Item_Range(unit,unit->items[i]));
			}
		}
		return ( slot == -1 ? IncorporateNewRange(current,GetUnitRangeMaskForSpells(unit)) : current );
	}
	else
	{
		// Get for this specific slot.
		if ( slot != 9 )
		{
			return gGet_Item_Range(unit,unit->items[slot]);
		}
		else
		{
			// Specifically return all the Gaiden magic that's usable.
			return GetUnitRangeMaskForSpells(unit);
		}
	}
}

// r0 = mask, r1 = minmax. See ReturnRangeBitfield.
long long GetUnitRangeMaskForSpells(Unit* unit)
{
	long long current = 0;
	u8* spells = SpellsGetter(unit,-1);
	for ( int i = 0 ; spells[i] ; i++ )
	{
		int spell = spells[i]|0xFF00;
		if ( CanCastSpell(unit,spell) && HasSufficientHP(unit,spell) )
		{
			current = IncorporateNewRange(current,gGet_Item_Range(unit,spell));
		}
	}
	return current;
}

static long long IncorporateNewRange(long long existing, long long new)
{
	u32 existingMask = existing & 0xFFFFFFFF;
	u32 newMask = new & 0xFFFFFFFF;
	long long existingMin = existing >> 40;
	long long newMin = new >> 40;
	long long existingMax = (existing >> 32) & 0xFF;
	long long newMax = (new >> 32) & 0xFF;
	return existingMask|newMask|(( newMin < existingMin ? newMin : existingMin ) << 40)|(( newMax > existingMax ? newMax : existingMax ) << 32);
}

// Called by the Skill System's proc loop alongside counter skills. If this is a gaiden spell, set the HP drain bit and write how much HP to drain.
void Proc_GaidenMagicHPCost(BattleUnit* attacker, BattleUnit* defender, NewBattleHit* buffer, BattleStats* battleData)
{
	// First, let's check if the attacker is using a (gaiden) spell.
	int type = GetItemType(attacker->weapon);
	if ( type >= 5 && type <= 8 )
	{
		// They're using a spell. Let's get the HP cost.
		int cost = GetItemAwardedExp(attacker->weapon);
		// Will they die if they try to cast this spell?
		
		// Let's set the HP depletion bit.
		buffer->attributes |= 0x100; // "HP drain" bit.
		// Now let's subtract the cost from their HP. The check before gurantees they have enough HP to cast right now.
		attacker->unit.curHP -= cost;
		buffer->damage -= cost;
	}
}

int InitGaidenSpellLearnPopup(void) // Responsible for returning a boolean for whether we should show a "spell learned" popup after battle and for setting it up.
{
	// We should show the popup only if we've reached a level that unlocks a spell.
	// This should happen AFTER a level up, so we leveled up if our previous level != our current level.
	BattleUnit* subject = NULL;
	if ( gBattleActor.levelPrevious != gBattleActor.unit.level ) { subject = &gBattleActor; }
	if ( gBattleTarget.levelPrevious != gBattleTarget.unit.level ) { subject = &gBattleTarget; }
	if ( !subject ) { return 0; } // If this isn't filled, we shouldn't show a popup.
	// Our unit leveled up! Let's see if they have a spell to gain at their new level.
	u8* spells = SpellsGetterForLevel(&subject->unit,subject->unit.level,-1);
	// Eh let's just handle learning one spell at a time for now.
	if ( *spells )
	{
		gPopupItem = *spells|0xFF00;
		return 1;
	} else { return 0; }
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
	int type = GetItemData(spell)->weaponType;
	if ( type != ITYPE_STAFF )
	{
		if ( !CanUnitUseWeaponNow(gActiveUnit,spell) ) { return 0; }
		// Next, we can initialize a "dummy" target list and check if it's empty. If not, then there's a valid target we can attack.
		MakeTargetListForWeapon(gActiveUnit,spell);
		if ( GetTargetListSize() == 0 ) { return 0; }
	}
	else
	{
		if ( !CanUnitUseItem(gActiveUnit,spell) ) { return 0; }
	}
	return 1;
}

static int CanCastSpell(Unit* unit, int spell) // Same as CanCastSpellNow but calls the functions... without the "Now."
{
	int type = GetItemData(spell)->weaponType;
	if ( type != ITYPE_STAFF )
	{
		if ( !CanUnitUseWeapon(gActiveUnit,spell) ) { return 0; }
		// Next, we can initialize a "dummy" target list and check if it's empty. If not, then there's a valid target we can attack.
		MakeTargetListForWeapon(gActiveUnit,spell);
		if ( GetTargetListSize() == 0 ) { return 0; }
	}
	else
	{
		if ( !CanUnitUseItem(gActiveUnit,spell) ) { return 0; }
	}
	return 1;
}

// This should loop through spells that are usable NOW. Basically, a spell should be considered usable if it appears in the spell menu.
// Don't check for HP because those are greyed out with error R-text.
static int GetNthUsableSpell(Unit* unit, int n, int type)
{
	u8* spells = SpellsGetter(unit,type);
	int k = -1;
	for ( int i = 0 ; spells[i] ; i++ )
	{
		if ( CanCastSpellNow(unit,spells[i]) )
		{
			k++;
			if ( k == n ) { return i; }
		}
	}
	return -1;
}

static int GetVanillaEquipped(Unit* unit)
{
	for ( int i = 0 ; i < 5 ; i++ )
	{
		if ( CanUnitUseWeapon(unit,unit->items[i]) ) { return unit->items[i]; }
	}
	return 0;
}

static int DoesUnitKnowSpell(Unit* unit, u8 spell)
{
	// Is this spell in this unit's spell list?
	u8* spells = SpellsGetter(unit,-1);
	for ( int i = 0 ; spells[i] ; i++ )
	{
		if ( spell == spells[i] ) { return 1; }
	}
	return 0;
}

static int GetSpellType(int spell)
{
	int wType = (GetItemData(spell))->weaponType;
	if ( wType == ITYPE_ANIMA || wType == ITYPE_DARK ) { return BLACK_MAGIC; }
	else if ( wType == ITYPE_STAFF || wType == ITYPE_LIGHT) { return WHITE_MAGIC; }
	else { return -1; }
}
