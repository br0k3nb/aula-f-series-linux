
void FUN_00411800(void)

{
  char *pcVar1;
  byte bVar2;
  LPCWSTR pWVar3;
  char cVar4;
  void *_Dst;
  UINT UVar5;
  uint uVar6;
  int iVar7;
  byte *pbVar8;
  size_t sVar9;
  undefined4 uVar10;
  int iVar11;
  LPCWSTR in_ECX;
  undefined4 *puVar12;
  int iVar13;
  int *piVar14;
  uint *puVar15;
  int in_EDX;
  int iVar16;
  int iVar17;
  undefined1 *puVar18;
  int *piVar19;
  wchar_t *lpText;
  undefined4 local_d0c;
  LPCWSTR local_d08;
  int local_d04;
  int local_d00;
  int local_cfc [30];
  undefined1 local_c84;
  undefined4 local_c83;
  undefined4 local_c7f;
  undefined4 local_c7b;
  undefined4 local_c77;
  undefined4 local_c73;
  undefined4 local_c6f;
  undefined4 local_c6b;
  undefined2 local_c67;
  undefined1 local_c65;
  int local_c64 [10];
  WCHAR local_c3c [1040];
  WCHAR local_41c [260];
  wchar_t local_214 [262];
  uint local_8;
  
  local_8 = DAT_00650674 ^ (uint)&local_d0c;
  local_c84 = 0;
  local_c83 = 0;
  local_c7f = 0;
  local_c7b = 0;
  local_c77 = 0;
  local_c73 = 0;
  local_c6f = 0;
  local_c6b = 0;
  local_c67 = 0;
  local_c65 = 0;
  local_d08 = in_ECX;
  local_d04 = in_EDX;
  if (*(int *)(in_EDX + 0x24) == 0) {
    _Dst = _malloc(0x3574);
    *(void **)(in_EDX + 0x24) = _Dst;
    if (_Dst == (void *)0x0) goto LAB_00413427;
    _memset(_Dst,0,0x3574);
  }
  iVar11 = *(int *)(in_EDX + 0x24);
  iVar17 = iVar11 + 0x18;
  GetPrivateProfileStringW(L"OPT",L"UID",(LPCWSTR)0x0,(LPWSTR)(in_EDX + 0xb2),0xf,in_ECX);
  UVar5 = GetPrivateProfileIntW(L"OPT",L"FwVerNeedToUpdate",0,in_ECX);
  *(UINT *)(in_EDX + 0x2d8) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ForceUpdate",0,in_ECX);
  *(UINT *)(in_EDX + 0x2dc) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MacroBufferSize",0x1000,in_ECX);
  *(UINT *)(*(int *)(in_EDX + 0x24) + 4) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MaxLoopCnt",0,in_ECX);
  *(UINT *)(*(int *)(in_EDX + 0x24) + 0xc) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MaxDelay",0,in_ECX);
  *(UINT *)(*(int *)(in_EDX + 0x24) + 0x10) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MaxAction",0,in_ECX);
  *(UINT *)(*(int *)(in_EDX + 0x24) + 0x14) = UVar5;
  GetPrivateProfileStringW(L"OPT",L"clrKeyOv",(LPCWSTR)0x0,local_41c,0x104,in_ECX);
  if (local_41c[0] == L'\0') {
    uVar6 = 0xffffff;
  }
  else {
    local_d0c = (int *)((uint)local_d0c & 0xff000000);
    FUN_00465b20(local_41c,3);
    uVar6 = (uint)local_d0c & 0xffffff;
    in_ECX = local_d08;
  }
  *(uint *)(iVar11 + 0x2d18) = uVar6;
  GetPrivateProfileStringW(L"OPT",L"clrKeyDn",(LPCWSTR)0x0,local_41c,0x104,in_ECX);
  if (local_41c[0] == L'\0') {
    uVar6 = 0xffffff;
  }
  else {
    local_d0c = (int *)((uint)local_d0c & 0xff000000);
    FUN_00465b20(local_41c,3);
    uVar6 = (uint)local_d0c & 0xffffff;
    in_ECX = local_d08;
  }
  *(uint *)(iVar11 + 0x2d1c) = uVar6;
  GetPrivateProfileStringW(L"OPT",L"clrKeyHasFunc",(LPCWSTR)0x0,local_41c,0x104,in_ECX);
  if (local_41c[0] == L'\0') {
    uVar6 = 0x1d45e8;
  }
  else {
    local_d0c = (int *)((uint)local_d0c & 0xff000000);
    FUN_00465b20(local_41c,3);
    uVar6 = (uint)local_d0c & 0xffffff;
    in_ECX = local_d08;
  }
  *(uint *)(iVar11 + 0x2d20) = uVar6;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowSleep",1,in_ECX);
  *(char *)(iVar11 + 0x2da4) = (char)UVar5;
  GetPrivateProfileStringW(L"OPT",L"SleepUI",(LPCWSTR)0x0,local_c3c,0x410,in_ECX);
  if (local_c3c[0] == L'\0') {
    cVar4 = '\0';
  }
  else {
    cVar4 = FUN_00465a50(local_c3c,0x10);
    in_ECX = local_d08;
  }
  *(char *)(iVar11 + 0x2da5) = cVar4;
  if (cVar4 == '\0') {
    iVar7 = *(int *)(in_EDX + 0x18);
    *(undefined1 *)(iVar11 + 0x2da5) = 10;
    if (iVar7 == 0x16) {
      iVar7 = 0;
      local_d0c = (int *)(1 - (iVar11 + 0x2dec));
      puVar12 = (undefined4 *)(iVar11 + 0x2dac);
      do {
        pcVar1 = (char *)(iVar7 + 0x2dd4 + iVar17);
        *puVar12 = (char *)((int)local_d0c + (int)pcVar1);
        *pcVar1 = (char)iVar7 + '\x01';
        iVar7 = iVar7 + 1;
        puVar12 = puVar12 + 1;
      } while (iVar7 < (int)(uint)*(byte *)(iVar11 + 0x2da5));
    }
    else {
      local_c64[0] = 0x1e;
      local_c64[1] = 0x3c;
      local_c64[2] = 0x5a;
      local_c64[3] = 0x78;
      local_c64[4] = 0xb4;
      local_c64[5] = 0xf0;
      local_c64[6] = 300;
      local_c64[7] = 600;
      local_c64[8] = 900;
      local_c64[9] = 0x4b0;
      iVar7 = 0;
      local_d0c = (int *)(iVar11 + 0x2dac);
      do {
        iVar13 = local_c64[iVar7];
        piVar14 = local_d0c + 1;
        *local_d0c = iVar13;
        *(char *)(iVar7 + 0x2dd4 + iVar17) =
             ((char)(iVar13 / 0x1e) + (char)(iVar13 >> 0x1f)) -
             (char)((longlong)iVar13 * 0x88888889 >> 0x3f);
        iVar7 = iVar7 + 1;
        local_d0c = piVar14;
      } while (iVar7 < (int)(uint)*(byte *)(iVar11 + 0x2da5));
    }
  }
  else {
    GetPrivateProfileStringW(L"OPT",L"SleepHW",(LPCWSTR)0x0,local_c3c,0x410,in_ECX);
    if (local_c3c[0] == L'\0') {
      uVar6 = 0;
    }
    else {
      uVar6 = FUN_00465b20(local_c3c,0x10);
      in_ECX = local_d08;
    }
    if (uVar6 != *(byte *)(iVar11 + 0x2da5)) {
      MessageBoxW((HWND)0x0,L"SleepUI!=SleepHW",L"Warning",0);
    }
  }
  GetPrivateProfileStringW(L"OPT",L"Speed",(LPCWSTR)0x0,local_c3c,0x410,in_ECX);
  iVar7 = 0;
  if (local_c3c[0] != L'\0') {
    iVar7 = FUN_00465b20(local_c3c,8);
    in_ECX = local_d08;
  }
  *(int *)(iVar11 + 0x2dfc) = iVar7;
  if (iVar7 < 1) {
    local_d0c = (int *)0x4030201;
    *(undefined4 *)(iVar11 + 0x2e00) = 0x4030201;
    *(undefined4 *)(iVar11 + 0x2e08) = 0x4030201;
    *(undefined4 *)(iVar11 + 0x2dfc) = 4;
  }
  else {
    GetPrivateProfileStringW(L"OPT",L"SpeedHW",(LPCWSTR)0x0,local_c3c,0x410,in_ECX);
    if ((local_c3c[0] == L'\0') || (iVar7 = FUN_00465b20(local_c3c,8), iVar7 < 1)) {
      *(undefined4 *)(iVar11 + 0x2e08) = *(undefined4 *)(iVar11 + 0x2e00);
      *(undefined4 *)(iVar11 + 0x2e0c) = *(undefined4 *)(iVar11 + 0x2e04);
    }
    else if (iVar7 != *(int *)(iVar11 + 0x2dfc)) {
      MessageBoxW((HWND)0x0,L"SpeedUI!=SpeedHW",L"Warning",0);
    }
  }
  GetPrivateProfileStringW(L"OPT",L"Light",(LPCWSTR)0x0,local_c3c,0x410,local_d08);
  iVar7 = 0;
  if (local_c3c[0] != L'\0') {
    iVar7 = FUN_00465b20(local_c3c,0x14);
  }
  *(int *)(iVar11 + 0x2e10) = iVar7;
  if (iVar7 < 1) {
    iVar7 = 0;
    do {
      *(char *)((int)local_c64 + iVar7) = (char)iVar7 + '\x01';
      iVar7 = iVar7 + 1;
    } while (iVar7 < 0x14);
    *(int *)(iVar11 + 0x2e14) = local_c64[0];
    *(int *)(iVar11 + 0x2e28) = local_c64[0];
    *(int *)(iVar11 + 0x2e18) = local_c64[1];
    *(int *)(iVar11 + 0x2e2c) = local_c64[1];
    *(int *)(iVar11 + 0x2e1c) = local_c64[2];
    *(int *)(iVar11 + 0x2e30) = local_c64[2];
    *(int *)(iVar11 + 0x2e20) = local_c64[3];
    *(int *)(iVar11 + 0x2e34) = local_c64[3];
    *(int *)(iVar11 + 0x2e24) = local_c64[4];
    *(int *)(iVar11 + 0x2e38) = local_c64[4];
    *(undefined4 *)(iVar11 + 0x2e10) = 0x14;
  }
  else {
    GetPrivateProfileStringW(L"OPT",L"LightHW",(LPCWSTR)0x0,local_c3c,0x410,local_d08);
    if ((local_c3c[0] == L'\0') || (iVar7 = FUN_00465b20(local_c3c,0x14), iVar7 < 1)) {
      *(undefined4 *)(iVar11 + 0x2e28) = *(undefined4 *)(iVar11 + 0x2e14);
      *(undefined4 *)(iVar11 + 0x2e2c) = *(undefined4 *)(iVar11 + 0x2e18);
      *(undefined4 *)(iVar11 + 0x2e30) = *(undefined4 *)(iVar11 + 0x2e1c);
      *(undefined4 *)(iVar11 + 0x2e34) = *(undefined4 *)(iVar11 + 0x2e20);
      *(undefined4 *)(iVar11 + 0x2e38) = *(undefined4 *)(iVar11 + 0x2e24);
    }
    else if (iVar7 != *(int *)(iVar11 + 0x2e10)) {
      MessageBoxW((HWND)0x0,L"LightUI!=LightHW",L"Warning",0);
    }
  }
  pWVar3 = local_d08;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"SideLightNum",5,local_d08);
  *(UINT *)(iVar11 + 0x2e3c) = UVar5;
  FUN_00465ca0(0,L"ptKB");
  GetPrivateProfileStringW(L"OPT",L"LedParam",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if (local_c3c[0] == L'\0') {
LAB_00412664:
    pbVar8 = (byte *)(iVar11 + 0x2fa3);
    iVar7 = 0x18;
    do {
      pbVar8[-1] = *(char *)(iVar11 + 0x2e10) - 1;
      *pbVar8 = (*(char *)(iVar11 + 0x2dfc) + -1) * '\x10' | 7;
      pbVar8 = pbVar8 + 2;
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
  }
  else {
    iVar7 = FUN_00465b20(local_c3c,0x30);
    if (iVar7 < 1) goto LAB_00412664;
    iVar13 = 0;
    if (0 < iVar7 / 2) {
      do {
        if (*(int *)(iVar11 + 0x2e10) + -1 < (int)(uint)*(byte *)(iVar11 + 0x2fa2 + iVar13)) {
          lpText = L"LedParam: wrong light >= pVar->nLightNum";
LAB_0041205c:
          MessageBoxW((HWND)0x0,lpText,L"Error",0);
          break;
        }
        if (*(int *)(iVar11 + 0x2dfc) + -1 < (int)(uint)(*(byte *)(iVar13 + 0x2f8b + iVar17) >> 4))
        {
          lpText = L"LedParam: wrong speed >= pVar->nSpeedNum";
          goto LAB_0041205c;
        }
        iVar13 = iVar13 + 2;
      } while (iVar13 < iVar7 / 2);
    }
  }
  pWVar3 = local_d08;
  GetPrivateProfileStringW(L"OPT",L"UnRectKey",(LPCWSTR)0x0,local_c3c,0x410,local_d08);
  if (((local_c3c[0] != L'\0') && (iVar7 = FUN_00465a50(local_c3c,0x1e), 0 < iVar7)) &&
     (iVar7 = iVar7 + -1, 3 < iVar7 / 2)) {
    iVar16 = 0;
    *(int *)(iVar11 + 0x2e58) = local_cfc[0];
    iVar13 = 1;
    if (1 < iVar7) {
      piVar14 = (int *)(iVar11 + 0x2e60);
      while (local_cfc[iVar13] != 0) {
        piVar14[-1] = local_cfc[iVar13];
        *piVar14 = local_cfc[iVar13 + 1];
        iVar16 = iVar16 + 1;
        piVar14 = piVar14 + 2;
        if ((7 < iVar16) || (iVar13 = iVar13 + 2, iVar7 <= iVar13)) break;
      }
    }
    if ((*(int *)(iVar11 + 0x2e5c) != *(int *)(iVar11 + 0x2e54 + iVar16 * 8)) ||
       (*(int *)(iVar11 + 0x2e60) != *(int *)(iVar11 + 0x2e58 + iVar16 * 8))) {
      MessageBoxW((HWND)0x0,L"EnterPt[0]!=EnterPt[Last] ",L"Error",0);
    }
  }
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowAutoDetect",0,pWVar3);
  *(short *)(iVar11 + 0x2d58) = (short)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"CfgVal",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d64) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"KbLayout",0,pWVar3);
  *(UINT *)(iVar11 + 0x2ea8) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"KbDrawType",0,pWVar3);
  *(UINT *)(iVar11 + 0x2eac) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"FillKeyWhenHasFunc",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d24) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"RespireType",0,pWVar3);
  *(UINT *)(iVar11 + 0x2ea0) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"KbLanguage",0,pWVar3);
  *(UINT *)(iVar11 + 0x2eb0) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"SyncKB",1,pWVar3);
  *(char *)(iVar11 + 0x2f96) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"LightBarMusicCmd",0,pWVar3);
  *(char *)(iVar11 + 0x2f97) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowEffect",0xffffff,pWVar3);
  *(UINT *)(iVar11 + 0x2d34) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowLED",1,pWVar3);
  *(char *)(iVar11 + 0x2d38) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowMusic",1,pWVar3);
  *(char *)(iVar11 + 0x2d39) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowRtEffect",0,pWVar3);
  *(char *)(iVar11 + 0x2d3c) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowPower",0,pWVar3);
  *(char *)(iVar11 + 0x2d3a) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowPowerText",1,pWVar3);
  *(char *)(iVar11 + 0x2d3b) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowDebounce",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d40) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"RandomVal",7,pWVar3);
  *(char *)(iVar11 + 0x2fa1) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MusicUI",0,pWVar3);
  *(UINT *)(iVar11 + 0x2eb4) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MaxFnNum",0x30,pWVar3);
  *(char *)(iVar11 + 0x2f93) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MatrixLen",0x80,pWVar3);
  *(char *)(iVar11 + 0x2f94) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"CRC",1,pWVar3);
  *(char *)(iVar11 + 0x2f95) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"LayerMask",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d54) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"SleepTime",0x78,pWVar3);
  *(UINT *)(iVar11 + 0x2da8) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"LedMask",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d44) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ChannelMask",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d4c) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MusicMask",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d50) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"MaxEftKeyIndex",0,pWVar3);
  *(UINT *)(iVar11 + 0x2e9c) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"DefLedIndex",0,pWVar3);
  *(char *)(iVar11 + 0x2ebb) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"WheelMode",0,pWVar3);
  *(UINT *)(iVar11 + 0x2d9c) = UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"FnAlign",1,pWVar3);
  *(char *)(iVar11 + 0x2d5a) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowTapTip",0,pWVar3);
  *(char *)(iVar11 + 0x2d7f) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowSideLED",0,pWVar3);
  *(char *)(iVar11 + 0x2d81) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowDevTy",1,pWVar3);
  *(char *)(iVar11 + 0x2d82) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"RGB",1,pWVar3);
  *(char *)(iVar11 + 0x2d8b) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"FnNotToSet",1,pWVar3);
  *(char *)(iVar11 + 0x2d8c) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"IC2481",0,pWVar3);
  *(char *)(iVar11 + 0x2d8d) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"XYDirect",1,pWVar3);
  *(char *)(iVar11 + 0x2d8e) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"SubFw",0,pWVar3);
  *(char *)(iVar11 + 0x2d8f) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"CmdReset",0,pWVar3);
  *(char *)(iVar11 + 0x2d90) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"VirtualKB",0,pWVar3);
  *(char *)(iVar11 + 0x2d91) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"RoundGrid",0,pWVar3);
  *(char *)(iVar11 + 0x2d92) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowGameTip",0,pWVar3);
  *(char *)(iVar11 + 0x2d93) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"DefSideLedIndex",0,pWVar3);
  *(char *)(iVar11 + 0x2d80) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"ShowKbOption",0,pWVar3);
  *(char *)(iVar11 + 0x2d94) = (char)UVar5;
  GetPrivateProfileStringW(L"OPT",L"ResetItem",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if (local_c3c[0] != L'\0') {
    FUN_00465b20(local_c3c,4);
  }
  GetPrivateProfileStringW(L"OPT",L"KeyLineStart",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if (local_c3c[0] != L'\0') {
    FUN_00465b20(local_c3c,6);
  }
  GetPrivateProfileStringW(L"OPT",L"KnobIndex",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if (local_c3c[0] != L'\0') {
    FUN_00465b20(local_c3c,4);
  }
  GetPrivateProfileStringW(L"OPT",L"NaviInx",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if ((local_c3c[0] == L'\0') || (iVar7 = FUN_00465b20(local_c3c,8), iVar7 < 1)) {
    uVar6 = 0;
    do {
      *(char *)(uVar6 + 0x2d6b + iVar17) = (char)uVar6;
      uVar6 = uVar6 + 1;
    } while (uVar6 < 8);
  }
  GetPrivateProfileStringW(L"OPT",L"LightBarLedPos",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if (local_c3c[0] == L'\0') {
    cVar4 = '\0';
  }
  else {
    cVar4 = FUN_00465b20(local_c3c,0x16);
  }
  *(char *)(iVar11 + 0x2d7e) = cVar4;
  if (cVar4 == '\0') {
    *(undefined1 *)(iVar11 + 0x2d7e) = 10;
    iVar7 = 0;
    do {
      *(char *)(iVar7 + 0x2d50 + iVar17) = (char)iVar7 + '\x01';
      iVar7 = iVar7 + 1;
    } while (iVar7 < (int)(uint)*(byte *)(iVar11 + 0x2d7e));
  }
  *(undefined4 *)(iVar11 + 0x2e40) = 0xffffffff;
  *(undefined4 *)(iVar11 + 0x2e44) = 0xffffffff;
  *(undefined4 *)(iVar11 + 0x2e48) = 0xffffffff;
  *(undefined4 *)(iVar11 + 0x2e4c) = 0xffffffff;
  *(undefined4 *)(iVar11 + 0x2e50) = 0xffffffff;
  GetPrivateProfileStringW(L"OPT",L"KeyNotToSet",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if ((local_c3c[0] != L'\0') && (sVar9 = FUN_00465b20(local_c3c,0x20), 0 < (int)sVar9)) {
    if (0x14 < sVar9) {
      sVar9 = 0x14;
    }
    if (0 < (int)sVar9) {
      _memcpy((void *)(iVar11 + 0x2e40),&local_c84,sVar9);
    }
  }
  *(undefined4 *)(iVar11 + 0x2e54) = 0xffffffff;
  GetPrivateProfileStringW(L"OPT",L"KeyNotToSetColor",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if ((local_c3c[0] != L'\0') && (sVar9 = FUN_00465b20(local_c3c,0x20), 0 < (int)sVar9)) {
    if (4 < sVar9) {
      sVar9 = 4;
    }
    if (0 < (int)sVar9) {
      _memcpy((void *)(iVar11 + 0x2e54),&local_c84,sVar9);
    }
  }
  UVar5 = GetPrivateProfileIntW(L"OPT",L"SendSysParam",0,pWVar3);
  *(short *)(iVar11 + 0x354c) = (short)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"SyncMask",7,pWVar3);
  *(short *)(iVar11 + 0x354e) = (short)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"SendTimeOnOpenKB",0,pWVar3);
  *(char *)(iVar11 + 0x3550) = (char)UVar5;
  if (*(int *)(local_d04 + 0x310) != 0) {
    UVar5 = GetPrivateProfileIntW(L"OPT",L"RGBScreen",1,pWVar3);
    *(char *)(iVar11 + 0x3551) = (char)UVar5;
    UVar5 = GetPrivateProfileIntW(L"OPT",L"MaxScreenFrame",0x10,pWVar3);
    *(UINT *)(iVar11 + 0x356c) = UVar5;
    UVar5 = GetPrivateProfileIntW(L"OPT",L"LatticeW",1,pWVar3);
    *(char *)(iVar11 + 0x3552) = (char)UVar5;
    UVar5 = GetPrivateProfileIntW(L"OPT",L"DrawLatticeLine",1,pWVar3);
    *(char *)(iVar11 + 0x3553) = (char)UVar5;
    UVar5 = GetPrivateProfileIntW(L"OPT",L"DrawFrameTime",0,pWVar3);
    *(UINT *)(iVar11 + 0x3570) = UVar5;
    GetPrivateProfileStringW(L"OPT",L"siScreen",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
    if ((local_c3c[0] != L'\0') && (iVar7 = FUN_00465a50(local_c3c,2), 0 < iVar7)) {
      *(int *)(iVar11 + 0x3564) = local_cfc[0];
      *(int *)(iVar11 + 0x3568) = local_cfc[1];
    }
    GetPrivateProfileStringW(L"OPT",L"FrameViewPos",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
    if ((local_c3c[0] != L'\0') && (iVar7 = FUN_00465a50(local_c3c,3), 0 < iVar7)) {
      if (iVar7 != 3) {
        MessageBoxW((HWND)0x0,L"FrameViewPos只能有3个值",L"Error",0);
      }
      *(int *)(iVar11 + 0x3554) = local_cfc[0];
      *(int *)(iVar11 + 0x3558) = local_cfc[1];
      *(int *)(iVar11 + 0x355c) = local_cfc[2] + local_cfc[0];
    }
  }
  local_d0c = (int *)((uint)local_d0c & 0xff000000);
  GetPrivateProfileStringW(L"OPT",L"RGBIndex",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if ((local_c3c[0] == L'\0') || (iVar7 = FUN_00465b20(local_c3c,3), iVar7 < 1)) {
    *(undefined1 *)(iVar11 + 0x2f98) = 2;
    *(undefined1 *)(iVar11 + 0x2f99) = 1;
    *(undefined1 *)(iVar11 + 0x2f9a) = 0;
  }
  else {
    bVar2 = local_d0c._2_1_;
    if ((uint)local_d0c._2_1_ + ((uint)local_d0c >> 8 & 0xff) + ((uint)local_d0c & 0xff) != 3) {
      MessageBoxW((HWND)0x0,L"R G B Index Error",L"Warning",0);
    }
    *(byte *)(iVar11 + 0x2f9a) = bVar2;
    *(undefined1 *)(iVar11 + 0x2f98) = (undefined1)local_d0c;
    *(undefined1 *)(iVar11 + 0x2f99) = local_d0c._1_1_;
  }
  GetPrivateProfileStringW(L"OPT",L"GameRGBIndex",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if ((local_c3c[0] == L'\0') || (iVar7 = FUN_00465b20(local_c3c,3), iVar7 < 1)) {
    *(undefined1 *)(iVar11 + 0x2f9b) = 2;
    *(undefined1 *)(iVar11 + 0x2f9c) = 1;
    *(undefined1 *)(iVar11 + 0x2f9d) = 0;
  }
  else {
    bVar2 = local_d0c._2_1_;
    if ((uint)local_d0c._2_1_ + ((uint)local_d0c >> 8 & 0xff) + ((uint)local_d0c & 0xff) != 3) {
      MessageBoxW((HWND)0x0,L"clrGmKey RGB Index Error",L"Warning",0);
    }
    *(byte *)(iVar11 + 0x2f9d) = bVar2;
    *(undefined1 *)(iVar11 + 0x2f9b) = (undefined1)local_d0c;
    *(undefined1 *)(iVar11 + 0x2f9c) = local_d0c._1_1_;
  }
  GetPrivateProfileStringW(L"OPT",L"MusicRGBIndex",(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
  if ((local_c3c[0] == L'\0') || (iVar7 = FUN_00465b20(local_c3c,3), iVar7 < 1)) {
    *(undefined1 *)(iVar11 + 0x2f9e) = *(undefined1 *)(iVar11 + 0x2f98);
    *(undefined1 *)(iVar11 + 0x2f9f) = *(undefined1 *)(iVar11 + 0x2f99);
    *(undefined1 *)(iVar11 + 0x2fa0) = *(undefined1 *)(iVar11 + 0x2f9a);
  }
  else {
    bVar2 = local_d0c._2_1_;
    if ((uint)local_d0c._2_1_ + ((uint)local_d0c >> 8 & 0xff) + ((uint)local_d0c & 0xff) != 3) {
      MessageBoxW((HWND)0x0,L"Music RGB Index Error",L"Warning",0);
    }
    *(byte *)(iVar11 + 0x2fa0) = bVar2;
    *(undefined1 *)(iVar11 + 0x2f9e) = (undefined1)local_d0c;
    *(undefined1 *)(iVar11 + 0x2f9f) = local_d0c._1_1_;
  }
  UVar5 = GetPrivateProfileIntW(L"OPT",L"GaoshouIndex",-1,pWVar3);
  *(char *)(iVar11 + 0x2ebc) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"GaoshouGroupNum",5,pWVar3);
  *(char *)(iVar11 + 0x2f8c) = (char)UVar5;
  UVar5 = GetPrivateProfileIntW(L"OPT",L"StartGp",0x20,pWVar3);
  *(char *)(iVar11 + 0x2f8e) = (char)UVar5;
  GetPrivateProfileStringW(L"OPT",L"clrGaoshou",(LPCWSTR)0x0,local_41c,0x104,pWVar3);
  if (local_41c[0] == L'\0') {
    uVar6 = 0xffffff;
  }
  else {
    local_d0c = (int *)((uint)local_d0c & 0xff000000);
    FUN_00465b20(local_41c,3);
    uVar6 = (uint)local_d0c & 0xffffff;
  }
  *(uint *)(iVar11 + 0x2f88) = uVar6;
  iVar7 = 0;
  if (*(char *)(iVar11 + 0x2f8c) != '\0') {
    local_d0c = (int *)(iVar11 + 0x2ebd);
    do {
      iVar7 = iVar7 + 1;
      local_d00 = iVar7;
      __snwprintf_s(local_214,0x14,0x13,L"GaoshouKey%d",iVar7);
      GetPrivateProfileStringW(L"OPT",local_214,(LPCWSTR)0x0,local_c3c,0x410,pWVar3);
      if (local_c3c[0] != L'\0') {
        FUN_00465b20(local_c3c,0x28);
        iVar7 = local_d00;
      }
      local_d0c = (int *)((int)local_d0c + 0x28);
    } while (iVar7 < (int)(uint)*(byte *)(iVar11 + 0x2f8c));
  }
  uVar10 = FUN_00413440(pWVar3,1);
  *(undefined4 *)(iVar11 + 0x2d28) = uVar10;
  FUN_00413800(local_d04);
  FUN_00413800(local_d04);
  FUN_00413800(local_d04);
  *(undefined1 *)(iVar11 + 0x2fd4) = 1;
  *(undefined1 *)(iVar11 + 0x2fd6) = 0;
  *(undefined1 *)(iVar11 + 0x2fd5) = 5;
  *(undefined1 *)(iVar11 + 0x2fd7) = 1;
  *(undefined1 *)(iVar11 + 0x2fd8) = 1;
  *(undefined1 *)(iVar11 + 0x2fd9) = 0;
  *(undefined1 *)(iVar11 + 0x2fda) = 1;
  *(undefined1 *)(iVar11 + 0x2fdb) = 1;
  local_c64[0] = 0xff;
  local_c64[1] = 0xff00;
  local_c64[2] = 0xff0000;
  local_c64[3] = 0xffff;
  local_c64[4] = 0xff00ff;
  local_c64[5] = 0xffff00;
  local_c64[6] = 0xffffff;
  local_c64[7] = 0;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x2fdc);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x2ffc) = 1;
  *(undefined1 *)(iVar11 + 0x2ffe) = 1;
  *(undefined1 *)(iVar11 + 0x2ffd) = 7;
  *(undefined1 *)(iVar11 + 0x2fff) = 1;
  *(undefined1 *)(iVar11 + 0x3000) = 0;
  *(undefined1 *)(iVar11 + 0x3001) = 0;
  *(undefined1 *)(iVar11 + 0x3002) = 0;
  *(undefined1 *)(iVar11 + 0x3003) = 0;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x3004);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x3024) = 1;
  *(undefined1 *)(iVar11 + 0x3026) = 2;
  *(undefined1 *)(iVar11 + 0x3025) = 0x11;
  *(undefined1 *)(iVar11 + 0x3027) = 1;
  *(undefined1 *)(iVar11 + 0x3028) = 1;
  *(undefined1 *)(iVar11 + 0x3029) = 0;
  *(undefined1 *)(iVar11 + 0x302a) = 1;
  *(undefined1 *)(iVar11 + 0x302b) = 1;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x302c);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x304c) = 1;
  *(undefined1 *)(iVar11 + 0x304e) = 3;
  *(undefined1 *)(iVar11 + 0x304d) = 0xc;
  *(undefined1 *)(iVar11 + 0x304f) = 1;
  *(undefined1 *)(iVar11 + 0x3050) = 1;
  *(undefined1 *)(iVar11 + 0x3051) = 0;
  *(undefined1 *)(iVar11 + 0x3052) = 1;
  *(undefined1 *)(iVar11 + 0x3053) = 1;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x3054);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x3074) = 1;
  *(undefined1 *)(iVar11 + 0x3076) = 4;
  *(undefined1 *)(iVar11 + 0x3075) = 1;
  *(undefined1 *)(iVar11 + 0x3077) = 0;
  *(undefined1 *)(iVar11 + 0x3078) = 1;
  *(undefined1 *)(iVar11 + 0x3079) = 0;
  *(undefined1 *)(iVar11 + 0x307a) = 1;
  *(undefined1 *)(iVar11 + 0x307b) = 1;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x307c);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x309c) = 1;
  *(undefined1 *)(iVar11 + 0x309e) = 5;
  *(undefined1 *)(iVar11 + 0x309d) = 3;
  *(undefined1 *)(iVar11 + 0x309f) = 1;
  *(undefined1 *)(iVar11 + 0x30a0) = 0;
  *(undefined1 *)(iVar11 + 0x30a1) = 0;
  *(undefined1 *)(iVar11 + 0x30a2) = 1;
  *(undefined1 *)(iVar11 + 0x30a3) = 1;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x30a4);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x30c4) = 1;
  *(undefined1 *)(iVar11 + 0x30c6) = 6;
  *(undefined1 *)(iVar11 + 0x30c5) = 2;
  *(undefined1 *)(iVar11 + 0x30c7) = 1;
  *(undefined1 *)(iVar11 + 0x30c8) = 0;
  *(undefined1 *)(iVar11 + 0x30c9) = 0;
  *(undefined1 *)(iVar11 + 0x30ca) = 0;
  *(undefined1 *)(iVar11 + 0x30cb) = 0;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x30cc);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x30ec) = 1;
  *(undefined1 *)(iVar11 + 0x30ee) = 7;
  *(undefined1 *)(iVar11 + 0x30ed) = 0x13;
  *(undefined1 *)(iVar11 + 0x30ef) = 1;
  *(undefined1 *)(iVar11 + 0x30f0) = 1;
  *(undefined1 *)(iVar11 + 0x30f1) = 0;
  *(undefined1 *)(iVar11 + 0x30f2) = 1;
  *(undefined1 *)(iVar11 + 0x30f3) = 1;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x30f4);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x3114) = 1;
  *(undefined1 *)(iVar11 + 0x3116) = 8;
  *(undefined1 *)(iVar11 + 0x3115) = 0xf;
  *(undefined1 *)(iVar11 + 0x3117) = 1;
  *(undefined1 *)(iVar11 + 0x3118) = 1;
  *(undefined1 *)(iVar11 + 0x3119) = 0;
  *(undefined1 *)(iVar11 + 0x311a) = 1;
  *(undefined1 *)(iVar11 + 0x311b) = 1;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x311c);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x313c) = 1;
  *(undefined1 *)(iVar11 + 0x313e) = 9;
  *(undefined1 *)(iVar11 + 0x313d) = 0xd;
  *(undefined1 *)(iVar11 + 0x313f) = 1;
  *(undefined1 *)(iVar11 + 0x3140) = 0;
  *(undefined1 *)(iVar11 + 0x3141) = 0;
  *(undefined1 *)(iVar11 + 0x3142) = 0;
  *(undefined1 *)(iVar11 + 0x3143) = 0;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x3144);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x3164) = 1;
  *(undefined1 *)(iVar11 + 0x3166) = 10;
  *(undefined1 *)(iVar11 + 0x3165) = 0x14;
  *(undefined1 *)(iVar11 + 0x3167) = 1;
  *(undefined1 *)(iVar11 + 0x3168) = 1;
  *(undefined1 *)(iVar11 + 0x3169) = 0;
  *(undefined1 *)(iVar11 + 0x316a) = 1;
  *(undefined1 *)(iVar11 + 0x316b) = 1;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x316c);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  *(undefined1 *)(iVar11 + 0x318c) = 1;
  *(undefined1 *)(iVar11 + 0x318e) = 0xb;
  *(undefined1 *)(iVar11 + 0x318d) = 0x10;
  *(undefined1 *)(iVar11 + 0x318f) = 1;
  *(undefined1 *)(iVar11 + 0x3190) = 1;
  *(undefined1 *)(iVar11 + 0x3191) = 0;
  *(undefined1 *)(iVar11 + 0x3192) = 0;
  *(undefined1 *)(iVar11 + 0x3193) = 0;
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x3194);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  piVar14 = local_c64;
  piVar19 = (int *)(iVar11 + 0x31bc);
  for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
    *piVar19 = *piVar14;
    piVar14 = piVar14 + 1;
    piVar19 = piVar19 + 1;
  }
  local_d0c = (int *)(iVar11 + 0x2fd5);
  *(undefined1 *)(iVar11 + 0x31b6) = 0xc;
  *(undefined1 *)(iVar11 + 0x31b4) = 1;
  *(undefined1 *)(iVar11 + 0x31b5) = 0x12;
  *(undefined1 *)(iVar11 + 0x31b7) = 1;
  *(undefined1 *)(iVar11 + 0x31b8) = 1;
  *(undefined1 *)(iVar11 + 0x31b9) = 0;
  *(undefined1 *)(iVar11 + 0x31ba) = 1;
  *(undefined1 *)(iVar11 + 0x31bb) = 1;
  local_d00 = 0;
  local_d04 = 0xbf1;
  do {
    local_d00 = local_d00 + 1;
    __snwprintf_s(local_214,0x1e,0x1d,L"LedOpt%d",local_d00);
    GetPrivateProfileStringW(L"OPT",local_214,(LPCWSTR)0x0,local_c3c,0x410,local_d08);
    if ((local_c3c[0] != L'\0') && (iVar7 = FUN_00465b20(local_c3c,0x20), 0 < iVar7)) {
      *(undefined1 *)((int)local_d0c + 1) = local_c84;
      *(undefined1 *)local_d0c = (undefined1)local_c83;
      *(undefined1 *)((int)local_d0c + 2) = local_c83._1_1_;
      *(undefined1 *)((int)local_d0c + 3) = local_c83._2_1_;
      *(undefined1 *)(local_d0c + 1) = local_c83._3_1_;
      *(undefined1 *)((int)local_d0c + -1) = 1;
      *(undefined1 *)((int)local_d0c + 5) = (undefined1)local_c7f;
      cVar4 = local_c7f._1_1_;
      if (local_c7f._1_1_ == -0x80) {
        cVar4 = '\0';
      }
      *(char *)((int)local_d0c + 6) = cVar4;
      piVar14 = local_c64;
      piVar19 = (int *)((int)local_d0c + 7);
      for (iVar13 = 8; iVar13 != 0; iVar13 = iVar13 + -1) {
        *piVar19 = *piVar14;
        piVar14 = piVar14 + 1;
        piVar19 = piVar19 + 1;
      }
      if (7 < iVar7) {
        iVar7 = 0;
        puVar12 = &local_c7f;
        do {
          if (*(uint3 *)((int)puVar12 + 2) == 0) break;
          iVar13 = iVar7 + local_d04;
          iVar7 = iVar7 + 1;
          *(uint *)(iVar17 + iVar13 * 4) = (uint)*(uint3 *)((int)puVar12 + 2);
          puVar12 = (undefined4 *)((int)puVar12 + 3);
        } while (iVar7 < 8);
      }
    }
    local_d0c = local_d0c + 10;
    local_d04 = local_d04 + 10;
  } while (local_d04 < 0xce1);
  if (*(char *)(iVar11 + 0x2d81) != '\0') {
    uVar6 = 0;
    puVar18 = (undefined1 *)(iVar11 + 0x3395);
    do {
      uVar6 = uVar6 + 1;
      __snwprintf_s(local_214,0x1e,0x1d,L"SideLedOpt%d",uVar6);
      GetPrivateProfileStringW(L"OPT",local_214,(LPCWSTR)0x0,local_c3c,0x410,local_d08);
      if ((local_c3c[0] != L'\0') && (iVar11 = FUN_00465b20(local_c3c,0x20), 0 < iVar11)) {
        puVar18[1] = local_c84;
        *puVar18 = (undefined1)local_c83;
        puVar18[2] = local_c83._1_1_;
        puVar18[3] = local_c83._2_1_;
        puVar18[4] = local_c83._3_1_;
        puVar18[-1] = 1;
        puVar18[5] = (undefined1)local_c7f;
        cVar4 = local_c7f._1_1_;
        if (local_c7f._1_1_ == -0x80) {
          cVar4 = '\0';
        }
        puVar18[6] = cVar4;
        if (7 < iVar11) {
          puVar15 = (uint *)(puVar18 + 7);
          iVar11 = 8;
          puVar12 = &local_c7f;
          do {
            *puVar15 = (uint)*(uint3 *)((int)puVar12 + 2);
            puVar15 = puVar15 + 1;
            iVar11 = iVar11 + -1;
            puVar12 = (undefined4 *)((int)puVar12 + 3);
          } while (iVar11 != 0);
        }
      }
      puVar18 = puVar18 + 0x28;
    } while (uVar6 < 0xb);
  }
LAB_00413427:
  __security_check_cookie(local_8 ^ (uint)&local_d0c);
  return;
}

