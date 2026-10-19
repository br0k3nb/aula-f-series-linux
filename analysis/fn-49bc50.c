
undefined4 FUN_0049bc50(int param_1,int param_2,uint param_3)

{
  char cVar1;
  int iVar2;
  byte bVar3;
  int iVar4;
  undefined1 uVar5;
  int iVar6;
  int unaff_EBX;
  uint uVar7;
  byte *pbVar8;
  undefined1 *puVar9;
  int iVar10;
  char *pcVar11;
  byte *pbVar12;
  int iStack_c;
  
  iVar2 = *(int *)(*(int *)(param_1 + 0x10) + 0x24);
  bVar3 = *(byte *)(param_2 + 0x243c);
  iVar10 = iVar2 + 0x18;
  if (((param_3 & 0x20) != 0) && (*(int *)(iVar2 + 0x2d95) != 0)) {
    (*DAT_0065e3e4)(L"ResetItem:");
    pcVar11 = (char *)(iVar2 + 0x2d96);
    iStack_c = 2;
    do {
      uVar7 = (uint)(byte)pcVar11[-1];
      cVar1 = *pcVar11;
      if ((uVar7 != 0) || (cVar1 != '\0')) {
        (*DAT_0065e3e4)(L"Cfg[%d]=%02x",uVar7,cVar1);
        *(char *)(unaff_EBX + uVar7) = cVar1;
      }
      pcVar11 = pcVar11 + 2;
      iStack_c = iStack_c + -1;
    } while (iStack_c != 0);
  }
  if ((param_3 & 2) != 0) {
    *(bool *)(unaff_EBX + 9) = *(byte *)(iVar2 + 0x2ebc) == bVar3;
    *(undefined1 *)(unaff_EBX + 10) = *(undefined1 *)(iVar2 + (uint)bVar3 * 0x28 + 0x2fd6);
    pbVar12 = (byte *)(unaff_EBX + 0x3b);
    *(char *)(unaff_EBX + 0xb) = *(char *)(iVar2 + 0x2f8e) + *(char *)(param_2 + 0x2440);
    *(undefined1 *)(unaff_EBX + 0x39) = 0xff;
    *(undefined1 *)(unaff_EBX + 0x38) = 0xff;
    pbVar8 = (byte *)(param_2 + 0x2f88);
    param_3 = 0x13;
    do {
      cVar1 = *(char *)(*pbVar8 + 0x2df0 + iVar10);
      if (pbVar8[2] == 0) {
        bVar3 = 0;
      }
      else {
        bVar3 = *(byte *)(iVar2 + 0x2fa1);
      }
      pbVar12[-1] = *(byte *)(pbVar8[1] + 0x2e10 + iVar10);
      *pbVar12 = bVar3 & 0xf | cVar1 << 4;
      pbVar12 = pbVar12 + 2;
      pbVar8 = pbVar8 + 0x24;
      param_3 = param_3 + -1;
    } while (param_3 != 0);
  }
  if (*(int *)(iVar2 + 0x2d40) != 0) {
    *(undefined1 *)(unaff_EBX + 3) = *(undefined1 *)(param_2 + 0x3340);
  }
  if (*(int *)(param_2 + 0x3334) == 0) {
    uVar5 = (undefined1)*(undefined4 *)(param_2 + 0x3338);
  }
  else {
    uVar5 = 0;
  }
  *(undefined1 *)(unaff_EBX + 0x16) = uVar5;
  if (*(char *)(iVar2 + 0x2d58) == '\x01') {
    *(bool *)(unaff_EBX + 0x17) = *(char *)(param_2 + 0x3342) == '\0';
  }
  if (*(short *)(param_2 + 0x333c) == 0) {
    uVar5 = *(undefined1 *)(*(ushort *)(param_2 + 0x333e) + 0x2dd4 + iVar10);
  }
  else {
    uVar5 = 0;
  }
  *(undefined1 *)(unaff_EBX + 0x18) = uVar5;
  if (*(int *)(iVar2 + 0x2d9c) != 0) {
    uVar5 = 0;
    iVar6 = 0;
    do {
      uVar7 = (uint)*(byte *)(iVar2 + 0x2da0 + iVar6);
      if (uVar7 != 0) {
        iVar4 = uVar7 * 0x10;
        if (*(int *)(iVar4 + 0x38 + param_2) != *(int *)(iVar4 + iVar10)) {
          uVar5 = *(undefined1 *)(iVar2 + 0x2d9c);
          break;
        }
        if (((char)((uint)*(int *)(iVar4 + iVar10) >> 0x18) == '\x02') &&
           (*(int *)(iVar4 + 0x3c + param_2) != *(int *)(iVar4 + 4 + iVar10))) {
          uVar5 = *(undefined1 *)(iVar2 + 0x2d9c);
          break;
        }
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < 4);
    (*DAT_0065e3e4)(L"nWheelMode=%d",uVar5);
    *(undefined1 *)(unaff_EBX + 0x1a) = uVar5;
  }
  if (*(char *)(iVar2 + 0x2d81) != '\0') {
    uVar7 = (uint)*(byte *)(param_2 + 0x3344);
    iVar10 = iVar2 + 0x3394 + uVar7 * 0x28;
    iVar6 = param_2 + 0x243c + uVar7 * 0x24;
    iVar4 = *(int *)(iVar6 + 0xf10);
    uVar5 = 0;
    if (*(int *)(*(int *)(param_1 + 0x10) + 0x18) == 0x1a) {
      if (iVar4 != 0xff00) {
        if (iVar4 == 0xff) {
          uVar5 = 1;
        }
        else if (iVar4 == 0xff0000) {
          uVar5 = 2;
        }
        else if (iVar4 == 0xffff) {
          uVar5 = 3;
        }
        else if (iVar4 == 0xffff00) {
          uVar5 = 4;
        }
        else if (iVar4 == 0xff00ff) {
          uVar5 = 5;
        }
        else if (iVar4 == 0xffffff) {
          uVar5 = 6;
        }
      }
      if (*(char *)(iVar6 + 0xf0e) != '\0') {
        uVar5 = 7;
      }
      *(undefined1 *)(unaff_EBX + 0x32) = uVar5;
      puVar9 = (undefined1 *)(param_2 + 0x243c + (uVar7 * 9 + 0x3c3) * 4);
      *(undefined1 *)(unaff_EBX + 0x33) = *(undefined1 *)(iVar6 + 0xf0d);
      *(undefined1 *)(unaff_EBX + 0x34) = *puVar9;
      *(undefined1 *)(unaff_EBX + 0x35) = *(undefined1 *)(iVar10 + 2);
    }
    else {
      if (iVar4 == 0xff00) {
        uVar5 = 1;
      }
      else if (iVar4 == 0xff) {
        uVar5 = 0;
      }
      else if (iVar4 == 0xff0000) {
        uVar5 = 2;
      }
      else if (iVar4 == 0xffff) {
        uVar5 = 3;
      }
      else if (iVar4 == 0xffff00) {
        uVar5 = 5;
      }
      else if (iVar4 == 0xff00ff) {
        uVar5 = 4;
      }
      else if (iVar4 == 0xffffff) {
        uVar5 = 6;
      }
      if (*(char *)(iVar6 + 0xf0e) != '\0') {
        uVar5 = 7;
      }
      *(undefined1 *)(unaff_EBX + 0x12) = *(undefined1 *)(iVar10 + 2);
      *(undefined1 *)(unaff_EBX + 0x13) = uVar5;
      puVar9 = (undefined1 *)(param_2 + 0x243c + (uVar7 * 9 + 0x3c3) * 4);
      *(undefined1 *)(unaff_EBX + 0x14) = *(undefined1 *)(iVar6 + 0xf0d);
      *(undefined1 *)(unaff_EBX + 0x15) = *puVar9;
    }
    (*DAT_0065e3e4)(L"SideLED: mode=%d, nLight=%d, nSpeed=%d, nColor=%d",*(undefined1 *)(iVar10 + 2)
                    ,*(undefined1 *)(iVar6 + 0xf0d),*puVar9,uVar5);
  }
  if ((*(int *)(param_1 + 0x20) != 0) && (uVar7 = *(uint *)(iVar2 + 0x2d64), uVar7 != 0)) {
    *(char *)((uVar7 & 0xff) + unaff_EBX) = (char)(uVar7 >> 8);
    (*DAT_0065e3e4)(L"Cfg[%d] = 0x%x",uVar7 & 0xff,(int)uVar7 >> 8 & 0xff);
  }
  if (*(char *)(iVar2 + 0x2f95) != '\0') {
    *(undefined1 *)(unaff_EBX + 0x7e) = 0x5a;
    *(undefined1 *)(unaff_EBX + 0x7f) = 0xa5;
  }
  return 1;
}

