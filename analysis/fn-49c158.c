
/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */
/* WARNING: Type propagation algorithm not settling */

void FUN_0049c080(int param_1,HWND param_2,UINT param_3,uint param_4,undefined4 param_5)

{
  byte bVar1;
  byte bVar2;
  code *pcVar3;
  BOOL BVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  byte *pbVar8;
  int iVar9;
  int *in_ECX;
  undefined2 *puVar10;
  undefined4 *puVar11;
  int *piVar12;
  uint uVar13;
  int *piVar14;
  uint uVar15;
  UINT Msg;
  WPARAM wParam;
  undefined4 uVar16;
  DWORD DVar17;
  int local_3bb8;
  byte *pbStack_3bb4;
  byte *pbStack_3bb0;
  int iStack_3bac;
  HWND local_3ba8;
  int *local_3ba4;
  undefined2 *puStack_3ba0;
  byte *pbStack_3b9c;
  int iStack_3b98;
  uint uStack_3b94;
  DWORD local_3b90;
  undefined4 uStack_3b8c;
  uint uStack_3b88;
  undefined1 uStack_3b84;
  undefined1 auStack_3b83 [125];
  char cStack_3b06;
  char cStack_3b05;
  undefined2 uStack_3b04;
  undefined1 uStack_390a;
  undefined1 uStack_3909;
  undefined2 uStack_3904;
  byte abStack_3901 [4349];
  undefined2 uStack_2804;
  ushort auStack_2802 [5119];
  uint local_4;
  
  local_4 = DAT_00650674 ^ (uint)&local_3bb8;
  local_3bb8 = param_1;
  local_3ba8 = param_2;
  if (((param_1 == 0) || (*(int *)(param_1 + 4) == 0)) || (in_ECX[4] == 0)) goto LAB_0049cc73;
  local_3ba4 = in_ECX;
  BVar4 = IsWindow(param_2);
  if (BVar4 == 0) {
    param_2 = (HWND)0x0;
    local_3ba8 = (HWND)0x0;
  }
  iVar9 = *(int *)(in_ECX[4] + 0x24);
  bVar1 = *(byte *)(iVar9 + 0x2f94);
  in_ECX[0xa6] = (uint)bVar1;
  in_ECX[0xa5] = (uint)bVar1;
  DVar17 = DAT_00659b04;
  pcVar3 = *(code **)(*in_ECX + 0x90);
  local_3b90 = DAT_00659b04;
  in_ECX[0xa0] = (int)param_2;
  (*pcVar3)();
  uStack_3b8c = 0;
  iStack_3b98 = DAT_00659b10;
  if (in_ECX[0x8f] == 0) {
    (*DAT_0065e3e4)(L"CDevG5KB::ApplySetting: hWnd=%x, nFlag=%x, nData=%x, nFw=%d, nMatrixLen=%d, bApplyNoDev=%d"
                    ,param_2,param_4,param_5,*(undefined4 *)(in_ECX[4] + 0x18),in_ECX[0xa5],
                    DAT_00659b10);
  }
  if (param_2 != (HWND)0x0) {
    PostMessageW(param_2,param_3,0x14,0);
  }
  uVar15 = 1;
  in_ECX[7] = 1;
  Sleep(DVar17);
  if (((param_4 & 0x20) == 0) || (in_ECX[8] = 1, *(char *)(iVar9 + 0x2d90) == '\0')) {
LAB_0049c1de:
    if ((param_4 & 1) != 0) {
      abStack_3901[0x7fd] = 0;
      _memset(abStack_3901 + 0x7fe,0,0x8ff);
      uStack_2804._0_1_ = 0;
      _memset(abStack_3901 + 0x10fe,0,0x27ff);
      uStack_3b94 = *(uint *)(*(int *)(in_ECX[4] + 0x24) + 4);
      iStack_3bac = 0;
      if (0x2800 < uStack_3b94) {
        MessageBoxW(local_3ba8,L"nMacroBufferSize > sizeof(bMacro)",L"",0);
      }
      iVar5 = FUN_0049b420(in_ECX,local_3bb8,abStack_3901 + 0x10fd,&iStack_3bac);
      if ((iVar5 == 0) && ((*DAT_0065e3e4)(L"FillMatrix Err"), iStack_3b98 == 0)) goto LAB_0049cc28;
      (*DAT_0065e3e4)(L"Matrix (Save)-------------------------");
      puStack_3ba0 = (undefined2 *)0x0;
      pbStack_3bb0 = (byte *)0x4;
      do {
        if (((*(uint *)(iVar9 + 0x2d54) & uVar15) == 0) &&
           ((*(uint *)(local_3bb8 + 0x30) & uVar15) != 0)) {
          iVar5 = FUN_0049c040();
          puStack_3ba0 = (undefined2 *)((int)puStack_3ba0 + iVar5);
        }
        puVar10 = puStack_3ba0;
        uVar15 = uVar15 << 1 | (uint)((int)uVar15 < 0);
        pbStack_3bb0 = pbStack_3bb0 + -1;
      } while (pbStack_3bb0 != (byte *)0x0);
      (*DAT_0065e3e4)(L"nKeyDirty=0x%x, nMacroNum=%d, nNeedWriteMacro=%d",
                      *(undefined4 *)(local_3bb8 + 0x30),iStack_3bac,puStack_3ba0);
      if (0 < (int)puVar10) {
        iVar5 = 0;
        pbStack_3bb4 = (byte *)(local_3bb8 + 0x38);
        uVar15 = 1;
        do {
          if ((((*(uint *)(iVar9 + 0x2d54) & uVar15) == 0) && (iVar5 != *(int *)(local_3bb8 + 0x34))
              ) && (iVar6 = FUN_0049c040(), 0 < iVar6)) {
            *(uint *)(local_3bb8 + 0x30) = *(uint *)(local_3bb8 + 0x30) | uVar15;
          }
          pbStack_3bb4 = pbStack_3bb4 + 0x900;
          iVar5 = iVar5 + 1;
          uVar15 = uVar15 << 1 | (uint)((int)uVar15 < 0);
        } while (iVar5 < 4);
      }
      FUN_0049b8b0(abStack_3901 + 0x7fd,*(undefined4 *)(iVar9 + 0x2d54),
                   *(undefined4 *)(local_3bb8 + 0x30));
      iVar5 = in_ECX[0xa5];
      pbStack_3bb4 = abStack_3901 + 0x7fd;
      iVar6 = 0;
      do {
        uVar15 = 1 << ((byte)iVar6 & 0x1f);
        if (((*(uint *)(iVar9 + 0x2d54) & uVar15) == 0) &&
           ((*(uint *)(local_3bb8 + 0x30) & uVar15) != 0)) {
          iVar7 = (**(code **)(*in_ECX + 0x5c))(iVar6,pbStack_3bb4,iVar5 * 4);
          if ((iVar7 == 0) && ((*DAT_0065e3e4)(L"SetMatrix Err, layer=%d",iVar6), iStack_3b98 == 0))
          goto LAB_0049cc28;
          iVar7 = (**(code **)(*in_ECX + 0x94))();
          if (iVar7 != 0) goto LAB_0049c555;
          Sleep(0x32);
        }
        uVar15 = uStack_3b94;
        pbStack_3bb4 = pbStack_3bb4 + iVar5 * 4;
        iVar6 = iVar6 + 1;
      } while (iVar6 < 4);
      *(undefined4 *)(local_3bb8 + 0x30) = 0;
      if ((0 < iStack_3bac) && (0 < (int)puStack_3ba0)) {
        if (DAT_0065e3e0 != 0) {
          (*DAT_0065e3e4)(L"Macro data: (total size=%d)",uStack_3b94);
          FUN_00407cd0(abStack_3901 + 0x10fd,uVar15,0x20,0);
          pbStack_3bb4 = (byte *)0x0;
          if (0 < iStack_3bac) {
            do {
              uStack_3904 = 0;
              uVar13 = (uint)*(ushort *)(abStack_3901 + (int)pbStack_3bb4 * 4 + 0x10ff);
              uVar15 = (uint)*(ushort *)(abStack_3901 + (int)pbStack_3bb4 * 4 + 0x10fd);
              FUN_00407ff0(L"Macro[%d]: addr=0x%x(%d), size=0x%x(%d include name size), name=",
                           pbStack_3bb4,uVar15,uVar15,uVar13,uVar13);
              pbVar8 = (byte *)(uint)abStack_3901[uVar15 + 0x10fd];
              pbStack_3b9c = abStack_3901 + uVar15 + 0x10fe;
              pbStack_3bb0 = pbVar8;
              FUN_00408040(pbStack_3b9c);
              FUN_00407ff0(L", data=\n");
              FUN_004080e0(pbVar8 + (int)pbStack_3b9c,(uVar13 - (int)pbVar8) + -1);
              (*DAT_0065e3e4)(&uStack_3904);
              pbStack_3bb4 = pbStack_3bb4 + 1;
            } while ((int)pbStack_3bb4 < iStack_3bac);
          }
        }
        Sleep(local_3b90);
        piVar12 = local_3ba4;
        iVar5 = (**(code **)(*local_3ba4 + 100))(abStack_3901 + 0x10fd,uStack_3b94);
        if ((iVar5 == 0) && (iStack_3b98 == 0)) goto LAB_0049cc28;
        iVar5 = (**(code **)(*piVar12 + 0x94))();
        if (iVar5 != 0) {
LAB_0049c555:
          wParam = 0;
          Msg = 0x42d;
          goto LAB_0049cc1a;
        }
        Sleep(0x32);
        in_ECX = local_3ba4;
      }
    }
    uStack_3b88 = (uint)((uint)*(byte *)(iVar9 + 0x2ebc) == *(uint *)(local_3bb8 + 0x243c));
    if ((((param_4 & 0x10) != 0) && (uStack_3b88 != 0)) || ((param_4 & 0x20) != 0)) {
      pbStack_3bb0 = (byte *)(uint)*(byte *)(iVar9 + 0x2f9d);
      bVar1 = *(byte *)(iVar9 + 0x2f9b);
      bVar2 = *(byte *)(iVar9 + 0x2f9c);
      uStack_3904 = uStack_3904 & 0xff00;
      _memset((void *)((int)&uStack_3904 + 1),0,0x761);
      (*DAT_0065e3e4)(L"bDirtyFlag = 0x%x",*(undefined4 *)(local_3bb8 + 0x2444));
      iStack_3bac = 0;
      if (*(char *)(iVar9 + 0x2f8c) != '\0') {
        puStack_3ba0 = &uStack_3904;
        pbStack_3bb0 = (byte *)(&uStack_3904 + (int)pbStack_3bb0 * 0x3f);
        pbStack_3bb4 = (byte *)(&uStack_3904 + (uint)bVar1 * 0x3f);
        pbStack_3b9c = (byte *)(&uStack_3904 + (uint)bVar2 * 0x3f);
        uStack_3b94 = 0x912;
        do {
          piVar12 = local_3ba4;
          if ((*(uint *)(local_3bb8 + 0x2444) & 1 << ((byte)iStack_3bac & 0x1f)) != 0) {
            iVar5 = 0;
            piVar14 = (int *)(iVar9 + 0x18);
            if (0 < *(int *)(iVar9 + 0x2d28)) {
              do {
                if (*piVar14 != 0) {
                  uVar16 = *(undefined4 *)(local_3bb8 + (iVar5 + uStack_3b94) * 4);
                  uVar15 = (uint)*(byte *)((int)piVar14 + 0xd);
                  if ((int)uVar15 < local_3ba4[0xa6]) {
                    pbStack_3bb4[uVar15] = (byte)uVar16;
                    pbStack_3b9c[uVar15] = (byte)((uint)uVar16 >> 8);
                    pbStack_3bb0[uVar15] = (byte)((uint)uVar16 >> 0x10);
                  }
                }
                iVar5 = iVar5 + 1;
                piVar14 = piVar14 + 4;
              } while (iVar5 < *(int *)(iVar9 + 0x2d28));
            }
            puVar10 = puStack_3ba0;
            if (DAT_0065e3e0 != 0) {
              (*DAT_0065e3e4)(L"ColorGroup[%d]: ",iStack_3bac);
              puVar10 = puStack_3ba0;
              FUN_00407cd0(puStack_3ba0,0x40,0,0);
              FUN_00407cd0(puVar10 + 0x3f,0x40,0,0);
              FUN_00407cd0(puVar10 + 0x7e,0x40,0,0);
            }
            Sleep(local_3b90);
            iVar5 = (**(code **)(*piVar12 + 0x74))(iStack_3bac,puVar10,0x17a);
            if ((iVar5 == 0) && (piVar12 = local_3ba4, iStack_3b98 == 0)) goto LAB_0049cc28;
            iVar5 = (**(code **)(*piVar12 + 0x94))();
            if (iVar5 != 0) goto LAB_0049ca0b;
          }
          uStack_3b94 = uStack_3b94 + 0x90;
          pbStack_3bb4 = pbStack_3bb4 + 0x17a;
          pbStack_3bb0 = pbStack_3bb0 + 0x17a;
          puStack_3ba0 = puStack_3ba0 + 0xbd;
          pbStack_3b9c = pbStack_3b9c + 0x17a;
          iStack_3bac = iStack_3bac + 1;
          in_ECX = local_3ba4;
        } while (iStack_3bac < (int)(uint)*(byte *)(iVar9 + 0x2f8c));
      }
      *(undefined4 *)(local_3bb8 + 0x2444) = 0;
    }
    if ((param_4 & 0x102) == 0) {
LAB_0049cc20:
      uStack_3b8c = 1;
    }
    else {
      if ((uStack_3b88 == 0) && ((param_4 & 0x40) != 0)) {
        uStack_3904 = uStack_3904 & 0xff00;
        _memset((void *)((int)&uStack_3904 + 1),0,0x3ff);
        pbStack_3bb0 = (byte *)(iVar9 + 0x2fd6);
        pbStack_3b9c = (byte *)0x0;
        puVar11 = (undefined4 *)(local_3bb8 + 0x2f90);
        do {
          if ((*(uint *)(iVar9 + 0x2d44) & 1 << ((byte)pbStack_3b9c & 0x1f)) == 0) {
            uVar16 = puVar11[-1];
            iVar5 = (uint)*pbStack_3bb0 * 0x15 + -0x3904;
            *(char *)((int)&uStack_3904 + (uint)*(byte *)(iVar9 + 0x2f98) + iVar5 + 0x3904) =
                 (char)uVar16;
            *(char *)((int)&uStack_3904 + (uint)*(byte *)(iVar9 + 0x2f99) + iVar5 + 0x3904) =
                 (char)((uint)uVar16 >> 8);
            *(char *)((int)&uStack_3904 + (uint)*(byte *)(iVar9 + 0x2f9a) + iVar5 + 0x3904) =
                 (char)((uint)uVar16 >> 0x10);
            uVar16 = *puVar11;
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f98) + iVar5 + 0x3904] = (byte)uVar16;
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f99) + iVar5 + 0x3904] =
                 (byte)((uint)uVar16 >> 8);
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f9a) + iVar5 + 0x3904] =
                 (byte)((uint)uVar16 >> 0x10);
            uVar16 = puVar11[1];
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f98) + iVar5 + 0x3907] = (byte)uVar16;
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f99) + iVar5 + 0x3907] =
                 (byte)((uint)uVar16 >> 8);
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f9a) + iVar5 + 0x3907] =
                 (byte)((uint)uVar16 >> 0x10);
            uVar16 = puVar11[2];
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f98) + iVar5 + 0x390a] = (byte)uVar16;
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f99) + iVar5 + 0x390a] =
                 (byte)((uint)uVar16 >> 8);
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f9a) + iVar5 + 0x390a] =
                 (byte)((uint)uVar16 >> 0x10);
            uVar16 = puVar11[3];
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f98) + iVar5 + 0x390d] = (byte)uVar16;
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f99) + iVar5 + 0x390d] =
                 (byte)((uint)uVar16 >> 8);
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f9a) + iVar5 + 0x390d] =
                 (byte)((uint)uVar16 >> 0x10);
            uVar16 = puVar11[4];
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f98) + iVar5 + 0x3910] = (byte)uVar16;
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f99) + iVar5 + 0x3910] =
                 (byte)((uint)uVar16 >> 8);
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f9a) + iVar5 + 0x3910] =
                 (byte)((uint)uVar16 >> 0x10);
            uVar16 = puVar11[5];
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f98) + iVar5 + 0x3913] = (byte)uVar16;
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f99) + iVar5 + 0x3913] =
                 (byte)((uint)uVar16 >> 8);
            abStack_3901[(uint)*(byte *)(iVar9 + 0x2f9a) + iVar5 + 0x3913] =
                 (byte)((uint)uVar16 >> 0x10);
          }
          pbStack_3bb0 = pbStack_3bb0 + 0x28;
          pbStack_3b9c = pbStack_3b9c + 1;
          puVar11 = puVar11 + 9;
        } while ((int)pbStack_3b9c < 0x13);
        Sleep(0x78);
        FUN_00407e70(&uStack_3904,0x1a4,0x15,3);
        if (*(char *)(iVar9 + 0x2f95) == '\0') {
          uVar16 = 0x1a4;
          puVar10 = &uStack_3904;
        }
        else {
          uStack_3b04._0_1_ = 0;
          _memset((undefined1 *)((int)&uStack_3b04 + 1),0,0x1ff);
          _memcpy(&uStack_3b04,&uStack_3904,0x1a4);
          uStack_390a = 0x5a;
          uStack_3909 = 0xa5;
          uVar16 = 0x200;
          puVar10 = &uStack_3b04;
        }
        iVar5 = (**(code **)(*in_ECX + 0x7c))(puVar10,uVar16);
        iVar6 = (**(code **)(*in_ECX + 0x94))();
        if (iVar6 != 0) {
          wParam = 0;
          Msg = 0x42d;
          goto LAB_0049cc1a;
        }
        if ((iVar5 == 0) && ((*DAT_0065e3e4)(L"SetLedRgbTab error"), iStack_3b98 == 0))
        goto LAB_0049cc28;
      }
      Sleep(local_3b90);
      uStack_3b84 = 0;
      _memset(auStack_3b83,0,0x7f);
      iVar5 = 0;
      if (iStack_3b98 == 0) {
        while( true ) {
          _memset(&uStack_3b84,0,0x80);
          iVar6 = (**(code **)(*in_ECX + 0x70))(&uStack_3b84,0x80);
          if (iVar6 == 0) break;
          iVar6 = (**(code **)(*in_ECX + 0x94))();
          if (iVar6 != 0) goto LAB_0049ca0b;
          if (DAT_0065e3e0 != 0) {
            FUN_00407cd0(&uStack_3b84,0x80,10,L"Cfg(Load): ");
          }
          if ((cStack_3b06 == 'Z') && (cStack_3b05 == -0x5b)) goto LAB_0049cb60;
          iVar5 = iVar5 + 1;
          if (4 < iVar5) {
            (*DAT_0065e3e4)(L"!!!Failed, Get wrong led profile data.");
            goto LAB_0049cc28;
          }
          (*DAT_0065e3e4)(L"!!!Get wrong led profile data, retry now.");
          Sleep(100);
        }
        (*DAT_0065e3e4)(L"GetLED failed");
      }
      else {
LAB_0049cb60:
        if (*(char *)(iVar9 + 0x2d8f) == '\x01') {
          FUN_0049b960(local_3bb8,param_4);
        }
        else {
          FUN_0049bc50(in_ECX,local_3bb8,param_4);
        }
        Sleep((-(uint)((param_4 & 0x40) != 0) & 0x28) + 0x3c);
        if (DAT_0065e3e0 != 0) {
          FUN_00407cd0(&uStack_3b84,0x80,10,L"Cfg(Save): ");
        }
        iVar9 = (**(code **)(*in_ECX + 0x6c))(&uStack_3b84,0x80);
        if ((iVar9 != 0) || (iStack_3b98 != 0)) {
          iVar9 = (**(code **)(*in_ECX + 0x94))();
          if (iVar9 == 0) {
            if (local_3ba8 == (HWND)0x0) goto LAB_0049cc20;
            wParam = 0x28;
            Msg = param_3;
          }
          else {
            wParam = 0;
            Msg = 0x42d;
          }
LAB_0049cc1a:
          PostMessageW(local_3ba8,Msg,wParam,0);
          goto LAB_0049cc20;
        }
      }
    }
  }
  else {
    (*DAT_0065e3e4)(L"Reset using cmd");
    iVar5 = (**(code **)(*in_ECX + 0x34))(0);
    if ((iVar5 != 0) || (iStack_3b98 != 0)) {
      param_4 = 0;
      goto LAB_0049c1de;
    }
  }
LAB_0049cc28:
  local_3ba4[7] = 0;
  local_3ba4[8] = 0;
  if (local_3ba8 == (HWND)0x0) {
    DVar17 = 0x1e;
  }
  else {
    PostMessageW(local_3ba8,param_3,100,0);
    DVar17 = 500;
  }
  Sleep(DVar17);
  (**(code **)(*local_3ba4 + 0x90))();
LAB_0049cc73:
  __security_check_cookie(local_4 ^ (uint)&local_3bb8);
  return;
LAB_0049ca0b:
  wParam = 0;
  Msg = 0x42d;
  goto LAB_0049cc1a;
}

