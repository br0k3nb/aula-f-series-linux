
/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */
/* WARNING: Type propagation algorithm not settling */

void FUN_0049b420(int *param_1,int param_2,void *param_3,int *param_4)

{
  size_t _Size;
  ushort uVar1;
  undefined4 uVar2;
  void *pvVar3;
  int iVar4;
  int in_ECX;
  uint uVar5;
  int *in_EDX;
  int iVar6;
  int iVar7;
  short sVar8;
  uint *puVar9;
  int iVar10;
  byte *pbVar11;
  uint _Size_00;
  int local_21d4;
  uint local_21d0;
  int local_21cc;
  uint *local_21c8;
  uint *local_21c4;
  int local_21c0;
  byte *pbStack_21bc;
  int local_21b8;
  int *local_21b4;
  undefined1 *puStack_21b0;
  int iStack_21ac;
  int *local_21a8;
  int local_21a4;
  int *local_21a0;
  void *local_219c;
  undefined4 local_2198;
  ushort auStack_2194 [200];
  undefined1 uStack_2004;
  undefined1 auStack_2003 [8191];
  uint local_4;
  
  local_4 = DAT_00650674 ^ (uint)&local_21d4;
  local_219c = param_3;
  local_21b4 = param_1;
  local_21a0 = param_4;
  local_21cc = in_ECX;
  if (((param_2 != 0) && (in_ECX != 0)) && (param_1[4] != 0)) {
    local_21a4 = *in_EDX;
    iVar7 = param_1[0xa5];
    iVar4 = *(int *)(param_1[4] + 0x24);
    iVar10 = iVar4 + 0x18;
    local_2198 = 0;
    local_21c0 = iVar10;
    local_21a8 = in_EDX;
    if (*(char *)(iVar4 + 0x2f96) != '\0') {
      Sleep(0x1e);
    }
    puVar9 = (uint *)(param_2 + 0x38);
    local_21b8 = iVar7 * 4;
    local_21d0 = 0;
    local_21d4 = local_21cc;
    local_21c4 = puVar9;
    do {
      local_21c8 = puVar9;
      if ((*(uint *)(iVar10 + 0x2d3c) & 1 << ((byte)local_21d0 & 0x1f)) == 0) {
        iVar7 = 0;
        pbVar11 = (byte *)(iVar10 + 0xc);
        do {
          if (*puVar9 != 0) {
            iVar4 = (**(code **)(*param_1 + 0x98))(puVar9,param_1[4]);
            uVar5 = (uint)*pbVar11;
            if ((iVar4 == -1) || (0x8f < uVar5)) {
              FUN_00449660(L"Invaild keycode, Layer=%d, Key[%d](Major=%x, Minor=%x, Combo{modify=%x, vk1=%x, vk2=%x}, Parameter=%x), MatrixInx=%d, "
                           ,local_21d0,iVar7,(int)*puVar9 >> 0x18 & 0xff,*puVar9 & 0xffffff,
                           (char)puVar9[1],*(undefined1 *)((int)puVar9 + 5),
                           *(undefined1 *)((int)puVar9 + 6),puVar9[2],uVar5);
              goto LAB_0049b680;
            }
            *(int *)(local_21d4 + uVar5 * 4) = iVar4;
          }
          iVar7 = iVar7 + 1;
          puVar9 = puVar9 + 4;
          pbVar11 = pbVar11 + 0x10;
        } while (iVar7 < 0x90);
        iVar10 = local_21c0;
        if (*(char *)(local_21c0 + 0x2f7d) != '\0') {
          *(undefined4 *)(local_21d4 + -4 + local_21b8) = 0xa55a0000;
        }
      }
      local_21d4 = local_21d4 + local_21b8;
      local_21d0 = local_21d0 + 1;
      puVar9 = local_21c8 + 0x240;
    } while ((int)local_21d0 < 4);
    uStack_2004 = 0;
    local_21c8 = puVar9;
    _memset(auStack_2003,0,0x1fff);
    auStack_2194[0] = 0;
    _memset(auStack_2194 + 1,0,0x18e);
    iVar7 = 0;
    puStack_21b0 = &uStack_2004;
    iStack_21ac = 1;
    local_21d0 = 0;
    local_21d4 = 0;
    local_21c8 = local_21c4;
    do {
      iVar4 = local_21d4;
      if ((*(uint *)(local_21c0 + 0x2d3c) & 1 << ((byte)local_21d4 & 0x1f)) == 0) {
        iVar10 = 0;
        pbStack_21bc = (byte *)(local_21c0 + 0xc);
        local_21c4 = local_21c8;
        do {
          if (*(char *)((int)local_21c4 + 3) == '\x05') {
            iVar6 = DAT_006709a8;
            if (local_21c4[1] != 0xffff0100) {
              iVar6 = FUN_00479ca0(*(undefined4 *)(DAT_006709a8 + 0x10),local_21c4[1]);
            }
            if ((iVar6 == 0) || (iVar6 = *(int *)(iVar6 + 0x14), iVar6 == 0)) {
              (*DAT_0065e3e4)(L"Key[%d][%d] Set To Macro NULL",iVar4,iVar10);
            }
            else {
              (*DAT_0065e3e4)(L"Key[%d][%d] Set To Macro %s(0x%08x), Macro_Buffer_ID=%d",iVar4,
                              iVar10,iVar6 + 0x14,*(undefined4 *)(iVar6 + 4),
                              *(undefined4 *)(iVar6 + 0x50));
              uVar5 = (uint)*pbStack_21bc;
              if (uVar5 < 0x90) {
                if (*(int *)(iVar6 + 0x50) < 1) {
                  if (local_21a4 <= (int)(local_21d0 & 0xffff)) {
                    (*DAT_0065e3e4)(L"!!!!Macro buffer too small");
                    iVar4 = local_21d4;
                    break;
                  }
                  *(int *)(iVar6 + 0x50) = iStack_21ac;
                  iStack_21ac = iStack_21ac + 1;
                  (*DAT_0065e3e4)(L"New Assign Buffer_ID= %d",*(undefined4 *)(iVar6 + 0x50));
                  iVar4 = FUN_0049b050(local_21b4,puStack_21b0);
                  auStack_2194[iVar7 * 2] = (short)local_21d0;
                  local_21d0 = local_21d0 + iVar4;
                  auStack_2194[iVar7 * 2U + 1] = (short)iVar4;
                  iVar7 = iVar7 + 1;
                  puStack_21b0 = puStack_21b0 + iVar4;
                }
                *(undefined1 *)(local_21cc + 3 + uVar5 * 4) = 0;
                puVar9 = (uint *)(local_21cc + uVar5 * 4);
                *puVar9 = *puVar9 | *(int *)(iVar6 + 0x50) * 0x1000000 - 0x1000000U;
                iVar4 = local_21d4;
              }
              else {
                MessageBoxW((HWND)0x0,L"Matrix inx warning",L"Warning",0);
                iVar4 = local_21d4;
              }
            }
          }
          pbStack_21bc = pbStack_21bc + 0x10;
          iVar10 = iVar10 + 1;
          local_21c4 = local_21c4 + 4;
        } while (iVar10 < 0x90);
      }
      pvVar3 = local_219c;
      local_21cc = local_21cc + local_21b8;
      local_21c8 = local_21c8 + 0x240;
      local_21d4 = iVar4 + 1;
    } while (local_21d4 < 4);
    if (0 < iVar7) {
      iVar10 = 0;
      iVar6 = 0;
      iVar4 = 0;
      local_21b4 = (int *)0x0;
      _Size = iVar7 * 4;
      sVar8 = (short)_Size;
      if (1 < iVar7) {
        do {
          uVar1 = auStack_2194[iVar4 * 2U + 1];
          auStack_2194[iVar4 * 2] = auStack_2194[iVar4 * 2] + sVar8;
          auStack_2194[iVar4 * 2U + 2] = auStack_2194[iVar4 * 2U + 2] + sVar8;
          iVar10 = iVar10 + (uint)uVar1;
          uVar2 = iVar4 * 2;
          iVar4 = iVar4 + 2;
          iVar6 = iVar6 + (uint)auStack_2194[uVar2 + 3];
        } while (iVar4 < iVar7 + -1);
      }
      uVar5 = 0;
      if (iVar4 < iVar7) {
        auStack_2194[iVar4 * 2] = auStack_2194[iVar4 * 2] + sVar8;
        uVar5 = (uint)auStack_2194[iVar4 * 2U + 1];
      }
      _Size_00 = local_21d0 & 0xffff;
      if (iVar6 + iVar10 + uVar5 == _Size_00) {
        _memcpy(local_219c,auStack_2194,_Size);
        _memcpy((void *)((int)pvVar3 + _Size),&uStack_2004,_Size_00);
        *local_21a8 = _Size_00 + _Size;
        if (local_21a0 != (int *)0x0) {
          *local_21a0 = iVar7;
        }
      }
      else {
        (*DAT_0065e3e4)(L"!!!!nTotalSize!=nAddr");
      }
    }
    if (DAT_006709a8 != 0) {
      FUN_00479f80(*(undefined4 *)(DAT_006709a8 + 0x10));
    }
  }
LAB_0049b680:
  __security_check_cookie(local_4 ^ (uint)&local_21d4);
  return;
}

