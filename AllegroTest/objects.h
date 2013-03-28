//Object IDS
enum IDS{PLAYER, BULLET, ENEMY, BRICK};

//Our Player
struct Mackenzie
{
	int ID;
	float x;
	float y;
	int lives;
	float vx;
	float vy;
	float acc;
	float dec;
	float maxSpeed;
	int boundx;
	int boundy;
	int score;
	bool onGround;
};

struct Bullet
{
	int ID;
	float x;
	float y;
	bool live;
	int speed;
};

struct Enemy
{
	int ID;
	float x;
	float y;
	bool live;
	float speed;
	int boundx;
	int boundy;
};

struct Brick
{
	int ID;
	float x;
	float y;
	int size;
};