///////////////////////////////////////////////////////////////////////////////////      
// throwBall.cpp
//
// This program simulates a bouncing rubber ball using projectile motion equations.
// The ball bounces off the ground (y = 0 plane) with energy loss via a coefficient
// of restitution, and eventually comes to rest.
//
// Physics:
//   Horizontal: x = xOffset + h * t   (continuous across bounces)
//   Vertical:   y = vY * t - (1/2) * g * t^2   (reset t each bounce)
//   On ground collision: vY = -vY_impact * restitution  (energy loss)
//   Stop condition: |vY| < threshold
//
// Interaction:
// Press space to toggle between animation on and off.
// Press right/left arrow keys to increase/decrease the initial horizontal velocity.
// Press up/down arrow keys to increase/decrease the initial vertical velocity.
// Press page up/down keys to increase/decrease gravitational acceleration.
// Press r to reset.
//
// Sumanta Guha. (Modified for bouncing ball simulation)
///////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static int isAnimate = 0;          // Animated?
static int animationPeriod = 50;   // Time interval between frames (ms).
static float t = 0.0;              // Time parameter (resets each bounce).
static float h = 0.5;              // Horizontal component of initial velocity.
static float v = 4.0;              // Initial vertical velocity (saved for reset).
static float g = 0.2;              // Gravitational acceleration.

// Bouncing physics variables.
static float currentVelocityY = 4.0f;  // Current vertical velocity at start of each bounce arc.
static float xOffset = 0.0f;           // Accumulated horizontal displacement across bounces.
static float restitution = 0.7f;       // Coefficient of restitution (energy loss per bounce).
static int isStopped = 0;             // Whether the ball has come to rest.
static float restX = 0.0;             // X position where ball came to rest.

static char theStringBuffer[10]; // String buffer.
static void* font = GLUT_BITMAP_8_BY_13; // Font selection.

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, const char *string)
{
    const char *c;

	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Routine to convert floating point to char string.
void floatToString(char * destStr, int precision, float val)
{
	sprintf(destStr, "%f", val);
	destStr[precision] = '\0';
}

// Write data.
void writeData(void)
{
	glColor3f(0.0f, 0.0f, 0.0f);

	floatToString(theStringBuffer, 4, h);
	glRasterPos3f(-4.5f, 4.5f, -5.1f);
	writeBitmapString(font, "Horizontal velocity: ");
	writeBitmapString(font, theStringBuffer);

	floatToString(theStringBuffer, 4, currentVelocityY);
	glRasterPos3f(-4.5f, 4.2f, -5.1f);
	writeBitmapString(font, "Current vertical velocity: ");
	writeBitmapString(font, theStringBuffer);

	floatToString(theStringBuffer, 4, g);
	glRasterPos3f(-4.5f, 3.9f, -5.1f);
	writeBitmapString(font, "Gravitation: ");
	writeBitmapString(font, theStringBuffer);

	floatToString(theStringBuffer, 4, restitution);
	glRasterPos3f(-4.5f, 3.6f, -5.1f);
	writeBitmapString(font, "Restitution: ");
	writeBitmapString(font, theStringBuffer);
}

// Draw a ground line for visual clarity.
void drawGround(void)
{
	glColor3f(0.3f, 0.6f, 0.3f); // Green ground line.
	glLineWidth(2.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);    // Ground is at y = 0 in scene coords.
		glVertex3f(40.0f, 0.0f, 0.0f);   // Extend far to the right.
	glEnd();
	glLineWidth(1.0f);
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	writeData();

	// Place scene in frustum: shift so y=0 is near the bottom of the view.
	glTranslatef(-15.0f, -15.0f, -25.0f);

	// Draw the ground line at y = 0.
	drawGround();

	// Compute current ball position using equations of motion.
	float currentX, currentY;

	if (isStopped)
	{
		// Ball has come to rest on the ground.
		currentX = restX;
		currentY = 0.0;
	}
	else
	{
		// Horizontal: continuous across bounces.
		currentX = xOffset + h * t;

		// Vertical: y = vY * t - (g/2) * t^2 (standard projectile equation).
		currentY = currentVelocityY * t - (g / 2.0f) * t * t;

		// Clamp ball so it never visually goes below ground.
		if (currentY < 0.0f) currentY = 0.0f;
	}

	// Position and draw the ball.
	glPushMatrix();
	glTranslatef(currentX, currentY, 0.0f);
	glColor3f(0.8f, 0.2f, 0.2f); // Red rubber ball.
	glutSolidSphere(1.0, 20, 20);
	// Wire overlay for visual detail.
	glColor3f(0.5f, 0.1f, 0.1f);
	glutWireSphere(1.02, 12, 12);
	glPopMatrix();

	glutSwapBuffers();
}

// Timer function: advances physics each frame.
void animate(int value)
{
	if (isAnimate)
	{
		if (!isStopped)
		{
			t += 0.5; // Time step increment.

			// Compute current vertical position: y = vY * t - (g/2) * t^2
			float currentY = currentVelocityY * t - (g / 2.0f) * t * t;

			// Ground collision detection: ball hits ground when y <= 0 (after launch).
			if (currentY <= 0.0 && t > 0.0)
			{
				// Compute vertical velocity at moment of impact: vY_impact = vY - g * t
				float impactVelocityY = currentVelocityY - g * t;

				// Apply coefficient of restitution: reverse and dampen vertical velocity.
				float newVelocityY = -impactVelocityY * restitution;

				// Accumulate horizontal offset so x motion is continuous.
				xOffset += h * t;

				// Stop condition: if the new bounce velocity is too small, ball comes to rest.
				if (fabs(newVelocityY) < 0.1)
				{
					isStopped = 1;
					restX = xOffset; // Remember the final X position.
				}
				else
				{
					// Prepare for next bounce arc.
					currentVelocityY = newVelocityY;
					t = 0.0; // Reset time for new parabolic arc.
				}
			}
		}

		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		if (isAnimate) isAnimate = 0;
		else
		{
			isAnimate = 1;
			animate(1);
		}
		break;
	case 'r':
		// Full reset: restore all state to initial values.
		isAnimate = 0;
		t = 0.0;
		currentVelocityY = v;
		xOffset = 0.0;
		isStopped = 0;
		restX = 0.0;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) { v += 0.05f; currentVelocityY = v; }
	if (key == GLUT_KEY_DOWN) { if (v > 0.1f) v -= 0.05f; currentVelocityY = v; }
	if (key == GLUT_KEY_RIGHT) h += 0.05f;
	if (key == GLUT_KEY_LEFT) { if (h > 0.1f) h -= 0.05f; }
	if (key == GLUT_KEY_PAGE_UP) g += 0.05f;
	if (key == GLUT_KEY_PAGE_DOWN) { if (g > 0.1f) g -= 0.05f; }

	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press space to toggle between animation on and off." << std::endl
		<< "Press right/left arrow keys to increase/decrease the initial horizontal velocity." << std::endl
		<< "Press up/down arrow keys to increase/decrease the initial vertical velocity." << std::endl
		<< "Press page up/down keys to increase/decrease gravitational acceleration." << std::endl
		<< "Press r to reset." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Bouncing Ball Simulation");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

