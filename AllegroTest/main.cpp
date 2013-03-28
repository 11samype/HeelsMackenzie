#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"
#include <math.h>

enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false, false};
const int WIDTH = 640;
const int HEIGHT = 480;
const int NUM_BULLETS = 5;
const int NUM_ENEMIES = 10;
const int NUM_BRICKS = 192;
const float GRAVITY = .175;

void InitMackenzie(Mackenzie &mackenzie);
void DrawMackenzie(Mackenzie &mackenzie);
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
void DrawEnemy(Enemy enemies[], int size);
void StartEnemy(Enemy enemies[], int size);
void UpdateEnemy(Enemy enemies[], int size);
void CollideEnemy(Enemy enemies[], int size, Mackenzie &mackenzie);

void InitBrick(Brick bricks[], int size);
void DrawBrick(Brick bricks[], int size);
void UpdateBrick(Brick bricks[], int size);
void CollideBrick(Brick bricks[], int size, Mackenzie &mackenzie);
bool IsBrickOnScreen(Brick bricks[], int brick);

int main ()
{
	bool done = false;
	bool redraw = true;

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
	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 18, 0);
	event_queue = al_create_event_queue();


	srand(time(NULL));
	InitMackenzie(mackenzie);
	InitBullet(bullets, NUM_BULLETS);
	InitEnemy(enemies, NUM_ENEMIES);
	InitBrick(bricks, NUM_BRICKS);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_start_timer(timer);

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
			al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH, 0, 
				ALLEGRO_ALIGN_RIGHT, "FPS : %i", actualFPS);
			//end calculate and display fps timer


			//start drawing and updating everything
			UpdateBullet(bullets, NUM_BULLETS);
			StartEnemy(enemies, NUM_ENEMIES);
			UpdateEnemy(enemies, NUM_ENEMIES);
			UpdateMackenzie(mackenzie);
			CollideBullet(bullets, NUM_BULLETS, enemies, NUM_ENEMIES);
			CollideEnemy(enemies, NUM_ENEMIES, mackenzie);
			CollideBrick(bricks, NUM_BRICKS, mackenzie);

			DrawMackenzie(mackenzie);
			DrawBullet(bullets, NUM_BULLETS);
			DrawEnemy(enemies, NUM_ENEMIES);
			DrawBrick(bricks, NUM_BRICKS);
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
	mackenzie.boundx = 40;
	mackenzie.boundy = 40;
	mackenzie.score = 0;
	mackenzie.onGround = false;
}

void DrawMackenzie(Mackenzie &mackenzie){
	al_draw_filled_rounded_rectangle(mackenzie.x, mackenzie.y,
		mackenzie.x + 40, mackenzie.y + 40, 10, 10, al_map_rgb(0, 255, 0));
	
	al_draw_filled_triangle(mackenzie.x + 10, mackenzie.y + 5,
		mackenzie.x + 5, mackenzie.y + 15,
		mackenzie.x + 15, mackenzie.y + 15, al_map_rgb(0, 0, 0)); 

	al_draw_filled_triangle(mackenzie.x + 30, mackenzie.y + 5,
		mackenzie.x + 25, mackenzie.y + 15,
		mackenzie.x + 35, mackenzie.y + 15, al_map_rgb(0, 0, 0));

	al_draw_filled_triangle(mackenzie.x + 20, mackenzie.y + 15,
		mackenzie.x + 15, mackenzie.y + 25,
		mackenzie.x + 25, mackenzie.y + 25, al_map_rgb(0, 0, 0));

	al_draw_filled_triangle(mackenzie.x + 20, mackenzie.y + 38,
		mackenzie.x + 38, mackenzie.y + 27,
		mackenzie.x + 2, mackenzie.y + 27, al_map_rgb(0, 0, 0));
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
					if(bullets[i].x > (enemies[j].x - enemies[j].boundx) &&
						bullets[i].x < (enemies[j].x + enemies[j].boundx) &&
						bullets[i].y > (enemies[j].y - enemies[j].boundy) &&
						bullets[i].y < (enemies[j].y + enemies[j].boundy))
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
		enemies[i].boundx = 18;
		enemies[i].boundy = 18;
	}
}
void DrawEnemy(Enemy enemies[], int size){
	for(int i = 0; i < size; i++){
		if(enemies[i].live){
			al_draw_filled_circle(enemies[i].x, enemies[i].y, 20, al_map_rgb(255,0,0));
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
			if(enemies[i].x - enemies[i].boundx < mackenzie.x + mackenzie.boundx &&
				enemies[i].x + enemies[i].boundx > mackenzie.x &&
				enemies[i].y - enemies[i].boundy < mackenzie.y + mackenzie.boundy &&
				enemies[i].y + enemies[i].boundy > mackenzie.y){
					mackenzie.lives--;
					enemies[i].live = false;
			}
			else if(enemies[i].x < 0){
				enemies[i].live = false;
				mackenzie.lives--;
			}
		}
	}
}

void InitBrick(Brick bricks[], int size){
	for(int i = 0; i < size; i++){
		bricks[i].ID = BRICK;
		bricks[i].size = 40;
	}

	bricks[1].x = 0;
	bricks[1].y = 0;
	bricks[2].x = 40;
	bricks[2].y = 0;
	bricks[3].x = 80;
	bricks[3].y = 0;
	bricks[4].x = 120;
	bricks[4].y = 0;
	bricks[5].x = 480;
	bricks[5].y = 0;
	bricks[6].x = 520;
	bricks[6].y = 0;
	bricks[7].x = 560;
	bricks[7].y = 0;
	bricks[8].x = 600;
	bricks[8].y = 0;

	bricks[9].x = 0;
	bricks[9].y = 40;
	bricks[10].x = 280;
	bricks[10].y = 40;
	bricks[11].x = 320;
	bricks[11].y = 40;
	bricks[12].x = 600;
	bricks[12].y = 40;

	bricks[13].x = 0;
	bricks[13].y = 80;
	bricks[14].x = 120;
	bricks[14].y = 80;
	bricks[15].x = 600;
	bricks[15].y = 80;

	bricks[16].x = 0;
	bricks[16].y = 120;
	bricks[17].x = 80;
	bricks[17].y = 120;
	bricks[18].x = 120;
	bricks[18].y = 120;
	bricks[19].x = 400;
	bricks[19].y = 120;
	bricks[20].x = 440;
	bricks[20].y = 120;
	bricks[21].x = 480;
	bricks[21].y = 120;
	bricks[22].x = 600;
	bricks[22].y = 120;

	bricks[23].x = 0;
	bricks[23].y = 160;
	bricks[24].x = 120;
	bricks[24].y = 160;
	bricks[25].x = 600;
	bricks[25].y = 160;

	bricks[26].x = 0;
	bricks[26].y = 200;
	bricks[27].x = 40;
	bricks[27].y = 200;
	bricks[28].x = 120;
	bricks[28].y = 200;
	bricks[29].x = 160;
	bricks[29].y = 200;
	bricks[30].x = 200;
	bricks[30].y = 200;
	bricks[31].x = 240;
	bricks[31].y = 200;
	bricks[32].x = 280;
	bricks[32].y = 200;
	bricks[33].x = 600;
	bricks[33].y = 200;

	bricks[34].x = 0;
	bricks[34].y = 240;
	bricks[35].x = 120;
	bricks[35].y = 240;
	bricks[36].x = 600;
	bricks[36].y = 240;

	bricks[37].x = 0;
	bricks[37].y = 280;
	bricks[38].x = 80;
	bricks[38].y = 280;
	bricks[39].x = 120;
	bricks[39].y = 280;
	bricks[40].x = 320;
	bricks[40].y = 280;
	bricks[41].x = 600;
	bricks[41].y = 280;

	bricks[42].x = 0;
	bricks[42].y = 320;
	bricks[43].x = 120;
	bricks[43].y = 320;
	bricks[44].x = 600;
	bricks[44].y = 320;

	bricks[45].x = 0;
	bricks[45].y = 360;
	bricks[46].x = 40;
	bricks[46].y = 360;
	bricks[47].x = 400;
	bricks[47].y = 360;
	bricks[48].x = 600;
	bricks[48].y = 360;

	bricks[49].x = 0;
	bricks[49].y = 400;
	bricks[50].x = 40;
	bricks[50].y = 400;
	bricks[51].x = 400;
	bricks[51].y = 400;
	bricks[52].x = 600;
	bricks[52].y = 400;

	bricks[53].x = 0;
	bricks[53].y = 440;
	bricks[54].x = 40;
	bricks[54].y = 440;
	bricks[55].x = 80;
	bricks[55].y = 440;
	bricks[56].x = 120;
	bricks[56].y = 440;
	bricks[57].x = 160;
	bricks[57].y = 440;
	bricks[58].x = 200;
	bricks[58].y = 440;
	bricks[59].x = 320;
	bricks[59].y = 440;
	bricks[60].x = 400;
	bricks[60].y = 440;
	bricks[61].x = 480;
	bricks[61].y = 440;
	bricks[62].x = 520;
	bricks[62].y = 440;
	bricks[63].x = 560;
	bricks[63].y = 440;
	bricks[64].x = 600;
	bricks[64].y = 440;

}
void DrawBrick(Brick bricks[], int size){
	for(int i = 0; i < size; i++){
		if(IsBrickOnScreen(bricks, i)){
				al_draw_filled_rectangle(bricks[i].x, bricks[i].y,
					bricks[i].x + bricks[i].size, bricks[i].y + bricks[i].size, al_map_rgb(165, 42, 42));
		}
	}
}

void UpdateBrick(Brick bricks[], int size){

}
void CollideBrick(Brick bricks[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(IsBrickOnScreen(bricks, i)){
			//check if collided with any brick on screen
			if(!((mackenzie.y + mackenzie.boundy) < bricks[i].y ||
				mackenzie.y > (bricks[i].y + bricks[i].size) ||
				(mackenzie.x + mackenzie.boundx) < bricks[i].x ||
				mackenzie.x > (bricks[i].x + bricks[i].size))){
					//find side collided on then change x or y coordinate accordingly
					float angle = atan2((bricks[i].y-mackenzie.y), (mackenzie.x - bricks[i].x));
					float cosAngle = cos(angle);
					float sinAngle = sin(angle);
					float difference = abs(cosAngle) - abs(sinAngle);
					if(abs(cosAngle) > abs(sinAngle)){
						if(cosAngle > 0){
							mackenzie.x = bricks[i].x + bricks[i].size;
							if(mackenzie.vx < 0)
								mackenzie.vx = 0;
						}
						else if(cosAngle < 0){
							mackenzie.x = bricks[i].x - mackenzie.boundx;
							if(mackenzie.vx > 0)
								mackenzie.vx = 0;
						}
					}
					else if(abs(sinAngle) > abs(cosAngle)){
						if(sinAngle > 0){
							mackenzie.y = bricks[i].y - mackenzie.boundy;
							if(mackenzie.vy > 0)
								mackenzie.vy = 0;
							mackenzie.onGround = true;
						}
						else if(sinAngle < 0){
							mackenzie.y = bricks[i].y + bricks[i].size;
							if(mackenzie.vy < 0)
								mackenzie.vy = 0;
						}
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