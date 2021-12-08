/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bfmatt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend skull fragments
static SkullFragment char_bfmatt_skull[15] = {
	{ 1 * 8, -87 * 8, -13, -13},
	{ 9 * 8, -88 * 8,   5, -22},
	{18 * 8, -87 * 8,   9, -22},
	{26 * 8, -85 * 8,  13, -13},
	
	{-3 * 8, -82 * 8, -13, -11},
	{ 8 * 8, -85 * 8,  -9, -15},
	{20 * 8, -82 * 8,   9, -15},
	{30 * 8, -79 * 8,  13, -11},
	
	{-1 * 8, -74 * 8, -13, -5},
	{ 8 * 8, -77 * 8,  -9, -9},
	{19 * 8, -75 * 8,   9, -9},
	{26 * 8, -74 * 8,  13, -5},
	
	{ 5 * 8, -73 * 8, -5, -3},
	{14 * 8, -76 * 8,  9, -6},
	{26 * 8, -67 * 8, 15, -3},
};

//Boyfriend player types
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
	BFMatt_ArcMain_Miss0,
	BFMatt_ArcMain_Miss1,
	BFMatt_ArcMain_Miss2,
	BFMatt_ArcMain_Miss3,
	BFMatt_ArcMain_Dead0, //BREAK
	
	BFMatt_ArcMain_Max,
};

enum
{
	BFMatt_ArcDead_Dead1, //Mic Drop
	BFMatt_ArcDead_Dead2, //Twitch
	BFMatt_ArcDead_Retry, //Retry prompt
	
	BFMatt_ArcDead_Max,
};

#define BFMatt_Arc_Max BFMatt_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[BFMatt_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
	u8 retry_bump;
	
	SkullFragment skull[COUNT_OF(char_bfmatt_skull)];
	u8 skull_scale;
} Char_BFMatt;

//Boyfriend player definitions
static const CharFrame char_bfmatt_frame[] = {
	{BFMatt_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{BFMatt_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{BFMatt_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{BFMatt_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{BFMatt_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{BFMatt_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	
	{BFMatt_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //6 left 7
	{BFMatt_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //7 left 8
	{BFMatt_ArcMain_Miss1, {  0,   0, 256, 256}, { 125, 250}}, //8 left 9
	
	{BFMatt_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //9 down 10
	{BFMatt_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //10 down 11
	{BFMatt_ArcMain_Miss0, {  0,   0, 256, 256}, { 125, 250}}, //11 down 12
	
	{BFMatt_ArcMain_Up0,   {   0,   0,  256, 256}, { 125, 250}}, //12 up 13
	{BFMatt_ArcMain_Up1,   {   0,   0,  256, 256}, { 125, 250}}, //13 up 14
	{BFMatt_ArcMain_Miss3, {   0,   0,  256, 256}, { 125, 250}}, //14 up 15
	
	{BFMatt_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //15 right 16
	{BFMatt_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //16 right 17
	{BFMatt_ArcMain_Miss2,  {  0,   0, 256, 256}, { 125, 250}}, //17 right 18
	
	{BFMatt_ArcMain_Dead0, {  0,   0, 256, 256}, { 125, 250}}, //18 dead0 0

	{BFMatt_ArcDead_Dead1, {  0,   0, 256, 256}, { 125, 250}}, //19 dead1 0
	
	{BFMatt_ArcDead_Dead2, {  0,   0, 256, 256}, { 125, 250}}, //20 dead2 body twitch 0
};

static const Animation char_bfmatt_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  5, ASCR_REPEAT, 1}},      	   //CharAnim_Idle
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},       		   		   //CharAnim_Left
	{1, (const u8[]){ 6,  8, ASCR_BACK, 1}},                      	   	   //CharAnim_LeftAlt
	{1, (const u8[]){ 6,  8, ASCR_BACK, 1}},                     	  	   //CharAnim_LeftAltB
	{2, (const u8[]){ 9,  10, ASCR_BACK, 1}},  		   		   //CharAnim_Down
	{1, (const u8[]){ 9,  11, ASCR_BACK, 1}},   	          	   	   //CharAnim_DownAlt
	{1, (const u8[]){ 9,  11, ASCR_BACK, 1}},   	          	   	   //CharAnim_DownAltB
	{2, (const u8[]){ 12, 13, ASCR_BACK, 1}},                		   //CharAnim_Up
	{1, (const u8[]){ 12, 14, ASCR_BACK, 1}},                                  //CharAnim_UpAlt
	{1, (const u8[]){ 12, 14, ASCR_BACK, 1}},   	          	           //CharAnim_UpAltB
	{2, (const u8[]){ 15, 16, ASCR_BACK, 1}},                                  //CharAnim_Right
	{1, (const u8[]){ 15, 17, ASCR_BACK, 1}},                                  //CharAnim_RightAlt
	{1, (const u8[]){ 15, 17, ASCR_BACK, 1}},                            	   //CharAnim_RightAltB
	
	{5, (const u8[]){18, 18, 18, 18, 18, 18, 18, ASCR_CHGANI, PlayerAnim_Dead1}}, 		  //PlayerAnim_Dead0
	{5, (const u8[]){18, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){19, 19, 19, 19, 19, 19, 19, ASCR_CHGANI, PlayerAnim_Dead3}}, 		  //PlayerAnim_Dead2
	{3, (const u8[]){19, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){20, 20, 20, 20, 20, ASCR_CHGANI, PlayerAnim_Dead3}},            	  //PlayerAnim_Dead4
	{3, (const u8[]){20, 20, 20, 20, 20, ASCR_CHGANI, PlayerAnim_Dead3}},             	  //PlayerAnim_Dead5
	
	{10, (const u8[]){30, 30, 30, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){30, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Boyfriend player functions
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
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_Right))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != CharAnim_LeftAltB &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != CharAnim_DownAltB &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != CharAnim_UpAltB &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != CharAnim_RightAltB) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead3)
	{
		//Tick skull fragments
		if (this->skull_scale)
		{
			SkullFragment *frag = this->skull;
			for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BFMatt, skull); i++, frag++)
			{
				//Draw fragment
				RECT frag_src = {
					(i & 1) ? 112 : 96,
					(i >> 1) << 4,
					16,
					16
				};
				fixed_t skull_dim = (FIXED_DEC(16,1) * this->skull_scale) >> 6;
				fixed_t skull_rad = skull_dim >> 1;
				RECT_FIXED frag_dst = {
					character->x + (((fixed_t)frag->x << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.x,
					character->y + (((fixed_t)frag->y << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.y,
					skull_dim,
					skull_dim,
				};
				Stage_DrawTex(&this->tex_retry, &frag_src, &frag_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
				
				//Move fragment
				frag->x += frag->xsp;
				frag->y += ++frag->ysp;
			}
			
			//Decrease scale
			this->skull_scale--;
		}
		
		//Draw input options
		u8 input_scale = 16 - this->skull_scale;
		if (input_scale > 16)
			input_scale = 0;
		
		RECT button_src = {
			 0, 96,
			16, 16
		};
		RECT_FIXED button_dst = {
			character->x - FIXED_DEC(32,1) - stage.camera.x,
			character->y - FIXED_DEC(88,1) - stage.camera.y,
			(FIXED_DEC(16,1) * input_scale) >> 4,
			FIXED_DEC(16,1),
		};
		
		//Cross - Retry
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Circle - Blueball
		button_src.x = 16;
		button_dst.y += FIXED_DEC(56,1);
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Draw 'RETRY'
		u8 retry_frame;
		
		if (character->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 2 - (this->retry_bump >> 3);
			if (retry_frame >= 3)
				retry_frame = 0;
			if (this->retry_bump & 2)
				retry_frame += 3;
			
			if (++this->retry_bump == 0xFF)
				this->retry_bump = 0xFD;
		}
		else
		{
			//Idle
			retry_frame = 1 +  (this->retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++this->retry_bump >= 55)
				this->retry_bump = 0;
		}
		
		RECT retry_src = {
			(retry_frame & 1) ? 48 : 0,
			(retry_frame >> 1) << 5,
			48,
			32
		};
		RECT_FIXED retry_dst = {
			character->x -  FIXED_DEC(7,1) - stage.camera.x,
			character->y - FIXED_DEC(92,1) - stage.camera.y,
			FIXED_DEC(48,1),
			FIXED_DEC(32,1),
		};
		Stage_DrawTex(&this->tex_retry, &retry_src, &retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BFMatt_SetFrame);
	Character_Draw(character, &this->tex, &char_bfmatt_frame[this->frame]);
}

void Char_BFMatt_SetAnim(Character *character, u8 anim)
{
	Char_BFMatt *this = (Char_BFMatt*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			//Begin reading dead.arc and adjust focus
			this->arc_dead = IO_AsyncReadFile(&this->file_dead_arc);
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);
			break;
		case PlayerAnim_Dead2:
			//Unload main.arc
			Mem_Free(this->arc_main);
			this->arc_main = this->arc_dead;
			this->arc_dead = NULL;
			
			//Find dead.arc files
			const char **pathp = (const char *[]){
				"dead1.tim", //BFMatt_ArcDead_Dead1
				"dead2.tim", //BFMatt_ArcDead_Dead2
				"retry.tim", //BFMatt_ArcDead_Retry
				NULL
			};
			IO_Data *arc_ptr = this->arc_ptr;
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
			
			//Load retry art
			Gfx_LoadTex(&this->tex_retry, this->arc_ptr[BFMatt_ArcDead_Retry], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BFMatt_Free(Character *character)
{
	Char_BFMatt *this = (Char_BFMatt*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_dead);
}

Character *Char_BFMatt_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BFMatt *this = Mem_Alloc(sizeof(Char_BFMatt));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BFMatt_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BFMatt_Tick;
	this->character.set_anim = Char_BFMatt_SetAnim;
	this->character.free = Char_BFMatt_Free;
	
	Animatable_Init(&this->character.animatable, char_bfmatt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 10;
	
	this->character.focus_x = FIXED_DEC(-50,1);
	this->character.focus_y = FIXED_DEC(-65,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BFMATT.ARC;1");
	this->arc_dead = NULL;
	IO_FindFile(&this->file_dead_arc, "\\CHAR\\MATTDEAD.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //BFMatt_ArcMain_Idle0
		"idle1.tim",  //BFMatt_ArcMain_Idle1
		"idle2.tim",  //BFMatt_ArcMain_Idle2
		"idle3.tim",  //BFMatt_ArcMain_Idle3
		"idle4.tim",  //BFMatt_ArcMain_Idle4
		"idle5.tim",  //BFMatt_ArcMain_Idle5
		"left0.tim",  //BFMatt_ArcMain_Left0
		"left1.tim",  //BFMatt_ArcMain_Left1
		"down0.tim",  //BFMatt_ArcMain_Down0
		"down1.tim",  //BFMatt_ArcMain_Down1
		"up0.tim",    //BFMatt_ArcMain_Up0
		"up1.tim",    //BFMatt_ArcMain_Up1
		"right0.tim", //BFMatt_ArcMain_Right0
		"right1.tim", //BFMatt_ArcMain_Right1
		"miss0.tim",  //BFMatt_ArcMain_Miss0
		"miss1.tim",  //BFMatt_ArcMain_Miss1
		"miss2.tim",  //BFMatt_ArcMain_Miss2
		"miss3.tim",  //BFMatt_ArcMain_Miss3
		"dead0.tim",  //BFMatt_ArcMain_Dead0

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize player state
	this->retry_bump = 0;
	
	//Copy skull fragments
	memcpy(this->skull, char_bfmatt_skull, sizeof(char_bfmatt_skull));
	this->skull_scale = 64;
	
	SkullFragment *frag = this->skull;
	for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BFMatt, skull); i++, frag++)
	{
		//Randomize trajectory
		frag->xsp += RandomRange(-4, 4);
		frag->ysp += RandomRange(-2, 2);
	}
	
	return (Character*)this;
}
