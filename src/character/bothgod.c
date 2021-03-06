/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bothgod.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BothGod character structure
enum
{
	BothGod_ArcMain_Idle0,
	BothGod_ArcMain_Idle1,
	BothGod_ArcMain_Idle2,
	BothGod_ArcMain_Idle3,
	BothGod_ArcMain_Idle4,
	BothGod_ArcMain_Idle5,
	BothGod_ArcMain_Idle6,
	BothGod_ArcMain_Left0,
	BothGod_ArcMain_Left1,
	BothGod_ArcMain_LeftA0,
	BothGod_ArcMain_LeftA1,
	BothGod_ArcMain_LeftA2,
	BothGod_ArcMain_LeftA3,
	BothGod_ArcMain_LeftB0,
	BothGod_ArcMain_LeftB1,
	BothGod_ArcMain_Down0,
	BothGod_ArcMain_Down1,
	BothGod_ArcMain_DownA0,
	BothGod_ArcMain_DownA1,
	BothGod_ArcMain_DownA2,
	BothGod_ArcMain_DownA3,
	BothGod_ArcMain_DownB0,
	BothGod_ArcMain_DownB1,
	BothGod_ArcMain_Up0,
	BothGod_ArcMain_Up1,
	BothGod_ArcMain_UpA0,
	BothGod_ArcMain_UpA1,
	BothGod_ArcMain_UpB0,
	BothGod_ArcMain_UpB1,
	BothGod_ArcMain_Right0,
	BothGod_ArcMain_Right1,
	BothGod_ArcMain_RightA0,
	BothGod_ArcMain_RightA1,
	BothGod_ArcMain_RightA2,
	BothGod_ArcMain_RightA3,
	BothGod_ArcMain_RightA4,
	BothGod_ArcMain_RightB0,
	BothGod_ArcMain_RightB1,
	BothGod_ArcMain_RightB2,
	
	BothGod_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BothGod_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BothGod;

//BothGod character definitions
static const CharFrame char_bothgod_frame[] = {
	{BothGod_ArcMain_Idle0,  {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{BothGod_ArcMain_Idle1,  {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{BothGod_ArcMain_Idle2,  {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{BothGod_ArcMain_Idle3,  {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{BothGod_ArcMain_Idle4,  {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{BothGod_ArcMain_Idle5,  {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	{BothGod_ArcMain_Idle6,  {  0,   0, 256, 256}, { 125, 250}}, //6 idle 7
	
	{BothGod_ArcMain_Left0,  {  0,   0, 256, 256}, { 125, 250}}, //7 left 8
	{BothGod_ArcMain_Left1,  {  0,   0, 256, 256}, { 125, 250}}, //8 left 9

	{BothGod_ArcMain_LeftA0, {  0,   0, 256, 256}, { 125, 250}}, //9 left 10
	{BothGod_ArcMain_LeftA1, {  0,   0, 256, 256}, { 125, 250}}, //10 left 11
	{BothGod_ArcMain_LeftA2, {  0,   0, 256, 256}, { 125, 250}}, //11 left 12
	{BothGod_ArcMain_LeftA3, {  0,   0, 256, 256}, { 125, 250}}, //12 left 13

	{BothGod_ArcMain_LeftB0, {  0,   0, 256, 256}, { 125, 250}}, //13 left 14
	{BothGod_ArcMain_LeftB1, {  0,   0, 256, 256}, { 125, 250}}, //14 left 15
	
	{BothGod_ArcMain_Down0,  {  0,   0, 256, 256}, { 125, 250}}, //15 down 16
	{BothGod_ArcMain_Down1,  {  0,   0, 256, 256}, { 125, 250}}, //16 down 17
	
	{BothGod_ArcMain_DownA0, {  0,   0, 256, 256}, { 125, 250}}, //17 down 18
	{BothGod_ArcMain_DownA1, {  0,   0, 256, 256}, { 125, 250}}, //18 down 19
	{BothGod_ArcMain_DownA2, {  0,   0, 256, 256}, { 125, 250}}, //19 down 20
	{BothGod_ArcMain_DownA3, {  0,   0, 256, 256}, { 125, 250}}, //20 down 21
	
	{BothGod_ArcMain_DownB0, {  0,   0, 256, 256}, { 125, 250}}, //21 down 22
	{BothGod_ArcMain_DownB1, {  0,   0, 256, 256}, { 125, 250}}, //22 down 23
	
	{BothGod_ArcMain_Up0,    {   0,   0,  256, 256}, { 125, 250}}, //23 up 24
	{BothGod_ArcMain_Up1,    {   0,   0,  256, 256}, { 125, 250}}, //24 up 25

	{BothGod_ArcMain_UpA0,   {   0,   0,  256, 256}, { 125, 250}}, //25 up 26
	{BothGod_ArcMain_UpA1,   {   0,   0,  256, 256}, { 125, 250}}, //26 up 27

	{BothGod_ArcMain_UpB0,   {   0,   0,  256, 256}, { 125, 250}}, //27 up 28
	{BothGod_ArcMain_UpB1,   {   0,   0,  256, 256}, { 125, 250}}, //28 up 29
	
	{BothGod_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //29 right 30
	{BothGod_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //30 right 31

	{BothGod_ArcMain_RightA0, {  0,   0, 256, 256}, { 125, 250}}, //31 right 32
	{BothGod_ArcMain_RightA1, {  0,   0, 256, 256}, { 125, 250}}, //32 right 33
	{BothGod_ArcMain_RightA2, {  0,   0, 256, 256}, { 125, 250}}, //33 right 34
	{BothGod_ArcMain_RightA3, {  0,   0, 256, 256}, { 125, 250}}, //34 right 35
	{BothGod_ArcMain_RightA4, {  0,   0, 256, 256}, { 125, 250}}, //35 right 36

	{BothGod_ArcMain_RightB0, {  0,   0, 256, 256}, { 125, 250}}, //36 right 37
	{BothGod_ArcMain_RightB1, {  0,   0, 256, 256}, { 125, 250}}, //37 right 38
	{BothGod_ArcMain_RightB2, {  0,   0, 256, 256}, { 125, 250}}, //38 right 39
};

static const Animation char_bothgod_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  5,  4,  3,  2,  1, ASCR_REPEAT, 1}},	//CharAnim_Idle
	{1, (const u8[]){ 7,  8, ASCR_BACK, 1}},       		   				//CharAnim_Left
	{1, (const u8[]){ 9,  10,  11,  12, ASCR_BACK, 1}},                     	   	//CharAnim_LeftAlt
	{1, (const u8[]){ 13, 14, ASCR_BACK, 1}},                     	   	//CharAnim_LeftAltB
	{1, (const u8[]){ 15, 16, ASCR_BACK, 1}},  		   				//CharAnim_Down
	{1, (const u8[]){ 17, 18, 19, 20, ASCR_BACK, 1}},   	          	   		//CharAnim_DownAlt
	{1, (const u8[]){ 21, 22, ASCR_BACK, 1}},   	          	   		//CharAnim_DownAltB
	{1, (const u8[]){ 23, 24, ASCR_BACK, 1}},                		   		//CharAnim_Up
	{1, (const u8[]){ 25, 26, ASCR_BACK, 1}},                             			//CharAnim_UpAlt
	{1, (const u8[]){ 27, 28, ASCR_BACK, 1}},   	          	   		        //CharAnim_UpAltB
	{1, (const u8[]){ 29, 30, ASCR_BACK, 1}},  						//CharAnim_Right
	{1, (const u8[]){ 31, 32, 33, 34, 35, ASCR_BACK, 1}},                            //CharAnim_RightAlt
	{1, (const u8[]){ 36, 37, 38, ASCR_BACK, 1}},                             	//CharAnim_RightAltB
};

//BothGod character functions
void Char_BothGod_SetFrame(void *user, u8 frame)
{
	Char_BothGod *this = (Char_BothGod*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bothgod_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BothGod_Tick(Character *character)
{
	Char_BothGod *this = (Char_BothGod*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BothGod_SetFrame);
	Character_Draw(character, &this->tex, &char_bothgod_frame[this->frame]);
}

void Char_BothGod_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BothGod_Free(Character *character)
{
	Char_BothGod *this = (Char_BothGod*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BothGod_New(fixed_t x, fixed_t y)
{
	//Allocate bothgod object
	Char_BothGod *this = Mem_Alloc(sizeof(Char_BothGod));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BothGod_New] Failed to allocate bothgod object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BothGod_Tick;
	this->character.set_anim = Char_BothGod_SetAnim;
	this->character.free = Char_BothGod_Free;
	
	Animatable_Init(&this->character.animatable, char_bothgod_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.spec = CHAR_SPEC_SWAPANIM;


	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BOTHGOD.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",   //BothGod_ArcMain_Idle0
		"idle1.tim",   //BothGod_ArcMain_Idle1
		"idle2.tim",   //BothGod_ArcMain_Idle2
		"idle3.tim",   //BothGod_ArcMain_Idle3
		"idle4.tim",   //BothGod_ArcMain_Idle4
		"idle5.tim",   //BothGod_ArcMain_Idle5
		"idle6.tim",   //BothGod_ArcMain_Idle6
		"left0.tim",   //BothGod_ArcMain_Left0
		"left1.tim",   //BothGod_ArcMain_Left1
		"lefta0.tim",  //BothGod_ArcMain_LeftA0
		"lefta1.tim",  //BothGod_ArcMain_LeftA1
		"lefta2.tim",  //BothGod_ArcMain_LeftA2
		"lefta3.tim",  //BothGod_ArcMain_LeftA3
		"leftb0.tim",  //BothGod_ArcMain_LeftB0
		"leftb1.tim",  //BothGod_ArcMain_LeftB1
		"down0.tim",   //BothGod_ArcMain_Down0
		"down1.tim",   //BothGod_ArcMain_Down1
		"downa0.tim",  //BothGod_ArcMain_DownA0
		"downa1.tim",  //BothGod_ArcMain_DownA1
		"downa2.tim",  //BothGod_ArcMain_DownA2
		"downa3.tim",  //BothGod_ArcMain_DownA3
		"downb0.tim",  //BothGod_ArcMain_DownB0
		"downb1.tim",  //BothGod_ArcMain_DownB1
		"up0.tim",     //BothGod_ArcMain_Up0
		"up1.tim",     //BothGod_ArcMain_Up1
		"upa0.tim",    //BothGod_ArcMain_UpA0
		"upa1.tim",    //BothGod_ArcMain_UpA1
		"upb0.tim",    //BothGod_ArcMain_UpB0
		"upb1.tim",    //BothGod_ArcMain_UpB1
		"right0.tim",  //BothGod_ArcMain_Right0
		"right1.tim",  //BothGod_ArcMain_Right1
		"righta0.tim", //BothGod_ArcMain_RightA0
		"righta1.tim", //BothGod_ArcMain_RightA1
		"righta2.tim", //BothGod_ArcMain_RightA2
		"righta3.tim", //BothGod_ArcMain_RightA3
		"righta4.tim", //BothGod_ArcMain_RightA4
		"rightb0.tim", //BothGod_ArcMain_RightB0
		"rightb1.tim", //BothGod_ArcMain_RightB1
		"rightb2.tim", //BothGod_ArcMain_RightB2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
