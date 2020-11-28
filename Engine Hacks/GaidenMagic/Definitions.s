
.include "FE-CLib-master/reference/FE8U-20190316.s"

@ Vanilla function declarations:

SET_FUNC GetUnitRangeMask, (0x080171E8+1)

SET_FUNC CanUnitUseWeapon, (0x8016750+1)

SET_FUNC CanUnitUseStaff, (0x8016800 + 1)

SET_FUNC DrawItemMenuCommand, (0x08016848+1)

SET_FUNC GetWeaponRangeMask, (0x080170D4+1)

SET_FUNC AttackUMEffect, (0x08022B30+1)

SET_FUNC DrawItemRText, (0x08088E60+1)

SET_FUNC RTextUp, (0x08089354+1)

SET_FUNC RTextDown, (0x08089384+1)

SET_FUNC RTextLeft, (0x080893B4+1)

SET_FUNC RTextRight, (0x080893E4+1)


@ Data declarations:

SET_DATA SpellsBuffer, 0x202B6D0

SET_DATA SelectedSpell, 0x0203F080

SET_DATA UsingSpellMenu, 0x0203F082

SET_DATA DidSelectSpell, 0x0203F084

SET_DATA SpellTargetSelection, 0x0859D3F8

SET_DATA StatScreenBufferMap, 0x02003C94

SET_DATA gBG0MapBuffer, 0x02022CA8


@ Autohooks:

SET_FUNC NewGetUnitEquippedWeapon, (0x08016B28+1)

SET_FUNC NewGetUnitEquippedWeaponSlot, (0x08016B58+1)

SET_FUNC NewGetUnitRangeMask, (0x080171E8+1)

SET_FUNC SetUpBattleWeaponDataHack, (0x0802A730+1)

SET_FUNC NewMenuRText, (0x08024588+1)

SET_FUNC NewGetUnitUseFlags, (0x08018B28+1)

SET_FUNC GaidenActionStaffDoorChestUseItemHack, (0x0802FC48+1)

SET_FUNC GaidenPreActionHack, (0x0801D1D0+1)

SET_FUNC GaidenSetupBattleUnitForStaffHack, (0x0802CB24+1)

SET_FUNC GaidenExecStandardHealHack, (0x0802EBB4+1)

SET_FUNC GaidenExecFortifyHack, (0x0802F184+1)
