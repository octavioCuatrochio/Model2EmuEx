// ===== FUN_004b6010 @ 004b6010 (610 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b6010(void)

{
  short *psVar1;
  int iVar2;
  undefined *puVar3;
  int iVar4;
  uint uVar5;
  undefined4 *puVar6;
  int iVar7;
  
  iVar7 = DAT_010ee034;
  do {
    if (DAT_010f0040 != 0) {
      DAT_010ee034 = iVar7;
      return;
    }
    iVar4 = 1000;
    DAT_010481f4 = &DAT_010edef0;
    DAT_010481f8 = &DAT_010ee040;
    DAT_010481fc = &DAT_010ef040;
    DAT_010481e0 = DAT_010f0048;
    DAT_010481e8 = DAT_010f0044;
    _DAT_010481ec = DAT_010ee038;
    DAT_010481f0 = DAT_010ee030;
    _DAT_01048244 = DAT_010ee03c;
    DAT_0104824c = iVar7;
    DAT_010ee034 = iVar7;
    do {
      iVar4 = iVar4 + -1;
      if ((*(int *)(iVar7 + 0x84) != 0) && (*(int *)(DAT_010f0048 + 0x88) == 0x20)) {
        DAT_010f0040 = 1;
      }
      if ((*(int *)(iVar7 + 0x88) != 0) && (*(int *)(DAT_010f0044 + 0x88) == 0)) {
        DAT_010f0040 = 1;
        break;
      }
      if (DAT_010f0040 != 0) break;
      DAT_01048248 = 0;
      DAT_01048240 = 0;
      (**(code **)(iVar7 + 0x80))();
      if ((DAT_01048248 != 0) && (uVar5 = 0, DAT_01048248 != 0)) {
        do {
          (*(code *)(&DAT_01048200)[uVar5])();
          uVar5 = uVar5 + 1;
        } while (uVar5 < DAT_01048248);
      }
      if ((DAT_01048240 != 0) && (uVar5 = 0, DAT_01048240 != 0)) {
        puVar6 = &DAT_01048124;
        do {
          (*(code *)puVar6[1])();
          uVar5 = uVar5 + 1;
          puVar6 = puVar6 + 3;
        } while (uVar5 < DAT_01048240);
      }
      iVar7 = DAT_010ed9a4;
      puVar3 = DAT_010481f4;
      if (DAT_010ed9a4 != 0) {
        iVar2 = *(int *)(DAT_010481f4 + 0x114);
        *(int *)(DAT_010481f4 + 0x114) = DAT_010ed9a4;
        DAT_0104824c = iVar7 * 0x94 + DAT_010481f0;
        DAT_01048248 = 0;
        DAT_01048240 = 0;
        (**(code **)(DAT_0104824c + 0x80))();
        if ((DAT_01048248 != 0) && (uVar5 = 0, DAT_01048248 != 0)) {
          do {
            (*(code *)(&DAT_01048200)[uVar5])();
            uVar5 = uVar5 + 1;
          } while (uVar5 < DAT_01048248);
        }
        if ((DAT_01048240 != 0) && (uVar5 = 0, DAT_01048240 != 0)) {
          puVar6 = &DAT_01048124;
          do {
            (*(code *)puVar6[1])();
            uVar5 = uVar5 + 1;
            puVar6 = puVar6 + 3;
          } while (uVar5 < DAT_01048240);
        }
        puVar3 = DAT_010481f4;
        *(int *)(DAT_010481f4 + 0x114) = iVar2;
        DAT_0104824c = iVar2 * 0x94 + DAT_010481f0;
        DAT_010ed9a4 = 0;
      }
      iVar7 = DAT_0104824c;
      if ((*(uint *)(puVar3 + 0x110) & 0x4000) == 0) {
        iVar7 = DAT_0104824c + 0x94;
        *(int *)(puVar3 + 0x114) = *(int *)(puVar3 + 0x114) + 1;
        DAT_0104824c = iVar7;
      }
      else {
        psVar1 = (short *)(puVar3 + 0x122);
        *psVar1 = *psVar1 + -1;
        if (*psVar1 == 0) {
          *(uint *)(puVar3 + 0x110) = *(uint *)(puVar3 + 0x110) & 0xffffbfff;
        }
      }
    } while (iVar4 != 0);
  } while( true );
}


// ===== FUN_004b64a0 @ 004b64a0 (132 bytes) =====


float FUN_004b64a0(void)

{
  float10 fVar1;
  int iVar2;
  float fVar3;
  float10 fVar4;
  
  fVar3 = (float)(*(code *)*DAT_0104824c)();
  iVar2 = DAT_010481f4;
  fVar1 = (float10)0;
  *(uint *)(DAT_010481f4 + 0x110) = *(uint *)(DAT_010481f4 + 0x110) & 0xfffffdff;
  fVar4 = (float10)fVar3;
  if (fVar4 < fVar1 != (fVar4 == fVar1)) {
    *(uint *)(iVar2 + 0x110) = *(uint *)(iVar2 + 0x110) | 0x200;
  }
  *(uint *)(iVar2 + 0x110) = *(uint *)(iVar2 + 0x110) & 0xffffffe7;
  fVar4 = (float10)log2(fVar4);
  fVar4 = (float10)0.6931471805599453 * fVar4 * (float10)3.321928;
  if (fVar4 < fVar1) {
    *(uint *)(iVar2 + 0x110) = *(uint *)(iVar2 + 0x110) | 0x10;
  }
  if (fVar4 == fVar1) {
    *(uint *)(iVar2 + 0x110) = *(uint *)(iVar2 + 0x110) | 8;
  }
  return (float)fVar4;
}


// ===== FUN_004b6530 @ 004b6530 (140 bytes) =====


float FUN_004b6530(void)

{
  int iVar1;
  float fVar2;
  
  fVar2 = (float)(*(code *)*DAT_0104824c)();
  iVar1 = DAT_010481f4;
  *(uint *)(DAT_010481f4 + 0x110) = *(uint *)(DAT_010481f4 + 0x110) & 0xfffffbff;
  if (fVar2 < 0.0 != (fVar2 == 0.0)) {
    *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) | 0x400;
  }
  *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) & 0xffffffe7;
  fVar2 = 1.0 / SQRT(fVar2);
  if (fVar2 < 0.0) {
    *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) | 0x10;
  }
  if (fVar2 == 0.0) {
    *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) | 8;
  }
  return fVar2;
}


// ===== FUN_004b65c0 @ 004b65c0 (110 bytes) =====


undefined4 FUN_004b65c0(void)

{
  int iVar1;
  float fVar2;
  float fVar3;
  
  fVar2 = (float)(*(code *)*DAT_0104824c)();
  fVar3 = (float)(*(code *)DAT_0104824c[1])();
  iVar1 = DAT_010481f4;
  *(uint *)(DAT_010481f4 + 0x110) = *(uint *)(DAT_010481f4 + 0x110) & 0xffffffe7;
  if (ABS(fVar3) - ABS(fVar2) < 0.0) {
    *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) | 0x10;
  }
  if (ABS(fVar3) - ABS(fVar2) == 0.0) {
    *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) | 8;
  }
  return 0;
}


// ===== FUN_004b6630 @ 004b6630 (97 bytes) =====


float FUN_004b6630(void)

{
  int iVar1;
  float fVar2;
  
  fVar2 = (float)(*(code *)*DAT_0104824c)();
  iVar1 = DAT_010481f4;
  *(uint *)(DAT_010481f4 + 0x110) = *(uint *)(DAT_010481f4 + 0x110) & 0xffffffe7;
  fVar2 = ABS(fVar2);
  if (fVar2 < 0.0) {
    *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) | 0x10;
  }
  if (fVar2 == 0.0) {
    *(uint *)(iVar1 + 0x110) = *(uint *)(iVar1 + 0x110) | 8;
  }
  return fVar2;
}


// ===== FUN_004b66a0 @ 004b66a0 (48 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b66a0(void)

{
  uint uVar1;
  undefined4 in_ECX;
  
  uVar1 = (**(code **)(DAT_0104824c + 0x30))();
  if (uVar1 < 0x400) {
    *(undefined4 *)(DAT_010481f8 + uVar1 * 4) = in_ECX;
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004b66ca. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*_DAT_010481ec)();
  return;
}


// ===== FUN_004b6730 @ 004b6730 (1257 bytes) =====


void FUN_004b6730(void)

{
  uint *puVar1;
  int iVar2;
  undefined4 uVar3;
  uint in_EAX;
  uint uVar4;
  undefined4 unaff_ESI;
  uint uVar5;
  
  iVar2 = DAT_0104824c;
  uVar5 = in_EAX >> 0x10 & 0x3f;
  uVar4 = in_EAX & 0xffff;
  switch(in_EAX >> 0x16 & 0x1f) {
  case 0:
    *(undefined **)(DAT_0104824c + 0x68) = &DAT_004b3820;
    return;
  case 1:
    puVar1 = (uint *)(DAT_0104824c + 0x70);
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b37e0;
    *puVar1 = uVar4;
    if (uVar5 == 0x3f) {
      *(uint *)(iVar2 + 0x74) = DAT_010481f4 + 0x80 + (uVar4 >> 0xd) * 4;
      return;
    }
    goto LAB_004b6795;
  case 2:
    puVar1 = (uint *)(DAT_0104824c + 0x70);
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b37b0;
    *puVar1 = uVar4;
    if (uVar5 == 0x3f) {
      *(uint *)(iVar2 + 0x74) = DAT_010481f4 + 0x80 + (uVar4 >> 0xd) * 4;
      return;
    }
LAB_004b6795:
    *(int *)(iVar2 + 0x74) = iVar2 + 0x70;
    return;
  case 3:
    *(uint *)(DAT_0104824c + 0x70) = uVar5;
    *(undefined **)(iVar2 + 0x68) = &DAT_004b37a0;
    return;
  case 4:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b4c80;
    *(undefined **)(iVar2 + 0x6c) = (&PTR_LAB_00572e40)[uVar4 >> 6 & 0x3f];
    return;
  case 5:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b4c40;
    *(undefined **)(iVar2 + 0x6c) = (&PTR_LAB_00573040)[uVar4 >> 6 & 0x3f];
    return;
  default:
    return;
  case 8:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b3770;
    *(uint *)(iVar2 + 0x70) = uVar4;
    *(undefined4 *)(iVar2 + 0x78) = 0xffff;
    return;
  case 9:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b3770;
    *(uint *)(iVar2 + 0x70) = uVar4;
    *(undefined4 *)(iVar2 + 0x78) = 0x7000;
    return;
  case 10:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b3770;
    *(uint *)(iVar2 + 0x70) = uVar4;
    *(undefined4 *)(iVar2 + 0x78) = 0xe00;
    return;
  case 0xb:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b3770;
    *(uint *)(iVar2 + 0x70) = uVar4;
    *(undefined4 *)(iVar2 + 0x78) = 0x80;
    return;
  case 0xc:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b3770;
    *(uint *)(iVar2 + 0x70) = uVar4;
    *(undefined4 *)(iVar2 + 0x78) = 0x10;
    return;
  case 0xd:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b3770;
    *(uint *)(iVar2 + 0x70) = uVar4;
    *(undefined4 *)(iVar2 + 0x78) = 7;
    return;
  case 0x10:
    *(undefined4 *)(DAT_0104824c + 0x6c) = *(undefined4 *)(&DAT_005732c8 + uVar5 * 4);
    *(undefined4 *)(iVar2 + 0x90) = 1;
    if (uVar5 == 0x3f) {
      *(undefined **)(iVar2 + 0x68) = &DAT_004b3760;
    }
    else {
      *(undefined1 **)(iVar2 + 0x68) = &LAB_004b4bf0;
    }
    break;
  case 0x11:
    *(undefined4 *)(DAT_0104824c + 0x6c) = *(undefined4 *)(&DAT_005732c8 + uVar5 * 4);
    *(undefined4 *)(iVar2 + 0x90) = 1;
    if (uVar5 == 0x3f) {
      *(undefined **)(iVar2 + 0x68) = &DAT_004b3750;
    }
    else {
      *(undefined1 **)(iVar2 + 0x68) = &LAB_004b4ba0;
    }
    break;
  case 0x12:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b4b60;
    *(undefined4 *)(iVar2 + 0x90) = 1;
    break;
  case 0x13:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b4af0;
    *(uint *)(iVar2 + 0x70) = in_EAX & 0xfff;
    *(undefined4 *)(iVar2 + 0x90) = 1;
    return;
  case 0x14:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b4aa0;
    goto LAB_004b6973;
  case 0x15:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b4a50;
LAB_004b6973:
    *(uint *)(iVar2 + 0x70) = in_EAX & 0xfff;
    *(uint *)(iVar2 + 0x74) = DAT_010481f4 + 0x80 + (in_EAX >> 0xd & 7) * 4;
    *(undefined4 *)(iVar2 + 0x90) = 1;
    *(int *)(iVar2 + 0x6c) = 1 << ((byte)(in_EAX >> 0x10) & 0xf);
    return;
  case 0x18:
    *(undefined4 *)(DAT_0104824c + 0x6c) = *(undefined4 *)(&DAT_005732c8 + uVar5 * 4);
    *(undefined1 **)(iVar2 + 0x68) = &LAB_004b49e0;
    *(undefined4 *)(iVar2 + 0x90) = 1;
    break;
  case 0x19:
    *(undefined4 *)(DAT_0104824c + 0x6c) = *(undefined4 *)(&DAT_005732c8 + uVar5 * 4);
    *(undefined1 **)(iVar2 + 0x68) = &LAB_004b4970;
    *(undefined4 *)(iVar2 + 0x90) = 1;
    break;
  case 0x1a:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b4910;
    *(undefined4 *)(iVar2 + 0x90) = 1;
    break;
  case 0x1b:
    *(undefined1 **)(DAT_0104824c + 0x68) = &LAB_004b48c0;
    *(undefined4 *)(iVar2 + 0x90) = 1;
    return;
  }
  iVar2 = DAT_0104824c;
  switch(in_EAX >> 0xc & 0xf) {
  case 0:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3880;
    *(uint *)(iVar2 + 0x70) = in_EAX & 0xfff;
    return;
  case 1:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3860;
    *(uint *)(iVar2 + 0x70) = in_EAX & 0xfff;
    return;
  case 2:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3850;
    break;
  case 3:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3830;
    break;
  case 4:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3850;
    if ((in_EAX & 0x800) != 0) {
      uVar3 = FUN_004b3b50();
      *(undefined4 *)(iVar2 + 0x74) = uVar3;
      return;
    }
    goto LAB_004b6339;
  case 5:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3830;
    if ((in_EAX & 0x800) != 0) {
      uVar3 = FUN_004b3b50();
      *(undefined4 *)(iVar2 + 0x74) = uVar3;
      return;
    }
LAB_004b6339:
    uVar3 = FUN_004b3b50();
    *(undefined4 *)(iVar2 + 0x74) = uVar3;
    return;
  case 6:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3850;
    if ((in_EAX & 0x800) != 0) {
      uVar3 = FUN_004b3b20(unaff_ESI);
      *(undefined4 *)(iVar2 + 0x74) = uVar3;
      return;
    }
    goto LAB_004b6397;
  case 7:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b3830;
    if ((in_EAX & 0x800) != 0) {
      uVar3 = FUN_004b3b20(unaff_ESI);
      *(undefined4 *)(iVar2 + 0x74) = uVar3;
      return;
    }
LAB_004b6397:
    uVar3 = FUN_004b3b20(unaff_ESI);
    *(undefined4 *)(iVar2 + 0x74) = uVar3;
    return;
  case 8:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4dd0;
    *(uint *)(iVar2 + 0x70) = in_EAX & 0x3ff;
    return;
  case 9:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4da0;
    *(uint *)(iVar2 + 0x70) = in_EAX & 0x3ff;
    return;
  case 10:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4d80;
    *(uint *)(iVar2 + 0x70) = in_EAX & 0x3ff;
    return;
  case 0xb:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4d50;
    *(uint *)(iVar2 + 0x70) = in_EAX & 0x3ff;
    return;
  case 0xc:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4d30;
    break;
  case 0xd:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4d00;
    break;
  case 0xe:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4ce0;
    break;
  case 0xf:
    *(undefined1 **)(DAT_0104824c + 0x7c) = &LAB_004b4cb0;
    break;
  default:
    goto switchD_004b6292_default;
  }
  *(uint *)(iVar2 + 0x74) = DAT_010481f4 + 0x80 + (in_EAX >> 6 & 7) * 4;
switchD_004b6292_default:
  return;
}


// ===== FUN_004b6ac0 @ 004b6ac0 (199 bytes) =====


void FUN_004b6ac0(void)

{
  int iVar1;
  int iVar2;
  uint in_EAX;
  int in_ECX;
  uint uVar3;
  uint uVar4;
  
  iVar2 = DAT_0104824c;
  uVar3 = in_EAX >> 0x13;
  uVar4 = uVar3 & 0x7f;
  *(uint *)(DAT_0104824c + 0x60) = in_EAX >> 0x1b | in_ECX << 5;
  if (uVar4 < 0x40) {
    *(undefined **)(iVar2 + 0x2c) = (&PTR_LAB_00573040)[uVar4];
    return;
  }
  if (uVar4 < 0x48) {
    *(undefined1 **)(iVar2 + 0x2c) = &LAB_004b66d0;
    *(undefined **)(iVar2 + 0x30) = (&PTR_LAB_005731c0)[in_EAX & 0xf];
    iVar1 = DAT_010481f4;
    *(uint *)(iVar2 + 0x34) = DAT_010481f4 + 0x80 + (uVar3 & 7) * 4;
    *(uint *)(iVar2 + 0x3c) = iVar1 + 0x80 + (in_EAX >> 4 & 7) * 4;
    *(uint *)(iVar2 + 0x40) = in_EAX >> 7 & 0xfff;
    return;
  }
  if (uVar4 < 0x68) {
    *(undefined1 **)(iVar2 + 0x2c) = &LAB_004b5dc0;
    *(undefined **)(iVar2 + 0x4c) = (&PTR_LAB_00573200)[in_EAX & 0xf];
    iVar1 = DAT_010481f4;
    *(uint *)(iVar2 + 0x50) = DAT_010481f4 + 0x80 + (uVar3 & 7) * 4;
    *(uint *)(iVar2 + 0x58) = iVar1 + 0x80 + (in_EAX >> 4 & 7) * 4;
    *(uint *)(iVar2 + 0x5c) = in_EAX >> 7 & 0xfff;
  }
  return;
}


// ===== FUN_004b6b90 @ 004b6b90 (856 bytes) =====


void FUN_004b6b90(void)

{
  int iVar1;
  undefined *puVar2;
  int iVar3;
  int iVar4;
  uint in_EAX;
  uint in_EDX;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  
  iVar4 = DAT_0104824c;
  iVar3 = DAT_010481f4;
  uVar6 = in_EAX >> 0x18;
  uVar7 = uVar6 & 0x7f;
  if ((in_EDX & 0x40) == 0) {
    uVar5 = (in_EDX & 0x3f) * 2;
    if ((int)in_EAX < 0) {
      uVar5 = uVar5 | 1;
    }
    if (uVar5 < 0x40) {
      *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572e40)[uVar5];
    }
    else if (uVar5 < 0x48) {
      iVar1 = DAT_010481f4 + 0x80;
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b6700;
      puVar2 = (&PTR_LAB_005731c0)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x34) = iVar1 + (uVar5 & 7) * 4;
      *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(undefined **)(iVar4 + 0x30) = puVar2;
      *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x3fff;
    }
    else if (uVar5 == 0x58) {
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b3fa0;
      *(uint *)(iVar4 + 0x60) = in_EAX & 0xffffff;
    }
    else if (uVar5 < 0x68) {
      iVar1 = DAT_010481f4 + 0x80;
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b5eb0;
      puVar2 = (&PTR_LAB_00573200)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x50) = iVar1 + (uVar5 & 7) * 4;
      *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(undefined **)(iVar4 + 0x4c) = puVar2;
      *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x3fff;
    }
    if (uVar7 < 0x40) {
LAB_004b6e46:
      *(undefined **)(iVar4 + 0x2c) = (&PTR_LAB_00573040)[uVar7];
      return;
    }
    if (uVar7 < 0x48) {
      *(code **)(iVar4 + 0x2c) = FUN_004b66a0;
      *(undefined **)(iVar4 + 0x30) = (&PTR_LAB_005731c0)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x38) = iVar3 + 0x80 + (uVar6 & 7) * 4;
      *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x3fff;
      return;
    }
    if ((uVar7 != 0x58) && (uVar7 < 0x68)) {
      *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d60;
      *(undefined **)(iVar4 + 0x4c) = (&PTR_LAB_00573200)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x54) = iVar3 + 0x80 + (uVar6 & 7) * 4;
      *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x3fff;
      *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
    }
  }
  else if ((in_EDX & 0x20) == 0) {
    if (uVar7 < 0x40) {
      *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572e40)[uVar7];
    }
    else if (uVar7 < 0x48) {
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b5ed0;
      *(undefined **)(iVar4 + 0x30) = (&PTR_LAB_005731c0)[in_EAX & 0xf];
      iVar3 = DAT_010481f4;
      *(uint *)(iVar4 + 0x34) = DAT_010481f4 + 0x80 + (uVar6 & 7) * 4;
      *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x3fff;
    }
    else if (uVar7 == 0x58) {
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b3fa0;
      *(uint *)(iVar4 + 0x60) = in_EAX & 0xffffff;
    }
    else if (uVar7 < 0x68) {
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b5eb0;
      *(undefined **)(iVar4 + 0x4c) = (&PTR_LAB_00573200)[in_EAX & 0xf];
      iVar3 = DAT_010481f4;
      *(uint *)(iVar4 + 0x50) = DAT_010481f4 + 0x80 + (uVar6 & 7) * 4;
      *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x3fff;
    }
    uVar6 = (in_EDX & 0x1f) * 2;
    *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d00;
    *(uint *)(iVar4 + 100) = uVar6;
    if ((int)in_EAX < 0) {
      *(uint *)(iVar4 + 100) = uVar6 | 1;
      return;
    }
  }
  else {
    uVar5 = (in_EDX & 0x1f) * 2;
    *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b5e50;
    *(uint *)(iVar4 + 100) = uVar5;
    if ((int)in_EAX < 0) {
      *(uint *)(iVar4 + 100) = uVar5 | 1;
    }
    if (uVar7 < 0x40) goto LAB_004b6e46;
    if (uVar7 < 0x48) {
      *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d90;
      *(undefined **)(iVar4 + 0x30) = (&PTR_LAB_005731c0)[in_EAX & 0xf];
      iVar3 = DAT_010481f4;
      *(uint *)(iVar4 + 0x38) = DAT_010481f4 + 0x80 + (uVar6 & 7) * 4;
      *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x3fff;
      return;
    }
    if ((uVar7 != 0x58) && (uVar7 < 0x68)) {
      *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d60;
      *(undefined **)(iVar4 + 0x4c) = (&PTR_LAB_00573200)[in_EAX & 0xf];
      iVar3 = DAT_010481f4;
      *(uint *)(iVar4 + 0x54) = DAT_010481f4 + 0x80 + (uVar6 & 7) * 4;
      *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x3fff;
      *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      return;
    }
  }
  return;
}


// ===== FUN_004b6ef0 @ 004b6ef0 (524 bytes) =====


void FUN_004b6ef0(void)

{
  int iVar1;
  int iVar2;
  uint in_EAX;
  uint in_EDX;
  uint uVar3;
  uint uVar4;
  
  iVar2 = DAT_0104824c;
  iVar1 = DAT_010481f4;
  uVar4 = in_EDX >> 6 & 3;
  uVar3 = in_EAX >> 0x12 & 3;
  if (uVar4 == 0) {
    uVar4 = in_EAX >> 0x1c;
    if ((in_EDX & 1) != 0) {
      uVar4 = uVar4 | 0x10;
    }
    *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572d40)[in_EDX >> 1 & 0x1f];
    *(undefined **)(iVar2 + 0x2c) = (&PTR_LAB_00572f40)[uVar4];
  }
  else if (uVar4 == 1) {
    *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572d40)[in_EDX >> 1 & 0x1f];
    *(undefined1 **)(iVar2 + 0x2c) = &LAB_004b66d0;
    *(undefined **)(iVar2 + 0x30) = (&PTR_LAB_00573140)[in_EAX >> 0x14 & 0xf];
    *(uint *)(iVar2 + 0x34) = iVar1 + 0x80 + (in_EAX >> 0x1e | (in_EDX & 1) * 4) * 4;
    *(uint *)(iVar2 + 0x3c) = iVar1 + 0x80 + (in_EAX >> 0x1b & 7) * 4;
    *(uint *)(iVar2 + 0x40) = in_EAX >> 0x18 & 7;
  }
  else if (uVar4 == 2) {
    *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b6700;
    *(undefined **)(iVar2 + 0x2c) = (&PTR_LAB_00572f40)[in_EDX >> 1 & 0x1f];
    *(undefined **)(iVar2 + 0x30) = (&PTR_LAB_00573140)[in_EAX >> 0x14 & 0xf];
    *(uint *)(iVar2 + 0x34) = iVar1 + 0x80 + (in_EAX >> 0x1e | (in_EDX & 1) * 4) * 4;
    *(uint *)(iVar2 + 0x3c) = iVar1 + 0x80 + (in_EAX >> 0x1b & 7) * 4;
    *(uint *)(iVar2 + 0x40) = in_EAX >> 0x18 & 7;
  }
  if (uVar3 == 0) {
    *(undefined **)(iVar2 + 0x44) = (&PTR_LAB_00572dc0)[in_EAX >> 0xd & 0x1f];
    *(undefined **)(iVar2 + 0x48) = (&PTR_LAB_00572fc0)[in_EAX >> 8 & 0xf];
  }
  else {
    if (uVar3 == 1) {
      *(undefined **)(iVar2 + 0x44) = (&PTR_LAB_00572dc0)[in_EAX >> 0xd & 0x1f];
      *(undefined1 **)(iVar2 + 0x48) = &LAB_004b5dc0;
      *(undefined **)(iVar2 + 0x4c) = (&PTR_LAB_00573180)[in_EAX & 0xf];
      *(uint *)(iVar2 + 0x50) = iVar1 + 0x80 + (in_EAX >> 10 & 7) * 4;
      *(uint *)(iVar2 + 0x58) = iVar1 + 0x80 + (in_EAX >> 7 & 7) * 4;
      *(uint *)(iVar2 + 0x5c) = in_EAX >> 4 & 7;
      return;
    }
    if (uVar3 == 2) {
      *(undefined1 **)(iVar2 + 0x44) = &LAB_004b5eb0;
      *(undefined **)(iVar2 + 0x48) = (&PTR_LAB_00572fc0)[in_EAX >> 0xd & 0x1f];
      *(undefined **)(iVar2 + 0x4c) = (&PTR_LAB_00573180)[in_EAX & 0xf];
      *(uint *)(iVar2 + 0x50) = iVar1 + 0x80 + (in_EAX >> 10 & 7) * 4;
      *(uint *)(iVar2 + 0x58) = iVar1 + 0x80 + (in_EAX >> 7 & 7) * 4;
      *(uint *)(iVar2 + 0x5c) = in_EAX >> 4 & 7;
      return;
    }
  }
  return;
}


// ===== FUN_004b7100 @ 004b7100 (821 bytes) =====


void FUN_004b7100(void)

{
  int iVar1;
  undefined *puVar2;
  int iVar3;
  int iVar4;
  uint in_EAX;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  
  iVar4 = DAT_0104824c;
  iVar3 = DAT_010481f4;
  if ((in_EAX & 0x4000000) == 0) {
    uVar5 = in_EAX >> 0x13;
    uVar7 = in_EAX >> 0xc;
    uVar6 = uVar5 & 0x7f;
    uVar8 = uVar7 & 0x7f;
    if (uVar6 < 0x40) {
      *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572e40)[uVar6];
    }
    else if (uVar6 < 0x48) {
      iVar1 = DAT_010481f4 + 0x80;
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b6700;
      puVar2 = (&PTR_LAB_005731c0)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x34) = iVar1 + (uVar5 & 7) * 4;
      *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(undefined **)(iVar4 + 0x30) = puVar2;
      *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x1f;
    }
    else if (uVar6 == 0x58) {
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b3fa0;
      *(uint *)(iVar4 + 0x60) = in_EAX & 0xfff;
    }
    else if (uVar6 < 0x68) {
      iVar1 = DAT_010481f4 + 0x80;
      *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b5eb0;
      puVar2 = (&PTR_LAB_00573200)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x50) = iVar1 + (uVar5 & 7) * 4;
      *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(undefined **)(iVar4 + 0x4c) = puVar2;
      *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x1f;
    }
    if (uVar8 < 0x40) {
      *(undefined **)(iVar4 + 0x2c) = (&PTR_LAB_00573040)[uVar8];
      return;
    }
    if (uVar8 < 0x48) {
      *(code **)(iVar4 + 0x2c) = FUN_004b66a0;
      *(undefined **)(iVar4 + 0x30) = (&PTR_LAB_005731c0)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x38) = iVar3 + 0x80 + (uVar7 & 7) * 4;
      *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x1f;
      return;
    }
    if ((uVar8 != 0x58) && (uVar8 < 0x68)) {
      *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d60;
      *(undefined **)(iVar4 + 0x4c) = (&PTR_LAB_00573200)[in_EAX & 0xf];
      *(uint *)(iVar4 + 0x54) = iVar3 + 0x80 + (uVar7 & 7) * 4;
      *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x1f;
    }
  }
  else {
    uVar5 = in_EAX >> 0xc;
    uVar6 = uVar5 & 0x7f;
    if ((in_EAX & 0x2000000) == 0) {
      if (uVar6 < 0x40) {
        *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572e40)[uVar6];
      }
      else if (uVar6 < 0x48) {
        *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b6700;
        *(undefined **)(iVar4 + 0x30) = (&PTR_LAB_005731c0)[in_EAX & 0xf];
        iVar3 = DAT_010481f4;
        *(uint *)(iVar4 + 0x34) = DAT_010481f4 + 0x80 + (uVar5 & 7) * 4;
        *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
        *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x1f;
      }
      else if (uVar6 == 0x58) {
        *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b3fa0;
        *(uint *)(iVar4 + 0x60) = in_EAX & 0xfff;
      }
      else if (uVar6 < 0x68) {
        *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b5eb0;
        *(undefined **)(iVar4 + 0x4c) = (&PTR_LAB_00573200)[in_EAX & 0xf];
        iVar3 = DAT_010481f4;
        *(uint *)(iVar4 + 0x50) = DAT_010481f4 + 0x80 + (uVar5 & 7) * 4;
        *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
        *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x1f;
      }
      *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d00;
      *(uint *)(iVar4 + 100) = in_EAX >> 0x13 & 0x3f;
      return;
    }
    *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b5e50;
    *(uint *)(iVar4 + 100) = in_EAX >> 0x13 & 0x3f;
    if (uVar6 < 0x40) {
      *(undefined **)(iVar4 + 0x2c) = (&PTR_LAB_00573040)[uVar6];
      return;
    }
    if (uVar6 < 0x48) {
      *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d90;
      *(undefined **)(iVar4 + 0x30) = (&PTR_LAB_005731c0)[in_EAX & 0xf];
      iVar3 = DAT_010481f4;
      *(uint *)(iVar4 + 0x38) = DAT_010481f4 + 0x80 + (uVar5 & 7) * 4;
      *(uint *)(iVar4 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x40) = in_EAX >> 7 & 0x1f;
      return;
    }
    if ((uVar6 != 0x58) && (uVar6 < 0x68)) {
      *(undefined1 **)(iVar4 + 0x2c) = &LAB_004b5d60;
      *(undefined **)(iVar4 + 0x4c) = (&PTR_LAB_00573200)[in_EAX & 0xf];
      iVar3 = DAT_010481f4;
      *(uint *)(iVar4 + 0x54) = DAT_010481f4 + 0x80 + (uVar5 & 7) * 4;
      *(uint *)(iVar4 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
      *(uint *)(iVar4 + 0x5c) = in_EAX >> 7 & 0x1f;
      return;
    }
  }
  return;
}


// ===== FUN_004b7440 @ 004b7440 (387 bytes) =====


void FUN_004b7440(void)

{
  int iVar1;
  uint in_EAX;
  uint uVar2;
  int iVar3;
  
  iVar1 = DAT_0104824c;
  uVar2 = in_EAX >> 0x18 & 3;
  if (uVar2 == 0) {
    *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572d40)[in_EAX >> 0x14 & 0x1f];
    *(undefined **)(iVar1 + 0x2c) = (&PTR_LAB_00572f40)[in_EAX >> 0xf & 0x1f];
    *(undefined **)(iVar1 + 0x44) = (&PTR_LAB_00572dc0)[in_EAX >> 10 & 0x1f];
    *(undefined **)(iVar1 + 0x48) = (&PTR_LAB_00572fc0)[in_EAX >> 5 & 0x1f];
    return;
  }
  if (uVar2 == 1) {
    *(undefined **)(DAT_0104824c + 0x28) = (&PTR_LAB_00572d40)[in_EAX >> 0x14 & 0x1f];
    *(undefined1 **)(iVar1 + 0x2c) = &LAB_004b66d0;
    *(undefined **)(iVar1 + 0x30) = (&PTR_LAB_00573140)[in_EAX & 0xf];
    iVar3 = DAT_010481f4;
    *(uint *)(iVar1 + 0x34) = DAT_010481f4 + 0x80 + (in_EAX >> 0x11 & 7) * 4;
    *(uint *)(iVar1 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 0xe & 7) * 4;
    *(undefined **)(iVar1 + 0x44) = (&PTR_LAB_00572dc0)[in_EAX >> 10 & 0xf];
    *(undefined1 **)(iVar1 + 0x48) = &LAB_004b5dc0;
  }
  else {
    if (uVar2 != 2) {
      return;
    }
    *(undefined1 **)(DAT_0104824c + 0x28) = &LAB_004b6700;
    *(undefined **)(iVar1 + 0x2c) = (&PTR_LAB_00572f40)[in_EAX >> 0x14 & 0x1f];
    *(undefined **)(iVar1 + 0x30) = (&PTR_LAB_00573140)[in_EAX & 0xf];
    iVar3 = DAT_010481f4;
    *(uint *)(iVar1 + 0x34) = DAT_010481f4 + 0x80 + (in_EAX >> 0x11 & 7) * 4;
    *(uint *)(iVar1 + 0x3c) = iVar3 + 0x80 + (in_EAX >> 0xe & 7) * 4;
    *(undefined1 **)(iVar1 + 0x44) = &LAB_004b5eb0;
    *(undefined **)(iVar1 + 0x48) = (&PTR_LAB_00572fc0)[in_EAX >> 10 & 0xf];
  }
  *(undefined **)(iVar1 + 0x4c) = (&PTR_LAB_00573180)[in_EAX & 0xf];
  *(uint *)(iVar1 + 0x50) = iVar3 + 0x80 + (in_EAX >> 7 & 7) * 4;
  *(uint *)(iVar1 + 0x58) = iVar3 + 0x80 + (in_EAX >> 4 & 7) * 4;
  return;
}


// ===== FUN_004b75d0 @ 004b75d0 (222 bytes) =====


void FUN_004b75d0(void)

{
  code *pcVar1;
  undefined4 *puVar2;
  uint in_ECX;
  undefined *puVar3;
  uint uVar4;
  uint unaff_ESI;
  uint uVar5;
  
  puVar2 = DAT_0104824c;
  *DAT_0104824c = (&PTR_LAB_00572ac0)[in_ECX >> 0x14 & 0xf];
  uVar4 = in_ECX >> 0x18 & 0x1f;
  uVar5 = in_ECX >> 0xf & 0x1f;
  if (uVar4 < 0x10) {
    puVar3 = (&PTR_LAB_00572b40)[uVar5];
  }
  else {
    puVar3 = (&PTR_LAB_00572ac0)[uVar5];
  }
  puVar2[1] = puVar3;
  puVar2[2] = (&PTR_LAB_00572cc0)[in_ECX >> 10 & 0x1f];
  pcVar1 = (code *)(&PTR_FUN_00573248)[uVar4];
  puVar2[3] = pcVar1;
  puVar2[4] = in_ECX;
  if ((((pcVar1 == (code *)&DAT_004b5630) || (pcVar1 == FUN_004b65c0)) ||
      (pcVar1 == (code *)&LAB_004b51e0)) || (pcVar1 == (code *)&LAB_004b38e0)) {
    puVar2[2] = &DAT_004b3b80;
  }
  puVar2[5] = (&PTR_LAB_00572bc0)[in_ECX >> 5 & 0xf];
  uVar4 = in_ECX >> 9 & 1;
  if (uVar4 == 0) {
    puVar3 = (&PTR_LAB_00572bc0)[in_ECX & 0x1f];
  }
  else {
    puVar3 = (&PTR_LAB_00572c40)[in_ECX & 0x1f];
  }
  puVar2[6] = puVar3;
  puVar2[7] = (&PTR_LAB_00572cc0)[unaff_ESI >> 0x1b];
  puVar2[9] = unaff_ESI;
  if (uVar4 != 0) {
    puVar2[8] = FUN_004b4e60;
    return;
  }
  puVar2[8] = &LAB_004b4df0;
  return;
}


// ===== FUN_004b76b0 @ 004b76b0 (232 bytes) =====


void FUN_004b76b0(void)

{
  code *pcVar1;
  undefined4 *puVar2;
  uint in_EAX;
  uint uVar3;
  uint uVar4;
  undefined *puVar5;
  
  puVar2 = DAT_0104824c;
  uVar3 = in_EAX >> 0x14 & 0xf;
  uVar4 = in_EAX >> 0xf & 0x1f;
  if ((in_EAX & 0x200) == 0) {
    DAT_0104824c[5] = (&PTR_LAB_00572bc0)[uVar3];
    uVar3 = in_EAX >> 0x18 & 0x1f;
    if (uVar3 == 0) {
      puVar5 = (&PTR_LAB_00572bc0)[uVar4];
    }
    else {
      puVar5 = (&PTR_LAB_00572c40)[uVar4];
    }
    puVar2[6] = puVar5;
    puVar2[2] = (&PTR_LAB_00572cc0)[in_EAX >> 10 & 0x1f];
    if (uVar3 != 0) {
      puVar2[3] = FUN_004b4e60;
      puVar2[4] = in_EAX;
      return;
    }
    puVar2[3] = &LAB_004b4df0;
    puVar2[4] = in_EAX;
  }
  else {
    *DAT_0104824c = (&PTR_LAB_00572ac0)[uVar3];
    uVar3 = in_EAX >> 0x18 & 0x1f;
    if (uVar3 < 0x10) {
      puVar5 = (&PTR_LAB_00572b40)[uVar4];
    }
    else {
      puVar5 = (&PTR_LAB_00572ac0)[uVar4];
    }
    puVar2[1] = puVar5;
    puVar2[2] = (&PTR_LAB_00572cc0)[in_EAX >> 10 & 0x1f];
    pcVar1 = (code *)(&PTR_FUN_00573248)[uVar3];
    puVar2[4] = in_EAX;
    puVar2[3] = pcVar1;
    if ((((pcVar1 == (code *)&DAT_004b5630) || (pcVar1 == FUN_004b65c0)) ||
        (pcVar1 == (code *)&LAB_004b51e0)) || (pcVar1 == (code *)&LAB_004b38e0)) {
      puVar2[2] = &DAT_004b3b80;
      return;
    }
  }
  return;
}


// ===== FUN_004b77a0 @ 004b77a0 (370 bytes) =====


void FUN_004b77a0(void)

{
  undefined4 *puVar1;
  uint unaff_EBX;
  
  puVar1 = DAT_0104824c;
  DAT_0104824c[0x24] = 0;
  switch(unaff_EBX >> 0x1d) {
  case 0:
    puVar1[0x20] = &LAB_004b36e0;
    FUN_004b75d0();
    FUN_004b7440();
    break;
  case 1:
    puVar1[0x20] = &LAB_004b3690;
    FUN_004b75d0();
    FUN_004b7100();
    break;
  case 2:
    puVar1[0x20] = &LAB_004b3630;
    FUN_004b75d0();
    FUN_004b6730();
    break;
  case 4:
    puVar1[0x20] = &LAB_004b35e0;
    FUN_004b76b0();
    FUN_004b6ef0();
    break;
  case 5:
    puVar1[0x20] = &LAB_004b35a0;
    FUN_004b76b0();
    FUN_004b6b90();
    break;
  case 6:
    puVar1[0x20] = &LAB_004b3560;
    FUN_004b76b0();
    FUN_004b6730();
    break;
  case 7:
    puVar1[0x20] = &LAB_004b3550;
    FUN_004b6ac0();
  }
  if ((((((undefined1 *)*puVar1 == &LAB_004b5ef0) || ((undefined1 *)puVar1[1] == &LAB_004b5ef0)) ||
       ((undefined1 *)puVar1[5] == &LAB_004b5ef0)) ||
      (((undefined1 *)puVar1[6] == &LAB_004b5ef0 || ((undefined1 *)puVar1[10] == &LAB_004b5ef0))))
     || ((undefined1 *)puVar1[0x11] == &LAB_004b5ef0)) {
    puVar1[0x21] = 1;
  }
  else {
    puVar1[0x21] = 0;
  }
  if ((((undefined1 *)puVar1[2] == &LAB_004b5df0) || ((undefined1 *)puVar1[7] == &LAB_004b5df0)) ||
     (((undefined1 *)puVar1[0xb] == &LAB_004b5df0 || ((undefined1 *)puVar1[0x12] == &LAB_004b5df0)))
     ) {
    puVar1[0x22] = 1;
  }
  else {
    puVar1[0x22] = 0;
  }
  if (DAT_010481f0 < puVar1) {
    if ((puVar1[0x21] != 0) && (puVar1[-1] != 0)) {
      puVar1[-4] = 1;
    }
    if ((puVar1[0x22] != 0) && (puVar1[-1] != 0)) {
      puVar1[-3] = 1;
    }
  }
  return;
}


// ===== FUN_004b7940 @ 004b7940 (346 bytes) =====


void FUN_004b7940(uint param_1)

{
  int *in_EAX;
  int iVar1;
  void *pvVar2;
  int iVar3;
  
  DAT_010481f0 = DAT_010ee030;
  if (in_EAX != (int *)0x0) {
    iVar3 = *in_EAX;
    iVar1 = FUN_00401050(iVar3,*(undefined4 *)(iVar3 + 4));
    if (in_EAX[1] == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
      FUN_004a68db("list<T> too long");
    }
    in_EAX[1] = in_EAX[1] + 1;
    *(int *)(iVar3 + 4) = iVar1;
    **(int **)(iVar1 + 4) = iVar1;
  }
  iVar3 = 0;
  if (DAT_010481f0 != (void *)0x0) {
    _free(DAT_010481f0);
  }
  param_1 = param_1 >> 3;
  DAT_010481f4 = &DAT_010edef0;
  pvVar2 = _malloc(param_1 * 0x94);
  DAT_010481f0 = pvVar2;
  DAT_010ee030 = pvVar2;
  for (; DAT_0104824c = pvVar2, param_1 != 0; param_1 = param_1 - 1) {
    *(int *)((int)pvVar2 + 0x8c) = iVar3;
    FUN_004b77a0();
    if (((*(undefined1 **)((int)pvVar2 + 0xc) == &LAB_004b38e0) &&
        (*(undefined **)((int)pvVar2 + 0x68) == &DAT_004b3820)) &&
       (*(undefined1 **)((int)pvVar2 + -0x2c) == &LAB_004b37e0)) {
      *(undefined **)((int)pvVar2 + -0x2c) = &DAT_004b3820;
    }
    pvVar2 = (void *)((int)pvVar2 + 0x94);
    iVar3 = iVar3 + 1;
  }
  DAT_010ee030 = DAT_010481f0;
  _memset(&DAT_010edef0,0,0x140);
  DAT_010ee004 = 0;
  DAT_010ee034 = DAT_010ee030;
  return;
}


// ===== FUN_004b7ab0 @ 004b7ab0 (11 bytes) =====


void FUN_004b7ab0(void)

{
  DAT_010ed9a0 = 0xffffffff;
  return;
}


// ===== FUN_004b7ac0 @ 004b7ac0 (14 bytes) =====


bool FUN_004b7ac0(uint param_1)

{
  return (param_1 & 0xff000000) == 0;
}


// ===== FUN_004b7ad0 @ 004b7ad0 (153 bytes) =====


int FUN_004b7ad0(void)

{
  int in_EAX;
  uint in_ECX;
  
  if (0x400 < in_ECX) {
    return 0;
  }
  if ((in_EAX == 1) && (DAT_010ed964 != 0)) {
    in_EAX = 0;
  }
  if (DAT_0057285a == '\0') {
    (**(code **)(*DAT_00dcec00 + 0x5c))(DAT_00dcec00);
    DAT_00dceff0 = DAT_00dceff0 + 1;
    return in_EAX;
  }
  (**(code **)(*DAT_00dcec00 + 0x5c))(DAT_00dcec00);
  DAT_00dceff0 = DAT_00dceff0 + 1;
  return in_EAX;
}


// ===== FUN_004b7b70 @ 004b7b70 (42 bytes) =====


void FUN_004b7b70(void)

{
  int in_EAX;
  undefined4 uVar1;
  int in_ECX;
  int in_EDX;
  int iVar2;
  
  uVar1 = 0;
  if (in_ECX != 0) {
    iVar2 = (in_ECX - 1U >> 1) + 1;
    do {
      uVar1 = FUN_004553a0(uVar1,in_EAX,in_EDX * 2);
      in_EAX = in_EAX + 0x400;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  return;
}


// ===== FUN_004b7ba0 @ 004b7ba0 (184 bytes) =====


void FUN_004b7ba0(void)

{
  uint uVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  char local_100 [256];
  
  CreateDirectoryA("TEXCACHE",(LPSECURITY_ATTRIBUTES)0x0);
  piVar5 = (int *)*DAT_010f00f8;
  piVar3 = DAT_010f00f8;
  if (piVar5 != DAT_010f00f8) {
    do {
      iVar6 = *(int *)(piVar5[3] + 0x2c);
      if (iVar6 != 0) {
        uVar1 = *(uint *)(piVar5[3] + 0xc);
        iVar4 = 0x20 << ((byte)(uVar1 >> 3) & 7);
        uVar2 = 0;
        if (iVar4 != 0) {
          iVar4 = (iVar4 - 1U >> 1) + 1;
          do {
            uVar2 = FUN_004553a0(uVar2,iVar6,(0x20 << ((byte)uVar1 & 7)) * 2);
            iVar6 = iVar6 + 0x400;
            iVar4 = iVar4 + -1;
          } while (iVar4 != 0);
        }
        _sprintf(local_100,"TEXCACHE\\%X_%X.png",piVar5[2],uVar2);
        D3DXSaveTextureToFileA(local_100,3,*(undefined4 *)(piVar5[3] + 0x10),0);
        piVar3 = DAT_010f00f8;
      }
      piVar5 = (int *)*piVar5;
    } while (piVar5 != piVar3);
  }
  return;
}


// ===== FUN_004b7c60 @ 004b7c60 (144 bytes) =====


void FUN_004b7c60(void)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int iVar5;
  int *piVar6;
  
  puVar4 = DAT_010f00f8;
  puVar1 = (undefined4 *)*DAT_010f00f8;
  DAT_00dceff0 = 0;
  puVar3 = DAT_010f017c;
  for (; DAT_010f017c = puVar3, puVar1 != puVar4; puVar1 = (undefined4 *)*puVar1) {
    iVar2 = puVar1[3];
    if ((*(int *)(iVar2 + 0x10) != 0) && (*(char *)(iVar2 + 0x29) == '\0')) {
      do {
        iVar5 = (**(code **)(**(int **)(iVar2 + 0x10) + 8))(*(int **)(iVar2 + 0x10));
      } while (iVar5 != 0);
      DAT_00dceff0 = DAT_00dceff0 + -1;
    }
    FUN_0048d1c3(iVar2);
    puVar3 = DAT_010f017c;
  }
  for (puVar1 = (undefined4 *)*puVar3; puVar1 != puVar3; puVar1 = (undefined4 *)*puVar1) {
    piVar6 = *(int **)puVar1[3];
    if (piVar6 != (int *)puVar1[3]) {
      do {
        (**(code **)(*(int *)piVar6[4] + 8))((int *)piVar6[4]);
        piVar6 = (int *)*piVar6;
      } while (piVar6 != (int *)puVar1[3]);
    }
  }
  FUN_004535f0();
  return;
}


// ===== FUN_004b7cf0 @ 004b7cf0 (367 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b7cf0(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int *local_30;
  int *local_2c;
  uint local_28;
  uint local_24;
  int *local_20;
  int *local_1c;
  uint local_18;
  undefined4 local_14;
  int *local_10 [3];
  
  local_30 = (int *)**(int **)(&DAT_010f0124 + (DAT_010ed99c ^ 1) * 0x2c);
  if (local_30 != *(int **)(&DAT_010f0124 + (DAT_010ed99c ^ 1) * 0x2c)) {
    do {
      uVar6 = local_30[2] & 0x1fff003f;
      iVar4 = 0;
      local_24 = uVar6;
      do {
        uVar5 = iVar4 << 0xd | uVar6;
        local_28 = uVar5;
        FUN_004538b0(&local_20);
        if (local_20 != local_1c) {
          iVar3 = 0;
          piVar1 = local_20;
          do {
            piVar1 = (int *)*piVar1;
            iVar3 = iVar3 + 1;
          } while (piVar1 != local_1c);
          if (iVar3 != 0) {
            local_28 = uVar5;
            FUN_004537f0(&local_2c);
            if (local_2c == DAT_010f00f8) {
              iVar3 = *DAT_010f00f8;
              local_14 = 0;
              local_18 = uVar5;
              iVar2 = FUN_00454750(iVar3,*(undefined4 *)(iVar3 + 4));
              if (_DAT_010f00fc == 0x1ffffffe) {
                    /* WARNING: Subroutine does not return */
                FUN_004a68db("list<T> too long");
              }
              _DAT_010f00fc = _DAT_010f00fc + 1;
              *(int *)(iVar3 + 4) = iVar2;
              **(int **)(iVar2 + 4) = iVar2;
              FUN_00454180(&DAT_010f00f4,local_10,*DAT_010f00f8 + 8,*DAT_010f00f8);
              local_2c = local_10[0];
              uVar6 = local_24;
            }
            if (local_2c[3] != 0) {
              *(undefined1 *)(local_2c[3] + 0x28) = 1;
            }
          }
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < 8);
      local_30 = (int *)*local_30;
    } while (local_30 != (int *)*(int *)(&DAT_010f0124 + (DAT_010ed99c ^ 1) * 0x2c));
  }
  FUN_004533d0();
  DAT_010ed99c = DAT_010ed99c ^ 1;
  return;
}


// ===== FUN_004b7e60 @ 004b7e60 (93 bytes) =====


void FUN_004b7e60(int param_1)

{
  undefined4 *puVar1;
  void *local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0050421a;
  local_10 = ExceptionList;
  if (param_1 != 0) {
    local_8 = 0;
    ExceptionList = &local_10;
    puVar1 = (undefined4 *)FUN_004532e0();
    *puVar1 = 0;
  }
  ExceptionList = local_10;
  return;
}


// ===== Catch_All@004b7ebd @ 004b7ebd (6 bytes) =====


undefined4 Catch_All_004b7ebd(void)

{
  return 0x4b7eaa;
}


// ===== FUN_004b7ed0 @ 004b7ed0 (518 bytes) =====


void FUN_004b7ed0(void)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  int local_40;
  char *local_3c;
  int *piStack_38;
  int *piStack_34;
  int *local_30;
  int local_2c;
  undefined **local_24 [4];
  void *pvStack_14;
  undefined1 *puStack_10;
  undefined4 local_c;
  
  piVar3 = DAT_010f00f8;
  local_c = 0xffffffff;
  puStack_10 = &LAB_0050422c;
  pvStack_14 = ExceptionList;
  piVar6 = (int *)*DAT_010f00f8;
  iVar7 = 0;
  local_2c = 0;
  ExceptionList = &pvStack_14;
  piVar4 = (int *)FUN_0048d1ce(0xc);
  if (piVar4 == (int *)0x0) {
    local_3c = (char *)0x0;
    std::exception::exception((exception *)local_24,&local_3c);
    local_24[0] = std::bad_alloc::vftable;
                    /* WARNING: Subroutine does not return */
    __CxxThrowException_8(local_24,&DAT_005250a0);
  }
  *piVar4 = (int)piVar4;
  piVar4[1] = (int)piVar4;
  local_c = 0;
  piVar1 = (int *)*piVar4;
  *piVar4 = (int)piVar4;
  piVar4[1] = (int)piVar4;
  local_2c = 0;
  local_30 = piVar4;
  while (piVar1 != piVar4) {
    piVar2 = (int *)*piVar1;
    FUN_0048d1c3(piVar1);
    piVar1 = piVar2;
  }
  if (piVar6 != piVar3) {
    do {
      local_40 = piVar6[2];
      iVar5 = piVar6[3];
      piVar6 = (int *)*piVar6;
      if ((*(int *)(iVar5 + 0x14) != -0x54545455) &&
         (200 < (uint)(DAT_005aa76c - *(int *)(iVar5 + 0x14)))) {
        if ((*(int *)(iVar5 + 0x10) != 0) && (*(char *)(iVar5 + 0x29) == '\0')) {
          (**(code **)(**(int **)(iVar5 + 0x10) + 8))(*(int **)(iVar5 + 0x10));
          DAT_00dceff0 = DAT_00dceff0 + -1;
        }
        FUN_0048d1c3(iVar5);
        iVar5 = FUN_00402b90(piVar4,piVar4[1],&local_40);
        if (iVar7 == 0x3ffffffe) {
                    /* WARNING: Subroutine does not return */
          FUN_004a68db("list<T> too long");
        }
        piVar4[1] = iVar5;
        iVar7 = iVar7 + 1;
        **(int **)(iVar5 + 4) = iVar5;
        local_2c = iVar7;
      }
    } while (piVar6 != piVar3);
    while (iVar7 != 0) {
      local_40 = *(int *)(piVar4[1] + 8);
      FUN_004538b0(&piStack_38);
      piVar3 = piStack_34;
      for (piVar6 = piStack_38; piVar6 != piStack_34; piVar6 = (int *)*piVar6) {
      }
      piVar6 = piStack_38;
      if ((piStack_38 == (int *)*DAT_010f00f8) && (piStack_34 == DAT_010f00f8)) {
        FUN_004533d0();
      }
      else {
        while (piVar6 != piVar3) {
          piVar1 = (int *)*piVar6;
          FUN_00453c20(piVar6);
          piVar6 = piVar1;
          iVar7 = local_2c;
        }
      }
      piVar6 = (int *)piVar4[1];
      if (piVar6 != piVar4) {
        *(int *)piVar6[1] = *piVar6;
        *(int *)(*piVar6 + 4) = piVar6[1];
        FUN_0048d1c3(piVar6);
        iVar7 = iVar7 + -1;
        local_2c = iVar7;
      }
    }
  }
  piVar6 = (int *)*piVar4;
  *piVar4 = (int)piVar4;
  piVar4[1] = (int)piVar4;
  while (piVar6 != piVar4) {
    piVar3 = (int *)*piVar6;
    FUN_0048d1c3(piVar6);
    piVar6 = piVar3;
  }
  FUN_0048d1c3(piVar4);
  ExceptionList = pvStack_14;
  return;
}


// ===== FUN_004b80e0 @ 004b80e0 (763 bytes) =====


void FUN_004b80e0(int param_1,ushort *param_2)

{
  ushort uVar1;
  ushort uVar2;
  int in_EAX;
  int *piVar3;
  uint uVar4;
  int *piVar5;
  undefined *puVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  ushort *unaff_EBP;
  int *piVar10;
  int iVar11;
  int *piVar12;
  ushort *puVar13;
  int unaff_retaddr;
  undefined4 in_stack_00000018;
  int *piVar14;
  int iStack_30;
  uint uStack_1c;
  int *piStack_18;
  int iStack_10;
  uint uStack_c;
  uint local_8;
  int local_4;
  
  puVar13 = (ushort *)(in_EAX + (int)param_2);
  piVar3 = *(int **)(param_1 + 0x10);
  if (piVar3 != (int *)0x0) {
    local_4 = 0;
    (**(code **)(*piVar3 + 0x4c))(piVar3,in_stack_00000018,&local_8,0,0);
    uVar7 = local_8;
    if (piStack_18 != (int *)0x0) {
      iVar11 = param_1;
      if (param_2 == (ushort *)0x0) {
        *piStack_18 = (-(uint)(unaff_retaddr != 0) & 0x1000000) - 0xf0f10;
      }
      else {
        uVar8 = local_4 - 1;
        *(ushort **)(param_1 + 0x1c) = puVar13;
        *(ushort **)(param_1 + 0x2c) = puVar13;
        if (0x100000 < (int)param_2 + local_8 + uVar8 * 0x400) {
          return;
        }
        uVar4 = uStack_1c >> 2;
        piVar3 = piStack_18;
        if (unaff_retaddr != 0) {
          piVar3 = _malloc(local_8 * local_4 * 4);
          uVar4 = uVar7;
        }
        if (local_4 != 0) {
          piVar5 = piVar3 + uVar4;
          iStack_30 = (uVar8 >> 1) + 1;
          piVar14 = piVar3;
          do {
            param_2 = puVar13;
            if (uVar7 != 0) {
              iVar11 = (local_8 - 1 >> 1) + 1;
              uVar8 = uStack_c & 0x3ff;
              piVar10 = piVar5;
              piVar12 = piVar14;
              do {
                uVar1 = *param_2;
                if (uVar8 == 0x3fe) {
                  if ((uStack_c & 0x80000) == 0) {
                    param_2 = param_2 + 0x3fe00;
                  }
                  else {
                    param_2 = param_2 + -0x3fe00;
                  }
                  uVar8 = 0;
                }
                if ((uVar1 >> 0xc == 0xf) && (unaff_retaddr != 0)) {
                  puVar6 = &DAT_00f0f0f0;
                }
                else {
                  uVar7 = (uint)((byte)(uVar1 >> 8) & 0xf0);
                  puVar6 = (undefined *)(((uVar7 | 0xffffff00) << 8 | uVar7) << 8 | uVar7);
                }
                *piVar12 = (int)puVar6;
                uVar2 = uVar1 >> 8 & 0xf;
                if ((uVar2 == 0xf) && (unaff_retaddr != 0)) {
                  puVar6 = &DAT_00f0f0f0;
                }
                else {
                  uVar7 = (uint)(byte)((char)uVar2 << 4);
                  puVar6 = (undefined *)(((uVar7 | 0xffffff00) << 8 | uVar7) << 8 | uVar7);
                }
                piVar12[1] = (int)puVar6;
                uVar2 = uVar1 >> 4 & 0xf;
                if ((uVar2 == 0xf) && (unaff_retaddr != 0)) {
                  puVar6 = &DAT_00f0f0f0;
                }
                else {
                  uVar7 = (uint)(byte)((char)uVar2 << 4);
                  puVar6 = (undefined *)(((uVar7 | 0xffffff00) << 8 | uVar7) << 8 | uVar7);
                }
                *piVar10 = (int)puVar6;
                if (((uVar1 & 0xf) == 0xf) && (unaff_retaddr != 0)) {
                  puVar6 = &DAT_00f0f0f0;
                }
                else {
                  uVar7 = (uint)(byte)((char)(uVar1 & 0xf) << 4);
                  puVar6 = (undefined *)(((uVar7 | 0xffffff00) << 8 | uVar7) << 8 | uVar7);
                }
                param_2 = param_2 + 1;
                piVar10[1] = (int)puVar6;
                uVar8 = uVar8 + 2;
                piVar12 = piVar12 + 2;
                piVar10 = piVar10 + 2;
                iVar11 = iVar11 + -1;
                uVar7 = local_8;
                puVar13 = unaff_EBP;
              } while (iVar11 != 0);
            }
            piVar14 = piVar14 + uVar4 * 2;
            puVar13 = puVar13 + 0x200;
            piVar5 = piVar5 + uVar4 * 2;
            iStack_30 = iStack_30 + -1;
            iVar11 = iStack_10;
            unaff_EBP = puVar13;
          } while (iStack_30 != 0);
        }
        if (unaff_retaddr != 0) {
          if (local_4 != 0) {
            iVar9 = local_4;
            piVar5 = piVar3;
            do {
              FUN_00453210(piVar5,local_8);
              FID_conflict__memcpy(piStack_18,piVar5,uVar7 * 4);
              piStack_18 = piStack_18 + (uStack_1c >> 2);
              piVar5 = piVar5 + uVar7;
              iVar9 = iVar9 + -1;
              iVar11 = iStack_10;
            } while (iVar9 != 0);
          }
          _free(piVar3);
        }
        *(ushort **)(iVar11 + 0x24) = param_2 + -2;
        *(undefined4 *)(iVar11 + 0x18) = **(undefined4 **)(iVar11 + 0x1c);
        *(undefined4 *)(iVar11 + 0x20) = *(undefined4 *)(param_2 + -2);
      }
      (**(code **)(**(int **)(iVar11 + 0x10) + 0x50))(*(int **)(iVar11 + 0x10),param_1);
      *(undefined1 *)(iVar11 + 0x28) = 0;
    }
  }
  return;
}


// ===== FUN_004b83e0 @ 004b83e0 (47 bytes) =====


void FUN_004b83e0(void)

{
  uint in_ECX;
  
  FUN_004b7e60(*(undefined4 *)
                (&DAT_01046120 + (((in_ECX & 0x3ff) >> 5) * 0x40 + (in_ECX >> 0xe)) * 4));
  *(undefined4 *)(&DAT_01046120 + (((in_ECX & 0x3ff) >> 5) * 0x40 + (in_ECX >> 0xe)) * 4) = 0;
  return;
}


// ===== FUN_004b8410 @ 004b8410 (47 bytes) =====


void FUN_004b8410(void)

{
  uint in_ECX;
  
  FUN_004b7e60(*(undefined4 *)
                (&DAT_01044120 + (((in_ECX & 0x3ff) >> 5) * 0x40 + (in_ECX >> 0xe)) * 4));
  *(undefined4 *)(&DAT_01044120 + (((in_ECX & 0x3ff) >> 5) * 0x40 + (in_ECX >> 0xe)) * 4) = 0;
  return;
}


// ===== FUN_004b8440 @ 004b8440 (385 bytes) =====


void FUN_004b8440(void)

{
  FILE *_File;
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined1 local_41c [4];
  undefined4 local_418;
  undefined1 local_414 [4];
  undefined1 local_410;
  undefined4 local_40c;
  undefined1 local_408 [4];
  char local_404;
  undefined1 local_403 [255];
  char local_304 [256];
  undefined1 local_204 [256];
  CHAR local_104 [260];
  
  GetCurrentDirectoryA(0xff,local_104);
  SetCurrentDirectoryA("SCRIPTS");
  _sprintf(local_304,"%s.pat",&DAT_01044100);
  _File = _fopen(local_304,"rt");
  if (_File != (FILE *)0x0) {
    iVar1 = _feof(_File);
    while (iVar1 == 0) {
      local_404 = '\0';
      _memset(local_403,0,0xff);
      _fgets(&local_404,0xff,_File);
      if (local_404 == '\0') break;
      if ((local_404 != '/') &&
         (iVar1 = FID_conflict__sscanf
                            (&local_404,"%x %x %x %s",local_414,&local_418,local_41c,local_204),
         iVar1 == 4)) {
        local_40c = local_418;
        local_410 = local_41c[0];
        iVar1 = D3DXCreateTextureFromFileA(DAT_00dcec00,local_204,local_408);
        if (iVar1 == 0) {
          piVar2 = (int *)FUN_00453460();
          iVar1 = *piVar2;
          iVar3 = FUN_00453d00(iVar1,*(undefined4 *)(iVar1 + 4));
          if (piVar2[1] == 0x15555554) {
                    /* WARNING: Subroutine does not return */
            FUN_004a68db("list<T> too long");
          }
          piVar2[1] = piVar2[1] + 1;
          *(int *)(iVar1 + 4) = iVar3;
          **(int **)(iVar3 + 4) = iVar3;
        }
      }
      iVar1 = _feof(_File);
    }
    _fclose(_File);
  }
  SetCurrentDirectoryA(local_104);
  return;
}


// ===== FUN_004b85d0 @ 004b85d0 (243 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b85d0(void)

{
  char cVar1;
  char *in_EAX;
  int iVar2;
  
  if (in_EAX != (char *)0x0) {
    iVar2 = (int)&DAT_01044100 - (int)in_EAX;
    do {
      cVar1 = *in_EAX;
      in_EAX[iVar2] = cVar1;
      in_EAX = in_EAX + 1;
    } while (cVar1 != '\0');
  }
  DAT_00dceff0 = 0;
  FUN_004533d0();
  _memset(&DAT_01043100,0xff,0x1000);
  _DAT_01003020 = 0xffffffff;
  _DAT_01003024 = 0xffffffff;
  _DAT_01003028 = 0xffffffff;
  _DAT_0100302c = 0xffffffff;
  _DAT_01003030 = 0xffffffff;
  _DAT_01003034 = 0xffffffff;
  _DAT_01003038 = 0xffffffff;
  _DAT_0100303c = 0xffffffff;
  _DAT_01023070 = 0xffffffff;
  _DAT_01023074 = 0xffffffff;
  _DAT_01023078 = 0xffffffff;
  _DAT_0102307c = 0xffffffff;
  _DAT_01023080 = 0xffffffff;
  _DAT_01023084 = 0xffffffff;
  _DAT_01023088 = 0xffffffff;
  _DAT_0102308c = 0xffffffff;
  _DAT_005aeb50 = 0xffffffff;
  _DAT_005aeb54 = 0xffffffff;
  _DAT_005aeb58 = 0xffffffff;
  _DAT_005aeb5c = 0xffffffff;
  _DAT_005aeb60 = 0xffffffff;
  _DAT_005aeb64 = 0xffffffff;
  _DAT_005aeb68 = 0xffffffff;
  _DAT_005aeb6c = 0xffffffff;
  _memset(&DAT_01044120,0,0x2000);
  _memset(&DAT_01046120,0,0x2000);
  FUN_004b8440();
  return;
}


// ===== FUN_004b86d0 @ 004b86d0 (1659 bytes) =====


int * FUN_004b86d0(uint param_1,uint param_2,uint param_3,undefined1 *param_4)

{
  undefined4 *puVar1;
  uint *puVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  undefined4 uVar9;
  uint local_40;
  uint *local_3c;
  uint local_38;
  uint uStack_34;
  uint uStack_30;
  uint uStack_2c;
  int local_28;
  uint local_24;
  int local_20;
  int iStack_1c;
  uint local_18;
  undefined1 uStack_14;
  uint uStack_c;
  
  uVar7 = (param_2 << 0x10 | param_1) & 0x1fffe03f;
  uVar8 = param_1 & 0x103f;
  local_18 = (uint)CONCAT21(CONCAT11((&DAT_005aeb50)[param_3 >> 10 & 0x1f],
                                     (&DAT_01023070)[param_3 >> 5 & 0x1f]),
                            (&DAT_01003020)[param_3 & 0x1f]);
  if (uVar7 == DAT_010ed9a0) {
    return (int *)0x0;
  }
  local_40 = uVar7;
  local_24 = uVar8;
  FUN_004babc0();
  local_20 = DAT_010f00f8;
  DAT_010ed9a0 = uVar7;
  FUN_004537f0(&local_28);
  if (local_28 == local_20) {
    puVar2 = (uint *)0x0;
  }
  else {
    puVar1 = (undefined4 *)FUN_004532e0();
    puVar2 = (uint *)*puVar1;
    uVar8 = local_24;
  }
  local_3c = puVar2;
  if (puVar2 == (uint *)0x0) {
    puVar2 = (uint *)FUN_0048d1ce(0x30);
    local_3c = puVar2;
    puVar1 = (undefined4 *)FUN_004532e0();
    *puVar1 = puVar2;
  }
  else {
    piVar4 = (int *)puVar2[4];
    if (piVar4 == (int *)0x0) {
      return (int *)0x0;
    }
    if ((char)puVar2[10] == '\0') {
      puVar2[5] = DAT_005aa76c;
      *param_4 = *(undefined1 *)((int)puVar2 + 0x2a);
      return piVar4;
    }
    if (puVar2[3] == uVar8) goto LAB_004b8972;
    (**(code **)(*piVar4 + 8))(piVar4);
    DAT_00dceff0 = DAT_00dceff0 + -1;
  }
  iVar5 = DAT_010f017c;
  local_38 = param_1 & 0x4000;
  if ((local_38 == 0) || (FUN_00453ab0(&local_20,&local_40), local_20 == iVar5)) {
LAB_004b88b6:
    if (local_38 == 0) {
      uStack_2c = 0;
      if (DAT_0057285a == '\0') {
        uVar9 = 0;
      }
      else {
        uVar9 = 0x200;
      }
      (**(code **)(*DAT_00dcec00 + 0x5c))
                (DAT_00dcec00,1,1,DAT_010ed964 == 0,uVar9,0x15,DAT_0057285a == '\0',&uStack_2c,0);
      DAT_00dceff0 = DAT_00dceff0 + 1;
      puVar2[4] = uStack_2c;
    }
    else if ((uint)(0x20 << ((byte)(param_1 >> 3) & 7)) < 0x401) {
      uVar8 = FUN_004b7ad0();
      puVar2[4] = uVar8;
    }
    else {
      puVar2[4] = 0;
    }
    *(undefined2 *)((int)puVar2 + 0x29) = 0;
  }
  else {
    FUN_004531e0();
    iVar3 = FUN_004b7b70();
    piVar4 = (int *)FUN_00453460();
    iVar5 = 0;
    if (piVar4[1] == 1) {
      puVar1 = *(undefined4 **)*piVar4;
LAB_004b8878:
      uStack_14 = (undefined1)puVar1[2];
      iVar5 = puVar1[3];
      uStack_c = puVar1[4];
    }
    else {
      for (puVar1 = *(undefined4 **)*piVar4; puVar1 != (undefined4 *)*piVar4;
          puVar1 = (undefined4 *)*puVar1) {
        if (puVar1[3] == iVar3) goto LAB_004b8878;
      }
    }
    if (iVar3 != iVar5) goto LAB_004b88b6;
    *(undefined1 *)((int)puVar2 + 0x29) = 1;
    *(undefined1 *)((int)puVar2 + 0x2a) = uStack_14;
    puVar2[4] = uStack_c;
  }
  puVar2[3] = local_24;
  puVar2[0xb] = 0;
LAB_004b8972:
  if (*(char *)((int)puVar2 + 0x29) == '\0') {
    uStack_34 = ((param_2 & 0xfff) >> 6 & 0x1f) * 0x20;
    uVar8 = uStack_34;
    if ((param_2 & 0x20) != 0) {
      uVar8 = uStack_34 + 0x400;
    }
    DAT_00dcec54 = DAT_00dcec54 + 1;
    iStack_1c = (uVar8 >> 1) * 0x400 + (param_2 & 0x1f) * 0x20;
    iVar3 = 0x20 << ((byte)param_1 & 7);
    iVar5 = 0x20 << ((byte)(param_1 >> 3) & 7);
    uVar8 = param_2 & 0x1f;
    if ((param_2 & 0x20) != 0) {
      uStack_34 = uStack_34 + 0x400;
    }
    local_38 = param_1 & 0x4000;
    if (local_38 != 0) {
      if ((param_2 & 0x1000) == 0) {
        if ((uStack_34 + iVar5 < 0x801) && (uVar8 * 0x20 + iVar3 < 0x401)) {
          local_24 = (int)(iVar5 + (iVar5 >> 0x1f & 0x1fU)) >> 5;
          local_28 = 0;
          if (0 < (int)local_24) {
            uVar6 = (int)((iVar3 >> 0x1f & 0x1fU) + iVar3) >> 5;
            uVar7 = local_40;
            uStack_30 = uVar6;
            do {
              if (0 < (int)uVar6) {
                puVar2 = (uint *)(&DAT_01044120 + ((uStack_34 >> 5) + uVar8 * 0x40 + local_28) * 4);
                local_20 = uStack_30;
                do {
                  uVar6 = *puVar2;
                  if ((uVar6 != 0) && (((uVar6 ^ uVar7) & 0x1fff003f) != 0)) {
                    FUN_004b7e60(uVar6);
                    uVar7 = local_40;
                  }
                  *puVar2 = uVar7;
                  puVar2 = puVar2 + 0x40;
                  local_20 = local_20 + -1;
                } while (local_20 != 0);
                local_20 = 0;
                uVar6 = uStack_30;
              }
              local_28 = local_28 + 1;
            } while (local_28 < (int)local_24);
          }
        }
      }
      else if ((uStack_34 + iVar5 < 0x801) && (uVar8 * 0x20 + iVar3 < 0x401)) {
        local_24 = (int)(iVar5 + (iVar5 >> 0x1f & 0x1fU)) >> 5;
        local_28 = 0;
        if (0 < (int)local_24) {
          uVar6 = (int)((iVar3 >> 0x1f & 0x1fU) + iVar3) >> 5;
          uVar7 = local_40;
          uStack_30 = uVar6;
          do {
            if (0 < (int)uVar6) {
              puVar2 = (uint *)(&DAT_01046120 + ((uStack_34 >> 5) + uVar8 * 0x40 + local_28) * 4);
              local_20 = uStack_30;
              do {
                uVar6 = *puVar2;
                if ((uVar6 != 0) && (((uVar6 ^ uVar7) & 0x1fff003f) != 0)) {
                  FUN_004b7e60(uVar6);
                  uVar7 = local_40;
                }
                *puVar2 = uVar7;
                puVar2 = puVar2 + 0x40;
                local_20 = local_20 + -1;
              } while (local_20 != 0);
              local_20 = 0;
              uVar6 = uStack_30;
            }
            local_28 = local_28 + 1;
          } while (local_28 < (int)local_24);
        }
      }
    }
    uStack_30 = param_2 & 0x1000;
    uStack_34 = param_1 & 0x2000;
    FUN_004b80e0(local_3c,iStack_1c,iVar3,iVar5,uStack_34,0,local_38);
    puVar2 = local_3c;
    if (DAT_010ed960 != 0) {
      uVar8 = param_2 >> 6 & 0x1f;
      param_2 = param_2 & 0x3f;
      FUN_004b80e0(local_3c,(uVar8 + 0x60) * 0x2000 + param_2 * 0x10,iVar3 >> 1,iVar5 >> 1,uStack_34
                   ,1,local_38);
      FUN_004b80e0(local_3c,(uVar8 * 8 + 0x700 >> 1) * 0x400 + param_2 * 8 + 0x200,iVar3 >> 2,
                   iVar5 >> 2,uStack_34,2,local_38);
      FUN_004b80e0(local_3c,(uVar8 * 4 + 0x780 >> 1) * 0x400 + param_2 * 4 + 0x300,iVar3 >> 3,
                   iVar5 >> 3,uStack_34,3,local_38);
      FUN_004b80e0(local_3c,(uVar8 * 2 + 0x7c0 >> 1) * 0x400 + param_2 * 2 + 0x380,iVar3 >> 4,
                   iVar5 >> 4,uStack_34,4,local_38);
      puVar2 = local_3c;
    }
  }
  puVar2[1] = param_3;
  puVar2[2] = local_18;
  *puVar2 = local_40;
  uVar8 = DAT_005aa76c;
  *param_4 = *(undefined1 *)((int)puVar2 + 0x2a);
  puVar2[5] = uVar8;
  *(undefined1 *)(puVar2 + 10) = 0;
  return (int *)puVar2[4];
}


// ===== FUN_004b8d50 @ 004b8d50 (769 bytes) =====


void FUN_004b8d50(void)

{
  ushort uVar1;
  char *pcVar2;
  ushort *puVar3;
  ushort *puVar4;
  int iVar5;
  uint *puVar6;
  int iVar7;
  ushort *puVar8;
  int iVar9;
  int iStack_1c;
  int iStack_18;
  undefined1 local_8 [8];
  
  if (DAT_00573320 != 0) {
    (**(code **)(*DAT_01003014 + 0x4c))(DAT_01003014,0,local_8,0,0);
    iVar9 = 0;
    iVar7 = DAT_005aa6dc;
    do {
      pcVar2 = (char *)(iStack_1c * iVar9 + iStack_18);
      iVar5 = 0;
      do {
        uVar1 = *(ushort *)(iVar7 + iVar5 * 2);
        if (0x3f < uVar1) {
          uVar1 = 0x3f;
        }
        *pcVar2 = (char)uVar1 * '\x04';
        iVar5 = iVar5 + 1;
        pcVar2 = pcVar2 + 1;
      } while (iVar5 < 0x80);
      iVar9 = iVar9 + 1;
      iVar7 = iVar7 + 0x100;
    } while (iVar9 < 0x100);
    (**(code **)(*DAT_01003014 + 0x50))(DAT_01003014,0);
    DAT_00573320 = 0;
  }
  if (DAT_00573324 != 0) {
    (**(code **)(*DAT_005aeb9c + 0x4c))(DAT_005aeb9c,0,local_8,0,0);
    iVar7 = DAT_005aa8d0;
    puVar3 = (ushort *)(DAT_005aa8b8 + 0x4002);
    iVar9 = 0;
    do {
      puVar6 = (uint *)(iVar9 * iStack_1c + iStack_18);
      iVar5 = 0;
      puVar8 = puVar3 + 0x2001;
      puVar4 = puVar3;
      do {
        *puVar6 = ((*(byte *)((uint)*(byte *)((puVar3[iVar5 + -0x2001] & 0xff) + iVar7) +
                             DAT_005aa88c) | 0xffffff00) << 8 |
                  (uint)*(byte *)((uint)*(byte *)((puVar4[-1] & 0xff) + iVar7) + DAT_005aa8d4)) << 8
                  | (uint)*(byte *)((uint)*(byte *)((puVar8[-2] & 0xff) + iVar7) + DAT_005aa888);
        puVar6[1] = ((*(byte *)((uint)*(byte *)((*(ushort *)
                                                  ((int)puVar3 + (-0x4002 - (int)(puVar3 + -1)) +
                                                  (int)puVar4) & 0xff) + iVar7) + DAT_005aa88c) |
                     0xffffff00) << 8 |
                    (uint)*(byte *)((uint)*(byte *)((*puVar4 & 0xff) + iVar7) + DAT_005aa8d4)) << 8
                    | (uint)*(byte *)((uint)*(byte *)((*(ushort *)
                                                        ((int)puVar3 + (0x3ffe - (int)(puVar3 + -1))
                                                        + (int)puVar4) & 0xff) + iVar7) +
                                     DAT_005aa888);
        puVar6[2] = ((*(byte *)((uint)*(byte *)((puVar8[-0x4000] & 0xff) + iVar7) + DAT_005aa88c) |
                     0xffffff00) << 8 |
                    (uint)*(byte *)((uint)*(byte *)((puVar4[1] & 0xff) + iVar7) + DAT_005aa8d4)) <<
                    8 | (uint)*(byte *)((uint)*(byte *)((*puVar8 & 0xff) + iVar7) + DAT_005aa888);
        puVar6[3] = ((*(byte *)((uint)*(byte *)((puVar3[iVar5 + -0x1ffe] & 0xff) + iVar7) +
                               DAT_005aa88c) | 0xffffff00) << 8 |
                    (uint)*(byte *)((uint)*(byte *)((puVar4[2] & 0xff) + iVar7) + DAT_005aa8d4)) <<
                    8 | (uint)*(byte *)((uint)*(byte *)((puVar8[1] & 0xff) + iVar7) + DAT_005aa888);
        iVar5 = iVar5 + 4;
        puVar4 = puVar4 + 4;
        puVar8 = puVar8 + 4;
        puVar6 = puVar6 + 4;
      } while (iVar5 < 0x40);
      iVar9 = iVar9 + 1;
      puVar3 = puVar3 + 0x100;
    } while (iVar9 < 0x20);
    (**(code **)(*DAT_005aeb9c + 0x50))(DAT_005aeb9c,0);
    DAT_00573324 = 0;
  }
  return;
}


// ===== FUN_004b9060 @ 004b9060 (91 bytes) =====


void FUN_004b9060(void)

{
  (**(code **)(*DAT_00dcec00 + 0x5c))(DAT_00dcec00,0x80,0x100,1,0,0x32,1,&DAT_01003014,0);
  (**(code **)(*DAT_00dcec00 + 0x5c))(DAT_00dcec00,0x40,0x20,1,0,0x16,1,&DAT_005aeb9c,0);
  DAT_00573320 = 1;
  DAT_00573324 = 1;
  return;
}


// ===== FUN_004b90c0 @ 004b90c0 (678 bytes) =====


void FUN_004b90c0(void)

{
  int iVar1;
  FILE *_File;
  char *pcVar2;
  undefined4 uVar3;
  LPCSTR lpText;
  int iVar4;
  undefined4 *puVar5;
  int **lpCaption;
  UINT uType;
  char *pcVar6;
  int *piVar7;
  char *pcVar8;
  int *piVar9;
  int *piStack_2bc;
  undefined4 uStack_2b8;
  int *piStack_2b4;
  undefined4 *puStack_2b0;
  int *piStack_2ac;
  int iStack_2a8;
  undefined4 uStack_2a4;
  undefined4 uStack_2a0;
  int *piStack_29c;
  char *pcStack_298;
  undefined4 uStack_294;
  undefined1 *puStack_290;
  undefined1 *puStack_28c;
  undefined4 uStack_288;
  int *piStack_284;
  undefined1 *puStack_280;
  int iVar10;
  undefined1 local_134 [196];
  ushort uStack_70;
  
  puStack_280 = local_134;
  piStack_284 = DAT_00dcec00;
  uStack_288 = 0x4b90e7;
  (**(code **)(*DAT_00dcec00 + 0x1c))();
  pcStack_298 = "ps.2.0";
  if (DAT_010ed973 == '\0') {
    if (0x2ff < uStack_70) {
      pcStack_298 = "ps.3.0";
    }
    uStack_288 = 0;
    puStack_28c = &stack0xfffffd90;
    puStack_290 = &stack0xfffffd8c;
    uStack_294 = 0;
    piStack_29c = (int *)0x51cd68;
    uStack_2a0 = 0;
    uStack_2a4 = 0;
    iStack_2a8 = 0xea1;
    piStack_2ac = (int *)0x51bd58;
    puStack_2b0 = (undefined4 *)0x4b9139;
    D3DXCompileShader();
    iVar10 = *DAT_00dcec00;
  }
  else {
    if (0x2ff < uStack_70) {
      pcStack_298 = "ps.3.0";
    }
    uStack_288 = 0;
    puStack_28c = &stack0xfffffd90;
    puStack_290 = &stack0xfffffd8c;
    uStack_294 = 0;
    piStack_29c = (int *)0x51cd78;
    uStack_2a0 = 0;
    uStack_2a4 = 0;
    iStack_2a8 = 0xea1;
    piStack_2ac = (int *)0x51bd58;
    puStack_2b0 = (undefined4 *)0x4b9170;
    D3DXCompileShader();
    iVar10 = *DAT_00dcec00;
  }
  puStack_2b0 = &DAT_00dcec44;
  piStack_2b4 = piStack_29c;
  uStack_2b8 = 0x4b9188;
  uStack_2b8 = (**(code **)(*piStack_29c + 0xc))();
  piStack_2bc = DAT_00dcec00;
  (**(code **)(iVar10 + 0x1a8))();
  (**(code **)(*piStack_2ac + 8))(piStack_2ac);
  iVar4 = 1;
  puVar5 = &DAT_00daebb8;
  iVar10 = 0x10;
  do {
    *puVar5 = 0;
    _sprintf(&stack0xfffffd88,"SCRIPTS\\mode_%d.ps",iVar4);
    _File = _fopen(&stack0xfffffd88,"rb");
    if (_File != (FILE *)0x0) {
      _fclose(_File);
      pcVar2 = "ps.2.0";
      if (iStack_2a8 == 0) {
        pcVar2 = "ps.3.0";
      }
      piVar9 = (int *)0x0;
      piVar7 = (int *)&stack0xfffffd88;
      D3DXCompileShaderFromFileA(piVar7,0,0,"PShader",pcVar2,0,&puStack_2b0,&piStack_2ac,0);
      iVar1 = *DAT_00dcec00;
      uVar3 = (**(code **)(*(int *)pcVar2 + 0xc))(pcVar2,puVar5);
      (**(code **)(iVar1 + 0x1a8))(DAT_00dcec00,uVar3);
      if (piVar9 != (int *)0x0) {
        _sprintf((char *)&piStack_2ac,"Pixel Shader Build Error mode_%d.ps",iVar4);
        uType = 0x10;
        lpCaption = &piStack_2ac;
        lpText = (LPCSTR)(**(code **)(*piVar9 + 0xc))(piVar9);
        MessageBoxA((HWND)0x0,lpText,(LPCSTR)lpCaption,uType);
      }
      (**(code **)(*piVar7 + 8))(piVar7);
    }
    puVar5 = puVar5 + 1;
    iVar4 = iVar4 + 1;
    iVar10 = iVar10 + -1;
  } while (iVar10 != 0);
  uStack_2a4 = 0;
  uStack_2a0 = 3;
  piStack_29c = (int *)0x100000;
  pcStack_298 = (char *)0xa0004;
  uStack_294 = 0x140000;
  puStack_290 = &DAT_010a0004;
  puStack_28c = (undefined1 *)0x180000;
  uStack_288 = 0x50001;
  piStack_284 = (int *)0xff;
  puStack_280 = (undefined1 *)0x11;
  (**(code **)(*DAT_00dcec00 + 0x158))(DAT_00dcec00,&uStack_2a4,&DAT_00dcefe4);
  pcVar2 = "vs.2.0";
  if (piStack_2b4 == (int *)0x0) {
    pcVar2 = "vs.3.0";
  }
  pcVar8 = "VShader";
  pcVar6 = 
  "struct VS_IN                                                 \n{                                                            \n   float4   Pos             : POSITION;                      \n   float4   LightMap\t\t : COLOR0;\t\t\t\t\t\t  \n   float4   BaseColor\t\t : COLOR1;\t\t\t\t\t\t  \n   float2   Tex             : TEXCOORD0;                     \n};                                                           \n                                                             \nstruct VS_OUT                                                \n{                                                            \n   float4 Position        : POSITION;                        \n   float4 LightMap\t\t : COLOR0;\t \t\t\t\t\t      \n   float4 BaseColor\t\t : COLOR1;\t\t\t\t\t\t  \n   float2 TexCoord0       : TEXCOORD0;                       \n};                                                           \n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  \nstruct PS_IN                                                \n{                                                            \n   float4 LightMap\t\t : COLOR0;\t \t\t\t\t\t      \n   float4 BaseColor\t\t : COLOR1;\t\t\t\t\t\t  \n   float2 TexCoord0       : TEXCOORD0;                       \n};                                                           \n                                                             \nstruct PS_OUT\t\t\t\t\t\t\t\t\t\t\t\t  \n{\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  \n\tfloat4 Color:COLOR0;\t\t\t\t\t\t\t\t\t\t  \n};\t\t\t\t\t\t\t\t\t\t\t\t\t          \n                                                             \n                                                             \nsampler        Texture : register(s0);\t\t\t\t\t      \nsampler        Luma : register(s2);\t\t\t\t\t      \nsampler        Xlat : register(s3);\t\t\t\t\t      \n                                                             \nfloat mesh:register(c1);\nfloat2 scale:register(c2);\nfloat4 dist:register(c4);\n\nVS_OUT VShader( VS_IN In )                              \n{                                                            \n   VS_OUT Out;                                               \n   Out.Position  = In.Pos;\t\t\t\t\t\t          \n\tif(Out.Position.z<0.0) Out.Position.z=0.0;\n\tif(..." /* TRUNCATED STRING LITERAL */
  ;
  D3DXCompileShader("struct VS_IN                                                 \n{                                                            \n   float4   Pos             : POSITION;                      \n   float4   LightMap\t\t : COLOR0;\t\t\t\t\t\t  \n   float4   BaseColor\t\t : COLOR1;\t\t\t\t\t\t  \n   float2   Tex             : TEXCOORD0;                     \n};                                                           \n                                                             \nstruct VS_OUT                                                \n{                                                            \n   float4 Position        : POSITION;                        \n   float4 LightMap\t\t : COLOR0;\t \t\t\t\t\t      \n   float4 BaseColor\t\t : COLOR1;\t\t\t\t\t\t  \n   float2 TexCoord0       : TEXCOORD0;                       \n};                                                           \n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  \nstruct PS_IN                                                \n{                                                            \n   float4 LightMap\t\t : COLOR0;\t \t\t\t\t\t      \n   float4 BaseColor\t\t : COLOR1;\t\t\t\t\t\t  \n   float2 TexCoord0       : TEXCOORD0;                       \n};                                                           \n                                                             \nstruct PS_OUT\t\t\t\t\t\t\t\t\t\t\t\t  \n{\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  \n\tfloat4 Color:COLOR0;\t\t\t\t\t\t\t\t\t\t  \n};\t\t\t\t\t\t\t\t\t\t\t\t\t          \n                                                             \n                                                             \nsampler        Texture : register(s0);\t\t\t\t\t      \nsampler        Luma : register(s2);\t\t\t\t\t      \nsampler        Xlat : register(s3);\t\t\t\t\t      \n                                                             \nfloat mesh:register(c1);\nfloat2 scale:register(c2);\nfloat4 dist:register(c4);\n\nVS_OUT VShader( VS_IN In )                              \n{                                                            \n   VS_OUT Out;                                               \n   Out.Position  = In.Pos;\t\t\t\t\t\t          \n\tif(Out.Position.z<0.0) Out.Position.z=0.0;\n\tif(..." /* TRUNCATED STRING LITERAL */
                    ,0xea1,0,0,"VShader",pcVar2,0,&piStack_2bc,&uStack_2b8,0);
  iVar10 = *DAT_00dcec00;
  uVar3 = (**(code **)(*(int *)pcVar8 + 0xc))(pcVar8,&DAT_00dcec18);
  (**(code **)(iVar10 + 0x16c))(DAT_00dcec00,uVar3);
  (**(code **)(*(int *)pcVar6 + 8))(pcVar6);
  return;
}


// ===== FUN_004b9370 @ 004b9370 (6 bytes) =====


void FUN_004b9370(void)

{
  undefined4 in_EAX;
  
  DAT_010ed974 = in_EAX;
  return;
}


// ===== FUN_004b9380 @ 004b9380 (6 bytes) =====


void FUN_004b9380(void)

{
  undefined4 in_EAX;
  
  DAT_010ed978 = in_EAX;
  return;
}


// ===== FUN_004b9390 @ 004b9390 (8 bytes) =====


void FUN_004b9390(void)

{
  DAT_010ed970 = 1;
  return;
}


// ===== FUN_004b93a0 @ 004b93a0 (26 bytes) =====


void FUN_004b93a0(void)

{
  char in_CL;
  
  if (in_CL < '\0') {
    DAT_010ed8f0 = 1;
    return;
  }
  DAT_00573324 = 1;
  return;
}


// ===== FUN_004b93c0 @ 004b93c0 (3 bytes) =====


undefined1 FUN_004b93c0(void)

{
  return 1;
}


// ===== FUN_004b93d0 @ 004b93d0 (8 bytes) =====


void FUN_004b93d0(void)

{
  DAT_01003018 = DAT_01003018 + 4;
  return;
}


// ===== FUN_004b93e0 @ 004b93e0 (8 bytes) =====


void FUN_004b93e0(void)

{
  DAT_01003018 = DAT_01003018 + 0x30;
  return;
}


// ===== FUN_004b93f0 @ 004b93f0 (8 bytes) =====


void FUN_004b93f0(void)

{
  DAT_01003018 = DAT_01003018 + 0xc;
  return;
}


// ===== FUN_004b9400 @ 004b9400 (8 bytes) =====


void FUN_004b9400(void)

{
  DAT_01003018 = DAT_01003018 + 0xc;
  return;
}


// ===== FUN_004b9410 @ 004b9410 (8 bytes) =====


void FUN_004b9410(void)

{
  DAT_01003018 = DAT_01003018 + 8;
  return;
}


// ===== FUN_004b9420 @ 004b9420 (8 bytes) =====


void FUN_004b9420(void)

{
  DAT_01003018 = DAT_01003018 + 0x18;
  return;
}


// ===== FUN_004b9430 @ 004b9430 (38 bytes) =====


void FUN_004b9430(void)

{
  uint uVar1;
  
  uVar1 = *(uint *)(DAT_01003018 + 0x20);
  while ((uVar1 & 3) != 0) {
    uVar1 = *(uint *)(DAT_01003018 + 0x44);
    DAT_01003018 = DAT_01003018 + 0x24;
  }
  DAT_01003018 = DAT_01003018 + 0x24;
  return;
}


// ===== FUN_004b9460 @ 004b9460 (8 bytes) =====


void FUN_004b9460(void)

{
  DAT_01003018 = DAT_01003018 + 0x10;
  return;
}


// ===== FUN_004b9470 @ 004b9470 (8 bytes) =====


void FUN_004b9470(void)

{
  DAT_01003018 = DAT_01003018 + 4;
  return;
}


// ===== FUN_004b9480 @ 004b9480 (8 bytes) =====


void FUN_004b9480(void)

{
  DAT_01003018 = DAT_01003018 + 4;
  return;
}


// ===== FUN_004b9490 @ 004b9490 (8 bytes) =====


void FUN_004b9490(void)

{
  DAT_01003018 = DAT_01003018 + 4;
  return;
}


// ===== FUN_004b94a0 @ 004b94a0 (22 bytes) =====


void FUN_004b94a0(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar2 = &DAT_010f00a8;
  puVar3 = &DAT_010f0068;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  return;
}


// ===== FUN_004b94c0 @ 004b94c0 (19 bytes) =====


void FUN_004b94c0(void)

{
  DAT_01003018 = DAT_01003018 + 8 + *(int *)(DAT_01003018 + 4) * 4;
  return;
}


// ===== FUN_004b94e0 @ 004b94e0 (83 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b94e0(void)

{
  _DAT_005aeb98 = *DAT_01003018;
  if (((DAT_010ed960 != 0) || (DAT_010ed964 != 0)) && (DAT_010ed970 != '\0')) {
    (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,8,4.0 / _DAT_005aeb98);
  }
  DAT_01003018 = DAT_01003018 + 1;
  return;
}


// ===== FUN_004b9540 @ 004b9540 (8 bytes) =====


void FUN_004b9540(void)

{
  DAT_01003018 = DAT_01003018 + 4;
  return;
}


// ===== FUN_004b9550 @ 004b9550 (8 bytes) =====


void FUN_004b9550(void)

{
  DAT_010ed972 = 1;
  return;
}


// ===== FUN_004b9560 @ 004b9560 (60 bytes) =====


void FUN_004b9560(void)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = 1;
  do {
    piVar3 = DAT_01003018;
    DAT_01003018 = piVar3 + 1;
    if (uVar2 == 0x80000000) break;
    uVar2 = uVar2 * 2;
  } while (uVar2 < 0x80000001);
  iVar1 = *DAT_01003018;
  DAT_01003018 = piVar3 + 2;
  if (iVar1 != 0) {
    DAT_01003018 = DAT_01003018 + iVar1 * 3;
  }
  return;
}


// ===== FUN_004b95a0 @ 004b95a0 (8 bytes) =====


void FUN_004b95a0(void)

{
  DAT_01003018 = DAT_01003018 + 8;
  return;
}


// ===== FUN_004b95b0 @ 004b95b0 (40 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b95b0(void)

{
  undefined4 *puVar1;
  
  puVar1 = DAT_01003018;
  _DAT_010f00d8 = *DAT_01003018;
  DAT_01003018 = DAT_01003018 + 3;
  _DAT_010f00dc = puVar1[1];
  _DAT_010f00e0 = puVar1[2];
  return;
}


// ===== FUN_004b95e0 @ 004b95e0 (125 bytes) =====


/* WARNING: Removing unreachable block (ram,0x004b960f) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b95e0(void)

{
  uint *puVar1;
  float10 fVar2;
  
  puVar1 = DAT_01003018;
  DAT_010ed988 = *DAT_01003018 >> 0x17 & 0xff;
  fVar2 = (float10)FUN_005006a0();
  _DAT_00dcec28 = (float)fVar2;
  fVar2 = (float10)FUN_005006a0();
  _DAT_005aeba0 = (float)fVar2;
  DAT_01003018 = puVar1 + 1;
  return;
}


// ===== FUN_004b9660 @ 004b9660 (22 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b9660(void)

{
  _DAT_010ed998 = *DAT_01003018;
  DAT_01003018 = DAT_01003018 + 1;
  return;
}


// ===== FUN_004b9680 @ 004b9680 (265 bytes) =====


void FUN_004b9680(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  undefined4 *puVar4;
  uint *puVar5;
  uint uVar6;
  
  uVar1 = DAT_01003018[1];
  if (uVar1 == 0) {
    DAT_01003018 = DAT_01003018 + 2;
    return;
  }
  puVar5 = DAT_01003018 + 3;
  puVar4 = (undefined4 *)(&DAT_00dcec6c + (*DAT_01003018 >> 2) * 0x1c);
  uVar6 = uVar1;
  do {
    uVar2 = puVar5[-1];
    uVar3 = *puVar5;
    puVar4[-3] = (float)(uVar2 & 0xff);
    puVar4[-2] = (float)(uVar2 >> 8 & 0xff);
    *(char *)(puVar4 + 2) = (char)(uVar2 >> 0x18);
    puVar4[-1] = (float)(uVar2 >> 0x10 & 0xff);
    switch(uVar2 >> 0x18) {
    case 0:
      *puVar4 = 0;
      puVar4[1] = 0;
      break;
    case 1:
      *puVar4 = 1;
      puVar4[1] = 0;
      break;
    case 2:
    case 3:
      *puVar4 = 2;
      puVar4[1] = 1;
      break;
    case 4:
    case 5:
    case 6:
      *puVar4 = 4;
      puVar4[1] = 2;
      break;
    default:
      *puVar4 = 8;
      puVar4[1] = 3;
    }
    puVar4[3] = uVar3;
    puVar5 = puVar5 + 2;
    uVar6 = uVar6 - 1;
    puVar4 = puVar4 + 7;
  } while (uVar6 != 0);
  DAT_01003018 = DAT_01003018 + uVar1 * 2 + 2;
  return;
}


// ===== FUN_004b97b0 @ 004b97b0 (87 bytes) =====


void FUN_004b97b0(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  undefined *puVar4;
  uint *puVar5;
  
  uVar2 = *DAT_01003018;
  uVar1 = DAT_01003018[1];
  puVar4 = &DAT_00daebf8;
  if (0xffffff < uVar2) {
    puVar4 = &DAT_010230f8;
    uVar2 = uVar2 & 0xffffff;
  }
  uVar3 = 0;
  if (uVar1 != 0) {
    puVar5 = DAT_01003018 + 2;
    do {
      *(uint *)(puVar4 + uVar3 * 4 + uVar2 * 4) = *puVar5;
      uVar3 = uVar3 + 1;
      puVar5 = puVar5 + 1;
    } while (uVar3 < uVar1);
    DAT_01003018 = DAT_01003018 + uVar1 + 2;
    return;
  }
  DAT_01003018 = DAT_01003018 + 2;
  return;
}


// ===== FUN_004b9810 @ 004b9810 (101 bytes) =====


void FUN_004b9810(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  
  uVar1 = *DAT_01003018;
  uVar2 = DAT_01003018[1];
  if ((uVar1 & 0x800000) == 0) {
    DAT_01003018 = DAT_01003018 + uVar2 + 2;
    return;
  }
  uVar3 = 0;
  if (uVar2 != 0) {
    puVar4 = DAT_01003018 + 2;
    do {
      *(short *)(&DAT_01003070 + uVar3 * 2 + (uVar1 & 0xffff) * 2) = (short)*puVar4;
      uVar3 = uVar3 + 1;
      puVar4 = puVar4 + 1;
    } while (uVar3 < uVar2);
    DAT_01003018 = DAT_01003018 + uVar2 + 2;
    return;
  }
  DAT_01003018 = DAT_01003018 + 2;
  return;
}


// ===== FUN_004b9880 @ 004b9880 (115 bytes) =====


void FUN_004b9880(void)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = *DAT_01003018;
  uVar2 = DAT_01003018[1];
  DAT_010430fc = 1;
  if (uVar1 < 0x800000) {
    DAT_00dceff4 = (undefined *)(DAT_005aa884 + uVar1 * 2);
  }
  else {
    DAT_00dceff4 = &DAT_01003070 + (uVar1 & 0xffff) * 2;
  }
  if (uVar2 < 0x800000) {
    DAT_00daebb4 = (undefined *)(DAT_005aa884 + uVar2 * 2);
  }
  else {
    DAT_00daebb4 = &DAT_01003070 + (uVar2 & 0xffff) * 2;
  }
  DAT_01003018 = (uint *)(*DAT_00dceff8)();
  return;
}


// ===== FUN_004b9900 @ 004b9900 (1 bytes) =====


void FUN_004b9900(void)

{
  return;
}


// ===== FUN_004b9910 @ 004b9910 (120 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b9910(void)

{
  switch(DAT_00dcec40 & 3) {
  case 0:
    _DAT_01003000 = _DAT_00dcec4c;
    _DAT_00dcefe8 = _DAT_005aeb4c;
    return;
  case 1:
    _DAT_01003000 = _DAT_010230b4;
    _DAT_00dcefe8 = _DAT_00dcec1c;
    return;
  case 2:
    _DAT_01003000 = _DAT_0100300c;
    _DAT_00dcefe8 = _DAT_00dcebf8;
    return;
  case 3:
    _DAT_01003000 = _DAT_005aeb94;
    _DAT_00dcefe8 = _DAT_00dcec50;
  }
  return;
}


// ===== FUN_004b99a0 @ 004b99a0 (139 bytes) =====


float10 FUN_004b99a0(int param_1,int param_2,float param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  float10 fVar4;
  
  fVar4 = ((float10)param_3 + (float10)param_3) * (float10)*(float *)(param_1 + 8) -
          (float10)*(float *)(param_2 + 8);
  if ((fVar4 < (float10)0) || (((&DAT_00dcec74)[DAT_00dcec24 * 0x1c] & 7) == 0)) {
    fVar4 = (float10)0;
  }
  uVar2 = (uint)*(byte *)((int)&PTR_DAT_00511840 + ((byte)(&DAT_00dcec74)[DAT_00dcec24 * 0x1c] & 7))
  ;
  iVar3 = 0;
  if (7 < uVar2) {
    iVar1 = (uVar2 - 8 >> 3) + 1;
    iVar3 = iVar1 * 8;
    do {
      iVar1 = iVar1 + -1;
      fVar4 = fVar4 * fVar4 * fVar4 * fVar4;
      fVar4 = fVar4 * fVar4;
      fVar4 = fVar4 * fVar4;
      fVar4 = fVar4 * fVar4;
      fVar4 = fVar4 * fVar4;
      fVar4 = fVar4 * fVar4;
      fVar4 = fVar4 * fVar4;
    } while (iVar1 != 0);
  }
  if (iVar3 < (int)uVar2) {
    iVar3 = uVar2 - iVar3;
    do {
      iVar3 = iVar3 + -1;
      fVar4 = fVar4 * fVar4;
    } while (iVar3 != 0);
  }
  return fVar4 * (float10)*(float *)(&DAT_00dcec68 + DAT_00dcec24 * 0x1c);
}


// ===== FUN_004b9a30 @ 004b9a30 (79 bytes) =====


void FUN_004b9a30(int param_1,int param_2)

{
  int in_ECX;
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  DAT_00dcec20 = DAT_00dcec20 + 1;
  puVar2 = (undefined4 *)(in_ECX + 0x18);
  puVar3 = (undefined4 *)((int)DAT_01003010 + -0x60);
  for (iVar1 = 8; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  puVar2 = (undefined4 *)(param_1 + 0x18);
  puVar3 = (undefined4 *)((int)DAT_01003010 + -0x40);
  for (iVar1 = 8; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  puVar2 = (undefined4 *)(param_2 + 0x18);
  puVar3 = (undefined4 *)((int)DAT_01003010 + -0x20);
  for (iVar1 = 8; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  DAT_01003064 = DAT_01003064 + 3;
  DAT_01003010 = (undefined4 *)((int)DAT_01003010 + -0x60);
  return;
}


// ===== FUN_004b9a80 @ 004b9a80 (69 bytes) =====


void FUN_004b9a80(float param_1,float param_2,float param_3,float param_4)

{
  if (param_1 < param_2) {
    param_1 = param_2;
  }
  if (param_1 < param_3) {
    param_1 = param_3;
  }
  if (param_4 <= param_1) {
    return;
  }
  return;
}


// ===== FUN_004b9ad0 @ 004b9ad0 (69 bytes) =====


void FUN_004b9ad0(float param_1,float param_2,float param_3,float param_4)

{
  if (param_2 < param_1) {
    param_1 = param_2;
  }
  if (param_3 < param_1) {
    param_1 = param_3;
  }
  if (param_1 <= param_4) {
    return;
  }
  return;
}


// ===== FUN_004b9b20 @ 004b9b20 (290 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b9b20(void)

{
  float fVar1;
  float *in_EAX;
  
  in_EAX[0xc] = 0.0;
  in_EAX[10] = 0.0;
  in_EAX[0xd] = 0.0;
  in_EAX[0xb] = 0.0;
  in_EAX[8] = in_EAX[2];
  fVar1 = _DAT_010ed97c * 0.5;
  if (DAT_010430fc == 0) {
    in_EAX[6] = ((((_DAT_01003000 - 8.0) * _DAT_00daebb0 - fVar1) - _DAT_00dcec5c) * in_EAX[2] +
                *in_EAX * _DAT_00dcec14 * _DAT_00daebb0) / fVar1;
    in_EAX[7] = -(((_DAT_00dceffc * 90.0 +
                   (((384.0 - _DAT_00dcefe8) * _DAT_00dceffc - _DAT_010ed980 * 0.5) - _DAT_01003068)
                   ) * in_EAX[2] - _DAT_00dcebfc * _DAT_00dceffc * in_EAX[1]) /
                 (_DAT_010ed980 * 0.5));
    in_EAX[8] = in_EAX[2];
    in_EAX[9] = in_EAX[2];
    return;
  }
  in_EAX[6] = ((((_DAT_01003000 - 8.0) - _DAT_00dcec5c) * _DAT_00daebb0 - fVar1) * in_EAX[2] +
              *in_EAX * _DAT_00daebb0) / fVar1;
  in_EAX[7] = -(((_DAT_00dceffc * 90.0 +
                 (((384.0 - _DAT_00dcefe8) - _DAT_01003068) * _DAT_00dceffc - _DAT_010ed980 * 0.5))
                 * in_EAX[2] - in_EAX[1] * _DAT_00dceffc) / (_DAT_010ed980 * 0.5));
  in_EAX[8] = in_EAX[2];
  in_EAX[9] = in_EAX[2];
  return;
}


// ===== FUN_004b9c50 @ 004b9c50 (62 bytes) =====


undefined4 * FUN_004b9c50(void)

{
  float *in_EAX;
  float *in_ECX;
  
  DAT_010f01d4 = in_EAX[2] * in_ECX[1] - in_ECX[2] * in_EAX[1];
  DAT_010f01d8 = in_ECX[2] * *in_EAX - in_EAX[2] * *in_ECX;
  DAT_010f01dc = *in_ECX * in_EAX[1] - *in_EAX * in_ECX[1];
  return &DAT_010f01d4;
}


// ===== FUN_004b9c90 @ 004b9c90 (21 bytes) =====


float10 FUN_004b9c90(void)

{
  float *in_EAX;
  float *in_ECX;
  
  return (float10)in_ECX[2] * (float10)in_EAX[2] +
         (float10)*in_ECX * (float10)*in_EAX + (float10)in_ECX[1] * (float10)in_EAX[1];
}


// ===== FUN_004b9cb0 @ 004b9cb0 (308 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b9cb0(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float *in_EAX;
  
  fVar1 = *in_EAX;
  fVar2 = in_EAX[1];
  fVar3 = in_EAX[2];
  if ((DAT_010ed95a != '\0') && (SUB41(in_EAX[3],0) != '\0')) {
    *in_EAX = _DAT_010f0078 * fVar2 + DAT_010f0068 * fVar1 + _DAT_010f0088 * fVar3 + _DAT_010f0098;
    in_EAX[1] = _DAT_010f008c * fVar3 + DAT_010f006c * fVar1 + _DAT_010f007c * fVar2 + _DAT_010f009c
    ;
    in_EAX[2] = fVar3 * _DAT_010f0090 + _DAT_010f0080 * fVar2 + _DAT_010f0070 * fVar1 +
                _DAT_010f00a0;
    return;
  }
  *in_EAX = DAT_010f00b8 * fVar2 + DAT_010f00a8 * fVar1 + DAT_010f00c8 * fVar3 + _DAT_010f00d8;
  in_EAX[1] = DAT_010f00cc * fVar3 + DAT_010f00ac * fVar1 + DAT_010f00bc * fVar2 + _DAT_010f00dc;
  in_EAX[2] = fVar3 * DAT_010f00d0 + DAT_010f00c0 * fVar2 + DAT_010f00b0 * fVar1 + _DAT_010f00e0;
  return;
}


// ===== FUN_004b9df0 @ 004b9df0 (113 bytes) =====


void FUN_004b9df0(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  undefined *puVar4;
  float *in_ECX;
  
  puVar4 = PTR_DAT_00573330;
  fVar1 = *in_ECX;
  fVar2 = in_ECX[1];
  fVar3 = in_ECX[2];
  *in_ECX = fVar1 * *(float *)PTR_DAT_00573330 + *(float *)(PTR_DAT_00573330 + 0x10) * fVar2 +
            *(float *)(PTR_DAT_00573330 + 0x20) * fVar3;
  in_ECX[1] = *(float *)(puVar4 + 0x24) * fVar3 +
              *(float *)(puVar4 + 4) * fVar1 + *(float *)(puVar4 + 0x14) * fVar2;
  in_ECX[2] = fVar1 * *(float *)(puVar4 + 8) + *(float *)(puVar4 + 0x18) * fVar2 +
              *(float *)(puVar4 + 0x28) * fVar3;
  return;
}


// ===== FUN_004b9e70 @ 004b9e70 (11 bytes) =====


void FUN_004b9e70(void)

{
  DAT_01003064 = 0;
  return;
}


// ===== FUN_004b9e80 @ 004b9e80 (186 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b9e80(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  char in_DL;
  
  if (*(int *)(DAT_00dcefe0 + 0x70) == 0) {
    _DAT_010ed984 = 0.0;
    return;
  }
  fVar1 = *(float *)(DAT_00dcefe0 + 0x60) * 1.3333334;
  if (*(float *)(DAT_00dcefe0 + 100) < fVar1) {
    fVar1 = *(float *)(DAT_00dcefe0 + 100);
  }
  fVar2 = fVar1 * 0.002016129 * 496.0;
  _DAT_010ed984 = (*(float *)(DAT_00dcefe0 + 100) - fVar2) * 0.5;
  fVar1 = *(float *)(DAT_00dcefe0 + 0x60) * 0.0026041667 * 384.0;
  fVar3 = (*(float *)(DAT_00dcefe0 + 0x60) - fVar1) * 0.5;
  _DAT_00daebb0 = (((fVar2 + _DAT_010ed984) - _DAT_010ed984) + 1.0) * 0.002016129;
  _DAT_00dceffc = (((fVar1 + fVar3) - fVar3) + 1.0) * 0.0026041667;
  if (in_DL != '\0') {
    DAT_010ed958 = in_DL;
    _DAT_010ed984 = 0.0;
    _DAT_00daebb0 = _DAT_00daebb0 / _DAT_0057332c;
    return;
  }
  DAT_010ed958 = in_DL;
  return;
}


// ===== FUN_004b9f40 @ 004b9f40 (36 bytes) =====


void FUN_004b9f40(void)

{
  PTR_FUN_005733a8 = FUN_004b9480;
  PTR_FUN_005733b0 = FUN_004b9470;
  PTR_FUN_00573450 = FUN_004b93d0;
  PTR_FUN_00573458 = FUN_004b93d0;
  return;
}


// ===== FUN_004b9f70 @ 004b9f70 (147 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004b9f70(void)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  
  iVar1 = DAT_005aa700;
  puVar3 = (uint *)((DAT_005aa870 & 0x7ffff) + DAT_005aa700);
  DAT_00dcec54 = 0;
  _DAT_00dcefec = 10;
  DAT_010ed972 = '\0';
  DAT_01003018 = puVar3;
  do {
    if (0x40000 < (int)((int)DAT_01003018 - (int)puVar3 & 0xfffffffcU)) {
      return;
    }
    uVar2 = *DAT_01003018;
    if ((int)uVar2 < 0) {
      DAT_01003018 = (uint *)(iVar1 + (uVar2 & 0x7ffff));
    }
    else {
      uVar2 = uVar2 >> 0x17 & 0x3f;
      DAT_01003018 = DAT_01003018 + 1;
      DAT_00dcec08 = 0;
      if (uVar2 < 0x21) {
        (*(code *)(&PTR_FUN_005733e8)[uVar2])();
      }
    }
  } while (DAT_010ed972 == '\0');
  return;
}


// ===== FUN_004ba010 @ 004ba010 (149 bytes) =====


void FUN_004ba010(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_48[0] = *DAT_01003018;
  local_48[1] = DAT_01003018[1];
  local_48[2] = DAT_01003018[2];
  local_48[3] = 0;
  local_48[4] = DAT_01003018[3];
  local_34 = DAT_01003018[4];
  local_30 = DAT_01003018[5];
  local_2c = 0;
  local_28 = DAT_01003018[6];
  local_24 = DAT_01003018[7];
  local_20 = DAT_01003018[8];
  local_1c = 0;
  local_18 = DAT_01003018[9];
  local_14 = DAT_01003018[10];
  local_10 = DAT_01003018[0xb];
  local_c = 0x3f800000;
  puVar2 = local_48;
  puVar3 = &DAT_010f0068;
  DAT_01003018 = DAT_01003018 + 0xc;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  return;
}


// ===== FUN_004ba0b0 @ 004ba0b0 (149 bytes) =====


void FUN_004ba0b0(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_48[0] = *DAT_01003018;
  local_48[1] = DAT_01003018[1];
  local_48[2] = DAT_01003018[2];
  local_48[3] = 0;
  local_48[4] = DAT_01003018[3];
  local_34 = DAT_01003018[4];
  local_30 = DAT_01003018[5];
  local_2c = 0;
  local_28 = DAT_01003018[6];
  local_24 = DAT_01003018[7];
  local_20 = DAT_01003018[8];
  local_1c = 0;
  local_18 = DAT_01003018[9];
  local_14 = DAT_01003018[10];
  local_10 = DAT_01003018[0xb];
  local_c = 0x3f800000;
  puVar2 = local_48;
  puVar3 = &DAT_010f00a8;
  DAT_01003018 = DAT_01003018 + 0xc;
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  return;
}


// ===== FUN_004ba150 @ 004ba150 (68 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004ba150(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar2 = DAT_01003018;
  _DAT_010f005c = *DAT_01003018;
  puVar1 = DAT_01003018 + 1;
  DAT_01003018 = DAT_01003018 + 3;
  _DAT_010f0060 = *puVar1;
  _DAT_010f0064 = puVar2[2];
  return;
}


// ===== FUN_004ba1a0 @ 004ba1a0 (46 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004ba1a0(void)

{
  float fVar1;
  float *pfVar2;
  
  pfVar2 = DAT_01003018;
  fVar1 = *DAT_01003018;
  DAT_01003018 = DAT_01003018 + 2;
  _DAT_00dcec14 = _DAT_0057332c * fVar1;
  _DAT_00dcebfc = pfVar2[1];
  return;
}


// ===== FUN_004ba1d0 @ 004ba1d0 (893 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004ba1d0(void)

{
  undefined4 uVar1;
  undefined4 local_20;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  local_14 = 0;
  local_20 = (undefined4)(longlong)ROUND(_DAT_010ed984);
  local_18 = local_20;
  local_20 = (undefined4)(longlong)ROUND(_DAT_00daebb0 * 496.0);
  local_10 = local_20;
  local_20 = (undefined4)(longlong)ROUND(_DAT_00dceffc * 384.0);
  local_c = local_20;
  local_8 = 0;
  local_4 = 0x3f800000;
  (**(code **)(*DAT_00dcec00 + 0xbc))(DAT_00dcec00,&local_18);
  _DAT_00dcec2c = 0;
  if (DAT_010ed959 == '\0') {
    uVar1 = 3;
  }
  else {
    uVar1 = 2;
  }
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,8,uVar1);
  (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,5,1);
  (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,6,1);
  if (DAT_010ed994 != 0) {
    (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,5,2);
    (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,6,2);
  }
  if ((DAT_010ed960 != 0) || (DAT_010ed964 != 0)) {
    (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,7,3);
  }
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,7,1);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0xe,1);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0x17,7);
  DAT_005aa762 = 1;
  (**(code **)(*DAT_00dcec00 + 0x15c))(DAT_00dcec00,DAT_00dcefe4);
  (**(code **)(*DAT_00dcec00 + 0x170))(DAT_00dcec00,DAT_00dcec18);
  (**(code **)(*DAT_00dcec00 + 0x1ac))(DAT_00dcec00,DAT_00dcec44);
  DAT_010ed971 = 0;
  FUN_004b8d50();
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,2,DAT_01003014);
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,3,DAT_005aeb9c);
  (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,2,5,1);
  (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,2,6,1);
  (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,3,5,1);
  (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,3,6,1);
  if (DAT_010ed95c != 0) {
    (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0x16,1);
  }
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,9,2);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0x89,0);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0xf,1);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0x19,5);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0x18,0x30);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0x13,6);
  (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0x14,5);
  _DAT_00dcec30 = 0xff00;
  _DAT_00daeba8 = 0;
  DAT_00dcec0c = 0;
  DAT_00dcec20 = 0;
  DAT_00dcec38 = 0;
  DAT_01003010 = &DAT_00daeb88;
  DAT_01003064 = 0;
  DAT_00573338 = 0xffffffff;
  _memset(&DAT_00dcf000,0,0x4000);
  return;
}


// ===== FUN_004ba550 @ 004ba550 (147 bytes) =====


void FUN_004ba550(void)

{
  ushort uVar1;
  float fVar2;
  float fVar3;
  int in_EAX;
  int iVar4;
  
  iVar4 = 0x20 << ((byte)*DAT_00daebb4 & 7);
  fVar2 = (float)iVar4;
  if (iVar4 < 0) {
    fVar2 = fVar2 + 4.2949673e+09;
  }
  uVar1 = *DAT_00dceff4;
  fVar2 = ((float)DAT_00dceff4[1] * 0.125) / fVar2;
  iVar4 = 0x20 << ((byte)(*DAT_00daebb4 >> 3) & 7);
  *(float *)(in_EAX + 0x30) = fVar2;
  fVar3 = (float)iVar4;
  if (iVar4 < 0) {
    fVar3 = fVar3 + 4.2949673e+09;
  }
  fVar3 = ((float)uVar1 * 0.125) / fVar3;
  DAT_00dceff4 = DAT_00dceff4 + 2;
  *(float *)(in_EAX + 0x34) = fVar3;
  *(float *)(in_EAX + 0x14) = fVar3;
  *(float *)(in_EAX + 0x10) = fVar2;
  return;
}


// ===== FUN_004ba5f0 @ 004ba5f0 (435 bytes) =====


void FUN_004ba5f0(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  int *piVar6;
  undefined4 local_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  
  puVar2 = DAT_010f00e8;
  puVar1 = (undefined4 *)*DAT_010f00e8;
  (**(code **)(*DAT_00dcec00 + 0x100))(DAT_00dcec00,0,&local_34);
  iVar3 = *DAT_00dcec00;
  if (DAT_010ed973 == '\0') {
    (**(code **)(iVar3 + 0xe4))(DAT_00dcec00,0x1b,1);
    local_34 = 0x3f800000;
    puVar4 = (undefined4 *)&stack0xffffffc4;
    uStack_30 = 0x3f800000;
    iVar3 = *DAT_00dcec00;
  }
  else {
    puVar4 = &uStack_2c;
    uStack_2c = 0x3f800000;
    uStack_28 = 0x3f800000;
    uStack_24 = 0x3f800000;
    uStack_20 = 0x3f800000;
  }
  (**(code **)(iVar3 + 0x1b4))(DAT_00dcec00,1,puVar4,1);
  for (; puVar1 != puVar2; puVar1 = (undefined4 *)*puVar1) {
    (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0,puVar1[4]);
    DAT_00573338 = 1;
    if ((puVar1[6] & 0x100) == 0) {
      uVar5 = 1;
    }
    else {
      uVar5 = 2;
    }
    (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,1,uVar5);
    if ((puVar1[6] & 0x200) == 0) {
      uVar5 = 1;
    }
    else {
      uVar5 = 2;
    }
    (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,2,uVar5);
    (**(code **)(*DAT_00dcec00 + 0x14c))(DAT_00dcec00,4,(int)puVar1[3] / 3,puVar1[2],0x20);
    _free((void *)puVar1[2]);
  }
  iVar3 = *DAT_00dcec00;
  if (DAT_010ed973 == '\0') {
    (**(code **)(iVar3 + 0xe4))(DAT_00dcec00,0x1b,0);
    iVar3 = *DAT_00dcec00;
  }
  uStack_20 = 0;
  uStack_24 = 0;
  uStack_28 = 0;
  uStack_2c = 0;
  piVar6 = DAT_00dcec00;
  (**(code **)(iVar3 + 0x1b4))(DAT_00dcec00,1,&uStack_2c,1);
  FUN_00453110();
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0,piVar6);
  return;
}


// ===== FUN_004ba7b0 @ 004ba7b0 (186 bytes) =====


void FUN_004ba7b0(void)

{
  int *piVar1;
  int *piVar2;
  
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0,0);
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,2,0);
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,3,0);
  FUN_004b7c60();
  (**(code **)(*DAT_00dcec00 + 0x170))(DAT_00dcec00,0);
  (**(code **)(*DAT_00dcec18 + 8))(DAT_00dcec18);
  (**(code **)(*DAT_00dcec00 + 0x1ac))(DAT_00dcec00,0);
  (**(code **)(*DAT_00dcec44 + 8))(DAT_00dcec44);
  piVar2 = &DAT_00daebb8;
  do {
    piVar1 = (int *)*piVar2;
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 8))(piVar1);
    }
    piVar2 = piVar2 + 1;
  } while ((int)piVar2 < 0xdaebf8);
  (**(code **)(*DAT_01003014 + 8))(DAT_01003014);
  (**(code **)(*DAT_005aeb9c + 8))(DAT_005aeb9c);
  return;
}


// ===== FUN_004ba870 @ 004ba870 (172 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004ba870(void)

{
  uint uVar1;
  uint *puVar2;
  char local_80 [128];
  
  FUN_004ba5f0();
  _sprintf(local_80,"Objects: %d  Polys: %d",DAT_00dcec38,DAT_00dcec20);
  _DAT_010230b0 = DAT_010ed968;
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0,0);
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,2,0);
  (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,3,0);
  uVar1 = 0;
  puVar2 = &DAT_00dcf004;
  do {
    if (uVar1 < puVar2[-1]) {
      uVar1 = puVar2[-1];
    }
    if (uVar1 < *puVar2) {
      uVar1 = *puVar2;
    }
    if (uVar1 < puVar2[1]) {
      uVar1 = puVar2[1];
    }
    if (uVar1 < puVar2[2]) {
      uVar1 = puVar2[2];
    }
    puVar2 = puVar2 + 4;
  } while ((int)puVar2 < 0xdd3004);
  return;
}


// ===== FUN_004ba920 @ 004ba920 (68 bytes) =====


void FUN_004ba920(void)

{
  FUN_004ba5f0();
  (**(code **)(*DAT_00dcec00 + 0xac))(DAT_00dcec00,0,0,2,&DAT_00ff00ff,0,0);
  DAT_005aa762 = 0;
  _memset(&DAT_00dcf000,0,0x4000);
  return;
}


// ===== FUN_004ba970 @ 004ba970 (75 bytes) =====


void FUN_004ba970(void)

{
  float fVar1;
  float *in_ECX;
  
  fVar1 = SQRT(in_ECX[2] * in_ECX[2] + *in_ECX * *in_ECX + in_ECX[1] * in_ECX[1]);
  if (fVar1 != 0.0) {
    *in_ECX = *in_ECX / fVar1;
    in_ECX[1] = in_ECX[1] / fVar1;
    in_ECX[2] = in_ECX[2] / fVar1;
    return;
  }
  return;
}


// ===== FUN_004ba9c0 @ 004ba9c0 (146 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004ba9c0(void)

{
  int iVar1;
  void *_Dst;
  int iVar2;
  size_t _Size;
  undefined1 local_c [12];
  
  _Size = DAT_01003064 << 5;
  _Dst = _malloc(_Size);
  FID_conflict__memcpy(_Dst,DAT_01003010,_Size);
  (**(code **)(*DAT_00dcec00 + 0x100))(DAT_00dcec00,0,local_c);
  iVar1 = DAT_010f00e8;
  iVar2 = FUN_00453150(DAT_010f00e8,*(undefined4 *)(DAT_010f00e8 + 4));
  if (_DAT_010f00ec == 0xccccccb) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  _DAT_010f00ec = _DAT_010f00ec + 1;
  *(int *)(iVar1 + 4) = iVar2;
  **(int **)(iVar2 + 4) = iVar2;
  return;
}


// ===== FUN_004baa60 @ 004baa60 (245 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004baa60(void)

{
  int iVar1;
  uint *puVar2;
  uint uVar3;
  
  iVar1 = DAT_005aa700;
  puVar2 = (uint *)((DAT_005aa870 & 0x7ffff) + DAT_005aa700);
  DAT_00dcec54 = 0;
  _DAT_00dcefec = 10;
  DAT_010ed972 = '\0';
  DAT_01003018 = puVar2;
  FUN_004b7cf0();
  _DAT_0057332c = _DAT_00573244;
  if (DAT_010ed958 == '\0') {
    _DAT_0057332c = 0x3f800000;
  }
  FUN_004ba1d0();
  while ((DAT_010ed972 == '\0' && ((int)((int)DAT_01003018 - (int)puVar2 & 0xfffffffcU) < 0x40001)))
  {
    uVar3 = *DAT_01003018;
    if ((int)uVar3 < 0) {
      DAT_01003018 = (uint *)(iVar1 + (uVar3 & 0x7ffff));
    }
    else {
      uVar3 = uVar3 >> 0x17 & 0x3f;
      DAT_01003018 = DAT_01003018 + 1;
      DAT_00dcec08 = 0;
      if (uVar3 < 0x21) {
        (*(code *)(&PTR_FUN_00573340)[uVar3])();
      }
    }
  }
  FUN_004ba870();
  uVar3 = DAT_0100306c - DAT_005aa76c >> 0x1f;
  if (0xfa < (int)((DAT_0100306c - DAT_005aa76c ^ uVar3) - uVar3)) {
    FUN_004b7ed0();
    DAT_0100306c = DAT_005aa76c;
  }
  DAT_010ed9a0 = 0xffffffff;
  return;
}


// ===== FUN_004bab60 @ 004bab60 (89 bytes) =====


void FUN_004bab60(void)

{
  if (DAT_01003064 != 0) {
    if ((DAT_010ed98c & 0x8000) != 0) {
      FUN_004ba9c0(DAT_010ed98c);
      DAT_01003064 = 0;
      return;
    }
    (**(code **)(*DAT_00dcec00 + 0x14c))(DAT_00dcec00,4,DAT_01003064 / 3,DAT_01003010,0x20);
    DAT_01003064 = 0;
  }
  return;
}


// ===== FUN_004babc0 @ 004babc0 (92 bytes) =====


void FUN_004babc0(void)

{
  if (DAT_01003064 != 0) {
    if ((*DAT_00daebb4 & 0x8000) != 0) {
      FUN_004ba9c0(*DAT_00daebb4);
      DAT_01003064 = 0;
      return;
    }
    (**(code **)(*DAT_00dcec00 + 0x14c))(DAT_00dcec00,4,DAT_01003064 / 3,DAT_01003010,0x20);
    DAT_01003064 = 0;
  }
  return;
}


// ===== FUN_004bac20 @ 004bac20 (1022 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004bac20(void)

{
  short *psVar1;
  short *psVar2;
  short *psVar3;
  short *psVar4;
  longlong lVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  int iStack_34;
  int local_30;
  int local_2c;
  undefined4 local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  
  iVar6 = FUN_005008f0();
  iVar7 = FUN_005008f0();
  local_48 = FUN_005008f0();
  iVar8 = FUN_005008f0();
  _DAT_00dcec04 = (float)local_48;
  uVar9 = iVar8 - local_48;
  _DAT_01003008 = (float)iVar8;
  _DAT_010430f8 = (float)iVar6;
  _DAT_005aeba4 = (float)iVar7;
  if (local_48 < 0) {
    local_48 = 0;
  }
  if (0x1f0 < (int)(uVar9 + local_48)) {
    uVar9 = 0x1f0 - local_48;
  }
  uVar10 = iVar7 - iVar6 >> 0x1f;
  local_3c = 0x180 - iVar7;
  uVar10 = (iVar7 - iVar6 ^ uVar10) - uVar10;
  if (local_3c < 0) {
    local_3c = 0;
  }
  if (0x180 < (int)(uVar10 + local_3c)) {
    uVar10 = 0x180 - local_3c;
  }
  local_44 = (uVar10 ^ (int)uVar10 >> 0x1f) - ((int)uVar10 >> 0x1f);
  local_40 = (uVar9 ^ (int)uVar9 >> 0x1f) - ((int)uVar9 >> 0x1f);
  if (0x180 < local_3c) {
    local_3c = 0x17f;
  }
  if (0x1f0 < local_40 + local_48) {
    local_40 = 0x1f0 - local_40;
  }
  if (0x180 < local_44 + local_3c) {
    local_44 = 0x180 - local_3c;
  }
  FUN_004babc0();
  FUN_004ba5f0();
  local_38 = (int)(longlong)ROUND((float)local_48 * _DAT_00daebb0 + _DAT_010ed984);
  local_30 = local_38;
  local_38 = (int)(longlong)ROUND((float)local_3c * _DAT_00dceffc);
  local_2c = local_38;
  local_38 = (int)(longlong)ROUND((float)local_40 * _DAT_00daebb0);
  local_28 = local_38;
  lVar5 = (longlong)ROUND(_DAT_00dceffc * (float)local_44);
  local_38 = (int)lVar5;
  local_24 = local_38;
  local_20 = 0;
  local_1c = 0x3f800000;
  iVar6 = (**(code **)(*DAT_00dcec00 + 0xbc))(DAT_00dcec00,&local_30);
  _DAT_010ed97c = (float)local_30;
  if (local_30 < 0) {
    _DAT_010ed97c = _DAT_010ed97c + 4.2949673e+09;
  }
  _DAT_010ed980 = (float)local_2c;
  if (local_2c < 0) {
    _DAT_010ed980 = _DAT_010ed980 + 4.2949673e+09;
  }
  _DAT_00dcec5c = (float)local_38;
  if (local_38 < 0) {
    _DAT_00dcec5c = _DAT_00dcec5c + 4.2949673e+09;
  }
  _DAT_00dcec5c = _DAT_00dcec5c - _DAT_010ed984;
  iStack_34 = (int)((ulonglong)lVar5 >> 0x20);
  _DAT_01003068 = (float)iStack_34;
  if (lVar5 < 0) {
    _DAT_01003068 = _DAT_01003068 + 4.2949673e+09;
  }
  if (iVar6 != 0) {
    uStack_10 = 0;
    uStack_c = 0x3f800000;
    local_20 = 0;
    local_1c = 0;
    uStack_18 = 1;
    uStack_14 = 1;
    (**(code **)(*DAT_00dcec00 + 0xbc))(DAT_00dcec00,&local_20);
  }
  iVar6 = DAT_01003018;
  psVar1 = (short *)(DAT_01003018 + 0xe);
  psVar2 = (short *)(DAT_01003018 + 0xc);
  _DAT_00dcec4c = (float)(int)*(short *)(DAT_01003018 + 10) + _DAT_005aeb90;
  psVar3 = (short *)(DAT_01003018 + 0x12);
  _DAT_005aeb4c = (float)(int)*(short *)(DAT_01003018 + 8) + _DAT_00dcec58;
  DAT_005aa762 = 1;
  psVar4 = (short *)(DAT_01003018 + 0x10);
  DAT_01003018 = DAT_01003018 + 0x18;
  _DAT_010230b4 = (float)(int)*psVar1 + _DAT_005aeb90;
  _DAT_00dcec1c = (float)(int)*psVar2 + _DAT_00dcec58;
  _DAT_0100300c = (float)(int)*psVar3 + _DAT_005aeb90;
  _DAT_00dcebf8 = (float)(int)*psVar4 + _DAT_00dcec58;
  _DAT_005aeb94 = (float)(int)*(short *)(iVar6 + 0x16) + _DAT_005aeb90;
  _DAT_00dcec50 = _DAT_00dcec58 + (float)(int)*(short *)(iVar6 + 0x14);
  return;
}


// ===== FUN_004bb020 @ 004bb020 (216 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004bb020(void)

{
  uint in_EAX;
  int iVar1;
  undefined4 uVar2;
  
  if ((in_EAX & 0x10) == 0) {
    iVar1 = (in_EAX & 0xf) * 8;
  }
  else {
    iVar1 = (~in_EAX & 0xf) * -8 + -8;
  }
  DAT_00daebb4 = (ushort *)((int)DAT_00daebb4 + iVar1);
  if ((DAT_00daebb4[2] != _DAT_010ed990) || (*DAT_00daebb4 != DAT_010ed98c)) {
    FUN_004bab60();
  }
  if (((*DAT_00daebb4 ^ DAT_010ed98c) & 0x300) != 0) {
    if ((*DAT_00daebb4 & 0x100) == 0) {
      uVar2 = 1;
    }
    else {
      uVar2 = 2;
    }
    (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,1,uVar2);
    if ((*DAT_00daebb4 & 0x200) == 0) {
      (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,2,1);
    }
    else {
      (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,2,2);
    }
  }
  _DAT_010ed990 = (uint)DAT_00daebb4[2];
  DAT_010ed98c = (uint)*DAT_00daebb4;
  return;
}


// ===== FUN_004bb100 @ 004bb100 (76 bytes) =====


void FUN_004bb100(void)

{
  ushort uVar1;
  int unaff_EDI;
  
  if (unaff_EDI != 0) {
    uVar1 = *(ushort *)(DAT_00daebb4 + 4);
    if (DAT_00573338 != 3) {
      (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0,0);
    }
    (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0);
    DAT_00573338 = uVar1 >> 0xc & 1;
  }
  return;
}


// ===== FUN_004bb150 @ 004bb150 (301 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004bb150(void)

{
  ushort uVar1;
  int iVar2;
  uint3 in_ECX;
  undefined4 uVar3;
  undefined4 unaff_EDI;
  uint uVar4;
  byte bVar5;
  undefined4 uStack_4;
  
  uVar1 = *(ushort *)(DAT_005aa8cc + 0x2000 + (uint)((ushort)DAT_00daebb4[3] >> 6) * 2);
  _DAT_00dcec30 =
       (uint)CONCAT21(CONCAT11((char)uVar1 * '\b',(char)(uVar1 >> 5) << 3),(byte)(uVar1 >> 10) << 3)
  ;
  _DAT_0100301c = (ushort)DAT_00daebb4[1] & 0xff;
  _DAT_01003004 = 0xffffff;
  uStack_4 = (uint)in_ECX;
  DAT_01003060 = _DAT_00dcec30;
  iVar2 = FUN_004b86d0(*DAT_00daebb4,DAT_00daebb4[2],uVar1 & 0x7fff,(int)&uStack_4 + 3);
  if (iVar2 != 0) {
    uVar4 = (ushort)DAT_00daebb4[2] >> 0xc & 1;
    if (DAT_00573338 != 3) {
      (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0,0);
    }
    (**(code **)(*DAT_00dcec00 + 0x104))(DAT_00dcec00,0,iVar2);
    bVar5 = (byte)((uint)unaff_EDI >> 0x18);
    if (bVar5 == 0) {
      uVar3 = DAT_00dcec44;
      if (DAT_010ed971 == 0) {
        DAT_00573338 = uVar4;
        DAT_010ed971 = bVar5;
        return;
      }
    }
    else {
      if (DAT_010ed971 == bVar5) {
        DAT_00573338 = uVar4;
        DAT_010ed971 = bVar5;
        return;
      }
      uVar3 = (&DAT_00daebb4)[bVar5];
    }
    DAT_00573338 = uVar4;
    (**(code **)(*DAT_00dcec00 + 0x1ac))(DAT_00dcec00,uVar3);
    DAT_010ed971 = bVar5;
  }
  return;
}


// ===== FUN_004bb280 @ 004bb280 (375 bytes) =====


void FUN_004bb280(void)

{
  uint uVar1;
  uint uVar2;
  uint in_ECX;
  uint uVar3;
  
  uVar3 = DAT_01003018[1] & 0xffffff;
  uVar2 = *DAT_01003018 & 0xffffff;
  uVar1 = DAT_01003018[2];
  DAT_010430fc = 0;
  if (uVar2 == 0) {
    if (((uVar3 == 0) && (uVar1 == 0)) && (DAT_01003018[3] == 0)) {
      DAT_01003018 = DAT_01003018 + 4;
      return;
    }
  }
  else if (0x7fffff < uVar2) {
    DAT_00dceff4 = &DAT_01003070 + (*DAT_01003018 & 0xffff) * 2;
    goto LAB_004bb2e1;
  }
  DAT_00dceff4 = (undefined *)(DAT_005aa884 + uVar2 * 2);
LAB_004bb2e1:
  if (uVar3 < 0x800000) {
    DAT_00daebb4 = (undefined *)(DAT_005aa884 + uVar3 * 2);
  }
  else {
    DAT_00daebb4 = &DAT_01003070 + (DAT_01003018[1] & 0xffff) * 2;
  }
  if (DAT_010ed978 == 3) {
    uVar2 = in_ECX >> 6;
  }
  else {
    uVar2 = in_ECX >> 0x1d;
  }
  DAT_00dcec40 = uVar2 & 3;
  FUN_004b9910();
  PTR_DAT_00573330 = (undefined *)&DAT_010f00a8;
  if (DAT_005aa762 != '\0') {
    *(undefined2 *)(DAT_005aa728 + 0x11d60) = 0;
    FUN_004ba5f0();
    (**(code **)(*DAT_00dcec00 + 0xac))(DAT_00dcec00,0,0,2,&DAT_00ff00ff,0,0);
    DAT_005aa762 = '\0';
    _memset(&DAT_00dcf000,0,0x4000);
  }
  DAT_01003064 = 0;
  FUN_004bb150();
  if (uVar1 != 0x20000007) {
    (*DAT_00dceff8)();
    DAT_00dcec38 = DAT_00dcec38 + 1;
  }
  DAT_01003018 = DAT_01003018 + 4;
  return;
}


// ===== FUN_004bb400 @ 004bb400 (2481 bytes) =====


/* WARNING (jumptable): Unable to track spacebase fully for stack */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float * FUN_004bb400(void)

{
  longlong lVar1;
  float fVar2;
  float fVar3;
  bool bVar4;
  float *pfVar5;
  float *in_ECX;
  int iVar6;
  int in_EDX;
  uint uVar7;
  float *pfVar8;
  uint uVar9;
  float *pfVar10;
  uint uVar11;
  float10 fVar12;
  float10 fVar13;
  float10 fVar14;
  float10 fVar15;
  float10 fVar16;
  float10 fVar17;
  float10 fVar18;
  float fStack_144;
  float fStack_140;
  float fStack_13c;
  float fStack_138;
  undefined4 uStack_134;
  int local_130;
  int iStack_12c;
  int iStack_128;
  uint uStack_124;
  uint uStack_120;
  int local_11c;
  float afStack_118 [3];
  byte bStack_10c;
  float afStack_100 [4];
  uint uStack_f0;
  undefined4 uStack_ec;
  float fStack_e0;
  float fStack_dc;
  float fStack_d8;
  byte bStack_d4;
  float afStack_c8 [4];
  uint uStack_b8;
  undefined4 uStack_b4;
  longlong lStack_a8;
  undefined4 uStack_9c;
  float local_98 [3];
  byte bStack_8c;
  float afStack_80 [4];
  uint uStack_70;
  undefined4 uStack_6c;
  float fStack_60;
  float fStack_5c;
  float fStack_58;
  byte bStack_54;
  float afStack_48 [4];
  uint uStack_38;
  undefined4 uStack_34;
  float fStack_28;
  float fStack_24;
  float fStack_20;
  undefined4 uStack_1c;
  float fStack_18;
  float fStack_14;
  undefined4 uStack_c;
  
  local_130 = 1;
  local_11c = in_EDX;
  if (((*DAT_00daebb4 ^ DAT_010ed98c) & 0x300) != 0) {
    if ((*DAT_00daebb4 & 0x100) == 0) {
      (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,1,1);
    }
    else {
      (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,1,2);
    }
    if ((*DAT_00daebb4 & 0x200) == 0) {
      (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,2,1);
    }
    else {
      (**(code **)(*DAT_00dcec00 + 0x114))(DAT_00dcec00,0,2,2);
    }
  }
  iVar6 = DAT_010430fc;
  DAT_010ed98c = (uint)*DAT_00daebb4;
  _DAT_010ed990 = (uint)DAT_00daebb4[2];
  DAT_01003064 = 0;
  local_98[0] = *in_ECX;
  bStack_8c = *(byte *)in_ECX & 1;
  local_98[1] = in_ECX[1];
  DAT_00dcec10 = 0;
  local_98[2] = in_ECX[2];
  _DAT_00573334 = 0xfffe1dc0;
  PTR_DAT_00573330 = (undefined *)&DAT_010f00a8;
  if (DAT_010430fc == 0) {
    FUN_004b9cb0();
  }
  FUN_004b9b20();
  fStack_e0 = in_ECX[3];
  fStack_dc = in_ECX[4];
  bStack_d4 = *(byte *)(in_ECX + 3) & 1;
  pfVar8 = in_ECX + 6;
  fStack_d8 = in_ECX[5];
  if (iVar6 == 0) {
    FUN_004b9cb0();
  }
  FUN_004b9b20();
  iStack_128 = 0;
  if (0 < in_EDX) {
    do {
      fVar2 = *pfVar8;
      DAT_00dcec3c = (uint)fVar2 >> 10 & 3;
      uStack_120 = (uint)fVar2 >> 0xc & 0x1f;
      uVar11 = (uint)fVar2 >> 0x11 & 1;
      uVar7 = (uint)fVar2 >> 0x12 & 0x1f;
      uStack_124 = (uint)fVar2 >> 0x17 & 3;
      _DAT_00dcec48 = (uint)fVar2 >> 0x1b;
      uVar9 = (uint)fVar2 & 3;
      iStack_12c = 0;
      bVar4 = false;
      _DAT_00daebac = uVar11;
      DAT_00dcec24 = uVar7;
      if (uVar9 == 0) break;
      pfVar5 = pfVar8 + 1;
      if (DAT_00dcec08 == 0) {
        if (DAT_010430fc == 0) {
          fStack_140 = *pfVar5;
          pfVar5 = pfVar8 + 4;
          fStack_140 = -fStack_140;
          fStack_13c = -pfVar8[2];
          fStack_138 = -pfVar8[3];
        }
        else {
          _DAT_00dcec34 = (uint)*pfVar5 >> 0x17 & 0x7f;
          pfVar5 = pfVar8 + 3;
        }
      }
      else {
        PTR_DAT_00573330 = (undefined *)&DAT_010f0068;
        if (((uint)*pfVar5 & 1) == 0) {
          PTR_DAT_00573330 = (undefined *)&DAT_010f00a8;
        }
      }
      afStack_118[0] = *pfVar5;
      bStack_10c = *(byte *)pfVar5 & 1;
      afStack_118[1] = pfVar5[1];
      pfVar8 = pfVar5 + 3;
      afStack_118[2] = pfVar5[2];
      if (DAT_010430fc == 0) {
        FUN_004b9cb0();
      }
      if ((DAT_00dcec08 == 0) || (uVar9 != 2)) {
        fStack_60 = *pfVar8;
        bStack_54 = *(byte *)pfVar8 & 1;
        fStack_5c = pfVar5[4];
        pfVar8 = pfVar5 + 6;
        fStack_58 = pfVar5[5];
        if (DAT_010430fc == 0) {
          FUN_004b9cb0();
        }
        if (uVar9 == 2) goto LAB_004bb6bd;
      }
      else {
LAB_004bb6bd:
        iStack_12c = 1;
      }
      if (((DAT_010ed998 & 2) == 0) && (DAT_00dcec08 == 0)) {
        FUN_004b9df0();
        if (uVar11 == 0) {
          fVar12 = (float10)fStack_13c;
          fVar13 = (float10)fStack_140;
          fVar14 = (float10)fStack_138;
          if ((float10)fStack_d8 * fVar14 +
              (float10)fStack_e0 * fVar13 + (float10)fStack_dc * fVar12 <= (float10)0) {
            bVar4 = false;
          }
          else {
            bVar4 = true;
          }
        }
        else {
          fVar14 = (float10)fStack_138;
          fVar12 = (float10)fStack_13c;
          fVar13 = (float10)fStack_140;
        }
      }
      else {
        fVar12 = (float10)local_98[0] - (float10)fStack_e0;
        uStack_9c = 0;
        fVar16 = (float10)local_98[1] - (float10)fStack_dc;
        fVar15 = (float10)local_98[2] - (float10)fStack_d8;
        fVar18 = (float10)afStack_118[0] - (float10)fStack_e0;
        fVar17 = (float10)afStack_118[1] - (float10)fStack_dc;
        fVar14 = (float10)afStack_118[2] - (float10)fStack_d8;
        fVar13 = fVar14 * fVar16 - fVar17 * fVar15;
        DAT_010f01d4 = (float)fVar13;
        fStack_140 = DAT_010f01d4;
        fVar15 = fVar18 * fVar15 - fVar14 * fVar12;
        DAT_010f01d8 = (float)fVar15;
        fStack_13c = DAT_010f01d8;
        uStack_134 = DAT_010f01e0;
        fVar14 = fVar17 * fVar12 - fVar18 * fVar16;
        DAT_010f01dc = (float)fVar14;
        fStack_138 = DAT_010f01dc;
        fVar14 = SQRT(fVar14 * fVar14 + fVar13 * fVar13 + fVar15 * fVar15);
        if ((float10)0 == fVar14) {
          fVar14 = (float10)DAT_010f01dc;
          fVar13 = (float10)DAT_010f01d4;
          fVar12 = (float10)DAT_010f01d8;
        }
        else {
          fVar13 = fVar13 / fVar14;
          fStack_140 = (float)fVar13;
          fVar12 = (float10)DAT_010f01d8 / fVar14;
          fStack_13c = (float)fVar12;
          fVar14 = (float10)DAT_010f01dc / fVar14;
          fStack_138 = (float)fVar14;
        }
        if (uVar11 == 0) {
          if (fVar14 * (float10)fStack_d8 +
              fVar13 * (float10)fStack_e0 + fVar12 * (float10)fStack_dc <= (float10)0) {
            bVar4 = false;
          }
          else {
            bVar4 = true;
          }
        }
      }
      fVar16 = (float10)0;
      if (DAT_010430fc == 0) {
        uStack_1c = 0;
        fStack_28 = _DAT_010f005c;
        uStack_c = 0;
        fStack_24 = _DAT_010f0060;
        fStack_20 = _DAT_010f0064;
        fStack_18 = afStack_118[0];
        fStack_14 = afStack_118[1];
        fVar17 = (float10)_DAT_010f0064 * fVar14 +
                 (float10)_DAT_010f005c * fVar13 + (float10)_DAT_010f0060 * fVar12;
        fVar15 = fVar17;
        if ((float10)afStack_118[1] * fVar12 + (float10)afStack_118[0] * fVar13 +
            (float10)afStack_118[2] * fVar14 < fVar16) {
          fVar15 = fVar17 * (float10)-1.0;
        }
        if (fVar15 < fVar16) {
          fVar15 = fVar16;
        }
        fStack_144 = (float)fVar15;
        if ((DAT_010ed998 & 1) != 0) {
          fVar16 = (float10)FUN_004b99a0(&fStack_140,&fStack_28,(float)fVar17);
          fVar15 = (float10)fStack_144;
        }
        lVar1 = (longlong)
                ROUND((float10)*(float *)(&DAT_00dcec60 + uVar7 * 0x1c) * fVar15 +
                      (float10)*(float *)(&DAT_00dcec64 + uVar7 * 0x1c) + fVar16);
        lStack_a8._0_4_ = (uint)lVar1;
        if (0xff < (uint)lStack_a8) {
          lStack_a8._0_4_ = 0xff;
        }
        _DAT_00dcec34 = (uint)lStack_a8 >> 1;
        lStack_a8 = lVar1;
      }
      FUN_004b9b20();
      pfVar5 = (float *)FUN_004b9b20();
      if (iStack_12c != 0) {
        pfVar10 = afStack_118;
        for (iVar6 = 0xe; iVar6 != 0; iVar6 = iVar6 + -1) {
          *pfVar5 = *pfVar10;
          pfVar10 = pfVar10 + 1;
          pfVar5 = pfVar5 + 1;
        }
      }
      fVar2 = _DAT_0057333c;
      if (DAT_00dcec3c != 0) {
        fVar3 = fStack_d8;
        fVar2 = fStack_58;
        if (DAT_00dcec3c == 1) {
          if (local_98[2] < fStack_d8) {
            fVar3 = local_98[2];
          }
          if (afStack_118[2] < fVar3) {
            fVar3 = afStack_118[2];
          }
          if (fVar3 <= fStack_58) {
LAB_004bba56:
            fVar2 = fVar3;
          }
        }
        else if (DAT_00dcec3c == 2) {
          if (fStack_d8 < local_98[2]) {
            fVar3 = local_98[2];
          }
          if (fVar3 < afStack_118[2]) {
            fVar3 = afStack_118[2];
          }
          if (fStack_58 <= fVar3) goto LAB_004bba56;
        }
        else {
          fVar2 = 0.0;
        }
      }
      _DAT_0057333c = fVar2;
      if (uStack_124 == 0) {
        local_130 = 1;
        if (iStack_12c == 0) {
          DAT_00dceff4 = DAT_00dceff4 + 0x10;
        }
        else {
          DAT_00dceff4 = DAT_00dceff4 + 0xc;
        }
      }
      else {
        if (!bVar4) {
          FUN_004bb150();
        }
        uStack_38 = 0xff;
        if ((*DAT_00daebb4 & 0x4000) == 0) {
          uStack_38 = 0;
        }
        if ((DAT_005aa763 == '\0') || (local_130 != 0)) {
          uStack_b8 = (uint)CONCAT11(DAT_00dcec34 * '\x02',DAT_0100301c) << 8 | uStack_38;
          uStack_b4 = DAT_01003060;
          uStack_6c = DAT_01003060;
          uStack_70 = uStack_b8;
          uStack_38 = uStack_b8;
        }
        else {
          uStack_38 = (uint)CONCAT11(DAT_00dcec34 * '\x02',DAT_0100301c) << 8 | uStack_38;
        }
        uStack_ec = DAT_01003060;
        uStack_34 = DAT_01003060;
        local_130 = 0;
        uStack_f0 = uStack_38;
        fVar13 = (float10)FUN_00452fc0(fVar2);
        afStack_c8[2] = (float)fVar13;
        afStack_80[2] = (float)fVar13;
        afStack_48[2] = (float)fVar13;
        afStack_100[2] = (float)fVar13;
        FUN_004ba550();
        FUN_004ba550();
        FUN_004ba550();
        if (iStack_12c == 0) {
          FUN_004ba550();
          if (!bVar4) {
            pfVar5 = afStack_100;
            pfVar10 = DAT_01003010 + -0x18;
            for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
              *pfVar10 = *pfVar5;
              pfVar5 = pfVar5 + 1;
              pfVar10 = pfVar10 + 1;
            }
            pfVar5 = afStack_48;
            pfVar10 = DAT_01003010 + -0x10;
            for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
              *pfVar10 = *pfVar5;
              pfVar5 = pfVar5 + 1;
              pfVar10 = pfVar10 + 1;
            }
            pfVar5 = afStack_c8;
            pfVar10 = DAT_01003010 + -8;
            for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
              *pfVar10 = *pfVar5;
              pfVar5 = pfVar5 + 1;
              pfVar10 = pfVar10 + 1;
            }
            DAT_01003064 = DAT_01003064 + 3;
            pfVar5 = afStack_c8;
            pfVar10 = DAT_01003010 + -0x30;
            for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
              *pfVar10 = *pfVar5;
              pfVar5 = pfVar5 + 1;
              pfVar10 = pfVar10 + 1;
            }
            pfVar5 = afStack_80;
            pfVar10 = DAT_01003010 + -0x28;
            for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
              *pfVar10 = *pfVar5;
              pfVar5 = pfVar5 + 1;
              pfVar10 = pfVar10 + 1;
            }
            pfVar5 = afStack_100;
            pfVar10 = DAT_01003010 + -0x20;
            for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
              *pfVar10 = *pfVar5;
              pfVar5 = pfVar5 + 1;
              pfVar10 = pfVar10 + 1;
            }
            DAT_01003064 = DAT_01003064 + 3;
            DAT_00dcec20 = DAT_00dcec20 + 2;
            DAT_01003010 = DAT_01003010 + -0x30;
          }
        }
        else if (!bVar4) {
          pfVar5 = afStack_c8;
          pfVar10 = DAT_01003010 + -0x18;
          for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
            *pfVar10 = *pfVar5;
            pfVar5 = pfVar5 + 1;
            pfVar10 = pfVar10 + 1;
          }
          pfVar5 = afStack_80;
          pfVar10 = DAT_01003010 + -0x10;
          for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
            *pfVar10 = *pfVar5;
            pfVar5 = pfVar5 + 1;
            pfVar10 = pfVar10 + 1;
          }
          pfVar5 = afStack_100;
          pfVar10 = DAT_01003010 + -8;
          for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
            *pfVar10 = *pfVar5;
            pfVar5 = pfVar5 + 1;
            pfVar10 = pfVar10 + 1;
          }
          DAT_01003064 = DAT_01003064 + 3;
          DAT_00dcec20 = DAT_00dcec20 + 1;
          DAT_01003010 = DAT_01003010 + -0x18;
        }
      }
      FUN_004bb020();
      switch(uStack_124) {
      case 0:
      case 2:
        pfVar5 = afStack_118;
        pfVar10 = local_98;
        for (iVar6 = 0xe; iVar6 != 0; iVar6 = iVar6 + -1) {
          *pfVar10 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar10 = pfVar10 + 1;
        }
        pfVar5 = &fStack_e0;
        break;
      case 1:
        pfVar10 = afStack_118;
        pfVar5 = &fStack_e0;
        goto LAB_004bbd27;
      case 3:
        pfVar5 = local_98;
        break;
      default:
        goto switchD_004bbcf0_default;
      }
      pfVar10 = &fStack_60;
LAB_004bbd27:
      for (iVar6 = 0xe; iVar6 != 0; iVar6 = iVar6 + -1) {
        *pfVar5 = *pfVar10;
        pfVar10 = pfVar10 + 1;
        pfVar5 = pfVar5 + 1;
      }
switchD_004bbcf0_default:
      DAT_00dcec0c = DAT_00dcec0c + 1;
      DAT_00dcec10 = DAT_00dcec10 + 1;
      iStack_128 = iStack_128 + 1;
    } while (iStack_128 < local_11c);
    if (DAT_01003064 != 0) {
      if ((*DAT_00daebb4 & 0x8000) == 0) {
        (**(code **)(*DAT_00dcec00 + 0x14c))(DAT_00dcec00,4,DAT_01003064 / 3,DAT_01003010,0x20);
      }
      else {
        FUN_004ba9c0(*DAT_00daebb4);
      }
      DAT_01003064 = 0;
    }
  }
  _DAT_00daeba8 = _DAT_00daeba8 + 1;
  return pfVar8;
}


// ===== FUN_004bbdd0 @ 004bbdd0 (613 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004bbdd0(int param_1,int param_2)

{
  UINT UVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int *in_ECX;
  int in_EDX;
  undefined4 *puVar5;
  float fStack_30;
  float fStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  _DAT_00573328 = (float)in_EDX;
  if (in_EDX < 0) {
    _DAT_00573328 = _DAT_00573328 + 4.2949673e+09;
  }
  _DAT_00573328 = _DAT_00573328 * 0.002016129;
  _DAT_00dceffc = (float)param_1;
  if (param_1 < 0) {
    _DAT_00dceffc = _DAT_00dceffc + 4.2949673e+09;
  }
  _DAT_00dceffc = _DAT_00dceffc * 0.0026041667;
  DAT_00dcefe0 = param_2;
  DAT_00dceff8 = FUN_004bb400;
  _DAT_00daebb0 = _DAT_00573328;
  DAT_00dcec00 = in_ECX;
  UVar1 = GetPrivateProfileIntA("Renderer","FakeGouraud",0,".\\emulator.ini");
  DAT_005aa763 = UVar1 != 0;
  UVar1 = GetPrivateProfileIntA("Renderer","Bilinear",1,".\\emulator.ini");
  if (UVar1 != 0) {
    DAT_010ed994 = 1;
  }
  DAT_010ed960 = 0;
  UVar1 = GetPrivateProfileIntA("Renderer","Trilinear",0,".\\emulator.ini");
  if (UVar1 != 0) {
    DAT_010ed960 = 1;
  }
  DAT_010ed964 = 0;
  UVar1 = GetPrivateProfileIntA("Renderer","AutoMip",0,".\\emulator.ini");
  if (UVar1 != 0) {
    DAT_010ed964 = 1;
  }
  UVar1 = GetPrivateProfileIntA("Renderer","FSAA",0,".\\emulator.ini");
  if (UVar1 != 0) {
    (**(code **)(*DAT_00dcec00 + 0xe4))(DAT_00dcec00,0xb0,1);
  }
  DAT_010ed959 = 0;
  UVar1 = GetPrivateProfileIntA("Renderer","Wireframe",0,".\\emulator.ini");
  if (UVar1 != 0) {
    DAT_010ed959 = 1;
  }
  if (*(int *)(DAT_00dcefe0 + 0x70) == 1) {
    _DAT_00573244 = 0x3f400000;
  }
  else if (*(int *)(DAT_00dcefe0 + 0x70) == 2) {
    _DAT_00573244 = 0x3f55551d;
  }
  else {
    _DAT_00573244 = 0x3f800000;
  }
  DAT_010ed973 = 0;
  UVar1 = GetPrivateProfileIntA("Renderer","MeshTransparency",0,".\\emulator.ini");
  if (UVar1 != 0) {
    DAT_010ed973 = 1;
  }
  FUN_004b90c0();
  FUN_004b9060();
  iVar2 = FUN_005008f0();
  fStack_30 = (float)iVar2;
  iVar2 = FUN_005008f0();
  fStack_2c = (float)iVar2;
  uStack_28 = 0x3f800000;
  uStack_24 = 0x3f800000;
  (**(code **)(*DAT_00dcec00 + 0x1b4))(DAT_00dcec00,2,&fStack_30,1);
  uStack_20 = 0;
  uStack_1c = 0x3f800000;
  fStack_30 = 0.0;
  fStack_2c = 0.0;
  uStack_28 = 0x1f0;
  uStack_24 = 0x180;
  (**(code **)(*DAT_00dcec00 + 0xbc))(DAT_00dcec00,&fStack_30);
  DAT_010ed968 = 0;
  puVar3 = &DAT_00dd3000;
  do {
    puVar4 = (undefined4 *)((int)puVar3 + 0x46);
    puVar5 = &DAT_010230b8;
    for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar3 = *puVar5;
      puVar5 = puVar5 + 1;
      puVar3 = puVar3 + 1;
    }
    puVar3 = puVar4;
  } while ((int)puVar4 < 0x1003000);
  FUN_004b85d0();
  FUN_004b9e80();
  return;
}


// ===== FUN_004bc040 @ 004bc040 (15 bytes) =====


void FUN_004bc040(void)

{
  uint uVar1;
  uint uVar2;
  uint in_ECX;
  uint uVar3;
  
  DAT_00dcec08 = 1;
  uVar3 = DAT_01003018[1] & 0xffffff;
  uVar2 = *DAT_01003018 & 0xffffff;
  uVar1 = DAT_01003018[2];
  DAT_010430fc = 0;
  if (uVar2 == 0) {
    if (((uVar3 == 0) && (uVar1 == 0)) && (DAT_01003018[3] == 0)) {
      DAT_01003018 = DAT_01003018 + 4;
      return;
    }
  }
  else if (0x7fffff < uVar2) {
    DAT_00dceff4 = &DAT_01003070 + (*DAT_01003018 & 0xffff) * 2;
    goto LAB_004bb2e1;
  }
  DAT_00dceff4 = (undefined *)(DAT_005aa884 + uVar2 * 2);
LAB_004bb2e1:
  if (uVar3 < 0x800000) {
    DAT_00daebb4 = (undefined *)(DAT_005aa884 + uVar3 * 2);
  }
  else {
    DAT_00daebb4 = &DAT_01003070 + (DAT_01003018[1] & 0xffff) * 2;
  }
  if (DAT_010ed978 == 3) {
    uVar2 = in_ECX >> 6;
  }
  else {
    uVar2 = in_ECX >> 0x1d;
  }
  DAT_00dcec40 = uVar2 & 3;
  FUN_004b9910();
  PTR_DAT_00573330 = (undefined *)&DAT_010f00a8;
  if (DAT_005aa762 != '\0') {
    *(undefined2 *)(DAT_005aa728 + 0x11d60) = 0;
    FUN_004ba5f0();
    (**(code **)(*DAT_00dcec00 + 0xac))(DAT_00dcec00,0,0,2,&DAT_00ff00ff,0,0);
    DAT_005aa762 = '\0';
    _memset(&DAT_00dcf000,0,0x4000);
  }
  DAT_01003064 = 0;
  FUN_004bb150();
  if (uVar1 != 0x20000007) {
    (*DAT_00dceff8)();
    DAT_00dcec38 = DAT_00dcec38 + 1;
  }
  DAT_01003018 = DAT_01003018 + 4;
  return;
}


// ===== FUN_004bc050 @ 004bc050 (68 bytes) =====


void FUN_004bc050(void)

{
  PTR_FUN_00573384 = FUN_004bc040;
  PTR_FUN_005733ac = FUN_004ba010;
  PTR_FUN_005733b0 = FUN_004b94a0;
  PTR_FUN_0057342c = FUN_004b9460;
  PTR_FUN_00573454 = FUN_004b93e0;
  PTR_FUN_00573458 = FUN_004b9900;
  DAT_010ed95a = 1;
  return;
}


// ===== FUN_004bc0a0 @ 004bc0a0 (21 bytes) =====


void FUN_004bc0a0(void)

{
  DAT_010edee4 = &LAB_004c6100;
  DAT_010edee8 = &LAB_004c6020;
  return;
}


// ===== FUN_004bc0c0 @ 004bc0c0 (6 bytes) =====


void FUN_004bc0c0(void)

{
  undefined4 in_EAX;
  
  DAT_010ed950 = in_EAX;
  return;
}


// ===== FUN_004bc0d0 @ 004bc0d0 (18 bytes) =====


void FUN_004bc0d0(size_t param_1)

{
  void *in_ECX;
  void *in_EDX;
  
  FID_conflict__memcpy(in_EDX,in_ECX,param_1);
  return;
}


// ===== FUN_004bc0f0 @ 004bc0f0 (40 bytes) =====


void FUN_004bc0f0(size_t param_1)

{
  void *in_ECX;
  void *in_EDX;
  
  FID_conflict__memcpy(in_ECX,in_EDX,param_1);
  *(int *)((int)in_ECX + 0x2a8) = *(int *)((int)in_ECX + 8) * 0x6c + *(int *)((int)in_ECX + 0x2ac);
  return;
}


// ===== FUN_004bc120 @ 004bc120 (16 bytes) =====


undefined * FUN_004bc120(void)

{
  DAT_010eded4 = 0;
  return &DAT_010edc30;
}


// ===== FUN_004bc130 @ 004bc130 (50 bytes) =====


float FUN_004bc130(void)

{
  float in_ECX;
  float in_EDX;
  
  if (in_ECX <= in_EDX) {
    return in_EDX;
  }
  return in_ECX;
}


// ===== FUN_004bc170 @ 004bc170 (50 bytes) =====


float FUN_004bc170(void)

{
  float in_ECX;
  float in_EDX;
  
  if (in_EDX <= in_ECX) {
    return in_EDX;
  }
  return in_ECX;
}


// ===== FUN_004bc1b0 @ 004bc1b0 (71 bytes) =====


int FUN_004bc1b0(void)

{
  int iVar1;
  float in_ECX;
  
  iVar1 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xfffffffd;
  if (in_ECX == 0.0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 2;
    return -0x800000;
  }
  return ((uint)in_ECX >> 0x17 & 0xff) - 0x7f;
}


// ===== FUN_004bc200 @ 004bc200 (3 bytes) =====


undefined4 FUN_004bc200(void)

{
  return 0;
}


// ===== FUN_004bc210 @ 004bc210 (44 bytes) =====


undefined4 FUN_004bc210(void)

{
  float in_ECX;
  undefined4 local_8;
  
  local_8 = (undefined4)(longlong)ROUND(in_ECX);
  return local_8;
}


// ===== FUN_004bc240 @ 004bc240 (35 bytes) =====


uint FUN_004bc240(void)

{
  uint in_ECX;
  uint in_EDX;
  uint uVar1;
  
  uVar1 = (in_EDX ^ (int)in_EDX >> 0x1f) - ((int)in_EDX >> 0x1f);
  if ((int)uVar1 <= (int)((in_ECX ^ (int)in_ECX >> 0x1f) - ((int)in_ECX >> 0x1f))) {
    if ((int)in_ECX < 0) {
      uVar1 = -uVar1;
    }
    return uVar1;
  }
  return in_ECX;
}


// ===== FUN_004bc270 @ 004bc270 (155 bytes) =====


undefined4 FUN_004bc270(void)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  int in_ECX;
  
  fVar3 = **(float **)(in_ECX + 0x20) * **(float **)(in_ECX + 0x1c);
  fVar5 = **(float **)(in_ECX + 4) + **(float **)(in_ECX + 0xc);
  pfVar1 = *(float **)(in_ECX + 0x10);
  fVar4 = **(float **)(in_ECX + 4) - **(float **)(in_ECX + 0xc);
  **(float **)(in_ECX + 0x18) = fVar3;
  iVar6 = DAT_005aeb40;
  pfVar2 = *(float **)(in_ECX + 0x14);
  *pfVar1 = fVar5;
  *pfVar2 = fVar4;
  *(uint *)(iVar6 + 0x240) = *(uint *)(iVar6 + 0x240) & 0xffffffba;
  if ((fVar4 == 0.0) || (fVar5 == 0.0)) {
    *(uint *)(iVar6 + 0x240) = *(uint *)(iVar6 + 0x240) | 1;
  }
  if ((fVar4 < 0.0) || (fVar5 < 0.0)) {
    *(uint *)(iVar6 + 0x240) = *(uint *)(iVar6 + 0x240) | 4;
  }
  if (fVar3 < 0.0) {
    *(uint *)(iVar6 + 0x240) = *(uint *)(iVar6 + 0x240) | 0x40;
  }
  return 0;
}


// ===== FUN_004bc310 @ 004bc310 (129 bytes) =====


undefined4 FUN_004bc310(void)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  undefined4 *in_ECX;
  
  fVar2 = *(float *)in_ECX[2] + *(float *)*in_ECX;
  fVar3 = *(float *)*in_ECX - *(float *)in_ECX[2];
  pfVar1 = (float *)in_ECX[5];
  *(float *)in_ECX[4] = fVar2;
  *pfVar1 = fVar3;
  iVar4 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xfffffffa;
  if ((fVar2 == 0.0) || (fVar3 == 0.0)) {
    *(uint *)(iVar4 + 0x240) = *(uint *)(iVar4 + 0x240) | 1;
  }
  if (fVar2 < 0.0) {
    *(uint *)(iVar4 + 0x240) = *(uint *)(iVar4 + 0x240) | 4;
  }
  else if (fVar3 < 0.0) {
    *(uint *)(iVar4 + 0x240) = *(uint *)(iVar4 + 0x240) | 4;
    return 0;
  }
  return 0;
}


// ===== FUN_004bc3a0 @ 004bc3a0 (80 bytes) =====


undefined4 FUN_004bc3a0(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 *in_ECX;
  
  iVar1 = *(int *)*in_ECX + *(int *)in_ECX[2];
  iVar3 = *(int *)*in_ECX - *(int *)in_ECX[2];
  *(int *)in_ECX[4] = iVar1;
  *(int *)in_ECX[5] = iVar3;
  iVar2 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xfffffffa;
  if ((iVar1 == 0) || (iVar3 == 0)) {
    *(uint *)(iVar2 + 0x240) = *(uint *)(iVar2 + 0x240) | 1;
  }
  if ((iVar1 < 0) || (iVar3 < 0)) {
    *(uint *)(iVar2 + 0x240) = *(uint *)(iVar2 + 0x240) | 4;
  }
  return 0;
}


// ===== FUN_004bc3f0 @ 004bc3f0 (45 bytes) =====


void FUN_004bc3f0(void)

{
  uint in_ECX;
  byte in_DL;
  
  if ((in_ECX & 1 << (in_DL & 0x1f)) != 0) {
    *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffefff;
    return;
  }
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) | 0x1000;
  return;
}


// ===== FUN_004bc420 @ 004bc420 (70 bytes) =====


int FUN_004bc420(void)

{
  int iVar1;
  uint uVar2;
  uint in_ECX;
  uint in_EDX;
  
  iVar1 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffefff;
  uVar2 = (1 << ((byte)(in_EDX >> 6) & 0x1f)) - 1U & in_ECX;
  if (uVar2 == 0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 0x1000;
  }
  return uVar2 << ((byte)in_EDX & 0x1f);
}


// ===== FUN_004bc470 @ 004bc470 (68 bytes) =====


void FUN_004bc470(void)

{
  int iVar1;
  uint in_ECX;
  uint in_EDX;
  
  iVar1 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffefff;
  if (((1 << ((byte)(in_EDX >> 6) & 0x1f)) - 1U & in_ECX >> ((byte)in_EDX & 0x1f)) == 0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 0x1000;
  }
  return;
}


// ===== FUN_004bc4c0 @ 004bc4c0 (76 bytes) =====


uint FUN_004bc4c0(void)

{
  int iVar1;
  uint uVar2;
  int in_ECX;
  uint in_EDX;
  
  iVar1 = DAT_005aeb40;
  uVar2 = (1 << ((byte)(in_EDX >> 6) & 0x1f)) - 1U & *(uint *)(in_ECX + 0xc);
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffefff;
  if (uVar2 == 0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 0x1000;
  }
  return uVar2 << ((byte)in_EDX & 0x1f) | *(uint *)(in_ECX + 8);
}


// ===== FUN_004bc510 @ 004bc510 (70 bytes) =====


int FUN_004bc510(void)

{
  int iVar1;
  uint uVar2;
  uint in_ECX;
  uint in_EDX;
  
  iVar1 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffefff;
  uVar2 = (1 << ((byte)(in_EDX >> 6) & 0x1f)) - 1U & in_ECX;
  if (uVar2 == 0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 0x1000;
  }
  return uVar2 << ((byte)in_EDX & 0x1f);
}


// ===== FUN_004bc560 @ 004bc560 (80 bytes) =====


uint FUN_004bc560(void)

{
  int iVar1;
  uint uVar2;
  int in_ECX;
  uint in_EDX;
  
  iVar1 = DAT_005aeb40;
  uVar2 = (1 << ((byte)(in_EDX >> 6) & 0x1f)) - 1U & **(uint **)(in_ECX + 0xc);
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffefff;
  if (uVar2 == 0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 0x1000;
  }
  return uVar2 << ((byte)in_EDX & 0x1f) | **(uint **)(in_ECX + 8);
}


// ===== FUN_004bc5b0 @ 004bc5b0 (70 bytes) =====


int FUN_004bc5b0(void)

{
  int iVar1;
  uint uVar2;
  uint in_ECX;
  uint in_EDX;
  
  iVar1 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffefff;
  uVar2 = (1 << ((byte)(in_EDX >> 6) & 0x1f)) - 1U & in_ECX;
  if (uVar2 == 0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 0x1000;
  }
  return uVar2 << ((byte)in_EDX & 0x1f);
}


// ===== FUN_004bc600 @ 004bc600 (38 bytes) =====


void FUN_004bc600(void)

{
  int iVar1;
  int in_ECX;
  int in_EDX;
  
  iVar1 = DAT_005aeb40;
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xffffffbf;
  if (in_ECX * in_EDX < 0) {
    *(uint *)(iVar1 + 0x240) = *(uint *)(iVar1 + 0x240) | 0x40;
  }
  return;
}


// ===== FUN_004bc630 @ 004bc630 (3 bytes) =====


void FUN_004bc630(void)

{
  return;
}


// ===== FUN_004bc640 @ 004bc640 (3 bytes) =====


void FUN_004bc640(void)

{
  return;
}


// ===== FUN_004bc650 @ 004bc650 (1 bytes) =====


void FUN_004bc650(void)

{
  return;
}


// ===== FUN_004bc660 @ 004bc660 (20 bytes) =====


void FUN_004bc660(void)

{
  uint in_EAX;
  
  *(undefined **)(DAT_005aeb10 + 8) = (&PTR_FUN_00573490)[in_EAX & 0x1f];
  return;
}


// ===== FUN_004bc680 @ 004bc680 (6 bytes) =====


undefined4 FUN_004bc680(void)

{
  return 1;
}


// ===== FUN_004bc690 @ 004bc690 (20 bytes) =====


uint FUN_004bc690(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 0x12) & 1;
}


// ===== FUN_004bc6b0 @ 004bc6b0 (20 bytes) =====


uint FUN_004bc6b0(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 0x16) & 1;
}


// ===== FUN_004bc6d0 @ 004bc6d0 (20 bytes) =====


uint FUN_004bc6d0(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 0x15) & 1;
}


// ===== FUN_004bc6f0 @ 004bc6f0 (37 bytes) =====


uint FUN_004bc6f0(void)

{
  if ((*(uint *)(DAT_005aeb40 + 0x240) & 0x100000) != 0) {
    DAT_010ed950 = 1;
  }
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 0x14) & 1;
}


// ===== FUN_004bc720 @ 004bc720 (37 bytes) =====


uint FUN_004bc720(void)

{
  if ((*(uint *)(DAT_005aeb40 + 0x240) & 0x80000) != 0) {
    DAT_010ed950 = 1;
  }
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 0x13) & 1;
}


// ===== FUN_004bc750 @ 004bc750 (20 bytes) =====


uint FUN_004bc750(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 0xc) & 1;
}


// ===== FUN_004bc770 @ 004bc770 (20 bytes) =====


uint FUN_004bc770(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 0xb) & 1;
}


// ===== FUN_004bc790 @ 004bc790 (20 bytes) =====


uint FUN_004bc790(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 6) & 1;
}


// ===== FUN_004bc7b0 @ 004bc7b0 (20 bytes) =====


uint FUN_004bc7b0(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 7) & 1;
}


// ===== FUN_004bc7d0 @ 004bc7d0 (19 bytes) =====


uint FUN_004bc7d0(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 1) & 1;
}


// ===== FUN_004bc7f0 @ 004bc7f0 (20 bytes) =====


uint FUN_004bc7f0(void)

{
  return ~(*(uint *)(DAT_005aeb40 + 0x240) >> 3) & 1;
}


// ===== FUN_004bc810 @ 004bc810 (23 bytes) =====


undefined4 FUN_004bc810(void)

{
  if ((*(byte *)(DAT_005aeb40 + 0x240) & 5) == 0) {
    return 1;
  }
  return 0;
}


// ===== FUN_004bc830 @ 004bc830 (28 bytes) =====


undefined4 FUN_004bc830(void)

{
  if (((*(uint *)(DAT_005aeb40 + 0x240) & 1) == 0) && ((*(uint *)(DAT_005aeb40 + 0x240) & 4) != 0))
  {
    return 0;
  }
  return 1;
}


// ===== FUN_004bc850 @ 004bc850 (17 bytes) =====


uint FUN_004bc850(void)

{
  return ~*(uint *)(DAT_005aeb40 + 0x240) & 1;
}


// ===== FUN_004bc870 @ 004bc870 (16 bytes) =====


bool FUN_004bc870(void)

{
  return *(int *)(DAT_005aeb40 + 0x18) != 1;
}


// ===== FUN_004bc880 @ 004bc880 (16 bytes) =====


bool FUN_004bc880(void)

{
  return *(int *)(DAT_005aeb40 + 0x18) == 1;
}


// ===== FUN_004bc890 @ 004bc890 (3 bytes) =====


undefined4 FUN_004bc890(void)

{
  return 0;
}


// ===== FUN_004bc8a0 @ 004bc8a0 (17 bytes) =====


uint FUN_004bc8a0(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 0x40000;
}


// ===== FUN_004bc8c0 @ 004bc8c0 (17 bytes) =====


uint FUN_004bc8c0(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 0x400000;
}


// ===== FUN_004bc8e0 @ 004bc8e0 (17 bytes) =====


uint FUN_004bc8e0(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 0x200000;
}


// ===== FUN_004bc900 @ 004bc900 (29 bytes) =====


void FUN_004bc900(void)

{
  if ((*(uint *)(DAT_005aeb40 + 0x240) & 0x100000) != 0) {
    DAT_010ed950 = 1;
  }
  return;
}


// ===== FUN_004bc920 @ 004bc920 (29 bytes) =====


void FUN_004bc920(void)

{
  if ((*(uint *)(DAT_005aeb40 + 0x240) & 0x80000) != 0) {
    DAT_010ed950 = 1;
  }
  return;
}


// ===== FUN_004bc940 @ 004bc940 (17 bytes) =====


uint FUN_004bc940(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 0x1000;
}


// ===== FUN_004bc960 @ 004bc960 (17 bytes) =====


uint FUN_004bc960(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 0x800;
}


// ===== FUN_004bc980 @ 004bc980 (15 bytes) =====


uint FUN_004bc980(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 0x40;
}


// ===== FUN_004bc990 @ 004bc990 (17 bytes) =====


uint FUN_004bc990(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 0x80;
}


// ===== FUN_004bc9b0 @ 004bc9b0 (15 bytes) =====


uint FUN_004bc9b0(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 2;
}


// ===== FUN_004bc9c0 @ 004bc9c0 (15 bytes) =====


uint FUN_004bc9c0(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 8;
}


// ===== FUN_004bc9d0 @ 004bc9d0 (23 bytes) =====


undefined4 FUN_004bc9d0(void)

{
  if ((*(byte *)(DAT_005aeb40 + 0x240) & 5) == 0) {
    return 0;
  }
  return 1;
}


// ===== FUN_004bc9f0 @ 004bc9f0 (28 bytes) =====


undefined4 FUN_004bc9f0(void)

{
  if (((*(uint *)(DAT_005aeb40 + 0x240) & 1) == 0) && ((*(uint *)(DAT_005aeb40 + 0x240) & 4) != 0))
  {
    return 1;
  }
  return 0;
}


// ===== FUN_004bca10 @ 004bca10 (15 bytes) =====


uint FUN_004bca10(void)

{
  return *(uint *)(DAT_005aeb40 + 0x240) & 1;
}


// ===== FUN_004bca20 @ 004bca20 (3 bytes) =====


undefined4 FUN_004bca20(void)

{
  return 0;
}


// ===== FUN_004bca30 @ 004bca30 (6 bytes) =====


undefined4 FUN_004bca30(void)

{
  return 1;
}


// ===== FUN_004bca40 @ 004bca40 (1 bytes) =====


void FUN_004bca40(void)

{
  return;
}


// ===== FUN_004bca50 @ 004bca50 (42 bytes) =====


void FUN_004bca50(void)

{
  if (**(int **)(DAT_005aeb10 + 0x24) == *(int *)(DAT_005aeb10 + 0x1c)) {
    *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) | 0x40000;
    return;
  }
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xfffbffff;
  return;
}


// ===== FUN_004bca80 @ 004bca80 (47 bytes) =====


void FUN_004bca80(void)

{
  if ((**(uint **)(DAT_005aeb10 + 0x24) & *(uint *)(DAT_005aeb10 + 0x1c)) ==
      *(uint *)(DAT_005aeb10 + 0x1c)) {
    *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) | 0x40000;
    return;
  }
  *(uint *)(DAT_005aeb40 + 0x240) = *(uint *)(DAT_005aeb40 + 0x240) & 0xfffbffff;
  return;
}


// ===== FUN_004bcab0 @ 004bcab0 (15 bytes) =====


void FUN_004bcab0(void)

{
  **(uint **)(DAT_005aeb10 + 0x24) =
       **(uint **)(DAT_005aeb10 + 0x24) ^ *(uint *)(DAT_005aeb10 + 0x1c);
  return;
}


// ===== FUN_004bcac0 @ 004bcac0 (14 bytes) =====


void FUN_004bcac0(void)

{
  **(undefined4 **)(DAT_005aeb10 + 0x24) = *(undefined4 *)(DAT_005aeb10 + 0x1c);
  return;
}


// ===== FUN_004bcad0 @ 004bcad0 (54 bytes) =====


void FUN_004bcad0(void)

{
  int iVar1;
  
  iVar1 = (**(code **)(DAT_005aeb10 + 8))();
  if (iVar1 != 0) {
    *(int *)(DAT_005aeb40 + 8) = *(int *)(DAT_005aeb10 + 0x1c) + -1;
    DAT_005aeb10 = *(int *)(DAT_005aeb10 + 0x1c) * 0x6c + -0x6c + DAT_005aeb38;
  }
  return;
}


// ===== FUN_004bcb10 @ 004bcb10 (60 bytes) =====


void FUN_004bcb10(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = (**(code **)(DAT_005aeb10 + 8))();
  if (iVar2 != 0) {
    uVar1 = **(undefined4 **)(DAT_005aeb10 + 0x28);
    uVar3 = (**(code **)(DAT_005aeb10 + 4))();
    iVar2 = DAT_005aeb10;
    **(undefined4 **)(DAT_005aeb10 + 0x3c) = uVar3;
    **(undefined4 **)(iVar2 + 0x24) = uVar1;
  }
  return;
}


// ===== FUN_004bcb50 @ 004bcb50 (46 bytes) =====


void FUN_004bcb50(void)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(DAT_005aeb10 + 8))();
  if (iVar1 != 0) {
    uVar2 = (**(code **)(DAT_005aeb10 + 4))();
    **(undefined4 **)(DAT_005aeb10 + 0x3c) = uVar2;
  }
  return;
}


// ===== FUN_004bcb80 @ 004bcb80 (302 bytes) =====


int FUN_004bcb80(void)

{
  byte in_AL;
  byte bVar1;
  
  bVar1 = in_AL & 0xf;
  switch(in_AL >> 4) {
  case 0:
    return DAT_005aeb40 + 0xd8 + (uint)bVar1 * 4;
  case 1:
    return DAT_005aeb40 + 0x138 + (uint)bVar1 * 4;
  case 2:
    return DAT_005aeb40 + 0x178 + (uint)bVar1 * 4;
  case 3:
    return DAT_005aeb40 + 0x1f8 + (uint)bVar1 * 4;
  case 4:
    *(ushort *)(DAT_005aeb10 + 0x2c) = *(ushort *)(DAT_005aeb10 + 0x2c) | 0x80;
    return DAT_005aeb40 + 0x1b8 + (uint)bVar1 * 4;
  case 6:
    switch(bVar1) {
    case 0:
      goto switchD_004bcc5c_caseD_9;
    case 1:
      return DAT_005aeb40 + 4;
    case 3:
      return DAT_005aeb40 + 8;
    case 4:
      return DAT_005aeb40 + 0xc;
    case 5:
      return DAT_005aeb40 + 0x10;
    case 6:
      return DAT_005aeb40 + 0x14;
    case 7:
      return DAT_005aeb40 + 0x18;
    case 8:
      return DAT_005aeb40 + 0x1c;
    }
  case 7:
    switch(bVar1) {
    case 9:
switchD_004bcc5c_caseD_9:
      return DAT_005aeb40;
    case 10:
      return DAT_005aeb40 + 0x23c;
    case 0xb:
      return DAT_005aeb40 + 0x238;
    case 0xc:
      return DAT_005aeb40 + 0x240;
    case 0xd:
      return DAT_005aeb40 + 0x24c;
    case 0xe:
      return DAT_005aeb40 + 0x244;
    case 0xf:
      return DAT_005aeb40 + 0x250;
    }
  default:
    return DAT_005aeb10 + 0x30;
  }
}


// ===== FUN_004bcd10 @ 004bcd10 (41 bytes) =====


uint FUN_004bcd10(void)

{
  uint in_EAX;
  byte in_DL;
  
  if ((in_EAX & 1 << (in_DL - 1 & 0x1f)) != 0) {
    return in_EAX | ~((1 << (in_DL & 0x1f)) - 1U);
  }
  return in_EAX & (1 << (in_DL & 0x1f)) - 1U;
}


// ===== FUN_004bcd40 @ 004bcd40 (6 bytes) =====


void FUN_004bcd40(void)

{
                    /* WARNING: Could not recover jumptable at 0x004bcd40. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_005aeb48)();
  return;
}


// ===== FUN_004bcd50 @ 004bcd50 (6 bytes) =====


void FUN_004bcd50(void)

{
                    /* WARNING: Could not recover jumptable at 0x004bcd50. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*DAT_005aeb3c)();
  return;
}


// ===== FUN_004bcd60 @ 004bcd60 (39 bytes) =====


void FUN_004bcd60(void)

{
  int iVar1;
  
  iVar1 = DAT_005aeb40;
  *(int *)(DAT_005aeb40 + 0xd4) = *(int *)(DAT_005aeb40 + 0xd4) + -1;
  *(undefined4 *)(iVar1 + 0x18) = *(undefined4 *)(iVar1 + 0x9c + *(int *)(iVar1 + 0xd4) * 4);
  *(undefined4 *)(iVar1 + 0x14) = *(undefined4 *)(iVar1 + 0xb8 + *(int *)(iVar1 + 0xd4) * 4);
  return;
}


// ===== FUN_004bcd90 @ 004bcd90 (46 bytes) =====


void FUN_004bcd90(void)

{
  int iVar1;
  undefined4 in_ECX;
  undefined4 in_EDX;
  
  iVar1 = DAT_005aeb40;
  *(int *)(DAT_005aeb40 + 0xd4) = *(int *)(DAT_005aeb40 + 0xd4) + 1;
  *(undefined4 *)(iVar1 + 0x18) = in_ECX;
  *(undefined4 *)(iVar1 + 0x14) = in_EDX;
  *(undefined4 *)(iVar1 + 0x9c + *(int *)(iVar1 + 0xd4) * 4) = in_ECX;
  *(undefined4 *)(iVar1 + 0xb8 + *(int *)(iVar1 + 0xd4) * 4) = in_EDX;
  return;
}


// ===== FUN_004bcdc0 @ 004bcdc0 (13 bytes) =====


undefined4 FUN_004bcdc0(void)

{
  return *(undefined4 *)(DAT_005aeb40 + 0x20 + *(int *)(DAT_005aeb40 + 0x10) * 4);
}


// ===== FUN_004bcdd0 @ 004bcdd0 (22 bytes) =====


undefined4 FUN_004bcdd0(void)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar3 = DAT_005aeb40;
  piVar1 = (int *)(DAT_005aeb40 + 0x10);
  uVar2 = *(undefined4 *)(DAT_005aeb40 + 0x20 + *piVar1 * 4);
  *(undefined4 *)(DAT_005aeb40 + 0xc) = uVar2;
  *(int *)(iVar3 + 0x10) = *piVar1 + -1;
  return uVar2;
}


// ===== FUN_004bcdf0 @ 004bcdf0 (19 bytes) =====


void FUN_004bcdf0(void)

{
  int iVar1;
  undefined4 in_ECX;
  
  iVar1 = DAT_005aeb40;
  *(int *)(DAT_005aeb40 + 0x10) = *(int *)(DAT_005aeb40 + 0x10) + 1;
  *(undefined4 *)(iVar1 + 0xc) = in_ECX;
  *(undefined4 *)(iVar1 + 0x20 + *(int *)(iVar1 + 0x10) * 4) = in_ECX;
  return;
}


// ===== FUN_004bce10 @ 004bce10 (70 bytes) =====


void FUN_004bce10(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 auStack_20 [8];
  
  iVar1 = DAT_005aeb40;
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0xd8);
  puVar4 = auStack_20;
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0x264);
  puVar4 = (undefined4 *)(DAT_005aeb40 + 0xd8);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = auStack_20;
  puVar4 = (undefined4 *)(iVar1 + 0x264);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  return;
}


// ===== FUN_004bce60 @ 004bce60 (70 bytes) =====


void FUN_004bce60(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 auStack_20 [8];
  
  iVar1 = DAT_005aeb40;
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0xf8);
  puVar4 = auStack_20;
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0x284);
  puVar4 = (undefined4 *)(DAT_005aeb40 + 0xf8);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = auStack_20;
  puVar4 = (undefined4 *)(iVar1 + 0x284);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  return;
}


// ===== FUN_004bceb0 @ 004bceb0 (70 bytes) =====


void FUN_004bceb0(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 auStack_20 [8];
  
  iVar1 = DAT_005aeb40;
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0xd8);
  puVar4 = auStack_20;
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0x264);
  puVar4 = (undefined4 *)(DAT_005aeb40 + 0xd8);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = auStack_20;
  puVar4 = (undefined4 *)(iVar1 + 0x264);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  return;
}


// ===== FUN_004bcf00 @ 004bcf00 (70 bytes) =====


void FUN_004bcf00(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 auStack_20 [8];
  
  iVar1 = DAT_005aeb40;
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0xf8);
  puVar4 = auStack_20;
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = (undefined4 *)(DAT_005aeb40 + 0x284);
  puVar4 = (undefined4 *)(DAT_005aeb40 + 0xf8);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  puVar3 = auStack_20;
  puVar4 = (undefined4 *)(iVar1 + 0x284);
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  return;
}


// ===== FUN_004bcf50 @ 004bcf50 (29 bytes) =====


void FUN_004bcf50(void)

{
  undefined4 in_ECX;
  
  (&DAT_005aeb14)[DAT_010ed944] = in_ECX;
  DAT_010ed944 = DAT_010ed944 + 1;
  DAT_005aeb44 = 1;
  return;
}


// ===== FUN_004bcf70 @ 004bcf70 (69 bytes) =====


void FUN_004bcf70(void)

{
  uint uVar1;
  
  if ((DAT_005aeb44 == 0) && (DAT_010ed944 != 0)) {
    uVar1 = 0;
    if (DAT_010ed944 != 0) {
      do {
        (*(code *)(&DAT_005aeb14)[uVar1])();
        uVar1 = uVar1 + 1;
      } while (uVar1 < DAT_010ed944);
    }
    DAT_010ed944 = 0;
    return;
  }
  DAT_005aeb44 = DAT_005aeb44 + -1;
  return;
}


// ===== FUN_004bcfc0 @ 004bcfc0 (19 bytes) =====


void FUN_004bcfc0(void)

{
  uint in_EAX;
  
  DAT_010ede70 = DAT_010ede70 & ~in_EAX;
  DAT_005aeb40 = &DAT_010edc30;
  return;
}


// ===== FUN_004bcfe0 @ 004bcfe0 (17 bytes) =====


void FUN_004bcfe0(void)

{
  uint in_EAX;
  
  DAT_010ede70 = DAT_010ede70 | in_EAX;
  DAT_005aeb40 = &DAT_010edc30;
  return;
}


