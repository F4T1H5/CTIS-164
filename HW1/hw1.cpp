/*********
 CTIS164 - Template Source Program

ADDITIONAL FEATURES:
2 rockets passed over houses, mountains and roads.
When we press the left mouse button, the position of the rocket moving horizontally changes,
and when we press the right button, we can adjust the position of the rocket going vertically
Finally, it is a program where we can adjust the direction of the horizontal rocket
with the arrow left-right keys of the keyboard and the direction of the vertical rocket with the arrow up-down keys.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_FRAME_CENTER_WIDTH 10
#define WINDOW_FRAME_WIDTH 35
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  200 // Period for the timer.
#define TIMER_ON        1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define PI 3.1415

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

bool moveRocketRight = true;
bool isMoving = true;
bool moveRocket2Up = true;

float rocket_2 = 100.0;
float rocket_2Y = 10.0;
float rocket2Speed = 50;
float rocket_1 = -350.0;
float rocket_1Y = 10.0;
float rocket1Speed = 50;
float sunX = 0.0;
float sunY = 0.0;
float houseX = 200;
float houseY = -winHeight / 2 - 200;
float houseWidth = 100;
float houseHeight = 50;

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void drawWindowFrame() {
    // left wall
    glColor3ub(129, 133, 137); //gray color
    glBegin(GL_QUADS);
    glVertex2i(-winWidth / 2, winHeight / 2); //top-left
    glVertex2i(-winWidth / 2 + WINDOW_FRAME_WIDTH + 20, winHeight / 2); //top-right
    glVertex2i(-winWidth / 2 + WINDOW_FRAME_WIDTH + 20, -winHeight / 2); //bottom-right
    glVertex2i(-winWidth / 2, -winHeight / 2); // Bottom-left
    glEnd();

    //top and middle wall
    glColor3ub(129, 133, 137); // Gray color
    glBegin(GL_QUADS);
    glVertex2i(-winWidth / 2 + WINDOW_FRAME_WIDTH, winHeight / 2); //top-left
    glVertex2i(winWidth / 2, winHeight / 2); //top-right
    glVertex2i(winWidth / 2, winHeight / 2 - WINDOW_FRAME_WIDTH); //bottom-right
    glVertex2i(-winWidth / 2 + WINDOW_FRAME_WIDTH, winHeight / 2 - WINDOW_FRAME_WIDTH); //bottom-left
    glEnd();

    //central wall
    glBegin(GL_QUADS);
    glVertex2i(-WINDOW_FRAME_CENTER_WIDTH / 2, winHeight / 2); //top-left
    glVertex2i(WINDOW_FRAME_CENTER_WIDTH / 2, winHeight / 2); //top-right
    glVertex2i(WINDOW_FRAME_CENTER_WIDTH / 2, -winHeight / 2); //bottom-right
    glVertex2i(-WINDOW_FRAME_CENTER_WIDTH / 2, -winHeight / 2); //bottom-left
    glEnd();

    //right wall
    glColor3ub(129, 133, 137); //gray color
    glBegin(GL_QUADS);
    glVertex2i(winWidth / 2, winHeight / 2); //top-left
    glVertex2i(winWidth / 2 - WINDOW_FRAME_WIDTH - 20, winHeight / 2); //top-right
    glVertex2i(winWidth / 2 - WINDOW_FRAME_WIDTH - 20, -winHeight / 2); //bottom-right
    glVertex2i(winWidth / 2, -winHeight / 2); //bottom-left
    glEnd();

    //bottom  wall
    glColor3ub(129, 133, 137); //gray color
    glBegin(GL_QUADS);
    glVertex2i(-winWidth / 2 + WINDOW_FRAME_WIDTH, -winHeight / 2); //top-left
    glVertex2i(winWidth / 2 - WINDOW_FRAME_WIDTH, -winHeight / 2); //top-right
    glVertex2i(winWidth / 2 - WINDOW_FRAME_WIDTH, -winHeight / 2 + WINDOW_FRAME_WIDTH); //bottom-right
    glVertex2i(-winWidth / 2 + WINDOW_FRAME_WIDTH, -winHeight / 2 + WINDOW_FRAME_WIDTH); //bottom-left
    glEnd();

    //left and right wall pattern and orange color squares
    int patternX = -winWidth / 1.7 + 10;

    glColor3f(1.0, 0.5, 0.0);

    int squareSize = 15; //size of square

    for (int y = -winHeight / 1.7; y < winHeight / 1.7; y += squareSize * 2) {
        for (int x = patternX; x < patternX + WINDOW_FRAME_WIDTH; x += squareSize * 2) {
            glBegin(GL_QUADS);
            glVertex2i(x, y);
            glVertex2i(x + squareSize, y);
            glVertex2i(x + squareSize, y + squareSize);
            glVertex2i(x, y + squareSize);
            glEnd();
        }
    }

    for (int y = -winHeight / 1.7; y < winHeight / 1.7; y += squareSize * 1.7) {
        for (int x = winWidth / 1.7 - WINDOW_FRAME_WIDTH; x < winWidth / 1.7; x += squareSize * 2) {
            glBegin(GL_QUADS);
            glVertex2i(x, y);
            glVertex2i(x + squareSize, y);
            glVertex2i(x + squareSize, y + squareSize);
            glVertex2i(x, y + squareSize);
            glEnd();
        }
    }
}

void drawBackground() {
    // Sky
    glColor3f(0.529, 0.808, 0.922); //light blue
    glBegin(GL_POLYGON);
    glVertex2i(-winWidth / 2, winHeight / 2); //top-left
    glVertex2i(winWidth / 2, winHeight / 2); //top-right
    glVertex2i(winWidth / 2, -winHeight / 2); //bottom-right
    glVertex2i(-winWidth / 2, -winHeight / 2); //bottom-left
    glEnd();

    //background after the window frame
    glColor3f(0.529, 0.808, 0.922); //light Blue
    glBegin(GL_POLYGON);
    glVertex2i(-winWidth / 1.5, winHeight / 2); //top-left
    glVertex2i(winWidth / 1.5, winHeight / 2); //top-right
    glVertex2i(winWidth / 1.5, -winHeight / 2); //bottom-right
    glVertex2i(-winWidth / 1.5, -winHeight / 2); //bottom-left
    glEnd();

    //road
    glColor3f(0.2, 0.2, 0.2); //gray
    glBegin(GL_POLYGON);
    glVertex2i(-winWidth / 2, -winHeight / 2); //top-left
    glVertex2i(winWidth / 2, -winHeight / 2); //top-right
    glVertex2i(winWidth / 2, -winHeight / 2 + 100); // bottom-right
    glVertex2i(-winWidth / 2, -winHeight / 2 + 100); // bottom-left
    glEnd();

    //white lines
    glColor3f(1.0, 1.0, 1.0); //white
    glLineWidth(5.0);
    glBegin(GL_LINES);
    for (int i = -winWidth / 2; i < winWidth / 2; i += 40) {
        glVertex2i(i, -winHeight / 2 + 50); //left end of line
        glVertex2i(i + 20, -winHeight / 2 + 50);//right end of line
    }
    glEnd();

    //mountain
    glColor3f(0.545, 0.271, 0.075); //brown
    glBegin(GL_TRIANGLES);
    glVertex2i(-winWidth / 2, -winHeight / 2 + 100);
    glVertex2i(-winWidth / 2 + 200, -winHeight / 2 + 400);
    glVertex2i(-winWidth / 2 + 400, -winHeight / 2 + 100);
    glEnd();
}

void drawRocket() {
    //rocket body-red
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_POLYGON);
    glVertex2f(rocket_1 - 50, rocket_1Y - 10);
    glVertex2f(rocket_1 + 50, rocket_1Y - 10);
    glVertex2f(rocket_1 + 40, rocket_1Y + 10);
    glVertex2f(rocket_1 - 40, rocket_1Y + 10);
    glEnd();

    //fins-gray
    glColor3f(0.5, 0.5, 0.5);

    //left fin
    glBegin(GL_TRIANGLES);
    glVertex2f(rocket_1 - 40, rocket_1Y - 10);
    glVertex2f(rocket_1 - 50, rocket_1Y - 10);
    glVertex2f(rocket_1 - 45, rocket_1Y + 20);
    glEnd();

    //right fin
    glBegin(GL_TRIANGLES);
    glVertex2f(rocket_1 + 40, rocket_1Y - 10);
    glVertex2f(rocket_1 + 50, rocket_1Y - 10);
    glVertex2f(rocket_1 + 45, rocket_1Y + 20);
    glEnd();

    //nose-yellow
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(rocket_1 - 10, rocket_1Y + 10);
    glVertex2f(rocket_1 + 10, rocket_1Y + 10);
    glVertex2f(rocket_1, rocket_1Y + 30);
    glEnd();

    //flames-orange
    glColor3f(1.0, 0.5, 0.0);
    glBegin(GL_POLYGON);
    glVertex2f(rocket_1 - 10, rocket_1Y - 10);
    glVertex2f(rocket_1 + 10, rocket_1Y - 10);
    glVertex2f(rocket_1, rocket_1Y - 30);
    glEnd();

    //window-white
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(rocket_1 - 10, rocket_1Y + 10);
    glVertex2f(rocket_1 + 10, rocket_1Y + 10);
    glVertex2f(rocket_1, rocket_1Y + 20);
    glEnd();

    //stripe-blue
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(rocket_1 - 40, rocket_1Y);
    glVertex2f(rocket_1 + 40, rocket_1Y);
    glVertex2f(rocket_1 + 30, rocket_1Y + 5);
    glVertex2f(rocket_1 - 30, rocket_1Y + 5);
    glEnd();

    //flag-green
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(rocket_1, rocket_1Y + 10);
    glVertex2f(rocket_1, rocket_1Y + 30);
    glVertex2f(rocket_1 + 10, rocket_1Y + 20);
    glEnd();
}


void drawRocket2() {
    //rocket body-white
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(rocket_2 - 30, rocket_2Y - 5);
    glVertex2f(rocket_2 + 30, rocket_2Y - 5);
    glVertex2f(rocket_2 + 25, rocket_2Y + 50);
    glVertex2f(rocket_2 - 25, rocket_2Y + 50);
    glEnd();

    //first layer-black
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2f(rocket_2 - 25, rocket_2Y + 50);
    glVertex2f(rocket_2 + 25, rocket_2Y + 50);
    glVertex2f(rocket_2 + 20, rocket_2Y + 60);
    glVertex2f(rocket_2 - 20, rocket_2Y + 60);
    glEnd();

    //second layer-white
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(rocket_2 - 20, rocket_2Y + 60);
    glVertex2f(rocket_2 + 20, rocket_2Y + 60);
    glVertex2f(rocket_2 + 15, rocket_2Y + 70);
    glVertex2f(rocket_2 - 15, rocket_2Y + 70);
    glEnd();

    //nose cone-black
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(rocket_2 - 5, rocket_2Y + 70);
    glVertex2f(rocket_2 + 5, rocket_2Y + 70);
    glVertex2f(rocket_2, rocket_2Y + 80);
    glEnd();
}

void drawSun() {
    glColor3f(1.0, 1.0, 0.0); //yellow
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * PI / 180;
        float x = sunX - 300 + 40 * cos(angle);
        float y = sunY + 220 + 40 * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawHouse() {
    //house walls-brown
    glColor3f(0.647, 0.165, 0.165);
    glBegin(GL_POLYGON);
    glVertex2f(houseX, houseY);
    glVertex2f(houseX + houseWidth, houseY);
    glVertex2f(houseX + houseWidth, houseY + houseHeight);
    glVertex2f(houseX, houseY + houseHeight);
    glEnd();

    //house roof-red
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_POLYGON);
    glVertex2f(houseX - 20, houseY + houseHeight);
    glVertex2f(houseX + houseWidth + 20, houseY + houseHeight);
    glVertex2f(houseX + houseWidth / 2, houseY + houseHeight + 40);
    glEnd();

    //house door-black
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2f(houseX + houseWidth / 4, houseY);
    glVertex2f(houseX + houseWidth / 4 * 3, houseY);
    glVertex2f(houseX + houseWidth / 4 * 3, houseY + houseHeight / 2);
    glVertex2f(houseX + houseWidth / 4, houseY + houseHeight / 2);
    glEnd();

    //house window-white
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(houseX + houseWidth / 2 - 20, houseY + houseHeight / 2 - 10);
    glVertex2f(houseX + houseWidth / 2 + 20, houseY + houseHeight / 2 - 10);
    glVertex2f(houseX + houseWidth / 2 + 20, houseY + houseHeight / 2 + 10);
    glVertex2f(houseX + houseWidth / 2 - 20, houseY + houseHeight / 2 + 10);
    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();
    drawRocket();
    drawRocket2();
    drawSun();
    drawHouse();
    drawWindowFrame();

    glColor3f(0, 0, 0);

    glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y) {
    if (key == ' ')
        isMoving = !isMoving;

    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onSpecialKeyDown(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        moveRocketRight = false;
    }
    else if (key == GLUT_KEY_RIGHT) {
        moveRocketRight = true;
    }
    else if (key == GLUT_KEY_UP) {
        moveRocket2Up = true;
    }
    else if (key == GLUT_KEY_DOWN) {
        moveRocket2Up = false;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onResize(int w, int h)
{
    winWidth = WINDOW_WIDTH;
    winHeight = WINDOW_HEIGHT;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display();
}

void onClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        //new rocket position
        rocket_1 = (float)(x - winWidth / 1.9);
        rocket_1Y = (float)(winHeight / 2 - y);

        //rocket stays on boundaries
        if (rocket_1 < -winWidth / 1.3)
            rocket_1 = -winWidth / 1.3;
        else if (rocket_1 > winWidth / 1.3)
            rocket_1 = winWidth / 1.3;

        if (rocket_1Y < -winHeight / 1.3)
            rocket_1Y = -winHeight / 1.3;
        else if (rocket_1Y > winHeight / 1.3)
            rocket_1Y = winHeight / 1.3 - rocket_1Y;

        //rocket continue
        isMoving = true;

        // Refresh the display
        glutPostRedisplay();
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        //new rocket position
        rocket_2 = (float)(x - winWidth / 1.8);
        rocket_2Y = (float)(winHeight / 3.0 - y);

        //rocket stays on boundaries
        if (rocket_2 < -winWidth / 1.3)
            rocket_2 = -winWidth / 1.3;
        else if (rocket_2 > winWidth / 1.3)
            rocket_2 = winWidth / 1.3;

        if (rocket_2Y < -winHeight / 1.3)
            rocket_2Y = -winHeight / 1.3;
        else if (rocket_2Y > winHeight / 1.3)
            rocket_2Y = winHeight / 1.3;

        //rocket continue
        isMoving = true;

        // Refresh the display
        glutPostRedisplay();
    }
}

#if TIMER_ON == 1
void onTimer(int v) {
    if (isMoving) {
        //horizontal movement
        if (moveRocketRight) {
            rocket_1 += rocket1Speed;
            if (rocket_1 > WINDOW_WIDTH - 450)
                rocket_1 = -300;
        }
        else {
            rocket_1 -= rocket1Speed;
            if (rocket_1 < -300)
                rocket_1 = WINDOW_WIDTH - 650;
        }

        //vertical movement
        if (moveRocket2Up) {
            rocket_2Y += rocket2Speed;

            //reset bottom
            if (rocket_2Y > WINDOW_HEIGHT / 2) {
                rocket_2Y = -WINDOW_HEIGHT / 2;
            }
        }
        else {
            rocket_2Y -= rocket2Speed;
            //reset top
            if (rocket_2Y < -WINDOW_HEIGHT / 2) {
                rocket_2Y = WINDOW_HEIGHT / 2;
            }
        }
    }

    // Set the timer to call itself again
    glutTimerFunc(TIMER_PERIOD, onTimer, 1);
    glutPostRedisplay(); // display()
}
#endif

void Init() {
    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH + 200, WINDOW_HEIGHT);
    glutCreateWindow("Rockets with Mountain");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    // keyboard registration
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    // mouse registration
    glutMouseFunc(onClick);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();
    glutMainLoop();
}