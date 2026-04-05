///////////////////////////////////////////////////////////////////////////////////      
// throwBall.cpp
//
// This program simulates a bouncing rubber ball using projectile motion equations.
// The ball bounces off the ground (y = 0 plane) with energy loss via a coefficient
// of restitution, and eventually comes to rest.
//
// Physics:
//   Horizontal: x = xOffset + h * power * t   (continuous across bounces)
//   Vertical:   y = vY * power * t - (1/2) * g * mass * t^2  (reset t each bounce)
//   On ground collision: vY = -vY_impact * restitution  (energy loss)
//   Stop condition: |vY| < threshold
//
// Interaction:
//   Space       - Toggle animation on/off
//   R           - Reset all parameters
//   Arrow Up/Down    - Increase/decrease initial vertical velocity
//   Arrow Left/Right - Increase/decrease initial horizontal velocity
//   Page Up/Down     - Increase/decrease gravitational acceleration
//   W / S       - Increase/decrease ball mass (weight)
//   Q / A       - Increase/decrease ball power (launch strength)
//   E / D       - Increase/decrease restitution (bounciness)
//   1,2,3,4     - Switch background theme
//   0           - No background (white)
//   ESC         - Exit
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

// Window dimensions (updated in resize).
static int winW = 800;
static int winH = 600;

// Globals.
static int isAnimate = 0;          // Animated?
static int animationPeriod = 50;   // Time interval between frames (ms).
static float t = 0.0f;             // Time parameter (resets each bounce).
static float h = 0.5f;             // Horizontal component of initial velocity.
static float v = 4.0f;             // Initial vertical velocity (saved for reset).
static float g = 0.2f;             // Gravitational acceleration.

// Bouncing physics variables.
static float currentVelocityY = 4.0f;  // Current vertical velocity at start of each bounce arc.
static float xOffset = 0.0f;           // Accumulated horizontal displacement across bounces.
static float restitution = 0.7f;       // Coefficient of restitution (energy loss per bounce).
static int isStopped = 0;              // Whether the ball has come to rest.
static float restX = 0.0f;             // X position where ball came to rest.

// New physics parameters.
static float mass = 1.0f;              // Ball mass/weight: heavier = falls faster.
static float power = 1.0f;             // Launch power: multiplier on initial velocity.

// Background system: 0 = none (white), 1-4 = gradient themes.
static int currentBG = 1;
static GLuint bgTextures[4];
static const int BG_TEX_W = 256;
static const int BG_TEX_H = 256;

static char theStringBuffer[10]; // String buffer.
static void* fontSmall = GLUT_BITMAP_8_BY_13;
static void* fontLarge = GLUT_BITMAP_HELVETICA_12;

// ---------------------------------------------------------------------------
// Helper: linear interpolation.
// ---------------------------------------------------------------------------
static float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

// ---------------------------------------------------------------------------
// Generate a procedural gradient background texture.
// Each theme produces a 256x256 RGBA texture with a vertical gradient.
// ---------------------------------------------------------------------------
static GLuint createGradientTexture(int theme)
{
	unsigned char* pixels = new unsigned char[BG_TEX_W * BG_TEX_H * 3];

	for (int y = 0; y < BG_TEX_H; y++)
	{
		float frac = (float)y / (float)(BG_TEX_H - 1); // 0 = bottom, 1 = top
		float r, gv, b;

		switch (theme)
		{
		case 0: // Sky blue gradient
			r = lerp(0.85f, 0.30f, frac);
			gv = lerp(0.92f, 0.55f, frac);
			b = lerp(0.95f, 0.90f, frac);
			break;
		case 1: // Sunset orange-purple
			r = lerp(0.95f, 0.20f, frac);
			gv = lerp(0.55f, 0.10f, frac);
			b = lerp(0.20f, 0.40f, frac);
			break;
		case 2: // Space dark blue-black
			r = lerp(0.02f, 0.00f, frac);
			gv = lerp(0.05f, 0.00f, frac);
			b = lerp(0.15f, 0.05f, frac);
			break;
		case 3: // Green nature
			r = lerp(0.35f, 0.60f, frac);
			gv = lerp(0.55f, 0.85f, frac);
			b = lerp(0.25f, 0.95f, frac);
			break;
		default:
			r = gv = b = 1.0f;
			break;
		}

		for (int x = 0; x < BG_TEX_W; x++)
		{
			int idx = (y * BG_TEX_W + x) * 3;
			pixels[idx + 0] = (unsigned char)(r * 255.0f);
			pixels[idx + 1] = (unsigned char)(gv * 255.0f);
			pixels[idx + 2] = (unsigned char)(b * 255.0f);
		}
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BG_TEX_W, BG_TEX_H, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	delete[] pixels;
	return tex;
}

// ---------------------------------------------------------------------------
// Draw fullscreen textured quad as background (behind everything).
// ---------------------------------------------------------------------------
void drawBackground(void)
{
	if (currentBG == 0) return; // No background, use white clear color.

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bgTextures[currentBG - 1]);
	glColor3f(1.0f, 1.0f, 1.0f); // No tint.

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// ---------------------------------------------------------------------------
// Routine to draw a bitmap character string.
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Draw 2D overlay text at pixel position (x, y) from bottom-left.
// ---------------------------------------------------------------------------
void drawText2D(float x, float y, void* f, const char* text)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, (double)winW, 0.0, (double)winH, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2f(x, y);
	writeBitmapString(f, text);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// ---------------------------------------------------------------------------
// Write HUD data (parameter values) in top-left corner.
// ---------------------------------------------------------------------------
void writeData(void)
{
	// Choose text color based on background brightness.
	if (currentBG == 2) // dark space bg
		glColor3f(1.0f, 1.0f, 1.0f);
	else
		glColor3f(0.0f, 0.0f, 0.0f);

	float startY = (float)winH - 20.0f;
	float lineH = 16.0f;
	float x = 10.0f;

	floatToString(theStringBuffer, 4, h);
	drawText2D(x, startY, fontSmall, "Horiz. velocity (Left/Right): ");
	drawText2D(x + 210.0f, startY, fontSmall, theStringBuffer);

	floatToString(theStringBuffer, 4, v);
	drawText2D(x, startY - lineH, fontSmall, "Vert. velocity (Up/Down): ");
	drawText2D(x + 210.0f, startY - lineH, fontSmall, theStringBuffer);

	floatToString(theStringBuffer, 4, g);
	drawText2D(x, startY - 2.0f * lineH, fontSmall, "Gravity (PgUp/PgDn): ");
	drawText2D(x + 210.0f, startY - 2.0f * lineH, fontSmall, theStringBuffer);

	floatToString(theStringBuffer, 4, restitution);
	drawText2D(x, startY - 3.0f * lineH, fontSmall, "Restitution (E/D): ");
	drawText2D(x + 210.0f, startY - 3.0f * lineH, fontSmall, theStringBuffer);

	floatToString(theStringBuffer, 4, mass);
	drawText2D(x, startY - 4.0f * lineH, fontSmall, "Mass/Weight (W/S): ");
	drawText2D(x + 210.0f, startY - 4.0f * lineH, fontSmall, theStringBuffer);

	floatToString(theStringBuffer, 4, power);
	drawText2D(x, startY - 5.0f * lineH, fontSmall, "Power/Strength (Q/A): ");
	drawText2D(x + 210.0f, startY - 5.0f * lineH, fontSmall, theStringBuffer);

	// Bottom-left: controls legend.
	if (currentBG == 2)
		glColor3f(0.8f, 0.8f, 0.8f);
	else
		glColor3f(0.3f, 0.3f, 0.3f);

	drawText2D(10.0f, 52.0f, fontSmall, "Space=Play/Pause  R=Reset  1-4=Background  0=None");
	drawText2D(10.0f, 36.0f, fontSmall, "Arrows=Velocity  PgUp/Dn=Gravity  W/S=Mass  Q/A=Power  E/D=Bounce");
	drawText2D(10.0f, 20.0f, fontSmall, "ESC=Exit");
}

// ---------------------------------------------------------------------------
// Draw a ground line for visual clarity.
// ---------------------------------------------------------------------------
void drawGround(void)
{
	// Ground color adapts to background theme.
	if (currentBG == 2)
		glColor3f(0.3f, 0.3f, 0.5f);
	else if (currentBG == 4)
		glColor3f(0.2f, 0.45f, 0.15f);
	else
		glColor3f(0.3f, 0.6f, 0.3f);

	glLineWidth(2.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(40.0f, 0.0f, 0.0f);
	glEnd();
	glLineWidth(1.0f);
}

// ---------------------------------------------------------------------------
// Drawing routine.
// ---------------------------------------------------------------------------
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	// Draw background gradient first (behind everything).
	drawBackground();

	// Draw HUD text overlay.
	writeData();

	// Place scene in frustum: shift so y=0 is near the bottom of the view.
	glTranslatef(-15.0f, -15.0f, -25.0f);

	// Draw the ground line at y = 0.
	drawGround();

	// Effective gravity accounting for mass.
	float effectiveG = g * mass;

	// Compute current ball position using equations of motion.
	float currentX, currentY;

	if (isStopped)
	{
		// Ball has come to rest on the ground.
		currentX = restX;
		currentY = 0.0f;
	}
	else
	{
		// Horizontal: continuous across bounces, scaled by power.
		currentX = xOffset + h * power * t;

		// Vertical: y = vY * t - (effectiveG/2) * t^2 (standard projectile equation).
		currentY = currentVelocityY * t - (effectiveG / 2.0f) * t * t;

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

// ---------------------------------------------------------------------------
// Timer function: advances physics each frame.
// ---------------------------------------------------------------------------
void animate(int value)
{
	if (isAnimate)
	{
		if (!isStopped)
		{
			t += 0.5f; // Time step increment.

			// Effective gravity = base gravity * mass.
			float effectiveG = g * mass;

			// Compute current vertical position: y = vY * t - (effectiveG/2) * t^2
			float currentY = currentVelocityY * t - (effectiveG / 2.0f) * t * t;

			// Ground collision detection: ball hits ground when y <= 0 (after launch).
			if (currentY <= 0.0f && t > 0.0f)
			{
				// Compute vertical velocity at moment of impact: vY_impact = vY - effectiveG * t
				float impactVelocityY = currentVelocityY - effectiveG * t;

				// Apply coefficient of restitution: reverse and dampen vertical velocity.
				float newVelocityY = -impactVelocityY * restitution;

				// Accumulate horizontal offset so x motion is continuous.
				xOffset += h * power * t;

				// Stop condition: if the new bounce velocity is too small, ball comes to rest.
				if (fabs(newVelocityY) < 0.1f)
				{
					isStopped = 1;
					restX = xOffset; // Remember the final X position.
				}
				else
				{
					// Prepare for next bounce arc.
					currentVelocityY = newVelocityY;
					t = 0.0f; // Reset time for new parabolic arc.
				}
			}
		}

		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}
}

// ---------------------------------------------------------------------------
// Initialization routine.
// ---------------------------------------------------------------------------
void setup(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Generate procedural background textures.
	for (int i = 0; i < 4; i++)
		bgTextures[i] = createGradientTexture(i);
}

// ---------------------------------------------------------------------------
// OpenGL window reshape routine.
// ---------------------------------------------------------------------------
void resize(int w, int h)
{
	winW = w;
	winH = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

// ---------------------------------------------------------------------------
// Keyboard input processing routine.
// ---------------------------------------------------------------------------
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
	case 'r': case 'R':
		// Full reset: restore all state to initial values.
		isAnimate = 0;
		t = 0.0f;
		h = 0.5f;
		v = 4.0f;
		g = 0.2f;
		currentVelocityY = v * power;
		xOffset = 0.0f;
		isStopped = 0;
		restX = 0.0f;
		mass = 1.0f;
		power = 1.0f;
		restitution = 0.7f;
		currentVelocityY = v * power;
		glutPostRedisplay();
		break;

	// Mass/Weight controls.
	case 'w': case 'W':
		mass += 0.1f;
		glutPostRedisplay();
		break;
	case 's': case 'S':
		if (mass > 0.2f) mass -= 0.1f;
		glutPostRedisplay();
		break;

	// Power/Strength controls.
	case 'q': case 'Q':
		power += 0.1f;
		if (!isAnimate && !isStopped) currentVelocityY = v * power;
		glutPostRedisplay();
		break;
	case 'a': case 'A':
		if (power > 0.2f) power -= 0.1f;
		if (!isAnimate && !isStopped) currentVelocityY = v * power;
		glutPostRedisplay();
		break;

	// Restitution (bounciness) controls.
	case 'e': case 'E':
		if (restitution < 0.95f) restitution += 0.05f;
		glutPostRedisplay();
		break;
	case 'd': case 'D':
		if (restitution > 0.1f) restitution -= 0.05f;
		glutPostRedisplay();
		break;

	// Background selection.
	case '0': currentBG = 0; glutPostRedisplay(); break;
	case '1': currentBG = 1; glutPostRedisplay(); break;
	case '2': currentBG = 2; glutPostRedisplay(); break;
	case '3': currentBG = 3; glutPostRedisplay(); break;
	case '4': currentBG = 4; glutPostRedisplay(); break;

	default:
		break;
	}
}

// ---------------------------------------------------------------------------
// Callback routine for non-ASCII key entry.
// ---------------------------------------------------------------------------
void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) { v += 0.05f; if (!isAnimate && !isStopped) currentVelocityY = v * power; }
	if (key == GLUT_KEY_DOWN) { if (v > 0.1f) v -= 0.05f; if (!isAnimate && !isStopped) currentVelocityY = v * power; }
	if (key == GLUT_KEY_RIGHT) h += 0.05f;
	if (key == GLUT_KEY_LEFT) { if (h > 0.1f) h -= 0.05f; }
	if (key == GLUT_KEY_PAGE_UP) g += 0.05f;
	if (key == GLUT_KEY_PAGE_DOWN) { if (g > 0.1f) g -= 0.05f; }

	glutPostRedisplay();
}

// ---------------------------------------------------------------------------
// Routine to output interaction instructions to the C++ window.
// ---------------------------------------------------------------------------
void printInteraction(void)
{
	std::cout << "=== Bouncing Ball Simulation ===" << std::endl;
	std::cout << std::endl;
	std::cout << "Controls:" << std::endl;
	std::cout << "  Space        - Play / Pause animation" << std::endl;
	std::cout << "  R            - Reset all parameters" << std::endl;
	std::cout << "  Up/Down      - Increase/decrease vertical velocity" << std::endl;
	std::cout << "  Left/Right   - Increase/decrease horizontal velocity" << std::endl;
	std::cout << "  Page Up/Down - Increase/decrease gravity" << std::endl;
	std::cout << "  W / S        - Increase/decrease mass (weight)" << std::endl;
	std::cout << "  Q / A        - Increase/decrease power (launch strength)" << std::endl;
	std::cout << "  E / D        - Increase/decrease restitution (bounciness)" << std::endl;
	std::cout << "  1, 2, 3, 4   - Switch background theme" << std::endl;
	std::cout << "  0            - No background (white)" << std::endl;
	std::cout << "  ESC          - Exit" << std::endl;
	std::cout << std::endl;
}

// ---------------------------------------------------------------------------
// Main routine.
// ---------------------------------------------------------------------------
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(winW, winH);
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

