// ===== FUN_004c0230 @ 004c0230 (257 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004c0230(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *unaff_EBX;
  
  iVar2 = DAT_005aa87c;
  if (unaff_EBX != (int *)0x0) {
    iVar1 = *unaff_EBX;
    iVar3 = FUN_00401050(iVar1,*(undefined4 *)(iVar1 + 4));
    if (unaff_EBX[1] == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
      FUN_004a68db("list<T> too long");
    }
    unaff_EBX[1] = unaff_EBX[1] + 1;
    *(int *)(iVar1 + 4) = iVar3;
    **(int **)(iVar3 + 4) = iVar3;
  }
  _DAT_005aeb34 = iVar2 + 0x18;
  _DAT_005aeb08 = iVar2;
  _memset(&DAT_010edc30,0,0x2a4);
  *(undefined4 *)(&DAT_010edc50 + DAT_010edc40 * 4) = 0xffffffff;
  *(undefined4 *)(&DAT_010edce8 + DAT_010edd04 * 4) = 0xffffffff;
  DAT_010edc38 = 0x20018;
  FUN_004bf610(iVar2,param_1);
  DAT_010ed950 = 0;
  DAT_010ed944 = 0;
  DAT_005aeb44 = 0;
  DAT_010edc38 = 0x20004;
  DAT_010eded8 = &DAT_00d801b0 + DAT_010ededc;
  return;
}


// ===== FUN_004c0380 @ 004c0380 (117 bytes) =====

void __fastcall FUN_004c0380(uint param_1)

{
  int unaff_ESI;
  
  switch(param_1 & 3) {
  case 0:
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
  }
  FUN_00452d90();
  *(undefined1 *)(unaff_ESI + 0x24) = 1;
  return;
}


// ===== FUN_004c0410 @ 004c0410 (286 bytes) =====

void FUN_004c0410(void)

{
  int unaff_ESI;
  undefined4 local_8;
  undefined4 local_4;
  
  if (*(char *)(unaff_ESI + 0x10) != '\0') {
    local_8 = 0;
    do {
      local_4 = 0x40;
      do {
        FUN_00452d90();
        *(undefined1 *)(unaff_ESI + 0x24) = 1;
        FUN_00452d90();
        *(undefined1 *)(unaff_ESI + 0x24) = 1;
        FUN_00452d90();
        *(undefined1 *)(unaff_ESI + 0x24) = 1;
        FUN_00452d90();
        local_4 = local_4 + -1;
        *(undefined1 *)(unaff_ESI + 0x24) = 1;
      } while (local_4 != 0);
      local_8 = local_8 + 0x2000;
    } while (local_8 < 0x80000);
  }
  *(undefined1 *)(unaff_ESI + 0x12) = 0;
  *(undefined1 *)(unaff_ESI + 0x10) = 0;
  return;
}


// ===== FUN_004c06a0 @ 004c06a0 (29 bytes) =====

undefined4 __fastcall FUN_004c06a0(uint param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  
  piVar2 = (int *)((param_1 >> 0x10) * 0x10 + g_i960_ctx);
  iVar1 = *piVar2;
  if (iVar1 != 0) {
    return *(undefined4 *)(iVar1 + param_1);
  }
                    /* WARNING: Could not recover jumptable at 0x004c06bb. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  uVar3 = (*(code *)piVar2[3])();
  return uVar3;
}


// ===== FUN_004c0730 @ 004c0730 (74 bytes) =====

void FUN_004c0730(int param_1,uint param_2,int param_3)

{
  int in_EAX;
  undefined4 *puVar1;
  int iVar2;
  undefined4 unaff_EBX;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  
  iVar2 = (in_EAX - param_2 >> 0x10) + 1;
  if (iVar2 != 0) {
    puVar1 = (undefined4 *)(param_1 + 8 + (param_2 >> 0x10) * 0x10);
    do {
      if (param_3 == 0) {
        puVar1[-2] = 0;
      }
      else {
        puVar1[-2] = param_3 - param_2;
      }
      puVar1[-1] = unaff_EBX;
      *puVar1 = unaff_EDI;
      puVar1[1] = unaff_ESI;
      puVar1 = puVar1 + 4;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  return;
}


// ===== i960_op588_nor @ 004c07d0 (27 bytes) =====

void i960_op588_nor(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~**(uint **)(g_i960_cur_insn + 0xc) & ~**(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op58e_nand @ 004c07f0 (27 bytes) =====

void i960_op58e_nand(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~**(uint **)(g_i960_cur_insn + 0xc) | ~**(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op670_emul @ 004c0810 (26 bytes) =====

void i960_op670_emul(void)

{
  **(longlong **)(g_i960_cur_insn + 0x10) =
       (ulonglong)**(uint **)(g_i960_cur_insn + 0xc) * (ulonglong)**(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op023_testge @ 004c0830 (32 bytes) =====

void i960_op023_testge(void)

{
  **(uint **)(g_i960_cur_insn + 8) = (uint)((*(byte *)(g_i960_pc_ptr + 0xa8) & 3) != 0);
  return;
}


// ===== i960_op6e1_movre @ 004c0850 (30 bytes) =====

void i960_op6e1_movre(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 8);
  puVar2 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  *puVar2 = *puVar1;
  puVar2[1] = puVar1[1];
  puVar2[2] = (uint)*(ushort *)(puVar1 + 2);
  return;
}


// ===== FUN_004c0870 @ 004c0870 (55 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_004c0870(double param_1)

{
  int iVar1;
  
  iVar1 = (int)ROUND(ABS(param_1 + param_1) + (double)_DAT_0051cf44) >> 1;
  if (param_1 < 0.0) {
    iVar1 = -iVar1;
  }
  return iVar1;
}


// ===== i960_op640_spanbit @ 004c08b0 (87 bytes) =====

void i960_op640_spanbit(void)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  
  piVar1 = *(int **)(g_i960_cur_insn + 0x10);
  uVar3 = ~**(uint **)(g_i960_cur_insn + 8);
  if (uVar3 == 0) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 0;
    *piVar1 = -1;
    return;
  }
  iVar2 = 0x1f;
  if (uVar3 != 0) {
    for (; uVar3 >> iVar2 == 0; iVar2 = iVar2 + -1) {
    }
  }
  *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
  *piVar1 = iVar2;
  return;
}


// ===== i960_op641_scanbit @ 004c0910 (86 bytes) =====

void i960_op641_scanbit(void)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  piVar2 = *(int **)(g_i960_cur_insn + 0x10);
  if (uVar1 == 0) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 0;
    *piVar2 = -1;
    return;
  }
  iVar3 = 0x1f;
  if (uVar1 != 0) {
    for (; uVar1 >> iVar3 == 0; iVar3 = iVar3 + -1) {
    }
  }
  *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
  *piVar2 = iVar3;
  return;
}


// ===== i960_op59d_rotate @ 004c0970 (27 bytes) =====

void i960_op59d_rotate(void)

{
  byte bVar1;
  
  bVar1 = (byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f;
  **(uint **)(g_i960_cur_insn + 0x10) =
       **(uint **)(g_i960_cur_insn + 0xc) << bVar1 |
       **(uint **)(g_i960_cur_insn + 0xc) >> 0x20 - bVar1;
  return;
}


// ===== i960_op59a_shrdi @ 004c0990 (53 bytes) =====

void i960_op59a_shrdi(void)

{
  uint uVar1;
  byte bVar2;
  int iVar3;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 0xc);
  iVar3 = 0;
  bVar2 = (byte)**(undefined4 **)(g_i960_cur_insn + 8);
  if (((int)uVar1 < 0) && ((uVar1 & (1 << (bVar2 & 0x1f)) - 1U) != 0)) {
    iVar3 = 1;
  }
  **(int **)(g_i960_cur_insn + 0x10) = ((int)uVar1 >> (bVar2 & 0x1f)) + iVar3;
  return;
}


// ===== i960_op598_shro @ 004c09d0 (25 bytes) =====

void i960_op598_shro(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       **(uint **)(g_i960_cur_insn + 0xc) >> ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f);
  return;
}


// ===== i960_op59b_shri @ 004c09f0 (25 bytes) =====

void i960_op59b_shri(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) >> ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f);
  return;
}


// ===== i960_op59c_shlo @ 004c0a10 (25 bytes) =====

void i960_op59c_shlo(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f);
  return;
}


// ===== i960_op59e_shli @ 004c0a30 (25 bytes) =====

void i960_op59e_shli(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f);
  return;
}


// ===== i960_op030_bbc @ 004c0a50 (60 bytes) =====

void i960_op030_bbc(void)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = g_i960_cur_insn;
  piVar1 = g_i960_pc_ptr;
  if ((**(uint **)(g_i960_cur_insn + 0xc) &
      1 << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f)) == 0) {
    g_i960_pc_ptr[0x2a] = 2;
    *piVar1 = *piVar1 + *(int *)(iVar2 + 4);
    return;
  }
  g_i960_pc_ptr[0x2a] = 0;
  return;
}


// ===== i960_op037_bbs @ 004c0a90 (60 bytes) =====

void i960_op037_bbs(void)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = g_i960_cur_insn;
  piVar1 = g_i960_pc_ptr;
  if ((**(uint **)(g_i960_cur_insn + 0xc) &
      1 << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f)) != 0) {
    g_i960_pc_ptr[0x2a] = 2;
    *piVar1 = *piVar1 + *(int *)(iVar2 + 4);
    return;
  }
  g_i960_pc_ptr[0x2a] = 0;
  return;
}


// ===== i960_op03e_cmpible @ 004c0ad0 (102 bytes) =====

void i960_op03e_cmpible(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  if (iVar1 < iVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (iVar1 == iVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (iVar2 < iVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 6) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op03c_cmpibl @ 004c0b40 (102 bytes) =====

void i960_op03c_cmpibl(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  if (iVar1 < iVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (iVar1 == iVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (iVar2 < iVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 4) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op03d_cmpibne @ 004c0bb0 (102 bytes) =====

void i960_op03d_cmpibne(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  if (iVar1 < iVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (iVar1 == iVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (iVar2 < iVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 5) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op03a_cmpibe @ 004c0c20 (102 bytes) =====

void i960_op03a_cmpibe(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  if (iVar1 < iVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (iVar1 == iVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (iVar2 < iVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 2) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op03b_cmpibge @ 004c0c90 (102 bytes) =====

void i960_op03b_cmpibge(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  if (iVar1 < iVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (iVar1 == iVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (iVar2 < iVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 3) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op039_cmpibg @ 004c0d00 (102 bytes) =====

void i960_op039_cmpibg(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  if (iVar1 < iVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (iVar1 == iVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (iVar2 < iVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 1) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op031_cmpobg @ 004c0d70 (102 bytes) =====

void i960_op031_cmpobg(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  if (uVar1 < uVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (uVar1 == uVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (uVar2 < uVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 1) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op036_cmpoble @ 004c0de0 (102 bytes) =====

void i960_op036_cmpoble(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  if (uVar1 < uVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (uVar1 == uVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (uVar2 < uVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 6) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op034_cmpobl @ 004c0e50 (102 bytes) =====

void i960_op034_cmpobl(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  if (uVar1 < uVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (uVar1 == uVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (uVar2 < uVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 4) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op033_cmpobge @ 004c0ec0 (102 bytes) =====

void i960_op033_cmpobge(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  if (uVar1 < uVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (uVar1 == uVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (uVar2 < uVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 3) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op035_cmpobne @ 004c0f30 (102 bytes) =====

void i960_op035_cmpobne(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  if (uVar1 < uVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (uVar1 == uVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (uVar2 < uVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 5) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op032_cmpobe @ 004c0fa0 (102 bytes) =====

void i960_op032_cmpobe(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = g_i960_cur_insn;
  piVar3 = g_i960_pc_ptr;
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  if (uVar1 < uVar2) {
    g_i960_pc_ptr[0x2a] = 4;
  }
  else if (uVar1 == uVar2) {
    g_i960_pc_ptr[0x2a] = 2;
  }
  else if (uVar2 < uVar1) {
    g_i960_pc_ptr[0x2a] = 1;
  }
  else {
    g_i960_pc_ptr[0x2a] = 0;
  }
  if ((*(byte *)(piVar3 + 0x2a) & 2) != 0) {
    *piVar3 = *piVar3 + *(int *)(iVar4 + 4);
  }
  return;
}


// ===== i960_op5a7_cmpdeci @ 004c1010 (92 bytes) =====

void i960_op5a7_cmpdeci(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  piVar3 = *(int **)(g_i960_cur_insn + 0x10);
  if (iVar1 < iVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    *piVar3 = iVar2 + -1;
    return;
  }
  if (iVar1 == iVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    *piVar3 = iVar2 + -1;
    return;
  }
  *(uint *)(g_i960_pc_ptr + 0xa8) = (uint)(iVar2 < iVar1);
  *piVar3 = iVar2 + -1;
  return;
}


// ===== i960_op5a5_cmpinci @ 004c1070 (92 bytes) =====

void i960_op5a5_cmpinci(void)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  piVar3 = *(int **)(g_i960_cur_insn + 0x10);
  if (iVar1 < iVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    *piVar3 = iVar2 + 1;
    return;
  }
  if (iVar1 == iVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    *piVar3 = iVar2 + 1;
    return;
  }
  *(uint *)(g_i960_pc_ptr + 0xa8) = (uint)(iVar2 < iVar1);
  *piVar3 = iVar2 + 1;
  return;
}


// ===== i960_op5a6_cmpdeco @ 004c10d0 (89 bytes) =====

void i960_op5a6_cmpdeco(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  piVar3 = *(int **)(g_i960_cur_insn + 0x10);
  if (uVar1 < uVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    *piVar3 = uVar2 - 1;
    return;
  }
  if (uVar1 == uVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    *piVar3 = uVar2 - 1;
    return;
  }
  *(uint *)(g_i960_pc_ptr + 0xa8) = (uint)(uVar2 < uVar1);
  *piVar3 = uVar2 - 1;
  return;
}


// ===== i960_op5a4_cmpinco @ 004c1130 (89 bytes) =====

void i960_op5a4_cmpinco(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  piVar3 = *(int **)(g_i960_cur_insn + 0x10);
  if (uVar1 < uVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    *piVar3 = uVar2 + 1;
    return;
  }
  if (uVar1 == uVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    *piVar3 = uVar2 + 1;
    return;
  }
  *(uint *)(g_i960_pc_ptr + 0xa8) = (uint)(uVar2 < uVar1);
  *piVar3 = uVar2 + 1;
  return;
}


// ===== i960_op5a2_concmpo @ 004c1190 (42 bytes) =====

void i960_op5a2_concmpo(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0xa8) & 4) == 0) {
    *(uint *)(g_i960_pc_ptr + 0xa8) =
         2 - (uint)(**(uint **)(g_i960_cur_insn + 0xc) < **(uint **)(g_i960_cur_insn + 8));
  }
  return;
}


// ===== i960_op5a3_concmpi @ 004c11c0 (45 bytes) =====

void i960_op5a3_concmpi(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0xa8) & 4) == 0) {
    *(uint *)(g_i960_pc_ptr + 0xa8) =
         (**(int **)(g_i960_cur_insn + 8) <= **(int **)(g_i960_cur_insn + 0xc)) + 1;
  }
  return;
}


// ===== i960_op5a1_cmpi @ 004c11f0 (74 bytes) =====

void i960_op5a1_cmpi(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  iVar2 = **(int **)(g_i960_cur_insn + 0xc);
  if (iVar1 < iVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    return;
  }
  if (iVar1 == iVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    return;
  }
  *(uint *)(g_i960_pc_ptr + 0xa8) = (uint)(iVar2 < iVar1);
  return;
}


// ===== i960_op5a0_cmpo @ 004c1240 (74 bytes) =====

void i960_op5a0_cmpo(void)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  if (uVar1 < uVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    return;
  }
  if (uVar1 == uVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    return;
  }
  *(uint *)(g_i960_pc_ptr + 0xa8) = (uint)(uVar2 < uVar1);
  return;
}


// ===== i960_op085_balx @ 004c1290 (34 bytes) =====

void i960_op085_balx(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  puVar1 = g_i960_pc_ptr;
  **(undefined4 **)(g_i960_cur_insn + 0x10) = *g_i960_pc_ptr;
  *puVar1 = uVar2;
  return;
}


// ===== i960_op00b_bal @ 004c12c0 (24 bytes) =====

void i960_op00b_bal(void)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  iVar3 = g_i960_cur_insn;
  piVar2 = g_i960_pc_ptr;
  iVar1 = *g_i960_pc_ptr;
  g_i960_pc_ptr[0x1f] = iVar1;
  *piVar2 = *(int *)(iVar3 + 4) + iVar1;
  return;
}


// ===== i960_op010_bno @ 004c12e0 (26 bytes) =====

void i960_op010_bno(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 7) == 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op017_bo @ 004c1300 (26 bytes) =====

void i960_op017_bo(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 7) != 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op016_ble @ 004c1320 (26 bytes) =====

void i960_op016_ble(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 6) != 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op013_bge @ 004c1340 (26 bytes) =====

void i960_op013_bge(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 3) != 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op011_bg @ 004c1360 (26 bytes) =====

void i960_op011_bg(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 1) != 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op014_bl @ 004c1380 (26 bytes) =====

void i960_op014_bl(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 4) != 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op015_bne @ 004c13a0 (26 bytes) =====

void i960_op015_bne(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 5) != 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op012_be @ 004c13c0 (26 bytes) =====

void i960_op012_be(void)

{
  if ((*(byte *)(g_i960_pc_ptr + 0x2a) & 2) != 0) {
    *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  }
  return;
}


// ===== i960_op084_bx @ 004c13e0 (19 bytes) =====

void i960_op084_bx(void)

{
  undefined4 uVar1;
  
  uVar1 = (**(code **)(g_i960_cur_insn + 0x14))();
  *g_i960_pc_ptr = uVar1;
  return;
}


// ===== i960_op008_b @ 004c1400 (16 bytes) =====

void i960_op008_b(void)

{
  *g_i960_pc_ptr = *g_i960_pc_ptr + *(int *)(g_i960_cur_insn + 4);
  return;
}


// ===== i960_op66d_flushreg @ 004c1410 (154 bytes) =====

void i960_op66d_flushreg(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  
  iVar5 = *(int *)(g_i960_ctx + 0x20014c);
  iVar4 = g_i960_ctx;
  while (0 < iVar5) {
    piVar1 = (int *)(iVar4 + 0x200148);
    *piVar1 = *piVar1 + -1;
    if (*piVar1 < 0) {
      *(undefined4 *)(iVar4 + 0x200148) = 0x7f;
    }
    *(int *)(iVar4 + 0x20014c) = *(int *)(iVar4 + 0x20014c) + -1;
    uVar6 = *(uint *)(iVar4 + 0x200150 + *(int *)(iVar4 + 0x200148) * 4);
    iVar5 = 0;
    do {
      iVar3 = ((uVar6 >> 0x10) + 0x10000) * 0x10;
      iVar2 = *(int *)(iVar3 + iVar4);
      if (iVar2 == 0) {
        (**(code **)(iVar3 + iVar4 + 0xc))();
        iVar4 = g_i960_ctx;
      }
      else {
        *(undefined4 *)(uVar6 + iVar2) =
             *(undefined4 *)(iVar4 + 0x200350 + (*(int *)(iVar4 + 0x200148) * 0x10 + iVar5) * 4);
      }
      iVar5 = iVar5 + 1;
      uVar6 = uVar6 + 4;
    } while (iVar5 < 0x10);
    iVar5 = *(int *)(iVar4 + 0x20014c);
  }
  return;
}


// ===== i960_op086_callx @ 004c14c0 (279 bytes) =====

void i960_op086_callx(void)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  int iVar7;
  int iVar8;
  undefined4 *puVar9;
  uint uVar10;
  undefined4 *puVar11;
  
  uVar4 = (**(code **)(g_i960_cur_insn + 0x14))();
  iVar5 = g_i960_ctx;
  puVar6 = g_i960_pc_ptr;
  uVar1 = g_i960_pc_ptr[0x20];
  iVar2 = g_i960_pc_ptr[2];
  g_i960_pc_ptr[3] = *g_i960_pc_ptr;
  if (*(int *)(iVar5 + 0x20014c) == 0x80) {
    uVar10 = *(uint *)(iVar5 + 0x200150 + *(int *)(iVar5 + 0x200148) * 4);
    iVar7 = 0;
    do {
      iVar8 = (uVar10 >> 0x10) + 0x10000;
      iVar3 = *(int *)(iVar5 + iVar8 * 0x10);
      if (iVar3 == 0) {
        (**(code **)(iVar5 + 0xc + iVar8 * 0x10))();
        iVar5 = g_i960_ctx;
      }
      else {
        *(undefined4 *)(uVar10 + iVar3) =
             *(undefined4 *)(iVar5 + 0x200350 + (*(int *)(iVar5 + 0x200148) * 0x10 + iVar7) * 4);
      }
      iVar7 = iVar7 + 1;
      uVar10 = uVar10 + 4;
    } while (iVar7 < 0x10);
    *(int *)(iVar5 + 0x20014c) = *(int *)(iVar5 + 0x20014c) + -1;
    puVar6 = g_i960_pc_ptr;
  }
  puVar11 = (undefined4 *)(*(int *)(iVar5 + 0x200148) * 0x40 + 0x200350 + iVar5);
  puVar9 = puVar6;
  for (iVar7 = 0x10; puVar9 = puVar9 + 1, iVar7 != 0; iVar7 = iVar7 + -1) {
    *puVar11 = *puVar9;
    puVar11 = puVar11 + 1;
  }
  *(uint *)(iVar5 + 0x200150 + *(int *)(iVar5 + 0x200148) * 4) = puVar6[0x20] & 0xffffffc0;
  uVar10 = *(int *)(iVar5 + 0x200148) + 1U & 0x8000007f;
  if ((int)uVar10 < 0) {
    uVar10 = (uVar10 - 1 | 0xffffff80) + 1;
  }
  *(int *)(iVar5 + 0x20014c) = *(int *)(iVar5 + 0x20014c) + 1;
  *(uint *)(iVar5 + 0x200148) = uVar10;
  *puVar6 = uVar4;
  uVar10 = iVar2 + 0x3fU & 0xffffffc0;
  puVar6[1] = uVar1 & 0xffffffc0;
  puVar6[0x20] = uVar10;
  puVar6[2] = uVar10 + 0x40;
  return;
}


// ===== i960_op009_call @ 004c15e0 (265 bytes) =====

void i960_op009_call(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  uint uVar9;
  int *piVar10;
  
  iVar4 = g_i960_ctx;
  piVar5 = g_i960_pc_ptr;
  iVar1 = g_i960_pc_ptr[2];
  uVar2 = g_i960_pc_ptr[0x20];
  g_i960_pc_ptr[3] = *g_i960_pc_ptr;
  if (*(int *)(iVar4 + 0x20014c) == 0x80) {
    uVar9 = *(uint *)(iVar4 + 0x200150 + *(int *)(iVar4 + 0x200148) * 4);
    iVar6 = 0;
    do {
      iVar7 = (uVar9 >> 0x10) + 0x10000;
      iVar3 = *(int *)(iVar4 + iVar7 * 0x10);
      if (iVar3 == 0) {
        (**(code **)(iVar4 + 0xc + iVar7 * 0x10))();
        iVar4 = g_i960_ctx;
      }
      else {
        *(undefined4 *)(uVar9 + iVar3) =
             *(undefined4 *)(iVar4 + 0x200350 + (*(int *)(iVar4 + 0x200148) * 0x10 + iVar6) * 4);
      }
      iVar6 = iVar6 + 1;
      uVar9 = uVar9 + 4;
    } while (iVar6 < 0x10);
    *(int *)(iVar4 + 0x20014c) = *(int *)(iVar4 + 0x20014c) + -1;
    piVar5 = g_i960_pc_ptr;
  }
  piVar10 = (int *)(*(int *)(iVar4 + 0x200148) * 0x40 + 0x200350 + iVar4);
  piVar8 = piVar5;
  for (iVar6 = 0x10; piVar8 = piVar8 + 1, iVar6 != 0; iVar6 = iVar6 + -1) {
    *piVar10 = *piVar8;
    piVar10 = piVar10 + 1;
  }
  *(uint *)(iVar4 + 0x200150 + *(int *)(iVar4 + 0x200148) * 4) = piVar5[0x20] & 0xffffffc0;
  uVar9 = *(int *)(iVar4 + 0x200148) + 1U & 0x8000007f;
  if ((int)uVar9 < 0) {
    uVar9 = (uVar9 - 1 | 0xffffff80) + 1;
  }
  *(int *)(iVar4 + 0x20014c) = *(int *)(iVar4 + 0x20014c) + 1;
  *(uint *)(iVar4 + 0x200148) = uVar9;
  *piVar5 = *piVar5 + *(int *)(g_i960_cur_insn + 4);
  uVar9 = iVar1 + 0x3fU & 0xffffffc0;
  piVar5[1] = uVar2 & 0xffffffc0;
  piVar5[0x20] = uVar9;
  piVar5[2] = uVar9 + 0x40;
  return;
}


// ===== i960_op600_synmov @ 004c16f0 (157 bytes) =====

void i960_op600_synmov(void)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined4 uVar5;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  piVar4 = (int *)((**(uint **)(g_i960_cur_insn + 0xc) >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    uVar5 = (*(code *)piVar4[3])();
  }
  else {
    uVar5 = *(undefined4 *)(iVar2 + **(uint **)(g_i960_cur_insn + 0xc));
  }
  iVar2 = g_i960_pc_ptr;
  if (uVar1 == 0xff000004) {
    *(undefined4 *)(g_i960_pc_ptr + 0xbc) = uVar5;
    *(undefined4 *)(iVar2 + 0xa8) = 2;
    return;
  }
  piVar4 = (int *)(((uVar1 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar3 = *piVar4;
  if (iVar3 != 0) {
    *(undefined4 *)(iVar3 + uVar1) = uVar5;
    *(undefined4 *)(iVar2 + 0xa8) = 2;
    return;
  }
  (*(code *)piVar4[3])();
  *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
  return;
}


// ===== i960_op671_ediv @ 004c1790 (58 bytes) =====

void i960_op671_ediv(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 extraout_ECX;
  
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  uVar2 = __aulldvrm(**(undefined4 **)(g_i960_cur_insn + 0xc),
                     (*(undefined4 **)(g_i960_cur_insn + 0xc))[1],
                     **(undefined4 **)(g_i960_cur_insn + 8),0);
  *puVar1 = extraout_ECX;
  puVar1[1] = uVar2;
  return;
}


// ===== i960_op749_modi @ 004c17d0 (42 bytes) =====

int i960_op749_modi(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  piVar3 = *(int **)(g_i960_cur_insn + 0x10);
  iVar4 = (int)uVar2 % (int)uVar1;
  *piVar3 = iVar4;
  if (((int)(uVar2 ^ uVar1) < 0) && (iVar4 != 0)) {
    *piVar3 = iVar4 + uVar1;
  }
  return (int)uVar2 / (int)uVar1;
}


// ===== i960_op708_remo @ 004c1800 (26 bytes) =====

uint i960_op708_remo(void)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 0xc);
  uVar2 = **(uint **)(g_i960_cur_insn + 8);
  **(uint **)(g_i960_cur_insn + 0x10) = uVar1 % uVar2;
  return uVar1 / uVar2;
}


// ===== i960_op748_remi @ 004c1820 (25 bytes) =====

int i960_op748_remi(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = **(int **)(g_i960_cur_insn + 0xc);
  iVar2 = **(int **)(g_i960_cur_insn + 8);
  **(int **)(g_i960_cur_insn + 0x10) = iVar1 % iVar2;
  return iVar1 / iVar2;
}


// ===== i960_op70b_divo @ 004c1840 (37 bytes) =====

void i960_op70b_divo(void)

{
  if (**(uint **)(g_i960_cur_insn + 8) != 0) {
    **(uint **)(g_i960_cur_insn + 0x10) =
         **(uint **)(g_i960_cur_insn + 0xc) / **(uint **)(g_i960_cur_insn + 8);
    return;
  }
  **(uint **)(g_i960_cur_insn + 0x10) = 0;
  return;
}


// ===== i960_op74b_divi @ 004c1870 (25 bytes) =====

void i960_op74b_divi(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) / **(int **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op741_muli @ 004c1890 (22 bytes) =====

void i960_op741_muli(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) * **(int **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op701_mulo @ 004c18b0 (22 bytes) =====

void i960_op701_mulo(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) * **(int **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op655_modpc @ 004c18d0 (41 bytes) =====

void i960_op655_modpc(void)

{
  *(uint *)(g_i960_pc_ptr + 0xb0) =
       **(uint **)(g_i960_cur_insn + 0x10) & **(uint **)(g_i960_cur_insn + 0xc) |
       ~**(uint **)(g_i960_cur_insn + 0xc) & *(uint *)(g_i960_pc_ptr + 0xb0);
  return;
}


// ===== i960_op589_xnor @ 004c1900 (23 bytes) =====

void i960_op589_xnor(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~(**(uint **)(g_i960_cur_insn + 0xc) ^ **(uint **)(g_i960_cur_insn + 8));
  return;
}


// ===== i960_op586_xor @ 004c1920 (21 bytes) =====

void i960_op586_xor(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       **(uint **)(g_i960_cur_insn + 0xc) ^ **(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op584_notand @ 004c1940 (23 bytes) =====

void i960_op584_notand(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~**(uint **)(g_i960_cur_insn + 0xc) & **(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op582_andnot @ 004c1960 (23 bytes) =====

void i960_op582_andnot(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~**(uint **)(g_i960_cur_insn + 8) & **(uint **)(g_i960_cur_insn + 0xc);
  return;
}


// ===== i960_op58a_not @ 004c1980 (18 bytes) =====

void i960_op58a_not(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) = ~**(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op58d_notor @ 004c19a0 (23 bytes) =====

void i960_op58d_notor(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~**(uint **)(g_i960_cur_insn + 0xc) | **(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op58b_ornot @ 004c19c0 (23 bytes) =====

void i960_op58b_ornot(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~**(uint **)(g_i960_cur_insn + 8) | **(uint **)(g_i960_cur_insn + 0xc);
  return;
}


// ===== i960_op587_or @ 004c19e0 (21 bytes) =====

void i960_op587_or(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       **(uint **)(g_i960_cur_insn + 0xc) | **(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op581_and @ 004c1a00 (21 bytes) =====

void i960_op581_and(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       **(uint **)(g_i960_cur_insn + 0xc) & **(uint **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op020_testno @ 004c1a20 (28 bytes) =====

void i960_op020_testno(void)

{
  **(uint **)(g_i960_cur_insn + 8) = (uint)(*(int *)(g_i960_pc_ptr + 0xa8) == 0);
  return;
}


// ===== i960_op021_testg @ 004c1a40 (27 bytes) =====

void i960_op021_testg(void)

{
  **(uint **)(g_i960_cur_insn + 8) = *(byte *)(g_i960_pc_ptr + 0xa8) & 1;
  return;
}


// ===== i960_op026_testle @ 004c1a60 (32 bytes) =====

void i960_op026_testle(void)

{
  **(uint **)(g_i960_cur_insn + 8) = (uint)((*(byte *)(g_i960_pc_ptr + 0xa8) & 6) != 0);
  return;
}


// ===== i960_op024_testl @ 004c1a80 (30 bytes) =====

void i960_op024_testl(void)

{
  **(uint **)(g_i960_cur_insn + 8) = *(byte *)(g_i960_pc_ptr + 0xa8) >> 2 & 1;
  return;
}


// ===== i960_op025_testne @ 004c1aa0 (32 bytes) =====

void i960_op025_testne(void)

{
  **(uint **)(g_i960_cur_insn + 8) = (uint)((*(byte *)(g_i960_pc_ptr + 0xa8) & 5) != 0);
  return;
}


// ===== i960_op022_teste @ 004c1ac0 (29 bytes) =====

void i960_op022_teste(void)

{
  **(uint **)(g_i960_cur_insn + 8) = *(byte *)(g_i960_pc_ptr + 0xa8) >> 1 & 1;
  return;
}


// ===== i960_op5b2_subc @ 004c1ae0 (104 bytes) =====

void i960_op5b2_subc(void)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  uint uVar4;
  uint uVar5;
  char cVar6;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 8);
  uVar2 = **(uint **)(g_i960_cur_insn + 0xc);
  piVar3 = *(int **)(g_i960_cur_insn + 0x10);
  uVar4 = (uint)((*(uint *)(g_i960_pc_ptr + 0xa8) >> 1 & 1) != 0);
  uVar5 = uVar2 - uVar1;
  cVar6 = (SBORROW4(uVar2,uVar1) != SBORROW4(uVar5,uVar4)) << 7;
  *(uint *)(g_i960_pc_ptr + 0xa8) =
       (uint)(ushort)(CONCAT11(cVar6,uVar2 < uVar1 || uVar5 < uVar4) << 1 | (ushort)(cVar6 < '\0'));
  *piVar3 = uVar5 - uVar4;
  return;
}


// ===== i960_op5b0_addc @ 004c1b50 (141 bytes) =====

void i960_op5b0_addc(void)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  iVar3 = g_i960_pc_ptr;
  piVar1 = *(int **)(g_i960_cur_insn + 0x10);
  uVar2 = **(uint **)(g_i960_cur_insn + 8);
  uVar5 = *(uint *)(g_i960_pc_ptr + 0xa8) & 2;
  uVar5 = (uint)(**(uint **)(g_i960_cur_insn + 0xc) + uVar5 != 0 ||
                CARRY4(**(uint **)(g_i960_cur_insn + 0xc),uVar5));
  iVar6 = uVar2 + uVar5;
  uVar4 = *(uint *)(g_i960_pc_ptr + 0xa8) & 0xfffffff8;
  *(uint *)(g_i960_pc_ptr + 0xa8) = uVar4;
  if (CARRY4(uVar2,uVar5)) {
    *(uint *)(iVar3 + 0xa8) = uVar4 | 2;
    *piVar1 = iVar6;
    return;
  }
  *piVar1 = iVar6;
  return;
}


// ===== i960_op593_subi @ 004c1be0 (21 bytes) =====

void i960_op593_subi(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) - **(int **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op592_subo @ 004c1c00 (21 bytes) =====

void i960_op592_subo(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) - **(int **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op590_addo @ 004c1c20 (21 bytes) =====

void i960_op590_addo(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) + **(int **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op591_addi @ 004c1c40 (21 bytes) =====

void i960_op591_addi(void)

{
  **(int **)(g_i960_cur_insn + 0x10) =
       **(int **)(g_i960_cur_insn + 0xc) + **(int **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op5ae_chkbit @ 004c1c60 (45 bytes) =====

void i960_op5ae_chkbit(void)

{
  *(uint *)(g_i960_pc_ptr + 0xa8) =
       -(uint)((1 << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f) &
               **(uint **)(g_i960_cur_insn + 0xc)) != 0) & 2;
  return;
}


// ===== i960_op58f_alterbit @ 004c1c90 (61 bytes) =====

void i960_op58f_alterbit(void)

{
  byte bVar1;
  
  bVar1 = (byte)**(undefined4 **)(g_i960_cur_insn + 8);
  if ((*(byte *)(g_i960_pc_ptr + 0xa8) & 2) != 0) {
    **(uint **)(g_i960_cur_insn + 0x10) = **(uint **)(g_i960_cur_insn + 0xc) | 1 << (bVar1 & 0x1f);
    return;
  }
  **(uint **)(g_i960_cur_insn + 0x10) = **(uint **)(g_i960_cur_insn + 0xc) & ~(1 << (bVar1 & 0x1f));
  return;
}


// ===== i960_op58c_clrbit @ 004c1cd0 (33 bytes) =====

void i960_op58c_clrbit(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       ~(1 << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f)) &
       **(uint **)(g_i960_cur_insn + 0xc);
  return;
}


// ===== i960_op583_setbit @ 004c1d00 (31 bytes) =====

void i960_op583_setbit(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       1 << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f) |
       **(uint **)(g_i960_cur_insn + 0xc);
  return;
}


// ===== i960_op580_notbit @ 004c1d20 (31 bytes) =====

void i960_op580_notbit(void)

{
  **(uint **)(g_i960_cur_insn + 0x10) =
       1 << ((byte)**(undefined4 **)(g_i960_cur_insn + 8) & 0x1f) ^
       **(uint **)(g_i960_cur_insn + 0xc);
  return;
}


// ===== i960_op08c_lda @ 004c1d40 (22 bytes) =====

void i960_op08c_lda(void)

{
  undefined4 uVar1;
  
  uVar1 = (**(code **)(g_i960_cur_insn + 0x14))();
  **(undefined4 **)(g_i960_cur_insn + 0x10) = uVar1;
  return;
}


// ===== i960_op5fc_movq @ 004c1d60 (59 bytes) =====

void i960_op5fc_movq(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  puVar1 = (undefined4 *)g_i960_cur_insn[2];
  puVar2 = (undefined4 *)g_i960_cur_insn[4];
  uVar3 = *g_i960_cur_insn;
  *puVar2 = *puVar1;
  if ((uVar3 & 0x800) != 0) {
    puVar2[1] = *puVar1;
    puVar2[2] = *puVar1;
    puVar2[3] = *puVar1;
    return;
  }
  puVar2[1] = puVar1[1];
  puVar2[2] = puVar1[2];
  puVar2[3] = puVar1[3];
  return;
}


// ===== i960_op5ec_movt @ 004c1da0 (48 bytes) =====

void i960_op5ec_movt(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  puVar1 = (undefined4 *)g_i960_cur_insn[2];
  puVar2 = (undefined4 *)g_i960_cur_insn[4];
  uVar3 = *g_i960_cur_insn;
  *puVar2 = *puVar1;
  if ((uVar3 & 0x800) != 0) {
    puVar2[1] = *puVar1;
    puVar2[2] = *puVar1;
    return;
  }
  puVar2[1] = puVar1[1];
  puVar2[2] = puVar1[2];
  return;
}


// ===== i960_op5dc_movl @ 004c1dd0 (37 bytes) =====

void i960_op5dc_movl(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  puVar1 = (undefined4 *)g_i960_cur_insn[2];
  puVar2 = (undefined4 *)g_i960_cur_insn[4];
  uVar3 = *g_i960_cur_insn;
  *puVar2 = *puVar1;
  if ((uVar3 & 0x800) != 0) {
    puVar2[1] = *puVar1;
    return;
  }
  puVar2[1] = puVar1[1];
  return;
}


// ===== i960_op644_dmovt @ 004c1e00 (46 bytes) =====

void i960_op644_dmovt(void)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = **(int **)(g_i960_cur_insn + 8);
  piVar2 = *(int **)(g_i960_cur_insn + 0x10);
  *(uint *)(g_i960_pc_ptr + 0xa8) = -(uint)(9 < iVar1 - 0x30U) & 2;
  *piVar2 = iVar1;
  return;
}


// ===== i960_op5cc_mov @ 004c1e30 (16 bytes) =====

void i960_op5cc_mov(void)

{
  **(undefined4 **)(g_i960_cur_insn + 0x10) = **(undefined4 **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op0b2_stq @ 004c1e40 (208 bytes) =====

void i960_op0b2_stq(void)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  iVar2 = g_i960_pc_ptr;
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  *(uint *)(g_i960_pc_ptr + 0x104) = uVar3;
  *(undefined1 *)(iVar2 + 0x100) = 4;
  piVar4 = (int *)(((uVar3 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    (*(code *)piVar4[3])();
  }
  else {
    *(undefined4 *)(iVar2 + uVar3) = *puVar1;
  }
  piVar4 = (int *)(((uVar3 + 4 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    (*(code *)piVar4[3])();
  }
  else {
    *(undefined4 *)(iVar2 + uVar3 + 4) = puVar1[1];
  }
  piVar4 = (int *)(((uVar3 + 8 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    (*(code *)piVar4[3])();
  }
  else {
    *(undefined4 *)(iVar2 + uVar3 + 8) = puVar1[2];
  }
  piVar4 = (int *)(((uVar3 + 0xc >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 != 0) {
    *(undefined4 *)(iVar2 + uVar3 + 0xc) = puVar1[3];
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c1f0e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar4[3])();
  return;
}


// ===== i960_op0a2_stt @ 004c1f10 (167 bytes) =====

void i960_op0a2_stt(void)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  iVar2 = g_i960_pc_ptr;
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  *(uint *)(g_i960_pc_ptr + 0x104) = uVar3;
  *(undefined1 *)(iVar2 + 0x100) = 3;
  piVar4 = (int *)(((uVar3 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    (*(code *)piVar4[3])();
  }
  else {
    *(undefined4 *)(iVar2 + uVar3) = *puVar1;
  }
  piVar4 = (int *)(((uVar3 + 4 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    (*(code *)piVar4[3])();
  }
  else {
    *(undefined4 *)(iVar2 + uVar3 + 4) = puVar1[1];
  }
  piVar4 = (int *)(((uVar3 + 8 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 != 0) {
    *(undefined4 *)(iVar2 + uVar3 + 8) = puVar1[2];
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c1fb5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar4[3])();
  return;
}


// ===== i960_op09a_stl @ 004c1fc0 (124 bytes) =====

void i960_op09a_stl(void)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  iVar2 = g_i960_pc_ptr;
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  *(uint *)(g_i960_pc_ptr + 0x104) = uVar3;
  *(undefined1 *)(iVar2 + 0x100) = 2;
  piVar4 = (int *)(((uVar3 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    (*(code *)piVar4[3])();
  }
  else {
    *(undefined4 *)(iVar2 + uVar3) = *puVar1;
  }
  piVar4 = (int *)(((uVar3 + 4 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 != 0) {
    *(undefined4 *)(iVar2 + uVar3 + 4) = puVar1[1];
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c203a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar4[3])();
  return;
}


// ===== i960_op0ca_stis @ 004c2040 (63 bytes) =====

void i960_op0ca_stis(void)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar3 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar1 = *piVar3;
  if (iVar1 != 0) {
    *(undefined2 *)(iVar1 + uVar2) = **(undefined2 **)(g_i960_cur_insn + 0x10);
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c207d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar3[2])();
  return;
}


// ===== i960_op08a_stos @ 004c2080 (63 bytes) =====

void i960_op08a_stos(void)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar3 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar1 = *piVar3;
  if (iVar1 != 0) {
    *(undefined2 *)(iVar1 + uVar2) = **(undefined2 **)(g_i960_cur_insn + 0x10);
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c20bd. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar3[2])();
  return;
}


// ===== i960_op0c2_stib @ 004c20c0 (61 bytes) =====

void i960_op0c2_stib(void)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar3 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar1 = *piVar3;
  if (iVar1 != 0) {
    *(undefined1 *)(iVar1 + uVar2) = **(undefined1 **)(g_i960_cur_insn + 0x10);
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c20fb. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar3[1])();
  return;
}


// ===== i960_op082_stob @ 004c2100 (61 bytes) =====

void i960_op082_stob(void)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar3 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar1 = *piVar3;
  if (iVar1 != 0) {
    *(undefined1 *)(iVar1 + uVar2) = **(undefined1 **)(g_i960_cur_insn + 0x10);
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c213b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar3[1])();
  return;
}


// ===== i960_op092_st @ 004c2140 (61 bytes) =====

void i960_op092_st(void)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar3 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar1 = *piVar3;
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + uVar2) = **(undefined4 **)(g_i960_cur_insn + 0x10);
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x004c217b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)piVar3[3])();
  return;
}


// ===== i960_op0b0_ldq @ 004c2180 (190 bytes) =====

void i960_op0b0_ldq(void)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined4 uVar5;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  iVar2 = g_i960_pc_ptr;
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  *(uint *)(g_i960_pc_ptr + 0x104) = uVar3;
  *(undefined1 *)(iVar2 + 0x100) = 4;
  piVar4 = (int *)((uVar3 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    uVar5 = (*(code *)piVar4[3])();
  }
  else {
    uVar5 = *(undefined4 *)(iVar2 + uVar3);
  }
  *puVar1 = uVar5;
  piVar4 = (int *)((uVar3 + 4 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    uVar5 = (*(code *)piVar4[3])();
  }
  else {
    uVar5 = *(undefined4 *)(iVar2 + uVar3 + 4);
  }
  puVar1[1] = uVar5;
  piVar4 = (int *)((uVar3 + 8 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    uVar5 = (*(code *)piVar4[3])();
  }
  else {
    uVar5 = *(undefined4 *)(iVar2 + uVar3 + 8);
  }
  puVar1[2] = uVar5;
  piVar4 = (int *)((uVar3 + 0xc >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 != 0) {
    puVar1[3] = *(undefined4 *)(iVar2 + uVar3 + 0xc);
    return;
  }
  uVar5 = (*(code *)piVar4[3])();
  puVar1[3] = uVar5;
  return;
}


// ===== i960_op0a0_ldt @ 004c2240 (154 bytes) =====

void i960_op0a0_ldt(void)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined4 uVar5;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  iVar2 = g_i960_pc_ptr;
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  *(uint *)(g_i960_pc_ptr + 0x104) = uVar3;
  *(undefined1 *)(iVar2 + 0x100) = 3;
  piVar4 = (int *)((uVar3 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    uVar5 = (*(code *)piVar4[3])();
  }
  else {
    uVar5 = *(undefined4 *)(iVar2 + uVar3);
  }
  *puVar1 = uVar5;
  piVar4 = (int *)((uVar3 + 4 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    uVar5 = (*(code *)piVar4[3])();
  }
  else {
    uVar5 = *(undefined4 *)(iVar2 + uVar3 + 4);
  }
  puVar1[1] = uVar5;
  piVar4 = (int *)((uVar3 + 8 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 != 0) {
    puVar1[2] = *(undefined4 *)(iVar2 + uVar3 + 8);
    return;
  }
  uVar5 = (*(code *)piVar4[3])();
  puVar1[2] = uVar5;
  return;
}


// ===== i960_op098_ldl @ 004c22e0 (118 bytes) =====

void i960_op098_ldl(void)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined4 uVar5;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  iVar2 = g_i960_pc_ptr;
  puVar1 = *(undefined4 **)(g_i960_cur_insn + 0x10);
  *(uint *)(g_i960_pc_ptr + 0x104) = uVar3;
  *(undefined1 *)(iVar2 + 0x100) = 2;
  piVar4 = (int *)((uVar3 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 == 0) {
    uVar5 = (*(code *)piVar4[3])();
  }
  else {
    uVar5 = *(undefined4 *)(iVar2 + uVar3);
  }
  *puVar1 = uVar5;
  piVar4 = (int *)((uVar3 + 4 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 != 0) {
    puVar1[1] = *(undefined4 *)(iVar2 + uVar3 + 4);
    return;
  }
  uVar5 = (*(code *)piVar4[3])();
  puVar1[1] = uVar5;
  return;
}


// ===== i960_op090_ld @ 004c2360 (64 bytes) =====

void i960_op090_ld(void)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  int *piVar4;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar4 = (int *)((uVar2 >> 0x10) * 0x10 + g_i960_ctx);
  iVar1 = *piVar4;
  if (iVar1 != 0) {
    **(undefined4 **)(g_i960_cur_insn + 0x10) = *(undefined4 *)(iVar1 + uVar2);
    return;
  }
  uVar3 = (*(code *)piVar4[3])();
  **(undefined4 **)(g_i960_cur_insn + 0x10) = uVar3;
  return;
}


// ===== i960_op0c8_ldis @ 004c23a0 (70 bytes) =====

void i960_op0c8_ldis(void)

{
  int iVar1;
  short sVar2;
  uint uVar3;
  int *piVar4;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar4 = (int *)((uVar3 >> 0x10) * 0x10 + g_i960_ctx);
  iVar1 = *piVar4;
  if (iVar1 != 0) {
    **(int **)(g_i960_cur_insn + 0x10) = (int)*(short *)(iVar1 + uVar3);
    return;
  }
  sVar2 = (*(code *)piVar4[2])();
  **(int **)(g_i960_cur_insn + 0x10) = (int)sVar2;
  return;
}


// ===== i960_op088_ldos @ 004c23f0 (74 bytes) =====

void i960_op088_ldos(void)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar3 = (int *)((uVar2 >> 0x10) * 0x10 + g_i960_ctx);
  iVar1 = *piVar3;
  if (iVar1 != 0) {
    **(uint **)(g_i960_cur_insn + 0x10) = (uint)*(ushort *)(iVar1 + uVar2);
    return;
  }
  uVar2 = (*(code *)piVar3[2])();
  **(uint **)(g_i960_cur_insn + 0x10) = uVar2 & 0xffff;
  return;
}


// ===== i960_op0c0_ldib @ 004c2440 (70 bytes) =====

void i960_op0c0_ldib(void)

{
  int iVar1;
  char cVar2;
  uint uVar3;
  int *piVar4;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  piVar4 = (int *)((uVar3 >> 0x10) * 0x10 + g_i960_ctx);
  iVar1 = *piVar4;
  if (iVar1 != 0) {
    **(int **)(g_i960_cur_insn + 0x10) = (int)*(char *)(iVar1 + uVar3);
    return;
  }
  cVar2 = (*(code *)piVar4[1])();
  **(int **)(g_i960_cur_insn + 0x10) = (int)cVar2;
  return;
}


// ===== i960_op080_ldob @ 004c2490 (62 bytes) =====

void i960_op080_ldob(void)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  
  uVar3 = (**(code **)(g_i960_cur_insn + 0x14))();
  puVar1 = *(uint **)(g_i960_cur_insn + 0x10);
  piVar4 = (int *)((uVar3 >> 0x10) * 0x10 + g_i960_ctx);
  iVar2 = *piVar4;
  if (iVar2 != 0) {
    *puVar1 = (uint)*(byte *)(iVar2 + uVar3);
    return;
  }
  uVar3 = (*(code *)piVar4[1])();
  *puVar1 = uVar3 & 0xff;
  return;
}


// ===== i960_invalid_opcode @ 004c2530 (62 bytes) =====

void i960_invalid_opcode(void)

{
  char acStack_80 [128];
  
  _sprintf(acStack_80,"Invalid opcode at %X",*g_i960_pc_ptr + -4);
  MessageBoxA((HWND)0x0,acStack_80,"Invalid Opcode",0);
  return;
}


// ===== FUN_004c25f0 @ 004c25f0 (114 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c25f0(int param_1)

{
  int in_EAX;
  int iVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(param_1 + 0x200040) >> 0x14;
  if (uVar2 < *(uint *)(param_1 + 0x200024) >> 0x14) {
    _DAT_010f01cc = *(int *)(param_1 + 0x200000);
    uVar2 = in_EAX - *(int *)(param_1 + 0x200010);
  }
  else if (uVar2 < *(uint *)(param_1 + 0x200038) >> 0x14) {
    _DAT_010f01cc = *(int *)(param_1 + 0x200014);
    uVar2 = in_EAX - *(uint *)(param_1 + 0x200024);
  }
  else {
    _DAT_010f01cc = *(int *)(param_1 + 0x200028);
    uVar2 = in_EAX - *(uint *)(param_1 + 0x200038);
  }
  iVar1 = (uVar2 >> 2) * 0x30 + _DAT_010f01cc;
  if (*(undefined ***)(iVar1 + 0x18) != &PTR_LAB_00573598) {
    *(undefined4 *)(iVar1 + 0x1c) = *(undefined4 *)(iVar1 + 0x18);
    *(undefined ***)(iVar1 + 0x18) = &PTR_LAB_00573598;
  }
  return;
}


// ===== FUN_004c2670 @ 004c2670 (114 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c2670(int param_1)

{
  int in_EAX;
  int iVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(param_1 + 0x200040) >> 0x14;
  if (uVar2 < *(uint *)(param_1 + 0x200024) >> 0x14) {
    _DAT_010f01c8 = *(int *)(param_1 + 0x200000);
    uVar2 = in_EAX - *(int *)(param_1 + 0x200010);
  }
  else if (uVar2 < *(uint *)(param_1 + 0x200038) >> 0x14) {
    _DAT_010f01c8 = *(int *)(param_1 + 0x200014);
    uVar2 = in_EAX - *(uint *)(param_1 + 0x200024);
  }
  else {
    _DAT_010f01c8 = *(int *)(param_1 + 0x200028);
    uVar2 = in_EAX - *(uint *)(param_1 + 0x200038);
  }
  iVar1 = (uVar2 >> 2) * 0x30 + _DAT_010f01c8;
  if (*(undefined ***)(iVar1 + 0x18) != &PTR_LAB_0057346c) {
    *(undefined4 *)(iVar1 + 0x1c) = *(undefined4 *)(iVar1 + 0x18);
    *(undefined ***)(iVar1 + 0x18) = &PTR_LAB_0057346c;
  }
  return;
}


// ===== i960_get_byte_200140 @ 004c2720 (7 bytes) =====

undefined1 i960_get_byte_200140(void)

{
  int in_EAX;
  
  return *(undefined1 *)(in_EAX + 0x200140);
}


// ===== i960_get_pc @ 004c2730 (7 bytes) =====

undefined4 i960_get_pc(void)

{
  int in_EAX;
  
  return *(undefined4 *)(in_EAX + 0x200040);
}


// ===== i960_build_optable @ 004c2740 (68 bytes) =====

void i960_build_optable(void)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined **ppuVar3;
  undefined *puVar4;
  
  puVar2 = g_i960_opdefs;
  puVar4 = (undefined *)0x0;
  do {
    ppuVar3 = &g_i960_opdefs;
    puVar1 = puVar2;
    while (puVar1 != (undefined *)0x0) {
      if (ppuVar3[3] == puVar4) {
        if (*ppuVar3 != (undefined *)0x0) goto LAB_004c2779;
        break;
      }
      ppuVar3 = ppuVar3 + 9;
      puVar1 = *ppuVar3;
    }
    ppuVar3 = &g_i960_invalid_opdef;
LAB_004c2779:
    (&g_i960_optable)[(int)puVar4] = ppuVar3;
    puVar4 = puVar4 + 1;
    if (0xfff < (int)puVar4) {
      return;
    }
  } while( true );
}


// ===== i960_opcode_key @ 004c2790 (25 bytes) =====

uint __fastcall i960_opcode_key(uint param_1)

{
  uint uVar1;
  
  uVar1 = param_1 >> 0x18;
  if (uVar1 - 0x58 < 0x22) {
    uVar1 = uVar1 << 4 | param_1 >> 7 & 0xf;
  }
  return uVar1;
}


// ===== FUN_004c28b0 @ 004c28b0 (32 bytes) =====

void FUN_004c28b0(void)

{
  uint uVar1;
  
  uVar1 = *g_i960_cur_insn & 0xffffff;
  if ((*g_i960_cur_insn & 0x800000) != 0) {
    uVar1 = uVar1 | 0xff000000;
  }
  g_i960_cur_insn[1] = uVar1 - 4;
  return;
}


// ===== FUN_004c28d0 @ 004c28d0 (32 bytes) =====

void FUN_004c28d0(void)

{
  uint uVar1;
  
  uVar1 = *g_i960_cur_insn & 0x1fff;
  if ((*g_i960_cur_insn & 0x1000) != 0) {
    uVar1 = uVar1 | 0xfffff000;
  }
  g_i960_cur_insn[1] = uVar1 - 4;
  return;
}


// ===== FUN_004c2910 @ 004c2910 (84 bytes) =====

void FUN_004c2910(void)

{
  uint uVar1;
  int unaff_ESI;
  
  uVar1 = *g_i960_cur_insn >> 0x13 & 0x1f;
  if ((*g_i960_cur_insn & 0x2000) == 0) {
    g_i960_cur_insn[4] = unaff_ESI + 0x200044 + uVar1 * 4;
    return;
  }
  if (*(int *)(g_i960_cur_insn[6] + 0x14) == 0) {
    g_i960_cur_insn[4] = (uint)(&g_i960_literals + uVar1 * 4);
    return;
  }
  if (3 < uVar1) {
    g_i960_cur_insn[4] = (uint)&g_i960_literals;
    return;
  }
  g_i960_cur_insn[4] = unaff_ESI + 0x2000c8 + uVar1 * 8;
  return;
}


// ===== FUN_004c2990 @ 004c2990 (110 bytes) =====

void FUN_004c2990(void)

{
  uint uVar1;
  int unaff_ESI;
  
  uVar1 = *g_i960_cur_insn >> 0xe & 0x1f;
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    g_i960_cur_insn[3] = unaff_ESI + 0x200044 + uVar1 * 4;
    return;
  }
  if (*(int *)(g_i960_cur_insn[6] + 0x14) != 0) {
    switch(uVar1) {
    case 0:
    case 1:
    case 2:
    case 3:
      g_i960_cur_insn[3] = unaff_ESI + 0x2000c8 + uVar1 * 8;
      return;
    default:
      g_i960_cur_insn[3] = (uint)&g_i960_literals;
      return;
    case 0x10:
      g_i960_cur_insn[3] = (uint)&DAT_010ed948;
      return;
    case 0x16:
      g_i960_cur_insn[3] = (uint)&DAT_00573590;
      return;
    }
  }
  g_i960_cur_insn[3] = (uint)(&g_i960_literals + uVar1 * 4);
  return;
}


// ===== FUN_004c2a30 @ 004c2a30 (55 bytes) =====

void FUN_004c2a30(int param_1)

{
  uint uVar1;
  
  uVar1 = *g_i960_cur_insn >> 0x13 & 0x1f;
  if ((*g_i960_cur_insn & 0x2000) == 0) {
    g_i960_cur_insn[2] = param_1 + 0x200044 + uVar1 * 4;
    return;
  }
  g_i960_cur_insn[2] = (uint)(&g_i960_literals + uVar1 * 4);
  return;
}


// ===== FUN_004c2a70 @ 004c2a70 (107 bytes) =====

void FUN_004c2a70(void)

{
  uint uVar1;
  int unaff_ESI;
  
  uVar1 = *g_i960_cur_insn & 0x1f;
  if ((*g_i960_cur_insn & 0x800) == 0) {
    g_i960_cur_insn[2] = unaff_ESI + 0x200044 + uVar1 * 4;
    return;
  }
  if (*(int *)(g_i960_cur_insn[6] + 0x14) != 0) {
    switch(uVar1) {
    case 0:
    case 1:
    case 2:
    case 3:
      g_i960_cur_insn[2] = unaff_ESI + 0x2000c8 + uVar1 * 8;
      return;
    default:
      g_i960_cur_insn[2] = (uint)&g_i960_literals;
      return;
    case 0x10:
      g_i960_cur_insn[2] = (uint)&DAT_010ed948;
      return;
    case 0x16:
      g_i960_cur_insn[2] = (uint)&DAT_00573590;
      return;
    }
  }
  g_i960_cur_insn[2] = (uint)(&g_i960_literals + uVar1 * 4);
  return;
}


// ===== i960_op6d9_movrl @ 004c2b90 (47 bytes) =====

void i960_op6d9_movrl(void)

{
  if ((*g_i960_cur_insn & 0x800) != 0) {
    *(undefined8 *)g_i960_cur_insn[4] = *(undefined8 *)g_i960_cur_insn[2];
    return;
  }
  *(undefined8 *)g_i960_cur_insn[4] = *(undefined8 *)g_i960_cur_insn[2];
  return;
}


// ===== i960_op6c9_movr @ 004c2bc0 (53 bytes) =====

void i960_op6c9_movr(void)

{
  float fVar1;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    fVar1 = *(float *)g_i960_cur_insn[2];
  }
  else {
    fVar1 = (float)*(double *)g_i960_cur_insn[2];
  }
  if ((*g_i960_cur_insn & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  *(float *)g_i960_cur_insn[4] = fVar1;
  return;
}


// ===== i960_op6c3_cvtzril @ 004c2c00 (55 bytes) =====

void i960_op6c3_cvtzril(void)

{
  uint *puVar1;
  undefined8 uVar2;
  
  puVar1 = g_i960_cur_insn;
  if ((*g_i960_cur_insn & 0x800) != 0) {
    uVar2 = FUN_00500926();
    *(undefined8 *)puVar1[4] = uVar2;
    return;
  }
  uVar2 = FUN_00500926();
  *(undefined8 *)puVar1[4] = uVar2;
  return;
}


// ===== i960_op6c2_cvtzri @ 004c2c40 (87 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op6c2_cvtzri(void)

{
  double dVar1;
  int iVar2;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    dVar1 = (double)*(float *)g_i960_cur_insn[2];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[2];
  }
  iVar2 = (int)ROUND(ABS(dVar1 + dVar1) + (double)_DAT_0051cf44) >> 1;
  if (dVar1 < 0.0) {
    iVar2 = -iVar2;
  }
  *(int *)g_i960_cur_insn[4] = iVar2;
  return;
}


// ===== i960_op675_cvtilr @ 004c2ca0 (25 bytes) =====

void i960_op675_cvtilr(void)

{
  **(double **)(g_i960_cur_insn + 0x10) = (double)**(longlong **)(g_i960_cur_insn + 8);
  return;
}


// ===== i960_op674_cvtir @ 004c2cc0 (39 bytes) =====

void i960_op674_cvtir(void)

{
  if ((*g_i960_cur_insn & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)*(int *)g_i960_cur_insn[2];
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)*(int *)g_i960_cur_insn[2];
  return;
}


// ===== i960_op79b_divrl @ 004c2cf0 (58 bytes) =====

void i960_op79b_divrl(void)

{
  if ((*g_i960_cur_insn & 0x1000) != 0) {
    *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] / *(double *)g_i960_cur_insn[2];
    return;
  }
  *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] / *(double *)g_i960_cur_insn[2];
  return;
}


// ===== i960_op78b_divr @ 004c2d30 (72 bytes) =====

void i960_op78b_divr(void)

{
  uint uVar1;
  float10 fVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  if ((uVar1 & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  if ((uVar1 & 0x1000) == 0) {
    fVar3 = (float10)*(float *)g_i960_cur_insn[3];
  }
  else {
    fVar3 = (float10)*(double *)g_i960_cur_insn[3];
  }
  if ((uVar1 & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)(fVar3 / fVar2);
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)(fVar3 / fVar2);
  return;
}


// ===== i960_op79c_mulrl @ 004c2d80 (58 bytes) =====

void i960_op79c_mulrl(void)

{
  if ((*g_i960_cur_insn & 0x1000) != 0) {
    *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] * *(double *)g_i960_cur_insn[2];
    return;
  }
  *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] * *(double *)g_i960_cur_insn[2];
  return;
}


// ===== i960_op78c_mulr @ 004c2dc0 (72 bytes) =====

void i960_op78c_mulr(void)

{
  uint uVar1;
  float10 fVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  if ((uVar1 & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  if ((uVar1 & 0x1000) == 0) {
    fVar3 = (float10)*(float *)g_i960_cur_insn[3];
  }
  else {
    fVar3 = (float10)*(double *)g_i960_cur_insn[3];
  }
  if ((uVar1 & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)(fVar3 * fVar2);
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)(fVar3 * fVar2);
  return;
}


// ===== i960_op79d_subrl @ 004c2e10 (58 bytes) =====

void i960_op79d_subrl(void)

{
  if ((*g_i960_cur_insn & 0x1000) != 0) {
    *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] - *(double *)g_i960_cur_insn[2];
    return;
  }
  *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] - *(double *)g_i960_cur_insn[2];
  return;
}


// ===== i960_op78d_subr @ 004c2e50 (72 bytes) =====

void i960_op78d_subr(void)

{
  uint uVar1;
  float10 fVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  if ((uVar1 & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  if ((uVar1 & 0x1000) == 0) {
    fVar3 = (float10)*(float *)g_i960_cur_insn[3];
  }
  else {
    fVar3 = (float10)*(double *)g_i960_cur_insn[3];
  }
  if ((uVar1 & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)(fVar3 - fVar2);
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)(fVar3 - fVar2);
  return;
}


// ===== i960_op79f_addrl @ 004c2ea0 (58 bytes) =====

void i960_op79f_addrl(void)

{
  if ((*g_i960_cur_insn & 0x1000) != 0) {
    *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] + *(double *)g_i960_cur_insn[2];
    return;
  }
  *(double *)g_i960_cur_insn[4] = *(double *)g_i960_cur_insn[3] + *(double *)g_i960_cur_insn[2];
  return;
}


// ===== i960_op78f_addr @ 004c2ee0 (72 bytes) =====

void i960_op78f_addr(void)

{
  uint uVar1;
  float10 fVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  if ((uVar1 & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  if ((uVar1 & 0x1000) == 0) {
    fVar3 = (float10)*(float *)g_i960_cur_insn[3];
  }
  else {
    fVar3 = (float10)*(double *)g_i960_cur_insn[3];
  }
  if ((uVar1 & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)(fVar3 + fVar2);
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)(fVar3 + fVar2);
  return;
}


// ===== i960_op695_cmprl @ 004c2f30 (156 bytes) =====

void i960_op695_cmprl(void)

{
  double dVar1;
  double dVar2;
  
  dVar1 = *(double *)g_i960_cur_insn[2];
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    dVar2 = *(double *)g_i960_cur_insn[3];
  }
  else {
    dVar2 = *(double *)g_i960_cur_insn[3];
  }
  if (dVar1 < dVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    return;
  }
  if (dVar2 == dVar1) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    return;
  }
  if (dVar2 < dVar1) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 1;
    return;
  }
  *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 0;
  return;
}


// ===== i960_op685_cmpr @ 004c2fd0 (168 bytes) =====

void i960_op685_cmpr(void)

{
  double dVar1;
  double dVar2;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    dVar1 = (double)*(float *)g_i960_cur_insn[2];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[2];
  }
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    dVar2 = (double)*(float *)g_i960_cur_insn[3];
  }
  else {
    dVar2 = *(double *)g_i960_cur_insn[3];
  }
  if (dVar1 < dVar2) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 4;
    return;
  }
  if (dVar2 == dVar1) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    return;
  }
  if (dVar2 < dVar1) {
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 1;
    return;
  }
  *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 0;
  return;
}


// ===== i960_op00a_ret @ 004c3080 (718 bytes) =====

void i960_op00a_ret(void)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  undefined4 *puVar8;
  uint uVar9;
  uint uVar10;
  undefined4 *puVar11;
  int iVar12;
  undefined4 *puVar13;
  
  uVar6 = g_i960_pc_ptr[1] & 7;
  if (uVar6 == 0) {
    uVar6 = g_i960_pc_ptr[1] & 0xffffffc0;
    iVar12 = *(int *)(g_i960_ctx + 0x20014c);
    puVar8 = g_i960_pc_ptr;
    iVar7 = g_i960_ctx;
    while (g_i960_pc_ptr = puVar8, 0 < iVar12) {
      piVar1 = (int *)(iVar7 + 0x200148);
      *piVar1 = *piVar1 + -1;
      if (*piVar1 < 0) {
        *(undefined4 *)(iVar7 + 0x200148) = 0x7f;
      }
      *(int *)(iVar7 + 0x20014c) = *(int *)(iVar7 + 0x20014c) + -1;
      puVar8 = g_i960_pc_ptr;
      uVar9 = *(uint *)(iVar7 + 0x200150 + *(int *)(iVar7 + 0x200148) * 4);
      if (uVar9 == uVar6) {
        puVar11 = (undefined4 *)(*(int *)(iVar7 + 0x200148) * 0x40 + 0x200350 + iVar7);
        puVar13 = g_i960_pc_ptr;
        for (iVar12 = 0x10; puVar13 = puVar13 + 1, iVar12 != 0; iVar12 = iVar12 + -1) {
          *puVar13 = *puVar11;
          puVar11 = puVar11 + 1;
        }
        puVar8[0x20] = uVar6;
        *puVar8 = puVar8[3];
        return;
      }
      iVar12 = 0;
      do {
        iVar4 = ((uVar9 >> 0x10) + 0x10000) * 0x10;
        iVar2 = *(int *)(iVar4 + iVar7);
        if (iVar2 == 0) {
          (**(code **)(iVar4 + iVar7 + 0xc))();
          iVar7 = g_i960_ctx;
        }
        else {
          *(undefined4 *)(uVar9 + iVar2) =
               *(undefined4 *)(iVar7 + 0x200350 + (*(int *)(iVar7 + 0x200148) * 0x10 + iVar12) * 4);
        }
        iVar12 = iVar12 + 1;
        uVar9 = uVar9 + 4;
      } while (iVar12 < 0x10);
      puVar8 = g_i960_pc_ptr;
      iVar12 = *(int *)(iVar7 + 0x20014c);
    }
    iVar12 = 0;
    uVar9 = uVar6;
    do {
      iVar4 = (uVar9 >> 0x10) * 0x10;
      iVar2 = *(int *)(iVar4 + iVar7);
      if (iVar2 == 0) {
        uVar3 = (**(code **)(iVar4 + iVar7 + 0xc))();
        iVar7 = g_i960_ctx;
        puVar8 = g_i960_pc_ptr;
      }
      else {
        uVar3 = *(undefined4 *)(uVar9 + iVar2);
      }
      *(undefined4 *)(iVar12 + 4 + (int)puVar8) = uVar3;
      iVar12 = iVar12 + 4;
      uVar9 = uVar9 + 4;
    } while (iVar12 < 0x40);
    puVar8[0x20] = uVar6;
    *puVar8 = puVar8[3];
  }
  else if (uVar6 == 7) {
    uVar6 = g_i960_pc_ptr[0x20] - 0x10 >> 0x10;
    iVar12 = *(int *)(g_i960_ctx + uVar6 * 0x10);
    if (iVar12 == 0) {
      uVar3 = (**(code **)(g_i960_ctx + 0xc + uVar6 * 0x10))();
    }
    else {
      uVar3 = *(undefined4 *)(iVar12 + (g_i960_pc_ptr[0x20] - 0x10));
    }
    uVar6 = g_i960_pc_ptr[0x20] - 0xc >> 0x10;
    iVar12 = *(int *)(g_i960_ctx + uVar6 * 0x10);
    if (iVar12 == 0) {
      uVar6 = (**(code **)(g_i960_ctx + 0xc + uVar6 * 0x10))();
    }
    else {
      uVar6 = *(uint *)(iVar12 + (g_i960_pc_ptr[0x20] - 0xc));
    }
    uVar9 = g_i960_pc_ptr[1] & 0xfffffff8;
    iVar12 = *(int *)(g_i960_ctx + 0x20014c);
    puVar8 = g_i960_pc_ptr;
    iVar7 = g_i960_ctx;
    do {
      g_i960_pc_ptr = puVar8;
      if (iVar12 < 1) {
        iVar12 = 0;
        uVar10 = uVar9;
        do {
          iVar2 = *(int *)(iVar7 + (uVar10 >> 0x10) * 0x10);
          if (iVar2 == 0) {
            uVar5 = (**(code **)(iVar7 + 0xc + (uVar10 >> 0x10) * 0x10))();
            puVar8 = g_i960_pc_ptr;
            iVar7 = g_i960_ctx;
          }
          else {
            uVar5 = *(undefined4 *)(uVar10 + iVar2);
          }
          *(undefined4 *)(iVar12 + 4 + (int)puVar8) = uVar5;
          iVar12 = iVar12 + 4;
          uVar10 = uVar10 + 4;
        } while (iVar12 < 0x40);
LAB_004c31f8:
        *puVar8 = puVar8[3];
        puVar8[0x20] = uVar9;
        *(uint *)(iVar7 + 0x2000ec) = uVar6;
        *(uint *)(iVar7 + 0x2000e8) = uVar6 & 7;
        if ((*(byte *)(puVar8 + 0x2c) & 2) != 0) {
          puVar8[0x2c] = uVar3;
        }
        *(undefined1 *)(iVar7 + 0x202358) = 0;
        return;
      }
      piVar1 = (int *)(iVar7 + 0x200148);
      *piVar1 = *piVar1 + -1;
      if (*piVar1 < 0) {
        *(undefined4 *)(iVar7 + 0x200148) = 0x7f;
      }
      *(int *)(iVar7 + 0x20014c) = *(int *)(iVar7 + 0x20014c) + -1;
      puVar8 = g_i960_pc_ptr;
      uVar10 = *(uint *)(iVar7 + 0x200150 + *(int *)(iVar7 + 0x200148) * 4);
      if (uVar10 == uVar9) {
        puVar11 = (undefined4 *)(*(int *)(iVar7 + 0x200148) * 0x40 + 0x200350 + iVar7);
        puVar13 = g_i960_pc_ptr;
        for (iVar12 = 0x10; puVar13 = puVar13 + 1, iVar12 != 0; iVar12 = iVar12 + -1) {
          *puVar13 = *puVar11;
          puVar11 = puVar11 + 1;
        }
        goto LAB_004c31f8;
      }
      iVar12 = 0;
      do {
        iVar4 = (uVar10 >> 0x10) + 0x10000;
        iVar2 = *(int *)(iVar7 + iVar4 * 0x10);
        if (iVar2 == 0) {
          (**(code **)(iVar7 + 0xc + iVar4 * 0x10))();
          iVar7 = g_i960_ctx;
        }
        else {
          *(undefined4 *)(uVar10 + iVar2) =
               *(undefined4 *)(iVar7 + 0x200350 + (*(int *)(iVar7 + 0x200148) * 0x10 + iVar12) * 4);
        }
        iVar12 = iVar12 + 1;
        uVar10 = uVar10 + 4;
      } while (iVar12 < 0x10);
      iVar12 = *(int *)(iVar7 + 0x20014c);
      puVar8 = g_i960_pc_ptr;
    } while( true );
  }
  return;
}


// ===== i960_op602_synmovq @ 004c3350 (462 bytes) =====

void i960_op602_synmovq(void)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  
  uVar1 = **(uint **)(g_i960_cur_insn + 0xc);
  uVar2 = **(uint **)(g_i960_cur_insn + 8);
  iVar3 = *(int *)(g_i960_ctx + (uVar1 >> 0x10) * 0x10);
  if (iVar3 == 0) {
    uVar5 = (**(code **)(g_i960_ctx + 0xc + (uVar1 >> 0x10) * 0x10))();
  }
  else {
    uVar5 = *(undefined4 *)(iVar3 + uVar1);
  }
  iVar7 = g_i960_ctx;
  iVar6 = (uVar2 >> 0x10) + 0x10000;
  iVar3 = *(int *)(g_i960_ctx + iVar6 * 0x10);
  if (iVar3 == 0) {
    (**(code **)(g_i960_ctx + 0xc + iVar6 * 0x10))();
    iVar7 = g_i960_ctx;
  }
  else {
    *(undefined4 *)(iVar3 + uVar2) = uVar5;
  }
  uVar4 = uVar1 + 4 >> 0x10;
  iVar3 = *(int *)(iVar7 + uVar4 * 0x10);
  if (iVar3 == 0) {
    uVar5 = (**(code **)(iVar7 + 0xc + uVar4 * 0x10))();
    iVar7 = g_i960_ctx;
  }
  else {
    uVar5 = *(undefined4 *)(iVar3 + uVar1 + 4);
  }
  iVar6 = (uVar2 + 4 >> 0x10) + 0x10000;
  iVar3 = *(int *)(iVar7 + iVar6 * 0x10);
  if (iVar3 == 0) {
    (**(code **)(iVar7 + 0xc + iVar6 * 0x10))();
    iVar7 = g_i960_ctx;
  }
  else {
    *(undefined4 *)(iVar3 + uVar2 + 4) = uVar5;
  }
  uVar4 = uVar1 + 8 >> 0x10;
  iVar3 = *(int *)(iVar7 + uVar4 * 0x10);
  if (iVar3 == 0) {
    uVar5 = (**(code **)(iVar7 + 0xc + uVar4 * 0x10))();
    iVar7 = g_i960_ctx;
  }
  else {
    uVar5 = *(undefined4 *)(iVar3 + uVar1 + 8);
  }
  iVar6 = (uVar2 + 8 >> 0x10) + 0x10000;
  iVar3 = *(int *)(iVar7 + iVar6 * 0x10);
  if (iVar3 == 0) {
    (**(code **)(iVar7 + 0xc + iVar6 * 0x10))();
    iVar7 = g_i960_ctx;
  }
  else {
    *(undefined4 *)(iVar3 + uVar2 + 8) = uVar5;
  }
  uVar4 = uVar1 + 0xc >> 0x10;
  iVar3 = *(int *)(iVar7 + uVar4 * 0x10);
  if (iVar3 == 0) {
    uVar5 = (**(code **)(iVar7 + 0xc + uVar4 * 0x10))();
    iVar7 = g_i960_ctx;
  }
  else {
    uVar5 = *(undefined4 *)(iVar3 + uVar1 + 0xc);
  }
  iVar6 = (uVar2 + 0xc >> 0x10) + 0x10000;
  iVar3 = *(int *)(iVar7 + iVar6 * 0x10);
  if (iVar3 == 0) {
    (**(code **)(iVar7 + 0xc + iVar6 * 0x10))();
    iVar7 = g_i960_ctx;
  }
  else {
    *(undefined4 *)(iVar3 + uVar2 + 0xc) = uVar5;
  }
  if (uVar2 == 0xff000010) {
    if (*(char *)(iVar7 + 0x200103) == -0x6d) {
      *(undefined4 *)(iVar7 + 0x2000f4) = *(undefined4 *)(iVar7 + 0x200104);
      iVar3 = g_i960_pc_ptr;
      *(undefined4 *)(iVar7 + 0x2000f8) = *(undefined4 *)(iVar7 + 0x200108);
      *(undefined4 *)(iVar7 + 0x200040) = *(undefined4 *)(iVar7 + 0x20010c);
      *(undefined4 *)(iVar3 + 0xa8) = 2;
      return;
    }
    *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
    return;
  }
  *(undefined4 *)(g_i960_pc_ptr + 0xa8) = 2;
  return;
}


// ===== i960_op645_modac @ 004c3520 (76 bytes) =====

void i960_op645_modac(void)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  iVar2 = g_i960_ctx;
  puVar1 = *(uint **)(g_i960_cur_insn + 0x10);
  uVar3 = (*(uint *)(g_i960_ctx + 0x2000ec) ^ *(uint *)(g_i960_ctx + 0x2000e8)) & 7 ^
          *(uint *)(g_i960_ctx + 0x2000ec);
  uVar4 = **(uint **)(g_i960_cur_insn + 0xc) & **(uint **)(g_i960_cur_insn + 8) |
          ~**(uint **)(g_i960_cur_insn + 8) & uVar3;
  *(uint *)(g_i960_ctx + 0x2000ec) = uVar4;
  *(uint *)(iVar2 + 0x2000e8) = uVar4 & 7;
  *puVar1 = uVar3;
  return;
}


// ===== FUN_004c3570 @ 004c3570 (202 bytes) =====

void FUN_004c3570(void)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  int unaff_ESI;
  
  if ((undefined4 *)*g_i960_ctx == (undefined4 *)0x0) {
    uVar1 = (*(code *)g_i960_ctx[3])();
  }
  else {
    uVar1 = *(undefined4 *)*g_i960_ctx;
  }
  piVar4 = g_i960_ctx;
  *(undefined4 *)(unaff_ESI + 0x2000f4) = uVar1;
  if (*piVar4 == 0) {
    uVar1 = (*(code *)piVar4[3])();
    piVar4 = g_i960_ctx;
  }
  else {
    uVar1 = *(undefined4 *)(*piVar4 + 4);
  }
  *(undefined4 *)(unaff_ESI + 0x2000f8) = uVar1;
  if (*piVar4 == 0) {
    uVar1 = (*(code *)piVar4[3])();
    piVar4 = g_i960_ctx;
  }
  else {
    uVar1 = *(undefined4 *)(*piVar4 + 0xc);
  }
  *(undefined4 *)(unaff_ESI + 0x200040) = uVar1;
  uVar3 = *(int *)(unaff_ESI + 0x2000f8) + 0x18;
  *(undefined4 *)(unaff_ESI + 0x2000ec) = 0;
  *(undefined4 *)(unaff_ESI + 0x2000e8) = 0;
  *(undefined4 *)(unaff_ESI + 0x2000f0) = 0x1f2002;
  iVar2 = piVar4[(uVar3 >> 0x10) * 4];
  if (iVar2 == 0) {
    iVar2 = (*(code *)(piVar4 + (uVar3 >> 0x10) * 4)[3])();
  }
  else {
    iVar2 = *(int *)(iVar2 + uVar3);
  }
  *(int *)(unaff_ESI + 0x2000c0) = iVar2;
  *(int *)(unaff_ESI + 0x200048) = iVar2 + 0x40;
  *(undefined1 *)(unaff_ESI + 0x202358) = 0;
  *(undefined4 *)(unaff_ESI + 0x20014c) = 0;
  *(int *)(unaff_ESI + 0x20003c) = unaff_ESI + 0x200000;
  return;
}


// ===== FUN_004c36c0 @ 004c36c0 (267 bytes) =====

void __fastcall FUN_004c36c0(undefined4 param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  int unaff_ESI;
  
  puVar1 = g_i960_cur_insn;
  uVar3 = *g_i960_cur_insn;
  g_i960_cur_insn[9] = param_2 + 0x200044 + (uVar3 >> 0xe & 0x1f) * 4;
  if ((uVar3 & 0x1000) != 0) {
    puVar1[10] = param_2 + 0x200044 + (uVar3 & 0x1f) * 4;
    puVar1[0xb] = uVar3 >> 7 & 7;
    switch(uVar3 >> 10 & 0xf) {
    case 4:
      puVar1[5] = (uint)&LAB_004c2880;
      return;
    case 5:
      iVar2 = FUN_004c06a0();
      puVar1 = g_i960_cur_insn;
      g_i960_cur_insn[8] = iVar2 + 8 + unaff_ESI;
      puVar1[5] = (uint)&LAB_004c2860;
      return;
    case 7:
      puVar1[5] = (uint)&LAB_004c2840;
      return;
    case 0xc:
      uVar3 = FUN_004c06a0();
      puVar1 = g_i960_cur_insn;
      g_i960_cur_insn[5] = (uint)&LAB_004c2820;
      puVar1[8] = uVar3;
      return;
    case 0xd:
      uVar3 = FUN_004c06a0();
      puVar1 = g_i960_cur_insn;
      g_i960_cur_insn[5] = (uint)&LAB_004c2800;
      puVar1[8] = uVar3;
      return;
    case 0xe:
      uVar3 = FUN_004c06a0();
      puVar1 = g_i960_cur_insn;
      g_i960_cur_insn[5] = (uint)&LAB_004c27e0;
      puVar1[8] = uVar3;
      return;
    case 0xf:
      uVar3 = FUN_004c06a0();
      puVar1 = g_i960_cur_insn;
      g_i960_cur_insn[5] = (uint)&LAB_004c27b0;
      puVar1[8] = uVar3;
    }
    return;
  }
  puVar1[8] = uVar3 & 0x1fff;
  if ((uVar3 & 0x2000) == 0) {
    puVar1[5] = (uint)&LAB_004c28a0;
    return;
  }
  puVar1[5] = (uint)&LAB_004c2890;
  return;
}


// ===== i960_op6e3_cpyrsre @ 004c3800 (62 bytes) =====

void i960_op6e3_cpyrsre(void)

{
  double dVar1;
  double dVar2;
  
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  dVar2 = ABS(*(double *)g_i960_cur_insn[2]);
  if (!NAN(dVar1) && 0.0 < dVar1 != (dVar1 == 0.0)) {
    dVar2 = -dVar2;
  }
  *(double *)g_i960_cur_insn[4] = dVar2;
  return;
}


// ===== i960_op6e2_cpysre @ 004c3840 (62 bytes) =====

void i960_op6e2_cpysre(void)

{
  double dVar1;
  double dVar2;
  
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  dVar2 = ABS(*(double *)g_i960_cur_insn[2]);
  if (NAN(dVar1) || 0.0 < dVar1 == (dVar1 == 0.0)) {
    dVar2 = -dVar2;
  }
  *(double *)g_i960_cur_insn[4] = dVar2;
  return;
}


// ===== i960_op676_scalerl @ 004c3880 (71 bytes) =====

void i960_op676_scalerl(void)

{
  double dVar1;
  double *pdVar2;
  float10 fVar3;
  
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  pdVar2 = (double *)g_i960_cur_insn[4];
  fVar3 = (float10)FUN_005006a0();
  *pdVar2 = (double)(fVar3 * (float10)dVar1);
  return;
}


// ===== i960_op677_scaler @ 004c38d0 (91 bytes) =====

void i960_op677_scaler(void)

{
  double dVar1;
  uint uVar2;
  double *pdVar3;
  float10 fVar4;
  
  uVar2 = *g_i960_cur_insn;
  if ((uVar2 & 0x1000) == 0) {
    dVar1 = (double)*(float *)g_i960_cur_insn[3];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  pdVar3 = (double *)g_i960_cur_insn[4];
  fVar4 = (float10)FUN_005006a0();
  if ((uVar2 & 0x2000) != 0) {
    *pdVar3 = (double)(fVar4 * (float10)dVar1);
    return;
  }
  *(float *)pdVar3 = (float)(fVar4 * (float10)dVar1);
  return;
}


// ===== i960_op699_exprl @ 004c3930 (62 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op699_exprl(void)

{
  double *pdVar1;
  float10 fVar2;
  
  pdVar1 = *(double **)(g_i960_cur_insn + 0x10);
  fVar2 = (float10)FUN_005006a0();
  *pdVar1 = (double)(fVar2 - (float10)_DAT_0051ce30);
  return;
}


// ===== i960_op689_expr @ 004c3970 (81 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op689_expr(void)

{
  uint uVar1;
  double *pdVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  pdVar2 = (double *)g_i960_cur_insn[4];
  fVar3 = (float10)FUN_005006a0();
  if ((uVar1 & 0x2000) != 0) {
    *pdVar2 = (double)(fVar3 - (float10)_DAT_0051ce30);
    return;
  }
  *(float *)pdVar2 = (float)(fVar3 - (float10)_DAT_0051ce30);
  return;
}


// ===== i960_op69b_roundrl @ 004c39d0 (151 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op69b_roundrl(void)

{
  double dVar1;
  double *pdVar2;
  int iVar3;
  int local_44;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    dVar1 = *(double *)g_i960_cur_insn[2];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[2];
  }
  pdVar2 = (double *)g_i960_cur_insn[4];
  switch(*(uint *)(g_i960_pc_ptr + 0xac) >> 0x1e) {
  case 0:
    dVar1 = dVar1 + _DAT_0051cf50;
  case 1:
    FUN_00500530(dVar1);
  case 3:
    iVar3 = FUN_005008f0();
    *pdVar2 = (double)iVar3;
    return;
  case 2:
    FUN_004905b0(dVar1);
    iVar3 = FUN_005008f0();
    *pdVar2 = (double)iVar3;
    return;
  default:
    *pdVar2 = (double)local_44;
    return;
  }
}


// ===== i960_op68b_roundr @ 004c3a80 (159 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op68b_roundr(void)

{
  double dVar1;
  double *pdVar2;
  uint *puVar3;
  int local_44;
  
  puVar3 = g_i960_cur_insn;
  if ((*g_i960_cur_insn & 0x800) == 0) {
    dVar1 = (double)*(float *)g_i960_cur_insn[2];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[2];
  }
  pdVar2 = (double *)g_i960_cur_insn[4];
  switch(*(uint *)(g_i960_pc_ptr + 0xac) >> 0x1e) {
  case 0:
    dVar1 = dVar1 + _DAT_0051cf50;
  case 1:
    FUN_00500530(dVar1);
  case 3:
    local_44 = FUN_005008f0();
    break;
  case 2:
    FUN_004905b0(dVar1);
    local_44 = FUN_005008f0();
    puVar3 = g_i960_cur_insn;
  }
  if ((*puVar3 & 0x2000) == 0) {
    *(float *)pdVar2 = (float)local_44;
    return;
  }
  *pdVar2 = (double)local_44;
  return;
}


// ===== i960_op690_atanrl @ 004c3b30 (62 bytes) =====

void i960_op690_atanrl(void)

{
  float10 fVar1;
  
  if ((*g_i960_cur_insn & 0x1000) != 0) {
    fVar1 = (float10)fpatan((float10)*(double *)g_i960_cur_insn[3],
                            (float10)*(double *)g_i960_cur_insn[2]);
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  fVar1 = (float10)fpatan((float10)*(double *)g_i960_cur_insn[3],
                          (float10)*(double *)g_i960_cur_insn[2]);
  *(double *)g_i960_cur_insn[4] = (double)fVar1;
  return;
}


// ===== i960_op680_atanr @ 004c3b70 (74 bytes) =====

void i960_op680_atanr(void)

{
  uint uVar1;
  float10 fVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  if ((uVar1 & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  if ((uVar1 & 0x1000) == 0) {
    fVar3 = (float10)*(float *)g_i960_cur_insn[3];
  }
  else {
    fVar3 = (float10)*(double *)g_i960_cur_insn[3];
  }
  fVar2 = (float10)fpatan(fVar3,fVar2);
  if ((uVar1 & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar2;
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)fVar2;
  return;
}


// ===== i960_op69c_sinrl @ 004c3bc0 (51 bytes) =====

void i960_op69c_sinrl(void)

{
  float10 fVar1;
  
  if ((*g_i960_cur_insn & 0x800) != 0) {
    fVar1 = (float10)fsin((float10)*(double *)g_i960_cur_insn[2]);
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  fVar1 = (float10)fsin((float10)*(double *)g_i960_cur_insn[2]);
  *(double *)g_i960_cur_insn[4] = (double)fVar1;
  return;
}


// ===== i960_op68c_sinr @ 004c3c00 (55 bytes) =====

void i960_op68c_sinr(void)

{
  float10 fVar1;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    fVar1 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar1 = (float10)*(double *)g_i960_cur_insn[2];
  }
  fVar1 = (float10)fsin(fVar1);
  if ((*g_i960_cur_insn & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)fVar1;
  return;
}


// ===== i960_op69d_cosrl @ 004c3c40 (51 bytes) =====

void i960_op69d_cosrl(void)

{
  float10 fVar1;
  
  if ((*g_i960_cur_insn & 0x800) != 0) {
    fVar1 = (float10)fcos((float10)*(double *)g_i960_cur_insn[2]);
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  fVar1 = (float10)fcos((float10)*(double *)g_i960_cur_insn[2]);
  *(double *)g_i960_cur_insn[4] = (double)fVar1;
  return;
}


// ===== i960_op68d_cosr @ 004c3c80 (55 bytes) =====

void i960_op68d_cosr(void)

{
  float10 fVar1;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    fVar1 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar1 = (float10)*(double *)g_i960_cur_insn[2];
  }
  fVar1 = (float10)fcos(fVar1);
  if ((*g_i960_cur_insn & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)fVar1;
  return;
}


// ===== i960_op69e_tanrl @ 004c3cc0 (55 bytes) =====

void i960_op69e_tanrl(void)

{
  float10 fVar1;
  
  if ((*g_i960_cur_insn & 0x800) != 0) {
    fVar1 = (float10)fptan((float10)*(double *)g_i960_cur_insn[2]);
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  fVar1 = (float10)fptan((float10)*(double *)g_i960_cur_insn[2]);
  *(double *)g_i960_cur_insn[4] = (double)fVar1;
  return;
}


// ===== i960_op68e_tanr @ 004c3d00 (57 bytes) =====

void i960_op68e_tanr(void)

{
  float10 fVar1;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    fVar1 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar1 = (float10)*(double *)g_i960_cur_insn[2];
  }
  fVar1 = (float10)fptan(fVar1);
  if ((*g_i960_cur_insn & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar1;
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)fVar1;
  return;
}


// ===== i960_op698_sqrtrl @ 004c3d40 (51 bytes) =====

void i960_op698_sqrtrl(void)

{
  if ((*g_i960_cur_insn & 0x800) != 0) {
    *(double *)g_i960_cur_insn[4] = SQRT(*(double *)g_i960_cur_insn[2]);
    return;
  }
  *(double *)g_i960_cur_insn[4] = SQRT(*(double *)g_i960_cur_insn[2]);
  return;
}


// ===== i960_op688_sqrtr @ 004c3d80 (55 bytes) =====

void i960_op688_sqrtr(void)

{
  float fVar1;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    fVar1 = *(float *)g_i960_cur_insn[2];
  }
  else {
    fVar1 = (float)*(double *)g_i960_cur_insn[2];
  }
  if ((*g_i960_cur_insn & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)SQRT(fVar1);
    return;
  }
  *(float *)g_i960_cur_insn[4] = SQRT(fVar1);
  return;
}


// ===== i960_op692_logrl @ 004c3dc0 (63 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op692_logrl(void)

{
  double dVar1;
  float10 fVar2;
  
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  fVar2 = (float10)log2((float10)*(double *)g_i960_cur_insn[2]);
  *(double *)g_i960_cur_insn[4] =
       (double)(((float10)0.6931471805599453 * fVar2 * (float10)dVar1) / (float10)_DAT_005aeaf8);
  return;
}


// ===== i960_op682_logr @ 004c3e00 (86 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op682_logr(void)

{
  uint uVar1;
  float10 fVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  if ((uVar1 & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  if ((uVar1 & 0x1000) == 0) {
    fVar3 = (float10)*(float *)g_i960_cur_insn[3];
  }
  else {
    fVar3 = (float10)*(double *)g_i960_cur_insn[3];
  }
  fVar2 = (float10)log2(fVar2);
  fVar2 = ((float10)0.6931471805599453 * fVar2 * fVar3) / (float10)_DAT_005aeaf8;
  if ((uVar1 & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar2;
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)fVar2;
  return;
}


// ===== i960_op691_logeprl @ 004c3e60 (69 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op691_logeprl(void)

{
  double dVar1;
  float10 fVar2;
  
  if ((*g_i960_cur_insn & 0x1000) == 0) {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[3];
  }
  fVar2 = (float10)log2((float10)*(double *)g_i960_cur_insn[2] + (float10)_DAT_0051ce30);
  *(double *)g_i960_cur_insn[4] =
       (double)(((float10)0.6931471805599453 * fVar2 * (float10)dVar1) / (float10)_DAT_005aeaf8);
  return;
}


// ===== i960_op681_logepr @ 004c3eb0 (92 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op681_logepr(void)

{
  uint uVar1;
  float10 fVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  if ((uVar1 & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  if ((uVar1 & 0x1000) == 0) {
    fVar3 = (float10)*(float *)g_i960_cur_insn[3];
  }
  else {
    fVar3 = (float10)*(double *)g_i960_cur_insn[3];
  }
  fVar2 = (float10)log2(fVar2 + (float10)_DAT_0051ce30);
  fVar2 = ((float10)0.6931471805599453 * fVar2 * fVar3) / (float10)_DAT_005aeaf8;
  if ((uVar1 & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar2;
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)fVar2;
  return;
}


// ===== i960_op68a_logbnr @ 004c3f10 (73 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op68a_logbnr(void)

{
  float10 fVar1;
  float10 fVar2;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    fVar2 = (float10)*(float *)g_i960_cur_insn[2];
  }
  else {
    fVar2 = (float10)*(double *)g_i960_cur_insn[2];
  }
  fVar2 = (float10)log2(fVar2);
  fVar1 = (float10)log2((float10)_DAT_0051ce68);
  fVar2 = ((float10)0.6931471805599453 * fVar2) / ((float10)0.6931471805599453 * fVar1);
  if ((*g_i960_cur_insn & 0x2000) != 0) {
    *(double *)g_i960_cur_insn[4] = (double)fVar2;
    return;
  }
  *(float *)g_i960_cur_insn[4] = (float)fVar2;
  return;
}


// ===== i960_op6c0_cvtri @ 004c3f60 (129 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void i960_op6c0_cvtri(void)

{
  double dVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  
  if ((*g_i960_cur_insn & 0x800) == 0) {
    dVar1 = (double)*(float *)g_i960_cur_insn[2];
  }
  else {
    dVar1 = *(double *)g_i960_cur_insn[2];
  }
  puVar2 = (undefined4 *)g_i960_cur_insn[4];
  switch(*(uint *)(g_i960_pc_ptr + 0xac) >> 0x1e) {
  case 0:
    dVar1 = dVar1 + _DAT_0051cf50;
  case 1:
    FUN_00500530(dVar1);
  case 3:
    uVar3 = FUN_005008f0();
    *puVar2 = uVar3;
    return;
  case 2:
    FUN_004905b0(dVar1);
    uVar3 = FUN_005008f0();
    *puVar2 = uVar3;
    return;
  default:
    return;
  }
}


// ===== i960_op693_remrl @ 004c4000 (72 bytes) =====

void i960_op693_remrl(void)

{
  double *pdVar1;
  float10 fVar2;
  
  if ((*g_i960_cur_insn & 0x1000) != 0) {
    pdVar1 = (double *)g_i960_cur_insn[4];
    fVar2 = (float10)FUN_005009aa();
    *pdVar1 = (double)fVar2;
    return;
  }
  pdVar1 = (double *)g_i960_cur_insn[4];
  fVar2 = (float10)FUN_005009aa();
  *pdVar1 = (double)fVar2;
  return;
}


// ===== i960_op683_remr @ 004c4050 (89 bytes) =====

void i960_op683_remr(void)

{
  uint uVar1;
  double *pdVar2;
  float10 fVar3;
  
  uVar1 = *g_i960_cur_insn;
  pdVar2 = (double *)g_i960_cur_insn[4];
  fVar3 = (float10)FUN_005009aa();
  if ((uVar1 & 0x2000) != 0) {
    *pdVar2 = (double)fVar3;
    return;
  }
  *(float *)pdVar2 = (float)fVar3;
  return;
}


// ===== FUN_004c40b0 @ 004c40b0 (190 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004c40b0(int param_1)

{
  double dVar1;
  double *pdVar2;
  int iVar3;
  double local_18;
  undefined4 local_10;
  
  pdVar2 = *(double **)(param_1 + 0xc);
  if (*(double **)(param_1 + 8) <= pdVar2) {
    pdVar2 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar2 + 1) == 3) {
LAB_004c40fe:
    dVar1 = *pdVar2;
  }
  else {
    if (*(int *)(pdVar2 + 1) == 4) {
      iVar3 = FUN_004dbe20();
      if (iVar3 != 0) {
        local_10 = 3;
        pdVar2 = &local_18;
        goto LAB_004c40fe;
      }
    }
    dVar1 = 0.0;
  }
  local_18._0_4_ = (uint)(longlong)ROUND(dVar1);
  iVar3 = *(int *)((local_18._0_4_ >> 0x10) * 0x10 + g_i960_ctx);
  if (iVar3 == 0) {
    iVar3 = 0;
  }
  else {
    iVar3 = local_18._0_4_ + iVar3;
  }
  pdVar2 = *(double **)(param_1 + 8);
  dVar1 = (double)iVar3;
  if (iVar3 < 0) {
    dVar1 = dVar1 + _DAT_0051ce40;
  }
  *pdVar2 = dVar1;
  *(undefined4 *)(pdVar2 + 1) = 3;
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 0x10;
  return 1;
}


// ===== FUN_004c4170 @ 004c4170 (292 bytes) =====

undefined4 __fastcall FUN_004c4170(int param_1)

{
  double dVar1;
  uint uVar2;
  double dVar3;
  double *pdVar4;
  int iVar5;
  int *piVar6;
  double local_18;
  undefined4 local_10;
  
  pdVar4 = *(double **)(param_1 + 0xc);
  if (*(double **)(param_1 + 8) <= pdVar4) {
    pdVar4 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar4 + 1) == 3) {
LAB_004c41c7:
    dVar1 = *pdVar4;
  }
  else {
    if (*(int *)(pdVar4 + 1) == 4) {
      iVar5 = FUN_004dbe20();
      if (iVar5 != 0) {
        local_10 = 3;
        pdVar4 = &local_18;
        goto LAB_004c41c7;
      }
    }
    dVar1 = 0.0;
  }
  pdVar4 = (double *)(*(int *)(param_1 + 0xc) + 0x10);
  local_18 = (double)(longlong)ROUND(dVar1);
  dVar3 = local_18;
  local_18._0_4_ = (uint)(longlong)ROUND(dVar1);
  uVar2 = local_18._0_4_;
  if (*(double **)(param_1 + 8) <= pdVar4) {
    pdVar4 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar4 + 1) == 3) {
LAB_004c422e:
    dVar1 = *pdVar4;
  }
  else {
    if (*(int *)(pdVar4 + 1) == 4) {
      local_18 = dVar3;
      iVar5 = FUN_004dbe20();
      if (iVar5 != 0) {
        local_10 = 3;
        pdVar4 = &local_18;
        goto LAB_004c422e;
      }
    }
    dVar1 = 0.0;
  }
  piVar6 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  local_18 = (double)(longlong)ROUND(dVar1);
  iVar5 = *piVar6;
  if (iVar5 != 0) {
    *(uint *)(iVar5 + uVar2) = local_18._0_4_;
    return 0;
  }
  (*(code *)piVar6[3])();
  return 0;
}


// ===== FUN_004c42a0 @ 004c42a0 (297 bytes) =====

undefined4 __fastcall FUN_004c42a0(int param_1)

{
  double dVar1;
  uint uVar2;
  double dVar3;
  double *pdVar4;
  int iVar5;
  int *piVar6;
  double local_18;
  undefined4 local_10;
  
  pdVar4 = *(double **)(param_1 + 0xc);
  if (*(double **)(param_1 + 8) <= pdVar4) {
    pdVar4 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar4 + 1) == 3) {
LAB_004c42f7:
    dVar1 = *pdVar4;
  }
  else {
    if (*(int *)(pdVar4 + 1) == 4) {
      iVar5 = FUN_004dbe20();
      if (iVar5 != 0) {
        local_10 = 3;
        pdVar4 = &local_18;
        goto LAB_004c42f7;
      }
    }
    dVar1 = 0.0;
  }
  pdVar4 = (double *)(*(int *)(param_1 + 0xc) + 0x10);
  local_18 = (double)(longlong)ROUND(dVar1);
  dVar3 = local_18;
  local_18._0_4_ = (uint)(longlong)ROUND(dVar1);
  uVar2 = local_18._0_4_;
  if (*(double **)(param_1 + 8) <= pdVar4) {
    pdVar4 = (double *)&DAT_00511b28;
  }
  local_18 = dVar3;
  if (*(int *)(pdVar4 + 1) == 3) {
LAB_004c435e:
    dVar1 = *pdVar4;
  }
  else {
    if (*(int *)(pdVar4 + 1) == 4) {
      iVar5 = FUN_004dbe20();
      if (iVar5 != 0) {
        local_10 = 3;
        pdVar4 = &local_18;
        goto LAB_004c435e;
      }
    }
    dVar1 = 0.0;
  }
  local_18 = (double)CONCAT44(local_18._4_4_,(int)ROUND(dVar1));
  dVar3 = local_18;
  local_18._0_2_ = (undefined2)(int)ROUND(dVar1);
  piVar6 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  iVar5 = *piVar6;
  if (iVar5 != 0) {
    *(undefined2 *)(iVar5 + uVar2) = local_18._0_2_;
    return 0;
  }
  local_18 = dVar3;
  (*(code *)piVar6[2])();
  return 0;
}


// ===== FUN_004c43d0 @ 004c43d0 (292 bytes) =====

undefined4 __fastcall FUN_004c43d0(int param_1)

{
  double dVar1;
  uint uVar2;
  double dVar3;
  double *pdVar4;
  int iVar5;
  int *piVar6;
  double local_18;
  undefined4 local_10;
  
  pdVar4 = *(double **)(param_1 + 0xc);
  if (*(double **)(param_1 + 8) <= pdVar4) {
    pdVar4 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar4 + 1) == 3) {
LAB_004c4427:
    dVar1 = *pdVar4;
  }
  else {
    if (*(int *)(pdVar4 + 1) == 4) {
      iVar5 = FUN_004dbe20();
      if (iVar5 != 0) {
        local_10 = 3;
        pdVar4 = &local_18;
        goto LAB_004c4427;
      }
    }
    dVar1 = 0.0;
  }
  pdVar4 = (double *)(*(int *)(param_1 + 0xc) + 0x10);
  local_18 = (double)(longlong)ROUND(dVar1);
  dVar3 = local_18;
  local_18._0_4_ = (uint)(longlong)ROUND(dVar1);
  uVar2 = local_18._0_4_;
  if (*(double **)(param_1 + 8) <= pdVar4) {
    pdVar4 = (double *)&DAT_00511b28;
  }
  local_18 = dVar3;
  if (*(int *)(pdVar4 + 1) == 3) {
LAB_004c448e:
    dVar1 = *pdVar4;
  }
  else {
    if (*(int *)(pdVar4 + 1) == 4) {
      iVar5 = FUN_004dbe20();
      if (iVar5 != 0) {
        local_10 = 3;
        pdVar4 = &local_18;
        goto LAB_004c448e;
      }
    }
    dVar1 = 0.0;
  }
  piVar6 = (int *)(((uVar2 >> 0x10) + 0x10000) * 0x10 + g_i960_ctx);
  local_18 = (double)CONCAT44(local_18._4_4_,(int)ROUND(dVar1));
  dVar3 = local_18;
  iVar5 = *piVar6;
  local_18._0_1_ = (undefined1)(int)ROUND(dVar1);
  if (iVar5 != 0) {
    *(undefined1 *)(iVar5 + uVar2) = local_18._0_1_;
    return 0;
  }
  local_18 = dVar3;
  (*(code *)piVar6[1])();
  return 0;
}


// ===== FUN_004c4500 @ 004c4500 (194 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004c4500(int param_1)

{
  double dVar1;
  double *pdVar2;
  int iVar3;
  int *piVar4;
  double local_18;
  undefined4 local_10;
  
  pdVar2 = *(double **)(param_1 + 0xc);
  if (*(double **)(param_1 + 8) <= pdVar2) {
    pdVar2 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar2 + 1) == 3) {
LAB_004c454e:
    dVar1 = *pdVar2;
  }
  else {
    if (*(int *)(pdVar2 + 1) == 4) {
      iVar3 = FUN_004dbe20();
      if (iVar3 != 0) {
        local_10 = 3;
        pdVar2 = &local_18;
        goto LAB_004c454e;
      }
    }
    dVar1 = 0.0;
  }
  local_18 = (double)(longlong)ROUND(dVar1);
  piVar4 = (int *)((local_18._0_4_ >> 0x10) * 0x10 + g_i960_ctx);
  iVar3 = *piVar4;
  if (iVar3 == 0) {
    iVar3 = (*(code *)piVar4[3])();
  }
  else {
    iVar3 = *(int *)(iVar3 + local_18._0_4_);
  }
  pdVar2 = *(double **)(param_1 + 8);
  dVar1 = (double)iVar3;
  if (iVar3 < 0) {
    dVar1 = dVar1 + _DAT_0051ce40;
  }
  *pdVar2 = dVar1;
  *(undefined4 *)(pdVar2 + 1) = 3;
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 0x10;
  return 1;
}


// ===== FUN_004c45d0 @ 004c45d0 (191 bytes) =====

undefined4 __fastcall FUN_004c45d0(int param_1)

{
  double dVar1;
  ushort uVar2;
  double *pdVar3;
  int iVar4;
  int *piVar5;
  double local_18;
  undefined4 local_10;
  
  pdVar3 = *(double **)(param_1 + 0xc);
  if (*(double **)(param_1 + 8) <= pdVar3) {
    pdVar3 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar3 + 1) == 3) {
LAB_004c461e:
    dVar1 = *pdVar3;
  }
  else {
    if (*(int *)(pdVar3 + 1) == 4) {
      iVar4 = FUN_004dbe20();
      if (iVar4 != 0) {
        local_10 = 3;
        pdVar3 = &local_18;
        goto LAB_004c461e;
      }
    }
    dVar1 = 0.0;
  }
  local_18 = (double)(longlong)ROUND(dVar1);
  piVar5 = (int *)((local_18._0_4_ >> 0x10) * 0x10 + g_i960_ctx);
  iVar4 = *piVar5;
  if (iVar4 == 0) {
    uVar2 = (*(code *)piVar5[2])();
  }
  else {
    uVar2 = *(ushort *)(iVar4 + local_18._0_4_);
  }
  pdVar3 = *(double **)(param_1 + 8);
  *(undefined4 *)(pdVar3 + 1) = 3;
  *pdVar3 = (double)uVar2;
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 0x10;
  return 1;
}


// ===== FUN_004c4690 @ 004c4690 (187 bytes) =====

undefined4 __fastcall FUN_004c4690(int param_1)

{
  double dVar1;
  byte bVar2;
  double *pdVar3;
  int iVar4;
  int *piVar5;
  double local_18;
  undefined4 local_10;
  
  pdVar3 = *(double **)(param_1 + 0xc);
  if (*(double **)(param_1 + 8) <= pdVar3) {
    pdVar3 = (double *)&DAT_00511b28;
  }
  if (*(int *)(pdVar3 + 1) == 3) {
LAB_004c46de:
    dVar1 = *pdVar3;
  }
  else {
    if (*(int *)(pdVar3 + 1) == 4) {
      iVar4 = FUN_004dbe20();
      if (iVar4 != 0) {
        local_10 = 3;
        pdVar3 = &local_18;
        goto LAB_004c46de;
      }
    }
    dVar1 = 0.0;
  }
  local_18 = (double)(longlong)ROUND(dVar1);
  piVar5 = (int *)((local_18._0_4_ >> 0x10) * 0x10 + g_i960_ctx);
  iVar4 = *piVar5;
  if (iVar4 == 0) {
    bVar2 = (*(code *)piVar5[1])();
  }
  else {
    bVar2 = *(byte *)(iVar4 + local_18._0_4_);
  }
  pdVar3 = *(double **)(param_1 + 8);
  *(undefined4 *)(pdVar3 + 1) = 3;
  *pdVar3 = (double)bVar2;
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 0x10;
  return 1;
}


// ===== i960_decode_insn @ 004c4750 (267 bytes) =====

void i960_decode_insn(int param_1)

{
  int iVar1;
  uint *puVar2;
  int *piVar3;
  uint uVar4;
  uint uVar5;
  uint unaff_EBX;
  
  piVar3 = (int *)((unaff_EBX >> 0x10) * 0x10 + g_i960_ctx);
  iVar1 = *piVar3;
  if (iVar1 == 0) {
    uVar4 = (*(code *)piVar3[3])();
  }
  else {
    uVar4 = *(uint *)(iVar1 + unaff_EBX);
  }
  puVar2 = g_i960_cur_insn;
  uVar5 = uVar4 >> 0x18;
  *g_i960_cur_insn = uVar4;
  if (uVar5 - 0x58 < 0x22) {
    uVar5 = uVar5 << 4 | uVar4 >> 7 & 0xf;
  }
  uVar5 = (&g_i960_optable)[uVar5];
  puVar2[6] = uVar5;
  switch(*(undefined4 *)(uVar5 + 8)) {
  case 0:
    if ((*(byte *)(uVar5 + 0x20) & 1) != 0) {
      FUN_004c2a70();
    }
    if ((*(byte *)(uVar5 + 0x20) & 2) != 0) {
      FUN_004c2990();
    }
    if ((*(byte *)(uVar5 + 0x20) & 4) != 0) {
      FUN_004c2910();
      return;
    }
    break;
  case 1:
    if ((*(byte *)(uVar5 + 0x20) & 4) != 0) {
      puVar2[4] = param_1 + 0x200044 + (uVar4 >> 0x13 & 0x1f) * 4;
    }
    if ((*(byte *)(uVar5 + 0x20) & 8) != 0) {
      FUN_004c36c0();
      return;
    }
    break;
  case 2:
    if ((*(byte *)(uVar5 + 0x20) & 1) != 0) {
      FUN_004c2a30(param_1);
    }
    if ((*(byte *)(uVar5 + 0x20) & 2) != 0) {
      puVar2[3] = param_1 + 0x200044 + (*puVar2 >> 0xe & 0x1f) * 4;
    }
    if ((*(byte *)(uVar5 + 0x20) & 8) != 0) {
      FUN_004c28d0();
      return;
    }
    break;
  case 3:
    if ((*(byte *)(uVar5 + 0x20) & 8) != 0) {
      FUN_004c28b0();
    }
  }
  return;
}


// ===== FUN_004c4870 @ 004c4870 (123 bytes) =====

void FUN_004c4870(int param_1,char param_2,uint param_3,int param_4)

{
  int iVar1;
  void *_Dst;
  int iVar2;
  
  _Dst = (void *)FUN_004fc340();
  iVar1 = param_1 + param_2 * 0x14;
  *(void **)(iVar1 + 0x200000) = _Dst;
  iVar2 = *(int *)(param_1 + (param_3 >> 0x10) * 0x10);
  if (iVar2 == 0) {
    iVar2 = 0;
  }
  else {
    iVar2 = iVar2 + param_3;
  }
  *(int *)(iVar1 + 0x200004) = iVar2;
  _memset(_Dst,0,param_4 * 0x30);
  *(int *)(iVar1 + 0x20000c) = param_4;
  *(uint *)(iVar1 + 0x200010) = param_3;
  *(undefined1 *)(param_1 + (param_2 * 5 + 0x80002) * 4) = 0;
  return;
}


// ===== i960_predecode_region @ 004c48f0 (415 bytes) =====

void i960_predecode_region(int param_1)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  int *piVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  int *unaff_EBX;
  int iVar9;
  int local_8;
  
  uVar1 = unaff_EBX[3];
  iVar9 = 0;
  if (uVar1 >> 2 != 0) {
    local_8 = 0;
    do {
      g_i960_cur_insn = (uint *)(*unaff_EBX + local_8);
      uVar5 = unaff_EBX[4] + iVar9 * 4;
      piVar4 = (int *)((uVar5 >> 0x10) * 0x10 + g_i960_ctx);
      iVar2 = *piVar4;
      if (iVar2 == 0) {
        uVar5 = (*(code *)piVar4[3])();
      }
      else {
        uVar5 = *(uint *)(iVar2 + uVar5);
      }
      puVar3 = g_i960_cur_insn;
      uVar6 = uVar5 >> 0x18;
      *g_i960_cur_insn = uVar5;
      if (uVar6 - 0x58 < 0x22) {
        uVar6 = uVar6 << 4 | uVar5 >> 7 & 0xf;
      }
      uVar6 = (&g_i960_optable)[uVar6];
      puVar3[6] = uVar6;
      switch(*(undefined4 *)(uVar6 + 8)) {
      case 0:
        if ((*(byte *)(uVar6 + 0x20) & 1) != 0) {
          FUN_004c2a70();
        }
        if ((*(byte *)(uVar6 + 0x20) & 2) != 0) {
          FUN_004c2990();
        }
        if ((*(byte *)(uVar6 + 0x20) & 4) != 0) {
          FUN_004c2910();
        }
        break;
      case 1:
        if ((*(byte *)(uVar6 + 0x20) & 4) != 0) {
          puVar3[4] = param_1 + 0x200044 + (uVar5 >> 0x13 & 0x1f) * 4;
        }
        if ((*(byte *)(uVar6 + 0x20) & 8) != 0) {
          FUN_004c36c0();
        }
        break;
      case 2:
        if ((*(byte *)(uVar6 + 0x20) & 1) != 0) {
          uVar7 = uVar5 >> 0x13 & 0x1f;
          if ((uVar5 & 0x2000) == 0) {
            puVar8 = (undefined4 *)(param_1 + 0x200044 + uVar7 * 4);
          }
          else {
            puVar8 = &g_i960_literals + uVar7 * 4;
          }
          puVar3[2] = (uint)puVar8;
        }
        if ((*(byte *)(uVar6 + 0x20) & 2) != 0) {
          puVar3[3] = param_1 + 0x200044 + (uVar5 >> 0xe & 0x1f) * 4;
        }
        if ((*(byte *)(uVar6 + 0x20) & 8) != 0) {
          uVar6 = uVar5 & 0x1fff;
          if ((uVar5 & 0x1000) != 0) {
            uVar6 = uVar6 | 0xfffff000;
          }
LAB_004c4a6c:
          puVar3[1] = uVar6 - 4;
        }
        break;
      case 3:
        if ((*(byte *)(uVar6 + 0x20) & 8) != 0) {
          uVar6 = uVar5 & 0xffffff;
          if ((uVar5 & 0x800000) != 0) {
            uVar6 = uVar6 | 0xff000000;
          }
          goto LAB_004c4a6c;
        }
      }
      local_8 = local_8 + 0x30;
      iVar9 = iVar9 + 1;
    } while (iVar9 < (int)(uVar1 >> 2));
  }
  *(undefined1 *)(unaff_EBX + 2) = 1;
  return;
}


// ===== FUN_004c4aa0 @ 004c4aa0 (96 bytes) =====

void FUN_004c4aa0(void)

{
  int in_EAX;
  uint uVar1;
  
  uVar1 = *(uint *)(in_EAX + 0x200040) >> 0x14;
  if (uVar1 < *(uint *)(in_EAX + 0x200024) >> 0x14) {
    *(int *)(in_EAX + 0x20003c) = in_EAX + 0x200000;
  }
  else if (uVar1 < *(uint *)(in_EAX + 0x200038) >> 0x14) {
    *(int *)(in_EAX + 0x20003c) = in_EAX + 0x200014;
  }
  else {
    *(int *)(in_EAX + 0x20003c) = in_EAX + 0x200028;
  }
  if (*(char *)(*(int *)(in_EAX + 0x20003c) + 8) == '\0') {
    i960_predecode_region(in_EAX);
  }
  return;
}


// ===== i960_execute @ 004c4b00 (339 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int i960_execute(int param_1)

{
  uint *puVar1;
  int *piVar2;
  uint uVar3;
  int unaff_ESI;
  
  puVar1 = (uint *)(unaff_ESI + 0x200040);
  g_i960_pc_ptr = puVar1;
  g_i960_ctx = unaff_ESI;
  *(int *)(unaff_ESI + 0x202350) = param_1;
  if (0 < param_1) {
    while (*(int *)(unaff_ESI + 0x202354) == 0) {
      piVar2 = *(int **)(unaff_ESI + 0x20003c);
      uVar3 = *puVar1 - piVar2[4] >> 2;
      g_i960_cur_insn = (int *)(uVar3 * 0x30 + *piVar2);
      if (*g_i960_cur_insn != *(int *)(piVar2[1] + uVar3 * 4)) {
        i960_decode_insn(unaff_ESI);
      }
      piVar2 = g_i960_cur_insn;
      *puVar1 = *puVar1 + 4;
      *(undefined1 *)(unaff_ESI + 0x200140) = 0;
      (**(code **)piVar2[6])();
      _g_i960_pc_hist3 = g_i960_pc_hist1;
      g_i960_pc_hist1 = g_i960_pc_hist2;
      g_i960_pc_hist2 = g_i960_pc_hist0;
      if (((*puVar1 ^ g_i960_pc_hist0) & 0xffe00000) != 0) {
        uVar3 = *puVar1 >> 0x14;
        if (uVar3 < *(uint *)(unaff_ESI + 0x200024) >> 0x14) {
          *(int *)(unaff_ESI + 0x20003c) = unaff_ESI + 0x200000;
        }
        else if (uVar3 < *(uint *)(unaff_ESI + 0x200038) >> 0x14) {
          *(int *)(unaff_ESI + 0x20003c) = unaff_ESI + 0x200014;
        }
        else {
          *(int *)(unaff_ESI + 0x20003c) = unaff_ESI + 0x200028;
        }
        if (*(char *)(*(int *)(unaff_ESI + 0x20003c) + 8) == '\0') {
          i960_predecode_region(unaff_ESI);
        }
      }
      g_i960_pc_hist0 = *puVar1;
      *(int *)(unaff_ESI + 0x202350) =
           *(int *)(unaff_ESI + 0x202350) - *(int *)(g_i960_cur_insn[6] + 0x10);
      if (*(int *)(unaff_ESI + 0x202350) < 1) {
        return param_1 - *(int *)(unaff_ESI + 0x202350);
      }
    }
    *(undefined4 *)(unaff_ESI + 0x202350) = 0;
  }
  return param_1 - *(int *)(unaff_ESI + 0x202350);
}


// ===== FUN_004c4c60 @ 004c4c60 (693 bytes) =====

void FUN_004c4c60(int param_1,char param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  undefined4 *puVar9;
  uint uVar10;
  undefined4 *puVar11;
  int *piVar12;
  int local_8;
  
  uVar4 = *(int *)(param_1 + 0x2000f8) + 0x14;
  uVar6 = uVar4 >> 0x10;
  iVar3 = *(int *)(g_i960_ctx + uVar6 * 0x10);
  if (iVar3 == 0) {
    local_8 = (**(code **)(g_i960_ctx + 0xc + uVar6 * 0x10))();
  }
  else {
    local_8 = *(int *)(iVar3 + uVar4);
  }
  iVar3 = g_i960_ctx;
  uVar6 = *(uint *)(param_1 + 0x2000fc) >> (param_2 * '\b' & 0x1fU);
  uVar4 = uVar6 & 0xff;
  if (((char)uVar6 != '\0') && ((*(ushort *)(param_1 + 0x2000f2) & 0x1f) <= uVar4 >> 3)) {
    uVar6 = *(uint *)(param_1 + 0x2000c0);
    *(undefined4 *)(param_1 + 0x20004c) = *(undefined4 *)(param_1 + 0x200040);
    if (*(int *)(param_1 + 0x20014c) == 0x80) {
      uVar10 = *(uint *)(iVar3 + 0x200150 + *(int *)(iVar3 + 0x200148) * 4);
      uVar7 = 0;
      do {
        iVar8 = (uVar10 >> 0x10) + 0x10000;
        iVar5 = *(int *)(iVar3 + iVar8 * 0x10);
        if (iVar5 == 0) {
          (**(code **)(iVar3 + 0xc + iVar8 * 0x10))();
          iVar3 = g_i960_ctx;
        }
        else {
          *(undefined4 *)(uVar10 + iVar5) =
               *(undefined4 *)(iVar3 + 0x200350 + (*(int *)(iVar3 + 0x200148) * 0x10 + uVar7) * 4);
        }
        uVar7 = uVar7 + 1;
        uVar10 = uVar10 + 4;
      } while (uVar7 < 0x10);
      *(int *)(iVar3 + 0x20014c) = *(int *)(iVar3 + 0x20014c) + -1;
    }
    puVar1 = g_i960_pc_ptr;
    puVar11 = (undefined4 *)(*(int *)(iVar3 + 0x200148) * 0x40 + 0x200350 + iVar3);
    puVar9 = g_i960_pc_ptr;
    for (iVar5 = 0x10; puVar9 = puVar9 + 1, iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar11 = *puVar9;
      puVar11 = puVar11 + 1;
    }
    *(uint *)(iVar3 + 0x200150 + *(int *)(iVar3 + 0x200148) * 4) = puVar1[0x20] & 0xffffffc0;
    uVar10 = *(int *)(iVar3 + 0x200148) + 1U & 0x8000007f;
    if ((int)uVar10 < 0) {
      uVar10 = (uVar10 - 1 | 0xffffff80) + 1;
    }
    *(int *)(iVar3 + 0x20014c) = *(int *)(iVar3 + 0x20014c) + 1;
    *(uint *)(iVar3 + 0x200148) = uVar10;
    uVar10 = local_8 + 4 + uVar4 * 4;
    uVar7 = uVar10 >> 0x10;
    iVar5 = *(int *)(iVar3 + uVar7 * 0x10);
    if (iVar5 == 0) {
      uVar2 = (**(code **)(iVar3 + 0xc + uVar7 * 0x10))();
      iVar3 = g_i960_ctx;
    }
    else {
      uVar2 = *(undefined4 *)(iVar5 + uVar10);
    }
    if (((*(uint *)(param_1 + 0x2000f0) & 0x2000) == 0) &&
       (uVar10 = *(int *)(param_1 + 0x2000f8) + 0x18U >> 0x10, *(int *)(iVar3 + uVar10 * 0x10) == 0)
       ) {
      (**(code **)(iVar3 + 0xc + uVar10 * 0x10))();
      iVar3 = g_i960_ctx;
    }
    uVar10 = *(int *)(param_1 + 0x200048) + 0x3fU & 0xffffffc0;
    iVar8 = (uVar10 + 0x30 >> 0x10) + 0x10000;
    iVar5 = *(int *)(iVar3 + iVar8 * 0x10);
    if (iVar5 == 0) {
      (**(code **)(iVar3 + 0xc + iVar8 * 0x10))();
      iVar3 = g_i960_ctx;
    }
    else {
      *(undefined4 *)(iVar5 + 0x30 + uVar10) = *(undefined4 *)(param_1 + 0x2000f0);
    }
    iVar8 = (uVar10 + 0x34 >> 0x10) + 0x10000;
    iVar5 = *(int *)(iVar3 + iVar8 * 0x10);
    if (iVar5 == 0) {
      (**(code **)(iVar3 + 0xc + iVar8 * 0x10))();
      iVar3 = g_i960_ctx;
    }
    else {
      *(uint *)(iVar5 + 0x34 + uVar10) =
           (*(uint *)(param_1 + 0x2000ec) ^ *(uint *)(param_1 + 0x2000e8)) & 7 ^
           *(uint *)(param_1 + 0x2000ec);
    }
    piVar12 = (int *)(((uVar10 + 0x38 >> 0x10) + 0x10000) * 0x10 + iVar3);
    iVar3 = *piVar12;
    if (iVar3 == 0) {
      (*(code *)piVar12[3])();
    }
    else {
      *(uint *)(iVar3 + 0x38 + uVar10) = uVar4;
    }
    *(uint *)(param_1 + 0x2000c0) = uVar10 + 0x40;
    *(uint *)(param_1 + 0x200044) = uVar6 & 0xffffffc0 | 7;
    *(uint *)(param_1 + 0x2000f0) =
         *(uint *)(param_1 + 0x2000f0) & 0xffffe0ff | (uVar4 >> 3) << 0x10 | 0x2002;
    *(uint *)(param_1 + 0x200048) = uVar10 + 0x80;
    *(undefined4 *)(param_1 + 0x200040) = uVar2;
    FUN_004c4aa0();
    *(undefined4 *)(param_1 + 0x202354) = 0;
    *(undefined1 *)(param_1 + 0x202358) = 1;
  }
  return;
}


// ===== FUN_004c4f20 @ 004c4f20 (794 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c4f20(undefined4 param_1,undefined4 *param_2,int *param_3)

{
  float10 fVar1;
  undefined4 uVar2;
  int *in_EAX;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  undefined4 local_20;
  undefined *local_1c;
  undefined4 *local_18;
  undefined4 local_14;
  undefined1 *local_10;
  undefined1 *local_c;
  
  fVar1 = (float10)log2((float10)_DAT_0051ce68);
  _DAT_005aeaf8 = (double)((float10)0.6931471805599453 * fVar1);
  g_i960_ctx = param_2;
  _memset(param_2 + 0x80010,0,0x108);
  i960_build_optable();
  DAT_005aeb04 = param_1;
  uVar3 = FUN_004fc340();
  iVar6 = 0;
  piVar5 = &DAT_005aa8fc;
  DAT_010ed940 = uVar3;
  do {
    piVar5[-1] = iVar6;
    *piVar5 = iVar6;
    piVar5[1] = iVar6;
    piVar5[2] = iVar6;
    uVar2 = DAT_005aeb04;
    piVar5 = piVar5 + 4;
    iVar6 = iVar6 + 1;
  } while ((int)piVar5 < 0x5aaafc);
  param_2[0x80000] = uVar3;
  param_2[0x80001] = *param_2;
  param_2[0x80003] = uVar2;
  param_2[0x80004] = 0;
  i960_predecode_region(param_2);
  *(undefined1 *)(param_2 + 0x80007) = 0;
  *(undefined1 *)(param_2 + 0x8000c) = 0;
  param_2[0x8000f] = param_2 + 0x80000;
  param_2[0x808d5] = 0;
  param_2[0x80052] = 0;
  iVar6 = *param_3;
  local_20 = CONCAT31(local_20._1_3_,1);
  local_1c = &DAT_0051bc90;
  local_14 = 0x108;
  local_10 = &LAB_004c3680;
  local_c = &LAB_004c3640;
  local_18 = param_2;
  iVar4 = FUN_00401050(iVar6,*(undefined4 *)(iVar6 + 4));
  if (param_3[1] == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  param_3[1] = param_3[1] + 1;
  *(int *)(iVar6 + 4) = iVar4;
  **(int **)(iVar4 + 4) = iVar4;
  FUN_004e9860(FUN_004c4690);
  iVar6 = *in_EAX;
  local_20 = FUN_004d65e0(0xd);
  local_18 = (undefined4 *)0x4;
  FUN_004d0250(iVar6,iVar6 + 0x48,&local_20,*(int *)(iVar6 + 8) + -0x10);
  *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + -0x10;
  FUN_004e9860(FUN_004c45d0);
  iVar6 = *in_EAX;
  local_20 = FUN_004d65e0(0xd);
  local_18 = (undefined4 *)0x4;
  FUN_004d0250(iVar6,iVar6 + 0x48,&local_20,*(int *)(iVar6 + 8) + -0x10);
  *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + -0x10;
  FUN_004e9860(FUN_004c4500);
  iVar6 = *in_EAX;
  local_20 = FUN_004d65e0(0xe);
  local_18 = (undefined4 *)0x4;
  FUN_004d0250(iVar6,iVar6 + 0x48,&local_20,*(int *)(iVar6 + 8) + -0x10);
  *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + -0x10;
  FUN_004e9860(FUN_004c43d0);
  iVar6 = *in_EAX;
  local_20 = FUN_004d65e0(0xe);
  local_18 = (undefined4 *)0x4;
  FUN_004d0250(iVar6,iVar6 + 0x48,&local_20,*(int *)(iVar6 + 8) + -0x10);
  *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + -0x10;
  FUN_004e9860(FUN_004c42a0);
  iVar6 = *in_EAX;
  local_20 = FUN_004d65e0(0xe);
  local_18 = (undefined4 *)0x4;
  FUN_004d0250(iVar6,iVar6 + 0x48,&local_20,*(int *)(iVar6 + 8) + -0x10);
  *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + -0x10;
  FUN_004e9860(FUN_004c4170);
  iVar6 = *in_EAX;
  local_20 = FUN_004d65e0(0xf);
  local_18 = (undefined4 *)0x4;
  FUN_004d0250(iVar6,iVar6 + 0x48,&local_20,*(int *)(iVar6 + 8) + -0x10);
  *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + -0x10;
  FUN_004e9860(FUN_004c40b0);
  iVar6 = *in_EAX;
  local_20 = FUN_004d65e0(0xe);
  local_18 = (undefined4 *)0x4;
  FUN_004d0250(iVar6,iVar6 + 0x48,&local_20,*(int *)(iVar6 + 8) + -0x10);
  *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + -0x10;
  return;
}


// ===== FUN_004c5620 @ 004c5620 (219 bytes) =====

void __fastcall FUN_004c5620(uint param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_005aa724;
  if ((param_1 - 0x880000 < 0x4000) && ((param_1 & 0xf) == 0)) {
    uVar2 = param_1 >> 4 & 0xff;
    *(uint *)(DAT_005aa724 + *(int *)(DAT_005aa724 + 0x84) * 4) =
         (uVar2 << 0xf | uVar2) << 8 | uVar2;
    *(int *)(iVar1 + 0x88) = *(int *)(iVar1 + 0x88) + -1;
    *(uint *)(iVar1 + 0x84) = *(int *)(iVar1 + 0x84) + 1U & 0x1f;
    DAT_010ec00e = '\0';
    do {
      FUN_004a81c0();
    } while (DAT_010ec00e == '\0');
    return;
  }
  if ((DAT_010ed8cc & 0x80000000) != 0) {
    *(undefined4 *)(DAT_010ed8d0 + DAT_005aa87c) = param_2;
    DAT_010ed8d0 = DAT_010ed8d0 + 4;
    return;
  }
  if ((DAT_010ed8d0 != 0) && (param_1 - 0x884000 < 0x4000)) {
    *(undefined4 *)(DAT_005aa724 + *(int *)(DAT_005aa724 + 0x84) * 4) = param_2;
    *(int *)(iVar1 + 0x88) = *(int *)(iVar1 + 0x88) + -1;
    *(uint *)(iVar1 + 0x84) = *(int *)(iVar1 + 0x84) + 1U & 0x1f;
    DAT_010ec00e = 0;
    FUN_004a9bc0();
  }
  return;
}


// ===== FUN_004c5730 @ 004c5730 (92 bytes) =====

undefined4 __fastcall FUN_004c5730(int param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  iVar5 = DAT_005aa738;
  if (param_1 - 0x884000U < 0x4000) {
    piVar1 = (int *)(DAT_005aa738 + 0x80);
    uVar4 = *(undefined4 *)(DAT_005aa738 + *piVar1 * 4);
    piVar2 = (int *)(DAT_005aa738 + 0x88);
    *piVar2 = *piVar2 + 1;
    iVar3 = *piVar2;
    *(uint *)(iVar5 + 0x80) = *piVar1 + 1U & 0x1f;
    if (iVar3 != 0) {
      DAT_010ec00e = '\0';
      do {
        FUN_004a81c0();
      } while (DAT_010ec00e == '\0');
    }
    return uVar4;
  }
  return 0;
}


// ===== FUN_004c5790 @ 004c5790 (96 bytes) =====

uint __fastcall FUN_004c5790(int param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  iVar5 = DAT_005aa738;
  if (param_1 - 0x884000U < 0x4000) {
    piVar1 = (int *)(DAT_005aa738 + 0x80);
    uVar4 = *(uint *)(DAT_005aa738 + *piVar1 * 4);
    piVar2 = (int *)(DAT_005aa738 + 0x88);
    *piVar2 = *piVar2 + 1;
    iVar3 = *piVar2;
    *(uint *)(iVar5 + 0x80) = *piVar1 + 1U & 0x1f;
    if (iVar3 != 0) {
      DAT_010ec00e = '\0';
      do {
        FUN_004a81c0();
      } while (DAT_010ec00e == '\0');
    }
    return uVar4 & 0xffff;
  }
  return 0xffff;
}


// ===== FUN_004c57f0 @ 004c57f0 (92 bytes) =====

uint __fastcall FUN_004c57f0(int param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  iVar5 = DAT_005aa738;
  if (param_1 - 0x884000U < 0x4000) {
    piVar1 = (int *)(DAT_005aa738 + 0x80);
    uVar4 = *(uint *)(DAT_005aa738 + *piVar1 * 4);
    piVar2 = (int *)(DAT_005aa738 + 0x88);
    *piVar2 = *piVar2 + 1;
    iVar3 = *piVar2;
    *(uint *)(iVar5 + 0x80) = *piVar1 + 1U & 0x1f;
    if (iVar3 != 0) {
      DAT_010ec00e = '\0';
      do {
        FUN_004a81c0();
      } while (DAT_010ec00e == '\0');
    }
    return uVar4 & 0xff;
  }
  return 0xff;
}


