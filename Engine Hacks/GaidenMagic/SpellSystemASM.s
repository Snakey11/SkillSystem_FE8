
.thumb

.macro blh to, reg
    ldr \reg, =\to
    mov lr, \reg
    .short 0xF800
.endm

.global SetUpBattleWeaponDataHack
.type SetUpBattleWeaponDataHack, %function
SetUpBattleWeaponDataHack: @ Autohook to 0x0802A730. Creates case 9 for the switch (using a Gaiden Spell).
push { r4 - r7, lr }
mov r7, r9
mov r6, r8
push { r6, r7 }
mov r5, r0
mov r3, r1
@mov r0, #0x1
@neg r0, r0
@cmp r3, r0
@bne EnemyExists
	CheckWeaponSlot:
	mov r0, r5
	blh GetUnitEquippedWeaponSlot, r1
	mov r3, r0
EnemyExists:
ldr r0, [ r5, #0xC ]
mov r1, #0x80
lsl r1, r1, #0x4
and r0, r0, r1
cmp r0, #0x0
beq NotBallista
	mov r3, #0x8 @ 0x8 sets flag for ballista.
NotBallista:
mov r1, r5
add r1, #0x52
mov r0, #0x1
strb r0, [ r1 ]
mov r9, r1
cmp r3, #0x9
bhi NoWeapon	
	@ We've repointed this switch table to account for the spell menu case.
	lsl r0, r3, #0x02
	ldr r1, =SetUpBattleWeaponDataSwitchRepoint
	add r0, r0, r1
	ldr r0, [ r0 ]
	mov r15, r0
NoWeapon:
	@ This was a jump to FinalCase, but it seems the exact same as vanilla (with the exception of mov r0, #0xFF and mov r0, #0xFE, but that could be a typo?
	@ We're just gonna jump to vanilla.
	ldr r0, =#0x802A831
	bx r0

.global SetUpBattleWeaponDataForSpellMenu
.type SetUpBattleWeaponDataForSpellMenu, %function
SetUpBattleWeaponDataForSpellMenu: @ Called by entry 9 in SetUpBattleWeaponDataSwitchRepoint.
mov r4, r5
add r4, #0x51
mov r0, #0x09
strb r0, [ r4 ]
@ Now we need to think for a bit. If we're attacking, then we can trust SelectedSpell to contain the spell the player chose.
	@ If we're defending, we just need to select the first Gaiden Spell.
ldr r0, =gBattleTarget
cmp r0, r5
beq SpellMenuDefending
	ldr r1, =SelectedSpell
	ldrb r0, [ r1 ]
	b WriteSpell
SpellMenuDefending:
	@ r0 is already the defense struct, so...
	blh SpellsGetter, r1
	ldrb r0, [ r0 ] @ Get the first spell that this unit can use.
	cmp r0, #0x00
	beq SkipOrr @ If they don't have a spell to use, don't orr it with 0xFF00!
WriteSpell:
mov r2, #0xFF
lsl r2, r2, #8
orr r0, r0, r2 @ Max uses for this spell: 0xFF00|SelectedSpell.
SkipOrr:
mov r2, r5
add r2, r2, #0x48
mov r1, #0x1
strh r0, [ r2 ]
mov r0, r9
strb r1, [ r0 ] @ Set CanCounter.
mov r8, r4
mov r4, r2
ldr r1, =#0x802A84B
bx r1

.ltorg
.align

.global GaidenActionStaffDoorChestUseItemHack
.type GaidenActionStaffDoorChestUseItemHack, %function
GaidenActionStaffDoorChestUseItemHack: @ Autohook to 0x0802FC48.
push { r4 - r7, lr }
mov r7, r8
push { r7 }
mov r6, r0
ldr r4, =gActionData
@ Now we need to check whether we're using the spell menu.
ldr r0, =UsingSpellMenu
ldrb r0, [ r0 ] @ Nonzero if we're using GaidenMagic.
cmp r0, #0x00
bne ActionFixUsingSpell
	ldr r0, [ r4, #0x0C ]
	blh GetUnit, r1
	ldrb r1, [ r4, #0x12 ]
	lsl r1, r1, #0x01
	add r0, r0, #0x1E
	add r0, r0, r1
	ldrh r0, [ r0 ]
	blh GetItemIndex, r1
	b EndActionFix
ActionFixUsingSpell:
	ldr r0, =SelectedSpell
	ldrb r0, [ r0 ]
EndActionFix:
ldr r1, =#0x0802FC67
bx r1

.ltorg
.align

.global GaidenPreActionHack
.type GaidenPreActionHack, %function
GaidenPreActionHack: @ Autohook to 0x0801D1D0.
ldr r1, =UsingSpellMenu
ldrb r1, [ r1 ]
cmp r0, #0x00
bne PreActionFixUsingSpell
	ldrb r1, [ r4, #0x12 ]
	lsl r1, r1, #0x01
	add r0, r0, #0x1E
	add r0, r0, r1
	ldrh r0, [ r0 ]
	blh GetItemIndex, r1
	mov r2, r0
	b EndPreActionFix
PreActionFixUsingSpell:
	ldr r2, =SelectedSpell
	ldrb r2, [ r2 ]
EndPreActionFix:
ldr r0, =#0x0801D1E1
bx r0

.ltorg
.align

.global GaidenSetupBattleUnitForStaffHack
.type GaidenSetupBattleUnitForStaffHack, %function
GaidenSetupBattleUnitForStaffHack: @ Autohook to 0x0802CB24.
push { r4 - r7, lr } @ r0 = unit, r1 = gActionData+0x12 = inventory slot.
mov r2, r0
mov r7, r1
ldr r1, =UsingSpellMenu
ldrb r1, [ r1 ]
cmp r1, #0x00
bne SetupBattleUnitForStaffUsingSpell
	lsl r1, r7, #0x01
	add r0, #0x1E
	add r0, r0, r1
	ldrh r6, [ r0 ]
	cmp r7, #0x00
	bge SetupBattleUnitForStaffSkip
		mov r6, #0x00
	SetupBattleUnitForStaffSkip:
	b EndSetupBattleUnitForStaffFix
SetupBattleUnitForStaffUsingSpell:
	ldr r6, =SelectedSpell
	ldrb r6, [ r6 ]
	mov r0, #0xFF
	lsl r0, r0, #8
	orr r6, r0, r6
EndSetupBattleUnitForStaffFix:
ldr r0, =#0x0802CB39
bx r0

.ltorg
.align

.global GaidenExecStandardHealHack
.type GaidenExecStandardHealHack, %function
GaidenExecStandardHealHack: @ Autohook to 0x0802EBB4.
ldrb r0, [ r4, #0x0C ]
blh GetUnit, r1
mov r5, r0
ldrb r0, [ r4, #0x0C ]
blh GetUnit, r1
ldr r1, =UsingSpellMenu
ldrb r1, [ r1 ]
cmp r1, #0x00
bne ExecStandardHealUsingSpell
	ldrb r1, [ r4, #0x12 ]
	lsl r1, r1, #0x01
	add r0, r0, #0x1E
	add r0, r0, r1
	ldrh r1, [ r0 ]
	b EndExecStandardHeal
ExecStandardHealUsingSpell:
ldr r1, =SelectedSpell
ldrb r1, [ r1 ]
EndExecStandardHeal:
ldr r0, =#0x0802EBCD
bx r0

.ltorg
.align

.global GaidenExecFortifyHack
.type GaidenExecFortifyHack, %function
GaidenExecFortifyHack: @ Autohook to 0x0802F184.
ldrb r0, [ r4, #0x0C ]
blh GetUnit, r1
ldr r1, =UsingSpellMenu
ldrb r1, [ r1 ]
cmp r1, #0x00
bne ExecFortifyUsingSpell
	ldrb r1, [ r4, #0x12 ]
	lsl r1, r1, #0x01
	add r0, r0, #0x1E
	add r0, r0, r1
	ldrh r1, [ r0 ]
	b EndExecFortify
ExecFortifyUsingSpell:
ldr r1, =SelectedSpell
ldrb r1, [ r1 ]
EndExecFortify:
ldr r0, =#0x0802F195
bx r0

.ltorg
.align

.global GaidenStaffInventoryHack
.type GaidenStaffInventoryHack, %function
GaidenStaffInventoryHack: @ Autohook to 0x0802CC80. Prevent staff use updating inventory.
ldr r0, =UsingSpellMenu
ldrb r0, [ r0 ]
cmp r0, #0x00
bne SkipStaffInventory
	@ Vanilla behavior.
	mov r5, r4
	add r5, r5, #0x48
	ldrh r0, [ r5 ]
	blh GetItemAttributes, r1
	mov r1, #0x04
	and r1, r0, r1
	cmp r1, #0x00
	beq StaffInventoryDontDecItem
		mov r1, r4
		add r1, r1, #0x7D
		mov r0, #0x01
		strb r0, [ r1 ]
		ldrh r0, [ r5 ]
	StaffInventoryDontDecItem:
	ldr r1, =#0x0802CC9D
	bx r1
SkipStaffInventory:
ldr r0, =#0x0802CCB3
bx r0

.ltorg
.align

.global GaidenTargetSelectionBPressHack
.type GaidenTargetSelectionBPressHack, %function
GaidenTargetSelectionBPressHack: @ Autohook to 0x08022780. Unset spell variables when B pressing on target selection.
ldsb r1, [ r2, r1 ]
ldrb r2, [ r2, #0x11 ]
lsl r2, r2, #0x18
asr r2, r2, #0x18
blh EnsureCameraOntoPosition, r3
bl GaidenZeroOutSpellVariables
mov r0, #0x19
pop { r1 }
bx r1

.ltorg
.align
