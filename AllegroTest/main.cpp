#include <allegro5\allegro.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"

enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false, false};
const int WIDTH = 640;
const int HEIGHT = 480;
const int NUM_BULLETS = 5;
const int NUM_ENEMIES = 10;

void InitMackenzie(Mackenzie &mackenzie);
void DrawMackenzie(Mackenzie &mackenzie);
void JumpMackenzie(Mackenzie &mackenzie);
void MoveMackenzieDown(Mackenzie &mackenzie);
void MoveMackenzieLeft(Mackenzie &mackenzie);
void MoveMackenzieRight(Mackenzie &mackenzie);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, Mackenzie &mackenzie);
void UpdateBullet(Bullet bullet[], int size);

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

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;


	if(!al_init()){
		al_show_native_message_box(NULL, NULL, NULL,
			"failed to initialize allegro!", NULL, NULL);
		return -1;
	}
	display = al_create_display(WIDTH, HEIGHT);
	if(!display){
		al_show_native_message_box(NULL, NULL, NULL,
			"failed to initialize display!", NULL, NULL);
		return -1;
	}
	timer = al_create_timer(1.0/FPS);

	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 18, 0);

	event_queue = al_create_event_queue();

	InitMackenzie(mackenzie);
	InitBullet(bullets, NUM_BULLETS);

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
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[LEFT] = true;
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
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[LEFT] = false;
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
			if(keys[UP])
				JumpMackenzie(mackenzie);
			if(keys[DOWN])
				MoveMackenzieDown(mackenzie);
			if(keys[LEFT])
				MoveMackenzieLeft(mackenzie);
			if(keys[RIGHT])
				MoveMackenzieRight(mackenzie);
			
			UpdateBullet(bullets, NUM_BULLETS);

		}

		if(redraw && al_is_event_queue_empty(event_queue))
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
			DrawMackenzie(mackenzie);
			DrawBullet(bullets, NUM_BULLETS);
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
	mackenzie.speed = 7;
	mackenzie.boundx = 6;
	mackenzie.boundy = 7;
	mackenzie.score = 0;
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
	mackenzie.y -= mackenzie.speed;
	if(mackenzie.y < 0)
		mackenzie.y = 0;
}

void MoveMackenzieDown(Mackenzie &mackenzie){
	mackenzie.y += mackenzie.speed;
	if(mackenzie.y > HEIGHT-40)
		mackenzie.y = HEIGHT-40;
}
void MoveMackenzieLeft(Mackenzie &mackenzie){
	mackenzie.x += mackenzie.speed;
	if(mackenzie.x > WIDTH-40)
		mackenzie.x = WIDTH-40;
}
void MoveMackenzieRight(Mackenzie &mackenzie){
	mackenzie.x -= mackenzie.speed;
	if(mackenzie.x < 0)
		mackenzie.x = 0;
}

void InitBullet(Bullet bullet[], int size){
	for(int i = 0; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;
	}
}
void DrawBullet(Bullet bullet[], int size){
	for(int i = 0; i < size; i++){
		if(bullet[i].live){
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(255, 255, 255));
		}
	}
}
void FireBullet(Bullet bullet[], int size, Mackenzie &mackenzie){
	for(int i = 0; i < size; i++){
		if(!bullet[i].live){
			bullet[i].x = mackenzie.x + 42;
			bullet[i].y = mackenzie.y + 20;
			bullet[i].live = true;
			break;
		}
	}
}
void UpdateBullet(Bullet bullet[], int size){
	for(int i = 0; i < size; i++){
		if(bullet[i].live){
			bullet[i].x += bullet[i].speed;
			if(bullet[i].x > WIDTH)
				bullet[i].live = false;
		}
	}
}