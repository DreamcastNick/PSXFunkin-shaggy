/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfmatt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BFMatt character structure
enum
{
	BFMatt_ArcMain_Idle0,
	BFMatt_ArcMain_Idle1,
	BFMatt_ArcMain_Idle2,
	BFMatt_ArcMain_Idle3,
	BFMatt_ArcMain_Idle4,
	BFMatt_ArcMain_Idle5,
	BFMatt_ArcMain_Left0,
	BFMatt_ArcMain_Left1,
	BFMatt_ArcMain_Down0,
	BFMatt_ArcMain_Down1,
	BFMatt_ArcMain_Up0,
	BFMatt_ArcMain_Up1,
	BFMatt_ArcMain_Right0,
	BFMatt_ArcMain_Right1,
	
	BFMatt_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BFMatt_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BFMatt;

//BFMatt character definitions
static const CharFrame char_bfmatt_frame[] = {
	{BFMatt_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{BFMatt_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{BFMatt_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{BFMatt_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{BFMatt_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{BFMatt_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	
	{BFMatt_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //6 left 7
	{BFMatt_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //7 left 8
	
	{BFMatt_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //8 down 9
	{BFMatt_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //9 down 10
	
	{BFMatt_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //10 up 11
	{BFMatt_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //11 up 12
	
	{BFMatt_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //11 right 13
	{BFMatt_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //13 right 14
};

static const Animation char_bfmatt_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  5, ASCR_REPEAT, 1}},      	   //CharAnim_Idle
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},       		   		   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAltB
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},  		   		   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_DownAltB
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},                		   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_UpAltB
	{2, (const u8[]){ 12, 13, ASCR_BACK, 1}},                                  //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_RightAltB
};

//BFMatt character functions
void Char_BFMatt_SetFrame(void *user, u8 frame)
{
	Char_BFMatt *this = (Char_BFMatt*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bfmatt_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BFMatt_Tick(Character *character)
{
	Char_BFMatt *this = (Char_BFMatt*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BFMatt_SetFrame);
	Character_Draw(character, &this->tex, &char_bfmatt_frame[this->frame]);
}

void Char_BFMatt_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFMatt_Free(Character *character)
{
	Char_BFMatt *this = (Char_BFMatt*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BFMatt_New(fixed_t x, fixed_t y)
{
	//Allocate bfmatt object
	Char_BFMatt *this = Mem_Alloc(sizeof(Char_BFMatt));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFMatt_New] Failed to allocate bfmatt object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFMatt_Tick;
	this->character.set_anim = Char_BFMatt_SetAnim;
	this->character.free = Char_BFMatt_Free;
	
	Animatable_Init(&this->character.animatable, char_bfmatt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 2;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFMATT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //BFMatt_ArcMain_Idle0
		"idle1.tim", //BFMatt_ArcMain_Idle1
		"idle2.tim", //BFMatt_ArcMain_Idle2
		"idle3.tim", //BFMatt_ArcMain_Idle3
		"idle4.tim", //BFMatt_ArcMain_Idle4
		"idle5.tim", //BFMatt_ArcMain_Idle5
		"left0.tim",  //BFMatt_ArcMain_Left0
		"left1.tim",  //BFMatt_ArcMain_Left1
		"down0.tim",  //BFMatt_ArcMain_Down0
		"down1.tim",  //BFMatt_ArcMain_Down1
		"up0.tim",    //BFMatt_ArcMain_Up0
		"up1.tim",    //BFMatt_ArcMain_Up1
		"right0.tim", //BFMatt_ArcMain_Right0
		"right1.tim", //BFMatt_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
