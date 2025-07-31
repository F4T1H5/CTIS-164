/**********************************
 CTIS164 - Template Source Program

ADDITIONAL FEATURES :
1-A simple game where we try to pop the balloons within a certain time period, and random colors, positions, and speeds.
2-It is a game with a mechanism where we move the object with the Up and Down arrow keys and shoot with the spacebar.
3-The game has a mechanism where we can enlarge the size of the balloons by right-clicking the mouse.
**********************************/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>

#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 600
#define TIMER_PERIOD   20 // Period for the timer (50 fps)
#define TIMER_ON        1 // 0:Disable timer, 1:Enable timer
#define D2R 0.017453292
#define PI 3.141592654

#define MAX_FIRE 20  // 20 fires at a time.
#define FIRE_RATE 8  // After 8 frames, you can throw another one.
#define TARGET_RADIUS 20
#define NUM_BALLOONS 5

// Global variables for Template File
bool timeUp = false;
bool spacebar = false;
bool gamePaused = false;
int  winWidth, winHeight; // Current Window width and height
int gameTimer = 2000; // 20 seconds in frames
int initialGameTimer; // initialize game
int elapsedGameTime = 0;
bool gameOver = false;
bool rightMouseButtonPressed = false;

// User Defined Types and Variables
typedef struct {
	float x, y;
} point_t;

typedef struct {
	point_t pos;    // Position of the object
	float   angle;  // View angle 
	float   r;
} player_t;

typedef struct {
	point_t pos;
	float angle;
	bool active;
} fire_t;

typedef struct {
	int r, g, b;
} color_t;

typedef struct {
	point_t center;
	color_t color;
	float radius;
	float speed;
	bool popped; // whether balloon popped up or not
} balloon_t;

player_t player = { {-250, 20}, 0, 15 };
fire_t   fire[MAX_FIRE];
balloon_t target;
balloon_t balloons[NUM_BALLOONS];
color_t poppedBalloonColors[NUM_BALLOONS];

int fire_rate = 0;

// To draw a filled circle, centered at (x,y) with radius r
void circle(int x, int y, int r) {
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++) {
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

// To display text with variables
void vprint(int x, int y, void* font, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, str[i]);
}

void drawBg() {
	// Window Frame
	glColor3ub(0, 0, 0);  // Black color frame
	glLineWidth(15);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-250, -250);
	glVertex2f(250, -250);
	glVertex2f(250, 250);
	glVertex2f(-250, 250);
	glEnd();

	glBegin(GL_QUADS);
	// Green tones background
	glColor3ub(76, 187, 23);
	glVertex2f(-250, 125);
	glVertex2f(-250, 250);
	glVertex2f(250, 250);
	glVertex2f(250, 125);

	glColor3ub(50, 205, 50);
	glVertex2f(-250, 0);
	glVertex2f(-250, 125);
	glVertex2f(250, 125);
	glVertex2f(250, 0);

	glColor3ub(11, 218, 81);
	glVertex2f(-250, -125);
	glVertex2f(-250, 0);
	glVertex2f(250, 0);
	glVertex2f(250, -125);

	glColor3ub(152, 251, 152);
	glVertex2f(-250, -250);
	glVertex2f(-250, -125);
	glVertex2f(250, -125);
	glVertex2f(250, -250);

	glEnd();
}

void drawPlayer(player_t tp) {
	// suitable changes for location
	tp.pos.x -= 4;

	// players border check 
	tp.pos.x = fmaxf(-WINDOW_WIDTH / 2 + tp.r * 2, fminf(WINDOW_WIDTH / 2 - tp.r * 2, tp.pos.x));
	tp.pos.y = fmaxf(-WINDOW_HEIGHT / 2.3 + tp.r, fminf(WINDOW_HEIGHT / 2.3 - tp.r, tp.pos.y));

	// Draw bow base
	glColor3f(0.5, 0.3, 0.2); // Brown color
	glBegin(GL_QUADS);
	glVertex2f(tp.pos.x - tp.r * 2, tp.pos.y - tp.r / 2);
	glVertex2f(tp.pos.x + tp.r * 2, tp.pos.y - tp.r / 2);
	glVertex2f(tp.pos.x + tp.r * 2, tp.pos.y + tp.r / 2);
	glVertex2f(tp.pos.x - tp.r * 2, tp.pos.y + tp.r / 2);
	glEnd();

	// Draw bow string
	glColor3f(0.0, 0.0, 0.0); // Black color
	glLineWidth(2.0); // Set line width
	glBegin(GL_LINES);
	glVertex2f(tp.pos.x, tp.pos.y - tp.r / 2);
	glVertex2f(tp.pos.x, tp.pos.y + tp.r / 2);
	glEnd();

	// Draw bow limbs
	glColor3f(0.6, 0.6, 0.6); // Light gray color for limbs

	// Top limb
	glBegin(GL_QUADS);
	glVertex2f(tp.pos.x - tp.r, tp.pos.y + tp.r / 2);
	glVertex2f(tp.pos.x + tp.r, tp.pos.y + tp.r / 2);
	glVertex2f(tp.pos.x + tp.r, tp.pos.y + tp.r);
	glVertex2f(tp.pos.x - tp.r, tp.pos.y + tp.r);
	glEnd();

	// Bottom limb
	glBegin(GL_QUADS);
	glVertex2f(tp.pos.x - tp.r, tp.pos.y - tp.r / 2);
	glVertex2f(tp.pos.x + tp.r, tp.pos.y - tp.r / 2);
	glVertex2f(tp.pos.x + tp.r, tp.pos.y - tp.r);
	glVertex2f(tp.pos.x - tp.r, tp.pos.y - tp.r);
	glEnd();

	glColor3f(0.4, 0.2, 0.1); // Darker brown color

	// Top limb detail
	glBegin(GL_QUADS);
	glVertex2f(tp.pos.x - tp.r * 1.5, tp.pos.y + tp.r / 2);
	glVertex2f(tp.pos.x + tp.r * 1.5, tp.pos.y + tp.r / 2);
	glVertex2f(tp.pos.x + tp.r * 1.2, tp.pos.y + tp.r * 1.2);
	glVertex2f(tp.pos.x - tp.r * 1.2, tp.pos.y + tp.r * 1.2);
	glEnd();

	// Bottom limb detail
	glBegin(GL_QUADS);
	glVertex2f(tp.pos.x - tp.r * 1.5, tp.pos.y - tp.r / 2);
	glVertex2f(tp.pos.x + tp.r * 1.5, tp.pos.y - tp.r / 2);
	glVertex2f(tp.pos.x + tp.r * 1.2, tp.pos.y - tp.r * 1.2);
	glVertex2f(tp.pos.x - tp.r * 1.2, tp.pos.y - tp.r * 1.2);
	glEnd();
}

void drawBalloon(balloon_t t) {
	if (!t.popped) {
		glColor3ub(t.color.r, t.color.g, t.color.b);
		// balloon body
		circle(t.center.x, t.center.y, t.radius);

		// Draw balloon string
		glColor3f(0.2, 0.2, 0.2);  // Dark gray color
		glBegin(GL_LINES);
		glVertex2f(t.center.x, t.center.y - t.radius);
		glVertex2f(t.center.x, t.center.y - t.radius * 2);
		glEnd();
	}
}

void drawTarget(balloon_t t) {
	drawBalloon(t);
}

void drawFires() {
	for (int i = 0; i < MAX_FIRE; i++) {
		if (fire[i].active) {
			glColor3f(0.8, 0.2, 0.2);

			// Draw arrow body
			glBegin(GL_TRIANGLES);
			glVertex2f(fire[i].pos.x, fire[i].pos.y);
			glVertex2f(fire[i].pos.x - 15, fire[i].pos.y + 7.5); // Arrowhead
			glVertex2f(fire[i].pos.x - 15, fire[i].pos.y - 7.5); // Arrowhead
			glEnd();

			glBegin(GL_LINES);
			glVertex2f(fire[i].pos.x - 15, fire[i].pos.y);
			glVertex2f(fire[i].pos.x - 30, fire[i].pos.y);
			glEnd();
		}
	}
}

bool testCollision(fire_t fr, balloon_t& t) {
	if (t.popped) {
		// Regenerate new balloon starting position
		float xPos = rand() % 75 + 150;
		t.center = { xPos, -(float)winHeight / 2 - 300 };
		t.color.r = rand() % 256;
		t.color.g = rand() % 256;
		t.color.b = rand() % 256;
		t.radius = TARGET_RADIUS;
		t.speed = 3.0 * (rand() % 100) / 100.0 + 0.5;
		t.popped = false;
		return false;  // no collision
	}

	float dx = t.center.x - fr.pos.x;
	float dy = t.center.y - fr.pos.y;
	float d = sqrt(dx * dx + dy * dy);

	if (d <= t.radius) {
		t.popped = true;  // Balloon as popped
		return true;      // Collision
	}
	return false;  // no collision
}

// In the fire array, check if any fire is available.
// If there is a fire that we can use for firing, return its index.
int findAvailableFire() {
	for (int i = 0; i < MAX_FIRE; i++)
		if (fire[i].active == false) return i;
	return -1;
}

void resetBalloons() {
	for (int i = 0; i < NUM_BALLOONS; i++) {
		float xPos = rand() % 75 + 150;
		balloons[i].center = { xPos, -(float)winHeight / 2 - 350 };
		balloons[i].color.r = rand() % 256;
		balloons[i].color.g = rand() % 256;
		balloons[i].color.b = rand() % 256;
		balloons[i].radius = TARGET_RADIUS;
		balloons[i].speed = 5.0 * (rand() % 100) / 100.0 + 0.5;
		balloons[i].popped = false;
	}
}

void onMouseClick(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			// Right mouse button pressed
			rightMouseButtonPressed = true;
		}
		else if (state == GLUT_UP) {
			// Right mouse button released
			rightMouseButtonPressed = false;
		}
	}
}

void drawBalloons() {
	for (int i = 0; i < NUM_BALLOONS; i++) {
		if (!balloons[i].popped) {
			if (rightMouseButtonPressed) {
				// Increase balloon size
				balloons[i].radius *= 1.05; // Growth Rate
			}
			drawBalloon(balloons[i]);
		}
	}
}


void moveBalloons() {
	for (int i = 0; i < NUM_BALLOONS; i++) {
		if (!balloons[i].popped) {
			balloons[i].center.y += balloons[i].speed;
			if (balloons[i].center.y > winHeight / 2) {
				float xPos = rand() % 75 + 150;
				balloons[i].center = { xPos, -(float)winHeight / 2 };
				balloons[i].color.r = rand() % 256;
				balloons[i].color.g = rand() % 256;
				balloons[i].color.b = rand() % 256;
				balloons[i].radius = TARGET_RADIUS;
				balloons[i].speed = 3.0 * (rand() % 100) / 100.0 + 0.5;
				balloons[i].popped = false;
			}
		}
	}
}

// To display onto window using OpenGL commands
void display() {
	glClearColor(0.529, 0.808, 0.922, 1.0); // Sky blue color
	glClear(GL_COLOR_BUFFER_BIT);

	drawBg();
	drawPlayer(player);
	drawFires();
	drawBalloons();

	if (gameTimer >= 2000) {
		vprint(-260, 285, GLUT_BITMAP_8_BY_13, "F1 ---> Start Game");
	}

	if (gameTimer < 2000 && gameTimer > 0) {
		vprint(-260, 285, GLUT_BITMAP_8_BY_13, "Time Left: %d seconds", (gameTimer + 100) / 100);
		vprint(-260, 265, GLUT_BITMAP_8_BY_13, "F1--->Pause/Continue");
		vprint(-260, -270, GLUT_BITMAP_8_BY_13, "Spacebar ---> Shoot");
		vprint(-260, -290, GLUT_BITMAP_8_BY_13, "Arrow-Up and Arrow-Down ---> Object Movement");
		vprint(-30, -270, GLUT_BITMAP_8_BY_13, "Mouse Right Click ---> Ballons Size Increase");
	}
	glutSwapBuffers();
}

void resetGame() {
	resetBalloons(); // Add this line to reset balloons
	gameTimer = 2000; // Reset timer to 20 seconds
	initialGameTimer = gameTimer; // Store the initial game time
	elapsedGameTime = 0; // Reset elapsed time
	gamePaused = true; // Pause the game initially or as needed
	gameOver = false; // Reset the game over flag
	fire_rate = 0; // Reset fire rate
	player = { {-250, 20}, 0, 15 };

	// Reset arrow positions
	for (int i = 0; i < MAX_FIRE; i++) {
		fire[i].active = false;
	}
}

// Key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
void onKeyDown(unsigned char key, int x, int y) {
	// Exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ') {
		spacebar = true;
	}
	if (key == GLUT_KEY_F1) {
		if (gameOver) {
			resetGame();
			gamePaused = false;
			gameOver = false;
		}
		else {
			gamePaused = !gamePaused;  // Game pause
			if (!gamePaused) {
				int elapsed = initialGameTimer - gameTimer;
				// Resume the game
				gameTimer = initialGameTimer - elapsed;
			}
		}
	}

	if (key == GLUT_KEY_F1) {
		if (gameOver) {
			resetGame();
			gamePaused = false;
			gameOver = false; // Reset the game over flag when restarting
			// Reset the game timer
			gameTimer = initialGameTimer;
		}
		else {
			gamePaused = !gamePaused;
			if (!gamePaused) {
				int elapsed = initialGameTimer - gameTimer;
				gameTimer = initialGameTimer - elapsed;
			}
		}
	}
}

void onKeyUp(unsigned char key, int x, int y) {
	// Exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = false;
}

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
void onSpecialKeyDown(int key, int x, int y) {
	float speed = 3.0; // Game speed

	switch (key) {
	case GLUT_KEY_F1:
		gamePaused = !gamePaused;
		if (!gamePaused) {
			int elapsed = initialGameTimer - gameTimer;
			gameTimer = initialGameTimer - elapsed;
		}
		break;
	case GLUT_KEY_UP:
		if (!gamePaused && !gameOver)
		{
			player.pos.y += speed;
			break;
		}
	case GLUT_KEY_DOWN:
		if (!gamePaused && !gameOver)
		{
			player.pos.y -= speed;
			break;
		}
	}

	// Limitation players border
	player.pos.y = fmaxf(-WINDOW_HEIGHT / 2.5, fminf(WINDOW_HEIGHT / 2.5, player.pos.y));

	// To refresh the window, it calls the display() function
	glutPostRedisplay();
}

// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
void onResize(int w, int h) {
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // Refresh window
}

#if TIMER_ON == 1
void onTimer(int v) {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (!gamePaused) {
		if (spacebar && fire_rate == 0) {
			int availFire = findAvailableFire();
			if (availFire != -1) {
				// Calculate the offset from the dart shooter's position to the barrel
				float offset_x = player.r * cos(player.angle * D2R);
				float offset_y = player.r * sin(player.angle * D2R);

				// Set the initial position of the fire ball to align with the dart shooter's barrel
				fire[availFire].pos.x = player.pos.x + offset_x;
				fire[availFire].pos.y = player.pos.y + offset_y + 10;

				fire[availFire].angle = player.angle;
				fire[availFire].active = true;
				fire_rate = FIRE_RATE;
			}
		}

		if (fire_rate > 0) fire_rate--;

		for (int i = 0; i < MAX_FIRE; i++) {
			if (fire[i].active) {
				fire[i].pos.x += 10 * cos(fire[i].angle * D2R);
				fire[i].pos.y += 10 * sin(fire[i].angle * D2R);
				if (fire[i].pos.x > winWidth / 2.5 || fire[i].pos.x < -winWidth / 2 ||
					fire[i].pos.y > winHeight / 2 || fire[i].pos.y < -winHeight / 2) {
					fire[i].active = false;
				}

				// Check collision with target balloon
				if (testCollision(fire[i], target)) {
					fire[i].active = false;
					target.popped = true;
				}

				// Check collision with array of balloons
				for (int j = 0; j < NUM_BALLOONS; j++) {
					if (!balloons[j].popped && testCollision(fire[i], balloons[j])) {
						fire[i].active = false;
						balloons[j].popped = true;

						// Regenerate balloons
						float xPos = rand() % 75 + 150;
						balloons[j].center = { xPos, -(float)winHeight / 2 - 300 };
						balloons[j].color.r = rand() % 256;
						balloons[j].color.g = rand() % 256;
						balloons[j].color.b = rand() % 256;
						balloons[j].radius = TARGET_RADIUS;
						balloons[j].speed = 5.0 * (rand() % 100) / 100.0 + 0.5;
						balloons[j].popped = false;
					}
				}
			}
		}
		// Move balloons
		moveBalloons();

		// Move target balloon from bottom to top
		target.center.y += target.speed;
		if (target.center.y > winHeight / 2) {
			resetBalloons();
		}

		// Check game over conditions
		if (gameTimer <= 0) {
			gameOver = true;
			gamePaused = true;
		}

		// Timer countdown
		if (gameTimer > 0) {
			gameTimer -= TIMER_PERIOD / 6;
		}
		else {
			resetGame();
			gamePaused = true;
		}

		// Accumulate elapsed time
		elapsedGameTime += TIMER_PERIOD / 6;

		// To refresh the window, it calls display() function
		glutPostRedisplay();
	}
}
#endif

void init() {
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	resetBalloons();

	// Initialize game state
	gamePaused = true; // Initially paused
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(500, 200);
	glutCreateWindow("Simple Ballons Shooting Game");

	// Window Events
	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	// Keyboard Events
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);
	glutKeyboardUpFunc(onKeyUp);

	// Mouse events
	glutMouseFunc(onMouseClick);

#if TIMER_ON == 1
	// Timer Event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	init();
	glutMainLoop();
}