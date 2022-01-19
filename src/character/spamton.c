/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "spamton.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Spamton character structure
enum
{
	Spamton_ArcMain_Idle0,
	Spamton_ArcMain_Idle1,
	Spamton_ArcMain_Idle2,
	Spamton_ArcMain_Idle3,
	Spamton_ArcMain_Idle4,
	Spamton_ArcMain_Idle5,
	Spamton_ArcMain_Idle6,
	Spamton_ArcMain_Idle7,
	Spamton_ArcMain_Idle8,
	Spamton_ArcMain_Left0,
	Spamton_ArcMain_Left1,
	Spamton_ArcMain_Left2,
	Spamton_ArcMain_Left3,
	Spamton_ArcMain_Down0,
	Spamton_ArcMain_Down1,
	Spamton_ArcMain_Down2,
	Spamton_ArcMain_Down3,
	Spamton_ArcMain_Up0,
	Spamton_ArcMain_Up1,
	Spamton_ArcMain_Right0,
	Spamton_ArcMain_Right1,
	Spamton_ArcMain_Right2,
	Spamton_ArcMain_Right3,
	Spamton_ArcMain_Right4,
	
	Spamton_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Spamton_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Spamton;

//Spamton character definitions
static const CharFrame char_spamton_frame[] = {
	{Spamton_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Spamton_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Spamton_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Spamton_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Spamton_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Spamton_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	{Spamton_ArcMain_Idle6, {  0,   0, 256, 256}, { 125, 250}}, //6 idle 7
	{Spamton_ArcMain_Idle7, {  0,   0, 256, 256}, { 125, 250}}, //7 idle 8
	{Spamton_ArcMain_Idle8, {  0,   0, 256, 256}, { 125, 250}}, //8 idle 9
	
	{Spamton_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //9 left 1
	{Spamton_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //10 left 2
	{Spamton_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //11 left 3
	{Spamton_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 250}}, //12 left 4
	
	{Spamton_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //13 down 1
	{Spamton_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //14 down 2
	{Spamton_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //15 down 3
	{Spamton_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 250}}, //16 down 4
	
	{Spamton_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //17 up 1
	{Spamton_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //18 up 2
	
	{Spamton_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //19 right 1
	{Spamton_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //20 right 2
	{Spamton_ArcMain_Right2, {  0,   0, 256, 256}, { 125, 250}}, //21 right 3
	{Spamton_ArcMain_Right3, {  0,   0, 256, 256}, { 125, 250}}, //22 right 4
	{Spamton_ArcMain_Right4, {  0,   0, 256, 256}, { 125, 250}}, //23 right 5
};

static const Animation char_spamton_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7,  8, ASCR_REPEAT}},    //CharAnim_Idle
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},       		   		   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                     	   //CharAnim_LeftAltB
	{2, (const u8[]){13, 14, 15, 16, ASCR_BACK, 1}},  		   			   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	       //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	       //CharAnim_DownAltB
	{2, (const u8[]){17, 18, 17, ASCR_BACK, 1}},                		   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                         //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   	          	       //CharAnim_UpAltB
	{2, (const u8[]){19, 20, 21, 22, 23, ASCR_BACK, 1}}, 			   	   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                         //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},                         //CharAnim_RightAltB
};

//Spamton character functions
void Char_Spamton_SetFrame(void *user, u8 frame)
{
	Char_Spamton *this = (Char_Spamton*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_spamton_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Spamton_Tick(Character *character)
{
	Char_Spamton *this = (Char_Spamton*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Spamton_SetFrame);
	Character_Draw(character, &this->tex, &char_spamton_frame[this->frame]);
}

void Char_Spamton_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Spamton_Free(Character *character)
{
	Char_Spamton *this = (Char_Spamton*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Spamton_New(fixed_t x, fixed_t y)
{
	//Allocate spamton object
	Char_Spamton *this = Mem_Alloc(sizeof(Char_Spamton));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Spamton_New] Failed to allocate spamton object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Spamton_Tick;
	this->character.set_anim = Char_Spamton_SetAnim;
	this->character.free = Char_Spamton_Free;
	
	Animatable_Init(&this->character.animatable, char_spamton_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 7;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-125,1);
	this->character.focus_zoom = FIXED_DEC(5,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPAMTON.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Spamton_ArcMain_Idle0
		"idle1.tim", //Spamton_ArcMain_Idle1
		"idle2.tim", //Spamton_ArcMain_Idle2
		"idle3.tim", //Spamton_ArcMain_Idle3
		"idle4.tim", //Spamton_ArcMain_Idle4
		"idle5.tim", //Spamton_ArcMain_Idle5
		"idle6.tim", //Spamton_ArcMain_Idle6
		"idle7.tim", //Spamton_ArcMain_Idle7
		"idle8.tim", //Spamton_ArcMain_Idle8
		"left0.tim",  //Spamton_ArcMain_Left0
		"left1.tim",  //Spamton_ArcMain_Left1
		"left2.tim",  //Spamton_ArcMain_Left2
		"left3.tim",  //Spamton_ArcMain_Left3
		"down0.tim",  //Spamton_ArcMain_Down0
		"down1.tim",  //Spamton_ArcMain_Down1
		"down2.tim",  //Spamton_ArcMain_Down2
		"down3.tim",  //Spamton_ArcMain_Down3
		"up0.tim",    //Spamton_ArcMain_Up0
		"up1.tim",    //Spamton_ArcMain_Up1
		"right0.tim", //Spamton_ArcMain_Right0
		"right1.tim", //Spamton_ArcMain_Right1
		"right2.tim", //Spamton_ArcMain_Right2
		"right3.tim", //Spamton_ArcMain_Right3
		"right4.tim", //Spamton_ArcMain_Right4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
