void init_current_terrain();
void init_screen_locs();
bool handle_action(Point the_point,EventRecord event);
void flash_rect(Rect to_flash);
void swap_terrain();
void set_new_terrain(unsigned char selected_terrain);
void handle_keystroke(char chr,char chr2,EventRecord event);
void get_wandering_monst();
void get_town_info();
void edit_monst(short mode,short x,short y);
void get_sign_resource();
void set_info_strings();
cTown::cItem edit_item(cTown::cItem item);
void modify_lists();
void set_up_lights();
bool is_wall(short i,short j);
bool is_correctable_wall(short i,short j);
bool is_mountain(short i,short j);
bool is_hill(short i,short j);
bool is_hill_or_mountain(short i,short j);
bool is_water(short i,short j);
bool is_correctable_water(short i,short j);
void shy_change_circle_terrain(location center,short radius,unsigned char terrain_type,short probability);
void change_circle_terrain(location center,short radius,unsigned char terrain_type,short probability);
void change_rect_terrain(rectangle r,unsigned char terrain_type,short probability,bool hollow);
void swap_val(unsigned char *val,short a,short b);
void change_val_4 (unsigned char *val,short a,short b,short c,short d);
void change_val (unsigned char *val,short a,short b);
void frill_up_terrain();
void unfrill_terrain();
void set_terrain(location l,unsigned short terrain_type);
bool fix_rubble(location l);
bool fix_cave(location l);
bool fix_mountain(location l);
bool fix_hill(location l);
bool fix_water(location l);
bool out_fix_rubble(location l);
bool out_fix_cave(location l);
bool out_fix_mountain(location l);
bool out_fix_hill(location l);
bool out_fix_water(location l);
void adjust_space(location l);
bool is_lava(short x,short y);
unsigned char coord_to_ter(short x,short y);
short get_obscurity(short x,short y);
short can_see(location p1,location p2,short mode);
bool place_item(location spot_hit,short which_item,short property,short always,short odds); 
void place_items_in_town();
void set_up_start_screen();
void set_up_main_screen();
void start_town_edit();
void start_out_edit();
void shut_down_menus(short mode);
void start_terrain_editing();
void start_monster_editing(short just_redo_text);
void start_item_editing(short just_redo_text);
void start_special_item_editing();
void start_string_editing(short mode,short just_redo_text);
void start_special_editing(short mode,short just_redo_text);
void town_entry(location spot_hit);
void start_dialogue_editing(short restoring);
bool is_erasable_water(short i,short j);

bool monst_on_space(location loc,short m_num);
void place_edit_special(location loc);
void set_special(location spot_hit);
bool save_check(short which_dlog);
void update_item_menu();

unsigned short get_ground_from_ter(unsigned short ter);
bool terrain_matches(unsigned char x, unsigned char y, unsigned short ter);
