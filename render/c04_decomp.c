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


