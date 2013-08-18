// Object IDS
enum IDS{PLAYER, BULLET, GHOST, BRICK, SPRING, WATER,
	BOMB, JUMP, CANNON, EBULLET, LEVEL, FINISH, BOSS, SEAWEED, LIGHTNING, CUTSCENE};

// Our Player
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
	int boundDown;
	int boundUp;
	int boundLeft;
	int boundRight;
	int score;
	bool onGround;
	bool extraJump;
	bool isCrouched;
	bool invincible;
	int timer;
	bool isFacingRight;
	bool jumped;
	bool kicked;
};

struct Bullet
{
	int ID;
	float x;
	float y;
	bool live;
	int speed;
	bool isMovingRight;
};

struct Enemy
{
	int ID;
	float x;
	float y;
	bool live;
	float speed;
	int boundDown;
	int boundUp;
	int boundLeft;
	int boundRight;
	int timer;
	int health;
	float speedy;
	int attackCounter;
};

struct Brick
{
	int ID;
	float x;
	float y;
	int size;
	int timer;
	float xstart;
	float ystart;
	int level;
};

struct EBullet
{
	int ID;
	float x;
	float y;
	float vx;
	float vy;
	bool live;
};