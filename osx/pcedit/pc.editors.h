

bool give_to_pc(short pc_num,cItemRec  item, short print_result);
bool give_to_party(cItemRec item,short print_result);
void give_gold(short amount,bool print_result);
bool take_gold(short amount,bool print_result);
short pc_has_space(short pc_num);
void take_item(short pc_num,short which_item);
short char_select_pc(short active_only,short free_inv_only,char *title);
short select_pc(short active_only,short free_inv_only);
void give_spec_items();
void pick_race_abil(cPlayer *pc,short mode,class cDialog* parent_num);
void reset_boats();
void combine_things(short pc_num);