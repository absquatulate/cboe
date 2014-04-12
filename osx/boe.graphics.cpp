
#include <cstring>
#include <cstdio>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.graphutil.h"
#include "boe.monster.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.text.h"
#include "graphtool.h"
#include "soundtool.h"
#include "mathutil.h"

#include "boe.party.h"
#include "boe.town.h"
#include "boe.items.h"
#include "boe.dlgutil.h"
#include "boe.infodlg.h"

#include "scrollbar.h"

#include "restypes.hpp"
#include "boe.menus.h"

extern sf::RenderWindow mainPtr;
extern RECT	windRect;
extern short stat_window,give_delays;
extern eGameMode overall_mode;
extern short current_spell_range,town_type;
extern bool in_startup_mode,anim_onscreen,play_sounds,frills_on,startup_loaded,party_in_memory;
extern bool flushingInput;
extern short anim_step;
extern ter_num_t combat_terrain[64][64];
extern effect_pat_type current_pat;
extern bool web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern location ul;
extern location pc_pos[6],pc_dir[6],center;
extern short which_combat_type,current_pc;
extern bool monsters_going,boom_anim_active,skip_boom_delay;
extern sf::Image spell_pict;
extern short current_ground;
extern short num_targets_left;
extern location spell_targets[8];
extern short display_mode;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern sf::Texture bg_gworld;
extern RECT sbar_rect,item_sbar_rect,shop_sbar_rect,startup_top;
extern RECT talk_area_rect, word_place_rect;
extern RECT map_pat[];
extern location store_anim_ul;
extern long register_flag;
extern long ed_flag,ed_key;
extern bool fast_bang;
extern RECT	bg[];
extern cScenario scenario;
extern cUniverse univ;
extern cCustomGraphics spec_scen_g;
extern sf::RenderWindow mini_map;
bool map_visible = false;
extern bool show_startup_splash;

//***********************
RECT		menuBarRect;
short		menuBarHeight;
Region originalGrayRgn, newGrayRgn, underBarRgn;

short terrain_there[9][9]; // this is an optimization variabel. Keeps track of what terrain
	// is in the terrain spot, so things don't get redrawn. 
	// 0 - 299 just terrain graphic in place
	// 300 - blackness
	// -1 - nothign worth saving

long anim_ticks = 0;

// 0 - terrain   1 - buttons   2 - pc stats 
// 3 - item stats   4 - text bar   5 - text area (not right)
// TODO: The duplication of RECT here shouldn't be necessary...
RECT win_from_rects[6] = {RECT{0,0,351,279},RECT{0,0,37,258},RECT{0,0,115,288},RECT{0,0,143,288},RECT{0,0,21,279},RECT{0,0,0,288}};
RECT win_to_rects[6] = {RECT{5,5,356,284},RECT{383,5,420,263},RECT{0,0,116,271},RECT{0,0,144,271},RECT{358,5,379,284},RECT{0,0,138,256}};

// 0 - title  1 - button  2 - credits  3 - base button
RECT startup_from[4] = {RECT{0,0,274,602},RECT{274,0,322,301},RECT{0,301,67,579},RECT{274,301,314,341}}; ////
extern RECT startup_button[6];
	
//	RECT trim_rects[8] = {{0,0,5,28},{31,0,36,28},{0,0,36,5},{0,24,36,28},
//						{0,0,5,5},{0,24,5,28},{31,24,36,28},{31,0,36,5}};

RECT	top_left_rec = {0,0,36,28};
short which_graphic_index[6] = {50,50,50,50,50,50};

char combat_graphics[5] = {28,29,36,79,2};
short debug_nums[6] = {0,0,0,0,0,0};					     
short remember_tiny_text = 300; // Remembers what's in the tiny text-bar, to prevent redrawing.
	// 50 indicates area name, other number indicates which-rect.
	// Add 200 if last mess. was in town

char light_area[13][13];
char unexplored_area[13][13];

// Declare the graphics
sf::Texture status_gworld;
sf::Texture invenbtn_gworld;
sf::Texture vehicle_gworld;
sf::RenderTexture pc_stats_gworld;
sf::RenderTexture item_stats_gworld;
sf::RenderTexture text_area_gworld;
sf::RenderTexture terrain_screen_gworld;
sf::RenderTexture text_bar_gworld;
sf::Texture orig_text_bar_gworld;
sf::Texture buttons_gworld;
sf::Texture items_gworld;
sf::Texture tiny_obj_gworld;
sf::Texture fields_gworld;
sf::Texture boom_gworld;
sf::Texture roads_gworld;
sf::RenderTexture map_gworld;
sf::Texture small_ter_gworld;
sf::Texture missiles_gworld;
sf::Texture dlogpics_gworld;
sf::Texture anim_gworld;
sf::Texture talkfaces_gworld;
sf::Texture pc_gworld;
sf::Texture monst_gworld[NUM_MONST_SHEETS];
sf::Texture terrain_gworld[NUM_TER_SHEETS];

// Startup graphics, will die when play starts
// TODO: The way this is done now, they won't; fix this
sf::Texture startup_gworld;
sf::Texture startup_button_orig;
sf::RenderTexture startup_button_g;
sf::Texture anim_mess;

bool has_run_anim = false,currently_loading_graphics = false;
//short anim_step = 0;
//short overall_anim_step = 0;

RECT main_win_rect = {0,0,410,250};
RECT main_win2_source_rect = {0,0,410,265};
RECT main_win2_rect = {0,250,410,515};

RECT tiny_world_1_source_rect = {0,0,190,145},
	tiny_world_1_rect = {195,242,385,475};

RECT share_mess_source_rect = {0,0,59,120},
	share_mess_rect = {120,384,179,504};
//RECT start_buttons_source_rect = {0,0,180,180},
//	start_buttons_rect = {224,30,405,210};
RECT start_buttons_source_rect = {0,0,186,190},
	start_buttons_rect = {214,30,400,220};

// Array to store which spots have been seen. Time-saver for drawing fields
char spot_seen[9][9];

char anim_str[60];
location anim_str_loc;

extern RECT bw_pats[6];

extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
bool supressing_some_spaces = false;
location ok_space[4] = {loc(),loc(),loc(),loc()};
sf::Image hold_pict;

void adjust_window_mode()
{
	RECT r;
	sf::ContextSettings winSettings;
	winSettings.stencilBits = 1;
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	// TODO: Make display_mode an enum
	if (display_mode == 5) {
		ul.x = 14; ul.y = 2;
		mainPtr.create(sf::VideoMode(605,430,32), "Blades of Exile", sf::Style::Titlebar | sf::Style::Close, winSettings);
		mainPtr.setPosition({(desktop.width - 605) / 2, (desktop.height - 430) / 2});
		r = RECT(mainPtr);
	}
	else {
		mainPtr.create(desktop, "Blades of Exile", sf::Style::None, winSettings);
		mainPtr.setPosition({0,0});
			switch (display_mode) {
				case 0: ul.x = (windRect.right - 560) / 2; ul.y = (windRect.bottom - 422) / 2 + 14; break;
				case 1:	ul.x = 10; ul.y = 28; break;
				case 2: ul.x = windRect.right - 570 - 6; ul.y = 28;	break; // was 560. not 570
				case 3: ul.x = 10; ul.y = windRect.bottom - 422	- 6; break;
				case 4: ul.x = windRect.right - 570 - 6; ul.y = windRect.bottom - 422	- 6; break;
				}

			}
	if (overall_mode != MODE_STARTUP) { // TODO: This is odd - fix it
		if (in_startup_mode == true)
			draw_startup(0);
		if (in_startup_mode == false)
			//refresh_screen(0); 
			redraw_screen();
		}
	if (text_sbar != NULL) {
		text_sbar->relocate({ul.x + 546,ul.y + 283});
		item_sbar->relocate({ul.x + 546,ul.y + 146});
		shop_sbar->relocate({ul.x + 258,ul.y + 67});
		}
}

// TODO: Move to boe.startup.cpp
void plop_fancy_startup()
{
	sf::Texture pict_to_draw;
	sf::Event event;
	short i,j;
	
	RECT whole_window,from_rect;
	RECT logo_from = {0,0,350,350};
	RECT intro_from = {0,0,480,640};
	
	if(display_mode != 5) hideMenuBar();
	
//	for (i = 0;i < 8; i++)
//		OffsetRect(&trim_rects[i],61,37);
	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			terrain_there[i][j] = -1;
			
	win_to_rects[5].offset(TEXT_WIN_UL_X,TEXT_WIN_UL_Y);
	win_to_rects[2].offset(PC_WIN_UL_X,PC_WIN_UL_Y);
	win_to_rects[3].offset(ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
		
	whole_window = RECT(mainPtr);
	logo_from.offset((whole_window.right - logo_from.right) / 2,(whole_window.bottom - logo_from.bottom) / 2);
	pict_to_draw.loadFromImage(*ResMgr::get<ImageRsrc>("spidlogo"));
	from_rect = RECT(pict_to_draw);
	// TODO: Looping 10 times here is a bit of a hack; fix it
	for(int k = 0; k < 10; k++) {
		mainPtr.clear(sf::Color::Black);
		rect_draw_some_item(pict_to_draw, from_rect, mainPtr, logo_from);
		
		mainPtr.display();
		mainPtr.pollEvent(event);
	}
	play_sound(95); // Was originally negative, meaning async

	// Do bulk of graphics loading!!!

	Set_up_win();

	init_startup();
	
	int delay = 220;
	if(show_startup_splash){
		mainPtr.clear(sf::Color::Black);
		intro_from.offset((whole_window.right - intro_from.right) / 2,(whole_window.bottom - intro_from.bottom) / 2);
		pict_to_draw.loadFromImage(*ResMgr::get<ImageRsrc>("startsplash"));
		from_rect = RECT(pict_to_draw);
		rect_draw_some_item(pict_to_draw, from_rect, mainPtr, intro_from);
	} else delay = 60;
	delay = time_in_ticks(delay).asMilliseconds();
	mainPtr.display();
		sf::Clock timer;
		play_sound(-22);

		while(timer.getElapsedTime().asMilliseconds() < delay) {
			if(mainPtr.pollEvent(event)) {
				if(event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::KeyPressed)
					break;
			}
		}
	if(display_mode != 5) showMenuBar();
}

void fancy_startup_delay()
{
	// OK try this ... destroy and recreate window.
	/*DisposeWindow(mainPtr);
	mainPtr = GetNewCWindow(128,NULL,IN_FRONT);
	SetPort(mainPtr);						/* set window to current graf port 
	text_sbar = NewControl(mainPtr,&sbar_rect,tit,false,58,0,58,scrollBarProc,1);
	item_sbar = NewControl(mainPtr,&item_sbar_rect,tit,false,0,0,16,scrollBarProc,2);
	shop_sbar = NewControl(mainPtr,&shop_sbar_rect,tit,false,0,0,16,scrollBarProc,3);
	adjust_window_mode();
	RestoreDeviceClut(GetMainDevice()); */
	
	// TODO: This probably isn't needed here?
	TEXT.style = sf::Text::Regular;
	
	draw_startup(0);

	flushingInput = true;
}

void init_startup()
{
	startup_loaded = true;
	startup_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("startup"));
	startup_button_orig.loadFromImage(*ResMgr::get<ImageRsrc>("startbut"));
	startup_button_g.draw(sf::Sprite(startup_button_orig));
	anim_mess.loadFromImage(*ResMgr::get<ImageRsrc>("startanim"));
}

void init_animation()
{}

void next_animation_step() 
{}


void draw_startup(short but_type)
{
	RECT to_rect;
	RECT r1 = {-1000,-1000,5,1000},r2 = {-1000,-1000,1000,5},r3 = {418,-1000,2000,1000},
		r4 = {-1000,579,1000,2500};
	short i;
	
	if (startup_loaded == false)
		return;
	
//	r1.bottom = ul.x + 5;
//	tileImage(mainPtr, r1,bg_gworld,bg[4]);
//	r2.right = ul.y - 13;
//	tileImage(mainPtr, r2,bg_gworld,bg[4]);
//	r3.top += ul.x + 5;
//	tileImage(mainPtr, r3,bg_gworld,bg[4]);
//	r4.left += ul.y - 13;
//	tileImage(mainPtr, r4,bg_gworld,bg[4]);
	tileImage(mainPtr, windRect, bg_gworld, bg[4]);
	to_rect = startup_from[0];
	to_rect.offset(-13,5);
	rect_draw_some_item(startup_gworld,startup_from[0],to_rect,ul);
	to_rect = startup_top;
	to_rect.offset(ul);
	//PaintRect(&to_rect);
	
	for (i = 0; i < 5; i++) {
		rect_draw_some_item(startup_gworld,startup_from[1],startup_button[i],ul);
		draw_start_button(i,but_type);
		}
	draw_startup_anim(false);
	
	draw_startup_stats();
}

void draw_startup_anim(bool advance)
{
	static short startup_anim_pos = 43; // was a global variable, but since it's only used in this function I moved it here
	RECT anim_to = {4,1,44,276},anim_from;
	RECT anim_size = {0,0,48,301};
	
	anim_from = anim_to;
	anim_from.offset(-1,-4 + startup_anim_pos);
	if(advance) startup_anim_pos = (startup_anim_pos + 1) % 542;
	rect_draw_some_item(startup_button_orig,anim_size,startup_button[5],ul);
	anim_to.offset(startup_button[5].left, startup_button[5].top);
	rect_draw_some_item(anim_mess,anim_from,anim_to,ul,sf::BlendAlpha);
}

void draw_startup_stats()
{
	RECT from_rect,to_rect,party_to = {0,0,36,28},pc_rect,frame_rect;
	short i;
	char str[256];
	
	TEXT.font = "Dungeon";
	TEXT.style = sf::Text::Regular;
	TEXT.pointSize = 24;

	to_rect = startup_top;
	to_rect.offset(20 - 18, 35);
	TEXT.colour = sf::Color::White;

	if(!party_in_memory) {
		TEXT.pointSize = 20;
		to_rect.offset(175,40);
		win_draw_string(mainPtr,to_rect,"No Party in Memory",0,18,ul);
	} else {
		frame_rect = startup_top;
		frame_rect.inset(50,50);
		frame_rect.top += 30;
		frame_rect.offset(ul.x - 9,ul.y + 10);
		// TODO: Maybe I should rename that variable
		::frame_rect(mainPtr, frame_rect, sf::Color::White);

		to_rect.offset(203,37);
		win_draw_string(mainPtr,to_rect,"Your party:",0,18,ul);
		TEXT.pointSize = 12;
		TEXT.style = sf::Text::Bold;
		TEXT.font = "Geneva";
		for (i = 0; i < 6; i++) {
			pc_rect = startup_top;
			pc_rect.right = pc_rect.left + 300;
			pc_rect.bottom = pc_rect.top + 79;
			pc_rect.offset(60 + 232 * (i / 3) - 9,95 + 45 * (i % 3));
			
			if (univ.party[i].main_status > 0) {
				from_rect = calc_rect(2 * (univ.party[i].which_graphic / 8), univ.party[i].which_graphic % 8);
				to_rect = party_to,
				to_rect.offset(pc_rect.left,pc_rect.top);
				rect_draw_some_item(pc_gworld,from_rect,to_rect,ul,sf::BlendAlpha);

				TEXT.pointSize = 14;
				pc_rect.offset(35,0);
				win_draw_string(mainPtr,pc_rect,(char*)univ.party[i].name.c_str(),0,18,ul);
				to_rect.offset(pc_rect.left + 8,pc_rect.top + 8);
				
				}
			TEXT.pointSize = 12;
			pc_rect.offset(12,16);
			switch (univ.party[i].main_status) {
				case MAIN_STATUS_ALIVE:
					switch (univ.party[i].race) {
						case RACE_HUMAN: sprintf((char *) str,"Level %d Human",univ.party[i].level); break;
						case RACE_NEPHIL: sprintf((char *) str,"Level %d Nephilim",univ.party[i].level); break;
						case RACE_SLITH: sprintf((char *) str,"Level %d Slithzerikai",univ.party[i].level); break;
						case RACE_VAHNATAI: sprintf((char *) str,"Level %d Vahnatai",univ.party[i].level); break;
						case RACE_REPTILE: sprintf((char *) str,"Level %d Reptile",univ.party[i].level); break;
						case RACE_BEAST: sprintf((char *) str,"Level %d Beast",univ.party[i].level); break;
						case RACE_IMPORTANT: sprintf((char *) str,"Level %d V.I.P.",univ.party[i].level); break;
						case RACE_MAGE: sprintf((char *) str,"Level %d Human Mage",univ.party[i].level); break;
						case RACE_PRIEST: sprintf((char *) str,"Level %d Human Priest",univ.party[i].level); break;
						case RACE_HUMANOID: sprintf((char *) str,"Level %d Humanoid",univ.party[i].level); break;
						case RACE_DEMON: sprintf((char *) str,"Level %d Demon",univ.party[i].level); break;
						case RACE_UNDEAD: sprintf((char *) str,"Level %d Undead",univ.party[i].level); break;
						case RACE_GIANT: sprintf((char *) str,"Level %d Giant",univ.party[i].level); break;
						case RACE_SLIME: sprintf((char *) str,"Level %d Slime",univ.party[i].level); break;
						case RACE_STONE: sprintf((char *) str,"Level %d Golem",univ.party[i].level); break;
						case RACE_BUG: sprintf((char *) str,"Level %d Bug",univ.party[i].level); break;
						case RACE_DRAGON: sprintf((char *) str,"Level %d Dragon",univ.party[i].level); break;
						case RACE_MAGICAL: sprintf((char *) str,"Level %d Magical Creature",univ.party[i].level); break;
						case RACE_PLANT: sprintf((char *) str,"Level %d Plant",univ.party[i].level); break;
						case RACE_BIRD: sprintf((char *) str,"Level %d Bird",univ.party[i].level); break;
						default: sprintf((char *) str,"Level %d *ERROR INVALID RACE*",univ.party[i].level); break;
						}
					win_draw_string(mainPtr,pc_rect,(char *) str,0,18,ul);
					pc_rect.offset(0,13);
					sprintf((char *) str,"Health %d, Spell pts. %d",
						univ.party[i].max_health,univ.party[i].max_sp);
					win_draw_string(mainPtr,pc_rect,(char *) str,0,18,ul);
					break;
				case MAIN_STATUS_DEAD:
					win_draw_string(mainPtr,pc_rect,"Dead",0,18,ul);
					break;
				case MAIN_STATUS_DUST:
					win_draw_string(mainPtr,pc_rect,"Dust",0,18,ul);
					break;
				case MAIN_STATUS_STONE:
					win_draw_string(mainPtr,pc_rect,"Stone",0,18,ul);
					break;
				case MAIN_STATUS_FLED:
					win_draw_string(mainPtr,pc_rect,"Fled",0,18,ul);
					break;
				default: //absent, and all variations thereof; do nothing
					break;
				}
			}
		}
	
	TEXT.font = "Geneva";
	TEXT.pointSize = 10;
	TEXT.style = sf::Text::Bold;
	pc_rect = startup_from[0];
	pc_rect.offset(5,5);
	pc_rect.top = pc_rect.bottom - 25;
	pc_rect.left = pc_rect.right - 300;
	// TODO: Should replace this with a more appropriate copyright string
	win_draw_string(mainPtr,pc_rect,"Copyright 1997, All Rights Reserved, v1.0.2",0,18,ul);
		
	TEXT.colour = sf::Color::Black;
}



void draw_start_button(short which_position,short which_button)
{
	RECT from_rect,to_rect;
	const char *button_labels[] = {"Load Game","Make New Party","How To Order",
		"Start Scenario","Custom Scenario","Quit"};
	// The 0..65535 version of the blue component was 14472; the commented version was 43144431
	sf::Color base_color = {0,0,57};
	//RGBColor lit_color = {0,0,57},base_color = {0,0,169};
	
	from_rect = startup_from[3];
	from_rect.offset((which_button > 0) ? 40 : 0,0);
	to_rect = startup_button[which_position];
	to_rect.left += 4; to_rect.top += 4;
	to_rect.right = to_rect.left + 40;
	to_rect.bottom = to_rect.top + 40;
	rect_draw_some_item(startup_gworld,from_rect,to_rect,ul);
	
	TEXT.font = "Dungeon";
	TEXT.style = sf::Text::Regular;
	TEXT.pointSize = 24;
	to_rect = startup_button[which_position];
	//to_rect.left += 80;
	to_rect.offset(10, 10);
	if (which_button == 5)
		which_button = 4;
	// In the 0..65535 range, this was 14472 + (12288 * which_button)
	base_color.b += (48 * which_button);
	TEXT.colour = base_color;
	if (which_position == 3)
		to_rect.offset(-7,0);
	win_draw_string(mainPtr,to_rect,(char *) button_labels[which_position],1,18,ul);
	TEXT.colour = sf::Color::Black;
	TEXT.font = "Geneva";
	TEXT.style = sf::Text::Bold;
	TEXT.pointSize = 10;
	
}

void main_button_click(short mode,RECT button_rect)
{
	button_rect.offset(ul);
	mainPtr.setActive();
	clip_rect(mainPtr, button_rect);

	draw_buttons(1);
	if (play_sounds == true)
		play_sound(37);
	else sf::sleep(time_in_ticks(5));
	draw_buttons(0);
	undo_clip(mainPtr);
}

void arrow_button_click(RECT button_rect)
{
	button_rect.offset(ul);
	mainPtr.setActive();
	clip_rect(mainPtr, button_rect);
	
	refresh_stat_areas(1);
	mainPtr.display();
	if (play_sounds == true)
		play_sound(37);
	else sf::sleep(time_in_ticks(5));
	undo_clip(mainPtr);
	refresh_stat_areas(0);
}


void reload_startup()////
{
	if (startup_loaded == true)
		return;
	
	mini_map.setVisible(false);
	map_visible = false;				
	mainPtr.setActive();
	init_startup();

	startup_loaded = true;
	//load_main_screen();
}

void end_startup()
{
	if (startup_loaded == false)
		return;

	startup_gworld.create(1,1);
	startup_button_orig.create(1,1);
	startup_button_g.create(1,1);
	anim_mess.create(1,1);

	startup_loaded = false;
	load_main_screen();
}

// This loads the graphics at the top of the game.
void Set_up_win ()
{
	RECT temp_rect = {0,0,0,280},map_world_rect;
	RECT map_rect = {0,0,384,384};
	RECT pc_rect = {0,0,216,113};
	sf::Texture temp_gworld;
	char fn1[] = "Geneva";
	char fn2[] = "Dungeon Bold";
	char fn3[] = "Palatino";
	short i;
	RECT r;
	
	temp_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("terscreen"));
	terrain_screen_gworld.draw(sf::Sprite(temp_gworld));

	// Create and initialize map gworld
	
	if(!map_gworld.create(map_rect.width(), map_rect.height())) {
		play_sound(2,3);
		exit(1);
	} else {
		map_world_rect = RECT(map_gworld);
		fill_rect(map_gworld, map_world_rect, sf::Color::White);
	}

}

void load_main_screen()
{
	if (invenbtn_gworld.getSize().x > 0)
		return;
	sf::Texture temp_gworld;
	
	// TODO: Hopefully reusing a texture here won't cause issues...
	invenbtn_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("invenbtns"));
	temp_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("statarea"));
	pc_stats_gworld.draw(sf::Sprite(temp_gworld));
	temp_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("inventory"));
	item_stats_gworld.draw(sf::Sprite(temp_gworld));
	temp_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("transcript"));
	text_area_gworld.draw(sf::Sprite(temp_gworld));
	temp_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("textbar"));
	text_bar_gworld.draw(sf::Sprite(temp_gworld));
	orig_text_bar_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("textbar"));
	buttons_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("buttons"));
   
//	set_gworld_fonts(geneva_font_num);
}

void set_gworld_fonts(short font_num)
{
	// TODO: This doesn't even make sense since fonts aren't set for each surface in SFML
#if 0
	GrafPtr old_port;

	GetPort(&old_port);	
	SetPort(GetWindowPort(mainPtr));
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(pc_stats_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(item_stats_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(text_area_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(text_bar_gworld);
	TextFont(font_num);
	TextFace(bold);
	TextSize(10);
	SetPort(old_port);	
#endif
}

void draw_main_screen()
{
	if (overall_mode == MODE_TALKING) {
		put_background();
		}
		else {
			rect_draw_some_item(terrain_screen_gworld.getTexture(), win_from_rects[0], win_to_rects[0],ul);
 
			draw_buttons(0);
			if (overall_mode == MODE_COMBAT)
				draw_pcs(pc_pos[current_pc],1);
			if (overall_mode == MODE_FANCY_TARGET)
				draw_targets(center);	
			}	
	draw_text_area(0);
	text_sbar->show();
	text_sbar->draw();
	item_sbar->show();
	item_sbar->draw();
	if (overall_mode == MODE_SHOPPING) {
		shop_sbar->show();
		shop_sbar->draw();
		}
	else shop_sbar->hide();
}

// redraw_screen does the very first redraw, and any full redraw
void redraw_screen(){
	if(in_startup_mode)
		draw_startup(0);
	else{
		switch (overall_mode) {
			case MODE_TALKING:
				put_background();
				refresh_talking();
				break;
			case MODE_SHOPPING:
				put_background();
				refresh_shopping();
				break;
			default:
				draw_main_screen();
				draw_terrain(0);
				draw_text_bar(1);
				if (overall_mode == MODE_COMBAT)
					draw_pcs(pc_pos[current_pc],1);
				if (overall_mode == MODE_FANCY_TARGET)
					draw_targets(center);
				break;
		}
		put_pc_screen();
		put_item_screen(stat_window,0);
		print_buf();
		text_sbar->show();
		text_sbar->draw();
		item_sbar->show();
		item_sbar->draw();
		if (overall_mode == MODE_SHOPPING) {
			shop_sbar->show();
			shop_sbar->draw();
		}
		else shop_sbar->hide();
	}
}

void put_background()
{
	RECT bg_pict;

	if (is_out()) {
		if (univ.party.outdoor_corner.x >= 7)
			bg_pict = bg[0]; 
			else bg_pict = bg[10];
		}
	else if (is_combat()) {
		if (univ.party.outdoor_corner.x >= 7)
			bg_pict = bg[2]; 
			else bg_pict = bg[4];
		}
	else {
		if (univ.town->lighting_type > 0) {
			if (univ.party.outdoor_corner.x >= 7)
				bg_pict = bg[1]; 
				else bg_pict = bg[9]; 
			}
			else if ((univ.party.outdoor_corner.x >= 7) && (univ.town.num != 21)) // TODO: What's so special about town 21?
				bg_pict = bg[8]; 
				else bg_pict = bg[13]; 
		}
	tileImage(mainPtr, RECT(mainPtr), bg_gworld, bg_pict);
	text_sbar->show();
	text_sbar->draw();
	item_sbar->show();
	item_sbar->draw();
}

void draw_buttons(short mode)
//mode; // 0 - regular   1 - button action
{
	RECT	source_rect = {0,0,37,258}, dest_rec;	bool spec_draw = false;
	
	if (mode == 1) {
		spec_draw = true;
		mode -= 100;
		}
	
	sf::Texture& buttons_to_draw = buttons_gworld;
	
	if (is_combat()) { // TODO: Draw buttons one at a time instead of singly
		source_rect.top += 37;
		source_rect.bottom += 37;
		}
	if (is_town()) {
		source_rect.top += 74;
		source_rect.bottom += 74;
		}
	
	dest_rec = win_to_rects[1];
	dest_rec.offset(ul);
	
	// TODO: I think this was supposed to tint it blue?
//	if (spec_draw == true)
//		ForeColor(blueColor);
	rect_draw_some_item(buttons_gworld, source_rect, mainPtr, dest_rec);
	// Maybe try sf::BlendAdd when spec_draw is true?
//	if (spec_draw == true)
//		ForeColor(blackColor);

	put_background();

}

// In general, refreshes any area that has text in it, the stat areas, the text bar
void draw_text_area(short mode)
//short mode unused
{
	refresh_stat_areas(0);

	draw_text_bar(0);
}

void reset_text_bar()
{
remember_tiny_text = 300;
}


void draw_text_bar(short mode)
//short mode; // 0 - no redraw  1 - forced
{
	//short num_rect[3] = {12,10,4}; // Why? Just... why?
	short i;
	location loc;
	std::string combat_string;
	
	loc = (is_out()) ? global_to_local(univ.party.p_loc) : univ.town.p_loc;

	if (mode == 1)
		remember_tiny_text = 500;	   
	if ((PSD[SDF_PARTY_STEALTHY] > 0)    || (PSD[SDF_PARTY_FLIGHT] > 0) ||
		(PSD[SDF_PARTY_DETECT_LIFE] > 0) || (PSD[SDF_PARTY_FIREWALK] > 0) )
		remember_tiny_text = 500;
	if (is_out()) {
		for (i = 0; i < 8; i++)
			if (loc.in(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].info_rect[i])) 
				if ((remember_tiny_text == i) && (mode == 0))
					return;
					else {
						put_text_bar(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].out_strs(i + 1));
						remember_tiny_text = i;
						return;
						}
		if (remember_tiny_text != 50 + univ.party.i_w_c.x + univ.party.i_w_c.y) {
			put_text_bar((char *) univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].out_strs(0));
			remember_tiny_text = 50 + univ.party.i_w_c.x + univ.party.i_w_c.y;
			}
		}
	if (is_town()) {
		for (i = 0; i < 16; i++)
			if (loc.in(univ.town->room_rect(i))) 
				if ((remember_tiny_text == 200 + i) && (mode == 0))
					return;
					else {
						put_text_bar(univ.town->rect_names[i]);
						remember_tiny_text = 200 + i;
						return;
						}
		if (remember_tiny_text != 250) {
			put_text_bar((char *) univ.town->town_name); ////
			remember_tiny_text = 250;
			}
	
		}
	if ((is_combat()) && (current_pc < 6) && (monsters_going == false)) {
		std::ostringstream sout;
		sout << univ.party[current_pc].name << " (ap: " << univ.party[current_pc].ap << ')';
		combat_string = sout.str();
		put_text_bar((char *) combat_string.c_str());
		remember_tiny_text = 500;
		}
	if ((is_combat()) && (monsters_going == true))	// Print bar for 1st monster with >0 ap -
	   // that is monster that is going
	   for (i = 0; i < univ.town->max_monst(); i++)
	   	if ((univ.town.monst[i].active > 0) && (univ.town.monst[i].ap > 0)) {
	   		combat_string = print_monster_going(univ.town.monst[i].number,univ.town.monst[i].ap);
			put_text_bar((char *) combat_string.c_str());
			remember_tiny_text = 500;
			i = 400;	   
	   }
}

void put_text_bar(char *str) {
	text_bar_gworld.setActive();
	rect_draw_some_item(orig_text_bar_gworld, win_from_rects[4], text_bar_gworld, win_from_rects[4]);
	TEXT.colour = sf::Color::White;
	RECT to_rect = RECT(text_bar_gworld);
	// TODO: The 12 is the line height; not sure what it should be, so I just picked something
	win_draw_string(text_bar_gworld, to_rect, str, 2, 12);

	if (monsters_going == false) {
		to_rect.left = 205;
		to_rect.top = 14;
		if (PSD[SDF_PARTY_STEALTHY] > 0) {
			win_draw_string(text_bar_gworld, to_rect, "Stealth", 2, 12);
			to_rect.left -= 60;
			}
		if (PSD[SDF_PARTY_FLIGHT] > 0) {
			win_draw_string(text_bar_gworld, to_rect, "Flying", 2, 12);
			to_rect.left -= 60;
			}
		if (PSD[SDF_PARTY_DETECT_LIFE] > 0) {
			win_draw_string(text_bar_gworld, to_rect, "Detect Life", 2, 12);
			to_rect.left -= 60;
			}
		if (PSD[SDF_PARTY_FIREWALK] > 0) {
			win_draw_string(text_bar_gworld, to_rect, "Firewalk", 2, 12);
			to_rect.left -= 60;
			}
		}

	TEXT.colour = sf::Color::Black;
	mainPtr.setActive();
	rect_draw_some_item(text_bar_gworld.getTexture(), win_from_rects[4], win_to_rects[4],ul);
}

// this is used for determinign whether to round off walkway corners
// right now, trying a restrictive rule (just cave floor and grass, mainly)
bool is_nature(char x, char y, unsigned char ground_t)
{
	ter_num_t ter_type;
	
	ter_type = coord_to_ter((short) x,(short) y);
	return ground_t == scenario.ter_types[ter_type].ground_type;
}

ter_num_t get_ground_from_ter(ter_num_t ter){
	return get_ter_from_ground(scenario.ter_types[ter].ground_type);
}

ter_num_t get_ter_from_ground(unsigned char ground){
	for(int i = 0; i < 256; i++)
		if(scenario.ter_types[i].ground_type == ground)
			return i;
	return 0;
}

void draw_terrain(short	mode)
//mode ... if 1, don't place on screen after redoing
// if 2, only redraw over active monst
{
	char q,r;
	location where_draw;
	location sector_p_in,view_loc;
	char can_draw;
	ter_num_t spec_terrain;
	bool off_terrain = false,draw_frills = true;
	short i,j;
	
	if(overall_mode == MODE_TALKING || overall_mode == MODE_SHOPPING || overall_mode == MODE_STARTUP)
		return;
	
	if (mode == 2) {
		if (current_working_monster < 0) return;
		supressing_some_spaces = true;
		for (i = 0; i < 4; i++) ok_space[i].x = -1;
		if (current_working_monster >= 100) {
			for (i = 0; i < univ.town.monst[current_working_monster - 100].x_width; i++)
				for (j = 0; j < univ.town.monst[current_working_monster - 100].y_width; j++) {
					ok_space[i + 2 * j].x = univ.town.monst[current_working_monster - 100].cur_loc.x + i;
					ok_space[i + 2 * j].y = univ.town.monst[current_working_monster - 100].cur_loc.y + j;
					ok_space[i + 2 * j].x = ok_space[i + 2 * j].x - center.x + 4;
					ok_space[i + 2 * j].y = ok_space[i + 2 * j].y - center.y + 4;
					}
			}
		if (current_working_monster < 6) {
			ok_space[0] = pc_pos[current_working_monster];
			ok_space[0].x = ok_space[0].x - center.x + 4;
			ok_space[0].y = ok_space[0].y - center.y + 4;	
			}
		mode = 0;
		}

	mainPtr.setActive();
	
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) {
			light_area[i][j] = 0;
			unexplored_area[i][j] = 0;
			}
	
			
	sector_p_in.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
	sector_p_in.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;

	anim_ticks++;
	anim_onscreen = false;

	if (is_town())
		view_loc = univ.town.p_loc;
	if (is_combat())
		view_loc = pc_pos[(current_pc < 6) ? current_pc : first_active_pc()];
	
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) {
			where_draw =  (is_out()) ? univ.party.p_loc : center;
			where_draw.x += i - 6;
			where_draw.y += j - 6;
			if (!(is_out())) 
				light_area[i][j] = (is_town()) ? pt_in_light(view_loc,where_draw) : combat_pt_in_light(where_draw);
			if (!(is_out()) && ((where_draw.x < 0) || (where_draw.x > univ.town->max_dim() - 1) 
				|| (where_draw.y < 0) || (where_draw.y > univ.town->max_dim() - 1))) 
					unexplored_area[i][j] = 0;
				else unexplored_area[i][j] = 1 - is_explored(where_draw.x,where_draw.y);
			}

	for (q = 0; q < 9; q++) {
		for (r = 0; r < 9; r++) {
			where_draw = (is_out()) ? univ.party.p_loc : center;
			where_draw.x += q - 4;
			where_draw.y += r - 4;
			off_terrain = false;
			
			draw_frills = true;
			if (!(is_out()) && ((where_draw.x < 0) || (where_draw.x > univ.town->max_dim() - 1) 
								|| (where_draw.y < 0) || (where_draw.y > univ.town->max_dim() - 1))) {
				draw_frills = false;
				// Warning - this section changes where_draw
				if (where_draw.x < 0)
					where_draw.x = -1;
				if (where_draw.x > univ.town->max_dim() - 1)
					where_draw.x = univ.town->max_dim();
				if (where_draw.y < 0)
					where_draw.y = -1;
				if (where_draw.y > univ.town->max_dim() - 1) 
					where_draw.y = univ.town->max_dim();
				if (can_see(view_loc,where_draw,0) < 5)
					can_draw = 1;
				else can_draw = 0;
				spec_terrain = 0;
			}
			else if (is_out()) {
				if ((where_draw.x < 0) || (where_draw.x > 95) 
					|| (where_draw.y < 0) || (where_draw.y > 95))
					can_draw = 0;
				else {
					spec_terrain = univ.out[where_draw.x][where_draw.y];
					can_draw = univ.out.out_e[where_draw.x][where_draw.y];
				}
			}
			else if (is_combat()) {
				spec_terrain = combat_terrain[where_draw.x][where_draw.y];
				can_draw = (((is_explored(where_draw.x,where_draw.y)) ||
								  (which_combat_type == 0) || (monsters_going == true) || (overall_mode != MODE_COMBAT))
								 && (party_can_see(where_draw) < 6)) ? 1 : 0;
			}
			else {
				spec_terrain = univ.town->terrain(where_draw.x,where_draw.y);
				can_draw = is_explored(where_draw.x,where_draw.y);
				
				if (can_draw > 0) {
					if (pt_in_light(univ.town.p_loc,where_draw) == false)
						can_draw = 0;
				}
				if ((overall_mode == MODE_LOOK_TOWN) && (can_draw == 0))
					can_draw = (party_can_see(where_draw) < 6) ? 1 : 0;
			}
			spot_seen[q][r] = can_draw;
			
			if ((can_draw != 0) && (overall_mode != MODE_RESTING)) { // if can see, not a pit, and not resting
				if (is_combat()) anim_ticks = 0;
				
				eTrimType trim = scenario.ter_types[spec_terrain].trim_type;
				
				// Finally, draw this terrain spot
				if(trim == TRIM_WALKWAY){
					int trim = -1;
					unsigned char ground_t = scenario.ter_types[spec_terrain].trim_ter;
					ter_num_t ground_ter = get_ter_from_ground(ground_t);
					if (!loc_off_act_area(where_draw)) {
						if(is_nature(where_draw.x - 1,where_draw.y,ground_t)){ // check left
							if(is_nature(where_draw.x,where_draw.y - 1,ground_t)){ // check up
								if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
									if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
										trim = 8;
									else trim = 4;
								}else if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
									trim = 7;
								else trim = 1;
							}else if(is_nature(where_draw.x,where_draw.y + 1,ground_t)){ // check down
								if(is_nature(where_draw.x + 1,where_draw.y,ground_t)) // check right
									trim = 6;
								else trim = 0;
							}
						}else if(is_nature(where_draw.x,where_draw.y - 1,ground_t)){ // check up
							if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
								if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
									trim = 5;
								else trim = 2;
							}
						}else if(is_nature(where_draw.x + 1,where_draw.y,ground_t)){ // check right
							if(is_nature(where_draw.x,where_draw.y + 1,ground_t)) // check down
								trim = 3;
						}
					}
					draw_one_terrain_spot(q,r,trim < 0 ? spec_terrain : ground_ter);
					if(trim >= 0)
						draw_trim(q,r,trim + 50,spec_terrain);
				}else if(trim == TRIM_ROAD || trim == TRIM_N || trim == TRIM_S ||
						 trim == TRIM_W || trim == TRIM_E) {
					draw_one_terrain_spot(q,r,spec_terrain);
					place_road(q,r,where_draw,trim == TRIM_ROAD);
				}else if(spec_terrain == 65535) {
					draw_one_terrain_spot(q,r,-1);
				}else{
					current_ground = get_ground_from_ter(spec_terrain);
					draw_one_terrain_spot(q,r,spec_terrain);
				}
			}
			else {  // Can't see. Place darkness.
				draw_one_terrain_spot(q,r,-1);
			}
			
			if ((can_draw != 0) && (overall_mode != MODE_RESTING) && frills_on && draw_frills)
				place_trim((short) q,(short) r,where_draw,spec_terrain);
//			if((is_town() && univ.town.is_spot(where_draw.x,where_draw.y)) ||
//			   (is_out() && univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].special_spot[where_draw.x][where_draw.y]))
//				Draw_Some_Item(roads_gworld, calc_rect(6, 0), terrain_screen_gworld, loc(q,r), 1, 0);
			// TODO: Move draw_sfx, draw_items, draw_fields, draw_spec_items, etc to here
			
			if (is_town() || is_combat()) {
				draw_items(where_draw);
			}
			draw_fields(where_draw);
			//draw_monsters(where_draw);
			//draw_vehicles(where_draw);
			//if(is_combat) draw_pcs(where_draw); else draw_party(where_draw);
		}
	}
	
//	if ((overall_mode != MODE_RESTING) && (!is_out())) 
//		draw_sfx();
//		
//	// Now place items
//	if ((overall_mode > MODE_OUTDOORS) && (overall_mode != MODE_LOOK_OUTDOORS) && (overall_mode != MODE_RESTING))
//		draw_items();		
//		
//	// Now place fields
//	if ((overall_mode != MODE_RESTING) && (!is_out())) {
//		draw_fields();
//		draw_spec_items();
//		}
//
	// Not camping. Place misc. stuff
	if (overall_mode != MODE_RESTING) {
		if (is_out())
			draw_outd_boats(univ.party.p_loc);
			else if ((is_town()) || (which_combat_type == 1))
				draw_town_boat(center);		
		draw_monsters();
		}

	if ((overall_mode < MODE_COMBAT) || (overall_mode == MODE_LOOK_OUTDOORS) || ((overall_mode == MODE_LOOK_TOWN) && (point_onscreen(univ.town.p_loc,center) == true))
		|| (overall_mode == MODE_RESTING))
		draw_party_symbol(mode,center);
		else if (overall_mode != MODE_LOOK_TOWN)
			draw_pcs(center,0);
	
	// Now do the light mask thing
	apply_light_mask();
	apply_unseen_mask();
	
	if (mode == 0) {
		redraw_terrain();
		draw_text_bar(0);
		if ((overall_mode >= MODE_COMBAT) && (overall_mode != MODE_LOOK_OUTDOORS) && (overall_mode != MODE_LOOK_TOWN) && (overall_mode != MODE_RESTING))
			draw_pcs(center,1);
		if (overall_mode == MODE_FANCY_TARGET)
			draw_targets(center);
	}
	supressing_some_spaces = false;
}


ter_num_t get_ground_for_shore(ter_num_t ter){
	if(scenario.ter_types[ter].block_horse) return current_ground;
	else if(scenario.ter_types[ter].blockage > 2) return current_ground;
	else return ter;
}

void place_trim(short q,short r,location where,ter_num_t ter_type)
{
	bool at_top = false,at_bot = false,at_left = false,at_right = false;
	location targ;
	
	// FIrst quick check ... if a pit or barrier in outdoor combat, no trim
	if ((is_combat()) && (which_combat_type == 0) && (ter_type == 90))
		return;
	if (PSD[SDF_NO_SHORE_FRILLS] > 0)
		return;
		
	targ.x = q;
	targ.y = r;
	if (supressing_some_spaces && (targ != ok_space[0]) && (targ != ok_space[1]) &&
		(targ != ok_space[2]) && (targ != ok_space[3]))
			return;
	
					
	if (where.x == 0)
		at_left = true;
	if (where.y == 0)
		at_top = true;
	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS)) {
		if (where.x == 95)
			at_right = true;
		if (where.y == 95)
			at_bot = true;
		}
		else {
		if (where.x == univ.town->max_dim())
			at_right = true;
		if (where.y == univ.town->max_dim())
			at_bot = true;
		}
		
	// First, trim for fluids
	if(is_fluid(ter_type)){
		short trim = get_fluid_trim(where, ter_type);
		if (trim != 0) {
			ter_num_t shore;
			if (trim & 2){ // upper right
				shore = coord_to_ter(where.x + 1, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,5,shore);
			}if (trim & 8){ // lower right
				shore = coord_to_ter(where.x + 1, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,7,shore);
			}if (trim & 32){ // lower left
				shore = coord_to_ter(where.x - 1, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,6,shore);
			}if (trim & 128){ // upper left
				shore = coord_to_ter(where.x - 1, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,4,shore);
			}if (trim & 1){ // top
				shore = coord_to_ter(where.x, where.y - 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,2,shore);
			}if (trim & 4){ // right
				shore = coord_to_ter(where.x + 1, where.y);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,1,shore);
			}if (trim & 16){ // bottom
				shore = coord_to_ter(where.x, where.y + 1);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,3,shore);
			}if (trim & 64){ // left
				shore = coord_to_ter(where.x - 1, where.y);
				shore = get_ground_for_shore(shore);
				draw_trim(q,r,0,shore);
			}
		}
	}
	
	if(is_wall(ter_type) && !at_top && !at_bot && !at_left && !at_right){
	//if (((ter_type >= 100) && (ter_type <= 137)) && (at_top == false) &&
	//	(at_bot == false) && (at_left == false) && (at_right == false)) {
		ter_num_t to_left = coord_to_ter(where.x - 1,where.y);
		ter_num_t above = coord_to_ter(where.x,where.y - 1);
		ter_num_t to_right = coord_to_ter(where.x + 1,where.y);
		ter_num_t below = coord_to_ter(where.x,where.y + 1);
		if (is_wall(to_left) && is_wall(above) && is_ground(to_right) && is_ground(below))
			draw_trim(q,r,11,to_right);
		
		if (is_wall(to_left) && is_wall(below) && is_ground(to_right) && is_ground(above))
			draw_trim(q,r,9,to_right);
		
		if (is_wall(to_right) && is_wall(above) && is_ground(to_left) && is_ground(below))
			draw_trim(q,r,10,to_left);
			
		if (is_wall(to_right) && is_wall(below) && is_ground(to_left) && is_ground(above))
			draw_trim(q,r,8,to_left);		
			
		if (is_ground(to_left) && is_ground(above) && is_ground(to_right) && is_wall(below)) {
			draw_trim(q,r,8,to_right);
			draw_trim(q,r,9,to_right);
		}

		if (is_wall(to_left) && is_ground(below) && is_ground(to_right) && is_ground(above)) {
			draw_trim(q,r,9,to_right);
			draw_trim(q,r,11,to_right);
		}

		if (is_ground(to_right) && is_wall(above) && is_ground(to_left) && is_ground(below)) {
			draw_trim(q,r,10,to_left);
			draw_trim(q,r,11,to_left);
		}

		if (is_wall(to_right) && is_ground(below) && is_ground(to_left) && is_ground(above)) {
			draw_trim(q,r,8,to_left);
			draw_trim(q,r,10,to_left);
		}
		
		if (is_ground(to_right) && is_ground(below) && is_ground(to_left) && is_ground(above)) {
			draw_trim(q,r,8,to_left);
			draw_trim(q,r,9,to_right);
			draw_trim(q,r,10,to_left);
			draw_trim(q,r,11,to_right);
		}
	//	}
	}
}

void draw_trim(short q,short r,short which_trim,ter_num_t ground_ter)
//which_trim is 3 -> drawing wall trim -> might shift down if ground is grass
//short which_mode;  // 0 top 1 bottom 2 left 3 right 4 up left 5 up right 6 down right 7 down left
{
	// which_trim
	// 0 - left, 1 - right, 2 - top, 3 - bottom, 4 - tl, 5 - tr, 6 - bl, 7 - br
	// 8 - wall tl, 9 - wall tr, 10 - wall bl, 11 - wall br
	// 50 - walkway bl, 51 - walkway tl, 52 - walkway tr, 53 - walkway br
	// 54 - walkway top, 55 - walkway right, 56 - walkway bottom, 57 - walkway left
	// 58 - lone walkway
	RECT from_rect = {0,0,36,28},to_rect,mask_rect;
	sf::Texture* from_gworld;
	// TODO: The duplication of RECT here shouldn't be necessary...
	static RECT trim_rects[] = {
		RECT{0,0,36,14}, RECT{0,0,36,14},
		RECT{0,0,18,28}, RECT{0,0,18,28},
		RECT{0,0,18,14}, RECT{0,0,18,14}, RECT{0,0,18,14}, RECT{0,0,18,14},
		RECT{0,0,18,14}, RECT{0,0,18,14}, RECT{0,0,18,14}, RECT{0,0,18,14},
	};
	static RECT walkway_rects[] = {
		RECT{0,0,36,28}, RECT{0,0,36,28}, RECT{0,0,36,28}, RECT{0,0,36,28},
		RECT{0,0,36,28}, RECT{0,0,36,28}, RECT{0,0,36,28}, RECT{0,0,36,28},
		RECT{0,0,36,28},
	};
	static bool inited = false;
	if(!inited){
		inited = true;
		trim_rects[1].offset(14,0);
		trim_rects[2].offset(28,0);
		trim_rects[3].offset(28,18);
		trim_rects[4].offset(56,0);
		trim_rects[5].offset(70,0);
		trim_rects[6].offset(56,18);
		trim_rects[7].offset(70,18);
		trim_rects[8].offset(84,0);
		trim_rects[9].offset(98,0);
		trim_rects[10].offset(84,18);
		trim_rects[11].offset(98,18);
		int i;
		for(i = 0; i < 12; i++) trim_rects[i].offset(112,36);
		for(i = 0; i < 8 ; i++) walkway_rects[i].offset((i%4)*28,(i/4)*36);
		walkway_rects[8].offset(196,0);
	}
	sf::Color test_color = {0,0,0}, store_color;

	if (frills_on == false)
		return;
		
	// if current ground is grass, forget trim
//	if ((current_ground == 2) && (which_trim < 3))
//		return; 

	terrain_there[q][r] = -1;
		
//	from_rect.left = 28 * which_trim + trim_rects[which_mode].left;
//	from_rect.right = 28 * which_trim + trim_rects[which_mode].right;
//	from_rect.top = trim_rects[which_mode].top;
//	from_rect.bottom = trim_rects[which_mode].bottom;
//	
//	if ((which_trim == 3) && (current_ground == 2)) // trim corner walls with grass instead of cave floor
//		OffsetRect(&from_rect,0,36);
	unsigned short pic = scenario.ter_types[ground_ter].picture;
	if(pic < 400){
		from_gworld = &terrain_gworld[pic / 50];
		pic %= 50;
		from_rect.offset(28 * (pic % 10), 36 * (pic / 10));
	}else if(pic < 1000){
		from_gworld = &anim_gworld;
		pic %= 400;
		from_rect.offset(112 * (pic / 5),36 * (pic % 5));
	}else{
		pic %= 1000;
		graf_pos_ref(from_gworld, from_rect) = spec_scen_g.find_graphic(pic);
	}
	if(which_trim < 50) mask_rect = trim_rects[which_trim];
	else mask_rect = walkway_rects[which_trim - 50];
	to_rect = coord_to_rect(q,r);
//	to_rect.right = to_rect.left + trim_rects[which_mode].right;
//	to_rect.left = to_rect.left + trim_rects[which_mode].left;
//	to_rect.bottom = to_rect.top + trim_rects[which_mode].bottom;
//	to_rect.top = to_rect.top + trim_rects[which_mode].top;
	//	OffsetRect(&to_rect,-61,-37);
	if(which_trim == 0 || which_trim == 4 || which_trim == 6 || which_trim == 8 || which_trim == 10){
		from_rect.right -= 14;
		to_rect.right -= 14;
	}else if(which_trim == 1 || which_trim == 5 || which_trim == 7 || which_trim == 9 || which_trim == 11){
		from_rect.left += 14;
		to_rect.left += 14;
	}
	if(which_trim == 2 || which_trim == 4 || which_trim == 5 || which_trim == 8 || which_trim == 9){
		from_rect.bottom -= 18;
		to_rect.bottom -= 18;
	}else if(which_trim == 3 || which_trim == 6 || which_trim == 7 || which_trim == 10 || which_trim == 11){
		from_rect.top += 18;
		to_rect.top += 18;
	}

	rect_draw_some_item(*from_gworld, from_rect, terrain_screen_gworld, to_rect, sf::BlendAlpha);
}


bool extend_road_terrain(ter_num_t ter)
{
	eTrimType trim = scenario.ter_types[ter].trim_type;
	eTerSpec spec = scenario.ter_types[ter].special;
	ter_num_t flag = scenario.ter_types[ter].flag1.u;
	if(trim == TRIM_ROAD || trim == TRIM_CITY || trim == TRIM_WALKWAY)
		return true;
	if(spec == TER_SPEC_BRIDGE)
		return true;
	if(spec == TER_SPEC_TOWN_ENTRANCE && trim != TRIM_NONE)
		return true; // cave entrance, most likely
	if(spec == TER_SPEC_UNLOCKABLE || spec == TER_SPEC_CHANGE_WHEN_STEP_ON)
		return true; // closed door, possibly locked; or closed portcullis
	if(spec == TER_SPEC_CHANGE_WHEN_USED && scenario.ter_types[flag].special == TER_SPEC_CHANGE_WHEN_STEP_ON && scenario.ter_types[flag].flag1.u == ter)
		return true; // open door (I think) TODO: Verify this works
	if(spec == TER_SPEC_LOCKABLE)
		return true; // open portcullis (most likely)
	if(trim == TRIM_N || trim == TRIM_S || trim == TRIM_W || trim == TRIM_E)
		return true; // connect roads to trim boundaries
	return false;
}

bool connect_roads(ter_num_t ter){
	eTrimType trim = scenario.ter_types[ter].trim_type;
	eTerSpec spec = scenario.ter_types[ter].special;
	if(trim == TRIM_ROAD || trim == TRIM_CITY)
		return true;
	if(spec == TER_SPEC_TOWN_ENTRANCE && trim != TRIM_NONE)
		return true; // cave entrance, most likely
	return false;
}

void place_road(short q,short r,location where, bool here)
{
	location draw_loc;
	ter_num_t ter;
	RECT to_rect;
	//RECT road_rects[2] = {{76,112,80,125},{72,144,90,148}}; // 0 - rl partial  1 - ud partial
	// TODO: The duplication of RECT here shouldn't be necessary...
	static const RECT road_rects[4] = {
		RECT{4,112,8,125},	// horizontal partial
		RECT{0,144,18,148},	// vertical partial
		RECT{0,112,4,140},	// horizontal full
		RECT{0,140,36,144},	// vertical full
	};
	//RECT road_dest_rects[4] = {{0,12,18,16},{16,15,20,28},{18,12,36,16},{16,0,20,13}}; // top right bottom left
	static const RECT road_dest_rects[6] = {
		RECT{0,12,18,16},	// top
		RECT{16,15,20,28},	// right
		RECT{18,12,36,16},	// bottom
		RECT{16,0,20,13},	// left
		RECT{0,12,36,16},	// top + bottom
		RECT{16,0,20,28},	// right + left
	};
	draw_loc.x = q;
	draw_loc.y = r;
	
	terrain_there[q][r] = -1;
	
	if(here){
		if (where.y > 0)
			ter = coord_to_ter(where.x,where.y - 1);
		if ((where.y == 0) || extend_road_terrain(ter)) {
			to_rect = road_dest_rects[0];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[1], terrain_screen_gworld, to_rect);
		}
		
		if (((is_out()) && (where.x < 96)) || (!(is_out()) && (where.x < univ.town->max_dim() - 1)))
			ter = coord_to_ter(where.x + 1,where.y);
		if (((is_out()) && (where.x == 96)) || (!(is_out()) && (where.x == univ.town->max_dim() - 1)) 
			|| extend_road_terrain(ter)) {
			to_rect = road_dest_rects[1];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[0], terrain_screen_gworld, to_rect);
		}
		
		if (((is_out()) && (where.y < 96)) || (!(is_out()) && (where.y < univ.town->max_dim() - 1)))
			ter = coord_to_ter(where.x,where.y + 1);
		if (((is_out()) && (where.y == 96)) || (!(is_out()) && (where.y == univ.town->max_dim() - 1)) 
			|| extend_road_terrain(ter)) {
			to_rect = road_dest_rects[2];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[1], terrain_screen_gworld, to_rect);
		}
		
		if (where.x > 0)
			ter = coord_to_ter(where.x - 1,where.y);
		if ((where.x == 0) || extend_road_terrain(ter)) {
			to_rect = road_dest_rects[3];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[0], terrain_screen_gworld, to_rect);
		}
	}else{
		bool horz = false, vert = false;
		if (where.y > 0)
			ter = coord_to_ter(where.x,where.y - 1);
		if ((where.y == 0) || connect_roads(ter))
			vert = true;
		
		if (((is_out()) && (where.x < 96)) || (!(is_out()) && (where.x < univ.town->max_dim() - 1)))
			ter = coord_to_ter(where.x + 1,where.y);
		if (((is_out()) && (where.x == 96)) || (!(is_out()) && (where.x == univ.town->max_dim() - 1)) 
			|| connect_roads(ter))
			horz = true;
		
		if(vert){
			if (((is_out()) && (where.y < 96)) || (!(is_out()) && (where.y < univ.town->max_dim() - 1)))
				ter = coord_to_ter(where.x,where.y + 1);
			if (((is_out()) && (where.y == 96)) || (!(is_out()) && (where.y == univ.town->max_dim() - 1)) 
				|| connect_roads(ter))
				vert = true;
			else vert = false;
		}
		
		if(horz){
			if (where.x > 0)
				ter = coord_to_ter(where.x - 1,where.y);
			if ((where.x == 0) || connect_roads(ter)) 
				horz = true;
			else horz = false;
		}
		
		if(horz){
			to_rect = road_dest_rects[5];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[2], terrain_screen_gworld, to_rect);
		}
		if(vert){
			to_rect = road_dest_rects[4];
			to_rect.offset(13 + q * 28,13 + r * 36);
			rect_draw_some_item (roads_gworld, road_rects[3], terrain_screen_gworld, to_rect);
		}
	}
}

void draw_rest_screen()
{
	eGameMode store_mode;

	store_mode = overall_mode;
	overall_mode = MODE_RESTING;
	draw_terrain(0);
	overall_mode = store_mode ;
}

void boom_space(location where,short mode,short type,short damage,short sound)
// if mode is 100, force
//short type; // 0 - flame 1 - magic 2 - poison 3 - blood 4 - cold
	// 10s digit indicates sound  0 - normal ouch  1 - small sword  2 - loud sword
	// 3 - pole  4 - club  5 - fireball hit  6 - squish  7 - cold
	// 8 - acid  9 - claw  10 - bite  11 - slime  12 - zap  13 - missile hit
{
	location where_draw(4,4);
	RECT source_rect = {0,0,36,28},text_rect,dest_rect = {13,13,49,41},big_to = {13,13,337,265},store_rect;
	short del_len;
	char dam_str[20];
	short x_adj = 0,y_adj = 0,which_m;
	short sound_to_play[20] = {97,69,70,71,72, 73,55,75,42,86,
			87,88,89,98,0, 0,0,0,0,0};

	//sound_key = type / 10;
	//type = type % 10;
	
//	if ((cartoon_happening == true) && (anim_step < 140))
//		return;
	if ((mode != 100) && (party_can_see(where) == 6))
		return;
	if ((type < 0) || (type > 4))
		return;
	if ((boom_anim_active == true) && (type != 3))
		return;
	if ((PSD[SDF_NO_FRILLS] > 0) && fast_bang)
		return;
	if (is_out())
		return;
			
	// Redraw terrain in proper position
	if ((((point_onscreen(center,where) == false) && (overall_mode >= MODE_COMBAT)) || (overall_mode == MODE_OUTDOORS))
		) {
		play_sound(sound_to_play[sound]);

		return;
		}
		else if (is_combat()) {
			if (which_combat_type == 1)
				draw_terrain(0);
				else draw_terrain(0);
			}
			else if (fast_bang < 2) {
				draw_terrain(0);
				if (fast_bang == 1)
					fast_bang = 2;
				}

	where_draw.x = where.x - center.x + 4;
	where_draw.y = where.y - center.y + 4;
//	source_rect.left += 28 * type;
//	source_rect.right += 28 * type;
	
	// adjust for possible big monster
	which_m = monst_there(where);
	if (which_m < 90) {
		x_adj += 14 * (univ.town.monst[which_m].x_width - 1);
		y_adj += 18 * (univ.town.monst[which_m].y_width - 1);
		}
	dest_rect.offset(where_draw.x * 28,where_draw.y * 36);
	source_rect = store_rect = dest_rect;
	dest_rect.offset(x_adj,y_adj);
	dest_rect |= big_to;

	dest_rect.offset(win_to_rects[0].left,win_to_rects[0].top);

	source_rect.offset(-store_rect.left + 28 * type,-store_rect.top);
	rect_draw_some_item(boom_gworld,source_rect,dest_rect,ul,sf::BlendAlpha);
	
	if ((dest_rect.right - dest_rect.left >= 28) && (dest_rect.bottom - dest_rect.top >= 36)) {
		sprintf((char *) dam_str,"%d",damage);
		TEXT.pointSize = 10;
		TEXT.style = sf::Text::Bold;
		//text_rect = coord_to_rect(where_draw.x,where_draw.y);
		//OffsetRect(&text_rect,x_adj,y_adj);
		text_rect = dest_rect;
		text_rect.top += 10;
		if ((damage < 10) && (dest_rect.right - dest_rect.left > 19))
			text_rect.left += 10;
		text_rect.offset(-4,-5);
		win_draw_string(mainPtr,text_rect,(char *) dam_str,1,10,ul);
		TEXT.style = sf::Text::Regular;
		}
		play_sound((skip_boom_delay?-1:1)*sound_to_play[sound]);
	mainPtr.display();
		if ((sound == 6) && (fast_bang == 0) && (!skip_boom_delay))
			sf::sleep(time_in_ticks(12));

	
	if (fast_bang == 0 && !skip_boom_delay) {
		del_len = PSD[SDF_GAME_SPEED] * 3 + 4;
		if (play_sounds == false)
			sf::sleep(time_in_ticks(del_len));
		}
	redraw_terrain();
	if ((overall_mode >= MODE_COMBAT/*9*/) && (overall_mode != MODE_LOOK_OUTDOORS) && (overall_mode != MODE_LOOK_TOWN) && (overall_mode != MODE_RESTING))
		draw_pcs(center,1);	
}
	

void draw_pointing_arrows() 
{
	//RECT sources[4] = {{65,46,73,54},{56,46,64,54},{56,37,64,45},{65,37,73,45}};
	// TODO: The duplication of  here shouldn't be necessary...
	RECT sources[4] = {
		RECT{352,58,360,76}, // up
		RECT{352,20,360,38}, // left
		RECT{352,01,360,19}, // down
		RECT{352,39,360,57}  // right
	};
	RECT dests[8] = {RECT{7,100,15,108},RECT{7,170,15,178},RECT{140,7,148,15},RECT{212,7,220,15},
		RECT{346,100,354,108},RECT{346,170,354,178},RECT{140,274,148,282},RECT{212,274,220,282}};
	short i;
	
	if ((monsters_going == true) || /*(overall_mode <= MODE_TOWN) ||*/ (overall_mode <= MODE_COMBAT)
		|| (overall_mode == MODE_LOOK_OUTDOORS)) 
			return;
	for (i = 0; i < 4; i++) {
		rect_draw_some_item(terrain_screen_gworld.getTexture(),sources[i],dests[i * 2],ul,sf::BlendAlpha);
		rect_draw_some_item(terrain_screen_gworld.getTexture(),sources[i],dests[i * 2 + 1],ul,sf::BlendAlpha);
		}
}

void redraw_terrain()
{
	RECT to_rect;
	
	to_rect = win_to_rects[0];
	rect_draw_some_item(terrain_screen_gworld.getTexture(), win_from_rects[0], to_rect,ul);


	// Now place arrows
	draw_pointing_arrows();
	

}


void draw_targets(location center)
{
	RECT source_rect = {74,36,85,47},dest_rect;
	short i = 0;

	if (party_toast() == true)
		return;

	for (i = 0; i < 8; i++)
		if ((spell_targets[i].x != 120) && (point_onscreen(center,spell_targets[i]) == true)) {
			dest_rect = coord_to_rect(spell_targets[i].x - center.x + 4,spell_targets[i].y - center.y + 4);
			//OffsetRect(&dest_rect,5,5);
			//InsetRect(&dest_rect,8,12);
			rect_draw_some_item(roads_gworld,calc_rect(6,0),dest_rect,ul,sf::BlendAlpha);
			}
}

void frame_space(location where,short mode,short width,short height)
//mode;  // 0 - red   1 - green
{
	location where_put;
	RECT to_frame;
	
	if (point_onscreen(center,where) == false)
		return;

	where_put.x = 4 + where.x - center.x;
	where_put.y = 4 + where.y - center.y;
	
	to_frame.top = 18 + where_put.y * 36;
	to_frame.left = 18 + where_put.x * 28;
	to_frame.bottom = 54 + where_put.y * 36 + 36 * (height - 1);
	to_frame.right = 46 + where_put.x * 28 + 28 * (width - 1);				
	to_frame.offset(ul);
	
	frame_roundrect(mainPtr, to_frame, 8, (mode == 0) ? sf::Color::Red : sf::Color::Green);
}


void erase_spot(short i,short j)
{
	RECT to_erase;
	
	to_erase = coord_to_rect(i,j);
	fill_rect(mainPtr, to_erase, sf::Color::Black);
	
}

void draw_targeting_line(location where_curs)
{
	location which_space,store_loc;
	short i,j,k,l;
	RECT redraw_rect,redraw_rect2,terrain_rect = {0,0,351,279},target_rect;
	location from_loc;
	RECT on_screen_terrain_area = {23, 23, 346, 274};
	
	if (overall_mode >= MODE_COMBAT)
		from_loc = pc_pos[current_pc];
		else from_loc = univ.town.p_loc;
	if ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING) || (overall_mode == MODE_FANCY_TARGET)
	  || ((overall_mode == MODE_TOWN_TARGET) && (current_pat.pattern[4][4] != 0))) {
		
		on_screen_terrain_area.offset(ul);
		if (where_curs.in(on_screen_terrain_area)) {
		// && (point_onscreen(center,pc_pos[current_pc]) == true)){		
			i = (where_curs.x - 23 - ul.x) / 28;
			j = (where_curs.y - 23 - ul.y) / 36;
			which_space.x = center.x + (short) i - 4;
			which_space.y = center.y + (short) j - 4;
			
			k = (short) (from_loc.x - center.x + 4);
			l = (short) (from_loc.y - center.y + 4);
			k = (k * 28) + 32 + ul.x;
			l = (l * 36) + 36 + ul.y;
			
			if ((can_see(from_loc,which_space,0) < 5) 
			 && (dist(from_loc,which_space) <= current_spell_range)) {
			 	terrain_rect.inset(13,13);
			 	terrain_rect.offset(5 + ul.x,5 + ul.y);
				mainPtr.setActive();
				clip_rect(mainPtr, terrain_rect);
				// TODO: Not sure if this will even work
				sf::ConvexShape line(3);
				line.setPoint(0, where_curs);
				line.setPoint(1, location(k, l));
				line.setPoint(2, location(k - 1, l + 1));
				line.setOutlineThickness(2);
				line.setOutlineColor(sf::Color::Black);
				mainPtr.draw(line, sf::RenderStates(sf::BlendAdd));
#if 0
				PenMode (patXor);
				PenSize(2,2);
				MoveTo(where_curs.h,where_curs.v);
				LineTo(k,l);
#endif
				
				redraw_rect.left = min(where_curs.x,k) - 4;
				redraw_rect.right = max(where_curs.x,k) + 4;
				redraw_rect.top = min(where_curs.y,l) - 4;
				redraw_rect.bottom = max(where_curs.y,l) + 4;
				redraw_rect2 = redraw_rect | terrain_rect;
				
				// Now place targeting pattern
				for (i = 0; i < 9; i++)
					for (j = 0; j < 9; j++) {
						store_loc.x = center.x + i - 4;
						store_loc.y = center.y + j - 4;
						if ((abs(store_loc.x - which_space.x) <= 4) && 
							(abs(store_loc.y - which_space.y) <= 4) && 
							(current_pat.pattern[store_loc.x - which_space.x + 4][store_loc.y - which_space.y + 4] != 0)) {
								target_rect.left = 13 + BITMAP_WIDTH * i + 5 + ul.x;
								target_rect.right = target_rect.left + BITMAP_WIDTH;
								target_rect.top = 13 + BITMAP_HEIGHT * j + 5 + ul.y;
								target_rect.bottom = target_rect.top + BITMAP_HEIGHT;
								// TODO: Not sure if black is the right colour here
								frame_rect(mainPtr, target_rect, sf::Color::Black);
								target_rect.inset(-5,-5);
								target_rect = rectunion(redraw_rect2,redraw_rect2);
								
								// Now place number of shots left, if drawing center of target
								if ((overall_mode == MODE_FANCY_TARGET) && (store_loc.x - which_space.x + 4 == 4)
								 && (store_loc.y - which_space.y + 4 == 4)) {
									TEXT.colour = sf::Color::White;
									const char chr[2] = {static_cast<char>(num_targets_left + '0')};
									int x = ((target_rect.left + target_rect.right) / 2) - 3;
									int y = (target_rect.top + target_rect.bottom) / 2;
									win_draw_string(mainPtr, RECT(y, x, y + 12, x + 12), chr, 1, 12);
									TEXT.colour = sf::Color::Black;
									}
								
								}
						}
							
				mainPtr.display();
				sf::sleep(time_in_ticks(4));
				
				redraw_rect2.inset(-5,-5);
				redraw_partial_terrain(redraw_rect2);
				undo_clip(mainPtr);
				if (is_combat())
					draw_pcs(center,1);
					else draw_party_symbol(0,center);
				if (overall_mode == MODE_FANCY_TARGET)
					draw_targets(center);	
				}
			}
	}
}


bool party_toast()
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1)
			return false;
	return true;
}

void redraw_partial_terrain(RECT redraw_rect)
{
	RECT from_rect;

	from_rect = redraw_rect;
	from_rect.offset(-ul.x,-ul.y);

	// as rect_draw_some_item will shift redraw_rect before drawing, we need to compensate
	redraw_rect.offset(-ul.x + 5,-ul.y + 5);
	
	rect_draw_some_item(terrain_screen_gworld.getTexture(),from_rect,redraw_rect,ul);

}

/*
void HideShowMenuBar( )
{
	GDHandle	mainScreen;
	
	// store current gray region that displays
	// menu bar and the current height of the menu bar
	originalGrayRgn 	= LMGetGrayRgn(); 		
	menuBarHeight	= LMGetMBarHeight(); 	

	// calculate the rect of the menu bar to test 
	// if mouse down is in if desired
	mainScreen 			= GetMainDevice();
	menuBarRect 			= (**mainScreen).gdRect;
	menuBarRect.bottom 	= menuBarHeight;

	HideMenuBar();
		
	while( !Button() );

	ShowMenuBar();
	
	// restore the original gray region to 
	// make the menu bar visible
	LMSetGrayRgn( originalGrayRgn );	
}	

*/