// ===== FUN_00451a40 @ 00451a40 (73 bytes) =====


undefined * FUN_00451a40(void)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  if (DAT_010ed0bc == -0x41413502) {
    piVar1 = &DAT_010ed0bc;
    do {
      piVar1 = piVar1 + 4;
      iVar2 = iVar2 + 1;
    } while (*piVar1 == -0x41413502);
  }
  (&DAT_010ed0b4)[iVar2 * 4] = "SEGA Model 2";
  (&DAT_010ed0b0)[iVar2 * 4] = "model2";
  (&DAT_010ed0b8)[iVar2 * 4] = &LAB_004cc450;
  (&DAT_010ed0bc)[iVar2 * 4] = 0xbebecafe;
  return &DAT_010edc2b;
}


// ===== FUN_00451a90 @ 00451a90 (1020 bytes) =====


void FUN_00451a90(int param_1,int param_2,uint param_3)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  ushort uVar5;
  ushort uVar6;
  ushort uVar7;
  ushort uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  ushort *puVar14;
  uint uVar15;
  uint uVar16;
  int *unaff_EDI;
  uint local_5c;
  uint local_58;
  uint local_54;
  uint local_50;
  int local_4c;
  uint local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  
  FUN_004c0410();
  iVar9 = unaff_EDI[7];
  uVar1 = *(ushort *)(iVar9 + 0xa008);
  uVar10 = uVar1 & 0x4000;
  uVar2 = *(ushort *)(iVar9 + 0xa000);
  uVar11 = (uVar10 | 0x3fe0) >> 5;
  uVar3 = *(ushort *)(iVar9 + 0xa00c);
  uVar15 = -(uint)uVar2 & uVar11;
  uVar4 = *(ushort *)(iVar9 + 0xa008);
  local_5c = (uint)uVar4;
  uVar12 = uVar3 & 0x4000;
  uVar5 = *(ushort *)(iVar9 + 0xa004);
  uVar13 = (uVar12 | 0x3fe0) >> 5;
  local_3c = -(uint)uVar5 & uVar13;
  uVar6 = *(ushort *)(iVar9 + 0xa00c);
  local_58 = (uint)uVar6;
  uVar7 = *(ushort *)(iVar9 + 0xa002);
  local_54 = (uint)*(ushort *)(iVar9 + 0xa00a);
  local_30 = local_54 & 0x8000;
  uVar8 = *(ushort *)(iVar9 + 0xa006);
  local_38 = -(uint)uVar8 & 0x1ff;
  local_50 = (uint)*(ushort *)(iVar9 + 0xa00e);
  local_34 = local_50 & 0x8000;
  uVar16 = -(uint)uVar7 & 0x1ff;
  if (*(char *)((int)unaff_EDI + 0x11) == '\0') {
    local_34 = 1;
    local_30 = 1;
  }
  uVar12 = uVar12 >> 5;
  unaff_EDI[6] = 0;
  if (((uVar12 == 0) || ((uVar3 & 0x2000) == 0)) &&
     ((uVar10 = uVar10 >> 5, uVar10 == 0 || ((uVar1 & 0x2000) == 0)))) {
    puVar14 = (ushort *)(iVar9 + 0x8800);
    param_1 = param_1 - param_2;
    local_4c = 0;
    do {
      local_50 = local_50 & 0x1ff;
      local_54 = local_54 & 0x1ff;
      local_58 = local_58 & (uVar3 & 0x2000 | 0x1ff0) >> 4;
      local_5c = local_5c & (uVar1 & 0x2000 | 0x1ff0) >> 4;
      if ((uVar2 & 0x8000) != 0) {
        uVar15 = -(uint)puVar14[-0x400] & uVar11;
      }
      if ((uVar5 & 0x8000) != 0) {
        local_3c = -(uint)*puVar14 & uVar13;
      }
      if ((uVar7 & 0x8000) != 0) {
        uVar16 = -(uint)puVar14[-0x200] & 0x1ff;
      }
      if ((uVar8 & 0x8000) != 0) {
        local_38 = -(uint)puVar14[0x200] & 0x1ff;
      }
      iVar9 = *unaff_EDI;
      if (((uVar6 & 0x8000) == 0) && (uVar12 != 0)) {
        FUN_00451e90(param_2,unaff_EDI[3] + local_58 * 0x400);
      }
      else if (local_34 == 0) {
        if ((uVar6 & 0x8000) == 0) {
          FUN_00452150(unaff_EDI[1] + (local_58 * 0x200 + local_3c) * 2,
                       unaff_EDI[3] + (local_50 * 0x200 + local_38) * 2,local_38,local_4c + 0x200);
        }
        else {
          FUN_00452000(local_38);
        }
      }
      else if ((uVar6 & 0x8000) == 0) {
        FUN_00452000(local_3c);
      }
      if (((uVar4 & 0x8000) == 0) && (uVar10 != 0)) {
        FUN_004525c0(param_2 + param_1,unaff_EDI[2] + local_5c * 0x400);
      }
      else if (local_30 == 0) {
        if ((uVar4 & 0x8000) == 0) {
          FUN_004528b0(iVar9 + (local_5c * 0x200 + uVar15) * 2,
                       unaff_EDI[2] + (local_54 * 0x200 + uVar16) * 2,uVar16,local_4c);
        }
        else {
          FUN_00452740(uVar16);
        }
      }
      else if ((uVar4 & 0x8000) == 0) {
        FUN_00452740(uVar15);
      }
      local_58 = local_58 + 1;
      local_5c = local_5c + 1;
      local_54 = local_54 + 1;
      param_2 = param_2 + (param_3 & 0xfffffffe);
      local_50 = local_50 + 1;
      local_4c = local_4c + 1;
      puVar14 = puVar14 + 1;
    } while (local_4c < 0x180);
  }
  return;
}


// ===== FUN_00451e90 @ 00451e90 (353 bytes) =====


void FUN_00451e90(int param_1,ushort *param_2)

{
  ushort uVar1;
  int in_EAX;
  uint in_ECX;
  ushort *in_EDX;
  ushort *puVar2;
  ushort *puVar3;
  uint uVar4;
  int unaff_ESI;
  
  puVar2 = in_EDX;
  if ((in_ECX & 0x200) != 0) {
    in_ECX = in_ECX & 0x1ff;
    puVar2 = param_2;
    param_2 = in_EDX;
  }
  puVar2 = puVar2 + in_ECX;
  puVar3 = (ushort *)(param_1 + 4);
  param_1 = 0x7c;
  do {
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      puVar3[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      puVar3[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar4 = in_ECX + 1;
    puVar2 = puVar2 + 1;
    if ((uVar4 & 0x200) != 0) {
      uVar4 = 0;
      puVar2 = param_2;
    }
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      puVar3[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      puVar3[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar4 = uVar4 + 1;
    puVar2 = puVar2 + 1;
    if ((uVar4 & 0x200) != 0) {
      uVar4 = 0;
      puVar2 = param_2;
    }
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      *puVar3 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      *puVar3 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar4 = uVar4 + 1;
    puVar2 = puVar2 + 1;
    if ((uVar4 & 0x200) != 0) {
      uVar4 = 0;
      puVar2 = param_2;
    }
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      puVar3[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      puVar3[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    in_ECX = uVar4 + 1;
    puVar2 = puVar2 + 1;
    if ((in_ECX & 0x200) != 0) {
      in_ECX = 0;
      puVar2 = param_2;
    }
    puVar3 = puVar3 + 4;
    param_1 = param_1 + -1;
  } while (param_1 != 0);
  return;
}


// ===== FUN_00452000 @ 00452000 (333 bytes) =====


void FUN_00452000(uint param_1)

{
  ushort uVar1;
  int in_EAX;
  ushort *in_ECX;
  int in_EDX;
  ushort *puVar2;
  uint uVar3;
  int unaff_ESI;
  ushort *puVar4;
  ushort *puVar5;
  
  uVar3 = param_1;
  puVar2 = (ushort *)(in_EDX + 4);
  param_1 = 0x7c;
  do {
    uVar1 = *in_ECX;
    puVar5 = in_ECX + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      puVar2[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      puVar2[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      puVar5 = in_ECX + -0x1ff;
    }
    uVar1 = *puVar5;
    puVar4 = puVar5 + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      puVar2[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      puVar2[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      puVar4 = puVar5 + -0x1ff;
    }
    uVar1 = *puVar4;
    puVar5 = puVar4 + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      *puVar2 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      *puVar2 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      puVar5 = puVar4 + -0x1ff;
    }
    uVar1 = *puVar5;
    in_ECX = puVar5 + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
      puVar2[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
      puVar2[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      in_ECX = puVar5 + -0x1ff;
    }
    puVar2 = puVar2 + 4;
    param_1 = param_1 + -1;
  } while (param_1 != 0);
  return;
}


// ===== FUN_00452150 @ 00452150 (1135 bytes) =====


void FUN_00452150(ushort *param_1,ushort *param_2,ushort param_3,ushort *param_4)

{
  ushort uVar1;
  ushort uVar2;
  int in_EAX;
  uint in_ECX;
  ushort *in_EDX;
  ushort *puVar3;
  ushort *puVar4;
  int unaff_ESI;
  ushort *puVar5;
  ushort *puVar6;
  int local_8;
  int local_4;
  
  param_4 = (ushort *)(*(int *)(in_EAX + 0x1c) + 0xc000 + (int)param_4 * 8);
  uVar2 = 0x8000;
  local_4 = 0x3e;
  puVar3 = param_2;
  param_2 = (ushort *)(in_ECX & 0xffff);
  do {
    if ((*param_4 & uVar2) == 0) {
      local_8 = 2;
      do {
        uVar1 = *param_1;
        puVar4 = param_1 + 1;
        puVar6 = puVar3 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = param_1 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar3 + -0x1ff;
        }
        uVar1 = *puVar4;
        puVar3 = puVar4 + 1;
        puVar5 = puVar6 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar3 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar5 = puVar6 + -0x1ff;
        }
        uVar1 = *puVar3;
        puVar4 = puVar3 + 1;
        puVar6 = puVar5 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = puVar3 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar5 + -0x1ff;
        }
        uVar1 = *puVar4;
        param_1 = puVar4 + 1;
        puVar3 = puVar6 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          param_1 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar3 = puVar6 + -0x1ff;
        }
        in_EDX = in_EDX + 4;
        local_8 = local_8 + -1;
      } while (local_8 != 0);
    }
    else {
      *(undefined1 *)(in_EAX + 0x12) = 1;
      local_8 = 2;
      do {
        uVar1 = *puVar3;
        puVar6 = puVar3 + 1;
        puVar4 = param_1 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = param_1 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar3 + -0x1ff;
        }
        uVar1 = *puVar6;
        puVar5 = puVar6 + 1;
        puVar3 = puVar4 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar3 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar5 = puVar6 + -0x1ff;
        }
        uVar1 = *puVar5;
        puVar6 = puVar5 + 1;
        puVar4 = puVar3 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = puVar3 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar5 + -0x1ff;
        }
        uVar1 = *puVar6;
        puVar3 = puVar6 + 1;
        param_1 = puVar4 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 4;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 8;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          param_1 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar3 = puVar6 + -0x1ff;
        }
        in_EDX = in_EDX + 4;
        local_8 = local_8 + -1;
      } while (local_8 != 0);
    }
    if (uVar2 == 1) {
      param_4 = param_4 + 1;
      uVar2 = 0x8000;
    }
    else {
      uVar2 = uVar2 >> 1;
    }
    local_4 = local_4 + -1;
  } while (local_4 != 0);
  return;
}


// ===== FUN_004525c0 @ 004525c0 (377 bytes) =====


void FUN_004525c0(int param_1,ushort *param_2)

{
  ushort uVar1;
  int in_EAX;
  uint in_ECX;
  ushort *in_EDX;
  ushort *puVar2;
  ushort *puVar3;
  uint uVar4;
  int unaff_ESI;
  
  puVar2 = in_EDX;
  if ((in_ECX & 0x200) != 0) {
    in_ECX = in_ECX & 0x1ff;
    puVar2 = param_2;
    param_2 = in_EDX;
  }
  puVar2 = puVar2 + in_ECX;
  puVar3 = (ushort *)(param_1 + 4);
  param_1 = 0x7c;
  do {
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      puVar3[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      puVar3[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar4 = in_ECX + 1;
    puVar2 = puVar2 + 1;
    if ((uVar4 & 0x200) != 0) {
      uVar4 = 0;
      puVar2 = param_2;
    }
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      puVar3[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      puVar3[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar4 = uVar4 + 1;
    puVar2 = puVar2 + 1;
    if ((uVar4 & 0x200) != 0) {
      uVar4 = 0;
      puVar2 = param_2;
    }
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      *puVar3 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      *puVar3 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar4 = uVar4 + 1;
    puVar2 = puVar2 + 1;
    if ((uVar4 & 0x200) != 0) {
      uVar4 = 0;
      puVar2 = param_2;
    }
    uVar1 = *puVar2;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      puVar3[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      puVar3[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    in_ECX = uVar4 + 1;
    puVar2 = puVar2 + 1;
    if ((in_ECX & 0x200) != 0) {
      in_ECX = 0;
      puVar2 = param_2;
    }
    puVar3 = puVar3 + 4;
    param_1 = param_1 + -1;
  } while (param_1 != 0);
  return;
}


// ===== FUN_00452740 @ 00452740 (365 bytes) =====


void FUN_00452740(uint param_1)

{
  ushort uVar1;
  int in_EAX;
  ushort *in_ECX;
  int in_EDX;
  ushort *puVar2;
  uint uVar3;
  int unaff_ESI;
  ushort *puVar4;
  ushort *puVar5;
  
  uVar3 = param_1;
  puVar2 = (ushort *)(in_EDX + 4);
  param_1 = 0x7c;
  do {
    uVar1 = *in_ECX;
    puVar5 = in_ECX + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      puVar2[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      puVar2[-2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      puVar5 = in_ECX + -0x1ff;
    }
    uVar1 = *puVar5;
    puVar4 = puVar5 + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      puVar2[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      puVar2[-1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      puVar4 = puVar5 + -0x1ff;
    }
    uVar1 = *puVar4;
    puVar5 = puVar4 + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      *puVar2 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      *puVar2 = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      puVar5 = puVar4 + -0x1ff;
    }
    uVar1 = *puVar5;
    in_ECX = puVar5 + 1;
    if ((uVar1 & 0x1000) == 0) {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
      puVar2[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
    }
    else {
      *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
      puVar2[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
    }
    uVar3 = uVar3 + 1;
    if ((uVar3 & 0x200) != 0) {
      uVar3 = 0;
      in_ECX = puVar5 + -0x1ff;
    }
    puVar2 = puVar2 + 4;
    param_1 = param_1 + -1;
  } while (param_1 != 0);
  return;
}


// ===== FUN_004528b0 @ 004528b0 (1213 bytes) =====


void FUN_004528b0(ushort *param_1,ushort *param_2,ushort param_3,ushort *param_4)

{
  ushort uVar1;
  ushort uVar2;
  int in_EAX;
  uint in_ECX;
  ushort *in_EDX;
  ushort *puVar3;
  ushort *puVar4;
  int unaff_ESI;
  ushort *puVar5;
  ushort *puVar6;
  int local_8;
  int local_4;
  
  param_4 = (ushort *)(*(int *)(in_EAX + 0x1c) + 0xc000 + (int)param_4 * 8);
  uVar2 = 0x8000;
  local_4 = 0x3e;
  puVar3 = param_2;
  param_2 = (ushort *)(in_ECX & 0xffff);
  do {
    if ((*param_4 & uVar2) == 0) {
      local_8 = 2;
      do {
        uVar1 = *param_1;
        puVar4 = param_1 + 1;
        puVar6 = puVar3 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = param_1 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar3 + -0x1ff;
        }
        uVar1 = *puVar4;
        puVar3 = puVar4 + 1;
        puVar5 = puVar6 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar3 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar5 = puVar6 + -0x1ff;
        }
        uVar1 = *puVar3;
        puVar4 = puVar3 + 1;
        puVar6 = puVar5 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = puVar3 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar5 + -0x1ff;
        }
        uVar1 = *puVar4;
        param_1 = puVar4 + 1;
        puVar3 = puVar6 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          param_1 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar3 = puVar6 + -0x1ff;
        }
        in_EDX = in_EDX + 4;
        local_8 = local_8 + -1;
      } while (local_8 != 0);
    }
    else {
      *(undefined1 *)(in_EAX + 0x12) = 1;
      local_8 = 2;
      do {
        uVar1 = *puVar3;
        puVar6 = puVar3 + 1;
        puVar4 = param_1 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          *in_EDX = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = param_1 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar3 + -0x1ff;
        }
        uVar1 = *puVar6;
        puVar5 = puVar6 + 1;
        puVar3 = puVar4 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          in_EDX[1] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar3 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar5 = puVar6 + -0x1ff;
        }
        uVar1 = *puVar5;
        puVar6 = puVar5 + 1;
        puVar4 = puVar3 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          in_EDX[2] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          puVar4 = puVar3 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar6 = puVar5 + -0x1ff;
        }
        uVar1 = *puVar6;
        puVar3 = puVar6 + 1;
        param_1 = puVar4 + 1;
        if ((uVar1 & 0x1000) == 0) {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 1;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2) | 0x7000;
        }
        else {
          *(uint *)(in_EAX + 0x18) = *(uint *)(in_EAX + 0x18) | 2;
          in_EDX[3] = *(ushort *)(unaff_ESI + (uVar1 & 0xfff) * 2);
        }
        param_2 = (ushort *)((int)param_2 + 1);
        if (((uint)param_2 & 0x200) != 0) {
          param_2 = (ushort *)0x0;
          param_1 = puVar4 + -0x1ff;
        }
        param_3 = param_3 + 1;
        if ((param_3 & 0x200) != 0) {
          param_3 = 0;
          puVar3 = puVar6 + -0x1ff;
        }
        in_EDX = in_EDX + 4;
        local_8 = local_8 + -1;
      } while (local_8 != 0);
    }
    if (uVar2 == 1) {
      param_4 = param_4 + 1;
      uVar2 = 0x8000;
    }
    else {
      uVar2 = uVar2 >> 1;
    }
    local_4 = local_4 + -1;
  } while (local_4 != 0);
  return;
}


