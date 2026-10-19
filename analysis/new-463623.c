
void FUN_00463570(void)

{
  int in_EAX;
  HWND pHVar1;
  CWnd *pCVar2;
  int unaff_ESI;
  
  if (in_EAX == 0) {
    pHVar1 = GetParent(*(HWND *)(unaff_ESI + 0x20));
    pCVar2 = CWnd::FromHandle(pHVar1);
    pHVar1 = GetParent(*(HWND *)(pCVar2 + 0x20));
    pCVar2 = CWnd::FromHandle(pHVar1);
    in_EAX = *(int *)(pCVar2 + 0xa4);
    if (in_EAX == 0) {
      return;
    }
  }
  if (*(int *)(unaff_ESI + 0x613c) == 0) {
    if ((unaff_ESI != -0x21b8) && (*(int *)(unaff_ESI + 0x21d8) != 0)) {
      *(uint *)(in_EAX + 0x3334) = (uint)(*(int *)(unaff_ESI + 0x2260) == 0);
      *(undefined4 *)(in_EAX + 0x3338) =
           *(undefined4 *)(unaff_ESI + 0x285c + *(int *)(unaff_ESI + 0x239c) * 4);
    }
    if ((unaff_ESI != -0x36d0) && (*(int *)(unaff_ESI + 0x36f0) != 0)) {
      *(ushort *)(in_EAX + 0x333c) = (ushort)(*(int *)(unaff_ESI + 0x3778) == 0);
      *(undefined2 *)(in_EAX + 0x333e) =
           *(undefined2 *)(unaff_ESI + 0x3d74 + *(int *)(unaff_ESI + 0x38b4) * 4);
    }
    if ((unaff_ESI != -0x508c) && (*(int *)(unaff_ESI + 0x50ac) != 0)) {
      *(undefined2 *)(in_EAX + 0x3340) =
           *(undefined2 *)(unaff_ESI + 0x55a4 + *(int *)(unaff_ESI + 0x50e4) * 4);
    }
    if ((unaff_ESI != -0x33b8) && (*(int *)(unaff_ESI + 0x33d8) != 0)) {
      *(undefined2 *)(in_EAX + 0x3342) = *(undefined2 *)(unaff_ESI + 0x3460);
    }
    if ((unaff_ESI != -0x4f00) && (*(int *)(unaff_ESI + 0x4f20) != 0)) {
      if (*(int *)(unaff_ESI + 0x4fa8) != 0) {
        *(byte *)(in_EAX + 0x3330) = *(byte *)(in_EAX + 0x3330) | 1;
        return;
      }
      *(byte *)(in_EAX + 0x3330) = *(byte *)(in_EAX + 0x3330) & 0xfe;
    }
  }
  else {
    if ((unaff_ESI != -0x36d0) && (*(int *)(unaff_ESI + 0x36f0) != 0)) {
      *(bool *)(in_EAX + 0x2891) = *(int *)(unaff_ESI + 0x3778) == 0;
      *(undefined1 *)(in_EAX + 0x2892) =
           *(undefined1 *)(unaff_ESI + 0x3d74 + *(int *)(unaff_ESI + 0x38b4) * 4);
    }
    if ((unaff_ESI != -0x508c) && (*(int *)(unaff_ESI + 0x50ac) != 0)) {
      *(uint *)(in_EAX + 0x288c) =
           (uint)*(ushort *)(unaff_ESI + 0x55a4 + *(int *)(unaff_ESI + 0x50e4) * 4);
      return;
    }
  }
  return;
}

