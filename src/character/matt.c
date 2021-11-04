/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "matt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Matt character structure
enum
{
	Matt_ArcMain_Idle0,
	Matt_ArcMain_Idle1,
	Matt_ArcMain_Idle2,
	Matt_ArcMain_Idle3,
	Matt_ArcMain_Idle4,
	Matt_ArcMain_Idle5,
	Matt_ArcMain_Left0,
	Matt_ArcMain_Left1,
	Matt_ArcMain_Down0,
	Matt_ArcMain_Down1,
	Matt_ArcMain_Up0,
	Matt_ArcMain_Up1,
	Matt_ArcMain_Right0,
	Matt_ArcMain_Right1,
	
	Matt_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Matt_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Matt;

//Matt character definitions
static const CharFrame char_matt_frame[] = {
	{Matt_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Matt_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Matt_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Matt_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Matt_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Matt_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	
	{Matt_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //6 left 7
	{Matt_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //7 left 8
	
	{Matt_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //8 down 9
	{Matt_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //9 down 10
	
	{Matt_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //10 up 11
	{Matt_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //11 up 12
	
	{Matt_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //11 right 13
	{Matt_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //13 right 14
};

static const Animation char_matt_anim[CharAnim_Max] = {
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

//Matt character functions
void Char_Matt_SetFrame(void *user, u8 frame)
{
	Char_Matt *this = (Char_Matt*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_matt_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Matt_Tick(Character *character)
{
	Char_Matt *this = (Char_Matt*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Matt_SetFrame);
	Character_Draw(character, &this->tex, &char_matt_frame[this->frame]);
}

void Char_Matt_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Matt_Free(Character *character)
{
	Char_Matt *this = (Char_Matt*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Matt_New(fixed_t x, fixed_t y)
{
	//Allocate matt object
	Char_Matt *this = Mem_Alloc(sizeof(Char_Matt));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Matt_New] Failed to allocate matt object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Matt_Tick;
	this->character.set_anim = Char_Matt_SetAnim;
	this->character.free = Char_Matt_Free;
	
	Animatable_Init(&this->character.animatable, char_matt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 2;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MATT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Matt_ArcMain_Idle0
		"idle1.tim", //Matt_ArcMain_Idle1
		"idle2.tim", //Matt_ArcMain_Idle2
		"idle3.tim", //Matt_ArcMain_Idle3
		"idle4.tim", //Matt_ArcMain_Idle4
		"idle5.tim", //Matt_ArcMain_Idle5
		"left0.tim",  //Matt_ArcMain_Left0
		"left1.tim",  //Matt_ArcMain_Left1
		"down0.tim",  //Matt_ArcMain_Down0
		"down1.tim",  //Matt_ArcMain_Down1
		"up0.tim",    //Matt_ArcMain_Up0
		"up1.tim",    //Matt_ArcMain_Up1
		"right0.tim", //Matt_ArcMain_Right0
		"right1.tim", //Matt_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
