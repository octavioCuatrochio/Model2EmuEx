// ===== FUN_004ed5e0 @ 004ed5e0 (267 bytes) =====


void FUN_004ed5e0(undefined4 param_1,int param_2)

{
  int iVar1;
  int in_ECX;
  int unaff_EBP;
  undefined4 uVar2;
  undefined1 auStack_42 [2];
  undefined4 uStack_40;
  int *piStack_3c;
  undefined4 uStack_38;
  int *piStack_34;
  undefined1 *puStack_30;
  int *piStack_2c;
  undefined4 *puStack_28;
  
  iVar1 = param_2;
  piStack_2c = *(int **)(in_ECX + 4);
  puStack_28 = &param_2;
  puStack_30 = (undefined1 *)0x4ed601;
  (**(code **)(*piStack_2c + 0x174))();
  piStack_34 = *(int **)(in_ECX + 4);
  puStack_30 = &stack0xffffffe0;
  uStack_38 = 0x4ed614;
  (**(code **)(*piStack_34 + 0x1b0))();
  piStack_3c = *(int **)(in_ECX + 4);
  uStack_38 = 0;
  uStack_40 = 0x4ed624;
  (**(code **)(*piStack_3c + 0x170))();
  uStack_40 = 0;
  (**(code **)(**(int **)(in_ECX + 4) + 0x1ac))();
  piStack_34 = (int *)(unaff_EBP + 2);
  piStack_2c = (int *)(unaff_EBP + 1000);
  puStack_30 = (undefined1 *)(iVar1 + 2);
  puStack_28 = (undefined4 *)(iVar1 + 1000);
  (**(code **)(**(int **)(in_ECX + 4) + 0xe4))();
  (**(code **)(**(int **)(in_ECX + 0x24) + 0x38))(*(int **)(in_ECX + 0x24),0);
  uVar2 = 0;
  (**(code **)(**(int **)(in_ECX + 0x24) + 0x38))(*(int **)(in_ECX + 0x24),0);
  *(undefined1 *)(in_ECX + 0x1c) = 1;
  (**(code **)(**(int **)(in_ECX + 4) + 0x170))(*(int **)(in_ECX + 4),auStack_42);
  (**(code **)(**(int **)(in_ECX + 4) + 0x1ac))(*(int **)(in_ECX + 4),uVar2);
  return;
}


// ===== FUN_004ed6f0 @ 004ed6f0 (1395 bytes) =====


void FUN_004ed6f0(void)

{
  bool bVar1;
  int in_EAX;
  int *piVar2;
  code *pcVar3;
  int unaff_EDI;
  ushort in_FPUControlWord;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uStack_188;
  float fStack_184;
  float fStack_180;
  int *piStack_17c;
  undefined4 uStack_178;
  float fStack_174;
  float fStack_170;
  int *piStack_16c;
  undefined4 uStack_168;
  undefined4 uStack_164;
  undefined4 uStack_160;
  int *piStack_15c;
  undefined4 uStack_158;
  float fStack_154;
  float fStack_150;
  int *piStack_14c;
  undefined4 uStack_148;
  undefined4 uStack_144;
  undefined4 uStack_140;
  int *piStack_13c;
  undefined4 uStack_138;
  float fStack_134;
  float fStack_130;
  int *piStack_12c;
  undefined4 uStack_128;
  undefined4 uStack_124;
  int *piStack_120;
  undefined4 uStack_11c;
  undefined4 uStack_118;
  int *piStack_114;
  float fStack_110;
  undefined4 uStack_10c;
  int *piStack_108;
  undefined4 uStack_104;
  uint uStack_100;
  int *piStack_fc;
  undefined4 uStack_f8;
  undefined4 uStack_f4;
  int *piStack_f0;
  undefined4 uStack_ec;
  undefined4 uStack_e8;
  int *piStack_e4;
  undefined4 uStack_e0;
  undefined4 uStack_dc;
  int *piStack_d8;
  undefined4 *puStack_d4;
  int *piStack_d0;
  undefined4 uStack_cc;
  int *piStack_c8;
  uint uStack_c4;
  undefined8 uStack_c0;
  char cStack_ac;
  byte bStack_40;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  
  uStack_c0 = CONCAT44(&local_20,*(int **)(in_EAX + 4));
  uStack_c4 = 0x4ed710;
  (**(code **)(**(int **)(in_EAX + 4) + 0x1b0))();
  piStack_c8 = *(int **)(in_EAX + 4);
  uStack_c4 = 0;
  uStack_cc = 0x4ed720;
  (**(code **)(*piStack_c8 + 0x1ac))();
  piStack_d0 = *(int **)(in_EAX + 4);
  uStack_cc = 0;
  puStack_d4 = (undefined4 *)0x4ed730;
  (**(code **)(*piStack_d0 + 0x170))();
  uStack_30 = 0;
  uStack_2c = 0;
  bVar1 = false;
  uStack_c0._0_4_ = (undefined4)(longlong)ROUND(*(float *)(in_EAX + 100));
  uStack_28 = (undefined4)uStack_c0;
  piStack_c8 = (int *)CONCAT22(piStack_c8._2_2_,in_FPUControlWord);
  uStack_c4 = in_FPUControlWord | 0xc00;
  piStack_d8 = *(int **)(in_EAX + 4);
  uStack_c0 = (longlong)ROUND(*(float *)(in_EAX + 0x60));
  uStack_24 = (undefined4)uStack_c0;
  puStack_d4 = &uStack_30;
  local_20 = 0;
  uStack_1c = 0x3f800000;
  uStack_dc = 0x4ed7c8;
  (**(code **)(*piStack_d8 + 0xbc))();
  piStack_e4 = *(int **)(in_EAX + 4);
  uStack_dc = 0;
  uStack_e0 = 7;
  uStack_e8 = 0x4ed7da;
  (**(code **)(*piStack_e4 + 0xe4))();
  piStack_f0 = *(int **)(in_EAX + 4);
  uStack_e8 = 0;
  uStack_ec = 0xe;
  uStack_f4 = 0x4ed7ec;
  (**(code **)(*piStack_f0 + 0xe4))();
  piStack_fc = *(int **)(in_EAX + 4);
  uStack_f4 = 1;
  uStack_f8 = 0xf;
  uStack_100 = 0x4ed7fe;
  (**(code **)(*piStack_fc + 0xe4))();
  uStack_100 = (uint)bStack_40;
  piStack_108 = *(int **)(in_EAX + 4);
  uStack_104 = 0x18;
  uStack_10c = 0x4ed817;
  (**(code **)(*piStack_108 + 0xe4))();
  piStack_114 = *(int **)(in_EAX + 4);
  uStack_10c = 4;
  fStack_110 = 3.50325e-44;
  uStack_118 = 0x4ed829;
  (**(code **)(*piStack_114 + 0xe4))();
  piStack_120 = *(int **)(in_EAX + 4);
  uStack_118 = 0;
  uStack_11c = 0x89;
  uStack_124 = 0x4ed83e;
  (**(code **)(*piStack_120 + 0xe4))();
  uStack_124 = *(undefined4 *)(unaff_EDI + 0x20);
  piStack_12c = *(int **)(in_EAX + 4);
  uStack_128 = 0;
  fStack_130 = 7.24077e-39;
  (**(code **)(*piStack_12c + 0x104))();
  piStack_13c = *(int **)(in_EAX + 4);
  fStack_130 = 2.8026e-45;
  fStack_134 = 2.8026e-45;
  uStack_138 = 0;
  uStack_140 = 0x4ed866;
  (**(code **)(*piStack_13c + 0x10c))();
  piStack_14c = *(int **)(in_EAX + 4);
  uStack_140 = 2;
  uStack_144 = 1;
  uStack_148 = 0;
  fStack_150 = 7.240826e-39;
  (**(code **)(*piStack_14c + 0x10c))();
  piStack_15c = *(int **)(in_EAX + 4);
  fStack_150 = 2.8026e-45;
  fStack_154 = 7.00649e-45;
  uStack_158 = 0;
  uStack_160 = 0x4ed88e;
  (**(code **)(*piStack_15c + 0x10c))();
  piStack_16c = *(int **)(in_EAX + 4);
  uStack_160 = 2;
  uStack_164 = 4;
  uStack_168 = 0;
  fStack_170 = 7.240882e-39;
  (**(code **)(*piStack_16c + 0x10c))();
  piStack_17c = *(int **)(in_EAX + 4);
  if (cStack_ac == '\0') {
    fStack_170 = 1.4013e-45;
    fStack_174 = 7.00649e-45;
    uStack_178 = 0;
    fStack_180 = 7.241319e-39;
    (**(code **)(*piStack_17c + 0x114))();
    piVar2 = *(int **)(in_EAX + 4);
    pcVar3 = *(code **)(*piVar2 + 0x114);
    uVar6 = 1;
    uVar5 = 6;
    uVar4 = 0;
  }
  else {
    fStack_170 = 2.8026e-45;
    fStack_174 = 7.00649e-45;
    uStack_178 = 0;
    fStack_180 = 7.240928e-39;
    (**(code **)(*piStack_17c + 0x114))();
    fStack_180 = 2.8026e-45;
    fStack_184 = 8.40779e-45;
    uStack_188 = 0;
    (**(code **)(**(int **)(in_EAX + 4) + 0x114))(*(int **)(in_EAX + 4));
    if (*(char *)(in_EAX + 0x6c) == '\0') goto LAB_004ed9ee;
    bVar1 = true;
    (**(code **)(**(int **)(in_EAX + 4) + 0x114))(*(int **)(in_EAX + 4),0,5,1);
    (**(code **)(**(int **)(in_EAX + 4) + 0x114))(*(int **)(in_EAX + 4),0,6,1);
    (**(code **)(**(int **)(in_EAX + 4) + 0x104))
              (*(int **)(in_EAX + 4),1,*(undefined4 *)(unaff_EDI + 0x20));
    (**(code **)(**(int **)(in_EAX + 4) + 0x114))(*(int **)(in_EAX + 4),1,5,2);
    (**(code **)(**(int **)(in_EAX + 4) + 0x114))(*(int **)(in_EAX + 4),1,6,2);
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),0,5,2);
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),0,4,2);
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),1,2,2);
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),1,1,2);
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),1,0xb,0);
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),1,5,1);
    piVar2 = *(int **)(in_EAX + 4);
    pcVar3 = *(code **)(*piVar2 + 0x10c);
    uVar6 = 2;
    uVar5 = 4;
    uVar4 = 1;
  }
  (*pcVar3)(piVar2,uVar4,uVar5,uVar6);
LAB_004ed9ee:
  if ((char)piStack_c8 == '\0') {
    FUN_004ecfe0(&fStack_180,&piStack_f0,&fStack_184,(float)*(int *)(unaff_EDI + 0x14),
                 (float)*(int *)(unaff_EDI + 0x10));
    piStack_fc = *(int **)(unaff_EDI + 8);
    uStack_158 = *(undefined4 *)(unaff_EDI + 0xc);
    fStack_154 = fStack_180 - 0.5;
    fStack_110 = (float)piStack_f0 - 0.5;
    piStack_114 = (int *)((float)piStack_17c + 0.5);
    uStack_11c = piStack_fc;
    uStack_118 = uStack_158;
    fStack_130 = fStack_184;
  }
  else {
    uStack_158 = *(undefined4 *)(unaff_EDI + 0xc);
    fStack_154 = -0.5;
    piStack_114 = (int *)(*(float *)(in_EAX + 100) - 0.5);
    uStack_118 = *(undefined4 *)(unaff_EDI + 0xc);
    fStack_110 = -0.5;
    piStack_fc = *(int **)(unaff_EDI + 8);
    uStack_11c = *(undefined4 *)(unaff_EDI + 8);
    fStack_130 = *(float *)(in_EAX + 0x60);
  }
  fStack_130 = fStack_130 - 0.5;
  piStack_108 = (int *)0x3f800000;
  uStack_10c = 0;
  uStack_128 = 0x3f800000;
  piStack_12c = (int *)0x0;
  uStack_138 = 0;
  piStack_13c = (int *)0x0;
  uStack_148 = 0x3f800000;
  piStack_14c = (int *)0x0;
  piStack_15c = (int *)0x0;
  uStack_160 = 0xffffffff;
  uStack_164 = 0xffffffff;
  uStack_168 = 0x3f800000;
  piStack_16c = (int *)0x0;
  uStack_144 = 0xffffffff;
  uStack_140 = 0xffffffff;
  uStack_104 = 0xffffffff;
  uStack_f8 = 0;
  uStack_100 = 0xffffffff;
  uStack_124 = 0xffffffff;
  piStack_120 = (int *)0xffffffff;
  fStack_174 = fStack_154;
  fStack_170 = fStack_130;
  fStack_150 = fStack_110;
  fStack_134 = (float)piStack_114;
  (**(code **)(**(int **)(in_EAX + 4) + 0xe4))(*(int **)(in_EAX + 4),8,3);
  (**(code **)(**(int **)(in_EAX + 4) + 0x164))
            (*(int **)(in_EAX + 4),*(undefined4 *)(in_EAX + 0x48));
  (**(code **)(**(int **)(in_EAX + 4) + 0x14c))(*(int **)(in_EAX + 4),5,2,&uStack_188,0x20);
  if (bVar1) {
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),1,0xb,1);
    (**(code **)(**(int **)(in_EAX + 4) + 0x10c))(*(int **)(in_EAX + 4),1,2,1);
    (**(code **)(**(int **)(in_EAX + 4) + 0x104))(*(int **)(in_EAX + 4),1,0);
  }
  (**(code **)(**(int **)(in_EAX + 4) + 0x104))(*(int **)(in_EAX + 4),0,0);
  (**(code **)(**(int **)(in_EAX + 4) + 0x1ac))(*(int **)(in_EAX + 4),uStack_128);
  return;
}


// ===== FUN_004edc70 @ 004edc70 (31 bytes) =====


void FUN_004edc70(void)

{
  int in_EAX;
  
  (**(code **)(**(int **)(in_EAX + 4) + 0xac))(*(int **)(in_EAX + 4),0,0,1,0,0,0);
  return;
}


// ===== FUN_004edc90 @ 004edc90 (376 bytes) =====


void FUN_004edc90(void)

{
  int *piVar1;
  int unaff_ESI;
  undefined1 local_10 [4];
  int local_c;
  uint local_8;
  uint uStack_4;
  
  if ((*(int *)(*(int *)(unaff_ESI + 0x5c) + 0x24c) != 0) && (*(char *)(unaff_ESI + 0x2c) != '\0'))
  {
    local_8 = (uint)ROUND(*(float *)(unaff_ESI + 100));
    (**(code **)(**(int **)(*(int *)(*(int *)(unaff_ESI + 0x5c) + 0xf4) + 8) + 0x10))
              (0,&local_8,&local_c,local_10,local_8 & 0xffff,
               (int)ROUND(*(float *)(unaff_ESI + 0x60)) & 0xffff);
    FUN_004ed190(local_8 - 0x10,local_c + -0x10);
    piVar1 = *(int **)(*(int *)(*(int *)(unaff_ESI + 0x5c) + 0xf4) + 8);
    if (1 < *(byte *)((int)piVar1 + 0x569)) {
      uStack_4 = (uint)ROUND(*(float *)(unaff_ESI + 100));
      (**(code **)(*piVar1 + 0x10))
                (1,&local_8,&local_c,local_10,uStack_4 & 0xffff,
                 (int)ROUND(*(float *)(unaff_ESI + 0x60)) & 0xffff);
      FUN_004ed190(local_8 - 0x10,local_c + -0x10);
    }
  }
  (**(code **)(**(int **)(unaff_ESI + 4) + 0xa8))(*(int **)(unaff_ESI + 4));
  (**(code **)(**(int **)(unaff_ESI + 4) + 0x44))(*(int **)(unaff_ESI + 4),0,0,0,0);
  (**(code **)(**(int **)(unaff_ESI + 4) + 0xa4))(*(int **)(unaff_ESI + 4));
  *(undefined1 *)(unaff_ESI + 0x1c) = 0;
  return;
}


// ===== FUN_004ede10 @ 004ede10 (946 bytes) =====


void FUN_004ede10(void)

{
  int *piVar1;
  int in_EAX;
  int unaff_EDI;
  undefined4 uStack_138;
  float fStack_134;
  float fStack_130;
  int *piStack_12c;
  undefined4 uStack_128;
  float fStack_124;
  float fStack_120;
  int *piStack_11c;
  undefined4 uStack_118;
  undefined4 uStack_114;
  undefined4 uStack_110;
  int *piStack_10c;
  undefined4 uStack_108;
  float fStack_104;
  float fStack_100;
  int *piStack_fc;
  undefined4 uStack_f8;
  undefined4 uStack_f4;
  int *piStack_f0;
  undefined4 uStack_ec;
  undefined4 uStack_e8;
  int *piStack_e4;
  float fStack_e0;
  undefined4 uStack_dc;
  int *piStack_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  int *piStack_cc;
  int *piStack_c8;
  float fStack_c4;
  int *piStack_c0;
  undefined4 *puStack_bc;
  undefined8 local_a8;
  char cStack_60;
  undefined1 local_1c [4];
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  if (*(char *)(in_EAX + 0x3c) == '\0') {
    piStack_c0 = *(int **)(in_EAX + 4);
    puStack_bc = (undefined4 *)local_1c;
    fStack_c4 = 7.242894e-39;
    (**(code **)(*piStack_c0 + 0x1b0))();
    piStack_c8 = *(int **)(in_EAX + 4);
    fStack_c4 = 0.0;
    piStack_cc = (int *)0x4ede4e;
    (**(code **)(*piStack_c8 + 0x1ac))();
  }
  local_18 = 0;
  local_14 = 0;
  local_a8._0_4_ = (undefined4)(longlong)ROUND(*(float *)(in_EAX + 100));
  local_10 = (undefined4)local_a8;
  piStack_c0 = *(int **)(in_EAX + 4);
  local_a8 = (ulonglong)ROUND(*(float *)(in_EAX + 0x60));
  uStack_c = (undefined4)local_a8;
  puStack_bc = &local_18;
  uStack_8 = 0;
  uStack_4 = 0x3f800000;
  fStack_c4 = 7.243127e-39;
  (**(code **)(*piStack_c0 + 0xbc))();
  piStack_cc = *(int **)(in_EAX + 4);
  fStack_c4 = 0.0;
  piStack_c8 = (int *)0x7;
  uStack_d0 = 0x4edef6;
  (**(code **)(*piStack_cc + 0xe4))();
  piStack_d8 = *(int **)(in_EAX + 4);
  uStack_d0 = 0;
  uStack_d4 = 0xe;
  uStack_dc = 0x4edf08;
  (**(code **)(*piStack_d8 + 0xe4))();
  piStack_e4 = *(int **)(in_EAX + 4);
  uStack_dc = 0;
  fStack_e0 = 2.10195e-44;
  uStack_e8 = 0x4edf1a;
  (**(code **)(*piStack_e4 + 0xe4))();
  piStack_f0 = *(int **)(in_EAX + 4);
  uStack_e8 = 0;
  uStack_ec = 0x89;
  uStack_f4 = 0x4edf2f;
  (**(code **)(*piStack_f0 + 0xe4))();
  uStack_f4 = *(undefined4 *)(unaff_EDI + 0x20);
  piStack_fc = *(int **)(in_EAX + 4);
  uStack_f8 = 0;
  fStack_100 = 7.24326e-39;
  (**(code **)(*piStack_fc + 0x104))();
  piStack_10c = *(int **)(in_EAX + 4);
  fStack_100 = 2.8026e-45;
  fStack_104 = 2.8026e-45;
  uStack_108 = 0;
  uStack_110 = 0x4edf57;
  (**(code **)(*piStack_10c + 0x10c))();
  piStack_11c = *(int **)(in_EAX + 4);
  uStack_110 = 2;
  uStack_114 = 1;
  uStack_118 = 0;
  fStack_120 = 7.243316e-39;
  (**(code **)(*piStack_11c + 0x10c))();
  piStack_12c = *(int **)(in_EAX + 4);
  if (cStack_60 == '\0') {
    fStack_120 = 1.4013e-45;
    fStack_124 = 7.00649e-45;
    uStack_128 = 0;
    fStack_130 = 7.243376e-39;
    (**(code **)(*piStack_12c + 0x114))();
    fStack_130 = 1.4013e-45;
  }
  else {
    fStack_120 = 2.8026e-45;
    fStack_124 = 7.00649e-45;
    uStack_128 = 0;
    fStack_130 = 7.243358e-39;
    (**(code **)(*piStack_12c + 0x114))();
    fStack_130 = 2.8026e-45;
  }
  fStack_134 = 8.40779e-45;
  uStack_138 = 0;
  (**(code **)(**(int **)(in_EAX + 4) + 0x114))(*(int **)(in_EAX + 4));
  if ((char)uStack_138 == '\0') {
    FUN_004ecfe0(&fStack_130,(int)&local_a8 + 4,&fStack_134,(float)*(int *)(unaff_EDI + 0x14),
                 (float)*(int *)(unaff_EDI + 0x10));
    fStack_104 = fStack_130 - 0.5;
    uStack_108 = *(undefined4 *)(unaff_EDI + 0xc);
    piStack_c0 = (int *)(local_a8._4_4_ - 0.5);
    piStack_cc = *(int **)(unaff_EDI + 8);
    piStack_c8 = *(int **)(unaff_EDI + 0xc);
    fStack_e0 = fStack_134;
    piVar1 = piStack_12c;
  }
  else {
    uStack_108 = *(undefined4 *)(unaff_EDI + 0xc);
    fStack_104 = -0.5;
    piStack_cc = *(int **)(unaff_EDI + 8);
    piStack_c8 = *(int **)(unaff_EDI + 0xc);
    piStack_c0 = (int *)0xbf000000;
    fStack_e0 = *(float *)(in_EAX + 0x60);
    piVar1 = *(int **)(in_EAX + 100);
  }
  fStack_c4 = (float)piVar1 - 0.5;
  fStack_e0 = fStack_e0 - 0.5;
  piStack_d8 = (int *)0x3f800000;
  uStack_dc = 0;
  uStack_e8 = 0;
  uStack_ec = 0;
  uStack_f8 = 0x3f800000;
  piStack_fc = (int *)0x0;
  piStack_10c = (int *)0x0;
  uStack_110 = 0xffffffff;
  uStack_114 = 0xffffffff;
  uStack_118 = 0x3f800000;
  piStack_11c = (int *)0x0;
  puStack_bc = (undefined4 *)0x0;
  uStack_f4 = 0xffffffff;
  piStack_f0 = (int *)0xffffffff;
  uStack_d4 = 0xffffffff;
  uStack_d0 = 0xffffffff;
  local_a8 = local_a8 & 0xffffffff00000000;
  fStack_124 = fStack_104;
  fStack_120 = fStack_e0;
  fStack_100 = (float)piStack_c0;
  piStack_e4 = (int *)fStack_c4;
  (**(code **)(**(int **)(in_EAX + 4) + 0xe4))(*(int **)(in_EAX + 4),8,3);
  (**(code **)(**(int **)(in_EAX + 4) + 0x164))
            (*(int **)(in_EAX + 4),*(undefined4 *)(in_EAX + 0x48));
  (**(code **)(**(int **)(in_EAX + 4) + 0x14c))(*(int **)(in_EAX + 4),5,2,&uStack_138,0x20);
  if (*(char *)(in_EAX + 0x3c) == '\0') {
    (**(code **)(**(int **)(in_EAX + 4) + 0x1ac))(*(int **)(in_EAX + 4),piStack_c8);
  }
  return;
}


// ===== FUN_004ee1d0 @ 004ee1d0 (201 bytes) =====


int FUN_004ee1d0(int param_1)

{
  int iVar1;
  undefined4 in_EDX;
  int iStack_60;
  int *piStack_5c;
  int local_48;
  int local_44;
  
  piStack_5c = &local_48;
  iStack_60 = 0;
  local_48 = 0;
  iStack_60 = D3DXCreateTextureFromFileExA(*(undefined4 *)(param_1 + 4));
  if (iStack_60 != 0) {
    return 0;
  }
  piStack_5c = (int *)iStack_60;
  local_48 = iStack_60;
  local_44 = iStack_60;
  (**(code **)(iRam00000001 + 0x44))(1,0,&iStack_60);
  iVar1 = FUN_0048d1ce(0x2c);
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 8) = 0x3f800000;
    *(undefined4 *)(iVar1 + 0xc) = 0x3f800000;
    *(undefined4 *)(iVar1 + 0x20) = in_EDX;
    *(undefined4 *)(iVar1 + 0x10) = 0xffffffff;
    *(undefined4 *)(iVar1 + 0x14) = 0xffffffff;
    *(undefined4 *)(iVar1 + 0x18) = 0;
    *(undefined4 *)(iVar1 + 0x1c) = 0;
    return iVar1;
  }
  return 0;
}


