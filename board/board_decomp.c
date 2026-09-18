// ===== FUN_004c6510 @ 004c6510 (446 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c6510(char *param_1)

{
  int iVar1;
  
  FUN_004c0230(0x100);
  _DAT_00dcec58 = 0;
  _DAT_005aeb90 = 0;
  DAT_010ede70 = DAT_010ede70 & 0xffefffff | 0x80000;
  DAT_005aeb40 = &DAT_010edc30;
  DAT_010ed950 = 1;
  DAT_010edee4 = &LAB_004c6100;
  DAT_010edee8 = &LAB_004c6020;
  DAT_010ed974 = 0;
  iVar1 = __stricmp(param_1,"schamp");
  if (iVar1 != 0) {
    iVar1 = __stricmp(param_1,"sfight");
    if (iVar1 != 0) {
      iVar1 = __stricmp(param_1,"gunblade");
      if (iVar1 != 0) {
        iVar1 = __strnicmp(param_1,"fvip",4);
        if (iVar1 != 0) goto LAB_004c65d7;
      }
    }
  }
  _DAT_005aeb90 = _DAT_0051cf40;
  _DAT_00dcec58 = _DAT_0051d0f4;
LAB_004c65d7:
  iVar1 = __strnicmp(param_1,(char *)&PTR_DAT_005197d8,3);
  if (iVar1 == 0) {
    _DAT_005aeb90 = _DAT_0051cf40;
    _DAT_00dcec58 = _DAT_0051d0f4;
  }
  iVar1 = __strnicmp(param_1,"lastb",5);
  if (iVar1 == 0) {
    PTR_LAB_005733a8 = &LAB_004b9480;
    PTR_LAB_005733b0 = &LAB_004b9470;
    PTR_LAB_00573450 = &LAB_004b93d0;
    PTR_LAB_00573458 = &LAB_004b93d0;
  }
  DAT_010ed95c = 0;
  __stricmp(param_1,"zerogun");
  DAT_010ed95c = 1;
  iVar1 = __strnicmp(param_1,(char *)&PTR_DAT_005197d8,3);
  if (iVar1 == 0) {
    DAT_010ed974 = 1;
  }
  iVar1 = __strnicmp(param_1,(char *)&PTR_DAT_0051bc30,3);
  if (iVar1 == 0) {
    DAT_010ed974 = 1;
  }
  iVar1 = __strnicmp(param_1,(char *)&PTR_DAT_0051bc34,3);
  if (iVar1 == 0) {
    DAT_010ed974 = 1;
  }
  iVar1 = __strnicmp(param_1,"indy",4);
  DAT_010ed978 = 2;
  if (iVar1 == 0) {
    DAT_010ed974 = 3;
  }
  return;
}


// ===== io_4c6780 @ 004c6780 (32 bytes) =====

void io_4c6780(void)

{
  DAT_010ed8ec = DAT_010ed8ec + 1;
  if (9 < DAT_010ed8ec) {
    *(undefined4 *)(DAT_005aa730 + 0x202394) = 1;
  }
  return;
}


// ===== FUN_004c6aa0 @ 004c6aa0 (11 bytes) =====

void __fastcall FUN_004c6aa0(int param_1)

{
                    /* WARNING: Could not recover jumptable at 0x004c6aa9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(param_1 + 0x2023c8) + 0x50))();
  return;
}


// ===== FUN_004c6ab0 @ 004c6ab0 (82 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004c6ab0(void)

{
  int *piVar1;
  int *piVar2;
  uint uVar3;
  int unaff_EDI;
  
  piVar1 = *(int **)(unaff_EDI + 0x203864);
  uVar3 = 0x10;
  piVar2 = piVar1;
  do {
    if (*(int *)(((int)&DAT_010f01e8 - (int)piVar1) + (int)piVar2) != *piVar2) {
      *(undefined1 *)(unaff_EDI + 0x30) = 1;
      break;
    }
    uVar3 = uVar3 - 4;
    piVar2 = piVar2 + 1;
  } while (3 < uVar3);
  _DAT_010f01e8 = *piVar1;
  _DAT_010f01ec = piVar1[1];
  _DAT_010f01f0 = piVar1[2];
  _DAT_010f01f4 = piVar1[3];
  return;
}


// ===== FUN_004c6b10 @ 004c6b10 (857 bytes) =====

void FUN_004c6b10(void)

{
  uint uVar1;
  int iVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  int iVar6;
  int in_EAX;
  int iVar7;
  uint uVar8;
  undefined4 local_4;
  
  iVar6 = *(int *)(in_EAX + 0x20386c);
  iVar7 = 0;
  *(undefined1 *)(in_EAX + 0x30) = 1;
  local_4 = 0;
  uVar8 = 2;
  do {
    uVar1 = uVar8 - 2;
    bVar3 = *(byte *)(*(byte *)((*(ushort *)(((int)uVar1 >> 10 & 0x1fU) * 0x200 + 0x8080 + iVar6) &
                                0xff) + 0x203450 + in_EAX) + 0x203750 + in_EAX);
    bVar4 = *(byte *)(*(byte *)((*(ushort *)((uVar1 & 0x1f) * 0x200 + 0x80 + iVar6) & 0xff) +
                                0x203450 + in_EAX) + 0x203550 + in_EAX);
    bVar5 = *(byte *)(in_EAX + 0x203650 +
                     (uint)*(byte *)(in_EAX + 0x203450 +
                                    (*(ushort *)(iVar6 + 0x4080 + ((int)uVar1 >> 5 & 0x1fU) * 0x200)
                                    & 0xff)));
    *(uint *)(iVar7 + *(int *)(in_EAX + 0x203440)) = (uint)CONCAT21(CONCAT11(bVar4,bVar5),bVar3);
    *(ushort *)(*(int *)(in_EAX + 0x20344c) + local_4) =
         (bVar4 & 0xfff0 | (ushort)(bVar5 >> 4)) << 4 | (ushort)(bVar3 >> 4);
    uVar1 = uVar8 - 1;
    bVar3 = *(byte *)(*(byte *)((*(ushort *)(((int)uVar1 >> 10 & 0x1fU) * 0x200 + 0x8080 + iVar6) &
                                0xff) + 0x203450 + in_EAX) + 0x203750 + in_EAX);
    bVar4 = *(byte *)(*(byte *)((*(ushort *)((uVar1 & 0x1f) * 0x200 + 0x80 + iVar6) & 0xff) +
                                0x203450 + in_EAX) + 0x203550 + in_EAX);
    bVar5 = *(byte *)(in_EAX + 0x203650 +
                     (uint)*(byte *)(in_EAX + 0x203450 +
                                    (*(ushort *)(iVar6 + 0x4080 + ((int)uVar1 >> 5 & 0x1fU) * 0x200)
                                    & 0xff)));
    *(uint *)(iVar7 + 4 + *(int *)(in_EAX + 0x203440)) = (uint)CONCAT21(CONCAT11(bVar4,bVar5),bVar3)
    ;
    *(ushort *)(*(int *)(in_EAX + 0x20344c) + 2 + local_4) =
         (bVar4 & 0xfff0 | (ushort)(bVar5 >> 4)) << 4 | (ushort)(bVar3 >> 4);
    bVar3 = *(byte *)(in_EAX + 0x203750 +
                     (uint)*(byte *)(in_EAX + 0x203450 +
                                    (*(ushort *)
                                      (iVar6 + 0x8080 + ((int)uVar8 >> 10 & 0x1fU) * 0x200) & 0xff))
                     );
    bVar4 = *(byte *)(*(byte *)((*(ushort *)((uVar8 & 0x1f) * 0x200 + 0x80 + iVar6) & 0xff) +
                                0x203450 + in_EAX) + 0x203550 + in_EAX);
    bVar5 = *(byte *)(*(byte *)((*(ushort *)(((int)uVar8 >> 5 & 0x1fU) * 0x200 + 0x4080 + iVar6) &
                                0xff) + 0x203450 + in_EAX) + 0x203650 + in_EAX);
    *(uint *)(*(int *)(in_EAX + 0x203440) + 8 + iVar7) = (uint)CONCAT21(CONCAT11(bVar4,bVar5),bVar3)
    ;
    *(ushort *)(*(int *)(in_EAX + 0x20344c) + 4 + local_4) =
         (bVar4 & 0xfff0 | (ushort)(bVar5 >> 4)) << 4 | (ushort)(bVar3 >> 4);
    uVar1 = uVar8 + 1;
    bVar3 = *(byte *)(*(byte *)((*(ushort *)(((int)uVar1 >> 10 & 0x1fU) * 0x200 + 0x8080 + iVar6) &
                                0xff) + 0x203450 + in_EAX) + 0x203750 + in_EAX);
    bVar4 = *(byte *)(*(byte *)((*(ushort *)((uVar1 & 0x1f) * 0x200 + 0x80 + iVar6) & 0xff) +
                                0x203450 + in_EAX) + 0x203550 + in_EAX);
    bVar5 = *(byte *)(in_EAX + 0x203650 +
                     (uint)*(byte *)(in_EAX + 0x203450 +
                                    (*(ushort *)(iVar6 + 0x4080 + ((int)uVar1 >> 5 & 0x1fU) * 0x200)
                                    & 0xff)));
    *(uint *)(iVar7 + 0xc + *(int *)(in_EAX + 0x203440)) =
         (uint)CONCAT21(CONCAT11(bVar4,bVar5),bVar3);
    *(ushort *)(local_4 + 6 + *(int *)(in_EAX + 0x20344c)) =
         (bVar4 & 0xfff0 | (ushort)(bVar5 >> 4)) << 4 | (ushort)(bVar3 >> 4);
    local_4 = local_4 + 8;
    iVar2 = uVar8 + 2;
    iVar7 = iVar7 + 0x10;
    uVar8 = uVar8 + 4;
  } while (iVar2 < 0x8000);
  return;
}


// ===== io_4c6e80 @ 004c6e80 (41 bytes) =====

undefined2 io_4c6e80(void)

{
  undefined2 uVar1;
  
  if (*(int *)(DAT_005aa730 + 0x240) != 0) {
    return *(undefined2 *)(*(int *)(DAT_005aa730 + 0x240) + 0x20b940);
  }
  uVar1 = (**(code **)(DAT_005aa730 + 0x248))();
  return uVar1;
}


// ===== FUN_004c6eb0 @ 004c6eb0 (113 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004c6eb0(HWND param_1)

{
  int iVar1;
  CHAR local_14 [20];
  
  iVar1 = GetDlgCtrlID(param_1);
  GetWindowTextA(param_1,local_14,0x14);
  if (iVar1 == 0x7052e) {
    _DAT_010ed914 = param_1;
    return 1;
  }
  if (iVar1 == 0x504de) {
    _DAT_010ed918 = param_1;
    return 1;
  }
  if (iVar1 == 0x3053c) {
    _DAT_010ed91c = param_1;
  }
  return 1;
}


// ===== io_4c6f30 @ 004c6f30 (45 bytes) =====

void __fastcall io_4c6f30(uint param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_005aa6dc + (param_1 & 0x1ffff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 100))();
                    /* WARNING: Could not recover jumptable at 0x004c6f5b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 100))();
  return;
}


// ===== io_4c6f60 @ 004c6f60 (26 bytes) =====

void __fastcall io_4c6f60(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa6dc + (param_1 & 0x1ffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c6f78. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 100))();
  return;
}


// ===== io_4c6f80 @ 004c6f80 (25 bytes) =====

void __fastcall io_4c6f80(uint param_1,undefined1 param_2)

{
  *(undefined1 *)(DAT_005aa6dc + (param_1 & 0x1ffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c6f97. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 100))();
  return;
}


// ===== io_4c6fa0 @ 004c6fa0 (50 bytes) =====

void __fastcall io_4c6fa0(uint param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_005aa8cc + (param_1 & 0x3fff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x58))();
  if ((param_1 & 0x3fff) < 0x2000) {
    *(undefined1 *)(DAT_005aa72c + 0x24) = 1;
  }
  return;
}


// ===== io_4c6fe0 @ 004c6fe0 (51 bytes) =====

void __fastcall io_4c6fe0(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa8cc + (param_1 & 0x3fff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x58))();
  if ((param_1 & 0x3fff) < 0x2000) {
    *(undefined1 *)(DAT_005aa72c + 0x24) = 1;
  }
  return;
}


// ===== io_4c7020 @ 004c7020 (50 bytes) =====

void __fastcall io_4c7020(uint param_1,undefined1 param_2)

{
  *(undefined1 *)(DAT_005aa8cc + (param_1 & 0x3fff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x58))();
  if ((param_1 & 0x3fff) < 0x2000) {
    *(undefined1 *)(DAT_005aa72c + 0x24) = 1;
  }
  return;
}


// ===== io_4c7060 @ 004c7060 (15 bytes) =====

undefined4 __fastcall io_4c7060(uint param_1)

{
  return *(undefined4 *)((param_1 & 0xffff) + DAT_005aa8b8);
}


// ===== io_4c7070 @ 004c7070 (16 bytes) =====

undefined4 __fastcall io_4c7070(uint param_1)

{
  return CONCAT22((short)((uint)DAT_005aa8b8 >> 0x10),
                  *(undefined2 *)((param_1 & 0xffff) + DAT_005aa8b8));
}


// ===== io_4c7080 @ 004c7080 (15 bytes) =====

undefined4 __fastcall io_4c7080(uint param_1)

{
  return CONCAT31((int3)((uint)DAT_005aa8b8 >> 8),*(undefined1 *)((param_1 & 0xffff) + DAT_005aa8b8)
                 );
}


// ===== io_4c7090 @ 004c7090 (45 bytes) =====

void __fastcall io_4c7090(uint param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_005aa8b8 + (param_1 & 0xffff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x54))();
                    /* WARNING: Could not recover jumptable at 0x004c70bb. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x54))();
  return;
}


// ===== io_4c70c0 @ 004c70c0 (26 bytes) =====

void __fastcall io_4c70c0(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa8b8 + (param_1 & 0xffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c70d8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x54))();
  return;
}


// ===== io_4c70e0 @ 004c70e0 (29 bytes) =====

void __fastcall io_4c70e0(uint param_1,ushort param_2)

{
  *(ushort *)(DAT_005aa8b8 + (param_1 & 0xffff)) = param_2 & 0xff;
                    /* WARNING: Could not recover jumptable at 0x004c70fb. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x54))();
  return;
}


// ===== io_4c7100 @ 004c7100 (45 bytes) =====

void __fastcall io_4c7100(uint param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_005aa764 + (param_1 >> 1 & 0xfffff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x60))();
                    /* WARNING: Could not recover jumptable at 0x004c712b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x60))();
  return;
}


// ===== io_4c7130 @ 004c7130 (28 bytes) =====

void __fastcall io_4c7130(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa764 + (param_1 >> 1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c714a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x60))();
  return;
}


// ===== io_4c7150 @ 004c7150 (27 bytes) =====

void __fastcall io_4c7150(uint param_1,undefined1 param_2)

{
  *(undefined1 *)(DAT_005aa764 + (param_1 >> 1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c7169. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x60))();
  return;
}


// ===== io_4c7170 @ 004c7170 (45 bytes) =====

void __fastcall io_4c7170(uint param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_005aa8b4 + (param_1 >> 1 & 0xfffff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x5c))();
                    /* WARNING: Could not recover jumptable at 0x004c719b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x5c))();
  return;
}


// ===== io_4c71a0 @ 004c71a0 (28 bytes) =====

void __fastcall io_4c71a0(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa8b4 + (param_1 >> 1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c71ba. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x5c))();
  return;
}


// ===== io_4c71c0 @ 004c71c0 (27 bytes) =====

void __fastcall io_4c71c0(uint param_1,undefined1 param_2)

{
  *(undefined1 *)(DAT_005aa8b4 + (param_1 >> 1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c71d9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x5c))();
  return;
}


// ===== io_4c71e0 @ 004c71e0 (45 bytes) =====

void __fastcall io_4c71e0(uint param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_005aa764 + (param_1 & 0xfffff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x60))();
                    /* WARNING: Could not recover jumptable at 0x004c720b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x60))();
  return;
}


// ===== io_4c7210 @ 004c7210 (26 bytes) =====

void __fastcall io_4c7210(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa764 + (param_1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c7228. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x60))();
  return;
}


// ===== io_4c7230 @ 004c7230 (25 bytes) =====

void __fastcall io_4c7230(uint param_1,undefined1 param_2)

{
  *(undefined1 *)(DAT_005aa764 + (param_1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c7247. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x60))();
  return;
}


// ===== io_4c7250 @ 004c7250 (45 bytes) =====

void __fastcall io_4c7250(uint param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_005aa8b4 + (param_1 & 0xfffff)) = param_2;
  (**(code **)(DAT_005aa8c0 + 0x5c))();
                    /* WARNING: Could not recover jumptable at 0x004c727b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x5c))();
  return;
}


// ===== io_4c7280 @ 004c7280 (26 bytes) =====

void __fastcall io_4c7280(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa8b4 + (param_1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c7298. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x5c))();
  return;
}


// ===== io_4c72a0 @ 004c72a0 (25 bytes) =====

void __fastcall io_4c72a0(uint param_1,undefined1 param_2)

{
  *(undefined1 *)(DAT_005aa8b4 + (param_1 & 0xfffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c72b7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x5c))();
  return;
}


// ===== io_4c72c0 @ 004c72c0 (15 bytes) =====

void __fastcall io_4c72c0(uint param_1,undefined4 param_2)

{
  if ((param_1 & 0xfffff) == 0) {
    DAT_010ed8f4 = param_2;
  }
  return;
}


// ===== io_4c72d0 @ 004c72d0 (17 bytes) =====

void __fastcall io_4c72d0(uint param_1,ushort param_2)

{
  if ((param_1 & 0xfffff) == 0) {
    DAT_010ed8f4 = (uint)param_2;
  }
  return;
}


// ===== io_4c72f0 @ 004c72f0 (17 bytes) =====

void __fastcall io_4c72f0(uint param_1,byte param_2)

{
  if ((param_1 & 0xfffff) == 0) {
    DAT_010ed8f4 = (uint)param_2;
  }
  return;
}


// ===== io_4c7310 @ 004c7310 (24 bytes) =====

void __fastcall io_4c7310(undefined *param_1)

{
  if (param_1 == &DAT_008c0010) {
                    /* WARNING: Could not recover jumptable at 0x004c7325. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(DAT_005aa8c0 + 0xc))();
    return;
  }
  return;
}


// ===== io_4c75f0 @ 004c75f0 (43 bytes) =====

undefined4 __fastcall io_4c75f0(char param_1)

{
  if ((param_1 != '\x04') && (param_1 != '\x02')) {
    if (DAT_005aa74c != 0) {
      DAT_005aa74c = DAT_005aa74c + -1;
    }
    return 0xff;
  }
  return 7;
}


// ===== io_4c7620 @ 004c7620 (43 bytes) =====

undefined4 __fastcall io_4c7620(char param_1)

{
  if ((param_1 != '\x04') && (param_1 != '\x02')) {
    if (DAT_005aa74c != 0) {
      DAT_005aa74c = DAT_005aa74c + -1;
    }
    return 0xff;
  }
  return 7;
}


// ===== io_4c7650 @ 004c7650 (37 bytes) =====

undefined1 __fastcall io_4c7650(char param_1)

{
  if ((param_1 != '\x04') && (param_1 != '\x02')) {
    if (DAT_005aa74c != 0) {
      DAT_005aa74c = DAT_005aa74c + -1;
    }
    return 0xff;
  }
  return 7;
}


// ===== io_4c7680 @ 004c7680 (28 bytes) =====

void __fastcall io_4c7680(uint param_1,undefined4 param_2)

{
  int iVar1;
  
  *(undefined4 *)((param_1 & 0x7ffff) + DAT_005aa75c) = param_2;
  iVar1 = DAT_005aa72c;
  *(undefined1 *)(DAT_005aa72c + 0x10) = 1;
  *(undefined1 *)(iVar1 + 0x24) = 1;
  return;
}


// ===== io_4c76a0 @ 004c76a0 (29 bytes) =====

void __fastcall io_4c76a0(uint param_1,undefined2 param_2)

{
  int iVar1;
  
  *(undefined2 *)((param_1 & 0x7ffff) + DAT_005aa75c) = param_2;
  iVar1 = DAT_005aa72c;
  *(undefined1 *)(DAT_005aa72c + 0x10) = 1;
  *(undefined1 *)(iVar1 + 0x24) = 1;
  return;
}


// ===== io_4c76c0 @ 004c76c0 (28 bytes) =====

void __fastcall io_4c76c0(uint param_1,undefined1 param_2)

{
  int iVar1;
  
  *(undefined1 *)((param_1 & 0x7ffff) + DAT_005aa75c) = param_2;
  iVar1 = DAT_005aa72c;
  *(undefined1 *)(DAT_005aa72c + 0x10) = 1;
  *(undefined1 *)(iVar1 + 0x24) = 1;
  return;
}


// ===== io_4c76e0 @ 004c76e0 (10 bytes) =====

void io_4c76e0(void)

{
                    /* WARNING: Could not recover jumptable at 0x004c76e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x74))();
  return;
}


// ===== io_4c79c0 @ 004c79c0 (145 bytes) =====

void __fastcall io_4c79c0(undefined *param_1,uint param_2)

{
  if (param_1 != &DAT_00980000) {
    if (param_1 != &DAT_00980008) {
      if (param_1 == &DAT_0098000c) {
        DAT_010ed8e0 = param_2;
      }
      return;
    }
    if ((DAT_010ed8d4 ^ param_2) == 0x80000000) {
      if ((int)param_2 < 0) {
        DAT_010ed8d4 = param_2;
        DAT_010ed8d8 = 0;
        return;
      }
      (**(code **)(DAT_005aa8c0 + 0x40))();
    }
    DAT_010ed8d4 = param_2;
    return;
  }
  if ((DAT_010ed8cc ^ param_2) == 0x80000000) {
    if ((int)param_2 < 0) {
      DAT_010ed8cc = param_2;
      DAT_010ed8d0 = 0;
      return;
    }
    (**(code **)(DAT_005aa8c0 + 0x3c))();
  }
  DAT_010ed8cc = param_2;
  return;
}


// ===== io_4c7a60 @ 004c7a60 (1 bytes) =====

void io_4c7a60(void)

{
  return;
}


// ===== io_4c7a70 @ 004c7a70 (1 bytes) =====

void io_4c7a70(void)

{
  return;
}


// ===== io_4c7a80 @ 004c7a80 (84 bytes) =====

uint __fastcall io_4c7a80(undefined *param_1)

{
  if (param_1 == &DAT_00980000) {
    return DAT_010ed8cc;
  }
  if (param_1 == &DAT_00980004) {
    return 0xffffffff;
  }
  if (param_1 == &DAT_00900008) {
    return DAT_010ed8d4;
  }
  if (param_1 == &DAT_0098000c) {
    return DAT_010ed8e0 & 3 | DAT_010ed8dc & 4;
  }
  return (param_1 != &DAT_009c0004) - 1 & 5;
}


// ===== io_4c7ae0 @ 004c7ae0 (23 bytes) =====

void __fastcall io_4c7ae0(uint param_1,undefined4 param_2)

{
  if ((param_1 & 0xffff) < 0x4000) {
    *(undefined4 *)(DAT_005aa750 + (param_1 & 0xffff)) = param_2;
  }
  return;
}


// ===== io_4c7b00 @ 004c7b00 (24 bytes) =====

void __fastcall io_4c7b00(uint param_1,undefined2 param_2)

{
  if ((param_1 & 0xffff) < 0x4000) {
    *(undefined2 *)(DAT_005aa750 + (param_1 & 0xffff)) = param_2;
  }
  return;
}


// ===== io_4c7b20 @ 004c7b20 (74 bytes) =====

void __fastcall io_4c7b20(uint param_1,byte param_2)

{
  if ((param_1 & 0xffff) < 0x4000) {
    *(byte *)(DAT_005aa750 + (param_1 & 0xffff)) = param_2;
    return;
  }
  if ((param_1 & 3) == 0) {
    DAT_005a96bd = param_2;
    return;
  }
  if ((param_1 & 3) == 2) {
    DAT_005aa760 = DAT_005aa760 & 0xfe;
    DAT_005a96bf = DAT_005a96bf & 0x7f;
    if ((param_2 & 1) != 0) {
      DAT_005aa760 = DAT_005aa760 | 1;
      DAT_005a96bf = DAT_005a96bf | 0x80;
    }
  }
  return;
}


// ===== io_4c7b70 @ 004c7b70 (15 bytes) =====

undefined4 __fastcall io_4c7b70(uint param_1)

{
  return *(undefined4 *)((param_1 & 0xffff) + DAT_005aa750);
}


// ===== io_4c7b80 @ 004c7b80 (16 bytes) =====

undefined4 __fastcall io_4c7b80(uint param_1)

{
  return CONCAT22((short)((uint)DAT_005aa750 >> 0x10),
                  *(undefined2 *)((param_1 & 0xffff) + DAT_005aa750));
}


// ===== io_4c7ba0 @ 004c7ba0 (57 bytes) =====

undefined1 __fastcall io_4c7ba0(uint param_1)

{
  if (DAT_010ed8b8 != '\0') {
    if ((param_1 & 0xffff) < 0x4000) {
      return *(undefined1 *)(DAT_005aa750 + (param_1 & 0xffff));
    }
    if ((param_1 & 3) == 0) {
      return DAT_005a96bd;
    }
    if ((param_1 & 3) == 2) {
      return DAT_005aa760;
    }
  }
  return 0xff;
}


// ===== io_4c7be0 @ 004c7be0 (34 bytes) =====

void __fastcall io_4c7be0(uint param_1,undefined4 param_2)

{
  if ((param_1 & 0xfffffff0) == 0xff000010) {
    *(undefined4 *)(DAT_005aa730 + 0x200140 + (param_1 + 0xfffff0 & 0xfffffffc)) = param_2;
  }
  return;
}


// ===== io_4c7c10 @ 004c7c10 (50 bytes) =====

void __fastcall io_4c7c10(undefined *param_1,uint param_2)

{
  if (*(char *)(DAT_005aa730 + 0x200180) != '\0') {
    param_1 = *(undefined **)(DAT_005aa730 + 0x200184);
  }
  if (param_1 == &DAT_00e80000) {
    DAT_010ed8e4 = DAT_010ed8e4 & param_2;
    return;
  }
  if (param_1 == &DAT_00e80004) {
    DAT_010ed8e8 = param_2;
  }
  return;
}


// ===== io_4c7c50 @ 004c7c50 (1 bytes) =====

void io_4c7c50(void)

{
  return;
}


// ===== io_4c7c60 @ 004c7c60 (1 bytes) =====

void io_4c7c60(void)

{
  return;
}


// ===== io_4c7c70 @ 004c7c70 (39 bytes) =====

undefined4 __fastcall io_4c7c70(undefined *param_1)

{
  undefined4 uVar1;
  
  if (*(char *)(DAT_005aa730 + 0x200180) != '\0') {
    param_1 = *(undefined **)(DAT_005aa730 + 0x200184);
  }
  uVar1 = DAT_010ed8e4;
  if (param_1 != &DAT_00e80000) {
    uVar1 = DAT_010ed8e8;
  }
  return uVar1;
}


// ===== io_4c7ca0 @ 004c7ca0 (3 bytes) =====

undefined4 io_4c7ca0(void)

{
  return 0;
}


// ===== io_4c7cb0 @ 004c7cb0 (3 bytes) =====

undefined1 io_4c7cb0(void)

{
  return 0;
}


// ===== io_4c7cc0 @ 004c7cc0 (1 bytes) =====

void io_4c7cc0(void)

{
  return;
}


// ===== io_4c7cd0 @ 004c7cd0 (1 bytes) =====

void io_4c7cd0(void)

{
  return;
}


// ===== io_4c7ce0 @ 004c7ce0 (1 bytes) =====

void io_4c7ce0(void)

{
  return;
}


// ===== io_4c7d00 @ 004c7d00 (1 bytes) =====

void io_4c7d00(void)

{
  return;
}


// ===== io_4c7d10 @ 004c7d10 (1 bytes) =====

void io_4c7d10(void)

{
  return;
}


// ===== io_4c7d20 @ 004c7d20 (4 bytes) =====

undefined4 io_4c7d20(void)

{
  return 0xffffffff;
}


// ===== io_4c7d30 @ 004c7d30 (6 bytes) =====

undefined4 io_4c7d30(void)

{
  return 0xffff;
}


// ===== io_4c7d40 @ 004c7d40 (3 bytes) =====

undefined1 io_4c7d40(void)

{
  return 0xff;
}


// ===== io_4c7d50 @ 004c7d50 (4 bytes) =====

undefined4 io_4c7d50(void)

{
  return 0xffffffff;
}


// ===== io_4c7d60 @ 004c7d60 (6 bytes) =====

undefined4 io_4c7d60(void)

{
  return 0xffff;
}


// ===== io_4c7d70 @ 004c7d70 (3 bytes) =====

undefined1 io_4c7d70(void)

{
  return 0xff;
}


// ===== io_4c7d80 @ 004c7d80 (34 bytes) =====

void __fastcall io_4c7d80(uint param_1,undefined4 param_2)

{
  if (*(char *)(DAT_005aa730 + 0x200180) != '\0') {
    param_1 = *(uint *)(DAT_005aa730 + 0x200184);
  }
  (&DAT_005aa73c)[param_1 >> 2 & 0x3f] = param_2;
  return;
}


// ===== io_4c7db0 @ 004c7db0 (39 bytes) =====

uint __fastcall io_4c7db0(uint param_1)

{
  if (*(char *)(DAT_005aa730 + 0x200180) != '\0') {
    param_1 = *(uint *)(DAT_005aa730 + 0x200184);
  }
  return (&DAT_005aa73c)[param_1 >> 2 & 0x3f] & 0xffffff;
}


// ===== FUN_004c8070 @ 004c8070 (127 bytes) =====

undefined4 __fastcall FUN_004c8070(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)(param_1 + 0x2038a0);
  if (iVar1 != 0) {
    do {
      iVar2 = (**(code **)(**(int **)(iVar1 + 0x20) + 8))(*(int **)(iVar1 + 0x20));
    } while (iVar2 != 0);
    FUN_0048d1c3(iVar1);
  }
  iVar1 = *(int *)(param_1 + 0x2038a4);
  if (iVar1 != 0) {
    do {
      iVar2 = (**(code **)(**(int **)(iVar1 + 0x20) + 8))(*(int **)(iVar1 + 0x20));
    } while (iVar2 != 0);
    FUN_0048d1c3(iVar1);
  }
  iVar1 = *(int *)(param_1 + 0x2038a8);
  if (iVar1 != 0) {
    do {
      iVar2 = (**(code **)(**(int **)(iVar1 + 0x20) + 8))(*(int **)(iVar1 + 0x20));
    } while (iVar2 != 0);
    FUN_0048d1c3(iVar1);
  }
  (**(code **)(*(int *)(param_1 + 0x2023c8) + 0x48))();
  return 1;
}


// ===== FUN_004c8190 @ 004c8190 (124 bytes) =====

undefined4 __fastcall FUN_004c8190(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x18);
  if (*(char *)(iVar1 + 0x28d) != '\0') {
    QueryPerformanceCounter((LARGE_INTEGER *)(iVar1 + 0xb8));
    *(char **)(iVar1 + 200) = "SoundIdle";
  }
  FUN_004eeba0(*(int *)(param_1 + 0x14) + 0x68);
  FUN_004eeba0(*(int *)(param_1 + 0x14) + 0x2b178);
  if (*(char *)(*(int *)(param_1 + 0x18) + 0x28d) != '\0') {
    QueryPerformanceCounter((LARGE_INTEGER *)(*(int *)(param_1 + 0x18) + 0xc0));
  }
  return 1;
}


// ===== FUN_004c8210 @ 004c8210 (70 bytes) =====

undefined4 __fastcall FUN_004c8210(int param_1)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = *(int *)(param_1 + 0x14);
  piVar2 = *(int **)(iVar1 + 0xc);
  (**(code **)(*piVar2 + 0x50))(piVar2,0,0);
  piVar2 = *(int **)(iVar1 + 0xc);
  (**(code **)(*piVar2 + 0x48))(piVar2);
  iVar1 = *(int *)(param_1 + 0x14);
  piVar2 = *(int **)(iVar1 + 0x10);
  (**(code **)(*piVar2 + 0x50))(piVar2,0,0);
  piVar2 = *(int **)(iVar1 + 0x10);
  (**(code **)(*piVar2 + 0x48))(piVar2);
  return 1;
}


// ===== FUN_004c82d0 @ 004c82d0 (173 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __fastcall FUN_004c82d0(short param_1)

{
  uint uVar1;
  uint uVar2;
  
  if (param_1 == 1) {
    _DAT_005aa768 = 0;
    if (DAT_005aa761 != DAT_005a96be) {
      _DAT_005aa768 = (uint)(byte)(&DAT_005aa6e0)[DAT_005aa761];
      DAT_005aa761 = DAT_005aa761 + 1 & 0x1f;
      uVar2 = _DAT_005aa768;
      if (DAT_005aa761 != DAT_005a96be) goto LAB_004c8320;
    }
    DAT_00528d38 = 0;
    uVar2 = _DAT_005aa768;
  }
  else {
    uVar2 = 0;
  }
LAB_004c8320:
  if (param_1 != 0) {
    uVar1 = 0;
LAB_004c836f:
    return uVar2 << 8 | uVar1;
  }
  _DAT_005aa768 = 0;
  if (DAT_005aa761 != DAT_005a96be) {
    uVar1 = (uint)(byte)(&DAT_005aa6e0)[DAT_005aa761];
    DAT_005aa761 = DAT_005aa761 + 1 & 0x1f;
    _DAT_005aa768 = uVar1;
    if (DAT_005aa761 != DAT_005a96be) goto LAB_004c836f;
  }
  DAT_00528d38 = 0;
  return uVar2 << 8 | _DAT_005aa768;
}


// ===== FUN_004c8390 @ 004c8390 (128 bytes) =====

void __fastcall FUN_004c8390(int param_1)

{
  if ((-1 < DAT_005aa73c) && (DAT_005aa73c = DAT_005aa73c - param_1, DAT_005aa73c < 0)) {
    DAT_010ed8e4 = DAT_010ed8e4 | 4;
    DAT_005aa73c = -1;
  }
  if ((-1 < DAT_005aa740) && (DAT_005aa740 = DAT_005aa740 - param_1, DAT_005aa740 < 0)) {
    DAT_010ed8e4 = DAT_010ed8e4 | 8;
    DAT_005aa740 = -1;
  }
  if ((-1 < DAT_005aa744) && (DAT_005aa744 = DAT_005aa744 - param_1, DAT_005aa744 < 0)) {
    DAT_010ed8e4 = DAT_010ed8e4 | 0x10;
    DAT_005aa744 = -1;
  }
  if ((-1 < DAT_005aa748) && (DAT_005aa748 = DAT_005aa748 - param_1, DAT_005aa748 < 0)) {
    DAT_010ed8e4 = DAT_010ed8e4 | 0x20;
    DAT_005aa748 = -1;
  }
  return;
}


// ===== io_4c8410 @ 004c8410 (34 bytes) =====

undefined4 __fastcall io_4c8410(uint param_1)

{
  if ((param_1 & 0xfffff) == 0x40022) {
    return 0xff;
  }
  return *(undefined4 *)((param_1 & 0xfffff) + **(int **)(DAT_005aa730 + 4));
}


// ===== io_4c8560 @ 004c8560 (68 bytes) =====

undefined4 __fastcall io_4c8560(uint param_1)

{
  uint uVar1;
  
  if (0x1f < (param_1 & 0xfffff) - 0x40000) {
    return CONCAT22((short)((uint)*(int **)(DAT_005aa730 + 4) >> 0x10),
                    *(undefined2 *)((param_1 & 0xfffff) + **(int **)(DAT_005aa730 + 4)));
  }
  uVar1 = DAT_010ed910 + 1;
  DAT_010ed910 = uVar1 & 0xf;
  if (DAT_010ed910 == 0) {
    return 7;
  }
  if ((uVar1 & 2) != 0) {
    return 0;
  }
  return 4;
}


// ===== io_4c85b0 @ 004c85b0 (20 bytes) =====

undefined4 __fastcall io_4c85b0(uint param_1)

{
  return CONCAT31((int3)((uint)**(int **)(DAT_005aa730 + 4) >> 8),
                  *(undefined1 *)((param_1 & 0xfffff) + **(int **)(DAT_005aa730 + 4)));
}


// ===== io_4c85d0 @ 004c85d0 (76 bytes) =====

void __fastcall io_4c85d0(uint param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_005aa8c0;
  uVar2 = (param_1 & 0xfffff) >> 1;
  *(short *)(DAT_005aa6dc + (uVar2 & 0x1ffff)) = (short)param_2;
  (**(code **)(iVar1 + 100))();
  iVar1 = DAT_005aa8c0;
  *(short *)(DAT_005aa6dc + (uVar2 + 2 & 0x1ffff)) = (short)((uint)param_2 >> 0x10);
                    /* WARNING: Could not recover jumptable at 0x004c861a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(iVar1 + 100))();
  return;
}


// ===== io_4c8620 @ 004c8620 (28 bytes) =====

void __fastcall io_4c8620(uint param_1,undefined2 param_2)

{
  *(undefined2 *)(DAT_005aa6dc + (param_1 >> 1 & 0x1ffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c863a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 100))();
  return;
}


// ===== io_4c8640 @ 004c8640 (27 bytes) =====

void __fastcall io_4c8640(uint param_1,undefined1 param_2)

{
  *(undefined1 *)(DAT_005aa6dc + (param_1 >> 1 & 0x1ffff)) = param_2;
                    /* WARNING: Could not recover jumptable at 0x004c8659. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 100))();
  return;
}


// ===== io_4c86c0 @ 004c86c0 (163 bytes) =====

void __fastcall io_4c86c0(uint param_1,uint param_2)

{
  uint uVar1;
  byte bVar2;
  
  (**(code **)(DAT_005aa8c0 + 0x74))();
  param_1 = param_1 & 0xffff;
  if (param_1 == 0) {
    DAT_010ed900 = param_2 & 1;
    return;
  }
  bVar2 = (byte)param_2;
  if (param_1 == 0x1e) {
    DAT_010ed8fe = bVar2 & 7;
    return;
  }
  if (param_1 == 8) {
    DAT_00574cf4 = param_2 & 0xffff;
  }
  else {
    if (param_1 == 10) {
      DAT_00574cf0 = param_2 & 0xffff;
      return;
    }
    if (param_1 != 0x22) {
      if ((0x1ff < param_1) && (*(int *)(DAT_005aa730 + 0x3c) < 1)) {
        if ((param_1 != 0x202) || ((short)param_2 != 0x4d)) {
          *(byte *)(DAT_005aa71c + 0x7e00 + param_1) = bVar2;
        }
        DAT_010ed8c7 = 1;
      }
      return;
    }
  }
  if (bVar2 == 0x83) {
    return;
  }
  if (bVar2 == 0) {
    return;
  }
  if (DAT_005aa708 == 0) {
    return;
  }
  if (DAT_0057285b == bVar2) {
    return;
  }
  DAT_0057285b = bVar2;
  if (DAT_005aa708 == 1) {
    uVar1 = param_2 & 0xff;
    switch(param_2 & 0xf0) {
    case 0:
      if ((param_2 & 2) == 0) {
        (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,0,0);
        return;
      }
      break;
    case 0x10:
switchD_004c7744_caseD_10:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,0,0);
      return;
    case 0x20:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,5,param_2 & 0xf);
      return;
    case 0x30:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,1,param_2 & 0xf);
      return;
    case 0x40:
switchD_004c7744_caseD_40:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,2,param_2 & 0xf);
      return;
    case 0x50:
      goto switchD_004c7744_caseD_50;
    case 0x60:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,3,param_2 & 0xf);
      return;
    }
  }
  else {
    if (DAT_005aa708 != 3) {
      return;
    }
    switch(param_2 & 0xf0) {
    case 0x10:
      goto switchD_004c7744_caseD_40;
    default:
      goto switchD_004c7744_default;
    case 0x80:
    case 0x90:
      if ((param_2 & 0x1f) != 0) {
        (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,3,bVar2 >> 1 & 0xf);
        return;
      }
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,0,0);
      return;
    case 0xc0:
    case 0xd0:
      if ((param_2 & 0x1f) == 0) goto switchD_004c7744_caseD_10;
      uVar1 = (uint)(bVar2 >> 1);
    }
switchD_004c7744_caseD_50:
    (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,4,uVar1 & 0xf);
  }
switchD_004c7744_default:
  return;
}


// ===== io_4c8770 @ 004c8770 (136 bytes) =====

uint __fastcall io_4c8770(undefined *param_1)

{
  if (param_1 == &DAT_00980000) {
    return DAT_010ed8cc;
  }
  if (param_1 == &DAT_00980004) {
    return (uint)(*(int *)(DAT_005aa738 + 0x88) == 0x20);
  }
  if (param_1 == &DAT_00900008) {
    return DAT_010ed8d4;
  }
  if (param_1 == &DAT_0098000c) {
    DAT_010ed8ec = DAT_010ed8ec + 1;
    if (9 < DAT_010ed8ec) {
      *(undefined4 *)(DAT_005aa730 + 0x202394) = 1;
    }
    return DAT_010ed8e0 & 3 | DAT_010ed8dc & 4;
  }
  if (param_1 + -0x980030 < (undefined *)0x31) {
    return *(uint *)(param_1 + -0x40b328);
  }
  return 0;
}


// ===== io_4c8800 @ 004c8800 (37 bytes) =====

undefined4 __fastcall io_4c8800(undefined *param_1)

{
  if (param_1 != &DAT_00980004) {
    return 0xffff;
  }
  if (*(int *)(DAT_005aa738 + 0x88) == 0x20) {
    return 1;
  }
  return 0;
}


// ===== io_4c8830 @ 004c8830 (108 bytes) =====

uint __fastcall io_4c8830(undefined *param_1)

{
  if (param_1 == &DAT_0098000c) {
    DAT_010ed8ec = DAT_010ed8ec + 1;
    if (9 < DAT_010ed8ec) {
      *(undefined4 *)(DAT_005aa730 + 0x202394) = 1;
    }
    return DAT_010ed8e0 & 3 | DAT_010ed8dc & 4;
  }
  if (param_1 == &DAT_00980014) {
    return (uint)(byte)((DAT_010ed8d0 != 0) - 1);
  }
  if (param_1 == &DAT_00980004) {
    return (uint)(*(int *)(DAT_005aa738 + 0x88) == 0x20);
  }
  return 0;
}


// ===== FUN_004c88a0 @ 004c88a0 (420 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __fastcall FUN_004c88a0(ushort param_1)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  
  uVar2 = (uint)param_1;
  bVar1 = (byte)(param_1 >> 8);
  switch(uVar2) {
  case 0:
  case 6:
  case 0x22:
    return (uint)CONCAT11(bVar1,DAT_005aa876);
  default:
    if (param_1 < 0x200) {
      if (*(int *)(DAT_005aa730 + 0x3c) == 0) {
        uVar2 = uVar2 & 0xff;
        switch(uVar2) {
        case 0:
          return (uint)DAT_005aa70c;
        case 2:
          return (uint)DAT_005aa70d;
        case 4:
          return (uint)DAT_005aa70e;
        case 6:
          return (uint)DAT_005aa70f;
        case 8:
          return (uint)DAT_005aa710;
        case 10:
          return (uint)DAT_005aa711;
        case 0xc:
          return (uint)DAT_005aa712;
        case 0xe:
          return (uint)DAT_005aa713;
        case 0x10:
          return (uint)DAT_005aa714;
        }
        goto switchD_004c88b3_caseD_24;
      }
    }
    else if (*(int *)(DAT_005aa730 + 0x3c) < 1) {
      return (uint)CONCAT11(bVar1,*(undefined1 *)(uVar2 + 0x7e00 + DAT_005aa71c));
    }
    break;
  case 2:
    if (DAT_010ed900 != 0) {
      iVar3 = FUN_004eca10();
      return CONCAT31((int3)((uint)(iVar3 << 5) >> 8),(byte)(iVar3 << 5) | DAT_005aa874 & 0xdf) |
             0xd0;
    }
    return (uint)CONCAT11(bVar1,DAT_005aa874);
  case 4:
    return (uint)CONCAT11(bVar1,DAT_005aa875);
  case 8:
    return (uint)CONCAT11(bVar1,DAT_005aa877);
  case 0xc:
    return (uint)CONCAT11(bVar1,DAT_005aa878);
  case 0x10:
    return (uint)CONCAT11(bVar1,DAT_005aa879);
  case 0x12:
    return (uint)CONCAT11(bVar1,DAT_005aa87a);
  case 0x14:
    uVar2 = (uint)CONCAT11(bVar1,DAT_005aa876);
    if (DAT_005aa876 != '\0') {
      return uVar2;
    }
    break;
  case 0x16:
    break;
  case 0x18:
    uVar2 = DAT_010ed8ff - 1;
    switch(uVar2) {
    case 0:
      return DAT_00574cfc + 0x56;
    case 1:
      return DAT_00574cfc + 0x56 >> 8;
    case 2:
      return DAT_00574cf8 - 0x53;
    case 3:
      return DAT_00574cf8 + 0xad >> 8;
    case 4:
      return DAT_00574d04 + 0x56;
    case 5:
      return DAT_00574d04 + 0x56 >> 8;
    case 6:
      return DAT_00574d00 - 0x53;
    case 7:
      return DAT_00574d00 + 0xad >> 8;
    case 8:
      return ~_DAT_005aa734;
    }
    break;
  case 0x1c:
    uVar2 = CONCAT11(bVar1,(0 < *(int *)(DAT_005aa730 + 0x3c)) + -1) & 0xffffffe5;
    return CONCAT31((int3)(uVar2 >> 8),(char)uVar2 + '\x1a');
  case 0x1e:
    uVar2 = (uint)DAT_010ed8fe;
    DAT_010ed8fe = DAT_010ed8fe + 1;
    return (uint)(&DAT_005aa70c)[uVar2];
  case 0x24:
  case 0x40:
switchD_004c88b3_caseD_24:
    return uVar2 & 0xffffff00;
  case 0x42:
    return CONCAT31((uint3)bVar1,0x40);
  }
  return CONCAT31((int3)(uVar2 >> 8),0xff);
}


// ===== FUN_004c8b30 @ 004c8b30 (574 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c8b30(int param_1)

{
  int iVar1;
  undefined4 *puStack_30;
  int iStack_2c;
  undefined4 local_1a;
  undefined1 local_14 [8];
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  iVar1 = *(int *)(param_1 + 8);
  if (*(char *)(iVar1 + 0x56a) != '\0') {
    local_c = 0x20001;
    local_8 = 0x230;
    local_4 = **(undefined4 **)(iVar1 + 0x56c);
    iStack_2c = 1;
    puStack_30 = &local_c;
    (**(code **)(iVar1 + 0x58c))();
    iStack_2c = 0x4c8b78;
    ShowCursor(0);
  }
  iStack_2c = 0xff;
  puStack_30 = &local_1a;
  *(undefined1 *)(iVar1 + 0x210) = 1;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))(0,(int)&local_1a + 2,local_14);
  _DAT_005aa894 = (float)(0xff - (int)puStack_30);
  _DAT_005aa89c = (float)(0xff - iStack_2c);
  (**(code **)(**(int **)(param_1 + 8) + 0x10))(1,&puStack_30);
  _DAT_005aa890 = (float)(0xff - (int)&stack0xffffffcf);
  _DAT_005aa898 = 0;
  DAT_005aa70c = (char)((((int)ROUND((float)(0xff - (int)&stack0xffffffcf)) & 0xffU) << 7) >> 8) +
                 '@';
  DAT_005aa70d = (char)((((int)ROUND(_DAT_005aa894) & 0xffU) << 7) >> 8) + '@';
  DAT_005aa70e = 0x40;
  DAT_005aa70f = (char)((((int)ROUND(_DAT_005aa89c) & 0xffU) << 7) >> 8) + '@';
  if (((uint)&iStack_2c & 0x10000) != 0) {
    DAT_005aa875 = DAT_005aa875 & 0xfe;
  }
  if (((uint)&iStack_2c & 0x1000000) != 0) {
    DAT_005aa875 = DAT_005aa875 & 0xfd;
  }
  if ((DAT_00574cf4 == 0xe0) || (DAT_00574cf4 == 0xe)) {
    DAT_005aa876 = 0xfe;
  }
  if ((DAT_00574cf4 == 0xd0) || (DAT_00574cf4 == 0xd)) {
    DAT_005aa876 = 0xfd;
  }
  if ((DAT_00574cf4 == 0xb0) || (DAT_00574cf4 == 0xb)) {
    DAT_005aa876 = 0xfb;
  }
  if ((DAT_00574cf4 == 0x70) || (DAT_00574cf4 == 7)) {
    DAT_005aa876 = 0xf7;
  }
  return;
}


// ===== FUN_004c8d70 @ 004c8d70 (779 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c8d70(int param_1)

{
  float fVar1;
  float fVar2;
  uint uVar3;
  byte bVar4;
  byte bVar5;
  char local_c;
  
  fVar2 = (float)_DAT_0051d0a8;
  fVar1 = (float)_DAT_0051d0a0;
  bVar5 = ~DAT_005aa876;
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a1) == '\0') {
    if (((bVar5 & 0x20) == 0) || ((bVar5 & 0x10) != 0)) {
      if ((bVar5 & 0x20) == 0) {
        if ((bVar5 & 0x10) == 0) {
          if ((bVar5 & 0x10) == 0) {
            if (_DAT_005aa890 <= 0.0) {
              bVar4 = _DAT_005aa890 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa890) << 10) >> 8);
              goto LAB_004c8eea;
            }
            _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
          }
        }
        else {
          bVar4 = _DAT_005aa890 < fVar2 |
                  (byte)((ushort)((ushort)(NAN(_DAT_005aa890) || NAN(fVar2)) << 10) >> 8);
LAB_004c8eea:
          if ((POPCOUNT(bVar4) & 1U) != 0) {
            _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
          }
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa890) < _DAT_0051cf40) {
      _DAT_005aa890 = 0.0;
    }
    if (fVar2 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_0051d090;
    }
    if (_DAT_005aa890 < fVar1) {
      _DAT_005aa890 = _DAT_0051d0b0;
    }
  }
  else {
    uVar3 = (**(code **)(**(int **)(param_1 + 8) + 8))(1);
    _DAT_005aa890 = (float)(int)(((uVar3 & 0xffff) >> 8) - 0x80);
    fVar2 = (float)_DAT_0051d0a8;
    fVar1 = (float)_DAT_0051d0a0;
  }
  if (_DAT_005aa890 < fVar1) {
    _DAT_005aa890 = _DAT_0051d0b0;
  }
  local_c = (char)(int)ROUND(_DAT_005aa890 + _DAT_0051d098);
  DAT_005aa70c = local_c;
  if ((char)(*(int **)(param_1 + 8))[0x168] == '\0') {
    if (((bVar5 & 0x80) == 0) || ((bVar5 & 0x40) != 0)) {
      if ((bVar5 & 0x80) == 0) {
        if ((bVar5 & 0x40) == 0) {
          if ((bVar5 & 0x40) == 0) {
            if (_DAT_005aa898 <= 0.0) {
              bVar5 = _DAT_005aa898 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa898) << 10) >> 8);
              goto LAB_004c8fb3;
            }
            _DAT_005aa898 = _DAT_005aa898 - _DAT_0051cf40;
          }
        }
        else {
          bVar5 = _DAT_005aa898 < fVar2 |
                  (byte)((ushort)((ushort)(NAN(_DAT_005aa898) || NAN(fVar2)) << 10) >> 8);
LAB_004c8fb3:
          if ((POPCOUNT(bVar5) & 1U) != 0) {
            _DAT_005aa898 = _DAT_005aa898 + _DAT_0051cf40;
          }
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa898) {
      _DAT_005aa898 = _DAT_005aa898 - _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa898) < _DAT_0051cf40) {
      _DAT_005aa898 = 0.0;
    }
    if (fVar2 < _DAT_005aa898) {
      _DAT_005aa898 = _DAT_0051d090;
    }
    fVar2 = _DAT_0051d0b0;
    if (fVar1 <= _DAT_005aa898) goto LAB_004c8e91;
  }
  else {
    uVar3 = (**(code **)(**(int **)(param_1 + 8) + 8))(0);
    _DAT_005aa898 = (float)(int)(((uVar3 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0a0;
    fVar2 = _DAT_005aa898;
  }
  _DAT_005aa898 = fVar2;
LAB_004c8e91:
  if (_DAT_005aa898 < fVar1) {
    _DAT_005aa898 = _DAT_0051d0b0;
  }
  local_c = (char)(int)ROUND(_DAT_005aa898 + _DAT_0051d098);
  DAT_005aa70e = -local_c;
  return;
}


// ===== FUN_004c9080 @ 004c9080 (1229 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c9080(int param_1)

{
  float fVar1;
  float fVar2;
  undefined1 uVar3;
  byte bVar4;
  ushort uVar5;
  uint uVar6;
  int iVar7;
  undefined1 uVar8;
  
  fVar1 = (float)_DAT_0051d0b8;
  bVar4 = ~DAT_005aa876;
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a1) == '\0') {
    if ((bVar4 & 0x20) == 0) {
      if (1.0 < _DAT_005aa894) {
        _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
      }
    }
    else if (_DAT_005aa894 < fVar1) {
      _DAT_005aa894 = _DAT_005aa894 + _DAT_0051cf40;
    }
  }
  else {
    uVar5 = (**(code **)(**(int **)(param_1 + 8) + 8))(1);
    _DAT_005aa894 = (float)(uVar5 >> 8);
    fVar1 = (float)_DAT_0051d0b8;
  }
  fVar2 = _DAT_0051d0b4;
  if ((fVar1 < _DAT_005aa894) || (fVar2 = 1.0, _DAT_005aa894 < 1.0)) {
    _DAT_005aa894 = fVar2;
  }
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a2) == '\0') {
    if ((bVar4 & 0x10) == 0) {
      if (0.0 < _DAT_005aa898) {
        _DAT_005aa898 = _DAT_005aa898 - _DAT_0051cf40;
      }
    }
    else if (_DAT_005aa898 < fVar1) {
      _DAT_005aa898 = _DAT_005aa898 + _DAT_0051cf40;
    }
  }
  else {
    uVar5 = (**(code **)(**(int **)(param_1 + 8) + 8))(2);
    _DAT_005aa898 = (float)(uVar5 >> 8);
    fVar1 = (float)_DAT_0051d0b8;
  }
  if (_DAT_005aa898 <= fVar1) {
    if (_DAT_005aa898 < 1.0) {
      _DAT_005aa898 = 1.0;
    }
  }
  else {
    _DAT_005aa898 = _DAT_0051d0b4;
  }
  if ((char)(*(int **)(param_1 + 8))[0x168] == '\0') {
    fVar1 = (float)_DAT_0051d0a8;
    if (((bVar4 & 0x80) == 0) || ((bVar4 & 0x40) != 0)) {
      if ((bVar4 & 0x80) == 0) {
        if ((bVar4 & 0x40) == 0) {
          if ((bVar4 & 0x40) == 0) {
            if (_DAT_005aa890 <= 0.0) {
              bVar4 = _DAT_005aa890 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa890) << 10) >> 8);
              goto LAB_004c9317;
            }
            _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
          }
        }
        else {
          bVar4 = _DAT_005aa890 < fVar1 |
                  (byte)((ushort)((ushort)(NAN(_DAT_005aa890) || NAN(fVar1)) << 10) >> 8);
LAB_004c9317:
          if ((POPCOUNT(bVar4) & 1U) != 0) {
            _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
          }
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa890) < _DAT_0051cf40) {
      _DAT_005aa890 = 0.0;
    }
    if (fVar1 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_0051d090;
    }
    if (_DAT_005aa890 < (float)_DAT_0051d0a0) {
      _DAT_005aa890 = _DAT_0051d0b0;
    }
  }
  else {
    uVar6 = (**(code **)(**(int **)(param_1 + 8) + 8))(0);
    _DAT_005aa890 = (float)(int)(((uVar6 & 0xffff) >> 8) - 0x80);
  }
  if (_DAT_005aa890 < (float)_DAT_0051d0a0) {
    _DAT_005aa890 = _DAT_0051d0b0;
  }
  DAT_005aa70c = (char)(((int)ROUND(_DAT_005aa890 + _DAT_0051d098) & 0xffffU) * 0xc0 >> 8) + ' ';
  uVar5 = FUN_005008f0();
  if (uVar5 < 3) {
    uVar5 = 3;
  }
  else if (0xfd < uVar5) {
    uVar5 = 0xfd;
  }
  uVar8 = (undefined1)uVar5;
  DAT_005aa70d = uVar8;
  uVar5 = FUN_005008f0();
  if (uVar5 < 3) {
    uVar5 = 3;
  }
  else if (0xfd < uVar5) {
    uVar5 = 0xfd;
  }
  uVar3 = (undefined1)uVar5;
  DAT_005aa70e = uVar3;
  uVar5 = FUN_005008f0();
  if (uVar5 < 3) {
    uVar5 = 3;
  }
  else if (0xfd < uVar5) {
    uVar5 = 0xfd;
  }
  DAT_005aa70f = (undefined1)uVar5;
  DAT_005aa876 = DAT_005aa70c;
  DAT_005aa874 = uVar8;
  DAT_005aa875 = uVar3;
  if (DAT_010ed8fc != '\0') {
    *(undefined4 *)(param_1 + 0x203438) = 0;
  }
  DAT_005aa87a = DAT_005aa87a & 0x8f;
  if (DAT_010ed8fd == '\0') {
    if ((DAT_005aa878 & 1) == 0) {
      *(undefined4 *)(param_1 + 0x203438) = 1;
    }
    else if ((DAT_005aa878 & 2) == 0) {
      *(undefined4 *)(param_1 + 0x203438) = 2;
    }
    else if ((DAT_005aa878 & 4) == 0) {
      *(undefined4 *)(param_1 + 0x203438) = 3;
    }
    else if ((DAT_005aa878 & 8) == 0) {
      *(undefined4 *)(param_1 + 0x203438) = 4;
    }
    else if ((DAT_005aa878 & 0x10) == 0) {
      *(undefined4 *)(param_1 + 0x203438) = 0;
    }
    goto LAB_004c94ff;
  }
  bVar4 = (~s___AVModel2___00594a2c[0xd] ^ ~DAT_005aa878) & ~DAT_005aa878;
  if ((bVar4 & 2) == 0) {
    if (((bVar4 & 1) != 0) && (*(int *)(param_1 + 0x203438) != 0)) {
      iVar7 = *(int *)(param_1 + 0x203438) + -1;
      goto LAB_004c9498;
    }
  }
  else if (*(int *)(param_1 + 0x203438) != 4) {
    iVar7 = *(int *)(param_1 + 0x203438) + 1;
LAB_004c9498:
    *(int *)(param_1 + 0x203438) = iVar7;
  }
  s___AVModel2___00594a2c[0xd] = DAT_005aa878;
LAB_004c94ff:
  iVar7 = *(int *)(param_1 + 0x203438);
  if (iVar7 == 1) {
    DAT_005aa87a = DAT_005aa87a | 0x20;
    return;
  }
  if (iVar7 == 2) {
    DAT_005aa87a = DAT_005aa87a | 0x10;
    return;
  }
  if (iVar7 == 3) {
    DAT_005aa87a = DAT_005aa87a | 0x60;
    return;
  }
  if (iVar7 == 4) {
    DAT_005aa87a = DAT_005aa87a | 0x50;
  }
  return;
}


// ===== FUN_004c9550 @ 004c9550 (877 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004c9550(void)

{
  float fVar1;
  byte bVar2;
  ushort uVar3;
  uint uVar4;
  byte bVar5;
  undefined1 local_c;
  
  fVar1 = (float)_DAT_0051d0d8;
  bVar2 = ~DAT_005aa876;
  if ((char)(*(int **)(DAT_005aa730 + 8))[0x168] != '\0') {
    uVar4 = (**(code **)(**(int **)(DAT_005aa730 + 8) + 8))(0);
    _DAT_005aa890 = (float)(int)(((uVar4 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
    goto LAB_004c95cb;
  }
  if (((bVar2 & 0x80) == 0) || ((bVar2 & 0x40) != 0)) {
    if ((bVar2 & 0x80) == 0) {
      if ((bVar2 & 0x40) == 0) {
        if ((bVar2 & 0x40) == 0) {
          if (_DAT_005aa890 <= 0.0) {
            bVar5 = _DAT_005aa890 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa890) << 10) >> 8);
            goto LAB_004c96cd;
          }
          _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
        }
      }
      else {
        bVar5 = _DAT_005aa890 < fVar1 |
                (byte)((ushort)((ushort)(NAN(_DAT_005aa890) || NAN(fVar1)) << 10) >> 8);
LAB_004c96cd:
        if ((POPCOUNT(bVar5) & 1U) != 0) {
          _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
        }
      }
    }
  }
  else if (_DAT_0051d094 < _DAT_005aa890) {
    _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
  }
  if (ABS(_DAT_005aa890) < _DAT_0051cf40) {
    _DAT_005aa890 = 0.0;
  }
  if (fVar1 < _DAT_005aa890) {
    _DAT_005aa890 = _DAT_0051d0c0;
  }
LAB_004c95cb:
  if (_DAT_005aa890 < (float)_DAT_0051d0d0) {
    _DAT_005aa890 = _DAT_0051d0cc;
  }
  local_c = (undefined1)(int)ROUND(_DAT_005aa890 + fVar1);
  DAT_005aa70c = local_c;
  if (*(char *)((int)*(int **)(DAT_005aa730 + 8) + 0x5a1) == '\0') {
    if ((bVar2 & 0x20) == 0) {
      if (((bVar2 & 0x10) != 0) && (_DAT_0051d0c4 < _DAT_005aa894)) {
        _DAT_005aa894 = _DAT_005aa894 - _DAT_0051d040;
      }
    }
    else if (_DAT_005aa894 < _DAT_0051d0c8) {
      _DAT_005aa894 = _DAT_005aa894 + _DAT_0051d040;
    }
  }
  else {
    uVar3 = (**(code **)(**(int **)(DAT_005aa730 + 8) + 8))(1);
    _DAT_005aa894 = (float)(uVar3 >> 0xb);
  }
  fVar1 = _DAT_0051d0c8;
  if ((_DAT_005aa894 <= _DAT_0051d0c8) && (fVar1 = _DAT_005aa894, _DAT_005aa894 < _DAT_0051d0c4)) {
    fVar1 = _DAT_0051d0c4;
  }
  _DAT_005aa894 = fVar1;
  local_c = (undefined1)(int)ROUND(_DAT_005aa894);
  DAT_005aa70d = local_c;
  if (*(char *)((int)*(int **)(DAT_005aa730 + 8) + 0x5a2) == '\0') {
    if ((bVar2 & 8) == 0) {
      if (1.0 < _DAT_005aa898) {
        _DAT_005aa898 = _DAT_005aa898 - _DAT_0051cf40;
      }
    }
    else if (_DAT_005aa898 < (float)_DAT_0051d0b8) {
      _DAT_005aa898 = _DAT_005aa898 + _DAT_0051cf40;
    }
  }
  else {
    uVar3 = (**(code **)(**(int **)(DAT_005aa730 + 8) + 8))(2);
    _DAT_005aa898 = (float)(uVar3 >> 8);
  }
  if (_DAT_005aa898 <= (float)_DAT_0051d0b8) {
    if (_DAT_005aa898 < 1.0) {
      _DAT_005aa898 = 1.0;
    }
  }
  else {
    _DAT_005aa898 = _DAT_0051d0b4;
  }
  DAT_005aa875 = DAT_005aa70d;
  local_c = (undefined1)(int)ROUND(_DAT_005aa898);
  DAT_005aa70e = local_c;
  DAT_005aa874 = local_c;
  DAT_005aa876 = DAT_005aa70c;
  return;
}


// ===== FUN_004c9910 @ 004c9910 (1200 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c9910(int param_1)

{
  float fVar1;
  byte bVar2;
  ushort uVar3;
  uint uVar4;
  byte bVar5;
  undefined1 uStack_c;
  
  fVar1 = (float)_DAT_0051d0d8;
  bVar2 = ~DAT_005aa876;
  if ((char)(*(int **)(param_1 + 8))[0x168] == '\0') {
    if (((bVar2 & 0x80) == 0) || ((bVar2 & 0x40) != 0)) {
      if ((bVar2 & 0x80) == 0) {
        if ((bVar2 & 0x40) == 0) {
          if ((bVar2 & 0x40) == 0) {
            if (_DAT_005aa890 <= 0.0) {
              if (_DAT_005aa890 < 0.0) {
                _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
              }
            }
            else {
              _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
            }
          }
        }
        else if (_DAT_005aa890 < fVar1) {
          _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa890) < _DAT_0051cf40) {
      _DAT_005aa890 = 0.0;
    }
    if (fVar1 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_0051d0c0;
    }
  }
  else {
    uVar4 = (**(code **)(**(int **)(param_1 + 8) + 8))(0);
    _DAT_005aa890 = (float)(int)(((uVar4 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
  }
  if (_DAT_005aa890 < (float)_DAT_0051d0d0) {
    _DAT_005aa890 = _DAT_0051d0cc;
  }
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a1) == '\0') {
    if (((bVar2 & 0x20) == 0) || ((bVar2 & 0x10) != 0)) {
      if ((bVar2 & 0x20) == 0) {
        if ((bVar2 & 0x10) == 0) {
          if ((bVar2 & 0x10) == 0) {
            if (_DAT_005aa894 <= 0.0) {
              bVar5 = _DAT_005aa894 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa894) << 10) >> 8);
              goto LAB_004c9b9c;
            }
            _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
          }
        }
        else {
          bVar5 = _DAT_005aa894 < fVar1 |
                  (byte)((ushort)((ushort)(NAN(_DAT_005aa894) || NAN(fVar1)) << 10) >> 8);
LAB_004c9b9c:
          if ((POPCOUNT(bVar5) & 1U) != 0) {
            _DAT_005aa894 = _DAT_005aa894 + _DAT_0051cf40;
          }
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa894) {
      _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa894) < _DAT_0051cf40) {
      _DAT_005aa894 = 0.0;
    }
    if (fVar1 < _DAT_005aa894) {
      _DAT_005aa894 = _DAT_0051d0c0;
    }
  }
  else {
    uVar4 = (**(code **)(**(int **)(param_1 + 8) + 8))(1);
    _DAT_005aa894 = (float)(int)(((uVar4 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
  }
  if (_DAT_005aa894 < (float)_DAT_0051d0d0) {
    _DAT_005aa894 = _DAT_0051d0cc;
  }
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a2) == '\0') {
    if ((bVar2 & 1) == 0) {
      if (1.0 < _DAT_005aa898) {
        _DAT_005aa898 = _DAT_005aa898 - _DAT_0051cf40;
      }
    }
    else if (_DAT_005aa898 < (float)_DAT_0051d0b8) {
      _DAT_005aa898 = _DAT_005aa898 + _DAT_0051cf40;
    }
  }
  else {
    uVar3 = (**(code **)(**(int **)(param_1 + 8) + 8))(2);
    _DAT_005aa898 = (float)(uVar3 >> 8);
    fVar1 = (float)_DAT_0051d0d8;
  }
  if (_DAT_005aa898 <= (float)_DAT_0051d0b8) {
    if (_DAT_005aa898 < 1.0) {
      _DAT_005aa898 = 1.0;
    }
  }
  else {
    _DAT_005aa898 = _DAT_0051d0b4;
  }
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa890 + fVar1);
  DAT_005aa70c = uStack_c;
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa894 + fVar1);
  DAT_005aa70d = uStack_c;
  uStack_c = (undefined1)(int)ROUND((float)_DAT_0051cf88 - _DAT_005aa898);
  DAT_005aa70e = uStack_c;
  if (DAT_00574cf0 == 0xffffffff) {
    return;
  }
  if ((DAT_00574cf0 & 0xff) == 0xff) {
    DAT_010f01c0 = 0;
LAB_004c9d25:
    if (DAT_010f01c4 != 0) goto LAB_004c9d43;
    if (DAT_010f01c0 == 1) {
      DAT_010f01c0 = 2;
      goto LAB_004c9d39;
    }
    if (DAT_010f01c0 == 2) {
      DAT_010f01c0 = 3;
      goto LAB_004c9daa;
    }
  }
  else {
    if (((DAT_00574cf0 & 0xff) != 0) || (DAT_010f01c0 != 0)) goto LAB_004c9d25;
    DAT_010f01c0 = 1;
LAB_004c9d39:
    DAT_010f01c4 = 10;
LAB_004c9d43:
    DAT_010f01c4 = DAT_010f01c4 + -1;
  }
  if (DAT_010f01c0 == 1) {
    DAT_005aa876 = 0xce;
    DAT_00574cf0 = 0xffffffff;
    return;
  }
  if (DAT_010f01c0 == 2) {
    DAT_005aa876 = 0xff;
    DAT_00574cf0 = 0xffffffff;
    return;
  }
  if (DAT_010f01c0 != 3) {
    DAT_00574cf0 = 0xffffffff;
    return;
  }
LAB_004c9daa:
  DAT_005aa876 = (char)DAT_00574cf0;
  DAT_00574cf0 = 0xffffffff;
  return;
}


// ===== FUN_004c9dc0 @ 004c9dc0 (942 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004c9dc0(int param_1)

{
  float fVar1;
  uint uVar2;
  byte bVar3;
  undefined1 uStack_c;
  
  fVar1 = (float)_DAT_0051d0d8;
  bVar3 = ~DAT_005aa876;
  if ((char)(*(int **)(param_1 + 8))[0x168] == '\0') {
    if (((bVar3 & 0x80) == 0) || ((bVar3 & 0x40) != 0)) {
      if ((bVar3 & 0x80) == 0) {
        if ((bVar3 & 0x40) == 0) {
          if ((bVar3 & 0x40) == 0) {
            if (_DAT_005aa890 <= 0.0) {
              if (_DAT_005aa890 < 0.0) {
                _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
              }
            }
            else {
              _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
            }
          }
        }
        else if (_DAT_005aa890 < fVar1) {
          _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa890) < _DAT_0051cf40) {
      _DAT_005aa890 = 0.0;
    }
    if (fVar1 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_0051d0c0;
    }
  }
  else {
    uVar2 = (**(code **)(**(int **)(param_1 + 8) + 8))(0);
    _DAT_005aa890 = (float)(int)(((uVar2 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
  }
  if (_DAT_005aa890 < (float)_DAT_0051d0d0) {
    _DAT_005aa890 = _DAT_0051d0cc;
  }
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a1) == '\0') {
    if (((bVar3 & 0x20) == 0) || ((bVar3 & 0x10) != 0)) {
      if ((bVar3 & 0x20) == 0) {
        if ((bVar3 & 0x10) == 0) {
          if ((bVar3 & 0x10) == 0) {
            if (_DAT_005aa894 <= 0.0) {
              bVar3 = _DAT_005aa894 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa894) << 10) >> 8);
              goto LAB_004ca088;
            }
            _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
          }
        }
        else {
          bVar3 = _DAT_005aa894 < fVar1 |
                  (byte)((ushort)((ushort)(NAN(_DAT_005aa894) || NAN(fVar1)) << 10) >> 8);
LAB_004ca088:
          if ((POPCOUNT(bVar3) & 1U) != 0) {
            _DAT_005aa894 = _DAT_005aa894 + _DAT_0051cf40;
          }
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa894) {
      _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa894) < _DAT_0051cf40) {
      _DAT_005aa894 = 0.0;
    }
    if (fVar1 < _DAT_005aa894) {
      _DAT_005aa894 = _DAT_0051d0c0;
    }
  }
  else {
    uVar2 = (**(code **)(**(int **)(param_1 + 8) + 8))(1);
    _DAT_005aa894 = (float)(int)(((uVar2 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
  }
  if (_DAT_005aa894 < (float)_DAT_0051d0d0) {
    _DAT_005aa894 = _DAT_0051d0cc;
  }
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa890 + fVar1);
  DAT_005aa70c = uStack_c;
  uStack_c = (undefined1)(int)ROUND(fVar1 + _DAT_005aa894);
  DAT_005aa70d = uStack_c;
  if (DAT_00574cf0 == 0xffffffff) {
    return;
  }
  if ((DAT_00574cf0 & 0xff) == 0xff) {
    DAT_010f01b8 = 0;
LAB_004c9f3b:
    if (DAT_010f01bc != 0) goto LAB_004c9f5d;
    if (DAT_010f01b8 == 1) {
      DAT_010f01b8 = 2;
      goto LAB_004c9f53;
    }
    if (DAT_010f01b8 == 2) {
      DAT_010f01b8 = 3;
      goto LAB_004ca158;
    }
  }
  else {
    if (((DAT_00574cf0 & 0xff) != 0) || (DAT_010f01b8 != 0)) goto LAB_004c9f3b;
    DAT_010f01b8 = 1;
LAB_004c9f53:
    DAT_010f01bc = 10;
LAB_004c9f5d:
    DAT_010f01bc = DAT_010f01bc + -1;
  }
  if (DAT_010f01b8 == 1) {
    DAT_005aa876 = 0xce;
    DAT_00574cf0 = 0xffffffff;
    return;
  }
  if (DAT_010f01b8 == 2) {
    DAT_005aa876 = 0xff;
    DAT_00574cf0 = 0xffffffff;
    return;
  }
  if (DAT_010f01b8 != 3) {
    DAT_00574cf0 = 0xffffffff;
    return;
  }
LAB_004ca158:
  DAT_005aa876 = (char)DAT_00574cf0;
  DAT_00574cf0 = 0xffffffff;
  return;
}


// ===== FUN_004ca170 @ 004ca170 (726 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004ca170(int param_1)

{
  float fVar1;
  uint uVar2;
  byte bVar3;
  undefined1 uStack_c;
  
  fVar1 = (float)_DAT_0051d0d8;
  bVar3 = ~DAT_005aa876;
  if ((char)(*(int **)(param_1 + 8))[0x168] == '\0') {
    if (((bVar3 & 0x80) == 0) || ((bVar3 & 0x40) != 0)) {
      if ((bVar3 & 0x80) == 0) {
        if ((bVar3 & 0x40) == 0) {
          if ((bVar3 & 0x40) == 0) {
            if (_DAT_005aa890 <= 0.0) {
              if (_DAT_005aa890 < 0.0) {
                _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
              }
            }
            else {
              _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
            }
          }
        }
        else if (_DAT_005aa890 < fVar1) {
          _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
        }
      }
    }
    else if (_DAT_0051d094 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
    }
    if (ABS(_DAT_005aa890) < _DAT_0051cf40) {
      _DAT_005aa890 = 0.0;
    }
    if (fVar1 < _DAT_005aa890) {
      _DAT_005aa890 = _DAT_0051d0c0;
    }
  }
  else {
    uVar2 = (**(code **)(**(int **)(param_1 + 8) + 8))(0);
    _DAT_005aa890 = (float)(int)(((uVar2 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
  }
  if (_DAT_005aa890 < (float)_DAT_0051d0d0) {
    _DAT_005aa890 = _DAT_0051d0cc;
  }
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a1) != '\0') {
    uVar2 = (**(code **)(**(int **)(param_1 + 8) + 8))(1);
    _DAT_005aa894 = (float)(int)(((uVar2 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
    goto LAB_004ca253;
  }
  if (((bVar3 & 0x20) == 0) || ((bVar3 & 0x10) != 0)) {
    if ((bVar3 & 0x20) == 0) {
      if ((bVar3 & 0x10) == 0) {
        if ((bVar3 & 0x10) == 0) {
          if (_DAT_005aa894 <= 0.0) {
            bVar3 = _DAT_005aa894 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa894) << 10) >> 8);
            goto LAB_004ca3c9;
          }
          _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
        }
      }
      else {
        bVar3 = _DAT_005aa894 < fVar1 |
                (byte)((ushort)((ushort)(NAN(_DAT_005aa894) || NAN(fVar1)) << 10) >> 8);
LAB_004ca3c9:
        if ((POPCOUNT(bVar3) & 1U) != 0) {
          _DAT_005aa894 = _DAT_005aa894 + _DAT_0051cf40;
        }
      }
    }
  }
  else if (_DAT_0051d094 < _DAT_005aa894) {
    _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
  }
  if (ABS(_DAT_005aa894) < _DAT_0051cf40) {
    _DAT_005aa894 = 0.0;
  }
  if (fVar1 < _DAT_005aa894) {
    _DAT_005aa894 = _DAT_0051d0c0;
  }
LAB_004ca253:
  if (_DAT_005aa894 < (float)_DAT_0051d0d0) {
    _DAT_005aa894 = _DAT_0051d0cc;
  }
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa890 + fVar1);
  DAT_005aa70c = uStack_c;
  uStack_c = (undefined1)(int)ROUND(fVar1 + _DAT_005aa894);
  DAT_005aa70d = uStack_c;
  return;
}


// ===== FUN_004ca450 @ 004ca450 (872 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004ca450(int param_1)

{
  float fVar1;
  float fVar2;
  ushort uVar3;
  uint uVar4;
  byte bVar5;
  undefined1 uStack_c;
  
  fVar1 = (float)_DAT_0051d0b8;
  bVar5 = ~DAT_005aa876;
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a1) == '\0') {
    if ((bVar5 & 0x20) == 0) {
      if (1.0 < _DAT_005aa894) {
        _DAT_005aa894 = _DAT_005aa894 - _DAT_0051cf40;
      }
    }
    else if (_DAT_005aa894 < fVar1) {
      _DAT_005aa894 = _DAT_005aa894 + _DAT_0051cf40;
    }
  }
  else {
    uVar3 = (**(code **)(**(int **)(param_1 + 8) + 8))(1);
    _DAT_005aa894 = (float)(uVar3 >> 8);
    fVar1 = (float)_DAT_0051d0b8;
  }
  fVar2 = _DAT_0051d0b4;
  if ((fVar1 < _DAT_005aa894) || (fVar2 = 1.0, _DAT_005aa894 < 1.0)) {
    _DAT_005aa894 = fVar2;
  }
  if (*(char *)((int)*(int **)(param_1 + 8) + 0x5a2) == '\0') {
    if ((bVar5 & 0x10) == 0) {
      if (1.0 < _DAT_005aa898) {
        _DAT_005aa898 = _DAT_005aa898 - _DAT_0051cf40;
      }
    }
    else if (_DAT_005aa898 < fVar1) {
      _DAT_005aa898 = _DAT_005aa898 + _DAT_0051cf40;
    }
  }
  else {
    uVar3 = (**(code **)(**(int **)(param_1 + 8) + 8))(2);
    _DAT_005aa898 = (float)(uVar3 >> 8);
    fVar1 = (float)_DAT_0051d0b8;
  }
  if (_DAT_005aa898 <= fVar1) {
    if (_DAT_005aa898 < 1.0) {
      _DAT_005aa898 = 1.0;
    }
  }
  else {
    _DAT_005aa898 = _DAT_0051d0b4;
  }
  fVar1 = (float)_DAT_0051d0d8;
  if ((char)(*(int **)(param_1 + 8))[0x168] != '\0') {
    uVar4 = (**(code **)(**(int **)(param_1 + 8) + 8))(0);
    _DAT_005aa890 = (float)(int)(((uVar4 & 0xffff) >> 8) - 0x80);
    fVar1 = (float)_DAT_0051d0d8;
    goto LAB_004ca630;
  }
  if (((bVar5 & 0x80) == 0) || ((bVar5 & 0x40) != 0)) {
    if ((bVar5 & 0x80) == 0) {
      if ((bVar5 & 0x40) == 0) {
        if ((bVar5 & 0x40) == 0) {
          if (_DAT_005aa890 <= 0.0) {
            bVar5 = _DAT_005aa890 < 0.0 | (byte)((ushort)((ushort)NAN(_DAT_005aa890) << 10) >> 8);
            goto LAB_004ca739;
          }
          _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
        }
      }
      else {
        bVar5 = _DAT_005aa890 < fVar1 |
                (byte)((ushort)((ushort)(NAN(_DAT_005aa890) || NAN(fVar1)) << 10) >> 8);
LAB_004ca739:
        if ((POPCOUNT(bVar5) & 1U) != 0) {
          _DAT_005aa890 = _DAT_005aa890 + _DAT_0051cf40;
        }
      }
    }
  }
  else if (_DAT_0051d094 < _DAT_005aa890) {
    _DAT_005aa890 = _DAT_005aa890 - _DAT_0051cf40;
  }
  if (ABS(_DAT_005aa890) < _DAT_0051cf40) {
    _DAT_005aa890 = 0.0;
  }
  if (fVar1 < _DAT_005aa890) {
    _DAT_005aa890 = _DAT_0051d0c0;
  }
LAB_004ca630:
  if (_DAT_005aa890 < (float)_DAT_0051d0d0) {
    _DAT_005aa890 = _DAT_0051d0cc;
  }
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa890 + fVar1);
  DAT_005aa70c = uStack_c;
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa894);
  DAT_005aa70d = uStack_c;
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa898);
  DAT_005aa70e = uStack_c;
  uStack_c = (undefined1)(int)ROUND(_DAT_005aa89c);
  DAT_005aa70f = uStack_c;
  return;
}


// ===== FUN_004ca7c0 @ 004ca7c0 (697 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004ca7c0(int param_1)

{
  int unaff_EDI;
  undefined1 *puVar1;
  int iVar2;
  int iStack_60;
  int iStack_5c;
  undefined1 *puStack_58;
  undefined1 *puStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  int iStack_48;
  int iStack_44;
  undefined1 *puStack_40;
  undefined1 *puStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  undefined1 local_1e [6];
  undefined1 local_18 [4];
  undefined1 local_14 [8];
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  iVar2 = *(int *)(param_1 + 8);
  if (*(char *)(iVar2 + 0x56a) != '\0') {
    local_c = 0x20001;
    local_8 = 0x230;
    local_4 = **(undefined4 **)(iVar2 + 0x56c);
    uStack_34 = 1;
    uStack_38 = &local_c;
    puStack_3c = (undefined1 *)0x4ca802;
    (**(code **)(iVar2 + 0x58c))();
    uStack_34 = 0x4ca809;
    ShowCursor(0);
  }
  *(undefined1 *)(iVar2 + 0x210) = 1;
  uStack_34 = 0x1ef;
  uStack_38 = (undefined4 *)local_1e;
  puStack_3c = local_14;
  puStack_40 = local_18;
  iStack_44 = 0;
  iStack_48 = 0x4ca835;
  iVar2 = 0x17f;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))();
  DAT_00574cf8 = iVar2;
  iStack_48 = 0x17f;
  uStack_4c = 0x1ef;
  uStack_50 = (int)&uStack_38 + 3;
  puStack_54 = &stack0xffffffd4;
  puStack_58 = &stack0xffffffd0;
  iStack_5c = 1;
  iStack_60 = 0x4ca86a;
  DAT_00574cfc = unaff_EDI;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))();
  DAT_00574d00 = iStack_48;
  DAT_00574d04 = iStack_44;
  DAT_010ed8ff = 0;
  if (DAT_00574cf8 < 0) {
    DAT_00574cf8 = 0;
  }
  else if (0x1ef < DAT_00574cf8) {
    DAT_00574cf8 = 0x1ef;
  }
  if (DAT_00574cfc < 0) {
    DAT_00574cfc = 0;
  }
  else if (0x17f < DAT_00574cfc) {
    DAT_00574cfc = 0x17f;
  }
  if (iStack_48 < 0) {
    DAT_00574d00 = 0;
  }
  else if (0x1ef < iStack_48) {
    DAT_00574d00 = 0x1ef;
  }
  if (iStack_44 < 0) {
    DAT_00574d04 = 0;
  }
  else if (0x17f < iStack_44) {
    DAT_00574d04 = 0x17f;
  }
  iStack_60 = 0xff;
  iVar2 = 0;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))(0,&iStack_48,&iStack_44,(int)&uStack_50 + 2);
  _DAT_005aa890 = (float)iStack_60;
  _DAT_005aa898 = (float)(0xff - iStack_5c);
  puStack_58._0_1_ = (undefined1)(int)ROUND(_DAT_005aa890);
  DAT_005aa70c = puStack_58._0_1_;
  puStack_58 = (undefined1 *)(int)ROUND(_DAT_005aa898);
  DAT_005aa70e = puStack_58._0_1_;
  puVar1 = &stack0xffffff9b;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))(1,&iStack_60,&iStack_5c,puVar1,0xff);
  _DAT_005aa894 = 0x437f0000;
  DAT_005aa875 = 0xff;
  _DAT_005aa89c = (float)(0xff - iVar2);
  DAT_005aa70d = 0xff;
  DAT_005aa70f = (undefined1)(int)ROUND(_DAT_005aa89c);
  if (((uint)puVar1 & 0x10000) != 0) {
    DAT_005aa875 = 0xfe;
  }
  if (((uint)puVar1 & 0x1000000) != 0) {
    DAT_005aa875 = DAT_005aa875 & 0xfd;
  }
  if (((uint)puVar1 & 0x20000) != 0) {
    DAT_005aa875 = DAT_005aa875 & 0xef;
  }
  if (((uint)puVar1 & 0x2000000) != 0) {
    DAT_005aa875 = DAT_005aa875 & 0xdf;
  }
  *(undefined4 *)(*(int *)(*(int *)(param_1 + 0xc) + 0x5c) + 0x24c) =
       *(undefined4 *)(*(int *)(param_1 + 0xc) + 0x20);
  return;
}


// ===== FUN_004caa80 @ 004caa80 (722 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004caa80(int param_1)

{
  int unaff_EDI;
  undefined1 *puVar1;
  int iVar2;
  int iStack_60;
  int iStack_5c;
  undefined1 *puStack_58;
  undefined1 *puStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  int iStack_48;
  int iStack_44;
  undefined1 *puStack_40;
  undefined1 *puStack_3c;
  undefined4 uStack_38;
  undefined4 uStack_34;
  undefined1 local_1e [6];
  undefined1 local_18 [4];
  undefined1 local_14 [8];
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  iVar2 = *(int *)(param_1 + 8);
  if (*(char *)(iVar2 + 0x56a) != '\0') {
    local_c = 0x20001;
    local_8 = 0x230;
    local_4 = **(undefined4 **)(iVar2 + 0x56c);
    uStack_34 = 1;
    uStack_38 = &local_c;
    puStack_3c = (undefined1 *)0x4caac2;
    (**(code **)(iVar2 + 0x58c))();
    uStack_34 = 0x4caac9;
    ShowCursor(0);
  }
  *(undefined1 *)(iVar2 + 0x210) = 1;
  uStack_34 = 0x1ef;
  uStack_38 = (undefined4 *)local_1e;
  puStack_3c = local_14;
  puStack_40 = local_18;
  iStack_44 = 0;
  iStack_48 = 0x4caaf5;
  iVar2 = 0x17f;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))();
  DAT_00574cf8 = iVar2;
  iStack_48 = 0x17f;
  uStack_4c = 0x1ef;
  uStack_50 = (int)&uStack_38 + 3;
  puStack_54 = &stack0xffffffd4;
  puStack_58 = &stack0xffffffd0;
  iStack_5c = 1;
  iStack_60 = 0x4cab2a;
  DAT_00574cfc = unaff_EDI;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))();
  DAT_00574d00 = iStack_48;
  DAT_00574d04 = iStack_44;
  DAT_010ed8ff = 0;
  if (DAT_00574cf8 < 0) {
    DAT_00574cf8 = 0;
  }
  else if (0x1ef < DAT_00574cf8) {
    DAT_00574cf8 = 0x1ef;
  }
  if (DAT_00574cfc < 0) {
    DAT_00574cfc = 0;
  }
  else if (0x17f < DAT_00574cfc) {
    DAT_00574cfc = 0x17f;
  }
  if (iStack_48 < 0) {
    DAT_00574d00 = 0;
  }
  else if (0x1ef < iStack_48) {
    DAT_00574d00 = 0x1ef;
  }
  if (iStack_44 < 0) {
    DAT_00574d04 = 0;
  }
  else if (0x17f < iStack_44) {
    DAT_00574d04 = 0x17f;
  }
  iStack_60 = 0xff;
  iVar2 = 0;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))(0,&iStack_48,&iStack_44,(int)&uStack_50 + 2);
  _DAT_005aa890 = (float)iStack_60;
  _DAT_005aa898 = (float)(0xff - iStack_5c);
  puStack_58._0_1_ = (undefined1)(int)ROUND(_DAT_005aa890);
  DAT_005aa70c = puStack_58._0_1_;
  puStack_58 = (undefined1 *)(int)ROUND(_DAT_005aa898);
  DAT_005aa70e = puStack_58._0_1_;
  puVar1 = &stack0xffffff9b;
  (**(code **)(**(int **)(param_1 + 8) + 0x10))(1,&iStack_60,&iStack_5c,puVar1,0xff);
  _DAT_005aa894 = 0x437f0000;
  _DAT_005aa89c = (float)(0xff - iVar2);
  DAT_005aa70d = 0xff;
  DAT_005aa70f = (undefined1)(int)ROUND(_DAT_005aa89c);
  if (((uint)puVar1 & 0x10000) != 0) {
    DAT_005aa875 = DAT_005aa875 & 0xfe;
  }
  if (((uint)puVar1 & 0x1000000) != 0) {
    DAT_005aa875 = DAT_005aa875 & 0xfd;
  }
  if (((uint)puVar1 & 0x20000) == 0) {
    _DAT_005aa734 = _DAT_005aa734 | 1;
  }
  else {
    _DAT_005aa734 = _DAT_005aa734 & 0xfffffffe;
    DAT_005aa875 = DAT_005aa875 & 0xee;
  }
  if (((uint)puVar1 & 0x2000000) == 0) {
    _DAT_005aa734 = _DAT_005aa734 | 2;
  }
  else {
    _DAT_005aa734 = _DAT_005aa734 & 0xfffffffd;
    DAT_005aa875 = DAT_005aa875 & 0xdd;
  }
  *(undefined4 *)(*(int *)(*(int *)(param_1 + 0xc) + 0x5c) + 0x24c) =
       *(undefined4 *)(*(int *)(param_1 + 0xc) + 0x20);
  return;
}


// ===== FUN_004cad60 @ 004cad60 (137 bytes) =====

void __fastcall FUN_004cad60(int param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  *(undefined4 *)(param_1 + 0x203438) = 1;
  DAT_010ed8e8 = 0;
  FUN_004c3570();
  puVar1 = *(undefined4 **)(*(int *)(param_1 + 4) + 0x28);
  puVar2 = *(undefined4 **)(param_1 + 0x20389c);
  *puVar2 = *puVar1;
  puVar2[1] = puVar1[1];
  _memset(*(void **)(param_1 + 0x203850),0,0x100000);
  _memset(*(void **)(param_1 + 0x203858),0,0x40000);
  FUN_00462ae0();
  FUN_004b33a0();
  if (DAT_010ed8b8 != '\0') {
    DAT_005aa6c4 = 0;
    DAT_010ed8bc = 0;
  }
  return;
}


// ===== FUN_004caf20 @ 004caf20 (489 bytes) =====

void FUN_004caf20(int param_1)

{
  undefined1 uVar1;
  int iVar2;
  BOOL BVar3;
  FILE *_File;
  void *_DstBuf;
  int *piVar4;
  uint uVar5;
  char local_180;
  char local_17f;
  char local_17e;
  char local_17d;
  long local_17c;
  size_t local_178;
  undefined4 local_170;
  void *local_16c;
  tagOFNA local_168;
  CHAR local_110;
  undefined1 local_10f [267];
  
  local_110 = '\0';
  _memset(local_10f,0,0x103);
  _memset(&local_168,0,0x58);
  local_168.lpstrFile = &local_110;
  local_168.lStructSize = 0x58;
  local_168.hwndOwner = (HWND)0x0;
  local_168.lpstrFilter = "SSF Files";
  local_168.lpstrCustomFilter = (LPSTR)0x0;
  local_168.nMaxCustFilter = 0;
  local_168.nFilterIndex = 0;
  local_168.nMaxFile = 0x104;
  local_168.lpstrFileTitle = (LPSTR)0x0;
  local_168.nMaxFileTitle = 0;
  local_168.lpstrInitialDir = (LPCSTR)0x0;
  local_168.lpstrTitle = "Open a file";
  local_168.nFileOffset = 0;
  local_168.nFileExtension = 0;
  local_168.lpstrDefExt = (LPCSTR)0x0;
  local_168.lCustData = 0;
  local_168.lpfnHook = (LPOFNHOOKPROC)0x0;
  local_168.lpTemplateName = (LPCSTR)0x0;
  local_168.Flags = 0x80008;
  BVar3 = GetOpenFileNameA(&local_168);
  if (BVar3 != 0) {
    _File = _fopen(&local_110,"rb");
    if (_File != (FILE *)0x0) {
      _fread(&local_180,0x10,1,_File);
      if ((((local_180 == 'P') && (local_17f == 'S')) && (local_17e == 'F')) &&
         (local_17d == '\x11')) {
        _fseek(_File,local_17c,1);
        _DstBuf = (void *)FUN_0048d2ec(local_178);
        local_16c = _DstBuf;
        piVar4 = (int *)FUN_0048d2ec(0x100000);
        _fread(_DstBuf,1,local_178,_File);
        local_170 = 0x100000;
        FUN_0045df00(piVar4,&local_170,_DstBuf,local_178);
        FID_conflict__memcpy((void *)(*(int *)(param_1 + 0x20389c) + *piVar4),piVar4 + 1,0xffffc);
        _fclose(_File);
        iVar2 = *(int *)(param_1 + 0x20389c);
        uVar5 = 0;
        do {
          uVar1 = *(undefined1 *)(iVar2 + 1 + uVar5 * 2);
          *(undefined1 *)(iVar2 + 1 + uVar5 * 2) = *(undefined1 *)(iVar2 + uVar5 * 2);
          *(undefined1 *)(iVar2 + uVar5 * 2) = uVar1;
          uVar5 = uVar5 + 1;
        } while (uVar5 < 0x80000);
        FUN_0048d1c3(local_16c);
        FUN_0048d1c3(piVar4);
        FUN_00462ae0();
        FUN_004b33a0();
        return;
      }
      MessageBoxA((HWND)0x0,"Error","Error",0x10);
    }
  }
  return;
}


// ===== FUN_004cb1c0 @ 004cb1c0 (33 bytes) =====

uint FUN_004cb1c0(void)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_004c82d0();
  iVar2 = FUN_004c82d0();
  return iVar2 << 0x10 | uVar1 & 0xffff;
}


// ===== io_4cb480 @ 004cb480 (115 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall io_4cb480(char param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_005aa730;
  if (param_1 == '\0') {
    DAT_010ed8f8 = DAT_010ed8f8 + 1;
    _DAT_010f0bb0 = param_2;
    if (*(int *)(DAT_005aa730 + 0x3c) == 0) {
      DAT_005aa74c = DAT_005aa74c + 1;
      (&DAT_005aa6e0)[DAT_005a96be] = (char)param_2;
      DAT_005a96be = DAT_005a96be + 1 & 0x1f;
      return;
    }
    DAT_01078799 = DAT_01078799 + '\x01';
    uVar2 = (uint)DAT_010ed9d8;
    DAT_010ed9d8 = DAT_010ed9d8 + 1 & 0x7f;
    (&DAT_010c35b0)[uVar2] = (char)param_2;
    if (*(int *)(*(int *)(iVar1 + 4) + 0x104) != 0) {
      FUN_004b2bc0();
    }
    DAT_005aa74c = DAT_005aa74c + 1;
  }
  return;
}


// ===== io_4cb500 @ 004cb500 (109 bytes) =====

void __fastcall io_4cb500(char param_1,undefined1 param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_005aa730;
  if (param_1 == '\0') {
    DAT_010ed8f8 = DAT_010ed8f8 + 1;
    if (*(int *)(DAT_005aa730 + 0x3c) == 0) {
      DAT_005aa74c = DAT_005aa74c + 1;
      (&DAT_005aa6e0)[DAT_005a96be] = param_2;
      DAT_005a96be = DAT_005a96be + 1 & 0x1f;
      return;
    }
    DAT_01078799 = DAT_01078799 + '\x01';
    uVar2 = (uint)DAT_010ed9d8;
    DAT_010ed9d8 = DAT_010ed9d8 + 1 & 0x7f;
    (&DAT_010c35b0)[uVar2] = param_2;
    if (*(int *)(*(int *)(iVar1 + 4) + 0x104) != 0) {
      FUN_004b2bc0();
    }
    DAT_005aa74c = DAT_005aa74c + 1;
  }
  return;
}


// ===== io_4cb570 @ 004cb570 (109 bytes) =====

void __fastcall io_4cb570(char param_1,undefined1 param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_005aa730;
  if (param_1 == '\0') {
    DAT_010ed8f8 = DAT_010ed8f8 + 1;
    if (*(int *)(DAT_005aa730 + 0x3c) == 0) {
      DAT_005aa74c = DAT_005aa74c + 1;
      (&DAT_005aa6e0)[DAT_005a96be] = param_2;
      DAT_005a96be = DAT_005a96be + 1 & 0x1f;
      return;
    }
    DAT_01078799 = DAT_01078799 + '\x01';
    uVar2 = (uint)DAT_010ed9d8;
    DAT_010ed9d8 = DAT_010ed9d8 + 1 & 0x7f;
    (&DAT_010c35b0)[uVar2] = param_2;
    if (*(int *)(*(int *)(iVar1 + 4) + 0x104) != 0) {
      FUN_004b2bc0();
    }
    DAT_005aa74c = DAT_005aa74c + 1;
  }
  return;
}


// ===== io_4cb5e0 @ 004cb5e0 (644 bytes) =====

void __fastcall io_4cb5e0(uint param_1,byte param_2)

{
  int iVar1;
  uint uVar2;
  
  param_1 = param_1 & 0xffff;
  uVar2 = (uint)param_2;
  (**(code **)(DAT_005aa8c0 + 0x74))();
  if (param_1 == 0) {
    DAT_010ed900 = uVar2 & 1;
    *(uint *)(DAT_005aa730 + 0x202408) = param_2 >> 5 & 1;
    FUN_004ece50();
    iVar1 = DAT_005aa730;
    uVar2 = ~(uint)(param_2 >> 6) & 1;
    *(uint *)(DAT_005aa730 + 0x20240c) = uVar2;
    if (uVar2 == 0) {
      return;
    }
    *(undefined4 *)(iVar1 + 0x2023d0) = 0;
    *(undefined4 *)(iVar1 + 0x202414) = 0;
    *(undefined4 *)(iVar1 + 0x20241c) = 5;
    return;
  }
  if (param_1 == 0x1e) {
    DAT_010ed8fe = param_2 & 7;
    return;
  }
  if (param_1 == 0x12) {
    if (param_2 != 0x87) {
      return;
    }
    DAT_010ed8ff = DAT_010ed8ff + '\x01';
    return;
  }
  if (param_1 == 0x14) {
    DAT_010ed8ff = param_2;
    return;
  }
  if (param_1 != 8) {
    if (param_1 == 10) {
      DAT_00574cf0 = uVar2;
      return;
    }
    uVar2 = DAT_00574cf4;
    if (param_1 != 0x22) {
      if (param_1 < 0x200) {
        return;
      }
      if (0 < *(int *)(DAT_005aa730 + 0x3c)) {
        return;
      }
      *(byte *)(DAT_005aa71c + 0x7e00 + param_1) = param_2;
      return;
    }
  }
  DAT_00574cf4 = uVar2;
  if (param_2 == 0x83) {
    return;
  }
  if (param_2 == 0) {
    return;
  }
  if (DAT_005aa708 == 0) {
    return;
  }
  if (DAT_0057285b == param_2) {
    return;
  }
  DAT_0057285b = param_2;
  if (DAT_005aa708 == 1) {
    switch(param_2 & 0xf0) {
    case 0:
      if ((param_2 & 2) == 0) {
        (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,0,0);
        return;
      }
      break;
    case 0x10:
switchD_004c7744_caseD_10:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,0,0);
      return;
    case 0x20:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,5,param_2 & 0xf);
      return;
    case 0x30:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,1,param_2 & 0xf);
      return;
    case 0x40:
switchD_004c7744_caseD_40:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,2,param_2 & 0xf);
      return;
    case 0x50:
      goto switchD_004c7744_caseD_50;
    case 0x60:
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,3,param_2 & 0xf);
      return;
    }
  }
  else {
    if (DAT_005aa708 != 3) {
      return;
    }
    switch(param_2 & 0xf0) {
    case 0x10:
      goto switchD_004c7744_caseD_40;
    default:
      goto switchD_004c7744_default;
    case 0x80:
    case 0x90:
      if ((param_2 & 0x1f) != 0) {
        (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,3,param_2 >> 1 & 0xf);
        return;
      }
      (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,0,0);
      return;
    case 0xc0:
    case 0xd0:
      if ((param_2 & 0x1f) == 0) goto switchD_004c7744_caseD_10;
      param_2 = param_2 >> 1;
    }
switchD_004c7744_caseD_50:
    (**(code **)(**(int **)(DAT_005aa730 + 8) + 0x4c))(0,4,param_2 & 0xf);
  }
switchD_004c7744_default:
  return;
}


// ===== FUN_004cb700 @ 004cb700 (27 bytes) =====

undefined1 FUN_004cb700(void)

{
  undefined1 uVar1;
  
  FUN_004c88a0();
  uVar1 = (**(code **)(DAT_005aa8c0 + 0x70))();
  return uVar1;
}


// ===== FUN_004cb720 @ 004cb720 (27 bytes) =====

undefined1 FUN_004cb720(void)

{
  undefined1 uVar1;
  
  FUN_004c88a0();
  uVar1 = (**(code **)(DAT_005aa8c0 + 0x70))();
  return uVar1;
}


// ===== FUN_004cb740 @ 004cb740 (23 bytes) =====

void FUN_004cb740(void)

{
  FUN_004c88a0();
                    /* WARNING: Could not recover jumptable at 0x004cb755. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(DAT_005aa8c0 + 0x70))();
  return;
}


// ===== FUN_004cb870 @ 004cb870 (267 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004cb870(int param_1)

{
  int iVar1;
  
  _DAT_005aa8bc = 1;
  FUN_004ca450();
  DAT_005aa70c = (char)((uint)DAT_005aa70c * 0xc0 >> 8) + ' ';
  if (DAT_010ed8fc != '\0') {
    *(undefined4 *)(param_1 + 0x203438) = 0;
  }
  if ((DAT_005aa875 & 1) == 0) {
    *(undefined4 *)(param_1 + 0x203438) = 1;
  }
  else if ((DAT_005aa875 & 2) == 0) {
    *(undefined4 *)(param_1 + 0x203438) = 2;
  }
  else if ((DAT_005aa875 & 4) == 0) {
    *(undefined4 *)(param_1 + 0x203438) = 3;
  }
  else if ((DAT_005aa875 & 8) == 0) {
    *(undefined4 *)(param_1 + 0x203438) = 4;
  }
  else if ((DAT_005aa875 & 0x10) == 0) {
    *(undefined4 *)(param_1 + 0x203438) = 0;
  }
  DAT_005aa875 = DAT_005aa875 & 0x8f;
  iVar1 = *(int *)(param_1 + 0x203438);
  if (iVar1 == 1) {
    DAT_005aa875 = DAT_005aa875 | 0x20;
  }
  else if (iVar1 == 2) {
    DAT_005aa875 = DAT_005aa875 | 0x10;
  }
  else if (iVar1 == 3) {
    DAT_005aa875 = DAT_005aa875 | 0x60;
  }
  else if (iVar1 == 4) {
    DAT_005aa875 = DAT_005aa875 | 0x50;
  }
  if (DAT_0057285b == '\x01') {
    DAT_005aa876 = DAT_005aa70c;
    return;
  }
  if (DAT_0057285b == '\x02') {
    DAT_005aa876 = 0xff;
    return;
  }
  if (DAT_0057285b == '\x03') {
    DAT_005aa876 = (undefined1)DAT_00574cf0;
    return;
  }
  DAT_005aa876 = DAT_0057285b;
  return;
}


// ===== FUN_004cbae0 @ 004cbae0 (2074 bytes) =====

undefined4 __fastcall FUN_004cbae0(int param_1)

{
  ushort uVar1;
  int *piVar2;
  undefined4 *puVar3;
  char cVar4;
  ushort *puVar5;
  int iVar6;
  ushort *puVar7;
  uint uVar8;
  undefined4 uVar9;
  int iVar10;
  code *pcVar11;
  uint unaff_EDI;
  undefined4 uVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  
  piVar2 = *(int **)(*(int *)(param_1 + 0xc) + 4);
  iVar6 = *(int *)(param_1 + 0x20385c);
  cVar4 = '\0';
  iVar13 = 0;
  (**(code **)(*piVar2 + 0xac))(piVar2,0,0,1,0,0,0);
  pcVar11 = QueryPerformanceCounter_exref;
  if (*(char *)(param_1 + 0x2d) == '\0') {
    uVar8 = unaff_EDI & 0xff;
    goto LAB_004cc00e;
  }
  if ((DAT_010ed8c8 == 0) || (DAT_005aa76c % (DAT_010ed8c8 + 1U) == 0)) {
    iVar14 = -iVar6;
    iVar15 = 4;
    puVar5 = (ushort *)(iVar6 + 4);
    uVar8 = 2;
    do {
      if ((uVar8 - 2 & 0xf) == 0) {
        *(ushort *)((int)puVar5 + *(int *)(param_1 + 0x203444) + iVar14 + -4) =
             *(ushort *)(*(int *)(param_1 + 0x20344c) + (puVar5[-2] & 0x7fff) * 2) | 0xf000;
      }
      else {
        *(undefined2 *)((int)puVar5 + *(int *)(param_1 + 0x203444) + iVar14 + -4) =
             *(undefined2 *)(*(int *)(param_1 + 0x20344c) + (puVar5[-2] & 0x7fff) * 2);
      }
      if ((uVar8 - 1 & 0xf) == 0) {
        *(ushort *)((int)puVar5 + *(int *)(param_1 + 0x203444) + iVar14 + -2) =
             *(ushort *)(*(int *)(param_1 + 0x20344c) + (puVar5[-1] & 0x7fff) * 2) | 0xf000;
      }
      else {
        *(undefined2 *)((int)puVar5 + *(int *)(param_1 + 0x203444) + iVar14 + -2) =
             *(undefined2 *)(*(int *)(param_1 + 0x20344c) + (puVar5[-1] & 0x7fff) * 2);
      }
      if ((uVar8 & 0xf) == 0) {
        *(ushort *)(iVar15 + *(int *)(param_1 + 0x203444)) =
             *(ushort *)(*(int *)(param_1 + 0x20344c) + (*puVar5 & 0x7fff) * 2) | 0xf000;
      }
      else {
        *(undefined2 *)(iVar15 + *(int *)(param_1 + 0x203444)) =
             *(undefined2 *)(*(int *)(param_1 + 0x20344c) + (*puVar5 & 0x7fff) * 2);
      }
      if ((uVar8 + 1 & 0xf) == 0) {
        *(ushort *)((int)puVar5 + *(int *)(param_1 + 0x203444) + (2 - iVar6)) =
             *(ushort *)(*(int *)(param_1 + 0x20344c) + (puVar5[1] & 0x7fff) * 2) | 0xf000;
      }
      else {
        *(undefined2 *)((int)puVar5 + *(int *)(param_1 + 0x203444) + (2 - iVar6)) =
             *(undefined2 *)(*(int *)(param_1 + 0x20344c) + (puVar5[1] & 0x7fff) * 2);
      }
      iVar15 = iVar15 + 8;
      iVar10 = uVar8 + 2;
      puVar5 = puVar5 + 4;
      uVar8 = uVar8 + 4;
    } while (iVar10 < 0x1000);
    pcVar11 = QueryPerformanceCounter_exref;
    if (*(char *)(param_1 + 0x1c) == '\0') {
      FUN_004c6ab0();
      pcVar11 = QueryPerformanceCounter_exref;
      iVar6 = *(int *)(param_1 + 0x18);
      if (*(char *)(iVar6 + 0x28d) != '\0') {
        QueryPerformanceCounter((LARGE_INTEGER *)(iVar6 + 0x28));
        *(char **)(iVar6 + 0x38) = "Tilemap Generation";
      }
      puVar3 = *(undefined4 **)(param_1 + 0x2038a0);
      iVar6 = (**(code **)(*(int *)puVar3[8] + 0x4c))((int *)puVar3[8],0,puVar3,0,0x2000);
      if (iVar6 < 0) {
        uVar9 = 0;
      }
      else {
        uVar9 = puVar3[1];
        puVar3[9] = *puVar3;
      }
      puVar3 = *(undefined4 **)(param_1 + 0x2038a4);
      iVar6 = (**(code **)(*(int *)puVar3[8] + 0x4c))((int *)puVar3[8],0,puVar3,0,0x2000);
      if (iVar6 < 0) {
        uVar12 = 0;
      }
      else {
        puVar3[9] = *puVar3;
        uVar12 = puVar3[1];
      }
      FUN_00451a90(uVar9,uVar12,*(undefined4 *)(*(int *)(param_1 + 0x2038a0) + 0x24),
                   *(undefined4 *)(param_1 + 0x203444));
      piVar2 = *(int **)(*(int *)(param_1 + 0x2038a0) + 0x20);
      (**(code **)(*piVar2 + 0x50))(piVar2,0);
      piVar2 = *(int **)(*(int *)(param_1 + 0x2038a4) + 0x20);
      (**(code **)(*piVar2 + 0x50))(piVar2,0);
      if (*(char *)(*(int *)(param_1 + 0x18) + 0x28d) != '\0') {
        QueryPerformanceCounter((LARGE_INTEGER *)(*(int *)(param_1 + 0x18) + 0x30));
      }
      goto LAB_004cbd71;
    }
  }
  else {
LAB_004cbd71:
    iVar6 = *(int *)(param_1 + 0x203854) + 0x9810;
    if (*(short *)(*(int *)(param_1 + 0x203854) + 0x11d60) != 0) {
      puVar3 = *(undefined4 **)(param_1 + 0x2038a8);
      iVar15 = (**(code **)(*(int *)puVar3[8] + 0x4c))((int *)puVar3[8],0,puVar3,0,0x2000);
      if (iVar15 < 0) {
        iVar15 = 0;
      }
      else {
        puVar3[9] = *puVar3;
        iVar15 = puVar3[1];
      }
      iVar14 = 0x180;
      do {
        puVar5 = (ushort *)(iVar15 + 6);
        puVar7 = (ushort *)(iVar6 + 4);
        iVar10 = 0x7c;
        do {
          uVar1 = *(ushort *)(*(int *)(param_1 + 0x20344c) + (puVar7[-2] & 0x7fff) * 2);
          puVar5[-3] = (uVar1 & 0xff) << 4 | uVar1 >> 8 & 0xf;
          uVar1 = *(ushort *)(*(int *)(param_1 + 0x20344c) + (puVar7[-1] & 0x7fff) * 2);
          puVar5[-2] = (uVar1 & 0xff) << 4 | uVar1 >> 8 & 0xf;
          uVar1 = *(ushort *)(*(int *)(param_1 + 0x20344c) + (*puVar7 & 0x7fff) * 2);
          *(ushort *)((iVar15 - iVar6) + (int)puVar7) = (uVar1 & 0xff) << 4 | uVar1 >> 8 & 0xf;
          uVar1 = *(ushort *)(*(int *)(param_1 + 0x20344c) + (puVar7[1] & 0x7fff) * 2);
          *puVar5 = (uVar1 & 0xff) << 4 | uVar1 >> 8 & 0xf;
          puVar7 = puVar7 + 4;
          puVar5 = puVar5 + 4;
          iVar10 = iVar10 + -1;
        } while (iVar10 != 0);
        iVar6 = iVar13 + 0x400;
        iVar14 = iVar14 + -1;
        iVar15 = iVar15 + (*(uint *)(*(int *)(param_1 + 0x2038a8) + 0x24) & 0xfffffffe);
        iVar13 = iVar6;
      } while (iVar14 != 0);
      piVar2 = *(int **)(*(int *)(param_1 + 0x2038a8) + 0x20);
      (**(code **)(*piVar2 + 0x50))(piVar2,0);
      cVar4 = '\x01';
      pcVar11 = QueryPerformanceCounter_exref;
    }
    if (*(char *)(*(int *)(param_1 + 0x18) + 0x28d) != '\0') {
      (*pcVar11)(*(int *)(param_1 + 0x18) + 0x30);
    }
  }
  iVar6 = *(int *)(param_1 + 0x18);
  uVar8 = *(uint *)(param_1 + 0x2023b8);
  if (*(char *)(iVar6 + 0x28d) != '\0') {
    (*pcVar11)(iVar6 + 0x40);
    *(char **)(iVar6 + 0x50) = "Tilemap Render (Low)";
  }
  if (*(char *)(*(int *)(param_1 + 0xc) + 0x6e) != '\0') {
    piVar2 = *(int **)(*(int *)(param_1 + 0xc) + 4);
    (**(code **)(*piVar2 + 0xac))(piVar2,0,0,4,0,0,0);
    (**(code **)(*piVar2 + 0xe4))(piVar2,0x34,1);
    (**(code **)(*piVar2 + 0xe4))(piVar2,0x3a,1);
    (**(code **)(*piVar2 + 0xe4))(piVar2,0x3b,1);
    (**(code **)(*piVar2 + 0xe4))(piVar2,0x39,1);
    (**(code **)(*piVar2 + 0xe4))(piVar2,0x38,8);
    (**(code **)(*piVar2 + 0xe4))(piVar2,0x37,3);
  }
  if (cVar4 != '\0') {
    FUN_004ed6f0(0x90,*(undefined1 *)(param_1 + 0x2e),DAT_010ed8ba);
  }
  if (((uVar8 & 4) != 0) && (cVar4 == '\0')) {
    FUN_004ed6f0(0xff,*(undefined1 *)(param_1 + 0x2e),DAT_010ed8ba);
  }
  if ((uVar8 & 1) != 0) {
    FUN_004ed6f0(0x90,*(undefined1 *)(param_1 + 0x2e),DAT_010ed8bb);
  }
  if (*(char *)(*(int *)(param_1 + 0x18) + 0x28d) != '\0') {
    (*pcVar11)(*(int *)(param_1 + 0x18) + 0x48);
  }
LAB_004cc00e:
  iVar6 = *(int *)(param_1 + 0x18);
  if (*(char *)(iVar6 + 0x28d) != '\0') {
    (*pcVar11)(iVar6 + 0x70);
    *(char **)(iVar6 + 0x80) = "Polygon Render";
  }
  (**(code **)(*(int *)(param_1 + 0x2023c8) + 0x4c))();
  if (*(char *)(*(int *)(param_1 + 0x18) + 0x28d) != '\0') {
    (*pcVar11)(*(int *)(param_1 + 0x18) + 0x78);
  }
  if (*(char *)(param_1 + 0x2d) != '\0') {
    iVar6 = *(int *)(param_1 + 0x18);
    if (*(char *)(iVar6 + 0x28d) != '\0') {
      (*pcVar11)(iVar6 + 0x58);
      *(char **)(iVar6 + 0x68) = "Tilemap Render (High)";
    }
    if ((uVar8 & 8) != 0) {
      FUN_004ed6f0(0x20,*(undefined1 *)(param_1 + 0x2e),DAT_010ed8c4);
    }
    if ((uVar8 & 2) != 0) {
      FUN_004ed6f0(0x20,*(undefined1 *)(param_1 + 0x2e),DAT_010ed8c5);
    }
    if ((*(char *)(*(int *)(param_1 + 0xc) + 0x6e) != '\0') && ((uVar8 & 0xc) != 0)) {
      piVar2 = *(int **)(*(int *)(param_1 + 0xc) + 4);
      (**(code **)(*piVar2 + 0xe4))(piVar2,0x38,6);
      (**(code **)(*piVar2 + 0xe4))(piVar2,0x37,1);
      FUN_004ede10(*(undefined1 *)(param_1 + 0x2e));
      (**(code **)(*piVar2 + 0xe4))(piVar2,0x34,0);
    }
    if (*(char *)(*(int *)(param_1 + 0x18) + 0x28d) != '\0') {
      (*pcVar11)(*(int *)(param_1 + 0x18) + 0x60);
    }
  }
  if (*(char *)(param_1 + 0x2c) != '\0') {
    FUN_004ed5e0(2,0x80);
    _sprintf(&stack0xfffffedc,"Objects: %d",DAT_00dcec38);
    FUN_004ed5e0(2,0x94);
    _sprintf(&stack0xfffffedc,"Triangles: %d /   %d per sec",DAT_00dcec20,
             *(undefined4 *)(param_1 + 0x38));
    FUN_004ed5e0(2,0xa8);
    _sprintf(&stack0xfffffedc,"Textures cached: %d",DAT_00dceff0);
    FUN_004ed5e0(2,0xbc);
    _sprintf(&stack0xfffffedc,"Textures updated this frame: %d",DAT_00dcec54);
    FUN_004ed5e0(2,0xd0);
    _sprintf(&stack0xfffffedc,"Effective CPU Time: %d%%",*(undefined4 *)(param_1 + 0x20342c));
    FUN_004ed5e0(2,0xe4);
    *(int *)(param_1 + 0x34) = *(int *)(param_1 + 0x34) + DAT_00dcec20;
    if (DAT_005aa76c % 0x3c == 0) {
      *(undefined4 *)(param_1 + 0x38) = *(undefined4 *)(param_1 + 0x34);
      *(undefined4 *)(param_1 + 0x34) = 0;
    }
  }
  cVar4 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x2a);
  if (((cVar4 == '\0') ||
      (cVar4 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x36), cVar4 == '\0')) ||
     (cVar4 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x20), cVar4 == '\0')) {
    DAT_010f01b4 = '\0';
  }
  else if (DAT_010f01b4 == '\0') {
    FUN_004b7ba0();
    DAT_010f01b4 = '\x01';
  }
  cVar4 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x2a);
  if (((cVar4 == '\0') ||
      (cVar4 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x36), cVar4 == '\0')) ||
     (cVar4 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x13), cVar4 == '\0')) {
    DAT_010f01b5 = '\0';
  }
  else if (DAT_010f01b5 == '\0') {
    FUN_004b7c60();
    DAT_010f01b5 = 1;
    return 1;
  }
  return 1;
}


// ===== io_4cc300 @ 004cc300 (144 bytes) =====

void __fastcall io_4cc300(undefined *param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = DAT_005aa72c;
  *(undefined4 *)(((uint)param_1 & 0xffff) + DAT_005aa704) = param_2;
  if (param_1 < &DAT_01008000) {
    FUN_004c0380();
  }
  if (((undefined *)0x100bfff < param_1) && (param_1 < (undefined *)0x100e000)) {
    *(undefined1 *)(iVar1 + 0x11) = 1;
  }
  if (param_1 + 2 < &DAT_01008000) {
    FUN_004c0380();
  }
  if (param_1 + -0x100bffe < (undefined *)0x2000) {
    *(undefined1 *)(iVar1 + 0x11) = 1;
  }
  return;
}


// ===== io_4cc390 @ 004cc390 (88 bytes) =====

void __fastcall io_4cc390(undefined *param_1,undefined2 param_2)

{
  int iVar1;
  
  iVar1 = DAT_005aa72c;
  *(undefined2 *)(((uint)param_1 & 0xffff) + DAT_005aa704) = param_2;
  if (param_1 < &DAT_01008000) {
    FUN_004c0380();
  }
  if (((undefined *)0x100bfff < param_1) && (param_1 < (undefined *)0x100e000)) {
    *(undefined1 *)(iVar1 + 0x11) = 1;
  }
  return;
}


// ===== io_4cc3f0 @ 004cc3f0 (87 bytes) =====

void __fastcall io_4cc3f0(undefined *param_1,undefined1 param_2)

{
  int iVar1;
  
  iVar1 = DAT_005aa72c;
  *(undefined1 *)(((uint)param_1 & 0xffff) + DAT_005aa704) = param_2;
  if (param_1 < &DAT_01008000) {
    FUN_004c0380();
  }
  if (((undefined *)0x100bfff < param_1) && (param_1 < (undefined *)0x100e000)) {
    *(undefined1 *)(iVar1 + 0x11) = 1;
  }
  return;
}


// ===== FUN_004cc530 @ 004cc530 (107 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004cc530(undefined *param_1,uint param_2)

{
  if (param_1 == &DAT_00c70001) {
    _DAT_0108d8b8 = param_2 >> 8 & 7;
  }
  else if (param_1 < (undefined *)0xc60010) {
    FUN_004b08f0();
  }
  if (param_1 + 1 == &DAT_00c70001) {
    _DAT_0108d8b8 = param_2 & 7;
  }
  else if (param_1 + 1 < (undefined *)0xc60010) {
    FUN_004b08f0();
    return;
  }
  return;
}


// ===== FUN_004cc5a0 @ 004cc5a0 (101 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004cc5a0(undefined *param_1,uint param_2)

{
  if (param_1 == &DAT_00c50001) {
    _DAT_0108a468 = param_2 >> 8 & 7;
  }
  else if (param_1 < (undefined *)0xc40010) {
    FUN_004b08f0();
  }
  if (param_1 + 1 == &DAT_00c50001) {
    _DAT_0108a468 = param_2 & 7;
  }
  else if (param_1 + 1 < (undefined *)0xc40010) {
    FUN_004b08f0();
    return;
  }
  return;
}


// ===== FUN_004cc670 @ 004cc670 (159 bytes) =====

void FUN_004cc670(void)

{
  int in_EAX;
  uint uVar1;
  int iVar2;
  undefined4 local_40 [16];
  
  local_40[2] = 2;
  local_40[3] = 2;
  local_40[4] = 2;
  local_40[5] = 2;
  local_40[6] = 2;
  local_40[7] = 2;
  local_40[8] = 2;
  local_40[9] = 2;
  local_40[10] = 3;
  local_40[0xb] = 3;
  local_40[0] = 0;
  local_40[1] = 1;
  local_40[0xc] = 0;
  local_40[0xd] = 0;
  local_40[0xe] = 0;
  local_40[0xf] = 0;
  if (*(char *)(in_EAX + 0x202398) == '\0') {
    iVar2 = 0;
    while ((uVar1 = 1 << ((byte)iVar2 & 0x1f), (DAT_010ed8e8 & uVar1) == 0 ||
           ((DAT_010ed8e4 & uVar1) == 0))) {
      iVar2 = iVar2 + 1;
      if (0xb < iVar2) {
        return;
      }
    }
    FUN_004c4c60(in_EAX + 0x40,local_40[iVar2]);
  }
  return;
}


// ===== FUN_004cc710 @ 004cc710 (82 bytes) =====

void FUN_004cc710(void)

{
  int in_EAX;
  int iVar1;
  
  if (*(int *)(in_EAX + 0x3c) == 0) {
    iVar1 = 10;
    do {
      DAT_010ed9b0 = 0x4f;
      if (DAT_010482a4 != 0x80) {
        FUN_00462b1c();
      }
      FUN_004a9e00();
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    if (DAT_005aa761 != DAT_005a96be) {
      DAT_00528d38 = 2;
    }
    return;
  }
  FUN_004aefb0();
  return;
}


// ===== FUN_004cc7a0 @ 004cc7a0 (970 bytes) =====

/* WARNING (jumptable): Unable to track spacebase fully for stack */
/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004cc7a0(int param_1)

{
  byte bVar1;
  char cVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined2 in_FPUControlWord;
  int local_e20;
  undefined1 local_e1c [4];
  longlong local_e18;
  
  local_e20 = CONCAT22(local_e20._2_2_,in_FPUControlWord);
  iVar6 = *(int *)(param_1 + 0x18);
  local_e1c = (undefined1  [4])0x0;
  local_e18 = (longlong)ROUND(_DAT_00574dcc);
  uVar4 = (undefined4)(25000000 / (ulonglong)(uint)((int)local_e18 * 0xd2));
  if (*(char *)(iVar6 + 0x28d) != '\0') {
    QueryPerformanceCounter((LARGE_INTEGER *)(iVar6 + 0x88));
    *(char **)(iVar6 + 0x98) = "CPU (i960+DSP)";
  }
  _DAT_00574dcc = *(float *)(*(int *)(param_1 + 0x10) + 0x28);
  if (*(int *)(param_1 + 0x3c) == 0) {
    local_e18 = (longlong)ROUND(_DAT_00574dcc);
    uVar4 = (undefined4)(50000000 / (ulonglong)(uint)((int)local_e18 * 0xd2));
  }
  local_e20 = 0xd2;
  do {
    i960_execute(uVar4);
    FUN_004c8390();
    FUN_004cc670();
    if (*(char *)(*(int *)(param_1 + 0x18) + 0x286) != '\0') {
      FUN_004cc710();
    }
    if (DAT_010ed8f8 == 0) {
      if (*(int *)(param_1 + 0x202394) == 0) goto LAB_004cc8c1;
    }
    else {
      DAT_010ed8f8 = DAT_010ed8f8 + -1;
      DAT_010ed8e4 = DAT_010ed8e4 | 0x400;
      *(undefined4 *)(param_1 + 0x202394) = 0;
LAB_004cc8c1:
      local_e1c = (undefined1  [4])((int)local_e1c + 1);
    }
    if (*(char *)(param_1 + 0x2f) != '\0') {
      *(undefined4 *)(param_1 + 0x202394) = 0;
    }
    if (*(int *)(param_1 + 0x3c) == 0) {
      DAT_005aa760 = DAT_005aa760 ^ 0x80;
    }
    local_e20 = local_e20 + -1;
  } while (local_e20 != 0);
  if (*(int *)(param_1 + 0x3c) == 3) {
    DAT_010ed8e4 = DAT_010ed8e4 | 0x405;
  }
  else {
    DAT_010ed8e4 = DAT_010ed8e4 | 0x401;
  }
  FUN_004cc670();
  if ((((DAT_010ed8f4 & 4) == 0) && ((DAT_005aa76c & 1) != 0)) || ((DAT_010ed8f4 & 4) != 0)) {
    DAT_010ed8dc = DAT_010ed8dc ^ 4;
    *(int *)(param_1 + 0x20342c) = ((int)local_e1c * 100) / 0xd2;
  }
  *(undefined4 *)(param_1 + 0x202394) = 0;
  DAT_010ed8ec = 0;
  if (DAT_010ed8f0 != 0) {
    FUN_004c6b10();
  }
  DAT_005aa76c = DAT_005aa76c + 1;
  DAT_010ed8f0 = 0;
  uVar5 = (**(code **)(**(int **)(param_1 + 8) + 0xc))(7);
  if (((uVar5 & 1) != 0) && ((DAT_005aa87b & 1) == 0)) {
    *(bool *)(param_1 + 0x2c) = *(char *)(param_1 + 0x2c) == '\0';
  }
  bVar1 = (**(code **)(**(int **)(param_1 + 8) + 0xc))(7);
  if (((bVar1 & 2) != 0) && ((DAT_005aa87b & 2) == 0)) {
    *(bool *)(param_1 + 0x2d) = *(char *)(param_1 + 0x2d) == '\0';
  }
  uVar5 = (**(code **)(**(int **)(param_1 + 8) + 0xc))(7);
  if (((uVar5 & 4) != 0) && ((DAT_005aa87b & 4) == 0)) {
    DAT_010ed8c8 = (DAT_010ed8c8 + 1) % 5;
    switch(DAT_010ed8c8) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    default:
      goto switchD_004cca01_default;
    }
    FUN_004f2d30();
  }
switchD_004cca01_default:
  cVar2 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x1d);
  if ((cVar2 != '\0') &&
     (cVar2 = (**(code **)(**(int **)(param_1 + 8) + 0x44))(0x3f), cVar2 != '\0')) {
    FUN_004caf20(param_1);
  }
  iVar6 = 0;
  do {
    uVar3 = (**(code **)(**(int **)(param_1 + 8) + 0xc))(iVar6);
    (&DAT_005aa874)[iVar6] = uVar3;
    iVar6 = iVar6 + 1;
  } while (iVar6 < 8);
  if (*(char *)(*(int *)(param_1 + 0x18) + 0x28d) != '\0') {
    QueryPerformanceCounter((LARGE_INTEGER *)(*(int *)(param_1 + 0x18) + 0x90));
  }
  uVar4 = DAT_005a96a4;
  if (DAT_010ed8b8 == '\0') {
    return 1;
  }
  if (DAT_010ed8bc != 3) {
    return 1;
  }
  if (DAT_010ed8b9 == '\0') goto LAB_004ccb43;
  local_e20 = 0x4d45324d;
  local_e1c[0] = 2;
  puVar7 = (undefined4 *)(DAT_005aa750 + 0x2000);
  puVar8 = (undefined4 *)((int)local_e1c + 1);
  for (iVar6 = 0x380; iVar6 != 0; iVar6 = iVar6 + -1) {
    *puVar8 = *puVar7;
    puVar7 = puVar7 + 1;
    puVar8 = puVar8 + 1;
  }
  if (DAT_010ed8c0 == 0) {
LAB_004ccb08:
    if (DAT_010f01a8 != (FILE *)0x0) goto LAB_004ccb0c;
  }
  else {
    if (DAT_010f01a8 == (FILE *)0x0) {
      DAT_010f01a8 = _fopen("netdumpsend","wb");
      goto LAB_004ccb08;
    }
LAB_004ccb0c:
    _fwrite((void *)((int)local_e1c + 1),0xe00,1,DAT_010f01a8);
  }
  Ordinal_20(uVar4,&local_e20,0xe05,0,&DAT_005aa6c8,0x10);
  if (DAT_010ed8bc != 3) {
    return 1;
  }
LAB_004ccb43:
  if (DAT_005a96bc != '\0') {
    WaitForSingleObject(DAT_005a96a0,2000);
  }
  return 1;
}


// ===== FUN_004ccbe0 @ 004ccbe0 (2058 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004ccbe0(int param_1)

{
  undefined4 uVar1;
  void *_Memory;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  if (*(int *)(param_1 + 0x3c) == 0) {
    FUN_004b3280();
    FUN_004b3280();
    uVar2 = 0;
    do {
      *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
      *(code **)((int)&DAT_010482d4 + uVar2) = io_4c7d40;
      *(code **)((int)&DAT_010482d8 + uVar2) = io_4c7d30;
      *(code **)((int)&DAT_010482dc + uVar2) = io_4c7d20;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0x10000);
    uVar1 = *(undefined4 *)(*(int *)(param_1 + 4) + 0x28);
    uVar2 = 0;
    do {
      *(undefined4 *)((int)&DAT_010482d0 + uVar2) = uVar1;
      *(undefined4 *)((int)&DAT_010482d4 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010482d8 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010482dc + uVar2) = 0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0x200);
    iVar3 = *(int *)(*(int *)(param_1 + 4) + 0x28);
    uVar2 = 0x800;
    do {
      if (iVar3 == -0x20000) {
        *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
      }
      else {
        *(int *)((int)&DAT_010482d0 + uVar2) = iVar3 + -0x60000;
      }
      *(undefined4 *)((int)&DAT_010482d4 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010482d8 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010482dc + uVar2) = 0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xa00);
    uVar2 = 0xc200;
    do {
      *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
      *(undefined1 **)((int)&DAT_010482d4 + uVar2) = &LAB_004c6a40;
      *(code **)((int)&DAT_010482d8 + uVar2) = FUN_004c82d0;
      *(code **)((int)&DAT_010482dc + uVar2) = FUN_004cb1c0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xc300);
    uVar2 = 0xc400;
    do {
      *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
      *(undefined1 **)((int)&DAT_010482d4 + uVar2) = &LAB_004c6a20;
      *(undefined1 **)((int)&DAT_010482d8 + uVar2) = &LAB_004c82b0;
      *(undefined1 **)((int)&DAT_010482dc + uVar2) = &LAB_004cb1a0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xc600);
    uVar2 = 0xc600;
    do {
      *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
      *(undefined1 **)((int)&DAT_010482d4 + uVar2) = &LAB_004c6a10;
      *(undefined1 **)((int)&DAT_010482d8 + uVar2) = &LAB_004c82a0;
      *(undefined1 **)((int)&DAT_010482dc + uVar2) = &LAB_004cb190;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xc800);
    uVar2 = 0xd000;
    do {
      *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
      *(undefined1 **)((int)&DAT_010482d4 + uVar2) = &LAB_004c69f0;
      *(undefined1 **)((int)&DAT_010482d8 + uVar2) = &LAB_004c8260;
      *(undefined1 **)((int)&DAT_010482dc + uVar2) = &LAB_004cb110;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xd100);
    iVar3 = *(int *)(param_1 + 0x20389c);
    uVar2 = 0xf000;
    do {
      if (iVar3 == 0) {
        *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
      }
      else {
        *(int *)((int)&DAT_010482d0 + uVar2) = iVar3 + -0xf00000;
      }
      *(undefined4 *)((int)&DAT_010482d4 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010482d8 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010482dc + uVar2) = 0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xf100);
    uVar2 = 0;
    do {
      *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
      *(code **)((int)&DAT_010582d4 + uVar2) = io_4c7d10;
      *(code **)((int)&DAT_010582d8 + uVar2) = io_4c7d00;
      *(undefined **)((int)&DAT_010582dc + uVar2) = &DAT_004c7cf0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0x10000);
    uVar2 = 0xc100;
    do {
      *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
      *(undefined **)((int)&DAT_010582d4 + uVar2) = &DAT_004c6a30;
      *(undefined **)((int)&DAT_010582d8 + uVar2) = &DAT_004c82c0;
      *(undefined **)((int)&DAT_010582dc + uVar2) = &DAT_004cb1b0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xc200);
    uVar2 = 0xc400;
    do {
      *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
      *(undefined1 **)((int)&DAT_010582d4 + uVar2) = &LAB_004cbab0;
      *(code **)((int)&DAT_010582d8 + uVar2) = FUN_004cc5a0;
      *(undefined1 **)((int)&DAT_010582dc + uVar2) = &LAB_004cc650;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xc600);
    uVar2 = 0xc600;
    do {
      *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
      *(undefined1 **)((int)&DAT_010582d4 + uVar2) = &LAB_004cba80;
      *(code **)((int)&DAT_010582d8 + uVar2) = FUN_004cc530;
      *(undefined1 **)((int)&DAT_010582dc + uVar2) = &LAB_004cc630;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xc800);
    uVar2 = 0xd000;
    do {
      *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
      *(undefined1 **)((int)&DAT_010582d4 + uVar2) = &LAB_004cc610;
      *(undefined1 **)((int)&DAT_010582d8 + uVar2) = &LAB_004cc770;
      *(undefined1 **)((int)&DAT_010582dc + uVar2) = &LAB_004ccb80;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xd100);
    iVar3 = *(int *)(param_1 + 0x20389c);
    uVar2 = 0xf000;
    do {
      if (iVar3 == 0) {
        *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
      }
      else {
        *(int *)((int)&DAT_010582d0 + uVar2) = iVar3 + -0xf00000;
      }
      *(undefined4 *)((int)&DAT_010582d4 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010582d8 + uVar2) = 0;
      *(undefined4 *)((int)&DAT_010582dc + uVar2) = 0;
      uVar2 = uVar2 + 0x10;
    } while (uVar2 < 0xf100);
    _DAT_010482b4 = &PTR_LAB_00572ab8;
    puVar4 = &DAT_01048250;
    puVar5 = &DAT_00528ce4;
    for (iVar3 = 0x20; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
    PTR_DAT_00572abc = &DAT_004b3070;
    _DAT_010482ac = &LAB_004b3090;
    FUN_00462ae0();
    FUN_004b33a0();
    DAT_010e3fc0 = 0;
    DAT_010e3fc8 = 0;
    DAT_010e3fc4 = 0;
    DAT_010e3fcc = 0;
    DAT_010c3a3c = &DAT_007a1200;
    iVar3 = FUN_004eecf0(0);
    *(int *)(param_1 + 0x28) = iVar3 / 2;
    FUN_004eecf0(0xff);
    iVar3 = *(int *)(param_1 + 0x14);
    DAT_01092dc8 = iVar3 + 0x68;
    DAT_01092dcc = iVar3 + 0x2b178;
    DAT_01092dd0 = *(undefined4 *)(param_1 + 0x28);
    DAT_01092dd4 = *(undefined4 *)(iVar3 + 0xac4a8);
    FUN_004b01b0(*(undefined4 *)(*(int *)(param_1 + 4) + 0x30));
    FUN_004b01b0(*(int *)(*(int *)(param_1 + 4) + 0x30) + 0x400000);
    _Memory = DAT_010ed9d4;
    iVar3 = *(int *)(param_1 + 0x14);
    DAT_010e3fe8 = *(undefined4 *)(param_1 + 0x28);
    _DAT_010e3fd4 = iVar3 + 0x2b178;
    _DAT_010e3fd0 = iVar3 + 0x68;
    DAT_010e3fec = *(undefined4 *)(iVar3 + 0xac4a8);
    DAT_010e3fd8 = _DAT_010e3fd0;
    DAT_010e3fdc = _DAT_010e3fd0;
    DAT_010e3fe0 = _DAT_010e3fd4;
    DAT_010e3fe4 = _DAT_010e3fd4;
    if (DAT_010ed9d4 != (void *)0x0) {
      if (DAT_0107c7d4 != (void *)0x0) {
        _free(DAT_0107c7d4);
      }
      _free(_Memory);
      DAT_010ed9d4 = (void *)0x0;
    }
    FUN_004b2780();
    FUN_004b2680();
    return *(undefined4 *)(param_1 + 0x28);
  }
  FUN_004b3280();
  FUN_004b3280();
  FUN_004b3280();
  uVar1 = *(undefined4 *)(param_1 + 0x20389c);
  uVar2 = 0;
  do {
    *(undefined4 *)((int)&DAT_010482d0 + uVar2) = uVar1;
    *(undefined4 *)((int)&DAT_010482d4 + uVar2) = 0;
    *(undefined4 *)((int)&DAT_010482d8 + uVar2) = 0;
    *(undefined4 *)((int)&DAT_010482dc + uVar2) = 0;
    uVar2 = uVar2 + 0x10;
  } while (uVar2 < 0x1000);
  iVar3 = *(int *)(*(int *)(param_1 + 4) + 0x28);
  uVar2 = 0x6000;
  do {
    if (iVar3 == 0) {
      *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
    }
    else {
      *(int *)((int)&DAT_010482d0 + uVar2) = iVar3 + -0x600000;
    }
    *(undefined4 *)((int)&DAT_010482d4 + uVar2) = 0;
    *(undefined4 *)((int)&DAT_010482d8 + uVar2) = 0;
    *(undefined4 *)((int)&DAT_010482dc + uVar2) = 0;
    uVar2 = uVar2 + 0x10;
  } while (uVar2 < 0x6800);
  FUN_004b32c0(0x800000,*(undefined4 *)(*(int *)(param_1 + 4) + 0x30));
  uVar2 = 0x1000;
  do {
    *(undefined4 *)((int)&DAT_010482d0 + uVar2) = 0;
    *(code **)((int)&DAT_010482d4 + uVar2) = FUN_004ae870;
    *(undefined1 **)((int)&DAT_010482d8 + uVar2) = &LAB_004ae800;
    *(undefined1 **)((int)&DAT_010482dc + uVar2) = &LAB_004ab220;
    uVar2 = uVar2 + 0x10;
  } while (uVar2 < 0x1100);
  uVar1 = *(undefined4 *)(param_1 + 0x20389c);
  uVar2 = 0;
  do {
    *(undefined4 *)((int)&DAT_010582d0 + uVar2) = uVar1;
    *(undefined4 *)((int)&DAT_010582d4 + uVar2) = 0;
    *(undefined4 *)((int)&DAT_010582d8 + uVar2) = 0;
    *(undefined4 *)((int)&DAT_010582dc + uVar2) = 0;
    uVar2 = uVar2 + 0x10;
  } while (uVar2 < 0x1000);
  uVar2 = 0x1000;
  do {
    *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
    *(undefined1 **)((int)&DAT_010582d4 + uVar2) = &LAB_004afb00;
    *(undefined1 **)((int)&DAT_010582d8 + uVar2) = &LAB_004afa20;
    *(undefined1 **)((int)&DAT_010582dc + uVar2) = &LAB_004af900;
    uVar2 = uVar2 + 0x10;
  } while (uVar2 < 0x1100);
  _DAT_0105c2d0 = 0;
  _DAT_0105c2d4 = &DAT_004c6a90;
  _DAT_0105c2d8 = &DAT_004c8380;
  _DAT_0105c2dc = &DAT_004cb1f0;
  uVar2 = 0x6000;
  do {
    *(undefined4 *)((int)&DAT_010582d0 + uVar2) = 0;
    *(undefined **)((int)&DAT_010582d4 + uVar2) = &DAT_004c69e0;
    *(undefined **)((int)&DAT_010582d8 + uVar2) = &DAT_004c69d0;
    *(undefined **)((int)&DAT_010582dc + uVar2) = &DAT_004c69c0;
    uVar2 = uVar2 + 0x10;
  } while (uVar2 < 0x6800);
  puVar4 = *(undefined4 **)(*(int *)(param_1 + 4) + 0x28);
  puVar5 = *(undefined4 **)(param_1 + 0x20389c);
  *puVar5 = *puVar4;
  puVar5[1] = puVar4[1];
  _DAT_010482b4 = &PTR_LAB_00572ab8;
  puVar4 = &DAT_01048250;
  puVar5 = &DAT_00528ce4;
  for (iVar3 = 0x20; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  PTR_DAT_00572abc = &DAT_004b3070;
  _DAT_010482ac = &LAB_004b3090;
  FUN_00462ae0();
  FUN_004b33a0();
  FUN_004af2c0();
  DAT_0109bd1c = *(undefined4 *)(param_1 + 0x20389c);
  _DAT_0109bd24 = _malloc(0x100000);
  _memset(_DAT_0109bd24,0,0x100000);
  _DAT_010bfd64 = param_1 + 0x203428;
  _DAT_010482ac = &LAB_004aeac0;
  iVar3 = FUN_004eecf0(0);
  *(int *)(param_1 + 0x28) = iVar3 / 2;
  FUN_004eecf0(0xff);
  DAT_010bfd60 = *(undefined4 *)(param_1 + 0x28);
  DAT_010bf558 = *(int *)(param_1 + 0x14) + 0x68;
  DAT_010bf55c = *(int *)(param_1 + 0x14) + 0x2b178;
  iVar3 = *(int *)(param_1 + 4);
  DAT_010ed99e = 0;
  if (*(int *)(iVar3 + 0x104) == 0) {
    return DAT_010bfd60;
  }
  DAT_0107878c = *(undefined4 *)(iVar3 + 0x38);
  DAT_010787b8 = *(undefined4 *)(iVar3 + 0x40);
  DAT_0107878b = (undefined1)*(int *)(iVar3 + 0x104);
  FUN_004e9de0();
  FUN_004ea040();
  iVar3 = *(int *)(param_1 + 0x14);
  DAT_0107877c = iVar3 + 0x68;
  DAT_01078784 = iVar3 + 0x2b178;
  DAT_01078760 = *(undefined4 *)(param_1 + 0x28);
  DAT_010787b4 = *(undefined4 *)(iVar3 + 0xac4a8);
  return *(undefined4 *)(param_1 + 0x28);
}


// ===== FUN_004cd450 @ 004cd450 (7708 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004cd450(int param_1)

{
  int *piVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  double dVar5;
  undefined8 uVar6;
  UINT UVar7;
  int iVar8;
  undefined4 uVar9;
  undefined4 *puVar10;
  void *_Dst;
  int iVar11;
  int iVar12;
  undefined2 in_FPUControlWord;
  float10 fVar13;
  int local_17c;
  undefined8 local_178;
  undefined1 local_16c;
  char *local_168;
  undefined4 local_164;
  undefined4 local_160;
  int local_154;
  double local_150;
  undefined4 uStack_148;
  double local_140;
  CHAR local_138 [16];
  double local_128;
  double local_120;
  char acStack_118 [260];
  void *pvStack_14;
  undefined1 *puStack_10;
  undefined4 uStack_c;
  
  uStack_c = 0xffffffff;
  puStack_10 = &LAB_0050427e;
  pvStack_14 = ExceptionList;
  DAT_005aa730 = param_1;
  ExceptionList = &pvStack_14;
  *(undefined2 *)(param_1 + 0x2c) = 0x100;
  UVar7 = GetPrivateProfileIntA("Renderer","FilterTilemaps",0,".\\emulator.ini");
  *(bool *)(param_1 + 0x2e) = UVar7 != 0;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203850) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2RAM";
  local_160 = 0x100000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203854) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2FB";
  local_160 = 0x80000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203858) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2RAM2";
  local_160 = 0x40000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x20385c) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2PAL";
  local_160 = 0x10000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203860) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2CG";
  local_160 = 0x100000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203864) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2TILE";
  local_160 = 0x10000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203868) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2BACK";
  local_160 = 0x10000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x20386c) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2XLAT";
  local_160 = 0x10000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203870) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2LUMA";
  local_160 = 0x20000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203874) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2BUFRAM";
  local_160 = 0x80000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  uVar9 = FUN_004fc340();
  *(undefined4 *)(param_1 + 0x203878) = uVar9;
  uVar9 = FUN_004fc340();
  *(undefined4 *)(param_1 + 0x20387c) = uVar9;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203880) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2COPRORAM";
  local_160 = 0x20000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203884) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2TEX0";
  local_160 = 0x100000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x203888) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2TEX1";
  local_160 = 0x100000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x20388c) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2ZSHARED";
  local_160 = 0x10000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  local_164 = FUN_004fc340();
  iVar11 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x20389c) = local_164;
  iVar12 = *(int *)(iVar11 + 0x10c);
  local_16c = 0;
  local_168 = "M2SCSP";
  local_160 = 0x10000;
  iVar8 = FUN_00401050(iVar12,*(undefined4 *)(iVar12 + 4));
  if (*(int *)(iVar11 + 0x110) == 0xaaaaaa9) {
                    /* WARNING: Subroutine does not return */
    FUN_004a68db("list<T> too long");
  }
  *(int *)(iVar11 + 0x110) = *(int *)(iVar11 + 0x110) + 1;
  *(int *)(iVar12 + 4) = iVar8;
  **(int **)(iVar8 + 4) = iVar8;
  iVar11 = *(int *)(param_1 + 4);
  *(undefined4 *)(param_1 + 0x203894) = *(undefined4 *)(iVar11 + 0x20);
  DAT_005aa880 = *(undefined4 *)(iVar11 + 0x18);
  *(undefined4 *)(param_1 + 0x203898) = DAT_005aa880;
  if (*(int *)(iVar11 + 0x14) == 0) {
    *(undefined4 *)(param_1 + 0x203890) = 0;
  }
  else {
    *(undefined4 *)(param_1 + 0x203890) = *(undefined4 *)(iVar11 + 0x10);
  }
  DAT_005aa8b4 = *(undefined4 *)(param_1 + 0x203884);
  DAT_005aa764 = *(undefined4 *)(param_1 + 0x203888);
  DAT_005aa704 = *(undefined4 *)(param_1 + 0x203864);
  DAT_005aa8b8 = *(undefined4 *)(param_1 + 0x20386c);
  DAT_005aa75c = *(undefined4 *)(param_1 + 0x203860);
  DAT_005aa884 = *(undefined4 *)(iVar11 + 0x20);
  DAT_005aa6dc = *(undefined4 *)(param_1 + 0x203870);
  DAT_005aa8cc = *(undefined4 *)(param_1 + 0x20385c);
  _DAT_005aa6d8 = *(undefined4 *)(param_1 + 0x20388c);
  DAT_005aa71c = *(undefined4 *)(param_1 + 0x203868);
  DAT_005aa87c = *(undefined4 *)(param_1 + 0x203878);
  DAT_005aa754 = *(undefined4 *)(param_1 + 0x203880);
  DAT_005aa8dc = *(undefined4 *)(param_1 + 0x203890);
  DAT_005aa700 = *(undefined4 *)(param_1 + 0x203874);
  DAT_005aa8b0 = *(undefined4 *)(param_1 + 0x20387c);
  *(undefined4 *)(param_1 + 0x203894) = DAT_005aa884;
  DAT_005aa728 = *(undefined4 *)(param_1 + 0x203854);
  _DAT_005aa8c4 = *(undefined4 *)(param_1 + 0x20389c);
  FUN_004ecde0();
  iVar11 = *(int *)(param_1 + 4);
  *(undefined4 *)(param_1 + 0x202420) = 1;
  iVar12 = *(int *)(iVar11 + 0x100);
  *(int *)(param_1 + 0x3c) = iVar12;
  if (iVar12 == 2) {
    *(undefined ***)(param_1 + 0x2023c8) = &PTR_s_Model_2B_Driver_v2_0_by_ElSemi_00574c78;
  }
  else {
    if ((iVar12 == 0) || (iVar12 == 1)) {
      *(undefined ***)(param_1 + 0x2023c8) = &PTR_s_Model_2_2A_Driver_v1_0_by_ElSemi_00574b88;
      DAT_005aa8d8 = *(undefined4 *)(iVar11 + 0x50);
    }
    if (iVar12 == 3) {
      *(undefined ***)(param_1 + 0x2023c8) = &PTR_s_Model_2C_Driver_v2_0_by_ElSemi_00574c00;
    }
  }
  DAT_005aa8c0 = *(undefined4 *)(param_1 + 0x2023c8);
  DAT_005aa708 = *(undefined4 *)(iVar11 + 0x108);
  _memset(*(void **)(param_1 + 0x203850),0,0x100000);
  _memset(*(void **)(param_1 + 0x203854),0,0x80000);
  _memset(*(void **)(param_1 + 0x203858),0,0x40000);
  _memset(*(void **)(param_1 + 0x203880),0,0x20000);
  _DAT_005aa8ec = 0;
  _DAT_005aa8e8 = 0;
  _DAT_005aa8e4 = 0;
  _DAT_005aa8e0 = 0;
  DAT_005aa748 = 0xfffff;
  DAT_005aa744 = 0xfffff;
  DAT_005aa740 = 0xfffff;
  DAT_005aa73c = 0xfffff;
  DAT_005aa724 = FUN_0048d1ce(0x90);
  if (DAT_005aa724 == 0) {
    DAT_005aa724 = 0;
  }
  else {
    *(undefined4 *)(DAT_005aa724 + 0x80) = 0;
    *(undefined4 *)(DAT_005aa724 + 0x84) = 0;
    *(undefined4 *)(DAT_005aa724 + 0x88) = 0x20;
    *(undefined4 *)(DAT_005aa724 + 0x8c) = 0;
  }
  DAT_005aa738 = FUN_0048d1ce(0x90);
  if (DAT_005aa738 == 0) {
    DAT_005aa738 = 0;
  }
  else {
    *(undefined4 *)(DAT_005aa738 + 0x80) = 0;
    *(undefined4 *)(DAT_005aa738 + 0x84) = 0;
    *(undefined4 *)(DAT_005aa738 + 0x88) = 0x20;
    *(undefined4 *)(DAT_005aa738 + 0x8c) = 0;
  }
  puVar10 = (undefined4 *)(param_1 + 0x48);
  iVar11 = 0x10000;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c7d70;
    *puVar10 = io_4c7d60;
    puVar10[1] = io_4c7d50;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x100044);
  iVar11 = 0x10000;
  do {
    puVar10[-1] = 0;
    *puVar10 = io_4c7ce0;
    puVar10[1] = io_4c7cd0;
    puVar10[2] = io_4c7cc0;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  uVar9 = **(undefined4 **)(param_1 + 4);
  iVar11 = ((uint)(*(undefined4 **)(param_1 + 4))[1] >> 0x10) + 1;
  if (iVar11 != 0) {
    puVar10 = (undefined4 *)(param_1 + 0x48);
    do {
      puVar10[-2] = uVar9;
      puVar10[-1] = 0;
      *puVar10 = 0;
      puVar10[1] = 0;
      puVar10 = puVar10 + 4;
      iVar11 = iVar11 + -1;
    } while (iVar11 != 0);
  }
  if (*(int *)(param_1 + 0x3c) == 0) {
    FUN_004c0730(param_1 + 0x40,0x200000,*(undefined4 *)(param_1 + 0x203858));
    FUN_004c0730(param_1 + 0x40,0x220000,**(int **)(param_1 + 4) + 0x20000);
    *(undefined4 *)(param_1 + 0x280) = 0;
    *(code **)(param_1 + 0x284) = io_4c85b0;
    *(code **)(param_1 + 0x288) = io_4c8560;
    *(code **)(param_1 + 0x28c) = io_4c8410;
  }
  else {
    FUN_004c0730(param_1 + 0x40,0x200000,*(undefined4 *)(param_1 + 0x203858));
  }
  iVar11 = *(int *)(param_1 + 0x203850);
  puVar10 = (undefined4 *)(param_1 + 0x548);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x500000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x2023c8);
  uVar9 = *(undefined4 *)(iVar11 + 0x30);
  uVar3 = *(undefined4 *)(iVar11 + 0x2c);
  uVar4 = *(undefined4 *)(iVar11 + 0x28);
  puVar10 = (undefined4 *)(param_1 + 0x848);
  iVar11 = 8;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = uVar4;
    *puVar10 = uVar3;
    puVar10[1] = uVar9;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  iVar11 = *(int *)(param_1 + 0x2023c8);
  uVar9 = *(undefined4 *)(iVar11 + 0x18);
  uVar3 = *(undefined4 *)(iVar11 + 0x14);
  *(undefined4 *)(param_1 + 0x8c4) = *(undefined4 *)(iVar11 + 0x10);
  *(undefined4 *)(param_1 + 0x8cc) = uVar9;
  *(undefined4 *)(param_1 + 0x8c0) = 0;
  *(undefined4 *)(param_1 + 0x8c8) = uVar3;
  iVar11 = *(int *)(param_1 + 0x203874);
  puVar10 = (undefined4 *)(param_1 + 0x948);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x900000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  *(undefined4 *)(param_1 + 0x9c0) = 0;
  *(code **)(param_1 + 0x9c4) = io_4c8830;
  *(code **)(param_1 + 0x9c8) = io_4c8800;
  if (*(int *)(param_1 + 0x3c) == 0) {
    *(code **)(param_1 + 0x9cc) = io_4c7a80;
  }
  else {
    *(code **)(param_1 + 0x9cc) = io_4c8770;
  }
  *(undefined4 *)(param_1 + 0xec0) = 0;
  *(code **)(param_1 + 0xec4) = io_4c7cb0;
  *(code **)(param_1 + 0xec8) = io_4c7ca0;
  *(code **)(param_1 + 0xecc) = io_4c7c70;
  *(undefined4 *)(param_1 + 0xf40) = 0;
  *(code **)(param_1 + 0xf44) = io_4c7d40;
  *(code **)(param_1 + 0xf48) = io_4c7d30;
  *(code **)(param_1 + 0xf4c) = io_4c7db0;
  *(undefined4 *)(param_1 + 0x1cc0) = 0;
  *(code **)(param_1 + 0x1cc8) = io_4c7620;
  *(code **)(param_1 + 0x1ccc) = io_4c75f0;
  *(code **)(param_1 + 0x1cc4) = io_4c7650;
  *(undefined4 *)(param_1 + 0xa00) = 0;
  *(code **)(param_1 + 0xa04) = io_4c7650;
  *(code **)(param_1 + 0xa08) = io_4c7620;
  *(code **)(param_1 + 0xa0c) = io_4c75f0;
  if (0x1000000 < *(uint *)(*(int *)(param_1 + 4) + 0xc)) {
    FUN_004c0730(param_1 + 0x40,0x6000000,*(int *)(*(int *)(param_1 + 4) + 8) + 0x1000000);
  }
  if (*(int *)(param_1 + 0x203864) == 0) {
    *(undefined4 *)(param_1 + 0x1040) = 0;
  }
  else {
    *(int *)(param_1 + 0x1040) = *(int *)(param_1 + 0x203864) + -0x1000000;
  }
  *(undefined4 *)(param_1 + 0x1044) = 0;
  *(undefined4 *)(param_1 + 0x1048) = 0;
  *(undefined4 *)(param_1 + 0x104c) = 0;
  if (*(int *)(param_1 + 0x203864) == 0) {
    *(undefined4 *)(param_1 + 0x1050) = 0;
  }
  else {
    *(int *)(param_1 + 0x1050) = *(int *)(param_1 + 0x203864) + -0x1010000;
  }
  *(undefined4 *)(param_1 + 0x1054) = 0;
  *(undefined4 *)(param_1 + 0x1058) = 0;
  *(undefined4 *)(param_1 + 0x105c) = 0;
  if (*(int *)(param_1 + 0x203864) == 0) {
    *(undefined4 *)(param_1 + 0x1060) = 0;
  }
  else {
    *(int *)(param_1 + 0x1060) = *(int *)(param_1 + 0x203864) + -0x1020000;
  }
  *(undefined4 *)(param_1 + 0x1064) = 0;
  *(undefined4 *)(param_1 + 0x1068) = 0;
  *(undefined4 *)(param_1 + 0x106c) = 0;
  iVar11 = *(int *)(param_1 + 0x203860);
  puVar10 = (undefined4 *)(param_1 + 0x10c8);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x1080000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  if (*(int *)(param_1 + 0x203864) == 0) {
    *(undefined4 *)(param_1 + 0x1140) = 0;
  }
  else {
    *(int *)(param_1 + 0x1140) = *(int *)(param_1 + 0x203864) + -0x1100000;
  }
  *(undefined4 *)(param_1 + 0x1144) = 0;
  *(undefined4 *)(param_1 + 0x1148) = 0;
  *(undefined4 *)(param_1 + 0x114c) = 0;
  iVar11 = *(int *)(param_1 + 0x203860);
  puVar10 = (undefined4 *)(param_1 + 0x11c8);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x1180000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  if (*(int *)(param_1 + 0x20385c) == 0) {
    *(undefined4 *)(param_1 + 0x1840) = 0;
  }
  else {
    *(int *)(param_1 + 0x1840) = *(int *)(param_1 + 0x20385c) + -0x1800000;
  }
  *(undefined4 *)(param_1 + 0x1844) = 0;
  *(undefined4 *)(param_1 + 0x1848) = 0;
  *(undefined4 *)(param_1 + 0x184c) = 0;
  *(undefined4 *)(param_1 + 0x1850) = 0;
  *(code **)(param_1 + 0x1854) = io_4c7080;
  *(code **)(param_1 + 0x1858) = io_4c7070;
  *(code **)(param_1 + 0x185c) = io_4c7060;
  *(undefined4 *)(param_1 + 0x1a50) = 0;
  *(code **)(param_1 + 0x1a54) = io_4c7ba0;
  *(code **)(param_1 + 0x1a58) = io_4c7b80;
  *(code **)(param_1 + 0x1a5c) = io_4c7b70;
  *(undefined4 *)(param_1 + 0x1c40) = 0;
  *(code **)(param_1 + 0x1c44) = FUN_004cb740;
  *(code **)(param_1 + 0x1c48) = FUN_004cb720;
  *(code **)(param_1 + 0x1c4c) = FUN_004cb700;
  if (*(int *)(param_1 + 0x203868) == 0) {
    *(undefined4 *)(param_1 + 0x1d40) = 0;
  }
  else {
    *(int *)(param_1 + 0x1d40) = *(int *)(param_1 + 0x203868) + -0x1d00000;
  }
  *(undefined4 *)(param_1 + 0x1d44) = 0;
  *(undefined4 *)(param_1 + 0x1d48) = 0;
  *(undefined4 *)(param_1 + 0x1d4c) = 0;
  *(undefined4 *)(param_1 + 0x10840) = 0;
  *(code **)(param_1 + 0x10844) = io_4c7d40;
  *(code **)(param_1 + 0x10848) = io_4c6e80;
  *(code **)(param_1 + 0x1084c) = io_4c7d20;
  iVar11 = *(int *)(param_1 + 0x203884);
  puVar10 = (undefined4 *)(param_1 + 0x11048);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x11000000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203884);
  puVar10 = (undefined4 *)(param_1 + 0x11148);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x11100000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203888);
  puVar10 = (undefined4 *)(param_1 + 0x11248);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x11200000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203888);
  puVar10 = (undefined4 *)(param_1 + 0x11348);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x11300000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  if (*(int *)(param_1 + 0x203870) == 0) {
    *(undefined4 *)(param_1 + 0x11440) = 0;
  }
  else {
    *(int *)(param_1 + 0x11440) = *(int *)(param_1 + 0x203870) + -0x11400000;
  }
  *(undefined4 *)(param_1 + 0x11444) = 0;
  *(undefined4 *)(param_1 + 0x11448) = 0;
  *(undefined4 *)(param_1 + 0x1144c) = 0;
  iVar11 = *(int *)(param_1 + 0x203884);
  puVar10 = (undefined4 *)(param_1 + 0x12248);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12200000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203884);
  puVar10 = (undefined4 *)(param_1 + 0x12348);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12300000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203888);
  puVar10 = (undefined4 *)(param_1 + 0x12648);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12600000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203888);
  puVar10 = (undefined4 *)(param_1 + 0x12748);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12700000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203870);
  puVar10 = (undefined4 *)(param_1 + 0x12848);
  iVar12 = 2;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12800000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203854);
  puVar10 = (undefined4 *)(param_1 + 0x12c48);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12c00000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203854);
  puVar10 = (undefined4 *)(param_1 + 77000);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12c80000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(*(int *)(param_1 + 4) + 8);
  iVar12 = (*(uint *)(*(int *)(param_1 + 4) + 0xc) >> 0x10) + 1;
  if (iVar12 != 0) {
    puVar10 = (undefined4 *)(param_1 + 0x2048);
    do {
      if (iVar11 == 0) {
        puVar10[-2] = 0;
      }
      else {
        puVar10[-2] = iVar11 + -0x2000000;
      }
      puVar10[-1] = 0;
      *puVar10 = 0;
      puVar10[1] = 0;
      puVar10 = puVar10 + 4;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
  }
  iVar11 = *(int *)(param_1 + 0x203858);
  puVar10 = (undefined4 *)(param_1 + 0x100248);
  iVar12 = 4;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x200000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x2023c8);
  uVar9 = *(undefined4 *)(iVar11 + 0xc);
  uVar3 = *(undefined4 *)(iVar11 + 8);
  *(undefined4 *)(param_1 + 0x1008c4) = *(undefined4 *)(iVar11 + 4);
  *(undefined4 *)(param_1 + 0x1008cc) = uVar9;
  *(undefined4 *)(param_1 + 0x1008c0) = 0;
  *(undefined4 *)(param_1 + 0x1008c8) = uVar3;
  iVar11 = *(int *)(param_1 + 0x2023c8);
  uVar9 = *(undefined4 *)(iVar11 + 0x24);
  uVar3 = *(undefined4 *)(iVar11 + 0x20);
  *(undefined4 *)(param_1 + 0x100844) = *(undefined4 *)(iVar11 + 0x1c);
  *(undefined4 *)(param_1 + 0x10084c) = uVar9;
  *(undefined4 *)(param_1 + 0x100840) = 0;
  *(undefined4 *)(param_1 + 0x100848) = uVar3;
  *(undefined4 *)(param_1 + 0x100900) = 0;
  *(code **)(param_1 + 0x100904) = io_4c7d10;
  *(code **)(param_1 + 0x100908) = io_4c7d00;
  *(code **)(param_1 + 0x10090c) = io_4c7310;
  iVar11 = *(int *)(param_1 + 0x203874);
  puVar10 = (undefined4 *)(param_1 + 0x100948);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x900000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  *(undefined4 *)(param_1 + 0x1009c0) = 0;
  *(code **)(param_1 + 0x1009c4) = io_4c7a70;
  *(code **)(param_1 + 0x1009c8) = io_4c7a60;
  *(code **)(param_1 + 0x1009cc) = io_4c79c0;
  *(undefined4 *)(param_1 + 0x100ec0) = 0;
  *(code **)(param_1 + 0x100ec4) = io_4c7c60;
  *(code **)(param_1 + 0x100ec8) = io_4c7c50;
  *(code **)(param_1 + 0x100ecc) = io_4c7c10;
  *(undefined4 *)(param_1 + 0x100f40) = 0;
  *(code **)(param_1 + 0x100f44) = io_4c7d10;
  *(code **)(param_1 + 0x100f48) = io_4c7d00;
  *(code **)(param_1 + 0x100f4c) = io_4c7d80;
  iVar11 = *(int *)(param_1 + 0x203850);
  puVar10 = (undefined4 *)(param_1 + 0x100548);
  iVar12 = 0x10;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x500000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  *(undefined4 *)(param_1 + 0x100a00) = 0;
  *(code **)(param_1 + 0x100a04) = io_4cb570;
  *(code **)(param_1 + 0x100a08) = io_4cb500;
  *(code **)(param_1 + 0x100a0c) = io_4cb480;
  *(code **)(param_1 + 0x101cc4) = io_4cb570;
  *(code **)(param_1 + 0x101cc8) = io_4cb500;
  *(code **)(param_1 + 0x101ccc) = io_4cb480;
  *(undefined4 *)(param_1 + 0x101cc0) = 0;
  *(undefined4 *)(param_1 + 0x101040) = 0;
  *(code **)(param_1 + 0x101044) = io_4cc3f0;
  *(code **)(param_1 + 0x101048) = io_4cc390;
  *(code **)(param_1 + 0x10104c) = io_4cc300;
  *(code **)(param_1 + 0x101054) = io_4cc3f0;
  *(code **)(param_1 + 0x101058) = io_4cc390;
  *(code **)(param_1 + 0x10105c) = io_4cc300;
  *(undefined4 *)(param_1 + 0x101050) = 0;
  *(code **)(param_1 + 0x101064) = io_4cc3f0;
  *(code **)(param_1 + 0x101068) = io_4cc390;
  *(code **)(param_1 + 0x10106c) = io_4cc300;
  *(undefined4 *)(param_1 + 0x101060) = 0;
  puVar10 = (undefined4 *)(param_1 + 0x1010c8);
  iVar11 = 8;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c76c0;
    *puVar10 = io_4c76a0;
    puVar10[1] = io_4c7680;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  if (*(int *)(param_1 + 0x203864) == 0) {
    *(undefined4 *)(param_1 + 0x101140) = 0;
  }
  else {
    *(int *)(param_1 + 0x101140) = *(int *)(param_1 + 0x203864) + -0x1100000;
  }
  *(undefined4 *)(param_1 + 0x101144) = 0;
  *(undefined4 *)(param_1 + 0x101148) = 0;
  *(undefined4 *)(param_1 + 0x10114c) = 0;
  iVar11 = *(int *)(param_1 + 0x203860);
  puVar10 = (undefined4 *)(param_1 + 0x1011c8);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x1180000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  *(undefined4 *)(param_1 + 0x101840) = 0;
  *(code **)(param_1 + 0x101844) = io_4c7020;
  *(code **)(param_1 + 0x101848) = io_4c6fe0;
  *(code **)(param_1 + 0x10184c) = io_4c6fa0;
  *(undefined4 *)(param_1 + 0x101850) = 0;
  *(code **)(param_1 + 0x101854) = io_4c70e0;
  *(code **)(param_1 + 0x101858) = io_4c70c0;
  *(code **)(param_1 + 0x10185c) = io_4c7090;
  *(undefined4 *)(param_1 + 0x101a50) = 0;
  *(code **)(param_1 + 0x101a54) = io_4c7b20;
  *(code **)(param_1 + 0x101a58) = io_4c7b00;
  *(code **)(param_1 + 0x101a5c) = io_4c7ae0;
  *(undefined4 *)(param_1 + 0x101c40) = 0;
  *(code **)(param_1 + 0x101c44) = io_4cb5e0;
  *(code **)(param_1 + 0x101c48) = io_4c86c0;
  *(code **)(param_1 + 0x101c4c) = io_4c76e0;
  if (*(int *)(param_1 + 0x203868) == 0) {
    *(undefined4 *)(param_1 + 0x101d40) = 0;
  }
  else {
    *(int *)(param_1 + 0x101d40) = *(int *)(param_1 + 0x203868) + -0x1d00000;
  }
  *(undefined4 *)(param_1 + 0x101d44) = 0;
  *(undefined4 *)(param_1 + 0x101d48) = 0;
  *(undefined4 *)(param_1 + 0x101d4c) = 0;
  puVar10 = (undefined4 *)(param_1 + 0x110048);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c72f0;
    *puVar10 = io_4c72d0;
    puVar10[1] = io_4c72c0;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x111048);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c72a0;
    *puVar10 = io_4c7280;
    puVar10[1] = io_4c7250;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x111148);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c72a0;
    *puVar10 = io_4c7280;
    puVar10[1] = io_4c7250;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x111248);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c7230;
    *puVar10 = io_4c7210;
    puVar10[1] = io_4c71e0;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x111348);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c7230;
    *puVar10 = io_4c7210;
    puVar10[1] = io_4c71e0;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  *(undefined4 *)(param_1 + 0x111440) = 0;
  *(code **)(param_1 + 0x111444) = io_4c6f80;
  *(code **)(param_1 + 0x111448) = io_4c6f60;
  *(code **)(param_1 + 0x11144c) = io_4c6f30;
  iVar11 = *(int *)(param_1 + 0x203854);
  puVar10 = (undefined4 *)(param_1 + 0x111648);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x11600000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203854);
  puVar10 = (undefined4 *)(param_1 + 0x1116c8);
  iVar12 = 8;
  do {
    iVar8 = iVar12;
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x11680000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar8 + -1;
  } while (iVar12 != 0);
  iVar8 = iVar8 + 0xf;
  puVar10 = (undefined4 *)(param_1 + 0x112048);
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c71c0;
    *puVar10 = io_4c71a0;
    puVar10[1] = io_4c7170;
    puVar10 = puVar10 + 4;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x112148);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c71c0;
    *puVar10 = io_4c71a0;
    puVar10[1] = io_4c7170;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x112248);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c71c0;
    *puVar10 = io_4c71a0;
    puVar10[1] = io_4c7170;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x112348);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c71c0;
    *puVar10 = io_4c71a0;
    puVar10[1] = io_4c7170;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x112448);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c7150;
    *puVar10 = io_4c7130;
    puVar10[1] = io_4c7100;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x112548);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c7150;
    *puVar10 = io_4c7130;
    puVar10[1] = io_4c7100;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x112648);
  iVar11 = 0x10;
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c7150;
    *puVar10 = io_4c7130;
    puVar10[1] = io_4c7100;
    puVar10 = puVar10 + 4;
    iVar11 = iVar11 + -1;
  } while (iVar11 != 0);
  puVar10 = (undefined4 *)(param_1 + 0x112748);
  iVar11 = 0x10;
  do {
    iVar12 = iVar11;
    puVar10[-2] = 0;
    puVar10[-1] = io_4c7150;
    *puVar10 = io_4c7130;
    puVar10[1] = io_4c7100;
    puVar10 = puVar10 + 4;
    iVar11 = iVar12 + -1;
  } while (iVar11 != 0);
  iVar12 = iVar12 + 1;
  puVar10 = (undefined4 *)(param_1 + 0x112848);
  do {
    puVar10[-2] = 0;
    puVar10[-1] = io_4c8640;
    *puVar10 = io_4c8620;
    puVar10[1] = io_4c85d0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203854);
  puVar10 = (undefined4 *)(param_1 + 0x112c48);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12c00000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  iVar11 = *(int *)(param_1 + 0x203854);
  puVar10 = (undefined4 *)(param_1 + 0x112cc8);
  iVar12 = 8;
  do {
    if (iVar11 == 0) {
      puVar10[-2] = 0;
    }
    else {
      puVar10[-2] = iVar11 + -0x12c80000;
    }
    puVar10[-1] = 0;
    *puVar10 = 0;
    puVar10[1] = 0;
    puVar10 = puVar10 + 4;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  *(undefined4 *)(param_1 + 0x1ff040) = 0;
  *(code **)(param_1 + 0x1ff044) = io_4c7ce0;
  *(code **)(param_1 + 0x1ff048) = io_4c7cd0;
  *(code **)(param_1 + 0x1ff04c) = io_4c7be0;
  _DAT_005aa8c8 = *(int *)(param_1 + 0x20388c);
  DAT_005aa750 = _DAT_005aa8c8 + 0x8000;
  DAT_010ed8d0 = 0;
  DAT_010ed8d8 = 0;
  DAT_005aa720 = 0;
  DAT_005aa758 = 0;
  DAT_005aa870 = 0;
  _DAT_010ed904 = 0;
  DAT_005a96bf = 0;
  DAT_005aa760 = 0;
  DAT_005a96bd = 0;
  _memset(*(void **)(param_1 + 0x203868),0,0x10000);
  _memset(*(void **)(param_1 + 0x203874),0,0x80000);
  FUN_004c4f20(*(int *)(param_1 + 0x18) + 0x10c);
  FUN_004c4870(param_1 + 0x40,1,0x200000,0x40000);
  FUN_004c4870(param_1 + 0x40,2,FUN_00500000,0x100000);
  FUN_004c3570();
  _DAT_005aa8bc = 0;
  *(undefined4 *)(param_1 + 0x203438) = 1;
  UVar7 = GetPrivateProfileIntA("Input","HoldGears",0,".\\emulator.ini");
  DAT_010ed8fc = UVar7 != 0;
  UVar7 = GetPrivateProfileIntA("Input","UpDownGears",0,".\\emulator.ini");
  DAT_010ed8fd = UVar7 != 0;
  uVar9 = FUN_004fc340();
  *(undefined4 *)(param_1 + 0x203440) = uVar9;
  uVar9 = FUN_004fc340();
  *(undefined4 *)(param_1 + 0x20344c) = uVar9;
  uVar9 = FUN_004fc340();
  *(undefined4 *)(param_1 + 0x20343c) = uVar9;
  uVar9 = FUN_004fc340();
  *(undefined4 *)(param_1 + 0x203444) = uVar9;
  FUN_004c6b10();
  DAT_005aa8d0 = param_1 + 0x203450;
  DAT_005aa88c = param_1 + 0x203550;
  DAT_005aa888 = param_1 + 0x203750;
  DAT_005aa8d4 = param_1 + 0x203650;
  GetPrivateProfileStringA("Renderer","GammaR","1.0",local_138,0x100,".\\emulator.ini");
  local_140 = _atof(local_138);
  GetPrivateProfileStringA("Renderer","GammaG","1.0",local_138,0x100,".\\emulator.ini");
  local_178 = _atof(local_138);
  GetPrivateProfileStringA("Renderer","GammaB","1.0",local_138,0x100,".\\emulator.ini");
  local_150 = _atof(local_138);
  local_128 = 1.0 / local_140;
  local_154 = 0;
  local_17c = -0x3fc0;
  local_120 = 1.0 / local_178;
  local_150 = 1.0 / local_150;
  do {
    iVar12 = local_154;
    local_140 = (double)local_154 * _DAT_0051d120;
    fVar13 = (float10)FUN_005006a0();
    local_178._4_4_ = (undefined4)((ulonglong)local_178 >> 0x20);
    local_178._0_4_ = (uint)ROUND(fVar13 * (float10)_DAT_0051ce60);
    *(undefined1 *)(param_1 + 0x203550 + iVar12) = (undefined1)local_178;
    fVar13 = (float10)FUN_005006a0();
    local_154 = CONCAT22(local_154._2_2_,in_FPUControlWord);
    local_178._0_4_ = (uint)ROUND(fVar13 * (float10)_DAT_0051ce60);
    *(undefined1 *)(param_1 + 0x203650 + iVar12) = (undefined1)local_178;
    fVar13 = (float10)FUN_005006a0();
    local_178 = (double)CONCAT44(local_178._4_4_,(int)ROUND(fVar13 * (float10)_DAT_0051ce60));
    dVar5 = local_178;
    local_178._0_1_ = (undefined1)(int)ROUND(fVar13 * (float10)_DAT_0051ce60);
    *(undefined1 *)(param_1 + 0x203750 + iVar12) = (undefined1)local_178;
    iVar11 = local_17c / 0xbf;
    local_17c = local_17c + 0xff;
    *(byte *)(param_1 + 0x203450 + iVar12) = (iVar11 < 1) - 1U & (byte)iVar11;
    local_154 = iVar12 + 1;
    local_178 = dVar5;
  } while (local_17c < 0xbf40);
  piVar2 = (int *)(param_1 + 0x2023a0);
  DAT_005aa72c = piVar2;
  *(undefined1 *)(param_1 + 0x2023b0) = 0;
  if (*piVar2 == 0) {
    _Dst = (void *)FUN_0048d2ec(0x100000);
    local_178 = (double)CONCAT44(local_178._4_4_,_Dst);
    _memset(_Dst,0,0x100000);
    piVar1 = (int *)(DAT_010ed9fc + 4);
    iVar11 = FUN_00402b90(DAT_010ed9fc,*piVar1,&local_178);
    if (_DAT_010eda00 == 0x3ffffffe) {
                    /* WARNING: Subroutine does not return */
      FUN_004a68db("list<T> too long");
    }
    _DAT_010eda00 = _DAT_010eda00 + 1;
    *piVar1 = iVar11;
    **(int **)(iVar11 + 4) = iVar11;
    *piVar2 = (int)_Dst;
  }
  if (*(int *)(param_1 + 0x2023a4) == 0) {
    uVar9 = FUN_004fc340();
    *(undefined4 *)(param_1 + 0x2023a4) = uVar9;
  }
  if (*(int *)(param_1 + 0x2023a8) == 0) {
    *(int *)(param_1 + 0x2023a8) = *piVar2 + 0x80000;
  }
  if (*(int *)(param_1 + 0x2023ac) == 0) {
    *(int *)(param_1 + 0x2023ac) = *(int *)(param_1 + 0x2023a4) + 0x80000;
  }
  *(undefined4 *)(param_1 + 0x2023bc) = *(undefined4 *)(param_1 + 0x203864);
  *(undefined4 *)(param_1 + 0x2023c0) = *(undefined4 *)(param_1 + 0x203860);
  (**(code **)(*(int *)(param_1 + 0x2023c8) + 0x34))();
  local_178._0_5_ = (uint5)(uint)local_178;
  uStack_c = 0;
  _sprintf(acStack_118,"NVDATA\\%s.DAT",**(undefined4 **)(param_1 + 0x10));
  iVar11 = FUN_0045f6c0(acStack_118);
  uVar6 = local_178;
  local_178 = (double)CONCAT44(local_178._4_4_,iVar11);
  if (iVar11 != 0) {
    local_178._5_3_ = SUB83(uVar6,5);
    local_178._0_5_ = CONCAT14(1,iVar11);
  }
  uVar9 = *(undefined4 *)(param_1 + 0x203868);
  iVar11 = FUN_004ebe20();
  if (iVar11 != -1) {
    FUN_004ebd70(uVar9,0x10000);
  }
  iVar11 = (1 << ((byte)**(undefined4 **)(param_1 + 0x2023cc) & 0x1f)) *
           (*(undefined4 **)(param_1 + 0x2023cc))[1];
  iVar12 = FUN_004ebe20();
  if (iVar12 != -1) {
    FUN_004ebd70(param_1 + 0x202428,(int)((iVar11 >> 0x1f & 7U) + iVar11) >> 3);
  }
  FUN_0045f6d0((uint)local_178);
  local_178._0_5_ = (uint5)(uint)local_178;
  *(undefined2 *)(param_1 + 0x2f) = 0x100;
  if ((*(int *)(param_1 + 0x3c) != 0) && (*(int *)(param_1 + 0x3c) != 1)) {
    iVar11 = __strnicmp((char *)**(undefined4 **)(param_1 + 0x10),"indy",4);
    if (iVar11 != 0) {
      uVar9 = 2;
      goto LAB_004cf096;
    }
  }
  uVar9 = 1;
LAB_004cf096:
  FUN_004cf950(uVar9);
  iVar11 = *(int *)(*(int *)(param_1 + 0xc) + 0x70);
  if (iVar11 == 1) {
    *(undefined4 *)(*(int *)(param_1 + 0x10) + 0x20) = 0x294;
    *(undefined1 *)(*(int *)(param_1 + 0x18) + 0x27f) = 1;
  }
  else if (iVar11 == 2) {
    *(undefined4 *)(*(int *)(param_1 + 0x10) + 0x20) = 0x253;
    *(undefined1 *)(*(int *)(param_1 + 0x18) + 0x27f) = 1;
  }
  iVar11 = *(int *)(param_1 + 0x18);
  FUN_004e9860(&LAB_004cb400);
  iVar11 = *(int *)(iVar11 + 0x11c);
  uVar9 = FUN_004d65e0(0x14);
  uStack_148 = 4;
  local_150._0_4_ = uVar9;
  FUN_004d0250(iVar11,iVar11 + 0x48,&local_150,*(int *)(iVar11 + 8) + -0x10);
  *(int *)(iVar11 + 8) = *(int *)(iVar11 + 8) + -0x10;
  iVar11 = *(int *)(param_1 + 0x18);
  FUN_004e9860(&LAB_004cb380);
  iVar11 = *(int *)(iVar11 + 0x11c);
  local_150._0_4_ = FUN_004d65e0(0x15);
  uStack_148 = 4;
  FUN_004d0250(iVar11,iVar11 + 0x48,&local_150,*(int *)(iVar11 + 8) + -0x10);
  *(int *)(iVar11 + 8) = *(int *)(iVar11 + 8) + -0x10;
  iVar11 = *(int *)(param_1 + 0x18);
  FUN_004e9860(&LAB_004cb300);
  iVar11 = *(int *)(iVar11 + 0x11c);
  local_150._0_4_ = FUN_004d65e0(0x15);
  uStack_148 = 4;
  FUN_004d0250(iVar11,iVar11 + 0x48,&local_150,*(int *)(iVar11 + 8) + -0x10);
  *(int *)(iVar11 + 8) = *(int *)(iVar11 + 8) + -0x10;
  iVar11 = *(int *)(param_1 + 0x18);
  FUN_004e9860(&LAB_004cb280);
  iVar11 = *(int *)(iVar11 + 0x11c);
  local_150._0_4_ = FUN_004d65e0(0x16);
  uStack_148 = 4;
  FUN_004d0250(iVar11,iVar11 + 0x48,&local_150,*(int *)(iVar11 + 8) + -0x10);
  *(int *)(iVar11 + 8) = *(int *)(iVar11 + 8) + -0x10;
  iVar11 = *(int *)(param_1 + 0x18);
  FUN_004e9860(&LAB_004cb200);
  iVar11 = *(int *)(iVar11 + 0x11c);
  uVar9 = FUN_004d65e0(0x16);
  local_150 = (double)CONCAT44(local_150._4_4_,uVar9);
  uStack_148 = 4;
  FUN_004d0250(iVar11,iVar11 + 0x48,&local_150,*(int *)(iVar11 + 8) + -0x10);
  *(int *)(iVar11 + 8) = *(int *)(iVar11 + 8) + -0x10;
  ExceptionList = pvStack_14;
  return 1;
}


// ===== FUN_004cf2d0 @ 004cf2d0 (98 bytes) =====

void FUN_004cf2d0(void)

{
  undefined4 uVar1;
  int unaff_EDI;
  
  uVar1 = DAT_005a96a4;
  if (DAT_010ed8c0 == 0) {
LAB_004cf300:
    if (DAT_010f01a8 == (FILE *)0x0) goto LAB_004cf318;
  }
  else if (DAT_010f01a8 == (FILE *)0x0) {
    DAT_010f01a8 = _fopen("netdumpsend","wb");
    goto LAB_004cf300;
  }
  _fwrite((void *)(unaff_EDI + 5),0xe00,1,DAT_010f01a8);
LAB_004cf318:
  Ordinal_20(uVar1);
  return;
}


// ===== FUN_004cf340 @ 004cf340 (132 bytes) =====

void FUN_004cf340(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 local_e0b [897];
  
  if ((DAT_010ed8b8 != '\0') && (DAT_010ed8bc == 3)) {
    if (DAT_010ed8b9 != '\0') {
      puVar2 = (undefined4 *)(DAT_005aa750 + 0x2000);
      puVar3 = local_e0b;
      for (iVar1 = 0x380; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar3 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar3 = puVar3 + 1;
      }
      FUN_004cf2d0(&DAT_005aa6c8);
      if (DAT_010ed8bc != 3) {
        return;
      }
    }
    if (DAT_005a96bc != '\0') {
      WaitForSingleObject(DAT_005a96a0,2000);
    }
  }
  return;
}


// ===== FUN_004cf3d0 @ 004cf3d0 (879 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_004cf3d0(void)

{
  byte *_Str;
  undefined1 uVar1;
  undefined4 uVar2;
  int iVar3;
  FILE *pFVar4;
  undefined1 *puVar5;
  char cVar6;
  undefined2 unaff_BX;
  undefined4 unaff_EBP;
  bool bVar7;
  undefined2 uStack_232;
  undefined4 local_228;
  undefined4 local_224;
  undefined4 uStack_220;
  undefined1 auStack_21c [2];
  undefined2 uStack_21a;
  undefined4 local_20c;
  undefined4 local_208;
  char acStack_11c [284];
  
  uStack_232 = (undefined2)((uint)unaff_EBP >> 0x10);
  local_228 = 0;
  local_224 = 0;
  do {
    local_208 = DAT_005aa6c0;
    local_20c = 1;
    if (DAT_010ed8bc == 3) {
      local_228 = 2;
    }
    Ordinal_18(0x80,&local_20c,0,0,&local_228);
    iVar3 = Ordinal_151(DAT_005aa6c0,&uStack_220);
    if (iVar3 == 0) {
      return 0;
    }
    uStack_220 = 0x10;
    iVar3 = Ordinal_17(DAT_005aa6c0,DAT_005a96b8,0xe05,0,auStack_21c,&uStack_220);
    if (iVar3 != 0xe05) {
      _sprintf(acStack_11c,"Paquete Fragmentado: Esperado %d, recibido %d",0xe05,iVar3);
      MessageBoxA((HWND)0x0,acStack_11c,"Petada",0x10);
    }
    iVar3 = DAT_005a96b8;
    Ordinal_9(CONCAT22(unaff_BX,uStack_232));
    uVar2 = DAT_005a96a4;
    if (*(char *)(iVar3 + 4) == '\x02') {
      if (DAT_010ed8c0 != 0) {
        if (DAT_010f01ac != (FILE *)0x0) goto LAB_004cf71f;
        DAT_010f01ac = _fopen("netdumprecv","wb");
      }
      if (DAT_010f01ac == (FILE *)0x0) {
        return iVar3;
      }
LAB_004cf71f:
      _fwrite((void *)(iVar3 + 5),0xe00,1,DAT_010f01ac);
      return iVar3;
    }
    if ((*(char *)(iVar3 + 4) == '\0') && (DAT_010ed8bc == 1)) {
      _Str = (byte *)(iVar3 + 5);
      if (DAT_010ed8b9 == '\0') {
        *(undefined4 *)(iVar3 + 7 + (uint)*(byte *)(iVar3 + 5) * 6) = DAT_005a96ac;
        *(undefined2 *)(iVar3 + 5 + (uint)*_Str * 6) = uStack_21a;
        *_Str = *_Str + 1;
        if (DAT_010ed8c0 == 0) {
LAB_004cf5cb:
          if (DAT_010f01a8 != (FILE *)0x0) goto LAB_004cf5cf;
        }
        else {
          if (DAT_010f01a8 == (FILE *)0x0) {
            DAT_010f01a8 = _fopen("netdumpsend","wb");
            goto LAB_004cf5cb;
          }
LAB_004cf5cf:
          _fwrite(_Str,0xe00,1,DAT_010f01a8);
        }
        Ordinal_20(uVar2,iVar3,0xe05,0,&DAT_005aa6c8,0x10);
      }
      else {
        bVar7 = DAT_010ed8c0 == 0;
        *(undefined2 *)(iVar3 + 5 + (uint)*(byte *)(iVar3 + 5) * 6) = uStack_21a;
        uVar2 = DAT_005a96a4;
        _DAT_00574dd4 = (uint)*_Str;
        _DAT_00574dd0 = 1;
        *_Str = *_Str;
        pFVar4 = DAT_010f01a8;
        *(undefined1 *)(iVar3 + 4) = 1;
        *(undefined1 *)(iVar3 + 6) = 2;
        if (bVar7) {
LAB_004cf54a:
          if (pFVar4 != (FILE *)0x0) goto LAB_004cf54e;
        }
        else {
          if (pFVar4 == (FILE *)0x0) {
            pFVar4 = _fopen("netdumpsend","wb");
            DAT_010f01a8 = pFVar4;
            goto LAB_004cf54a;
          }
LAB_004cf54e:
          _fwrite(_Str,0xe00,1,pFVar4);
        }
        Ordinal_20(uVar2,iVar3,0xe05,0,&DAT_005aa6c8,0x10);
        DAT_010ed8bc = 2;
      }
    }
    puVar5 = DAT_005aa750;
    uVar2 = DAT_005a96a4;
  } while ((*(char *)(iVar3 + 4) != '\x01') || ((DAT_010ed8bc != 2 && (DAT_010ed8bc != 1))));
  if (DAT_010ed8b9 != '\0') {
    uVar1 = DAT_00574dd4;
    DAT_005aa750[2] = DAT_00574dd0;
    puVar5[3] = uVar1;
    goto LAB_004cf6af;
  }
  _DAT_00574dd0 = (uint)*(byte *)(iVar3 + 6);
  _DAT_00574dd4 = (uint)*(byte *)(iVar3 + 5);
  bVar7 = DAT_010ed8c0 == 0;
  *(byte *)(iVar3 + 6) = *(byte *)(iVar3 + 6) + 1;
  pFVar4 = DAT_010f01a8;
  *(undefined1 *)(iVar3 + 4) = 1;
  *(byte *)(iVar3 + 5) = *(byte *)(iVar3 + 5);
  if (bVar7) {
LAB_004cf66c:
    if (pFVar4 != (FILE *)0x0) goto LAB_004cf670;
  }
  else {
    if (pFVar4 == (FILE *)0x0) {
      pFVar4 = _fopen("netdumpsend","wb");
      DAT_010f01a8 = pFVar4;
      goto LAB_004cf66c;
    }
LAB_004cf670:
    _fwrite((byte *)(iVar3 + 5),0xe00,1,pFVar4);
  }
  Ordinal_20(uVar2,iVar3,0xe05,0,&DAT_005aa6c8,0x10);
  puVar5 = DAT_005aa750;
  cVar6 = DAT_00574dd4 - DAT_00574dd0;
  DAT_005aa750[3] = DAT_00574dd4;
  puVar5[2] = cVar6 + '\x02';
LAB_004cf6af:
  DAT_005aa760 = DAT_005aa760 & 0x7f;
  DAT_005aa760 = DAT_005aa760 ^ 0x80;
  *puVar5 = 1;
  DAT_010ed8bc = 3;
  return 0;
}


// ===== FUN_004cf740 @ 004cf740 (507 bytes) =====

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */

void FUN_004cf740(void)

{
  HANDLE hEvent;
  int iVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 local_1c18;
  undefined2 local_1c14;
  undefined1 local_1c12;
  undefined4 local_1c11;
  undefined2 local_1c0d;
  undefined4 local_e10;
  undefined1 local_e0c;
  undefined4 local_e0b [895];
  undefined4 uStack_c;
  
  uStack_c = 0x4cf750;
  puVar3 = DAT_005aa750;
  do {
    while( true ) {
      while( true ) {
        while ((DAT_005a96bd & 1) == 0) {
          *puVar3 = 0;
          DAT_005aa6c4 = 0;
          DAT_010ed8bc = 0;
          Sleep(0x32);
          puVar3 = DAT_005aa750;
        }
        iVar1 = FUN_004cf3d0();
        puVar3 = DAT_005aa750;
        if (DAT_010ed8bc != 0) break;
        DAT_005aa760 = DAT_005aa760 | 0x80;
        iVar1 = DAT_005aa6c4 + 1;
        DAT_005aa6c4 = iVar1;
        *DAT_005aa750 = 0;
        *(undefined2 *)(puVar3 + 2) = 0xffff;
        *(undefined2 *)(puVar3 + 0x12) = 0xe00;
        if (iVar1 == 0x3c) {
          DAT_010ed8bc = 1;
          DAT_010ed8b9 = '\0';
          if (DAT_00574ddc == 1) {
            DAT_010ed8b9 = (DAT_005aa760 & 1) != 0;
            DAT_00574dd8 = 0x1c0;
            *(undefined2 *)(puVar3 + 0x14) = 0x1c0;
          }
          else if (DAT_00574ddc == 2) {
            DAT_010ed8b9 = puVar3[1] == '\x01';
            DAT_00574dd8 = (uint)*(ushort *)(puVar3 + 0x14);
          }
          _memset(puVar3 + 0x2000,0,0xe00);
        }
      }
      if (DAT_010ed8bc == 1) break;
      if (((DAT_010ed8bc == 3) && (iVar1 != 0)) && (*(char *)(iVar1 + 4) == '\x02')) {
        local_e10 = 0x4d45324d;
        local_e0c = 2;
        puVar4 = (undefined4 *)(DAT_005aa750 + 0x2000);
        puVar5 = local_e0b;
        for (iVar2 = 0x380; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar5 = *puVar4;
          puVar4 = puVar4 + 1;
          puVar5 = puVar5 + 1;
        }
        if (DAT_010ed8b9 == '\0') {
          FUN_004cf2d0(&DAT_005aa6c8);
        }
        Ordinal_9(DAT_005aa6ca);
        hEvent = DAT_005a96a0;
        DAT_005aa760 = DAT_005aa760 ^ 0x80;
        puVar4 = (undefined4 *)(iVar1 + 5);
        puVar5 = (undefined4 *)(DAT_005aa750 + DAT_00574dd8 + 0x2000);
        for (iVar2 = 0x380; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar5 = *puVar4;
          puVar4 = puVar4 + 1;
          puVar5 = puVar5 + 1;
        }
        SetEvent(hEvent);
        puVar3 = DAT_005aa750;
      }
    }
    if (DAT_010ed8b9 != '\0') {
      _memset(&local_1c18,0,0xe05);
      local_1c18 = 0x4d45324d;
      local_1c14 = 0x100;
      local_1c12 = 0;
      local_1c11 = DAT_005a96ac;
      local_1c0d = DAT_005a96aa;
      FUN_004cf2d0(&DAT_005aa6c8);
      Sleep(100);
      puVar3 = DAT_005aa750;
    }
  } while( true );
}


// ===== FUN_004cf950 @ 004cf950 (429 bytes) =====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004cf950(void)

{
  UINT UVar1;
  undefined4 unaff_retaddr;
  undefined4 uStack_2cc;
  undefined4 uStack_2c8;
  undefined4 uStack_2c4;
  undefined4 uStack_2c0;
  undefined4 uStack_2bc;
  
  uStack_2bc = 0x4cf96f;
  DAT_005a96a0 = CreateEventA((LPSECURITY_ATTRIBUTES)0x0,0,1,(LPCSTR)0x0);
  uStack_2bc = 0x4cf98d;
  GetPrivateProfileIntA("Network","RXPort",0,".\\m2network.ini");
  DAT_005aa6ca = Ordinal_9();
  if (DAT_005aa6ca == 0) {
    DAT_010ed8b8 = 0;
    return;
  }
  DAT_010ed8b8 = 1;
  DAT_00574ddc = unaff_retaddr;
  Ordinal_115();
  uStack_2bc = 2;
  uStack_2c0 = 2;
  DAT_005a96b8 = &DAT_005a96c0;
  uStack_2c4 = 0x4cf9ed;
  DAT_005aa6c0 = Ordinal_23();
  uStack_2c4 = 0;
  uStack_2c8 = 2;
  uStack_2cc = 2;
  DAT_005a96a4 = Ordinal_23();
  _DAT_005aa6c8 = 2;
  GetPrivateProfileIntA("Network","RXPort",0x2329,".\\m2network.ini");
  DAT_005aa6ca = Ordinal_9();
  _DAT_005aa6cc = 0;
  Ordinal_2();
  Ordinal_6(DAT_005aa6c0,&DAT_005a96a8,&uStack_2cc);
  _DAT_005aa6c8 = 2;
  UVar1 = GetPrivateProfileIntA("Network","NextPort",0x2329,".\\m2network.ini");
  DAT_005aa6ca = Ordinal_9(UVar1);
  GetPrivateProfileStringA("Network","NextIP","",&stack0xfffffd2c,0xff,".\\m2network.ini");
  _DAT_005aa6cc = Ordinal_11(&stack0xfffffd2c);
  UVar1 = GetPrivateProfileIntA("Network","FrameSync",0,".\\m2network.ini");
  DAT_005a96bc = UVar1 != 0;
  CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,FUN_004cf740,(LPVOID)0x0,0,(LPDWORD)&stack0xfffffd24);
  return;
}


