.section .rodata
.balign 4
.global phantom_vgp_program_begin
.type phantom_vgp_program_begin, %object
phantom_vgp_program_begin:
.incbin "assets/PhantomMask_SH.bin"
.global phantom_vgp_program_end
phantom_vgp_program_end:
.size phantom_vgp_program_begin, phantom_vgp_program_end-phantom_vgp_program_begin

.balign 4
.global phantom_mask_main_pvr_begin
.type phantom_mask_main_pvr_begin, %object
phantom_mask_main_pvr_begin:
.incbin "assets/MaskMain.pvr"
.global phantom_mask_main_pvr_end
phantom_mask_main_pvr_end:
.size phantom_mask_main_pvr_begin, phantom_mask_main_pvr_end-phantom_mask_main_pvr_begin

.balign 4
.global phantom_room_still_pvr_begin
.type phantom_room_still_pvr_begin, %object
phantom_room_still_pvr_begin:
.incbin "assets/RoomStill.pvr"
.global phantom_room_still_pvr_end
phantom_room_still_pvr_end:
.size phantom_room_still_pvr_begin, phantom_room_still_pvr_end-phantom_room_still_pvr_begin
