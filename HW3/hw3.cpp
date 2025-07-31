/**********************************
 CTIS164 - Template Source Program

ADDITIONAL FEATURES :
--->	We can follow and hit moving balloons by throwing objects from a stationary gun (you need to use the spacebar to hit),
--->	The background changes smoothly and adds a pleasant atmosphere,
--->	A simple game where the speed of the balloons increases as the balloons pop.
**********************************/
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>
#include "vec.h"

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
int  winWidth, winHeight; // Current Window width and height
bool gameOver = false;
bool rightMouseButtonPressed = false;
float speedMultiplier = 1.0; // Current balloon speed

typedef struct {
	float x, y;
} point_t;

typedef struct {
	point_t pos;
	float   angle;
	float   r;
} player_t;

typedef struct {
	point_t pos;
	float angle;
	bool active;
	int targetIndex;
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
	bool reverse;
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

void vertex(vec_t P, vec_t Tr, double angle) {
	vec_t rotatedP = {
		P.x * cos(angle) - P.y * sin(angle),
		P.x * sin(angle) + P.y * cos(angle)
	};

	vec_t translatedRotatedP = {
		rotatedP.x + Tr.x,
		rotatedP.y + Tr.y
	};

	glVertex2d(translatedRotatedP.x, translatedRotatedP.y);
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

			// calculate the offset from the dart shooter's position to the barrel
			vec_t offset = { player.r * cos(player.angle * D2R), player.r * sin(player.angle * D2R) };

			// set the initial position of the fire ball to align with the dart shooter's barrel
			vec_t arrowPos = { fire[i].pos.x + offset.x, fire[i].pos.y + offset.y };

			glBegin(GL_TRIANGLES);
			vec_t P1 = { 0, 0 };
			vec_t P2 = { -25, 12.5 };
			vec_t P3 = { -25, -12.5 };
			vertex(P1, arrowPos, fire[i].angle * D2R);
			vertex(P2, arrowPos, fire[i].angle * D2R);
			vertex(P3, arrowPos, fire[i].angle * D2R);
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
		speedMultiplier += 0.5;
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
		balloons[i].reverse = false; // Initialize reverse flag
	}
	speedMultiplier = 1.0; // speed reset for balloons
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
			float xPos = balloons[i].center.x;
			float yPos = balloons[i].center.y;

			// update the x-coordinate with a half-moon pattern
			xPos += balloons[i].speed * cos(yPos * 0.01);

			// update the y-coordinate to move the balloon up or down
			yPos += balloons[i].speed * (balloons[i].reverse ? -1 : 1) * speedMultiplier;

			// update the balloon's position
			balloons[i].center.x = xPos;
			balloons[i].center.y = yPos;

			// check if the balloon has reached the top or bottom
			if (yPos > winHeight / 2 || yPos < -winHeight / 2) {
				// reverse the direction when a balloon reaches the top or bottom
				balloons[i].reverse = !balloons[i].reverse;

				// reset position to the top or bottom
				balloons[i].center.y = balloons[i].reverse ? winHeight / 2 : -winHeight / 2;
			}
		}
	}
}

// To display onto window using OpenGL commands
void display() {
	// Background colors created randomly
	static float colorChange = 0.0;
	float red = (sin(colorChange) + 1.0) * 0.5;
	float green = (cos(colorChange) + 1.0) * 0.5;
	float blue = (sin(colorChange + 0.5) + 1.0) * 0.5;
	colorChange += 0.05;  // Speed of color change
	glClearColor(red, green, blue, 1.0);  // set background color
	glClear(GL_COLOR_BUFFER_BIT);

	vprint(-200, 275, GLUT_BITMAP_8_BY_13, "***Balloon Popping Game With Moving Objects***");

	drawBg();
	drawPlayer(player);
	drawFires();
	drawBalloons();

	glutSwapBuffers();
}

void resetGame() {
	resetBalloons(); // Add this line to reset balloons
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
}

void onKeyUp(unsigned char key, int x, int y) {
	// Exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		spacebar = false;
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

	// shooting
	if (spacebar && fire_rate == 0) {
		int availFire = findAvailableFire();
		if (availFire != -1) {
			fire[availFire].pos.x = player.pos.x;
			fire[availFire].pos.y = player.pos.y + 10;
			fire[availFire].angle = angleV(subV({ target.center.x, target.center.y }, { player.pos.x, player.pos.y }));
			fire[availFire].active = true;

			// find the closest active balloon as the target
			float minDist = FLT_MAX;
			int targetIndex = -1;

			for (int j = 0; j < NUM_BALLOONS; j++) {
				if (!balloons[j].popped) {
					float dx = balloons[j].center.x - fire[availFire].pos.x;
					float dy = balloons[j].center.y - fire[availFire].pos.y;
					float dist = sqrt(dx * dx + dy * dy);

					if (dist < minDist) {
						minDist = dist;
						targetIndex = j;
					}
				}
			}

			fire[availFire].targetIndex = targetIndex; // set the target balloon index
			fire_rate = FIRE_RATE;
		}
	}

	if (fire_rate > 0) fire_rate--;

	for (int i = 0; i < MAX_FIRE; i++) {
		if (fire[i].active) {
			// if target balloon is still active, update the firing angle
			if (fire[i].targetIndex != -1 && !balloons[fire[i].targetIndex].popped) {
				float dx = balloons[fire[i].targetIndex].center.x - fire[i].pos.x;
				float dy = balloons[fire[i].targetIndex].center.y - fire[i].pos.y;
				fire[i].angle = atan2(dy, dx) * 180.0 / PI;
			}

			// update position of the fire
			fire[i].pos.x += 10 * cos(fire[i].angle * D2R);
			fire[i].pos.y += 10 * sin(fire[i].angle * D2R);

			// check collision with the target balloon
			if (fire[i].targetIndex != -1 && testCollision(fire[i], balloons[fire[i].targetIndex])) {
				fire[i].active = false;
				balloons[fire[i].targetIndex].popped = true;

				// reset the target index for the next shot
				fire[i].targetIndex = -1;
			}

			// check if the fire is out of bounds
			if (fire[i].pos.x > winWidth / 2.5 || fire[i].pos.x < -winWidth / 2 ||
				fire[i].pos.y > winHeight / 2 || fire[i].pos.y < -winHeight / 2) {
				fire[i].active = false;
				fire[i].targetIndex = -1; // reset the target index if the fire goes out of bounds
			}
		}
	}

	// move balloons
	moveBalloons();

	// reset popped balloons
	for (int i = 0; i < NUM_BALLOONS; i++) {
		if (balloons[i].popped) {
			float xPos = rand() % 75 + 150;
			balloons[i].center = { xPos, -(float)winHeight / 2 - 300 };
			balloons[i].color.r = rand() % 256;
			balloons[i].color.g = rand() % 256;
			balloons[i].color.b = rand() % 256;
			balloons[i].radius = TARGET_RADIUS;
			balloons[i].speed = 5.0 * (rand() % 100) / 100.0 + 0.5;
			balloons[i].popped = false;
		}
	}

	// move target balloon from bottom to top
	target.center.y += target.speed;
	if (target.center.y > winHeight / 2) {
		resetBalloons();
	}

	// to refresh the window, it calls display() function
	glutPostRedisplay();
}
#endif

void init() {
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	resetBalloons();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(500, 200);
	glutCreateWindow("Balloon Popping Game With Moving Objects");

	// Window Events
	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	// Keyboard Events
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

#if TIMER_ON == 1
	// Timer Event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	init();
	glutMainLoop();
}