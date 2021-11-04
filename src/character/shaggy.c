/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "shaggy.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Shaggy character structure
enum
{
	Shaggy_ArcMain_Idle0,
	Shaggy_ArcMain_Idle1,
	Shaggy_ArcMain_Idle2,
	Shaggy_ArcMain_Idle3,
	Shaggy_ArcMain_Idle4,
	Shaggy_ArcMain_Idle5,
	Shaggy_ArcMain_Idle6,
	Shaggy_ArcMain_Left0,
	Shaggy_ArcMain_Left1,
	Shaggy_ArcMain_Left2,
	Shaggy_ArcMain_Left3,
	Shaggy_ArcMain_Down0,
	Shaggy_ArcMain_Down1,
	Shaggy_ArcMain_Down2,
	Shaggy_ArcMain_Down3,
	Shaggy_ArcMain_Up0,
	Shaggy_ArcMain_Up1,
	Shaggy_ArcMain_Right0,
	Shaggy_ArcMain_Right1,
	Shaggy_ArcMain_Right2,
	Shaggy_ArcMain_Right3,
	Shaggy_ArcMain_Right4,
	
	Shaggy_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Shaggy_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Shaggy;

//Shaggy character definitions
static const CharFrame char_shaggy_frame[] = {
	{Shaggy_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Shaggy_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Shaggy_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Shaggy_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Shaggy_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Shaggy_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	{Shaggy_ArcMain_Idle6, {  0,   0, 256, 256}, { 125, 250}}, //6 idle 7
	
	{Shaggy_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //7 left 8
	{Shaggy_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //8 left 9
	{Shaggy_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //9 left 10
	{Shaggy_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 250}}, //10 left 11
	
	{Shaggy_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //11 down 12
	{Shaggy_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //12 down 13
	{Shaggy_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //13 down 14
	{Shaggy_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 250}}, //14 down 15
	
	{Shaggy_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //15 up 16
	{Shaggy_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //16 up 17
	
	{Shaggy_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //17 right 18
	{Shaggy_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //18 right 19
	{Shaggy_ArcMain_Right2, {  0,   0, 256, 256}, { 125, 250}}, //19 right 20
	{Shaggy_ArcMain_Right3, {  0,   0, 256, 256}, { 125, 250}}, //20 right 21
	{Shaggy_ArcMain_Right4, {  0,   0, 256, 256}, { 125, 250}}, //21 right 22
};

static const Animation char_shaggy_anim[CharAnim_Max] = {
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

//Shaggy character functions
void Char_Shaggy_SetFrame(void *user, u8 frame)
{
	Char_Shaggy *this = (Char_Shaggy*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_shaggy_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Shaggy_Tick(Character *character)
{
	Char_Shaggy *this = (Char_Shaggy*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Shaggy_SetFrame);
	Character_Draw(character, &this->tex, &char_shaggy_frame[this->frame]);
}

void Char_Shaggy_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Shaggy_Free(Character *character)
{
	Char_Shaggy *this = (Char_Shaggy*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Shaggy_New(fixed_t x, fixed_t y)
{
	//Allocate shaggy object
	Char_Shaggy *this = Mem_Alloc(sizeof(Char_Shaggy));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Shaggy_New] Failed to allocate shaggy object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Shaggy_Tick;
	this->character.set_anim = Char_Shaggy_SetAnim;
	this->character.free = Char_Shaggy_Free;
	
	Animatable_Init(&this->character.animatable, char_shaggy_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 11;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SHAGGY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Shaggy_ArcMain_Idle0
		"idle1.tim", //Shaggy_ArcMain_Idle1
		"idle2.tim", //Shaggy_ArcMain_Idle2
		"idle3.tim", //Shaggy_ArcMain_Idle3
		"idle4.tim", //Shaggy_ArcMain_Idle4
		"idle5.tim", //Shaggy_ArcMain_Idle5
		"idle6.tim", //Shaggy_ArcMain_Idle6
		"left0.tim",  //Shaggy_ArcMain_Left0
		"left1.tim",  //Shaggy_ArcMain_Left1
		"left2.tim",  //Shaggy_ArcMain_Left2
		"left3.tim",  //Shaggy_ArcMain_Left3
		"down0.tim",  //Shaggy_ArcMain_Down0
		"down1.tim",  //Shaggy_ArcMain_Down1
		"down2.tim",  //Shaggy_ArcMain_Down2
		"down3.tim",  //Shaggy_ArcMain_Down3
		"up0.tim",    //Shaggy_ArcMain_Up0
		"up1.tim",    //Shaggy_ArcMain_Up1
		"right0.tim", //Shaggy_ArcMain_Right0
		"right1.tim", //Shaggy_ArcMain_Right1
		"right2.tim", //Shaggy_ArcMain_Right2
		"right3.tim", //Shaggy_ArcMain_Right3
		"right4.tim", //Shaggy_ArcMain_Right4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
