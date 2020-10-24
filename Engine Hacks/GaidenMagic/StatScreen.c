
TextHandle* GaidenStatScreen(int x, int y, TextHandle* currHandle) // Called from MSS. Returns the next "blank" text handle.
{
	u8* spells = SpellsGetter(gpStatScreenUnit,-1);
	int tile = (currHandle-1)->tileIndexOffset;
	int iconX = x;
	int iconY = y;
	for ( int i = 0 ; spells[i] ; i++ )
	{
		const ItemData* item = GetItemData(spells[i]);
		DrawIcon(&StatScreenBufferMap[iconY][iconX],item->iconId,0x4000);
		
		tile += 6;
		currHandle->tileIndexOffset = tile;
		currHandle->xCursor = 0;
		currHandle->colorId = TEXT_COLOR_NORMAL;
		currHandle->tileWidth = 6;
		currHandle->useDoubleBuffer = 0;
		currHandle->currentBufferId = 0;
		currHandle->unk07 = 0;
		
		Text_Clear(currHandle);
		Text_SetColorId(currHandle,TEXT_COLOR_NORMAL);
		Text_InsertString(currHandle,0,TEXT_COLOR_NORMAL,GetStringFromIndex(item->nameTextId));
		Text_Display(currHandle,&StatScreenBufferMap[iconY][iconX+2]);
		
		currHandle++;
		if ( iconX == x ) { iconX += 8; }
		else { iconX = x; iconY += 2; }
	}
	return currHandle;
}

void GaidenRTextGetter(RTextProc* proc)
{
	int index = *(proc->rTextData+0x12);
	proc->type = SpellsGetter(gpStatScreenUnit,-1)[index]; // I think if this is positive, it treats this as an item bubble.
	proc->textID = GetItemData(proc->type)->descTextId;
}

void GaidenRTextLooper(RTextProc* proc)
{
	int index = *(proc->rTextData+0x12);
	// We can move to this index if we've learned this spell.
	if ( !DoesUnitKnowSpell(gpStatScreenUnit,SpellsGetter(gpStatScreenUnit,-1)[index]) )
	{
		if ( proc->direction == 0 ) { RTextDown(proc); }
		else if ( proc->direction == 0x10 ) { RTextUp(proc); }
		else if ( proc->direction == 0x40 ) { RTextRight(proc); }
		else if ( proc->direction == 0x80 ) { RTextLeft(proc); }
	}
}
