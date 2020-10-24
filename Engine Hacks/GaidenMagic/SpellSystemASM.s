
.thumb

.macro blh to, reg
    ldr \reg, =\to
    mov lr, \reg
    .short 0xF800
.endm

.equ GetUnitEquippedWeaponSlot, 0x08016B58
.equ UsingSpellMenu, 0x0203F082
.equ DefenseStruct, 0x0203A56C

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
ldr r0, =DefenseStruct
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

.global ZeroOutSpellAtBatteEnd
.type ZeroOutSpellAtBatteEnd, %function
ZeroOutSpellAtBatteEnd: @ Autohook to 0x0802C1EC.
push { r4 - r6, lr }
mov r4, r0
mov r5, r1
ldr r2, =SelectedSpell
mov r1, #0x0
str r1, [ r2 ]
ldrb r0, [ r5, #0x8 ]
strb r0, [ r4, #0x8 ]
ldrb r0, [ r5, #0x9 ]
strb r0, [ r4, #0x9 ]
ldrb r0, [ r5, #0x13 ]
strb r0, [ r4, #0x13 ]
ldr r0, [ r5, #0xC ]
str r0, [ r4, #0xC ]
ldr r1, =0x0802C203
bx r1
