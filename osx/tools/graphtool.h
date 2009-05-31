/*
 *  graphtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */
#define  LINES_IN_TEXT_WIN	11

struct m_pic_index_t {
	unsigned char i, x, y;
};

typedef unsigned short pic_num_t;
void init_graph_tool(void (*redraw_callback)(),Point* p);
GWorldPtr load_pict(int picture_to_get);
GWorldPtr load_bmp(unsigned char *data, unsigned long length);
void set_cursor(short which_curs);
void restore_cursor();
void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,Rect targ_rect,
						  char masked,short main_win);
void char_win_draw_string(WindowPtr dest_window,Rect dest_rect,const char *str,short mode,short line_height,bool main_win);
void char_port_draw_string(GrafPtr dest_window,Rect dest_rect,const char *str,short mode,short line_height,bool main_win);
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height,bool main_win);
short string_length(const char *str);
//OSStatus flip_pict(OSType domain, OSType type, short id, void *ptr, UInt32 size, bool isNative, void *refcon);
//void draw_terrain();
Rect calc_rect(short i, short j);
Rect get_custom_rect (short which_rect);
short get_custom_rect (short which_rect, Rect& store_rect);
void get_str(Str255 str,short i, short j);
GWorldPtr importPictureFileToGWorld(const FSSpec *fileSpec);
void writeGWorldToPNGFile(GWorldPtr gw, const FSSpec *fileSpec);
#ifndef _GRAPHTOOL_CPP
extern short arrow_curs[3][3];
extern short sword_curs, boot_curs, drop_curs, target_curs;
extern short talk_curs, key_curs, look_curs, current_cursor;
extern m_pic_index_t m_pic_index[200];
extern PixPatHandle bg[14];
extern short geneva_font_num,dungeon_font_num;
#endif
