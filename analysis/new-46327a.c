
void FUN_00463040(int param_1)

{
  int iVar1;
  uint uVar2;
  code *pcVar3;
  int unaff_EDI;
  undefined1 *local_20;
  undefined1 *local_1c;
  wchar_t local_18 [10];
  uint local_4;
  
  pcVar3 = InvalidateRect_exref;
  local_4 = DAT_00650674 ^ (uint)&local_20;
  if (param_1 != 0) {
    if (((unaff_EDI != -0x4d74) && (*(int *)(unaff_EDI + 0x4d94) != 0)) &&
       (DAT_006709d8 != *(int *)(unaff_EDI + 0x4e1c))) {
      *(int *)(unaff_EDI + 0x4e1c) = DAT_006709d8;
      InvalidateRect(*(HWND *)(unaff_EDI + 0x4d94),(RECT *)0x0,1);
    }
    if (((unaff_EDI != -0x3544) && (*(int *)(unaff_EDI + 0x3564) != 0)) &&
       (DAT_006709e0 != *(int *)(unaff_EDI + 0x35ec))) {
      *(int *)(unaff_EDI + 0x35ec) = DAT_006709e0;
      InvalidateRect(*(HWND *)(unaff_EDI + 0x3564),(RECT *)0x0,1);
    }
    if ((uint)(DAT_006709d4 == 0) != *(uint *)(unaff_EDI + 0x1f48)) {
      *(uint *)(unaff_EDI + 0x1f48) = (uint)(DAT_006709d4 == 0);
      InvalidateRect(*(HWND *)(unaff_EDI + 0x1ec0),(RECT *)0x0,1);
    }
    if (DAT_006709d4 != *(int *)(unaff_EDI + 0x20d4)) {
      *(int *)(unaff_EDI + 0x20d4) = DAT_006709d4;
      InvalidateRect(*(HWND *)(unaff_EDI + 0x204c),(RECT *)0x0,1);
    }
    FUN_004075d0();
    if (DAT_006709dc != *(int *)(unaff_EDI + 0x1dbc)) {
      *(int *)(unaff_EDI + 0x1dbc) = DAT_006709dc;
      InvalidateRect(*(HWND *)(unaff_EDI + 0x1d34),(RECT *)0x0,1);
    }
    iVar1 = FUN_004638b0();
    if (DAT_006709dc == 0) {
      if (iVar1 != 0) {
        FUN_00463810();
      }
    }
    else if (iVar1 == 0) {
      FUN_00463730();
    }
    local_20 = *(undefined1 **)(*(int *)(unaff_EDI + 0x6138) + 0x24);
    if (*(int *)(unaff_EDI + 0x613c) == 0) {
      if ((unaff_EDI != -0x21b8) && (*(int *)(unaff_EDI + 0x21d8) != 0)) {
        uVar2 = (uint)(*(int *)(param_1 + 0x3334) == 0);
        if (uVar2 != *(uint *)(unaff_EDI + 0x2260)) {
          *(uint *)(unaff_EDI + 0x2260) = uVar2;
          InvalidateRect(*(HWND *)(unaff_EDI + 0x21d8),(RECT *)0x0,1);
        }
        *(uint *)(unaff_EDI + 0x23a8) = (uint)(*(int *)(unaff_EDI + 0x2260) == 0);
        InvalidateRect(*(HWND *)(unaff_EDI + 0x2364),(RECT *)0x0,1);
        CWnd::ShowWindow((CWnd *)(unaff_EDI + 0x322c),
                         (-(uint)(*(int *)(unaff_EDI + 0x23a8) != 0) & 0xfffffffb) + 5);
        FUN_0044a210();
        __snwprintf_s(local_18,10,9,L"%d ms");
        local_1c = &stack0xffffffc8;
        FUN_004035e0(local_18);
        FUN_0045e680();
        pcVar3 = InvalidateRect_exref;
      }
      if ((unaff_EDI != -0x36d0) && (*(int *)(unaff_EDI + 0x36f0) != 0)) {
        uVar2 = (uint)(*(short *)(param_1 + 0x333c) == 0);
        if (uVar2 != *(uint *)(unaff_EDI + 0x3778)) {
          *(uint *)(unaff_EDI + 0x3778) = uVar2;
          (*pcVar3)();
        }
        *(uint *)(unaff_EDI + 0x38c0) = (uint)(*(int *)(unaff_EDI + 0x3778) == 0);
        (*pcVar3)();
        CWnd::ShowWindow((CWnd *)(unaff_EDI + 0x4744),
                         (-(uint)(*(int *)(unaff_EDI + 0x38c0) != 0) & 0xfffffffb) + 5);
        FUN_0044a210();
        __snwprintf_s(local_18,10,9,L"%d %s");
        local_1c = &stack0xffffffc8;
        FUN_004035e0(local_18);
        FUN_0045e680();
        pcVar3 = InvalidateRect_exref;
      }
      if ((unaff_EDI != -0x508c) && (*(int *)(unaff_EDI + 0x50ac) != 0)) {
        FUN_0044a210();
        if (1 < *(int *)(DAT_00670cf0 + -4)) {
          ATL::CSimpleStringT<wchar_t,0>::Fork
                    ((CSimpleStringT<wchar_t,0> *)&DAT_00670cf0,*(int *)(DAT_00670cf0 + -0xc));
        }
        __snwprintf_s(local_18,10,9,L"%d %s");
        local_1c = &stack0xffffffc8;
        FUN_004035e0(local_18);
        FUN_0045e680();
        pcVar3 = InvalidateRect_exref;
      }
      if (((unaff_EDI != -0x33b8) && (*(int *)(unaff_EDI + 0x33d8) != 0)) &&
         ((uint)*(ushort *)(param_1 + 0x3342) != *(uint *)(unaff_EDI + 0x3460))) {
        *(uint *)(unaff_EDI + 0x3460) = (uint)*(ushort *)(param_1 + 0x3342);
        (*pcVar3)();
      }
      if (((unaff_EDI != -0x4f00) && (*(int *)(unaff_EDI + 0x4f20) != 0)) &&
         (uVar2 = *(byte *)(param_1 + 0x3330) & 1, uVar2 != *(uint *)(unaff_EDI + 0x4fa8))) {
        *(uint *)(unaff_EDI + 0x4fa8) = uVar2;
        (*pcVar3)();
      }
    }
    else if ((unaff_EDI != -0x36d0) && (*(int *)(unaff_EDI + 0x36f0) != 0)) {
      uVar2 = (uint)(*(char *)(param_1 + 0x2891) == '\0');
      if (uVar2 != *(uint *)(unaff_EDI + 0x3778)) {
        *(uint *)(unaff_EDI + 0x3778) = uVar2;
        InvalidateRect(*(HWND *)(unaff_EDI + 0x36f0),(RECT *)0x0,1);
      }
      *(uint *)(unaff_EDI + 0x38c0) = (uint)(*(int *)(unaff_EDI + 0x3778) == 0);
      InvalidateRect(*(HWND *)(unaff_EDI + 0x387c),(RECT *)0x0,1);
      CWnd::ShowWindow((CWnd *)(unaff_EDI + 0x4744),
                       (-(uint)(*(int *)(unaff_EDI + 0x38c0) != 0) & 0xfffffffb) + 5);
      FUN_0044a210();
      __snwprintf_s(local_18,10,9,L"%d %s");
      local_20 = &stack0xffffffc8;
      FUN_004035e0(local_18);
      FUN_0045e680();
      pcVar3 = InvalidateRect_exref;
    }
    (*pcVar3)();
  }
  __security_check_cookie(local_4 ^ (uint)&local_20);
  return;
}

