#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"
#include <allegro5\allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <allegro5\allegro_audio.h>
#include <allegro5\allegro_acodec.h>
#define _CRT_SECURE_NO_WARNINGS

using namespace std;

/* Animation Frame Variables */

int frameDelayCount = 0;
int currentFrame = 0;
const int FRAME_DELAY = 6;
const int NUM_RUN_FRAMES = 8;
const int NUM_KICK_IDLE_FRAMES = 2;
const int NUM_JUMP_FRAMES = 4;

/* Key Listener Variables */

enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE, ENTER};
bool keys[6] = {false, false, false, false, false, false};

/* Screen Size */

const int WIDTH = 854;
const int HEIGHT = 480;

/* Number Of Objects */

const int NUM_BULLETS = 10;
const int NUM_ENEMIES = 200;
const int NUM_BRICKS = 6000;
const int NUM_EBULLETS = 100; //enemy bullets

/* Gameplay Variables */

float gravity = .175;
int stun = 4;
bool difficult = false;
int score = 0;

/* Game State Variables */

bool debug = false;
bool pause = false;

/* Cutscene Variables */

int inCutscene = -1;
fstream cutsceneFile;
string cutline = "";

/* File Load Variables */

string level = "levels/loadLevel.txt";
string levelEnemies = " ";
string levelBackground = "images/basicBackground.bmp";
bool loadNewLevel = false;

bool needPlayDeath = true;
int bossHitCount = 0;

void frameDelayIncrementer();

void attack(Mackenzie &mackenzie, Enemy &enemy);

/* Menu Functions */

void drawMenu(Mackenzie &mackenzie, int menuOption, ALLEGRO_FONT *size64, ALLEGRO_FONT *size18, ALLEGRO_BITMAP *title);
void drawStart(int startOption, bool difficult, ALLEGRO_FONT *size64, ALLEGRO_FONT *size18, ALLEGRO_BITMAP *title);
void drawPause(int pauseOption, ALLEGRO_FONT *size18);
void drawCredits(ALLEGRO_FONT *size64, ALLEGRO_FONT *size18, ALLEGRO_BITMAP *title);

/* Load Funcions */
void loadLevel(int levelNumber);
int loadSave(Mackenzie &mackenzie, int menuOption, bool newGame);

void hitMackenzie(Mackenzie &mackenzie, ALLEGRO_SAMPLE *mackenzie_hit);

void InitMackenzie(Mackenzie &mackenzie);

/* Draw Functions */

void DrawMackenzie(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, ALLEGRO_BITMAP *moonWalkerPic, int &mapoffx, int mackenzieWidth, int mackenzieHeight, ALLEGRO_BITMAP *bullet_blast, Bullet bullets[], bool michael);
void DrawRunRight(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, int &mapoffx);
void DrawMoonWalker(Mackenzie &mackenzie, ALLEGRO_BITMAP *moonWalkerPic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, int &mapoffx);
void DrawRunLeft(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, int &mapoffx);
void DrawInAir(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, int mackenzieWidth, int mackenzieHeight, int &mapoffx);
void DrawIdle(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, int mackenzieWidth, int mackenzieHeight, int &mapoffx);
void DrawJump(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, int mackenzieWidth, int mackenzieHeight, int &mapoffx);
void DrawKick(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, ALLEGRO_BITMAP *bullet_blast, Bullet bullets[], int &mapoffx);

/* Movement Functions */

void JumpMackenzie(Mackenzie &mackenzie, ALLEGRO_SAMPLE *jumpSound);
void MoveMackenzieDown(Mackenzie &mackenzie);
void MoveMackenzieLeft(Mackenzie &mackenzie);
void MoveMackenzieRight(Mackenzie &mackenzie);
void UpdateMackenzie(Mackenzie &mackenzie, int &mapoffx, int deadZone);

/* Bullet Functions */

void InitBullet(Bullet bullets[], int size);
void DrawBullet(Bullet bullets[], int size, int &mapoffx);
void FireBullet(Bullet bullets[], int size, Mackenzie &mackenzie, ALLEGRO_SAMPLE *gunShot);
void UpdateBullet(Bullet bullets[], int size, int &mapoffx);
void CollideBullet(Bullet bullets[], int bSize, Enemy enemies[], int eSize, Brick bricks[], int brSize, ALLEGRO_SAMPLE *ghost_death, ALLEGRO_SAMPLE *explosion);

/* Enemy Fuctions */

void InitEnemy(Enemy enemies[], int size, string levelEnemies, int &deadZone);
void DrawEnemy(Enemy enemies[], int size, ALLEGRO_BITMAP *ghost, int &mapoffx, ALLEGRO_BITMAP *bomb, ALLEGRO_BITMAP *boss, ALLEGRO_BITMAP *bossgrumpy, ALLEGRO_BITMAP *fish, int currentLevel);
void StartEnemy(Enemy enemies[], int size, int &mapoffx, EBullet ebullets[], int bSize, Mackenzie &mackenzie, ALLEGRO_FONT *font64, Brick bricks[], int brSize);
void UpdateEnemy(Enemy &enemy, EBullet ebullets[], int size, Mackenzie &mackenzie, ALLEGRO_FONT *font64, Brick bricks[], int brSize);
void CollideEnemy(Enemy enemies[], int size, Mackenzie &mackenzie, ALLEGRO_SAMPLE *ghost_death, ALLEGRO_SAMPLE *mackenzie_hit);

/* Brick Functions */

void InitBrick(Brick bricks[], int size, string level, Mackenzie &mackenzie);
void DrawBrick(Brick bricks[], int size, ALLEGRO_BITMAP *platform, ALLEGRO_BITMAP *spring, int &mapoffx, ALLEGRO_BITMAP *jump_pickup, ALLEGRO_FONT *size32, int unlocked, ALLEGRO_BITMAP *eyeball, Mackenzie &mackenzie, ALLEGRO_BITMAP *seaweed);
void UpdateBrick(Brick bricks[], int size, int &mapoffx, EBullet ebullets[], int ebsize, Mackenzie &mackenzie);
void CollideBrick(Brick bricks[], int size, Mackenzie &mackenzie, int &unlocked, ALLEGRO_SAMPLE *powerup, int &currentLevel, int menuOption);
void CollideBrickLeft(Brick bricks[], int size, Mackenzie &mackenzie);
void CollideBrickRight(Brick bricks[], int size, Mackenzie &mackenzie);
void CollideBrickUp(Brick bricks[], int size, Mackenzie &mackenzie);
void CollideBrickDown(Brick bricks[], int size, Mackenzie &mackenzie);
bool IsBrickOnScreen(Brick bricks[], int brick, int &mapoffx);

/* Enemy Bullet Functions */

void FireCannon(Brick cannon, EBullet ebullets[], int size, Mackenzie &mackenzie);
void InitEBullet(EBullet ebullets[], int size);
void DrawEBullet(EBullet ebullets[], int size, int &mapoffx, ALLEGRO_BITMAP *lightning);
void UpdateEBullet(EBullet ebullets[], int size, int &mapoffx);
void CollideEBullet(EBullet ebullets[], int ebsize, Mackenzie &mackenzie, Brick bricks[], int brSize, ALLEGRO_SAMPLE *mackenzie_hit);

void DrawBackground(ALLEGRO_BITMAP *back, Brick bricks[], int size, int &mapoffx);

void attack(Mackenzie &mackenzie, Enemy &enemy, EBullet ebullets[], int size);

/* Cutscene Functions */

void cutscene(Brick brick, int currentLevel);
void nextScenePage();

int main ()
{
	enum START{ZERO, NEW, LOAD, CREDITS, EXIT};
	enum MENU{ZERO1, SAVE1, SAVE2, SAVE3, BACK}; // menu items
	enum PAUSE{ZERO2, RETURN, LEVELSELECT, STARTMENU, EXITFROMPAUSE}; // pause menu items
	
	bool michael = false;
	bool done = true;
	bool redraw = true;
	bool menuSelect = false;
	int menuOption = SAVE1;
	bool exitGame = false;
	bool startUpScreen = true;
	int startOption = NEW;
	bool newGame = false;
	int unlocked = 1;
	int currentLevel = 0;
	bool credits = false;

	int pauseOption = RETURN;

	/* Set Up Bitmaps */

	ALLEGRO_BITMAP *back = NULL;
	ALLEGRO_BITMAP *platform = NULL;
	ALLEGRO_BITMAP *ghost = NULL;
	ALLEGRO_BITMAP *mackenziePic = NULL;
	ALLEGRO_BITMAP *bullet_blast = NULL;
	ALLEGRO_BITMAP *bomb = NULL;
	ALLEGRO_BITMAP *jump_pickup = NULL;
	ALLEGRO_BITMAP *spring = NULL;
	ALLEGRO_BITMAP *mackenziePicCrouch = NULL;
	ALLEGRO_BITMAP *heel = NULL;
	ALLEGRO_BITMAP *moonWalkerPic = NULL;
	ALLEGRO_BITMAP *eyeball = NULL;
	ALLEGRO_BITMAP *boss = NULL;
	ALLEGRO_BITMAP *title = NULL;
	ALLEGRO_BITMAP *seaweed = NULL;
	ALLEGRO_BITMAP *fish = NULL;
	ALLEGRO_BITMAP *lightning = NULL;
	ALLEGRO_BITMAP *bossgrumpy = NULL;

	/* FPS Variables */

	const int FPS = 60;
	int frames = 0;
	int currentTime = 0;
	int lastUpdateTime = 0;
	int elapsedTime = 0;
	int actualFPS = 0;

	int mapoffx = 0;    // 0 to start but does change
	int deadZone = 350; //

	/* Mackenzie Sprite Size */

	const int MACKENZIE_WIDTH = 32;
	const int MACKENZIE_HEIGHT = 64;

	/* Frame Variables */

	int mackenzieWalkFrame = 0;
	int mackenzieIdleFrame = 0;

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	if(!al_init())
	{
		return -1;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
	display = al_create_display(WIDTH, HEIGHT);
	
	if(!display)
	{
		return -1;
	}

	timer = al_create_timer(1.0/FPS);
	
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();

	/* Music & Sounds */
	
	const int MAX_SOUNDS = 4; // may need more

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(MAX_SOUNDS);

	ALLEGRO_SAMPLE *song = al_load_sample("music/MacAttack (Extended).wav");
	ALLEGRO_SAMPLE *death = al_load_sample("music/MacDeath.wav");
	ALLEGRO_SAMPLE *jumpSound = al_load_sample("music/Jump.wav");
	ALLEGRO_SAMPLE *gunShot = al_load_sample("music/gun_shot.wav");
	ALLEGRO_SAMPLE *powerup = al_load_sample("music/Powerup.wav");
	ALLEGRO_SAMPLE *moonWalkerSound = al_load_sample("music/mj_louder.wav");
	ALLEGRO_SAMPLE *ghost_death = al_load_sample("music/ghost_death.wav");
	ALLEGRO_SAMPLE *mackenzie_hit = al_load_sample("music/heels_hit.wav");
	ALLEGRO_SAMPLE *explosion = al_load_sample("music/explosion.wav");

	event_queue = al_create_event_queue();
	
	/* Fonts */

	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 18, 0);	
	ALLEGRO_FONT *font64 = al_load_font("arial.ttf", 64, 0);
	ALLEGRO_FONT *font32 = al_load_font("arial.ttf", 32, 0);

	/* Load Bitmaps */

	platform = al_load_bitmap("images/brick1.bmp");
	ghost = al_load_bitmap("images/ghost.bmp");
	mackenziePic = al_load_bitmap("images/mackenzie_sheet.bmp");
	bullet_blast = al_load_bitmap("images/gun_blast.bmp");
	bomb = al_load_bitmap("images/bomb.bmp");
	jump_pickup = al_load_bitmap("images/jump_pickup.bmp");
	spring = al_load_bitmap("images/spring_jump.bmp");
	mackenziePicCrouch = al_load_bitmap("images/mackenzie_crawl_sheet.bmp");
	heel = al_load_bitmap("images/heel.bmp");
	moonWalkerPic = al_load_bitmap("images/moonwalkersheet.bmp");
	eyeball = al_load_bitmap("images/eyeball.bmp");
	boss = al_load_bitmap("images/boss.bmp");
	title = al_load_bitmap("images/title.bmp");
	seaweed = al_load_bitmap("images/seaweed.bmp");
	fish = al_load_bitmap("images/fish.bmp");
	lightning = al_load_bitmap("images/lightning.bmp");
	bossgrumpy = al_load_bitmap("images/grumpyface.bmp");

	/* Set Up Event Queue */

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	/* Set Bitmap Transparency */

	al_convert_mask_to_alpha(ghost, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(mackenziePic, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(bullet_blast, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(bomb, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(jump_pickup, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(spring, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(mackenziePicCrouch, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(heel, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(moonWalkerPic, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(eyeball, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(boss, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(seaweed, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(fish, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(lightning, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(bossgrumpy, al_map_rgb(255, 0, 255));

	srand(time(NULL));

	/* Start Playing Music */

	al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);

	/* Set Up Player & Enemy Objects */

	Mackenzie mackenzie;
	Bullet bullets[NUM_BULLETS];
	Enemy enemies[NUM_ENEMIES];
	Brick bricks[NUM_BRICKS];
	EBullet ebullets[NUM_EBULLETS];
	ALLEGRO_EVENT ev;

	while(!exitGame)
	{
		keys[UP] = false;
		keys[DOWN] = false;
		keys[LEFT] = false;
		keys[RIGHT] = false;
		keys[SPACE] = false;
		keys[ENTER] = false;
		mapoffx = 0;
		menuOption = SAVE1;
		startOption = NEW;
		pauseOption = RETURN;

		InitMackenzie(mackenzie);
		InitBullet(bullets, NUM_BULLETS);
		InitEBullet(ebullets, NUM_EBULLETS);
		InitEnemy(enemies, NUM_ENEMIES, levelEnemies, deadZone);
		InitBrick(bricks, NUM_BRICKS, level, mackenzie);

		mackenzie.x = WIDTH/2 - 100;
		mackenzie.y = 60 + startOption * 80;
		drawStart(startOption, difficult, font64, font18, title);
		DrawMackenzie(mackenzie, mackenziePic, mackenziePicCrouch, moonWalkerPic,  mapoffx, MACKENZIE_WIDTH, MACKENZIE_HEIGHT, bullet_blast, bullets, michael);
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
		mackenzie.onGround = true;

		al_start_timer(timer);

		while(startUpScreen)
		{
			al_wait_for_event(event_queue, &ev);
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch(ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_UP:
					if(!(startOption <= NEW))
					{
						startOption--;
					}
					break;
				case ALLEGRO_KEY_DOWN:
					if(!(startOption >= EXIT))
					{
						startOption++;
					}
					break;
				case ALLEGRO_KEY_ENTER:
					startUpScreen = false;
					break;
				case ALLEGRO_KEY_SPACE:
					startUpScreen = false;
					break;
				case ALLEGRO_KEY_D:
					if(difficult)
					{
						difficult = false;
					}
					else
					{
						difficult = true;
					}
					break;
				}
			}
			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				al_destroy_display(display);
				al_destroy_sample(song);
				return 0;
			}
			else if(ev.type == ALLEGRO_EVENT_TIMER)
			{
				redraw = true;
			}
			if(redraw && al_is_event_queue_empty(event_queue))
			{
				redraw = false;
				mackenzie.x = WIDTH/2 - 100;
				mackenzie.y = 60 + startOption * 80;
				drawStart(startOption, difficult, font64, font18, title);
				DrawMackenzie(mackenzie, mackenziePic, mackenziePicCrouch, moonWalkerPic,  mapoffx, MACKENZIE_WIDTH, MACKENZIE_HEIGHT, bullet_blast, bullets, michael);
				
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
		}

		switch(startOption)
		{
		case NEW:
			newGame = true;
			menuSelect = true;
			break;
		case LOAD:
			newGame = false;
			menuSelect = true;
			break;
		case CREDITS:
			credits = true;
			break;
		case EXIT:
			exit(0);
			break;
		}
		
		while(credits)
		{
			al_wait_for_event(event_queue, &ev);

			if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch(ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ENTER:
					credits = false;
					startUpScreen = true;
					menuSelect = false;
					break;
				case ALLEGRO_KEY_SPACE:
					credits = false;
					startUpScreen = true;
					menuSelect = false;
					break;
				}
			}
			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				al_destroy_display(display);
				al_destroy_sample(song);
				return 0;
			}
			else if(ev.type == ALLEGRO_EVENT_TIMER)
			{
				redraw = true;
			}
			if(redraw && al_is_event_queue_empty(event_queue))
			{
				redraw = false;
				drawCredits(font64, font18, title);
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
		}

		mackenzie.x = WIDTH/2 - 135;
		mackenzie.y = 60 + menuOption * 80;
		drawMenu(mackenzie, menuOption, font64, font18, title);
		DrawMackenzie(mackenzie, mackenziePic, mackenziePicCrouch, moonWalkerPic, mapoffx, MACKENZIE_WIDTH, MACKENZIE_HEIGHT, bullet_blast, bullets, michael);
		
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
		mackenzie.onGround = true;

		while(menuSelect){
			al_wait_for_event(event_queue, &ev);

			if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch(ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_UP:
					if(!(menuOption <= SAVE1))
					{
						menuOption--;
					}
					break;
				case ALLEGRO_KEY_DOWN:
					if(!(menuOption >= BACK))
					{
						menuOption++;
					}
					break;
				case ALLEGRO_KEY_ENTER:
					menuSelect = false;
					done = false;
					break;
				case ALLEGRO_KEY_SPACE:
					menuSelect = false;
					done = false;
					break;
				}
			}
			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				al_destroy_display(display);
				al_destroy_sample(song);
				return 0;
			}
			else if(ev.type == ALLEGRO_EVENT_TIMER)
			{
				redraw = true;
			}
			if(redraw && al_is_event_queue_empty(event_queue))
			{
				redraw = false;
				mackenzie.x = WIDTH/2 - 135;
				mackenzie.y = 60 + menuOption * 80;
				drawMenu(mackenzie, menuOption, font64, font18, title);
				DrawMackenzie(mackenzie, mackenziePic, mackenziePicCrouch, moonWalkerPic, mapoffx, MACKENZIE_WIDTH, MACKENZIE_HEIGHT, bullet_blast, bullets, michael);
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
		}
		
		if(menuOption != BACK)
		{
			unlocked = loadSave(mackenzie, menuOption, newGame);
			redraw = true;
			loadNewLevel = true;
			level = "levels/loadLevel.txt";
			mackenzie.x = 64 + (unlocked - 1) * 192;
			mackenzie.y = 394;
			levelEnemies = " ";
			levelBackground = "images/basicBackground.bmp";
		}
		else
		{
			done = true;
			menuOption = false;
			startUpScreen = true;
		}

		while(!done)
		{
			al_wait_for_event(event_queue, &ev);
			if(loadNewLevel)
			{
				/* Set Up Difficulty */

				if(difficult)
				{
					//classic
					stun = 4;
					//mackenzie.lives = 3;
					gravity = .175;
					mackenzie.acc = .075;
					mackenzie.dec = .5;
				}
				else
				{
					//normal
					stun = 5; // outside of stun range, so no stun
					//mackenzie.lives = 5;
					gravity = .2;
					mackenzie.acc = .075;
					mackenzie.dec = .5;
				}

				InitEnemy(enemies, NUM_ENEMIES, levelEnemies, deadZone);
				InitBrick(bricks, NUM_BRICKS, level, mackenzie);
				InitBullet(bullets, NUM_BULLETS);
				InitEBullet(ebullets, NUM_EBULLETS);
				back = al_load_bitmap(levelBackground.c_str());
				inCutscene = -1;
				loadNewLevel = false;

				switch(currentLevel)
				{

				case 1:
					al_destroy_sample(song);
					song = al_load_sample("music/LevelStart.wav");
					al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					break;
	
				case 2:
					al_destroy_sample(song);
					song = al_load_sample("music/LevelStart (Acoustic).wav");
					al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					break;
	
				case 3:
					al_destroy_sample(song);
					song = al_load_sample("music/Somethings Amiss (Acoustic).wav");
					al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					break;
	
				case 4:
					al_destroy_sample(song);
					song = al_load_sample("music/MacAttack (Acoustic).wav");
					al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					break;
	
				case 5:
					al_destroy_sample(song);
					song = al_load_sample("music/Monstruous_Castle.wav");
					al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					break;
	
				case 6:
					al_destroy_sample(song);
					song = al_load_sample("music/Somethings Amiss.wav");
					al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					break;
	
				case 7:
					al_destroy_sample(song);
					song = al_load_sample("music/Somethings Amiss.wav");
					al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					break;
	
				}
			}

			if (inCutscene == 0)
			{
				if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
				{
					keys[LEFT] = false;
					keys[RIGHT] = false;
					keys[UP] = false;
					keys[DOWN] = false;
					keys[SPACE] = false;
					nextScenePage();
				}
				goto draw;
			}

			if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch(ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_D:
					if(debug)
					{
						debug = false;
						michael = false;
					}
					else
					{
						debug = true;
					}
					break;

				case ALLEGRO_KEY_L:
					if(debug)
					{
						mackenzie.lives++;
					}
					break;
				case ALLEGRO_KEY_M:
					if(debug)
					{
						if(michael)
						{
							michael = false;
						}
						else
						{
							michael = true;
						}
						al_play_sample(moonWalkerSound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

					}
					break;

				case ALLEGRO_KEY_UP:
					if(!mackenzie.isCrouched)
					{
						JumpMackenzie(mackenzie, jumpSound);
					}
					keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					if(difficult)
					{
						//classic
						if(mackenzie.onGround && mackenzie.timer < stun) // stun adjust
						{
							mackenzie.isCrouched = true;
							mackenzie.y += 32;
							mackenzie.x -= 14;
						}
					}
					else
					{
						//normal
						if(mackenzie.timer < stun) // stun adjust
						{
							mackenzie.isCrouched = true;
							
							if(!keys[LEFT] && !keys[RIGHT])
							{
								mackenzie.y += 32;
								mackenzie.x -= 14;
							}
						}
					}
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = true;
					mackenzie.kicked = true;
					FireBullet(bullets, NUM_BULLETS, mackenzie, gunShot);
					break;
				case ALLEGRO_KEY_R:
					startUpScreen = true;
					menuSelect = false;
					done = true;
					break;
				case ALLEGRO_KEY_ENTER:
					keys[ENTER] = true;
					break;
				case ALLEGRO_KEY_P:
					pause = true;
					break;
				}
			}
			else if(ev.type == ALLEGRO_EVENT_KEY_UP)
			{
				switch(ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					// may need to change
					if(difficult)
					{
						if(mackenzie.isCrouched)
						{
							mackenzie.isCrouched = false;
							mackenzie.y -= 32;
							mackenzie.x += 14;
						}
					}
					else
					{
						if(mackenzie.isCrouched)
						{
							mackenzie.isCrouched = false;
							if(!keys[LEFT] && !keys[RIGHT])
							{
								mackenzie.y -= 32;
								mackenzie.x += 14;
							}
						}
					}
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = false;
					break;
				case ALLEGRO_KEY_ESCAPE:
					exit(0);
					break;
				case ALLEGRO_KEY_ENTER:
					keys[ENTER] = false;
					break;
				}
			}
			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				al_destroy_display(display);
				al_destroy_sample(song);
				return 0;
			}

			else if(ev.type == ALLEGRO_EVENT_TIMER)
			{
				redraw = true;
				if(keys[DOWN])
				{
					MoveMackenzieDown(mackenzie);
				}
				
				if(keys[LEFT] && !keys[RIGHT])
				{
					MoveMackenzieLeft(mackenzie);
				}
				if(keys[RIGHT] && !keys[LEFT])
				{
					MoveMackenzieRight(mackenzie);
				}
			}
			while(pause)
			{
				al_wait_for_event(event_queue, &ev);

				if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					switch(ev.keyboard.keycode)
					{
					case ALLEGRO_KEY_UP:
						keys[UP] = true;
						if(!(pauseOption <= RETURN))
						{
							pauseOption--;
						}
						break;
					case ALLEGRO_KEY_DOWN:
						keys[DOWN] = true;
						if(!(pauseOption >= EXITFROMPAUSE))
						{
							pauseOption++;
						}
						break;
					case ALLEGRO_KEY_RIGHT:
						keys[RIGHT] = true;
						break;
					case ALLEGRO_KEY_LEFT:
						keys[LEFT] = true;
						break;
					case ALLEGRO_KEY_ENTER:
						switch(pauseOption)
						{
						case RETURN:
							pause = false;
							break;
						case LEVELSELECT:
							loadNewLevel = true;
							redraw = true;
							level = "levels/loadLevel.txt";
							mackenzie.x = 64 + (unlocked - 1) * 192;
							mackenzie.y = 394;
							mackenzie.vx = 0;
							mackenzie.vy = 0;
							levelEnemies = " ";
							levelBackground = "images/basicBackground.bmp";
							pause = false;
							break;
						case STARTMENU:
							startUpScreen = true;
							done = true;
							pause = false;
							break;
						case EXITFROMPAUSE:
							exit(0);
						}
						break;
					case ALLEGRO_KEY_P:
						pause = false;
						break;
					}
				}
				else if(ev.type == ALLEGRO_EVENT_KEY_UP)
				{
					switch(ev.keyboard.keycode)
					{
					case ALLEGRO_KEY_UP:
						keys[UP] = false;
						break;
					case ALLEGRO_KEY_DOWN:
						keys[DOWN] = false;
						break;
					case ALLEGRO_KEY_LEFT:
						keys[LEFT] = false;
						break;
					case ALLEGRO_KEY_RIGHT:
						keys[RIGHT] = false;
						break;
					case ALLEGRO_KEY_SPACE:
						keys[SPACE] = false;
						break;
					case ALLEGRO_KEY_ENTER:
						keys[ENTER] = false;
						break;
					}
				}
				else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				{
					/* Destroy Display */

					// TODO: destroy all other objects before close

					al_destroy_display(display);
					al_destroy_sample(song);
					return 0;
				}
				else if(ev.type == ALLEGRO_EVENT_TIMER)
				{
					redraw = true;
				}
				if(redraw && al_is_event_queue_empty(event_queue))
				{
					redraw = false;
					DrawBackground(back, bricks, NUM_BRICKS, mapoffx);
					DrawMackenzie(mackenzie, mackenziePic, mackenziePicCrouch, moonWalkerPic, mapoffx, MACKENZIE_WIDTH, MACKENZIE_HEIGHT, bullet_blast, bullets, michael);
					DrawBullet(bullets, NUM_BULLETS, mapoffx);
					DrawEnemy(enemies, NUM_ENEMIES, ghost, mapoffx, bomb, boss, bossgrumpy,  fish, currentLevel);
					DrawEBullet(ebullets, NUM_EBULLETS, mapoffx, lightning);
					DrawBrick(bricks, NUM_BRICKS, platform, spring, mapoffx, jump_pickup, font32, unlocked,	eyeball, mackenzie, seaweed);
					drawPause(pauseOption, font18);
					al_flip_display();
					al_clear_to_color(al_map_rgb(0,0,0));
				}
			}
			if(redraw && (al_is_event_queue_empty(event_queue)))
			{
				redraw = false;

				//begin calculate and display fps timer
				frames++;
				currentTime = clock();
				elapsedTime = currentTime - lastUpdateTime;

				if(elapsedTime >= 500)
				{
					actualFPS = (frames * 1000 / elapsedTime);
					frames = 0;
					lastUpdateTime = currentTime;
					UpdateBrick(bricks, NUM_BRICKS, mapoffx, ebullets, NUM_EBULLETS, mackenzie);
					if(!(mackenzie.timer <= 0))
					{
						mackenzie.timer--;
					}
				}
				//end calculate and display fps timer

				//start drawing and updating everything
				if(mackenzie.lives > 0)
				{
					UpdateBullet(bullets, NUM_BULLETS, mapoffx);
					StartEnemy(enemies, NUM_ENEMIES, mapoffx, ebullets, NUM_EBULLETS, mackenzie, font64, bricks, NUM_BRICKS);
					UpdateMackenzie(mackenzie, mapoffx, deadZone);
					UpdateEBullet(ebullets, NUM_EBULLETS, mapoffx);
					CollideBullet(bullets, NUM_BULLETS, enemies, NUM_ENEMIES, bricks, NUM_BRICKS, ghost_death, explosion);
					CollideEBullet(ebullets, NUM_EBULLETS, mackenzie, bricks, NUM_BRICKS, mackenzie_hit);
					CollideBrick(bricks, NUM_BRICKS,  mackenzie, unlocked, powerup, currentLevel, menuOption);
					CollideEnemy(enemies, NUM_ENEMIES, mackenzie, ghost_death, mackenzie_hit);
				}
				draw: //?
				DrawBackground(back, bricks, NUM_BRICKS, mapoffx);
				DrawMackenzie(mackenzie, mackenziePic, mackenziePicCrouch, moonWalkerPic, mapoffx, MACKENZIE_WIDTH, MACKENZIE_HEIGHT, bullet_blast, bullets, michael);
				DrawBullet(bullets, NUM_BULLETS, mapoffx);
				DrawEnemy(enemies, NUM_ENEMIES, ghost, mapoffx, bomb, boss, bossgrumpy, fish, currentLevel);
				DrawBrick(bricks, NUM_BRICKS, platform, spring, mapoffx, jump_pickup, font32, unlocked, eyeball, mackenzie, seaweed);
				DrawEBullet(ebullets, NUM_EBULLETS, mapoffx, lightning);
				if(mackenzie.lives <= 0)
				{
					mackenzie.dead = true;
					al_draw_textf(font64, al_map_rgb(rand()%255, rand()%255, rand()%255), 0 + rand()%300, 50 + rand()%(HEIGHT-150),	0, "GAME OVER");
					al_draw_text(font64, al_map_rgb(255, 255, 255), 130, HEIGHT-70, 0, "Press [P] to Pause");
					al_rest(.5);
				}

				if(mackenzie.dead)
				{
					if(needPlayDeath)
					{
						al_destroy_sample(song);
						song = al_load_sample("music/MacDeath.wav");
						al_play_sample(song, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
						needPlayDeath = false;
					}
				}

				if(mackenzie.y > HEIGHT + 100)
				{
					mackenzie.lives = 0;
				}
				
				/* Cutscene */

				if (cutline != "")
				{
					al_draw_filled_rounded_rectangle(0, HEIGHT - 80, WIDTH, HEIGHT, 10, 10, al_map_rgb(0,0,0));
					al_draw_textf(font18, al_map_rgb(255, 255, 255), 20, HEIGHT - 50, 0, "%s", cutline.c_str());
				}
				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH, 0, 
					ALLEGRO_ALIGN_RIGHT, "FPS : %i", actualFPS);

				/* Show Debug Text */

				if(debug)
				{
					al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH, HEIGHT/2, ALLEGRO_ALIGN_RIGHT, "DEBUG");
				}

				/* Draw Lives */

				if(mackenzie.lives < 10) {

					int offset = 40; // TODO: make into a costant?
					for(int i = 0; i < mackenzie.lives; i++)
					{
						al_draw_bitmap(heel, 10 + offset * i, 10, 0);
					}

				} else {

					al_draw_bitmap(heel, 10, 10, 0);
					al_draw_textf(font18, al_map_rgb(255, 255, 255), 50, 10, ALLEGRO_ALIGN_RIGHT, "%d", mackenzie.lives);

				}

				if(!difficult)
				{
					//draw score
					al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH * 3/5, 15, ALLEGRO_ALIGN_RIGHT, "SCORE: %d", score);
				}

				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
				//end drawing and updating everything
			}
		}
	}

	// TODO: destroy all other objects when game ends

	al_destroy_display(display);
	al_destroy_sample(song);

	return 0;
}

void drawCredits(ALLEGRO_FONT *size64, ALLEGRO_FONT *size18, ALLEGRO_BITMAP *title)
{
	al_draw_bitmap(title, 0, 0, 0);
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 162, 
		ALLEGRO_ALIGN_CENTER, "Programming:   Brandon Tom, Nathan Jarvis, Peter Samyn");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 242, 
		ALLEGRO_ALIGN_CENTER, "Art & Animation:   Peter Samyn, Brandon Tom, Nathan Jarvis");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 322, 
		ALLEGRO_ALIGN_CENTER, "Story & Cutscenes:   Katie Greenwald");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 402, 
		ALLEGRO_ALIGN_CENTER, "Music & Sound:   Joe Bota, Peter Samyn, Chris Lui");
}

void drawPause(int pauseOption, ALLEGRO_FONT *size18)
{
	al_draw_filled_rectangle(0, 0, 854, 480, al_map_rgba_f(0, 0, 0, .5));
	al_draw_filled_rounded_rectangle(250, 82, 604, 330, 5, 5, al_map_rgba_f(0, 0, 0, .75));
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2-120, 82 + pauseOption * 50, 0 , "---");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 132, 
		ALLEGRO_ALIGN_CENTER, "Return to Game");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 182, 
		ALLEGRO_ALIGN_CENTER, "Return to Level Select");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 232, 
		ALLEGRO_ALIGN_CENTER, "Return to Start Menu");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 282, 
		ALLEGRO_ALIGN_CENTER, "Exit Game");
}
void drawMenu(Mackenzie &mackenzie, int menuOption, ALLEGRO_FONT *size64, ALLEGRO_FONT *size18, ALLEGRO_BITMAP *title)
{
	al_draw_bitmap(title, 0, 0, 0);
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 162, 
		ALLEGRO_ALIGN_CENTER, "Save Slot 1   Level: %i", loadSave(mackenzie, 1, false));
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 242, 
		ALLEGRO_ALIGN_CENTER, "Save Slot 2   Level: %i", loadSave(mackenzie, 2, false));
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 322, 
		ALLEGRO_ALIGN_CENTER, "Save Slot 3   Level: %i", loadSave(mackenzie, 3, false));
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 402, 
		ALLEGRO_ALIGN_CENTER, "Back");
}
void drawStart(int startOption, bool difficult, ALLEGRO_FONT *size64, ALLEGRO_FONT *size18, ALLEGRO_BITMAP *title)
{
	string difficultString;
	if(difficult)
	{
		difficultString = "Classic";
	}
	else
	{
		difficultString = "Normal";
	}
	al_draw_bitmap(title, 0, 0, 0);
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 162, 
		ALLEGRO_ALIGN_CENTER, "New Game");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 242, 
		ALLEGRO_ALIGN_CENTER, "Load Game");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 322, 
		ALLEGRO_ALIGN_CENTER, "Credits");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 402, 
		ALLEGRO_ALIGN_CENTER, "Exit");
	al_draw_textf(size18, al_map_rgb(255, 255, 255), WIDTH/2, 450, 
		ALLEGRO_ALIGN_CENTER, "Difficulty (D): %s", difficultString.c_str());
}

int loadSave(Mackenzie &mackenzie, int menuOption, bool newGame)
{
	string line;
	string save;
	string scoreString;
	string livesString;
	int value = 0;
	if(menuOption == 1)
	{
		save = "saves/save1.txt";
	}
	else if(menuOption == 2)
	{
		save = "saves/save2.txt";
	}
	else if(menuOption == 3)
	{
		save = "saves/save3.txt";
	}
	if(newGame)
	{
		ofstream file (save);
		if(file.is_open())
		{
			if(difficult) {
				//classic
				file << "1\n0\n3";

			} else {
				//normal
				file << "1\n0\n5";
			}
		}
	}
	ifstream file (save);
	if (file.is_open())
	{
		while (file.good() && !file.eof())
		{
			getline (file, line);
			getline (file, scoreString);
			getline (file, livesString);
		}
		file.close();
	}
	value = atoi(line.c_str());
	score = atoi(scoreString.c_str());
	mackenzie.lives = atoi(livesString.c_str());
	return value;
}

void InitMackenzie(Mackenzie &mackenzie)
{
	mackenzie.x = 50;
	mackenzie.y = 50;
	mackenzie.ID = PLAYER;
	if(difficult) {
		//classic
		mackenzie.lives = 3;
	} else {
		//normal
		mackenzie.lives = 5;
	}
	mackenzie.vx = 0;
	mackenzie.vy = 0;
	mackenzie.acc = .075;
	mackenzie.dec = .5;
	mackenzie.maxSpeed = 5;
	mackenzie.boundLeft = 0;
	mackenzie.boundRight = 32;
	mackenzie.boundUp = 0;
	mackenzie.boundDown = 64;
	mackenzie.score = 0;
	mackenzie.onGround = false;
	mackenzie.isCrouched = false;
	mackenzie.invincible = false;
	mackenzie.timer = 0;
	mackenzie.isFacingRight = true;
	mackenzie.jumped = false;
	mackenzie.kicked = false;
	mackenzie.dead = false;
}
void DrawMackenzie(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, ALLEGRO_BITMAP *moonWalkerPic, int &mapoffx, int mackenzieWidth, int mackenzieHeight, ALLEGRO_BITMAP *bullet_blast, Bullet bullets[], bool michael)
{
	if(mackenzie.kicked && mackenzie.timer < stun)
	{
		DrawKick(mackenzie, mackenziePic, mackenziePicCrouch, mackenzieWidth, mackenzieHeight, bullet_blast, bullets, mapoffx);
	}
	else if(mackenzie.jumped)
	{
		DrawJump(mackenzie, mackenziePic, mackenzieWidth, mackenzieHeight, mapoffx);
	}

	else if(keys[RIGHT] && keys[LEFT])
	{
		if(michael)
		{

			DrawMoonWalker(mackenzie, moonWalkerPic, mackenziePicCrouch, mackenzieWidth, mackenzieHeight, mapoffx);
		}
		else
		{

			if(!(mackenzie.vy != 0)) // check for falling
			{
				DrawRunRight(mackenzie, mackenziePic, mackenziePicCrouch, mackenzieWidth, mackenzieHeight, mapoffx);
			}
			else
			{
				DrawInAir(mackenzie, mackenziePic, mackenzieWidth, mackenzieHeight, mapoffx);
			}

		}

	}

	else if(keys[RIGHT]) 
	{
		if(!(mackenzie.vy != 0))
		{
			DrawRunRight(mackenzie, mackenziePic, mackenziePicCrouch, mackenzieWidth, mackenzieHeight, mapoffx);
		}
		else
		{
			DrawInAir(mackenzie, mackenziePic, mackenzieWidth, mackenzieHeight, mapoffx);
		}
	}
	else if(keys[LEFT])
	{
		if(!(mackenzie.vy != 0))
		{
			DrawRunLeft(mackenzie, mackenziePic, mackenziePicCrouch, mackenzieWidth, mackenzieHeight, mapoffx);
		}
		else
		{
			DrawInAir(mackenzie, mackenziePic, mackenzieWidth, mackenzieHeight, mapoffx);
		}

	}
	else if(mackenzie.isCrouched)
	{
		if(mackenzie.isFacingRight)
		{
			if(mackenzie.timer != 0)
			{
				al_draw_tinted_bitmap_region(mackenziePicCrouch, al_map_rgba_f(.6, .6, .6, .3), 0, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
			}
			else
			{
				al_draw_bitmap_region(mackenziePicCrouch, 0, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
			}
		}
		else
		{
			if(mackenzie.timer != 0)
			{
				al_draw_tinted_bitmap_region(mackenziePicCrouch, al_map_rgba_f(.6, .6, .6, .3), 0, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
			}
			else
			{
				al_draw_bitmap_region(mackenziePicCrouch, 0, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
			}
		}
	}
	else
	{
		DrawIdle(mackenzie, mackenziePic, mackenzieWidth, mackenzieHeight, mapoffx);
	}

	frameDelayIncrementer();
}
void DrawRunRight(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, int &mapoffx)
{
	if(currentFrame > 7)
	{
		currentFrame = 0;
	}
	if(mackenzie.timer != 0)
	{
		if(mackenzie.isCrouched)
		{
			al_draw_tinted_bitmap_region(mackenziePicCrouch, al_map_rgba_f(.6, .6, .6, .3), currentFrame * mackenzieHeight, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
		else
		{
			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), currentFrame * mackenzieWidth, 0, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
	}
	else
	{
		if(mackenzie.isCrouched)
		{
			al_draw_bitmap_region(mackenziePicCrouch, currentFrame * mackenzieHeight, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, currentFrame * mackenzieWidth, 0, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
	}
	if(frameDelayCount == 0)
	{
		currentFrame++;
		if(currentFrame == NUM_RUN_FRAMES)
		{
			currentFrame = 0;
		}
	}
}

void DrawMoonWalker(Mackenzie &mackenzie, ALLEGRO_BITMAP *moonWalkerPic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, int &mapoffx)
{
	if(currentFrame > 5)
	{
		currentFrame = 0;
	}
	if(mackenzie.timer != 0)
	{
		if(mackenzie.isCrouched)
		{
			al_draw_tinted_bitmap_region(mackenziePicCrouch, al_map_rgba_f(.6, .6, .6, .3), currentFrame * mackenzieHeight, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
		else
		{
			al_draw_tinted_bitmap_region(moonWalkerPic, al_map_rgba_f(.6, .6, .6, .3), currentFrame * mackenzieWidth, 0, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
	}
	else
	{
		if(mackenzie.isCrouched)
		{
			al_draw_bitmap_region(mackenziePicCrouch, currentFrame * mackenzieHeight, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
		else
		{
			al_draw_bitmap_region(moonWalkerPic, currentFrame * mackenzieWidth, 0, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
	}
	if(frameDelayCount == 0)
	{
		currentFrame++;
		if(currentFrame == NUM_RUN_FRAMES)
		{
			currentFrame = 0;
		}
	}
}
void DrawRunLeft(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, int &mapoffx)
{
	if(currentFrame > 7)
	{
		currentFrame = 0;
	}
	if(mackenzie.timer != 0)
	{
		if(mackenzie.isCrouched)
		{
			al_draw_tinted_bitmap_region(mackenziePicCrouch, al_map_rgba_f(.6, .6, .6, .3), currentFrame * mackenzieHeight, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
		else
		{

			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), currentFrame * mackenzieWidth, 0, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
	}
	else
	{
		if(mackenzie.isCrouched)
		{
			al_draw_bitmap_region(mackenziePicCrouch, currentFrame * mackenzieHeight, 0, mackenzieHeight, mackenzieWidth, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, currentFrame * mackenzieWidth, 0, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}

	}
	if(frameDelayCount == 0)
	{
		currentFrame++;
		if(currentFrame == NUM_RUN_FRAMES)
		{
			currentFrame = 0;
		}
	}
}
void DrawInAir(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, int mackenzieWidth, int mackenzieHeight, int &mapoffx)
{
	if(keys[RIGHT])
	{
		if(mackenzie.timer != 0)
		{
			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), 7 * mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, 7 * mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, 0);
		}
	}
	else
	{
		if(mackenzie.timer != 0)
		{
			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), 7 * mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, 7 * mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x - 1-mapoffx, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
	}
}
void DrawIdle(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, int mackenzieWidth, int mackenzieHeight, int &mapoffx)
{
	if(currentFrame  > (NUM_KICK_IDLE_FRAMES - 1) * 3)
	{
		currentFrame = 0;
	}
	if(mackenzie.isFacingRight)
	{
		if(mackenzie.timer != 0)
		{
			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), currentFrame/3 * mackenzieWidth, mackenzieHeight * 2, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, currentFrame/3 * mackenzieWidth, mackenzieHeight * 2, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
		}
	}
	else
	{
		if(mackenzie.timer != 0)
		{
			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), currentFrame/3 * mackenzieWidth, mackenzieHeight * 2, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, currentFrame/3 * mackenzieWidth, mackenzieHeight * 2, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
	}
	if(frameDelayCount == 0)
	{
		currentFrame++;
		if(currentFrame == NUM_KICK_IDLE_FRAMES * 3)
		{
			currentFrame = 0;
		}
	}
}
void DrawJump(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, int mackenzieWidth, int mackenzieHeight, int &mapoffx)
{
	if(currentFrame > 3)
	{
		currentFrame = 0;
	}

	if(mackenzie.isFacingRight)
	{
		if(mackenzie.timer != 0)
		{
			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), (currentFrame * mackenzieWidth) + 128, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, (currentFrame * mackenzieWidth) + 128, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
		}
	}
	else
	{
		if(mackenzie.timer != 0)
		{
			al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), (currentFrame * mackenzieWidth) + 128, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
		else
		{
			al_draw_bitmap_region(mackenziePic, (currentFrame * mackenzieWidth) + 128, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
		}
	}
	if(frameDelayCount == 0)
	{
		currentFrame++;
		if(currentFrame == 4)
		{
			currentFrame = 0;
			mackenzie.jumped = false;
		}
	}
}
void DrawKick(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic, ALLEGRO_BITMAP *mackenziePicCrouch, int mackenzieWidth, int mackenzieHeight, ALLEGRO_BITMAP *bullet_blast, Bullet bullets[], int &mapoffx)
{
	if(currentFrame > 1)
	{
		currentFrame = 0;
	}

	if(mackenzie.isFacingRight)
	{
		if(mackenzie.timer != 0)
		{
			if(mackenzie.isCrouched)
			{
				al_draw_tinted_bitmap_region(mackenziePicCrouch, al_map_rgba_f(.6, .6, .6, .3), 0, mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
				al_draw_bitmap(bullet_blast, mackenzie.x + 45-mapoffx, mackenzie.y + 4, 0);
			}
			else
			{
				al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
				al_draw_bitmap(bullet_blast, mackenzie.x + 24-mapoffx, mackenzie.y + 28, 0);
			}

		}
		else
		{
			if(mackenzie.isCrouched)
			{
				al_draw_bitmap_region(mackenziePicCrouch, 0, mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
				al_draw_bitmap(bullet_blast, mackenzie.x + 45-mapoffx, mackenzie.y + 4, 0);
			}
			else
			{
				al_draw_bitmap_region(mackenziePic, mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, 0);
				al_draw_bitmap(bullet_blast, mackenzie.x + 24-mapoffx, mackenzie.y + 28, 0);
			}

		}

	}
	else
	{
		if(mackenzie.timer != 0)
		{
			
			if(mackenzie.isCrouched)
			{
				al_draw_tinted_bitmap_region(mackenziePicCrouch, al_map_rgba_f(.6, .6, .6, .3), 0, mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
				al_draw_bitmap(bullet_blast, mackenzie.x - 16-mapoffx, mackenzie.y + 4, ALLEGRO_FLIP_HORIZONTAL);
			}
			else
			{
				al_draw_tinted_bitmap_region(mackenziePic, al_map_rgba_f(.6, .6, .6, .3), mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
				al_draw_bitmap(bullet_blast, mackenzie.x - 26-mapoffx, mackenzie.y + 28, ALLEGRO_FLIP_HORIZONTAL);
			}

		}
		else
		{
			
			if(mackenzie.isCrouched)
			{
				al_draw_bitmap_region(mackenziePicCrouch, 0, mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
				al_draw_bitmap(bullet_blast, mackenzie.x - 16-mapoffx, mackenzie.y + 4, ALLEGRO_FLIP_HORIZONTAL);
			}
			else
			{
				al_draw_bitmap_region(mackenziePic, mackenzieWidth, mackenzieHeight, mackenzieWidth, mackenzieHeight, mackenzie.x -mapoffx- 1, mackenzie.y, ALLEGRO_FLIP_HORIZONTAL);
				al_draw_bitmap(bullet_blast, mackenzie.x - 26-mapoffx, mackenzie.y + 28, ALLEGRO_FLIP_HORIZONTAL);
			}

		}
	}

	if(frameDelayCount == 0)
	{
		currentFrame++;
		if(currentFrame > 1)
		{
			currentFrame = 0;
			mackenzie.kicked = false;
		}
	}
}

void JumpMackenzie(Mackenzie &mackenzie, ALLEGRO_SAMPLE *jumpSound)
{
	if(mackenzie.timer < stun)
	{
		if(mackenzie.onGround || mackenzie.extraJump)
		{
			al_play_sample(jumpSound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
			mackenzie.vy = -6.5;
			mackenzie.onGround = false;
			mackenzie.extraJump = false;
			mackenzie.jumped = true;
		}
	}
}
void MoveMackenzieDown(Mackenzie &mackenzie)
{
	//purposefully empty
}
void MoveMackenzieLeft(Mackenzie &mackenzie)
{
	if(mackenzie.timer < stun)
	{
		mackenzie.isFacingRight = false;
		if(mackenzie.isCrouched)
		{
			mackenzie.vx = -1.3;
		}
		else
		{
			if(mackenzie.vx > 0)
			{
				mackenzie.vx -= mackenzie.dec;
			}
			else if(mackenzie.vx > -mackenzie.maxSpeed)
			{
				mackenzie.vx -= mackenzie.acc;
			}
			else
			{
				mackenzie.vx = -mackenzie.maxSpeed;
			}
		}
	}
}
void MoveMackenzieRight(Mackenzie &mackenzie)
{
	if(mackenzie.timer < stun)
	{
		mackenzie.isFacingRight = true;
		if(mackenzie.isCrouched)
		{
			mackenzie.vx = 1.3;
		}
		else
		{
			if(mackenzie.vx < 0)
			{
				mackenzie.vx += mackenzie.dec;
			}
			else if(mackenzie.vx < mackenzie.maxSpeed)
			{
				mackenzie.vx += mackenzie.acc;
			}
			else
			{
				mackenzie.vx = mackenzie.maxSpeed;
			}
		}
	}
}
void UpdateMackenzie(Mackenzie &mackenzie, int &mapoffx, int deadZone)
{
	if(mackenzie.isCrouched)
	{
		mackenzie.boundDown = 32;
		mackenzie.boundRight = 64;
	}
	else
	{
		mackenzie.boundDown = 64;
		mackenzie.boundRight = 32;
	}

	if(mackenzie.timer == 0)
	{
		mackenzie.invincible = false;
	}

	//can jump at different heights if release up earlier
	if(!keys[UP] && mackenzie.vy < -4)
	{
		mackenzie.vy = -4;
	}

	//add gravity to velocity
	mackenzie.vy += gravity;

	//max vel downword
	if(mackenzie.vy > 15)
	{
		mackenzie.vy = 15;
	}

	//kinetic friction only if you dont press left or right
	if(!(keys[LEFT] ^ keys[RIGHT]))
	{
		// higher value means less sliding
		float slideValue;
		if(difficult)
		{
			//classic
			slideValue = 1;
		}
		else
		{
			//normal
			slideValue = 3;
		}
		
		mackenzie.vx -= mackenzie.acc * slideValue * ((mackenzie.vx > 0) - (mackenzie.vx < 0));
	}

	//static friction
	if((mackenzie.vx < mackenzie.acc) && (mackenzie.vx > -mackenzie.acc))
	{
		mackenzie.vx = 0;
	}

	//moonwalk?
	if(keys[RIGHT] && keys[LEFT])
	{
		mackenzie.vx = -2;
	}

	//Update position
	mackenzie.x += mackenzie.vx;
	mackenzie.y += mackenzie.vy;

	if((mackenzie.x - mapoffx) > WIDTH - deadZone)
	{
		mapoffx = mackenzie.x - (WIDTH - deadZone);
	}
	else if((mackenzie.x - mapoffx) < deadZone)
	{
		mapoffx = mackenzie.x - deadZone;
	}
	if(mapoffx < 0)
	{
		mapoffx = 0;
	}
	if(mackenzie.x < 0)
	{
		mackenzie.x = 0;
	}
}

void InitBullet(Bullet bullets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		bullets[i].ID = BULLET;
		bullets[i].speed = 10;
		bullets[i].live = false;
	}
}
void DrawBullet(Bullet bullets[], int size, int &mapoffx)
{
	for(int i = 0; i < size; i++)
	{
		if(bullets[i].live)
		{
			al_draw_filled_circle(bullets[i].x - mapoffx, bullets[i].y, 2, al_map_rgb(255, 255, 255));
		}
	}
}
void FireBullet(Bullet bullets[], int size, Mackenzie &mackenzie, ALLEGRO_SAMPLE *gunShot)
{
	if(mackenzie.timer < stun)
	{
		for(int i = 0; i < size; i++)
		{
			if(!bullets[i].live)
			{
				if(mackenzie.isFacingRight)
				{
					if(mackenzie.isCrouched)
					{
						bullets[i].x = mackenzie.x + 64;
						bullets[i].y = mackenzie.y + 20;
					}
					else
					{
						bullets[i].x = mackenzie.x + 32;
						bullets[i].y = mackenzie.y + 45;
					}
					bullets[i].live = true;
					bullets[i].isMovingRight = true;
					al_play_sample(gunShot, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					break;
				}
				else
				{
					if(mackenzie.isCrouched)
					{
						bullets[i].x = mackenzie.x;
						bullets[i].y = mackenzie.y + 20;
					}
					else
					{
						bullets[i].x = mackenzie.x;
						bullets[i].y = mackenzie.y + 45;
					}
					bullets[i].live = true;
					bullets[i].isMovingRight = false;
					al_play_sample(gunShot, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					break;
				}
			}

		}
	}
}
void UpdateBullet(Bullet bullets[], int size, int &mapoffx)
{
	for(int i = 0; i < size; i++)
	{
		if(bullets[i].live)
		{
			if(bullets[i].isMovingRight)
			{
				bullets[i].x += bullets[i].speed;
				if(bullets[i].x > WIDTH+mapoffx)
				{
					bullets[i].live = false;
				}
			}
			else
			{
				bullets[i].x -= bullets[i].speed;
				if(bullets[i].x < mapoffx)
				{
					bullets[i].live = false;
				}
			}
		}
	}
}
void CollideBullet(Bullet bullets[], int bSize, Enemy enemies[], int eSize, Brick bricks[], int brSize, ALLEGRO_SAMPLE *ghost_death, ALLEGRO_SAMPLE *explosion)
{
	for(int i = 0; i < bSize; i++)
	{
		if(bullets[i].live)
		{
			for(int j = 0; j < eSize; j++)
			{
				if(enemies[j].live)
				{
					if(bullets[i].x > (enemies[j].x + enemies[j].boundLeft) &&
						bullets[i].x < (enemies[j].x + enemies[j].boundRight) &&
						bullets[i].y > (enemies[j].y + enemies[j].boundUp) &&
						bullets[i].y < (enemies[j].y + enemies[j].boundDown))
					{
						if(enemies[j].ID != BOSS)
						{
							bullets[i].live = false;
							enemies[j].live = false;
							if(enemies[j].ID == GHOST)
							{
								al_play_sample(ghost_death, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
								score += 100;
							}
							else if(enemies[j].ID == BOMB)
							{
								al_play_sample(explosion, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
								score += 100;
							}
						}
						else
						{
							bossHitCount = 0;
							bullets[i].live = false;
							enemies[j].health--;
						}
					}
				}
			}
			for(int j = 0; j < brSize; j++)
			{
				if(bricks[j].ID != WATER && bricks[j].ID != JUMP && bricks[j].ID != LEVEL)
				{
					if(bullets[i].x > bricks[j].x &&
						bullets[i].x < (bricks[j].x + bricks[j].size) &&
						bullets[i].y > bricks[j].y &&
						bullets[i].y < (bricks[j].y + bricks[j].size))
					{
						bullets[i].live = false;
					}
				}
			}
		}
	}
}
void InitEnemy(Enemy enemies[], int size, string levelEnemies, int &deadZone)
{
	for(int i = 0; i < size; i++)
	{
		enemies[i].boundDown = 0;
		enemies[i].boundLeft = 0;
		enemies[i].boundUp = 0;
		enemies[i].boundDown = 0;
		enemies[i].ID = GHOST;
		enemies[i].live = false;
		enemies[i].speed = 0;
		enemies[i].x = -50;
		enemies[i].y = -50;
	}
	string line;
	int yOffset = 0;
	int enemyInc = 0;
	bool bossFight = false;
	ifstream file (levelEnemies);
	if (file.is_open())
	{
		while (file.good() && !file.eof())
		{
			getline (file, line);
			const char *lineArray = line.c_str();
			for(std::size_t i = 0; i < line.size(); i++)
			{
				enemies[enemyInc].x = i*32;
				enemies[enemyInc].y = yOffset;
				enemies[enemyInc].live = true;
				if(lineArray[i] == 'o')
				{
					enemies[enemyInc].ID = BOMB;
					enemies[enemyInc].x += 0;
					enemies[enemyInc].y += 0;
					enemies[enemyInc].boundDown = 24;
					enemies[enemyInc].boundUp = 8;
					enemies[enemyInc].boundLeft = 8;
					enemies[enemyInc].boundRight = 24;
					enemyInc++;
				}
				else if(lineArray[i] == 'g')
				{
					enemies[enemyInc].ID = GHOST;
					enemies[enemyInc].boundDown = 31;
					enemies[enemyInc].boundUp = -1;
					enemies[enemyInc].boundLeft = -1;
					enemies[enemyInc].boundRight = 31;
					enemies[enemyInc].speed = 3;
					enemyInc++;
				}
				else if(lineArray[i] == 'u')
				{
					bossFight = true;
					enemies[enemyInc].ID = BOSS;
					enemies[enemyInc].boundDown = 256;
					enemies[enemyInc].boundUp = 0;
					enemies[enemyInc].boundLeft =10;
					enemies[enemyInc].boundRight = 150;
					enemies[enemyInc].timer = 0;
					enemies[enemyInc].speed = 0;
					enemies[enemyInc].speedy = 0;
					enemies[enemyInc].health = 96;
					enemies[enemyInc].attackCounter = 0;
					enemyInc++;
				}
			}
			yOffset += 32;
		}
		file.close();
	}
	if(bossFight)
	{
		deadZone = 0;
	}
	else
	{
		deadZone = 350;
	}
}
void DrawEnemy(Enemy enemies[], int size, ALLEGRO_BITMAP *ghost, int &mapoffx, ALLEGRO_BITMAP *bomb, ALLEGRO_BITMAP *boss, ALLEGRO_BITMAP *bossgrumpy, ALLEGRO_BITMAP *fish, int currentLevel)
{
	for(int i = 0; i < size; i++)
	{
		if(enemies[i].live)
		{
			if(enemies[i].ID == GHOST)
			{
				if(currentLevel == 5)
				{
					al_draw_bitmap(fish, enemies[i].x - mapoffx, enemies[i].y, 0);
				}
				else
				{
					al_draw_bitmap(ghost, enemies[i].x - mapoffx, enemies[i].y, 0);
				}
			}
			else if(enemies[i].ID == BOMB)
			{
				al_draw_bitmap(bomb, enemies[i].x - mapoffx + enemies[i].boundLeft, enemies[i].y + enemies[i].boundUp, 0);
			}
			else if(enemies[i].ID == BOSS)
			{
				if(bossHitCount < 10)
				{
					al_draw_bitmap(bossgrumpy, enemies[i].x - mapoffx, enemies[i].y, ALLEGRO_FLIP_HORIZONTAL);
					al_draw_filled_rectangle(enemies[i].x - mapoffx, enemies[i].y + enemies[i].boundUp,
						enemies[i].x - mapoffx + enemies[i].health*2, enemies[i].y + enemies[i].boundUp + 10, al_map_rgb(0, 255, 0));
					al_draw_filled_rectangle(enemies[i].x - mapoffx + enemies[i].health*2, enemies[i].y + enemies[i].boundUp,
						enemies[i].x - mapoffx + 192, enemies[i].y + enemies[i].boundUp + 10, al_map_rgb(255, 0, 0));
				}
				else
				{
					al_draw_bitmap(boss, enemies[i].x - mapoffx, enemies[i].y, ALLEGRO_FLIP_HORIZONTAL);
					al_draw_filled_rectangle(enemies[i].x - mapoffx, enemies[i].y + enemies[i].boundUp,
						enemies[i].x - mapoffx + enemies[i].health*2, enemies[i].y + enemies[i].boundUp + 10, al_map_rgb(0, 255, 0));
					al_draw_filled_rectangle(enemies[i].x - mapoffx + enemies[i].health*2, enemies[i].y + enemies[i].boundUp,
						enemies[i].x - mapoffx + 192, enemies[i].y + enemies[i].boundUp + 10, al_map_rgb(255, 0, 0));
				}
				if(bossHitCount < 10)
				{
					bossHitCount++;
				}
			}
		}
	}
}
void StartEnemy(Enemy enemies[], int size, int &mapoffx, EBullet ebullets[], int bSize, Mackenzie &mackenzie, ALLEGRO_FONT *font64, Brick bricks[], int brSize)
{
	for(int i = 0; i < size; i++)
	{
		if(enemies[i].x + enemies[i].boundLeft -mapoffx < WIDTH && enemies[i].live)
		{
			UpdateEnemy(enemies[i], ebullets, bSize, mackenzie, font64, bricks, brSize);
		}
	}
}
void UpdateEnemy(Enemy &enemy, EBullet ebullets[], int size, Mackenzie &mackenzie, ALLEGRO_FONT *font64, Brick bricks[], int brSize)
{
	if(enemy.x + enemy.boundRight < 0 || enemy.y + enemy.boundDown < 0 || enemy.y + enemy.boundUp > HEIGHT)
	{
		//if go off the screen to left at beginning of level, above the level, or below the level then kill them
		enemy.live = false;
	}
	if(enemy.ID == GHOST)
	{
		enemy.x -= enemy.speed;
	}
	else if(enemy.ID == BOSS)
	{
		if(enemy.health == 0)
		{
			int i;
			bricks[brSize-1].ID = FINISH;
			bricks[brSize-1].size = 32;
			bricks[brSize-1].x = enemy.x + enemy.boundRight/2;
			bricks[brSize-1].y = enemy.y + enemy.boundDown/2;
			for(i = 2; i < 10; i++)
			{
				bricks[brSize-i].ID = JUMP;
				bricks[brSize-i].size = 8;
				bricks[brSize-i].timer = 0;
				bricks[brSize-i].x = enemy.x + enemy.boundRight/2;
				bricks[brSize-i].y = enemy.y + enemy.boundDown/2 + i*32;
				bricks[brSize-i].xstart = enemy.x + enemy.boundRight/2;
				bricks[brSize-i].ystart = enemy.y + enemy.boundDown/2 + i*32;
			}
			enemy.live = false;
		}
		else
		{
		enemy.timer++;
		attack(mackenzie, enemy, ebullets, size);
		enemy.x += enemy.speed;
		enemy.y += enemy.speedy;
		}
	}
}
void CollideEnemy(Enemy enemies[], int size, Mackenzie &mackenzie, ALLEGRO_SAMPLE *ghost_death, ALLEGRO_SAMPLE *mackenzie_hit)
{
	for(int i = 0; i < size; i++)
	{
		if(enemies[i].live)
		{
			if(enemies[i].x + enemies[i].boundLeft < mackenzie.x + mackenzie.boundRight &&
				enemies[i].x + enemies[i].boundRight > mackenzie.x + mackenzie.boundLeft &&
				enemies[i].y + enemies[i].boundUp < mackenzie.y + mackenzie.boundDown &&
				enemies[i].y + enemies[i].boundDown > mackenzie.y + mackenzie.boundUp && !mackenzie.invincible)
			{
					
				if(enemies[i].ID != BOSS)
				{
					enemies[i].live = false;
				}
				hitMackenzie(mackenzie, mackenzie_hit);
				if(enemies[i].ID == GHOST)
				{
					al_play_sample(ghost_death, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
			}
		}
	}
}

void InitBrick(Brick bricks[], int size, string level, Mackenzie &mackenzie)
{
	for(int i = 0; i < size; i++)
	{
		bricks[i].ID = BRICK;
		bricks[i].level = 0;
		bricks[i].size = 0;
		bricks[i].timer = 0;
		bricks[i].x = 0;
		bricks[i].xstart = 0;
		bricks[i].y = 0;
		bricks[i].ystart = 0;
	}
	string line;
	int yOffset = 0;
	int brickInc = 0;
	int levelInc = 1;
	ifstream file (level);
	if (file.is_open())
	{
		while (file.good() && !file.eof())
		{
			getline (file, line);
			const char *lineArray = line.c_str();
			for(std::size_t i = 0; i < line.size(); i++){
				if(lineArray[i] == 'm')
				{
					mackenzie.x = i*32;
					mackenzie.y = yOffset;
				}
				if(lineArray[i] != ' ' && lineArray[i] != 'm')
				{
					bricks[brickInc].size = 32;
					bricks[brickInc].x = i*32;
					bricks[brickInc].y = yOffset;
					if(lineArray[i] == 'b')
					{
						bricks[brickInc].ID = BRICK;
					}
					else if(lineArray[i] == 's')
					{
						bricks[brickInc].ID = SPRING;
					}
					else if(lineArray[i] == 'w')
					{
						bricks[brickInc].ID = WATER;
					}
					else if(lineArray[i] == 'j')
					{
						bricks[brickInc].ID = JUMP;
						bricks[brickInc].size = 8; //if changed from 8 than must change collision detection
						bricks[brickInc].timer = 0;
						bricks[brickInc].xstart = bricks[brickInc].x;
						bricks[brickInc].ystart = bricks[brickInc].y;
					}
					else if(lineArray[i] == 'c')
					{
						bricks[brickInc].ID = CANNON;
						bricks[brickInc].timer = 3;
					}
					else if(lineArray[i] == 'l')
					{
						bricks[brickInc].ID = LEVEL;
						bricks[brickInc].level = levelInc;
						levelInc++;
					}
					else if(lineArray[i] == 'f')
					{
						bricks[brickInc].ID = FINISH;
					}
					else if(lineArray[i] == 'e')
					{
						bricks[brickInc].ID = SEAWEED;
					}
					else if (lineArray[i] == 't')
					{
						bricks[brickInc].ID = CUTSCENE;
					}
					brickInc++;
				}
			}
			yOffset += 32;
		}
		file.close();
	}

}
void DrawBrick(Brick bricks[], int size, ALLEGRO_BITMAP *platform, ALLEGRO_BITMAP *spring, int &mapoffx, ALLEGRO_BITMAP *jump_pickup, ALLEGRO_FONT *size32, int unlocked, ALLEGRO_BITMAP *eyeball, Mackenzie &mackenzie, ALLEGRO_BITMAP *seaweed)
{
	for(int i = 0; i < size; i++)
	{
		if(IsBrickOnScreen(bricks, i, mapoffx))
		{
			if(bricks[i].ID == BRICK)
			{
				al_draw_bitmap(platform, bricks[i].x - mapoffx, bricks[i].y, 0);
			}
			else if(bricks[i].ID == SPRING)
			{
				al_draw_bitmap(spring, bricks[i].x - mapoffx, bricks[i].y, 0);
			}
			else if(bricks[i].ID == WATER)
			{
				al_draw_filled_rectangle(bricks[i].x - mapoffx, bricks[i].y, bricks[i].x + bricks[i].size - mapoffx,
					bricks[i].y + bricks[i].size, al_map_rgba_f(.1, .3, .7, .5));
			}
			else if(bricks[i].ID == JUMP)
			{
				al_draw_bitmap(jump_pickup, bricks[i].x - mapoffx + bricks[i].size, bricks[i].y + bricks[i].size, 0);
			}
			else if(bricks[i].ID == CANNON)
			{
				float mxcenter = mackenzie.x + mackenzie.boundLeft/2 + mackenzie.boundRight/2;
				float mycenter = mackenzie.y + mackenzie.boundDown/2 + mackenzie.boundUp/2;
				float deltax = bricks[i].x + bricks[i].size/2 - mxcenter;
				float deltay = bricks[i].y + bricks[i].size/2 - mycenter;
				float angle = atan2f(deltay, deltax);
				al_draw_rotated_bitmap(eyeball, bricks[i].size/2, bricks[i].size/2, bricks[i].x - mapoffx + bricks[i].size/2, bricks[i].y + bricks[i].size/2, angle + ALLEGRO_PI/2, 0);
			}
			else if(bricks[i].ID == SEAWEED)
			{
				al_draw_filled_rectangle(bricks[i].x - mapoffx, bricks[i].y, bricks[i].x - mapoffx + bricks[i].size, bricks[i].y + bricks[i].size, al_map_rgba_f(.1, .3, .7, .5));
				al_draw_bitmap(seaweed, bricks[i].x - mapoffx, bricks[i].y, 0);
			}
			else if(bricks[i].ID == LEVEL)
			{
				char buffer [5];
				if(bricks[i].level <= unlocked)
				{
					al_draw_textf(size32, al_map_rgb(10, 200, 10), bricks[i].x-mapoffx,
						bricks[i].y, 0, _itoa(bricks[i].level, buffer, 10));
				}
				else
				{
					al_draw_textf(size32, al_map_rgb(200, 10, 10), bricks[i].x-mapoffx,
						bricks[i].y, 0, _itoa(bricks[i].level, buffer, 10));
				}
			}
			else if(bricks[i].ID == FINISH)
			{
				al_draw_filled_rectangle(bricks[i].x - mapoffx, bricks[i].y, bricks[i].x + bricks[i].size - mapoffx,
					bricks[i].y + bricks[i].size, al_map_rgb(255, 255, 255));
				al_draw_filled_rectangle(bricks[i].x - mapoffx, bricks[i].y, bricks[i].x + 8 - mapoffx,
					bricks[i].y + 8, al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(bricks[i].x - mapoffx + 16, bricks[i].y, bricks[i].x + 24 - mapoffx,
					bricks[i].y + 8, al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(bricks[i].x - mapoffx + 8, bricks[i].y + 8, bricks[i].x + 16 - mapoffx,
					bricks[i].y + 16, al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(bricks[i].x - mapoffx + 24, bricks[i].y + 8, bricks[i].x + 32 - mapoffx,
					bricks[i].y + 16, al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(bricks[i].x - mapoffx, bricks[i].y + 16, bricks[i].x + 8 - mapoffx,
					bricks[i].y + 24, al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(bricks[i].x - mapoffx + 16, bricks[i].y + 16, bricks[i].x + 24 - mapoffx,
					bricks[i].y + 24, al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(bricks[i].x - mapoffx + 8, bricks[i].y + 24, bricks[i].x + 16 - mapoffx,
					bricks[i].y + 32, al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(bricks[i].x - mapoffx + 24, bricks[i].y + 24, bricks[i].x + 32 - mapoffx,
					bricks[i].y + 32, al_map_rgb(0, 0, 0));
			}
		}
	}
}
void UpdateBrick(Brick bricks[], int size, int &mapoffx, EBullet ebullets[], int ebsize, Mackenzie &mackenzie)
{
	for(int i = 0; i < size; i++)
	{
		if(bricks[i].ID == JUMP)
		{
			if(bricks[i].x == -32 && bricks[i].y == -32)
			{
				bricks[i].timer++;
			}
			if(bricks[i].timer > 6)
			{
				bricks[i].x = bricks[i].xstart;
				bricks[i].y = bricks[i].ystart;
				bricks[i].timer = 0;
			}
		}
		if(bricks[i].ID == CANNON)
		{
			bricks[i].timer++;
			if(bricks[i].timer > 4)
			{
				bricks[i].timer = 0;
				FireCannon(bricks[i], ebullets, ebsize, mackenzie);
			}
		}
	}
}
void CollideBrick(Brick bricks[], int size, Mackenzie &mackenzie, int &unlocked, ALLEGRO_SAMPLE *powerup, int &currentLevel, int menuOption)
{
	bool collided = false;
	for(int i = 0; i < size; i++)
	{
		if(bricks[i].ID == JUMP)
		{
			if(!((mackenzie.y + mackenzie.boundDown) < (bricks[i].y + bricks[i].size) ||
				(mackenzie.y + mackenzie.boundUp) > (bricks[i].y + bricks[i].size*3) ||
				(mackenzie.x + mackenzie.boundRight) < (bricks[i].x + bricks[i].size) ||
				(mackenzie.x + mackenzie.boundLeft) > (bricks[i].x + bricks[i].size*3))){
					mackenzie.extraJump = true;
					score += 100;
					al_play_sample(powerup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					bricks[i].x = -32;
					bricks[i].y = -32;
			}
		}
		else if (bricks[i].ID == CUTSCENE) 
		{
			//printf("%d", inCutscene);
			if (inCutscene < 0)
			{
				if (!((mackenzie.x + mackenzie.boundRight) < bricks[i].x ||
					  (mackenzie.x + mackenzie.boundLeft) > (bricks[i].x + bricks[i].size)))
				{
						  cutscene(bricks[i], currentLevel);
				}
			}
		}
		else
		{
			if(!((mackenzie.y + mackenzie.boundDown) < bricks[i].y ||
				(mackenzie.y + mackenzie.boundUp) > (bricks[i].y + bricks[i].size) ||
				(mackenzie.x + mackenzie.boundRight) < bricks[i].x ||
				(mackenzie.x + mackenzie.boundLeft) > (bricks[i].x + bricks[i].size)))
			{
				if(bricks[i].ID == LEVEL)
				{
					if(keys[SPACE] || keys[ENTER] || keys[DOWN])
					{
						if(bricks[i].level <= unlocked)
						{
							currentLevel = bricks[i].level;
							loadLevel(bricks[i].level);
						}
					}
				}
				else if(bricks[i].ID == FINISH)
				{
					if(difficult) {
						//classic
						if(unlocked == currentLevel)
						{
							unlocked++;
							string save;
							char buffer[5];
							if(menuOption == 1)
							{
								save = "saves/save1.txt";
							}
							else if(menuOption == 2)
							{
								save = "saves/save2.txt";
							}
							else if(menuOption == 3)
							{
								save = "saves/save3.txt";
							}
							ofstream file (save);
							if(file.is_open())
							{
								string saveContents = _itoa(unlocked, buffer, 10);
								saveContents += "\n";
								saveContents += _itoa(score, buffer, 10);
								saveContents += "\n";
								saveContents += _itoa(mackenzie.lives, buffer, 10);
								file << saveContents;
							}
						}

					} else {
						//normal
						if(unlocked == currentLevel)
						{
							unlocked++;
						}
						string save;
						char buffer[5];
						if(menuOption == 1)
						{
							save = "saves/save1.txt";
						}
						else if(menuOption == 2)
						{
							save = "saves/save2.txt";
						}
						else if(menuOption == 3)
						{
							save = "saves/save3.txt";
						}
						ofstream file (save);
						if(file.is_open())
						{
							string saveContents = _itoa(unlocked, buffer, 10);
							saveContents += "\n";
							saveContents += _itoa(score, buffer, 10);
							saveContents += "\n";
							saveContents += _itoa(mackenzie.lives, buffer, 10);
							file << saveContents;
						}
					}
					
					level = "levels/loadLevel.txt";
					levelEnemies = " ";
					levelBackground = "images/basicBackground.bmp";
					mackenzie.x = 64 + (unlocked - 1) * 192;
					mackenzie.y = 394;
					mackenzie.vx = 0;
					mackenzie.vy = 0;
					loadNewLevel = true;
				}
				else
				{
					if(difficult)
					{
						//classic
						if(bricks[i].ID == WATER)
						{
							if(mackenzie.isCrouched)
							{
								mackenzie.isCrouched = false;
								mackenzie.y -= 32;
								mackenzie.x += 14;
							}
						}
					}
					else
					{
						//normal
					}
					CollideBrickLeft(bricks, size, mackenzie);
					CollideBrickRight(bricks, size, mackenzie);
					CollideBrickUp(bricks, size, mackenzie);
					CollideBrickDown(bricks, size, mackenzie);
					collided = true;
				}
			}
		}
	}

	if(!collided)
	{
		mackenzie.onGround = false;
		
		// doesnt seem to change anything ??
		
		if(difficult)
		{
			//classic
			if(mackenzie.isCrouched)
			{
				mackenzie.isCrouched = false;
				mackenzie.y -= 32;
				mackenzie.x += 16;
			}
		}
		else
		{
			//normal
		}
	}

}
void CollideBrickLeft(Brick bricks[], int size, Mackenzie &mackenzie)
{
	for(int i = 0; i < size; i++)
	{
		if(bricks[i].x < mackenzie.x - 16)
		{
			if(bricks[i].x + bricks[i].size > mackenzie.x + mackenzie.boundLeft &&
				mackenzie.y + mackenzie.boundUp + 4 < bricks[i].y + bricks[i].size &&
				mackenzie.y + mackenzie.boundDown - 4 > bricks[i].y)
			{
				if(bricks[i].ID == WATER)
				{
					if(mackenzie.vx < -2)
					{
						mackenzie.vx = -2;
					}
					mackenzie.onGround = true;
				}
				else if(bricks[i].ID == LEVEL)
				{
					
				}
				else
				{
					if(!((bricks[i].x + bricks[i].size) == bricks[i+1].x
						&& bricks[i].y == bricks[i+1].y && bricks[i+1].ID != WATER))
					{
						mackenzie.x = bricks[i].x + bricks[i].size - mackenzie.boundLeft;
						if(mackenzie.vx < 0)
						{
							mackenzie.vx = 0;
						}
					}
				}
			}
		}
	}
}
void CollideBrickRight(Brick bricks[], int size, Mackenzie &mackenzie)
{
	for(int i = 0; i < size; i++)
	{
		if(bricks[i].x > mackenzie.x + 16)
		{
			if(bricks[i].x < mackenzie.x + mackenzie.boundRight &&
				mackenzie.y + mackenzie.boundUp + 4 < bricks[i].y + bricks[i].size &&
				mackenzie.y + mackenzie.boundDown - 4 > bricks[i].y)
			{
				if(bricks[i].ID == WATER)
				{
					if(mackenzie.vx > 2)
					{
						mackenzie.vx = 2;
					}
					mackenzie.onGround = true;
				}
				else if(bricks[i].ID == LEVEL)
				{
					
				}
				else
				{
					if(!(bricks[i].x == (bricks[i-1].x + bricks[i-1].size)
						&& bricks[i].y == bricks[i-1].y && bricks[i-1].ID != WATER))
					{
						mackenzie.x = bricks[i].x - mackenzie.boundRight;
						if(mackenzie.vx > 0)
						{
							mackenzie.vx = 0;
						}
					}
				}
			}
		}
	}
}
void CollideBrickUp(Brick bricks[], int size, Mackenzie &mackenzie)
{
	for(int i = 0; i < size; i++)
	{
		if(bricks[i].y < mackenzie.y - 16)
		{
			if(mackenzie.y + mackenzie.boundUp < bricks[i].y + bricks[i].size &&
				mackenzie.x < bricks[i].x + bricks[i].size &&
				mackenzie.x + mackenzie.boundRight > bricks[i].x)
			{
				if(bricks[i].ID == WATER)
				{
					if(mackenzie.vy < -3.3)
					{
						mackenzie.vy = -3.3;
					}
					mackenzie.onGround = true;
				}
				else
				{
					mackenzie.y = bricks[i].y + bricks[i].size - mackenzie.boundUp;
					if(mackenzie.vy < 0)
					{
						mackenzie.vy = 0;
					}
				}
			}
		}
	}
}
void CollideBrickDown(Brick bricks[], int size, Mackenzie &mackenzie)
{
	for(int i = 0; i < size; i++)
	{
		if(bricks[i].y > mackenzie.y + mackenzie.boundDown - 16)
		{
			if(bricks[i].y < mackenzie.y + mackenzie.boundDown &&
				mackenzie.x < bricks[i].x + bricks[i].size &&
				mackenzie.x + mackenzie.boundRight > bricks[i].x)
			{
				mackenzie.extraJump = false;
				if(bricks[i].ID == SPRING)
				{
					if(!(mackenzie.timer < stun))
					{
						mackenzie.vy = 0;
						mackenzie.y = bricks[i].y - mackenzie.boundDown;
					}
					else
					{
						mackenzie.vy = -10;
					}
					mackenzie.onGround = false;
					//no crouch while on spring
					if(mackenzie.isCrouched)
					{
						mackenzie.y -= 32;
						mackenzie.x += 14;
						mackenzie.isCrouched = false;
					}
				}
				else if(bricks[i].ID == WATER)
				{
					if(mackenzie.vy > 1.85)
					{
						mackenzie.vy = 1.85;
					}
					if(mackenzie.vy < -3.3)
					{
						mackenzie.vy = -3.3;
					}
					mackenzie.onGround = true;
				}
				else{
					mackenzie.y = bricks[i].y - mackenzie.boundDown;
					if(mackenzie.vy > 0)
					{
						mackenzie.vy = 0;
					}
					mackenzie.onGround = true;
				}
			}
		}
	}
}

bool IsBrickOnScreen(Brick bricks[], int brick, int &mapoffx)
{
	if(bricks[brick].ID == JUMP)
	{
		return (bricks[brick].x + bricks[brick].size*3-mapoffx > 0 &&
			bricks[brick].x-mapoffx + bricks[brick].size < WIDTH &&
			bricks[brick].y + bricks[brick].size < HEIGHT &&
			bricks[brick].y + bricks[brick].size*3 > 0);
	}
	return (bricks[brick].x + bricks[brick].size-mapoffx > 0 &&
		bricks[brick].x-mapoffx < WIDTH &&
		bricks[brick].y < HEIGHT &&
		bricks[brick].y + bricks[brick].size > 0);
}

void FireCannon(Brick cannon, EBullet ebullets[], int size, Mackenzie &mackenzie)
{
	for(int i = 0; i < size; i++)
	{
		if(!ebullets[i].live)
		{
			ebullets[i].live = true;
			ebullets[i].x = cannon.x + cannon.size/2;
			ebullets[i].y = cannon.y + cannon.size/2;
			float mxcenter = mackenzie.x + mackenzie.boundLeft/2 + mackenzie.boundRight/2;
			float mycenter = mackenzie.y + mackenzie.boundDown/2 + mackenzie.boundUp/2;
			float deltax = ebullets[i].x - mxcenter;
			float deltay = ebullets[i].y - mycenter;
			float angle = atan2f(deltay, deltax);
			ebullets[i].vx = cos(angle)*2;
			ebullets[i].vy = sin(angle)*2;
			break;
		}
	}
}
void InitEBullet(EBullet ebullets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		ebullets[i].ID = EBULLET;
		ebullets[i].live = false;
	}
}
void DrawEBullet(EBullet ebullets[], int size, int &mapoffx, ALLEGRO_BITMAP *lightning)
{
	for(int i = 0; i < size; i++)
	{
		if(ebullets[i].live)
		{
			if(ebullets[i].ID == LIGHTNING)
			{
				al_draw_bitmap(lightning, ebullets[i].x - mapoffx, ebullets[i].y, 0);
			}
			else
			{
				al_draw_filled_circle(ebullets[i].x - mapoffx, ebullets[i].y, 2, al_map_rgb(255, 10, 10));
			}
		}
	}
}
void UpdateEBullet(EBullet ebullets[], int size, int &mapoffx)
{
	for(int i = 0; i < size; i++)
	{
		if(ebullets[i].x > WIDTH+mapoffx || ebullets[i].x < mapoffx
			|| ebullets[i].y < 0 || ebullets[i].y > HEIGHT)
		{
			ebullets[i].live = false;
		}
		if(ebullets[i].live)
		{
			ebullets[i].x -= ebullets[i].vx;
			ebullets[i].y -= ebullets[i].vy;
		}
	}
}
void CollideEBullet(EBullet ebullets[], int ebsize, Mackenzie &mackenzie, Brick bricks[], int brsize, ALLEGRO_SAMPLE *mackenzie_hit)
{
	for(int i = 0; i < ebsize; i++)
	{
		if(ebullets[i].live)
		{
			for(int j = 0; j < brsize; j++)
			{
				if(bricks[j].ID != WATER && bricks[j].ID != JUMP && bricks[j].ID != CANNON)
				{
					if(ebullets[i].x > bricks[j].x &&
						ebullets[i].x < (bricks[j].x + bricks[j].size) &&
						ebullets[i].y > bricks[j].y &&
						ebullets[i].y < (bricks[j].y + bricks[j].size))
					{
						ebullets[i].live = false;
					}
				}
			}
			if(ebullets[i].ID == LIGHTNING)
			{
			
				if(ebullets[i].x + 32 > mackenzie.x + mackenzie.boundLeft &&
					ebullets[i].x < mackenzie.x + mackenzie.boundRight &&
					ebullets[i].y + 32 > mackenzie.y + mackenzie.boundUp &&
					ebullets[i].y < mackenzie.y + mackenzie.boundDown && !mackenzie.invincible)
			
				{
					ebullets[i].live = false;
					hitMackenzie(mackenzie, mackenzie_hit);
				}
			}
			else
			{
				if(ebullets[i].x > mackenzie.x + mackenzie.boundLeft &&
					ebullets[i].x < mackenzie.x + mackenzie.boundRight &&
					ebullets[i].y > mackenzie.y + mackenzie.boundUp &&
					ebullets[i].y < mackenzie.y + mackenzie.boundDown && !mackenzie.invincible)
				{
					ebullets[i].live = false;
					hitMackenzie(mackenzie, mackenzie_hit);
				}
			}
		}
	}
}

void frameDelayIncrementer()
{
	frameDelayCount++;
	if(frameDelayCount > FRAME_DELAY)
	{
		frameDelayCount = 0;
	}
}
void DrawBackground(ALLEGRO_BITMAP *back, Brick bricks[], int size, int &mapoffx)
{
	int imageWidth = al_get_bitmap_width(back);
	int levelWidth = 0;

	for(int i = 0; i < size; i++)
	{
		if(bricks[i].x > levelWidth)
		{
			levelWidth = bricks[i].x;
		}
	}

	int amount = levelWidth/imageWidth;

	for(int i = 0; i < (amount + 2); i++)
	{
		al_draw_bitmap(back, i*imageWidth - mapoffx, 0, 0);
	}
}
void hitMackenzie(Mackenzie &mackenzie, ALLEGRO_SAMPLE *mackenzie_hit)
{
	if(difficult)
	{
		//classic
		if(mackenzie.isCrouched)
		{
			mackenzie.isCrouched = false;
			mackenzie.y -= 32;
			mackenzie.x += 14;
			keys[DOWN] = false;
		}
	}
	else
	{
		//normal
	}
	mackenzie.invincible = true;
	mackenzie.timer = 4;
	mackenzie.vx = mackenzie.vx/3;
	mackenzie.vy = mackenzie.vy/3;
	mackenzie.lives--;
	al_play_sample(mackenzie_hit, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void loadLevel(int levelNumber)
{
	char buffer[5];
	level = "levels/Level";
	levelEnemies = "levels/LevelEnemies";
	levelBackground = "images/LevelBackground";
	level += _itoa(levelNumber, buffer, 10);
	levelEnemies += _itoa(levelNumber, buffer, 10);
	levelBackground += _itoa(levelNumber, buffer, 10);
	if(difficult)
	{
		//classic
		level += ".txt";
		levelEnemies += ".txt";
	}
	else
	{
		//normal
		level += "new.txt";
		levelEnemies += "new.txt";
	}
	levelBackground += ".bmp";
	loadNewLevel = true;
}

void attack(Mackenzie &mackenzie, Enemy &enemy, EBullet ebullets[], int size)
{
	if(enemy.timer > 2450)
	{
		enemy.timer = 0;
		enemy.speed = 0;
		enemy.speedy = 0;
		enemy.attackCounter = 0;
	}
	else if(enemy.timer > 2300)
	{
		if(enemy.x > 605)
		{
			enemy.speed = -2;
		}
		else if(enemy.x < 595)
		{
			enemy.speed = 2;
		}
		else
		{
			enemy.speed = 0;
		}
		if(enemy.y > 105)
		{
			enemy.speedy = -2;
		}
		else if(enemy.y < 95){
			enemy.speedy = 2;
		}
		else
		{
			enemy.speedy = 0;
		}
	}
	else if(enemy.timer > 1550)
	{
		enemy.attackCounter++;
		if(enemy.timer%15 == 0)
		{
			for(int i = 0; i < size; i++)
			{
				if(!ebullets[i].live)
				{
					ebullets[i].live = true;
					ebullets[i].ID = EBULLET;
					ebullets[i].x = enemy.x + enemy.boundRight/2;
					ebullets[i].y = enemy.y + enemy.boundDown/2;
					ebullets[i].vx = cos(enemy.attackCounter*(3.141592/180));
					ebullets[i].vy = -abs(sin(enemy.attackCounter*(3.151592/180)));
					break;
				}
			}
		}
	}
	else if(enemy.timer > 1400)
	{
		if(enemy.x > 305)
		{
			enemy.speed = -2;
		}
		else if(enemy.x < 295)
		{
			enemy.speed = 2;
		}
		else
		{
			enemy.speed = 0;
		}
		if(enemy.y > 5)
		{
			enemy.speedy = -2;
		}
		else if(enemy.y < 1)
		{
			enemy.speedy = 2;
		}
		else
		{
			enemy.speedy = 0;
		}
	}
	else if(enemy.timer > 550)
	{
		if(enemy.timer%552 == 0)
		{
		enemy.speed = -5;
		enemy.speedy = -3;
		}
		if(enemy.x >= 600)
		{
			enemy.speed = -5;
		}
		else if(enemy.x <= 1)
		{
			enemy.speed = 5;
		}
		if(enemy.y >= 100)
		{
			enemy.speedy = -1;
		}
		else if(enemy.y <= 0)
		{
			enemy.speedy = 1;
		}
		if(enemy.timer%30 == 0)
		{
			for(int i = 0; i < size; i++)
			{
				if(!ebullets[i].live)
				{
					ebullets[i].live = true;
					ebullets[i].ID = LIGHTNING;
					ebullets[i].x = enemy.x + enemy.boundRight/2 - 50;
					ebullets[i].y = enemy.y + enemy.boundDown/2;
					ebullets[i].vx = sin(ebullets[i].y)*3;
					ebullets[i].vy = -2;
					break;
				}
			}
		}

	}
	else if(enemy.timer > 300)
	{

	}
	else
	{
		if(enemy.timer%5 == 0)
		{
			for(int i = 0; i < size; i++)
			{
				if(!ebullets[i].live)
				{
					ebullets[i].live = true;
					ebullets[i].x = enemy.x + enemy.boundRight/2;
					ebullets[i].y = enemy.y + enemy.boundDown/2;
					float mxcenter = mackenzie.x + mackenzie.boundLeft/2 + mackenzie.boundRight/2;
					float mycenter = mackenzie.y + mackenzie.boundDown/2 + mackenzie.boundUp/2;
					float deltax = ebullets[i].x - mxcenter;
					float deltay = ebullets[i].y - mycenter;
					float angle = atan2f(deltay, deltax);
					ebullets[i].vx = cos(angle)*2;
					ebullets[i].vy = sin(angle)*2;
					break;
				}
			}
		}
	}
}
void cutscene(Brick brick, int currentLevel)
{
	string scene = "levels/testScene.txt";
	switch(currentLevel)
	{
	case 0:
		scene = "levels/cutscene0.txt";
		break;
	case 1:
		scene = "levels/cutscene1.txt";
		break;
	case 2:
		scene = "levels/cutscene2.txt";
		break;
	case 3:
		scene = "levels/cutscene3.txt";
		break;
	case 4:
		scene = "levels/cutscene4.txt";
		break;
	case 5:
		scene = "levels/cutscene5.txt";
		break;
	case 6:
		scene = "levels/cutscene6.txt";
		break;
	case 7:
		scene = "levels/cutscene7.txt";
		break;
	}
	cutsceneFile.open(scene);
	if (cutsceneFile.is_open())
	{
		inCutscene = 0;
		nextScenePage();
	}
	else
	{
		inCutscene = -1;
	}
}
void nextScenePage()
{
	if (cutsceneFile.good() && !cutsceneFile.eof())
	{
		getline (cutsceneFile, cutline);
	}
	else
	{
		cutsceneFile.close();
		inCutscene = 1;
		cutline = "";
	}
}