#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"
#include <math.h>
#include <allegro5\allegro_image.h>
#include <stdio.h>

enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false, false};
const int WIDTH = 640;
const int HEIGHT = 480;
const int NUM_BULLETS = 10;
const int NUM_ENEMIES = 10;
const int NUM_BRICKS = 200;
const float GRAVITY = .175;

void InitMackenzie(Mackenzie &mackenzie);
void DrawMackenzie(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic);
void JumpMackenzie(Mackenzie &mackenzie);
void MoveMackenzieDown(Mackenzie &mackenzie);
void MoveMackenzieLeft(Mackenzie &mackenzie);
void MoveMackenzieRight(Mackenzie &mackenzie);
void UpdateMackenzie(Mackenzie &mackenzie);

void InitBullet(Bullet bullets[], int size);
void DrawBullet(Bullet bullets[], int size);
void FireBullet(Bullet bullets[], int size, Mackenzie &mackenzie);
void UpdateBullet(Bullet bullets[], int size);
void CollideBullet(Bullet bullets[], int bSize, Enemy enemies[], int eSize);

void InitEnemy(Enemy enemies[], int size);
void DrawEnemy(Enemy enemies[], int size, ALLEGRO_BITMAP *ghost);
void StartEnemy(Enemy enemies[], int size);
void UpdateEnemy(Enemy enemies[], int size);
void CollideEnemy(Enemy enemies[], int size, Mackenzie &mackenzie);

void InitBrick(Brick bricks[], int size);
void DrawBrick(Brick bricks[], int size, ALLEGRO_BITMAP *platform);
void UpdateBrick(Brick bricks[], int size);
void CollideBrick(Brick bricks[], int size, Mackenzie &mackenzie);
void CollideBrickLeft(Brick bricks[], int size, Mackenzie &mackenzie);
void CollideBrickRight(Brick bricks[], int size, Mackenzie &mackenzie);
void CollideBrickUp(Brick bricks[], int size, Mackenzie &mackenzie);
void CollideBrickDown(Brick bricks[], int size, Mackenzie &mackenzie);
bool IsBrickOnScreen(Brick bricks[], int brick);

int main ()
{
	bool done = false;
	bool redraw = true;

	ALLEGRO_BITMAP *back = NULL;
	ALLEGRO_BITMAP *platform = NULL;
	ALLEGRO_BITMAP *ghost = NULL;
	ALLEGRO_BITMAP *mackenziePic = NULL;

	int FPS = 60;
	int frames = 0;
	int currentTime = 0;
	int lastUpdateTime = 0;
	int elapsedTime = 0;
	int actualFPS = 0;

	Mackenzie mackenzie;
	Bullet bullets[NUM_BULLETS];
	Enemy enemies[NUM_ENEMIES];
	Brick bricks[NUM_BRICKS];

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;


	if(!al_init())
		return -1;
	display = al_create_display(WIDTH, HEIGHT);
	if(!display)
		return -1;
	timer = al_create_timer(1.0/FPS);


	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 18, 0);
	event_queue = al_create_event_queue();
	ALLEGRO_FONT *font64 = al_load_font("arial.ttf", 64, 0);
	event_queue = al_create_event_queue();

	back = al_load_bitmap("basicBackground2.bmp");
	platform = al_load_bitmap("brick1.bmp");
	ghost = al_load_bitmap("ghost.bmp");
	mackenziePic = al_load_bitmap("mackenzie.bmp");

	int imageWidth = al_get_bitmap_width(back);
	int imageHeight = al_get_bitmap_height(back);


	srand(time(NULL));
	InitMackenzie(mackenzie);
	InitBullet(bullets, NUM_BULLETS);
	InitEnemy(enemies, NUM_ENEMIES);
	InitBrick(bricks, NUM_BRICKS);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_start_timer(timer);

	al_convert_mask_to_alpha(ghost, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(mackenziePic, al_map_rgb(255, 0, 255));

	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_UP:
				JumpMackenzie(mackenzie);
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
				FireBullet(bullets, NUM_BULLETS, mackenzie);
				break;
			case ALLEGRO_KEY_R:
				mackenzie.x = 50;
				mackenzie.y = 50;
				mackenzie.vx = 0;
				mackenzie.vy = 0;
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP){
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
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}

		else if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;
			if(keys[DOWN])
				MoveMackenzieDown(mackenzie);
			if(keys[LEFT] && !keys[RIGHT])
				MoveMackenzieLeft(mackenzie);
			if(keys[RIGHT] && !keys[LEFT])
				MoveMackenzieRight(mackenzie);
		}

		if(redraw && (al_is_event_queue_empty(event_queue)))
		{
			redraw = false;

			//begin calculate and display fps timer
			frames++;
			currentTime = clock();
			elapsedTime = currentTime - lastUpdateTime;

			if(elapsedTime >= 200)
			{
				actualFPS = (frames * 1000 / elapsedTime);
				frames = 0;
				lastUpdateTime = currentTime;
			}
			//end calculate and display fps timer


			//start drawing and updating everything
			UpdateBullet(bullets, NUM_BULLETS);
			StartEnemy(enemies, NUM_ENEMIES);
			UpdateEnemy(enemies, NUM_ENEMIES);
			UpdateMackenzie(mackenzie);
			CollideBullet(bullets, NUM_BULLETS, enemies, NUM_ENEMIES);
			CollideEnemy(enemies, NUM_ENEMIES, mackenzie);
			CollideBrick(bricks, NUM_BRICKS,  mackenzie);

			al_draw_bitmap(back, WIDTH / 2 - imageWidth / 2, HEIGHT / 2 - imageHeight / 2, 0);
			DrawBrick(bricks, NUM_BRICKS, platform);
			DrawMackenzie(mackenzie, mackenziePic);
			DrawBullet(bullets, NUM_BULLETS);
			DrawEnemy(enemies, NUM_ENEMIES, ghost);
			if(mackenzie.y > HEIGHT){
				al_draw_textf(font64, al_map_rgb(255, 255, 255), 100, HEIGHT/2, 
					0, "GAME OVER");
			}
			al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH, 0, 
				ALLEGRO_ALIGN_RIGHT, "FPS : %i", actualFPS);
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
			//end drawing and updating everything
		}
	}


	al_destroy_display(display);
	return 0;

}

void InitMackenzie(Mackenzie &mackenzie){
	mackenzie.x = 50;
	mackenzie.y = 50;
	mackenzie.ID = PLAYER;
	mackenzie.lives = 5;
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
	mackenzie.score = 0;
}
void DrawMackenzie(Mackenzie &mackenzie, ALLEGRO_BITMAP *mackenziePic){
	al_draw_bitmap(mackenziePic, mackenzie.x-1, mackenzie.y-1, 0);
}
void JumpMackenzie(Mackenzie &mackenzie){
	if(mackenzie.onGround && !(mackenzie.vy > 0)){
		mackenzie.vy = -6.5;
		mackenzie.onGround = false;
	}
}
void MoveMackenzieDown(Mackenzie &mackenzie){
	//to be implemented
}
void MoveMackenzieLeft(Mackenzie &mackenzie){
	if(mackenzie.vx > 0){
		mackenzie.vx -= mackenzie.dec;
	}
	else if(mackenzie.vx > -mackenzie.maxSpeed){
		mackenzie.vx -= mackenzie.acc;
	}
	else
		mackenzie.vx = -mackenzie.maxSpeed;
}
void MoveMackenzieRight(Mackenzie &mackenzie){
	if(mackenzie.vx < 0){
		mackenzie.vx += mackenzie.dec;
	}
	else if(mackenzie.vx < mackenzie.maxSpeed){
		mackenzie.vx += mackenzie.acc;
	}
	else
		mackenzie.vx = mackenzie.maxSpeed;
}
void UpdateMackenzie(Mackenzie &mackenzie){

	//can jump at different heights if release up earlier
	if(!keys[UP] && mackenzie.vy < -4)
		mackenzie.vy = -4;

	//add gravity to velocity
	mackenzie.vy += GRAVITY;

	//max vel downword
	if(mackenzie.vy > 15)
		mackenzie.vy = 15;

	//kinetic friction only if you dont press left or right
	if(!(keys[LEFT] ^ keys[RIGHT])){
		mackenzie.vx -= mackenzie.acc * ((mackenzie.vx > 0) - (mackenzie.vx < 0));
	}

	//static friction
	if(mackenzie.vx < mackenzie.acc && mackenzie.vx > -mackenzie.acc)
		mackenzie.vx = 0;

	mackenzie.x += mackenzie.vx;
	mackenzie.y += mackenzie.vy;
}


void InitBullet(Bullet bullets[], int size){
	for(int i = 0; i < size; i++)
	{
		bullets[i].ID = BULLET;
		bullets[i].speed = 10;
		bullets[i].live = false;
	}
}
void DrawBullet(Bullet bullets[], int size){
	for(int i = 0; i < size; i++){
		if(bullets[i].live){
			al_draw_filled_circle(bullets[i].x, bullets[i].y, 2, al_map_rgb(255, 255, 255));
		}
	}
}
void FireBullet(Bullet bullets[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(!bullets[i].live){
			bullets[i].x = mackenzie.x + 42;
			bullets[i].y = mackenzie.y + 20;
			bullets[i].live = true;
			break;
		}
	}
}
void UpdateBullet(Bullet bullets[], int size){
	for(int i = 0; i < size; i++){
		if(bullets[i].live){
			bullets[i].x += bullets[i].speed;
			if(bullets[i].x > WIDTH)
				bullets[i].live = false;
		}
	}
}
void CollideBullet(Bullet bullets[], int bSize, Enemy enemies[], int eSize){
	for(int i = 0; i < bSize; i++){
		if(bullets[i].live){
			for(int j = 0; j < eSize; j++){
				if(enemies[j].live){
					if(bullets[i].x > (enemies[j].x + enemies[j].boundLeft) &&
						bullets[i].x < (enemies[j].x + enemies[j].boundRight) &&
						bullets[i].y > (enemies[j].y + enemies[j].boundUp) &&
						bullets[i].y < (enemies[j].y + enemies[j].boundDown))
					{
						bullets[i].live = false;
						enemies[j].live = false;
					}
				}
			}
		}
	}
}

void InitEnemy(Enemy enemies[], int size){
	for(int i = 0; i < size; i++){
		enemies[i].ID = ENEMY;
		enemies[i].live = false;
		enemies[i].speed = 5;
		enemies[i].boundLeft = 1;
		enemies[i].boundRight = 31;
		enemies[i].boundUp = 1;
		enemies[i].boundDown = 31;
	}
}
void DrawEnemy(Enemy enemies[], int size, ALLEGRO_BITMAP *ghost){
	for(int i = 0; i < size; i++){
		if(enemies[i].live){
			al_draw_bitmap(ghost, enemies[i].x, enemies[i].y, 0);
		}
	}
}
void StartEnemy(Enemy enemies[], int size){
	for(int i = 0; i < size; i++){
		if(!enemies[i].live){
			if(rand() % 500 == 0){
				enemies[i].live = true;
				enemies[i].x = WIDTH;
				enemies[i].y = 30 + rand() % (HEIGHT - 60);

				break;
			}
		}
	}
}
void UpdateEnemy(Enemy enemies[], int size){
	for(int i = 0; i < size; i++){
		if(enemies[i].live){
			enemies[i].x -= enemies[i].speed;
		}
	}
}
void CollideEnemy(Enemy enemies[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(enemies[i].live){
			if(enemies[i].x + enemies[i].boundLeft < mackenzie.x + mackenzie.boundRight &&
				enemies[i].x + enemies[i].boundRight > mackenzie.x + mackenzie.boundLeft &&
				enemies[i].y + enemies[i].boundUp < mackenzie.y + mackenzie.boundDown &&
				enemies[i].y + enemies[i].boundDown > mackenzie.y + mackenzie.boundUp){
					mackenzie.lives--;
					enemies[i].live = false;
			}
			else if(enemies[i].x < 0){
				enemies[i].live = false;
			}
		}
	}
}

void InitBrick(Brick bricks[], int size){
	for(int i = 0; i < size; i++){
		bricks[i].ID = BRICK;
		bricks[i].size = 32;
	}

	bricks[0].x = 330;
	bricks[0].y = 458;
	bricks[0].ID = SPRING;

	bricks[1].x = 0;
	bricks[1].y = 0;
	bricks[2].x = 32;
	bricks[2].y = 0;
	bricks[3].x = 64;
	bricks[3].y = 0;
	bricks[4].x = 96;
	bricks[4].y = 0;
	bricks[5].x = 512;
	bricks[5].y = 0;
	bricks[6].x = 544;
	bricks[6].y = 0;
	bricks[7].x = 576;
	bricks[7].y = 0;
	bricks[8].x = 608;
	bricks[8].y = 0;

	bricks[9].x = 0;
	bricks[9].y = 32;
	bricks[10].x = 256;
	bricks[10].y = 32;
	bricks[11].x = 288;
	bricks[11].y = 32;
	bricks[65].x = 320;
	bricks[66].y = 32;
	bricks[12].x = 608;
	bricks[12].y = 32;

	bricks[13].x = 0;
	bricks[13].y = 64;
	bricks[15].x = 608;
	bricks[15].y = 64;

	bricks[16].x = 0;
	bricks[16].y = 96;
	bricks[17].x = 608;
	bricks[17].y = 96;

	bricks[18].x = 0;
	bricks[18].y = 128;
	bricks[19].x = 96;
	bricks[19].y = 128;
	bricks[20].x = 128;
	bricks[20].y = 128;
	bricks[21].x = 416;
	bricks[21].y = 128;
	bricks[22].x = 448;
	bricks[22].y = 128;
	bricks[23].x = 480;
	bricks[23].y = 128;
	bricks[24].x = 608;
	bricks[24].y = 128;

	bricks[25].x = 608;
	bricks[25].y = 160;
	bricks[26].x = 0;
	bricks[26].y = 160;
	bricks[27].x = 128;
	bricks[27].y = 160;

	bricks[28].x = 0;
	bricks[28].y = 192;
	bricks[29].x = 128;
	bricks[29].y = 192;
	bricks[30].x = 608;
	bricks[30].y = 192;

	bricks[31].x = 0;
	bricks[31].y = 224;
	bricks[32].x = 128;
	bricks[32].y = 224;
	bricks[33].x = 160;
	bricks[33].y = 224;
	bricks[34].x = 192;
	bricks[34].y = 224;
	bricks[35].x = 224;
	bricks[35].y = 224;
	bricks[36].x = 256;
	bricks[36].y = 224;
	bricks[37].x = 32;
	bricks[37].y = 224;
	bricks[38].x = 608;
	bricks[38].y = 224;

	bricks[39].x = 0;
	bricks[39].y = 256;
	bricks[40].x = 128;
	bricks[40].y = 256;
	bricks[41].x = 480;
	bricks[41].y = 256;
	bricks[42].x = 512;
	bricks[42].y = 256;
	bricks[43].x = 544;
	bricks[43].y = 256;
	bricks[44].x = 576;
	bricks[44].y = 256;
	bricks[45].x = 608;
	bricks[45].y = 256;

	bricks[46].x = 0;
	bricks[46].y = 288;
	bricks[47].x = 128;
	bricks[47].y = 288;
	bricks[48].x = 608;
	bricks[48].y = 288;

	bricks[49].x = 0;
	bricks[49].y = 320;
	bricks[50].x = 96;
	bricks[50].y = 320;
	bricks[51].x = 128;
	bricks[51].y = 320;
	bricks[52].x = 608;
	bricks[52].y = 320;

	bricks[53].x = 0;
	bricks[53].y = 352;
	bricks[54].x = 608;
	bricks[54].y = 352;

	bricks[55].x = 0;
	bricks[55].y = 384;
	bricks[56].x = 384;
	bricks[56].y = 384;
	bricks[57].x = 416;
	bricks[57].y = 384;
	bricks[58].x = 608;
	bricks[58].y = 384;

	bricks[59].x = 0;
	bricks[59].y = 416;
	bricks[60].x = 32;
	bricks[60].y = 416;
	bricks[61].x = 384;
	bricks[61].y = 416;
	bricks[62].x = 416;
	bricks[62].y = 416;
	bricks[63].x = 608;
	bricks[63].y = 416;

	bricks[64].x = 0;
	bricks[64].y = 448;
	bricks[65].x = 32;
	bricks[65].y = 448;
	bricks[66].x = 64;
	bricks[66].y = 448;
	bricks[67].x = 96;
	bricks[67].y = 448;
	bricks[68].x = 128;
	bricks[68].y = 448;
	bricks[69].x = 160;
	bricks[69].y = 448;
	bricks[70].x = 256;
	bricks[70].y = 448;
	bricks[71].x = 384;
	bricks[71].y = 448;
	bricks[72].x = 416;
	bricks[72].y = 448;
	bricks[73].x = 544;
	bricks[73].y = 448;
	bricks[74].x = 576;
	bricks[74].y = 448;
	bricks[75].x = 608;
	bricks[75].y = 448;

	bricks[76].x = 256;
	bricks[76].y = 256;
	bricks[77].x = 256;
	bricks[77].y = 288;
	bricks[78].x = 288;
	bricks[78].y = 288;

}
void DrawBrick(Brick bricks[], int size, ALLEGRO_BITMAP *platform){
	for(int i = 0; i < size; i++){
		if(IsBrickOnScreen(bricks, i)){
			if(bricks[i].ID == BRICK){
				al_draw_bitmap(platform, bricks[i].x, bricks[i].y, 0);
			}
			else if(bricks[i].ID == SPRING){
				al_draw_filled_rounded_rectangle(bricks[i].x, bricks[i].y, bricks[i].x + bricks[i].size,
					bricks[i].y + bricks[i].size, 5, 5, al_map_rgb(0, 255, 0));
			}
		}
	}
}

void UpdateBrick(Brick bricks[], int size){

}
void CollideBrick(Brick bricks[], int size, Mackenzie &mackenzie){

	for(int i = 0; i < size; i++){
		if(!((mackenzie.y + mackenzie.boundDown) < bricks[i].y ||
			(mackenzie.y + mackenzie.boundUp) > (bricks[i].y + bricks[i].size) ||
			(mackenzie.x + mackenzie.boundRight) < bricks[i].x ||
			(mackenzie.x + mackenzie.boundLeft) > (bricks[i].x + bricks[i].size))){
				CollideBrickLeft(bricks, size, mackenzie);
				CollideBrickRight(bricks, size, mackenzie);
				CollideBrickUp(bricks, size, mackenzie);
				CollideBrickDown(bricks, size, mackenzie);
		}
	}
}
void CollideBrickLeft(Brick bricks[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(bricks[i].x < mackenzie.x - 16){
			if(bricks[i].x + bricks[i].size > mackenzie.x + mackenzie.boundLeft &&
				mackenzie.y + 4 < bricks[i].y + bricks[i].size &&
				mackenzie.y + 60 > bricks[i].y){
					mackenzie.x = bricks[i].x + bricks[i].size - mackenzie.boundLeft;
					if(mackenzie.vx < 0)
						mackenzie.vx = 0;
			}
		}
	}
}
void CollideBrickRight(Brick bricks[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(bricks[i].x > mackenzie.x + 16){
			if(bricks[i].x < mackenzie.x + mackenzie.boundRight &&
				mackenzie.y + 4 < bricks[i].y + bricks[i].size &&
				mackenzie.y + 60 > bricks[i].y){
					mackenzie.x = bricks[i].x - mackenzie.boundRight;
					if(mackenzie.vx > 0)
						mackenzie.vx = 0;
			}
		}
	}
}
void CollideBrickUp(Brick bricks[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(bricks[i].y < mackenzie.y - 16){
			if(mackenzie.y + mackenzie.boundUp < bricks[i].y + bricks[i].size &&
				mackenzie.x < bricks[i].x + bricks[i].size &&
				mackenzie.x + 32 > bricks[i].x){
					mackenzie.y = bricks[i].y + bricks[i].size - mackenzie.boundUp;
					if(mackenzie.vy < 0)
						mackenzie.vy = 0;
			}
		}
	}
}
void CollideBrickDown(Brick bricks[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(bricks[i].y > mackenzie.y + 48){
			if(bricks[i].y < mackenzie.y + mackenzie.boundDown &&
				mackenzie.x < bricks[i].x + bricks[i].size &&
				mackenzie.x + 32 > bricks[i].x){
					mackenzie.y = bricks[i].y - mackenzie.boundDown;
					if(bricks[i].ID == BRICK){
						if(mackenzie.vy > 0)
							mackenzie.vy = 0;
						mackenzie.onGround = true;
					}
					if(bricks[i].ID == SPRING){
						mackenzie.vy = -10;
					}
			}
		}
	}
}

bool IsBrickOnScreen(Brick bricks[], int brick){
	return (bricks[brick].x + bricks[brick].size > 0 &&
		bricks[brick].x < WIDTH &&
		bricks[brick].y < HEIGHT &&
		bricks[brick].y + bricks[brick].size > 0);
}