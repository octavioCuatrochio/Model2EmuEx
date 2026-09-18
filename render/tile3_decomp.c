// ===== FUN_00452d70 @ 00452d70 (32 bytes) =====


void FUN_00452d70(void)

{
  int in_EAX;
  undefined4 in_EDX;
  
  *(undefined4 *)(in_EAX + *(int *)(in_EAX + 0x84) * 4) = in_EDX;
  *(int *)(in_EAX + 0x88) = *(int *)(in_EAX + 0x88) + -1;
  *(uint *)(in_EAX + 0x84) = *(int *)(in_EAX + 0x84) + 1U & 0x1f;
  return;
}


// ===== FUN_00452d90 @ 00452d90 (545 bytes) =====


void FUN_00452d90(void)

{
  uint uVar1;
  ushort uVar2;
  int in_EAX;
  ushort *puVar3;
  ushort in_CX;
  int in_EDX;
  uint *puVar4;
  int iVar5;
  
  puVar4 = (uint *)(in_EDX + 8);
  puVar3 = (ushort *)(in_EAX + 4);
  iVar5 = 2;
  do {
    uVar1 = puVar4[-2];
    puVar3[1] = (ushort)uVar1 & 0xf | in_CX;
    *puVar3 = (ushort)(uVar1 >> 4) & 0xf | in_CX;
    puVar3[-1] = (ushort)(uVar1 >> 8) & 0xf | in_CX;
    puVar3[-2] = (ushort)(uVar1 >> 0xc) & 0xf | in_CX;
    uVar2 = (ushort)(uVar1 >> 0x10);
    puVar3[5] = uVar2 & 0xf | in_CX;
    puVar3[4] = uVar2 >> 4 & 0xf | in_CX;
    puVar3[3] = (byte)(uVar1 >> 0x18) & 0xf | in_CX;
    puVar3[2] = uVar2 >> 0xc | in_CX;
    uVar1 = puVar4[-1];
    puVar3[0x201] = (ushort)uVar1 & 0xf | in_CX;
    puVar3[0x200] = (ushort)(uVar1 >> 4) & 0xf | in_CX;
    puVar3[0x1ff] = (ushort)(uVar1 >> 8) & 0xf | in_CX;
    puVar3[0x1fe] = (ushort)(uVar1 >> 0xc) & 0xf | in_CX;
    uVar2 = (ushort)(uVar1 >> 0x10);
    puVar3[0x205] = uVar2 & 0xf | in_CX;
    puVar3[0x204] = uVar2 >> 4 & 0xf | in_CX;
    puVar3[0x203] = (byte)(uVar1 >> 0x18) & 0xf | in_CX;
    puVar3[0x202] = uVar2 >> 0xc | in_CX;
    uVar1 = *puVar4;
    puVar3[0x401] = (ushort)uVar1 & 0xf | in_CX;
    puVar3[0x400] = (ushort)(uVar1 >> 4) & 0xf | in_CX;
    puVar3[0x3ff] = (ushort)(uVar1 >> 8) & 0xf | in_CX;
    puVar3[0x3fe] = (ushort)(uVar1 >> 0xc) & 0xf | in_CX;
    uVar2 = (ushort)(uVar1 >> 0x10);
    puVar3[0x405] = uVar2 & 0xf | in_CX;
    puVar3[0x404] = uVar2 >> 4 & 0xf | in_CX;
    puVar3[0x403] = (byte)(uVar1 >> 0x18) & 0xf | in_CX;
    puVar3[0x402] = uVar2 >> 0xc | in_CX;
    uVar1 = puVar4[1];
    puVar3[0x601] = (ushort)uVar1 & 0xf | in_CX;
    puVar3[0x600] = (ushort)(uVar1 >> 4) & 0xf | in_CX;
    puVar3[0x5ff] = (ushort)(uVar1 >> 8) & 0xf | in_CX;
    puVar3[0x5fe] = (ushort)(uVar1 >> 0xc) & 0xf | in_CX;
    uVar2 = (ushort)(uVar1 >> 0x10);
    puVar3[0x605] = uVar2 & 0xf | in_CX;
    puVar3[0x604] = uVar2 >> 4 & 0xf | in_CX;
    puVar3[0x603] = (byte)(uVar1 >> 0x18) & 0xf | in_CX;
    puVar3[0x602] = uVar2 >> 0xc | in_CX;
    puVar3 = puVar3 + 0x800;
    puVar4 = puVar4 + 4;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  return;
}


// ===== FUN_00452fc0 @ 00452fc0 (322 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 FUN_00452fc0(float param_1)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  float10 fVar4;
  float10 fVar5;
  
  fVar4 = (float10)param_1;
  if (DAT_010ed974 == 0) {
    return (float10)(DAT_00dcec0c / 2) * (float10)7.450581e-09 + (float10)1 / (fVar4 + (float10)1.0)
    ;
  }
  if (DAT_010ed974 == 1) {
    return (float10)(((int)(DAT_00dcec10 + (DAT_00dcec10 >> 0x1f & 7U)) >> 3) + _DAT_00daeba8) *
           (float10)5.9604645e-08 + ((float10)1 - fVar4 * (float10)3.0517578e-05);
  }
  if (DAT_010ed974 == 2) {
    return (float10)1 / (fVar4 + (float10)1.0);
  }
  if (DAT_010ed974 == 3) {
    return (float10)(((int)(DAT_00dcec10 + (DAT_00dcec10 >> 0x1f & 0xfU)) >> 4) + _DAT_00daeba8) *
           (float10)5.9604645e-08 + ((float10)1 - fVar4 * (float10)3.0517578e-05);
  }
  if (DAT_010ed974 == 4) {
    fVar5 = (float10)1;
    fVar4 = (float10)1 / (fVar4 + fVar5);
    uVar3 = (uint)(float)fVar4 >> 0xf;
    uVar1 = uVar3 + DAT_010ed988 * -0x100 & 0xf00 | uVar3 & 0xff;
    uVar2 = (&DAT_00dcf000)[uVar1];
    uVar3 = uVar2 + 1;
    (&DAT_00dcf000)[uVar1] = uVar3;
    if (0xff < uVar3) {
      (&DAT_00dcf000)[uVar1] = 0xff;
    }
    uVar2 = uVar1 << 8 | uVar2;
    if (fVar4 < (float10)0) {
      fVar5 = (float10)-1.0;
    }
    fVar4 = (float10)(int)uVar2;
    if ((int)uVar2 < 0) {
      fVar4 = fVar4 + (float10)4.2949673e+09;
    }
    fVar4 = fVar4 * (float10)9.536743e-07 * fVar5;
  }
  return fVar4;
}


// ===== FUN_00453110 @ 00453110 (61 bytes) =====


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00453110(void)

{
  int *piVar1;
  int *piVar2;
  
  piVar2 = (int *)*DAT_010f00e8;
  *DAT_010f00e8 = (int)DAT_010f00e8;
  DAT_010f00e8[1] = (int)DAT_010f00e8;
  _DAT_010f00ec = 0;
  if (piVar2 != DAT_010f00e8) {
    do {
      piVar1 = (int *)*piVar2;
      FUN_0048d1c3(piVar2);
      piVar2 = piVar1;
    } while (piVar1 != DAT_010f00e8);
  }
  return;
}


// ===== FUN_00453150 @ 00453150 (116 bytes) =====


void FUN_00453150(char *param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 *unaff_ESI;
  undefined **local_c [3];
  
  puVar1 = (undefined4 *)FUN_0048d1ce(0x1c);
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = param_1;
    puVar1[1] = param_2;
    if (puVar1 + 2 != (undefined4 *)0x0) {
      puVar1[2] = *unaff_ESI;
      puVar1[3] = unaff_ESI[1];
      puVar1[4] = unaff_ESI[2];
      puVar1[5] = unaff_ESI[3];
      puVar1[6] = unaff_ESI[4];
    }
    return;
  }
  param_1 = (char *)0x0;
  std::exception::exception((exception *)local_c,&param_1);
  local_c[0] = std::bad_alloc::vftable;
                    /* WARNING: Subroutine does not return */
  __CxxThrowException_8(local_c,&DAT_005250a0);
}


// ===== FUN_004531d0 @ 004531d0 (10 bytes) =====


void FUN_004531d0(void)

{
  FUN_00453370();
  return;
}


// ===== FUN_004531e0 @ 004531e0 (37 bytes) =====


int FUN_004531e0(void)

{
  uint uVar1;
  uint in_EDX;
  
  uVar1 = (in_EDX >> 6 & 0x1f) * 0x20;
  if ((in_EDX & 0x20) != 0) {
    uVar1 = uVar1 + 0x400;
  }
  return (uVar1 >> 1) * 0x400 + (in_EDX & 0x1f) * 0x20;
}


// ===== FUN_00453210 @ 00453210 (176 bytes) =====


void FUN_00453210(int param_1,int param_2)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int local_4;
  
  iVar6 = 0;
  if (0 < param_2) {
    do {
      if ((*(uint *)(param_1 + iVar6 * 4) & 0xff000000) == 0) {
        puVar4 = (uint *)(param_1 + -4 + iVar6 * 4);
        iVar7 = 1;
        puVar1 = puVar4 + 2;
        iVar3 = iVar6;
        local_4 = iVar6;
        while( true ) {
          local_4 = local_4 + 1;
          iVar3 = iVar3 + -1;
          iVar5 = 2;
          if (-1 < iVar3) {
            if ((*puVar4 & 0xff000000) != 0) {
              uVar2 = *(uint *)(param_1 + (iVar6 - iVar7) * 4) & 0xffffff;
              goto LAB_004532a9;
            }
            iVar5 = 1;
          }
          if (local_4 < param_2) {
            if ((*puVar1 & 0xff000000) != 0) {
              uVar2 = *(uint *)(param_1 + (iVar7 + iVar6) * 4) & 0xffffff;
              goto LAB_004532a9;
            }
            iVar5 = iVar5 + -1;
          }
          if (iVar5 == 2) break;
          iVar7 = iVar7 + 1;
          puVar1 = puVar1 + 1;
          puVar4 = puVar4 + -1;
        }
        uVar2 = *(uint *)(param_1 + iVar6 * 4);
LAB_004532a9:
        *(uint *)(param_1 + iVar6 * 4) = uVar2;
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < param_2);
  }
  return;
}


// ===== FUN_004532e0 @ 004532e0 (132 bytes) =====


int FUN_004532e0(void)

{
  int iVar1;
  int *in_EAX;
  int iVar2;
  int unaff_EDI;
  int local_c [3];
  
  FUN_004537f0(local_c);
  if (local_c[0] == *(int *)(unaff_EDI + 4)) {
    local_c[0] = *in_EAX;
    iVar1 = **(int **)(unaff_EDI + 4);
    local_c[1] = 0;
    iVar2 = FUN_00454750(iVar1,*(undefined4 *)(iVar1 + 4));
    if (*(int *)(unaff_EDI + 8) == 0x1ffffffe) {
                    /* WARNING: Subroutine does not return */
      FUN_004a68db("list<T> too long");
    }
    *(int *)(unaff_EDI + 8) = *(int *)(unaff_EDI + 8) + 1;
    *(int *)(iVar1 + 4) = iVar2;
    **(int **)(iVar2 + 4) = iVar2;
    FUN_00454180(unaff_EDI,local_c,**(int **)(unaff_EDI + 4) + 8,**(int **)(unaff_EDI + 4));
  }
  return local_c[0] + 0xc;
}


// ===== FUN_00453370 @ 00453370 (81 bytes) =====


void FUN_00453370(void)

{
  int *piVar1;
  int *piVar2;
  int unaff_ESI;
  
  if (*(int *)(unaff_ESI + 0x10) != 0) {
    FUN_0048d1c3(*(int *)(unaff_ESI + 0x10));
  }
  *(undefined4 *)(unaff_ESI + 0x10) = 0;
  *(undefined4 *)(unaff_ESI + 0x14) = 0;
  *(undefined4 *)(unaff_ESI + 0x18) = 0;
  piVar1 = *(int **)(unaff_ESI + 4);
  piVar2 = (int *)*piVar1;
  *piVar1 = (int)piVar1;
  *(int *)(*(int *)(unaff_ESI + 4) + 4) = *(int *)(unaff_ESI + 4);
  *(undefined4 *)(unaff_ESI + 8) = 0;
  if (piVar2 != *(int **)(unaff_ESI + 4)) {
    do {
      piVar1 = (int *)*piVar2;
      FUN_0048d1c3(piVar2);
      piVar2 = piVar1;
    } while (piVar1 != (int *)*(int *)(unaff_ESI + 4));
  }
  FUN_0048d1c3(*(undefined4 *)(unaff_ESI + 4));
  return;
}


// ===== FUN_004533d0 @ 004533d0 (130 bytes) =====


/* WARNING: Removing unreachable block (ram,0x0045341c) */
/* WARNING: Removing unreachable block (ram,0x00453420) */
/* WARNING: Removing unreachable block (ram,0x0045342e) */

void FUN_004533d0(void)

{
  int *piVar1;
  int *piVar2;
  int unaff_ESI;
  undefined4 local_4;
  
  piVar1 = *(int **)(unaff_ESI + 4);
  piVar2 = (int *)*piVar1;
  *piVar1 = (int)piVar1;
  *(int *)(*(int *)(unaff_ESI + 4) + 4) = *(int *)(unaff_ESI + 4);
  *(undefined4 *)(unaff_ESI + 8) = 0;
  if (piVar2 != *(int **)(unaff_ESI + 4)) {
    do {
      piVar1 = (int *)*piVar2;
      FUN_0048d1c3(piVar2);
      piVar2 = piVar1;
    } while (piVar1 != (int *)*(int *)(unaff_ESI + 4));
  }
  local_4 = *(undefined4 *)(unaff_ESI + 4);
  if (*(int *)(unaff_ESI + 0x10) != *(int *)(unaff_ESI + 0x14)) {
    *(int *)(unaff_ESI + 0x14) = *(int *)(unaff_ESI + 0x10);
  }
  FUN_00453e10(*(undefined4 *)(unaff_ESI + 0x10),&local_4);
  *(undefined4 *)(unaff_ESI + 0x20) = 7;
  *(undefined4 *)(unaff_ESI + 0x24) = 8;
  return;
}


