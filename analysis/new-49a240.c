
undefined4 FUN_0049a240(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  int in_ECX;
  
  if (*(int *)(in_ECX + 0x23c) != 0) {
                    /* WARNING: Could not recover jumptable at 0x0049a254. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar1 = (**(code **)(**(int **)(in_ECX + 0x23c) + 0x7c))();
    return uVar1;
  }
  iVar2 = FUN_004994c0(in_ECX,*(undefined4 *)(in_ECX + 0xc),1,10,0,param_2,0x14);
  if (iVar2 == 0) {
    (*DAT_0065e3e4)(L"SetLedRgbTab failed");
    return 0;
  }
  return 1;
}

