#include <GL/freeglut_std.h>
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>

#define WINDOW_W 1200
#define WINDOW_H 750
#define FPS 60
#define COUNTDOWN_SECONDS 60 
#define SCORE_LIMIT 20

using namespace std;

static GLfloat screenBottom = -30.5;

class Bullet {
public:
    GLfloat x;
    GLfloat y;
    GLfloat speed;
    GLfloat size;

    Bullet(GLfloat _x, GLfloat _y, GLfloat _speed, GLfloat _size) : x(_x), y(_y), speed(_speed), size(_size) {}
};

std::vector<Bullet> bullets;

class Star {
public:
    GLfloat x;
    GLfloat y;
    GLfloat speed;
    GLfloat size;

    Star(GLfloat _x, GLfloat _y, GLfloat _speed, GLfloat _size) : x(_x), y(_y), speed(_speed), size(_size) {}
};

std::vector<Star> stars;

GLfloat rocketPosition = 0.0;
GLfloat rocketWidth = 7.0;
GLfloat rocketHeight = 13.0;
GLfloat rocketSpeed = 0.5;
int score = 0;

bool gamePaused = false;
bool gameOver = true;
bool scoreLimitExceeded = false;
time_t startTime;

void togglePause() {
    gamePaused = !gamePaused;
}

void drawRocket() {
    glBegin(GL_TRIANGLES);
    glColor3f(0.7f, 0.2f, 0.2f);
    glVertex2f(rocketPosition, screenBottom);
    glVertex2f(rocketPosition + rocketWidth / 2, screenBottom + rocketHeight);
    glVertex2f(rocketPosition + rocketWidth, screenBottom);
    glEnd();
}

void drawBullet(GLfloat x, GLfloat y, GLfloat size) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.5f, 0.5f, 1.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(size / 2, -size * 2);
    glVertex2f(-size / 2, -size * 2);
    glEnd();
    glPopMatrix();
}

void drawStar(GLfloat x, GLfloat y, GLfloat size) {
    const GLfloat outerRadius = size;
    const GLfloat innerRadius = size * 0.382;
    const GLfloat angle = 3.14159f * 2.0f / 5.0f;

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);

    for (int i = 0; i < 5; ++i) {
        GLfloat outerX = x + cos(angle * i) * outerRadius;
        GLfloat outerY = y + sin(angle * i) * outerRadius;
        glVertex2f(outerX, outerY);
        GLfloat innerX = x + cos(angle * i + angle / 2.0f) * innerRadius;
        GLfloat innerY = y + sin(angle * i + angle / 2.0f) * innerRadius;
        glVertex2f(innerX, innerY);
    }

    GLfloat lastOuterX = x + cos(angle * 0) * outerRadius;
    GLfloat lastOuterY = y + sin(angle * 0) * outerRadius;
    glVertex2f(lastOuterX, lastOuterY);
    glEnd();
}

void moveRocket(int direction) {
    rocketPosition += direction * rocketSpeed;
    if (rocketPosition < -50.0)
        rocketPosition = -50.0;
    else if (rocketPosition > 40.0)
        rocketPosition = 40.0;
}

void shootBullet() {
    bullets.push_back(Bullet(rocketPosition + rocketWidth / 2, screenBottom + rocketHeight, 1.0f, 2.0f));
}

void drawBullets() {
    for (size_t i = 0; i < bullets.size(); ++i) {
        drawBullet(bullets[i].x, bullets[i].y, bullets[i].size);
    }
}

void moveBullets() {
    for (size_t i = 0; i < bullets.size(); ++i) {
        bullets[i].y += bullets[i].speed;
        if (bullets[i].y > 50.0) {
            bullets.erase(bullets.begin() + i);
            --i;
        }
    }
}

void drawStars() {
    for (size_t i = 0; i < stars.size(); ++i) {
        drawStar(stars[i].x, stars[i].y, stars[i].size);
    }
}

void moveStars() {
    for (size_t i = 0; i < stars.size(); ++i) {
        stars[i].y -= stars[i].speed;
        if (stars[i].y < screenBottom) {
            stars.erase(stars.begin() + i);
            --i;
        }
    }
}

void generateStars() {
    if (rand() % 40 == 0) {
        for (int i = 0; i < 2; ++i) {
            GLfloat x = static_cast<GLfloat>(rand() % 101) - 50.0f;
            GLfloat y = 50.0f;
            GLfloat speed = static_cast<GLfloat>(rand() % 6) / 10.0f + 0.5f;
            GLfloat size = static_cast<GLfloat>(rand() % 3) + 1.0f;
            stars.push_back(Star(x, y, speed, size));
        }
    }
}

void checkCollisions() {
    for (size_t i = 0; i < bullets.size(); ++i) {
        for (size_t j = 0; j < stars.size(); ++j) {
            GLfloat distX = bullets[i].x - stars[j].x;
            GLfloat distY = bullets[i].y - stars[j].y;
            GLfloat distance = sqrt(distX * distX + distY * distY);
            if (distance < stars[j].size) {
                stars.erase(stars.begin() + j);
                bullets.erase(bullets.begin() + i);
                score += static_cast<int>(stars[j].size);
                break;
            }
        }
    }
}

void drawScore() {
    glPushMatrix();
    glLoadIdentity();
    GLfloat x = -49.0f;
    GLfloat y = 25.0f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    std::stringstream ss;
    ss << "Score: " << score;
    std::
        string scoreStr = ss.str();
    for (int i = 0; i < scoreStr.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
    }

    glPopMatrix();
}

void drawStatus() {
    glPushMatrix();
    glLoadIdentity();
    GLfloat x = -49.0f;
    GLfloat y = 23.0f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);

    std::stringstream ss;
    if (gamePaused) {
        ss << "Status: Paused";
    }
    else {
        ss << "Status: Playing";
    }
    std::string statusStr = ss.str();

    for (int i = 0; i < statusStr.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, statusStr[i]);
    }

    glPopMatrix();


}

void drawCountdown(int remainingSeconds) {
    glPushMatrix();
    glLoadIdentity();

    GLfloat x = 35.0f;
    GLfloat y = 25.0f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);

    std::stringstream ss;
    ss << "Time Left: " << remainingSeconds << "s";
    std::string countdownStr = ss.str();

    for (int i = 0; i < countdownStr.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, countdownStr[i]);
    }

    glPopMatrix();

}

void drawGameOver() {
    glPushMatrix();
    glLoadIdentity();

    GLfloat x = -10.0f;
    GLfloat y = 0.0f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);

    std::string gameOverStr = "Game Over! Your Score: " + std::to_string(score);
    for (int i = 0; i < gameOverStr.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, gameOverStr[i]);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y - 2.0f);
    std::string restartMsg = "Press Enter to restart";
    for (int i = 0; i < restartMsg.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, restartMsg[i]);
    }

    glPopMatrix();


}

void drawWelcomeScreen() {
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);

    GLfloat titleX = -15.0f;
    GLfloat titleY = 0.0f;
    glRasterPos2f(titleX, titleY);
    std::string titleText = "SPACE SHOOTER GAME";
    for (int i = 0; i < titleText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, titleText[i]);
    }

    GLfloat descriptionX = -22.0f;
    GLfloat descriptionY = -5.0f;
    glRasterPos2f(descriptionX, descriptionY);
    std::string descriptionText1 = "The aim of this game is to reach the limit score";
    std::string descriptionText2 = "by shooting stars in a certain time.";
    std::string descriptionText3 = "Enjoy!";
    for (int i = 0; i < descriptionText1.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, descriptionText1[i]);
    }
    glRasterPos2f(descriptionX, descriptionY - 2.0f);
    for (int i = 0; i < descriptionText2.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, descriptionText2[i]);
    }
    glRasterPos2f(descriptionX, descriptionY - 4.0f);
    for (int i = 0; i < descriptionText3.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, descriptionText3[i]);
    }

    GLfloat developerX = -15.0f;
    GLfloat developerY = -10.0f;
    glRasterPos2f(developerX, developerY);
    std::string developerText = "Developed by EDA NUR YARDIM";
    for (int i = 0; i < developerText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, developerText[i]);
    }

    GLfloat startTextX = -20.0f;
    GLfloat startTextY = -15.0f;
    glRasterPos2f(startTextX, startTextY);
    std::string startText = "Press 's' to start game";
    for (int i = 0; i < startText.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, startText[i]);
    }

    glPopMatrix();



}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gameOver) {
        drawWelcomeScreen();
    }
    else {
        drawRocket();
        drawBullets();
        drawStars();
        drawScore();

        if (gamePaused) {
            drawStatus();
            glPushMatrix();
            glLoadIdentity();
            glColor3f(1.0f, 1.0f, 1.0f);
            glRasterPos2f(35.0f, 25.0f);

            std::string pauseText = "Press p to pause";
            for (int i = 0; i < pauseText.length(); i++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, pauseText[i]);
            }

            glRasterPos2f(35.0f, 23.0f);
            std::string shootText = "Press space to shoot";
            for (int i = 0; i < shootText.length(); i++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, shootText[i]);
            }

            glRasterPos2f(35.0f, 21.0f);
            std::string exitText = "Press x to exit";
            for (int i = 0; i < exitText.length(); i++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, exitText[i]);
            }

            glPopMatrix();
        }

        if (gameOver) {
            drawGameOver();
        }
        else {
            time_t currentTime = time(NULL);
            int elapsedSeconds = difftime(currentTime, startTime);
            int remainingSeconds = COUNTDOWN_SECONDS - elapsedSeconds;
            drawCountdown(remainingSeconds);

            if (remainingSeconds <= 0) {
                gameOver = true;
            }



        }
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!gamePaused && !gameOver) {
        generateStars();
        moveStars();
        moveBullets();
        checkCollisions();
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, update, 0);
}

void processNormalKeys(unsigned char key, int x, int y) {
    if (key == 27) { // Escape key
        exit(0);
    }
    else if (key == 'p' || key == 'P') { // Pause key
        togglePause();
    }
    else if (key == 's' || key == 'S') { // Start key
        if (gameOver) {
            gameOver = false;
            score = 0;
            startTime = time(NULL);
        }
    }
    else if (key == ' ') { 
        if (!gamePaused && !gameOver) {
            shootBullet();
        }
    }
}

void processSpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        moveRocket(-1);
        break;
    case GLUT_KEY_RIGHT:
        moveRocket(1);
        break;
    }
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (width >= height) {
        gluOrtho2D(-50.0 * aspect, 50.0 * aspect, screenBottom, 50.0);
    }
    else {
        gluOrtho2D(-50.0, 50.0, screenBottom / aspect, 50.0 / aspect);
    }


}

void initGL() {
    glClearColor(1.0f, 0.75f, 0.8f, 1.0f); // Pembe rengi (RGB: 255, 192, 204)
}


int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(NULL)));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Space Shooter Game");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    glutTimerFunc(0, update, 0);
    initGL();
    glutMainLoop();
    return 0;
}