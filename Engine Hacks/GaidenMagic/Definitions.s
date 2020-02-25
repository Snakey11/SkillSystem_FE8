
.include "FE-CLib-master/reference/FE8U-20190316.s"

@ Vanilla function declarations:

SET_FUNC GetUnitRangeMask, (0x080171E8+1)

SET_FUNC CanUnitUseWeapon, (0x8016750+1)

SET_FUNC DrawItemMenuCommand, (0x08016848+1)

SET_FUNC GetWeaponRangeMask, (0x080170D4+1)


@ Data declarations:

SET_DATA SpellsBuffer, 0x202B6D0

SET_DATA UsingSpellMenu, 0x0203F082

SET_DATA SelectedSpell, 0x0203F080


@ Autohooks:

SET_FUNC NewGetUnitEquippedWeapon, (0x08016B28+1)

SET_FUNC NewGetUnitEquippedWeaponSlot, (0x08016B58+1)

SET_FUNC NewGetUnitRangeMask, (0x080171E8+1)

SET_FUNC SetUpBattleWeaponDataHack, (0x0802A730+1)
