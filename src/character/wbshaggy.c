/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "wbshaggy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//WBShaggy character structure
enum
{
	WBShaggy_ArcMain_Idle0,
	WBShaggy_ArcMain_Idle1,
	WBShaggy_ArcMain_Idle2,
	WBShaggy_ArcMain_Idle3,
	WBShaggy_ArcMain_Idle4,
	WBShaggy_ArcMain_Idle5,
	WBShaggy_ArcMain_Idle6,
	WBShaggy_ArcMain_Left0,
	WBShaggy_ArcMain_Left1,
	WBShaggy_ArcMain_Left2,
	WBShaggy_ArcMain_Left3,
	WBShaggy_ArcMain_Down0,
	WBShaggy_ArcMain_Down1,
	WBShaggy_ArcMain_Down2,
	WBShaggy_ArcMain_Down3,
	WBShaggy_ArcMain_Up0,
	WBShaggy_ArcMain_Up1,
	WBShaggy_ArcMain_Right0,
	WBShaggy_ArcMain_Right1,
	WBShaggy_ArcMain_Right2,
	WBShaggy_ArcMain_Right3,
	WBShaggy_ArcMain_Right4,
	
	WBShaggy_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[WBShaggy_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_WBShaggy;

//WBShaggy character definitions
static const CharFrame char_wbshaggy_frame[] = {
	{WBShaggy_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{WBShaggy_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{WBShaggy_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{WBShaggy_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{WBShaggy_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{WBShaggy_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	{WBShaggy_ArcMain_Idle6, {  0,   0, 256, 256}, { 125, 250}}, //6 idle 7
	
	{WBShaggy_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //7 left 8
	{WBShaggy_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //8 left 9
	{WBShaggy_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //9 left 10
	{WBShaggy_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 250}}, //10 left 11
	
	{WBShaggy_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //11 down 12
	{WBShaggy_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //12 down 13
	{WBShaggy_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //13 down 14
	{WBShaggy_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 250}}, //14 down 15
	
	{WBShaggy_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //15 up 16
	{WBShaggy_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //16 up 17
	
	{WBShaggy_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //17 right 18
	{WBShaggy_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //18 right 19
	{WBShaggy_ArcMain_Right2, {  0,   0, 256, 256}, { 125, 250}}, //19 right 20
	{WBShaggy_ArcMain_Right3, {  0,   0, 256, 256}, { 125, 250}}, //20 right 21
	{WBShaggy_ArcMain_Right4, {  0,   0, 256, 256}, { 125, 250}}, //21 right 22
};

static const Animation char_wbshaggy_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  5,  4,  3,  2,  1, ASCR_REPEAT, 1}},      	   //CharAnim_Idle
	{2, (const u8[]){ 7,  8,  9, 10,  9,  8, ASCR_BACK, 1}},       		   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAltB
	{2, (const u8[]){ 11, 12,  13,  14,  13,  12, ASCR_BACK, 1}},  		   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_DownAltB
	{2, (const u8[]){ 15,  16,  15, ASCR_BACK, 1}},                		   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_UpAltB
	{2, (const u8[]){ 17,  18,  19,  20,  21, 20, 19, 18, 17, ASCR_BACK, 1}}, //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_RightAltB
};

//WBShaggy character functions
void Char_WBShaggy_SetFrame(void *user, u8 frame)
{
	Char_WBShaggy *this = (Char_WBShaggy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_wbshaggy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_WBShaggy_Tick(Character *character)
{
	Char_WBShaggy *this = (Char_WBShaggy*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_WBShaggy_SetFrame);
	Character_Draw(character, &this->tex, &char_wbshaggy_frame[this->frame]);
}

void Char_WBShaggy_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_WBShaggy_Free(Character *character)
{
	Char_WBShaggy *this = (Char_WBShaggy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_WBShaggy_New(fixed_t x, fixed_t y)
{
	//Allocate wbshaggy object
	Char_WBShaggy *this = Mem_Alloc(sizeof(Char_WBShaggy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_WBShaggy_New] Failed to allocate wbshaggy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_WBShaggy_Tick;
	this->character.set_anim = Char_WBShaggy_SetAnim;
	this->character.free = Char_WBShaggy_Free;
	
	Animatable_Init(&this->character.animatable, char_wbshaggy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 6;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-125,1);
	this->character.focus_zoom = FIXED_DEC(5,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\WBSHAGGY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //WBShaggy_ArcMain_Idle0
		"idle1.tim", //WBShaggy_ArcMain_Idle1
		"idle2.tim", //WBShaggy_ArcMain_Idle2
		"idle3.tim", //WBShaggy_ArcMain_Idle3
		"idle4.tim", //WBShaggy_ArcMain_Idle4
		"idle5.tim", //WBShaggy_ArcMain_Idle5
		"idle6.tim", //WBShaggy_ArcMain_Idle6
		"left0.tim",  //WBShaggy_ArcMain_Left0
		"left1.tim",  //WBShaggy_ArcMain_Left1
		"left2.tim",  //WBShaggy_ArcMain_Left2
		"left3.tim",  //WBShaggy_ArcMain_Left3
		"down0.tim",  //WBShaggy_ArcMain_Down0
		"down1.tim",  //WBShaggy_ArcMain_Down1
		"down2.tim",  //WBShaggy_ArcMain_Down2
		"down3.tim",  //WBShaggy_ArcMain_Down3
		"up0.tim",    //WBShaggy_ArcMain_Up0
		"up1.tim",    //WBShaggy_ArcMain_Up1
		"right0.tim", //WBShaggy_ArcMain_Right0
		"right1.tim", //WBShaggy_ArcMain_Right1
		"right2.tim", //WBShaggy_ArcMain_Right2
		"right3.tim", //WBShaggy_ArcMain_Right3
		"right4.tim", //WBShaggy_ArcMain_Right4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
