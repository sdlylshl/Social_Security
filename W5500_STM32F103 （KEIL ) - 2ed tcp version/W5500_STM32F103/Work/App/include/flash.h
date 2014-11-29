#ifndef __FLASH_H
#define __FLASH_H

u8 get_ip_addr(u32* ip);
int get_id(u32* id);
u8 save_link_to_flash(u8 i, u32 in, u16 out, u8 xor);
u8 save_cell_num(u8 i, u8* src);
u8 save_text_content(u8 i, u8 len, u8* src);
u8 save_text_num(u8 i, u8* src);
u8 save_arming_to_flash(u32 arm);

int save_ip_to_flash(u8* addr, u8 sep);
int save_mac_to_flash(u8* addr);
int save_port_to_flash(u16 port);
int save_id_to_flash(u32 id);


#endif
