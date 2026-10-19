
undefined1 * FUN_0049a970(uint *param_1,int param_2)

{
  char cVar1;
  byte bVar2;
  uint uVar3;
  undefined1 *puVar4;
  byte bVar5;
  uint uVar6;
  int iVar7;
  byte bVar8;
  undefined8 uVar9;
  
  uVar3 = (int)*param_1 >> 0x18 & 0xff;
  bVar5 = 0;
  uVar6 = *param_1 & 0xffffff;
  if (uVar3 == 1) {
    switch(uVar6) {
    case 0x11:
      puVar4 = (undefined1 *)0x10101;
      break;
    case 0x12:
      puVar4 = (undefined1 *)0x10301;
      break;
    case 0x13:
      puVar4 = (undefined1 *)0x10201;
      break;
    case 0x14:
      puVar4 = (undefined1 *)0x10401;
      break;
    case 0x15:
      puVar4 = (undefined1 *)0x10501;
      break;
    case 0x16:
      puVar4 = (undefined1 *)0x32020101;
      break;
    case 0x17:
      puVar4 = (undefined1 *)0x32030101;
      break;
    default:
      goto switchD_0049a99f_caseD_18;
    case 0x19:
      puVar4 = (undefined1 *)0x801;
      break;
    case 0x1a:
      puVar4 = (undefined1 *)0x901;
      break;
    case 0x1b:
      puVar4 = (undefined1 *)0x32ff0701;
      break;
    case 0x1c:
      puVar4 = (undefined1 *)0x32ff0601;
    }
    if ((param_2 != 0) && (*(int *)(param_2 + 0x14) == 1)) {
      if (uVar6 == 0x14) {
        return (undefined1 *)0x10501;
      }
      if (uVar6 == 0x15) {
        return (undefined1 *)0x10401;
      }
    }
  }
  else {
    if ((uVar3 == 4) || (uVar3 == 3)) {
      switch(uVar6) {
      case 0x21:
switchD_0049aa98_caseD_88:
        return (undefined1 *)0x83010002;
      case 0x22:
switchD_0049aa98_caseD_89:
        return (undefined1 *)0xcd000002;
      case 0x23:
switchD_0049aa98_caseD_8a:
        return (undefined1 *)0xb7000002;
      case 0x24:
switchD_0049aa98_caseD_8b:
        return (undefined1 *)0xb6000002;
      case 0x25:
switchD_0049aa98_caseD_8c:
        return (undefined1 *)0xb5000002;
      case 0x26:
switchD_0049aa98_caseD_8d:
        return (undefined1 *)0xe9000002;
      case 0x27:
switchD_0049aa98_caseD_8e:
        return (undefined1 *)0xea000002;
      case 0x28:
switchD_0049aa98_caseD_8f:
        return (undefined1 *)0xe2000002;
      case 0x29:
        return (undefined1 *)0x6f000002;
      case 0x2a:
        return (undefined1 *)0x70000002;
      case 0x30:
        return (undefined1 *)0x8a010002;
      case 0x31:
switchD_0049aa98_caseD_99:
        return (undefined1 *)0x92010002;
      case 0x32:
switchD_0049aa98_caseD_9a:
        return (undefined1 *)0x94010002;
      case 0x33:
        return (undefined1 *)0x21020002;
      case 0x34:
switchD_0049aa98_caseD_9b:
        return (undefined1 *)0x23020002;
      case 0x35:
        return (undefined1 *)0x24020002;
      case 0x36:
        return (undefined1 *)0x25020002;
      case 0x37:
        return (undefined1 *)0x26020002;
      case 0x38:
        return (undefined1 *)0x27020002;
      case 0x39:
        return (undefined1 *)0x2a020002;
      }
    }
    else {
      if (uVar3 == 2) {
        bVar5 = (byte)param_1[1];
        if (bVar5 == 0) {
          if (*(char *)((int)param_1 + 5) == -6) {
            return (undefined1 *)0xd;
          }
          if (*(char *)((int)param_1 + 5) == -5) {
            return (undefined1 *)0x10d;
          }
        }
        bVar2 = *(byte *)((int)param_1 + 5);
        if ((bVar2 == 0) || (5 < bVar2)) {
          switch(bVar2) {
          case 0x40:
            return (undefined1 *)0x2c000000;
          default:
            bVar2 = FUN_004821d0();
            uVar3 = FUN_004821d0();
            bVar8 = (bVar5 & 1) != 0;
            if ((bVar5 & 2) != 0) {
              bVar8 = bVar8 | 2;
            }
            if ((bVar5 & 4) != 0) {
              bVar8 = bVar8 | 4;
            }
            if ((bVar5 & 8) != 0) {
              bVar8 = bVar8 | 8;
            }
            if ((bVar5 & 0x10) != 0) {
              bVar8 = bVar8 | 0x10;
            }
            if ((bVar5 & 0x20) != 0) {
              bVar8 = bVar8 | 0x20;
            }
            if ((bVar5 & 0x40) != 0) {
              bVar8 = bVar8 | 0x40;
            }
            if ((char)bVar5 < '\0') {
              bVar8 = bVar8 | 0x80;
            }
            return (undefined1 *)((((uVar3 & 0xff) << 8 | (uint)bVar2) << 8 | (uint)bVar8) << 8);
          case 0x88:
            goto switchD_0049aa98_caseD_88;
          case 0x89:
            goto switchD_0049aa98_caseD_89;
          case 0x8a:
            goto switchD_0049aa98_caseD_8a;
          case 0x8b:
            goto switchD_0049aa98_caseD_8b;
          case 0x8c:
            goto switchD_0049aa98_caseD_8c;
          case 0x8d:
            goto switchD_0049aa98_caseD_8d;
          case 0x8e:
            goto switchD_0049aa98_caseD_8e;
          case 0x8f:
            goto switchD_0049aa98_caseD_8f;
          case 0x99:
            goto switchD_0049aa98_caseD_99;
          case 0x9a:
            goto switchD_0049aa98_caseD_9a;
          case 0x9b:
            goto switchD_0049aa98_caseD_9b;
          }
        }
        goto switchD_0049acfb_caseD_ad;
      }
      if (uVar3 == 5) {
        uVar3 = param_1[1];
        uVar9 = FUN_00479f50();
        iVar7 = (int)((ulonglong)uVar9 >> 0x20);
        if ((int)uVar9 != 0) {
          cVar1 = *(char *)(iVar7 + 0xf);
          uVar3 = 0;
          bVar5 = 1;
          if (cVar1 == '\0') {
            uVar3 = 1;
            bVar5 = *(byte *)(iVar7 + 0xe);
          }
          else if (cVar1 == '\x02') {
            uVar3 = 4;
          }
          else if (cVar1 == '\x01') {
            uVar3 = 2;
          }
          return (undefined1 *)(((uint)bVar5 << 8 | uVar3) << 8 | 3);
        }
        (*DAT_0065e3e4)(L"keyinfo_to_hardware_code can\'t find macro id 0x%x",uVar3);
      }
      else if (uVar3 == 6) {
        switch(uVar6) {
        case 0x43:
          return (undefined1 *)0x1d0100;
        case 0x48:
          return (undefined1 *)0x160100;
        case 0x51:
          return (undefined1 *)0x3d0400;
        case 0x52:
          return (undefined1 *)0xf0800;
        case 0x54:
          return (undefined1 *)0x150800;
        case 0x55:
          return (undefined1 *)0x70800;
        case 0x56:
          return (undefined1 *)0x2e0100;
        case 0x57:
          return (undefined1 *)0x2d0100;
        case 0x5a:
          return &LAB_004c0500;
        case 0x60:
          return (undefined1 *)0x7;
        case 0x61:
          return (undefined1 *)0x1000007;
        case 0x62:
          return (undefined1 *)0x2000007;
        case 99:
          return (undefined1 *)0x4000007;
        case 100:
          return (undefined1 *)0x2000010;
        case 0x65:
          return (undefined1 *)0x4000010;
        case 0x66:
          return (undefined1 *)0x7000005;
        case 0x67:
          return (undefined1 *)0x7000006;
        case 0x68:
          return (undefined1 *)0x7000007;
        case 0x69:
          return (undefined1 *)0x7000008;
        case 0x6a:
          return (undefined1 *)0x7000009;
        case 0xa2:
switchD_0049abfa_caseD_a2:
          return (undefined1 *)0x0;
        }
      }
      else {
        if (uVar3 == 9) {
          if (uVar6 != 0) {
            puVar4 = (undefined1 *)FUN_004683b0();
            return puVar4;
          }
switchD_0049acfb_caseD_ad:
          return (undefined1 *)param_1[2];
        }
        if (uVar3 == 8) {
          switch(uVar6) {
          case 0xa2:
          case 0xa7:
            goto switchD_0049abfa_caseD_a2;
          case 0xa4:
            switch(*(ushort *)((int)param_1 + 10) & 0xff) {
            case 0xf0:
              uVar3 = 1;
              bVar5 = 1;
              break;
            case 0xf1:
              uVar3 = 1;
              bVar5 = 2;
              break;
            case 0xf2:
              uVar3 = 1;
              bVar5 = 3;
              break;
            case 0xf3:
              uVar3 = 1;
              bVar5 = 4;
              break;
            case 0xf4:
              uVar3 = 1;
              bVar5 = 5;
              break;
            default:
              uVar3 = 0;
            }
            return (undefined1 *)
                   (((((int)param_1[2] >> 8 & 0xffU) << 8 | (uint)(byte)param_1[2]) << 8 |
                    (uint)bVar5) << 8 | uVar3);
          case 0xa6:
            return (undefined1 *)((param_1[2] - 1) * 0x1000000 | 0x505);
          case 0xa8:
            return (undefined1 *)0x105;
          case 0xa9:
            return (undefined1 *)0x205;
          case 0xaa:
            return (undefined1 *)0x405;
          case 0xab:
            return (undefined1 *)0xe;
          case 0xad:
            goto switchD_0049acfb_caseD_ad;
          case 0xae:
            return (undefined1 *)0x8;
          }
        }
      }
    }
switchD_0049a99f_caseD_18:
    puVar4 = (undefined1 *)0xffffffff;
  }
  return puVar4;
}

