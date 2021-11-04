/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "kshaggy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//KShaggy character structure
enum
{
	KShaggy_ArcMain_Idle0,
	KShaggy_ArcMain_Idle1,
	KShaggy_ArcMain_Idle2,
	KShaggy_ArcMain_Left0,
	KShaggy_ArcMain_Left1,
	KShaggy_ArcMain_Left2,
	KShaggy_ArcMain_Left3,
	KShaggy_ArcMain_Down0,
	KShaggy_ArcMain_Down1,
	KShaggy_ArcMain_Down2,
	KShaggy_ArcMain_Down3,
	KShaggy_ArcMain_Up0,
	KShaggy_ArcMain_Up1,
	KShaggy_ArcMain_Right0,
	KShaggy_ArcMain_Right1,
	KShaggy_ArcMain_Right2,
	KShaggy_ArcMain_Right3,
	KShaggy_ArcMain_Right4,
	
	KShaggy_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[KShaggy_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_KShaggy;

//KShaggy character definitions
static const CharFrame char_kshaggy_frame[] = {
	{KShaggy_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{KShaggy_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{KShaggy_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	
	{KShaggy_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //3 left 4
	{KShaggy_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //4 left 5
	{KShaggy_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //5 left 6
	{KShaggy_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 250}}, //6 left 7
	
	{KShaggy_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //7 down 8
	{KShaggy_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //8 down 9
	{KShaggy_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //9 down 10
	{KShaggy_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 250}}, //10 down 11
	
	{KShaggy_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //11 up 12
	{KShaggy_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //12 up 13
	
	{KShaggy_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //13 right 14
	{KShaggy_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //14 right 15
	{KShaggy_ArcMain_Right2, {  0,   0, 256, 256}, { 125, 250}}, //15 right 16
	{KShaggy_ArcMain_Right3, {  0,   0, 256, 256}, { 125, 250}}, //16 right 17
	{KShaggy_ArcMain_Right4, {  0,   0, 256, 256}, { 125, 250}}, //17 right 18
};

static const Animation char_kshaggy_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2, ASCR_REPEAT, 1}},      	   		   //CharAnim_Idle
	{2, (const u8[]){ 3,  4,  5,  6,  4,  3, ASCR_BACK, 1}},       		   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAltB
	{2, (const u8[]){ 7,  8,  9,  10,  9,  8, ASCR_BACK, 1}},  		   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	   //CharAnim_DownAltB
	{2, (const u8[]){ 11,  12,  11, ASCR_BACK, 1}},                		   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_UpAltB
	{2, (const u8[]){ 13,  14,  15,  16,  17,  16, 15, 14, ASCR_BACK, 1}},     //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                             //CharAnim_RightAltB
};

//KShaggy character functions
void Char_KShaggy_SetFrame(void *user, u8 frame)
{
	Char_KShaggy *this = (Char_KShaggy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_kshaggy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_KShaggy_Tick(Character *character)
{
	Char_KShaggy *this = (Char_KShaggy*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_KShaggy_SetFrame);
	Character_Draw(character, &this->tex, &char_kshaggy_frame[this->frame]);
}

void Char_KShaggy_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_KShaggy_Free(Character *character)
{
	Char_KShaggy *this = (Char_KShaggy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_KShaggy_New(fixed_t x, fixed_t y)
{
	//Allocate kshaggy object
	Char_KShaggy *this = Mem_Alloc(sizeof(Char_KShaggy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_KShaggy_New] Failed to allocate kshaggy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_KShaggy_Tick;
	this->character.set_anim = Char_KShaggy_SetAnim;
	this->character.free = Char_KShaggy_Free;
	
	Animatable_Init(&this->character.animatable, char_kshaggy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 10;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\KSHAGGY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //KShaggy_ArcMain_Idle0
		"idle1.tim", //KShaggy_ArcMain_Idle1
		"idle2.tim", //KShaggy_ArcMain_Idle2
		"left0.tim",  //KShaggy_ArcMain_Left0
		"left1.tim",  //KShaggy_ArcMain_Left1
		"left2.tim",  //KShaggy_ArcMain_Left2
		"left3.tim",  //KShaggy_ArcMain_Left3
		"down0.tim",  //KShaggy_ArcMain_Down0
		"down1.tim",  //KShaggy_ArcMain_Down1
		"down2.tim",  //KShaggy_ArcMain_Down2
		"down3.tim",  //KShaggy_ArcMain_Down3
		"up0.tim",    //KShaggy_ArcMain_Up0
		"up1.tim",    //KShaggy_ArcMain_Up1
		"right0.tim", //KShaggy_ArcMain_Right0
		"right1.tim", //KShaggy_ArcMain_Right1
		"right2.tim", //KShaggy_ArcMain_Right2
		"right3.tim", //KShaggy_ArcMain_Right3
		"right4.tim", //KShaggy_ArcMain_Right4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
