#include <GL/freeglut.h>
#include <iostream>
#include <cstdlib>
#include <ctime>;
#include <math.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"               //unsigned char *texture = stbi_load(fliename,&width,&height,&imgchannel,0)
/*  Initialize material property, light source, lighting model,
 *  and depth buffer.
 */
bool startMovement = false;
bool texswitch = true;
bool Simple_Smooth = false;
int mouseCurPositionX = 0;
int mouseCurPositionY = 0;
int screenW = 900;
int screenH = 700;
enum Camera_type{
    Rotate,
    Translate,
    Zoom
};
enum Texture {
    Wall,
    Floor,
    Glass,
    Pillow,
    Bookshell3,
    BlackGray,
    Bookshell1,
    Bed
};
Camera_type CamState;
GLfloat CamPos[3] = { 0.0, -1.0, 0.0 };
//GLfloat CamFront[3] = { 0.0, 0.0, -1.0 };
//GLfloat CamSide[3] = { 1.0, 0.0, 0.0 };
//GLfloat CamUp[3] = { 0.0, 1.0, 0.0 };
GLfloat Xangle = 0.0;
GLfloat Yangle = 0.0;
GLfloat fovy = 60.0;
GLuint tb1[8] = { 0 };
unsigned short int books[5] = { 0 };
GLfloat vertices[][3] = {
    {-1.0,-1.0,-1.0},{ 1.0,-1.0,-1.0},
    { 1.0, 1.0,-1.0},{-1.0, 1.0,-1.0},
    {-1.0,-1.0, 1.0},{ 1.0,-1.0, 1.0},
    { 1.0, 1.0, 1.0},{-1.0, 1.0, 1.0}
};
GLfloat vertices2[][3] = {
    {-0.5,-0.5,-0.5},{ 0.5,-0.5,-0.5},
    { 0.5, 0.5,-0.5},{-0.5, 0.5,-0.5},
    {-0.5,-0.5, 0.5},{ 0.5,-0.5, 0.5},
    { 0.5, 0.5, 0.5},{-0.5, 0.5, 0.5}
};
void tex_polygon(int a, int b, int c, int d,int s=1) {
    GLfloat nx=0, ny=0, nz=0;
    if (a == 0) {
        if (b == 1)
            nz = -1.0;
        else if (b == 3)
            nx = -1.0;
        else if (b == 4)
            ny = -1.0;
    }
    else if (a == 1) {
        nx = 1.0;
    }
    else if (a == 2) {
        ny = 1.0;
    }
    else if (a == 4) {
        nz = 1.0;
    }
    glBegin(GL_POLYGON);
    glNormal3f(nx, ny, nz);
    if (s <= 1) {
        glTexCoord2f(0.0, 0.0); glVertex3fv(vertices[a]);
        glTexCoord2f(1.0, 0.0); glVertex3fv(vertices[b]);
        glTexCoord2f(1.0, 1.0); glVertex3fv(vertices[c]);
        glTexCoord2f(0.0, 1.0); glVertex3fv(vertices[d]);
    }
    else {
        glTexCoord2f(0.0, 0.0); glVertex3fv(vertices2[a]);
        glTexCoord2f(1.0, 0.0); glVertex3fv(vertices2[b]);
        glTexCoord2f(1.0, 1.0); glVertex3fv(vertices2[c]);
        glTexCoord2f(0.0, 1.0); glVertex3fv(vertices2[d]);
    }
    glEnd();
}
void cubeL(GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    tex_polygon(0, 1, 2, 3);
    tex_polygon(2, 6, 7, 3);
    tex_polygon(0, 3, 7, 4);
    tex_polygon(1, 5, 6, 2);
    tex_polygon(4, 7, 6, 5);
    tex_polygon(0, 4, 5, 1);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void cubeS(GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    tex_polygon(0, 1, 2, 3, 2);
    tex_polygon(2, 6, 7, 3, 2);
    tex_polygon(0, 3, 7, 4, 2);
    tex_polygon(1, 5, 6, 2, 2);
    tex_polygon(4, 7, 6, 5, 2);
    tex_polygon(0, 4, 5, 1, 2);
    glBindTexture(GL_TEXTURE_2D, 0);
}
GLfloat* matV(GLfloat r, GLfloat g, GLfloat b, GLfloat w) {

    GLfloat v[] = { r,g,b,w };
    return v;
}
// initialize texture files
void InitTexture() {

    unsigned char* data1;
    char resource[][20] = { "wall.jpg","floor.jpg","roughglass.jpg","pillow.jpg","bookshell3.jpg","blackgray.jpg","bookshell1.jpg","bed.jpg"};
    
    int width, height, channel;
    glGenTextures(8, tb1);
    for (int i = 0; i < (sizeof(tb1)/sizeof(GLuint)); i++) {
        glBindTexture(GL_TEXTURE_2D, tb1[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        data1 = stbi_load(resource[i], &width, &height, &channel, 0);
      
        if (data1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
            std::cout << "이미지 "<<i<<" 로드 성공! 폭: " << width << " 높이: " << height << "\n";
        }
        else {
            std::cout << "이미지 로드 실패\n";
        }
        stbi_image_free(data1);

    }
}

void init(void)
{
    srand((unsigned int)time(NULL));
   
    for (int i = 0; i < 5; i++) {
        int num = rand();
        books[i] = num % 8;
    }
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    InitTexture();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    std::cout << "초기화 완료\n";
}

void normalize(GLfloat v[3]) {
    GLfloat a = 1.0 / sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}

void crossfunc(GLfloat target[3], GLfloat v1[3], GLfloat v2[3]) {
    target[0] = v1[1] * v2[2] - v1[2] * v2[1];
    target[1] = v1[0] * v2[2] - v1[2] * v2[0];
    target[2] = v1[0] * v2[1] - v1[1] * v2[0];
    normalize(target);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glRotatef(-Xangle,1.0,0.0,0.0);
    glRotatef(-Yangle, 0.0, 1.0, 0.0);
    glTranslatef(CamPos[0], CamPos[1], CamPos[2]);
    //light0
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_AMBIENT, matV(0.05, 0.05, 0.05, 1.0));
    glLightfv(GL_LIGHT0, GL_DIFFUSE, matV(0.4, 0.4, 0.4, 1.0));
    glLightfv(GL_LIGHT0, GL_SPECULAR, matV(0.6, 0.6, 0.6, 1.0));
    glLightfv(GL_LIGHT0, GL_POSITION, matV(0.0, 3.0, 0.0, 1.0));
    glPopMatrix();
    
    //back wall
    glPushMatrix();
    glTranslatef(0.0, 1.0, -2.0);
    glScalef(2.0, 2.0, 0.1);
    if (texswitch)
        cubeL(tb1[Wall]);
    else
        glutSolidCube(2.0);
    //glutSolidCube(2.0);
    glPopMatrix();
    //right wall
    glPushMatrix();
    glTranslatef(2.0, 1.0, 0.0);
    glScalef(0.1, 2.0, 2.0);
    if (texswitch)
        cubeL(tb1[Wall]);
    else
        glutSolidCube(2.0);
    //glutSolidCube(2.0);
    glPopMatrix();
    //left wall
    glPushMatrix();
    glTranslatef(-2.0, 1.0, 0.0);
    glScalef(0.1, 2.0, 2.0);
    if (texswitch)
        cubeL(tb1[Wall]);
    else
        glutSolidCube(2.0);
    glPopMatrix();
    //down wall
    glPushMatrix();
    glTranslatef(0.0, -1.0, 0.0);
    glScalef(2.0, 0.1, 2.0);
    if (texswitch)
        cubeL(tb1[Floor]);
    else
        glutSolidCube(2.0);
    //glutSolidCube(2.0);
    glPopMatrix();

    //front wall
    glPushMatrix();
    glTranslatef(0.0, 1.0, 2.0);
    glScalef(2.0, 2.0, 0.1);
    if (texswitch)
        cubeL(tb1[Glass]);
    else
        glutSolidCube(2.0);
    glPopMatrix();
   
    //up wall
    glPushMatrix();
    glTranslatef(0.0, 3.0, 0.0);
    glScalef(2.0, 0.1, 2.0);
    if (texswitch)
        cubeL(tb1[Wall]);
    else
        glutSolidCube(2.0);
    glPopMatrix();

    //bed
    glPushMatrix();
    glTranslatef(-1.0, -0.6, -0.8);
    //bed head
    glPushMatrix();
    glTranslatef(1.0, 0.1, 0.0);
    glScalef(0.05, 1.0, 2.2);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //bed body
    glPushMatrix();
    glScalef(2.0, 0.6, 2.2);
    if (texswitch)
        cubeS(tb1[Bed]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //pllow
    glPushMatrix();
    glTranslatef(0.71, 0.25, 0.0);
    glScalef(0.5, 0.4, 0.8);
    if (texswitch)
        cubeS(tb1[Pillow]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //이불
    glPushMatrix();
    glTranslatef(-0.8, 0.1, 0.0);
    glScalef(2.1, 0.6, 2.3);
    if (texswitch)
        cubeS(tb1[Pillow]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix(); // bed
    
    //book shell 1
    glPushMatrix();
    glTranslatef(0.56, 0.0, -1.8);
    //back
    glPushMatrix();
    glScalef(1.0, 2.2, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side1
    glPushMatrix();
    glTranslatef(-0.46, 0.0, 0.1);
    glScalef(0.1, 2.2, 0.6);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side2
    glPushMatrix();
    glTranslatef(0.44, 0.0, 0.1);
    glScalef(0.1, 2.2, 0.6);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack1
    glPushMatrix();
    glTranslatef(0.0, 1.06, 0.2);
    glScalef(1.01, 0.1, 0.6);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack2
    glPushMatrix();
    glTranslatef(0.01, 0.6, 0.2);
    glScalef(0.9, 0.1, 0.5);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack3
    glPushMatrix();
    glTranslatef(0.01, 0.1, 0.2);
    glScalef(0.9, 0.1, 0.5);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack3
    glPushMatrix();
    glTranslatef(0.01, -0.4, 0.2);
    glScalef(0.9, 0.1, 0.5);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack4
    glPushMatrix();
    glTranslatef(0.01, -0.85, 0.2);
    glScalef(1.0, 0.1, 0.5);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //books
    GLfloat index = -0.3;
    GLfloat index2 = 0.8;
    for(int j=0;j<4;j++)
    for (int i = 0; index + i * 0.15 < 0.3; i++) {
        glPushMatrix(); 
        
        glTranslatef((index + i * 0.15), 0.8-(j*0.5), 0.2);
        glScalef(0.1, 0.3, 0.3);
        if (texswitch)
            cubeS(tb1[books[i]]);
        else
            glutSolidCube(1.0);
        glPopMatrix();
    }
    
    glPopMatrix(); // book shell 1

    //book shell 2
    //back
    glPushMatrix();
    glTranslatef(1.85, -0.4, -1.4);
    glScalef(0.1, 1.0, 1.0);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side1
    glPushMatrix();
    glTranslatef(1.6, -0.4, -1.8);
    glScalef(0.4, 1.0, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side2
    glPushMatrix();
    glTranslatef(1.6, -0.4, -1.3);
    glScalef(0.4, 1.0, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side3
    glPushMatrix();
    glTranslatef(1.6, -0.4, -0.85);
    glScalef(0.4, 1.0, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack1
    glPushMatrix();
    glTranslatef(1.6, 0.1, -1.33);
    glScalef(0.4, 0.1, 1.02);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack2
    glPushMatrix();
    glTranslatef(1.6, -0.35, -1.33);
    glScalef(0.4, 0.1, 1.02);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack3
    glPushMatrix();
    glTranslatef(1.6, -0.85, -1.33);
    glScalef(0.4, 0.1, 1.02);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix(); //book shell2

    //마우스 장패드
    glPushMatrix();
   
    glTranslatef(1.4, 0.25, 0.6);
    glScalef(0.6, 0.01, 1.6);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();

    //table
    glPushMatrix();
   
    glTranslatef(1.4, 0.2, 0.4);
    //up
    glPushMatrix();
    glTranslatef(-0.1, 0.0, 0.2);
    glScalef(0.9, 0.1, 2.6);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //drawer
    glPushMatrix();
    glTranslatef(0.0, -0.6, -0.65);
    glScalef(0.8, 1.0, 0.8);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack1
    glPushMatrix();
    glTranslatef(-0.4, -0.15, -0.65);
    //handler
    glPushMatrix();
    glTranslatef(-0.1, 0.0, 0.0);
    glScalef(0.03, 0.03, 0.03);
    glutSolidSphere(1.0, 8, 8);
    glPopMatrix();
    glScalef(0.1, 0.2, 0.75);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack2
    glPushMatrix();
    glTranslatef(-0.4, -0.4, -0.65);
    //handler
    glPushMatrix();
    glTranslatef(-0.1, 0.0, 0.0);
    glScalef(0.03, 0.03, 0.03);
    glutSolidSphere(1.0, 8, 8);
    glPopMatrix();
    glScalef(0.1, 0.2, 0.75);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack3
    glPushMatrix();
    glTranslatef(-0.4, -0.65, -0.65);
    //handler
    glPushMatrix();
    glTranslatef(-0.1, 0.0, 0.0);
    glScalef(0.03, 0.03, 0.03);
    glutSolidSphere(1.0, 8, 8);
    glPopMatrix();
    glScalef(0.1, 0.2, 0.75);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack4
    glPushMatrix();
    glTranslatef(-0.4, -0.9, -0.65);
    //handler
    glPushMatrix();
    glTranslatef(-0.1, 0.0, 0.0);
    glScalef(0.03, 0.03, 0.03);
    glutSolidSphere(1.0, 8, 8);
    glPopMatrix();
    glScalef(0.1, 0.2, 0.75);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix(); //table

    //lamp
    glPushMatrix();
    glLightfv(GL_LIGHT2, GL_AMBIENT, matV(0.04, 0.02, 0.01, 1.0));
    glLightfv(GL_LIGHT2, GL_DIFFUSE, matV(0.18, 0.16, 0.16, 1.0));
    glLightfv(GL_LIGHT2, GL_POSITION, matV(0.0, 0.0, 0.0, 1.0));

    glTranslatef(1.4, 0.35, 0.0);
    //body
    GLfloat plane[] = { 1.0,0.0,0.0,0.0 };
    glPushMatrix();
    glRotatef(90, 1.0, 0.0, 0.0);
    glBindTexture(GL_TEXTURE_2D, tb1[Wall]);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, plane);
    glEnable(GL_TEXTURE_GEN_S);
    glutSolidCylinder(0.1,0.1,8,8);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    //head
    glPushMatrix();
    glTranslatef(0.0, 0.015, 0.0);
    glBindTexture(GL_TEXTURE_2D, tb1[Wall]);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, plane);
    glutSolidSphere(0.1, 8, 8);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_GEN_S);
    glPopMatrix();
    glPopMatrix();
    //laptop
    glPushMatrix();
    glTranslatef(1.4, 0.28, 0.6);
    //head
    glPushMatrix();
    glTranslatef(0.0, 0.55, 0.0);
    //center
    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_AMBIENT, matV(0.0, 0.05, 0.1, 1.0));
    glLightfv(GL_LIGHT1, GL_DIFFUSE, matV(0.01, 0.1, 0.4, 1.0));
    glLightfv(GL_LIGHT1, GL_POSITION, matV(-0.04, 0.0, 0.0, 1.0));

    glScalef(0.04, 0.3, 0.5);
    if(texswitch)
        cubeS(tb1[Glass]);
    else
        glutSolidCube(1.0);
    glPopMatrix();

    //back
    glPushMatrix();
    glTranslatef(0.02, 0.0, 0.0);
    glScalef(0.01, 0.4, 0.6);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side1
    glPushMatrix();
    glTranslatef(0.0, 0.18, 0.0);
    glScalef(0.04, 0.04, 0.6);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side2
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.28);
    glScalef(0.04, 0.32, 0.04);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side3
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.28);
    glScalef(0.04, 0.32, 0.04);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side4
    glPushMatrix();
    glTranslatef(0.0, -0.18, 0.0);
    glScalef(0.04, 0.04, 0.6);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
    //arm
    glPushMatrix();
    glTranslatef(0.02, 0.35, -0.3);
    glScalef(0.3, 0.3, 0.6);
    glutSolidCylinder(0.1,1,8,8);
    glPopMatrix();
    //body
    glPushMatrix();
    glTranslatef(-0.15, 0.2, 0.0);
    glRotatef(-45.0, 0.0, 0.0, 1.0);
    glScalef(0.05, 0.4, 0.6);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //standing bottom
    glPushMatrix();
    glTranslatef(-0.05, 0.0, 0.0);
    glScalef(0.4, 0.03, 0.4);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //standing top
    glPushMatrix();
    glTranslatef(-0.1, 0.15, 0.0);
    glRotatef(45.0, 0.0, 0.0, 1.0);
    glScalef(0.4, 0.03, 0.4);
    if (texswitch)
        cubeS(tb1[BlackGray]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    
    glPopMatrix(); // laptop

    //book shell 3
    glPushMatrix();
    glTranslatef(1.3, 0.4, 1.85);
    //back
    glPushMatrix();
    glScalef(0.9, 2.6, 0.1); 
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 1
    glPushMatrix();
    glTranslatef(0.4, 0.0, -0.1);
    glScalef(0.1, 2.6, 0.4);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 2
    glPushMatrix();
    glTranslatef(-0.5, 0.0, -0.1);
    glScalef(0.1, 2.6, 0.4);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack 1
    glPushMatrix();
    glTranslatef(-0.04, 1.26, 0.0);
    glScalef(1.0, 0.1, 0.62);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack 2
    glPushMatrix();
    glTranslatef(-0.05, 0.86, 0.0);
    glScalef(0.9, 0.1, 0.58);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack 3
    glPushMatrix();
    glTranslatef(-0.05, 0.46, 0.0);
    glScalef(0.9, 0.1, 0.58);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stack 4(drawer)
    glPushMatrix();
    glTranslatef(-0.05, 0.0, 0.0);
    //body
    glPushMatrix();
    glScalef(0.9, 0.26, 0.58);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //handler1
    glPushMatrix();
    glTranslatef(0.25, 0.0, -0.2);
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.1);
    glScalef(0.2, 0.2, 0.2);
    glutSolidSphere(0.1, 8, 8);
    glPopMatrix();
    glScalef(0.2, 0.2, 0.05);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //handler2
    glPushMatrix();
    glTranslatef(-0.01, 0.0, -0.2);
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.1);
    glScalef(0.2, 0.2, 0.2);
    glutSolidSphere(0.1, 8, 8);
    glPopMatrix();
    glScalef(0.2, 0.2, 0.05);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //handler3
    glPushMatrix();
    glTranslatef(-0.27, 0.0, -0.2);
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.1);
    glScalef(0.2, 0.2, 0.2);
    glutSolidSphere(0.1, 8, 8);
    glPopMatrix();
    glScalef(0.2, 0.2, 0.05);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();
    //stack 5
    glPushMatrix();
    glTranslatef(-0.05, -0.6, 0.0);
    glScalef(0.9, 0.1, 0.58);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //books
    index = -0.3;
    index2 = 1.06;
    for(int j=0;j<3;j++)
    for (int i = 0; index + i * 0.15 < 0.3; i++) {
        int num = rand();
        glPushMatrix();
        glTranslatef(index + i * 0.15, index2-j*0.4, -0.1);
        glScalef(0.1, 0.3, 0.3);
        if (texswitch)
            cubeS(tb1[books[i]]);
        else
            glutSolidCube(1.0);
        glPopMatrix();
    }

    glPopMatrix();// book shell 3

    //chair
    glPushMatrix();
    glTranslatef(0.8, 0.0, 0.5);
    //back
    glPushMatrix();
    glTranslatef(-0.2, -0.3, 0.0);
    glPushMatrix(); //1
    glTranslatef(0.0, 0.81, 0.0);
    glScalef(0.05, 0.1, 0.5);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); //2
    glTranslatef(0.0, 0.65, 0.0);
    glScalef(0.04, 0.05, 0.45);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix();
    //leg 1
    glPushMatrix();
    glTranslatef(-0.2, -0.24, -0.22);
    glScalef(0.05, 1.4, 0.05);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //leg 2
    glPushMatrix();
    glTranslatef(-0.2, -0.24, 0.22);
    glScalef(0.05, 1.4, 0.05);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //sitting
    glPushMatrix();
    glTranslatef(0.02, -0.2, 0.0);
    glScalef(0.5, 0.1, 0.45);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //leg 3
    glPushMatrix();
    glTranslatef(0.2, -0.65, -0.22);
    glScalef(0.05, 0.95, 0.05);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //leg 4
    glPushMatrix();
    glTranslatef(0.2, -0.65, 0.22);
    glScalef(0.05, 0.95, 0.05);
    if (texswitch)
        cubeS(tb1[Bookshell3]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix(); // chair

    //stairs 1
    glPushMatrix();
    glTranslatef(-1.6, 1.0, 1.3);
    glScalef(1.0, 4.0, 1.2);
    if (texswitch)
        cubeS(tb1[Wall]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stairs 2
    glPushMatrix();
    glTranslatef(-1.6, 1.8, 0.5);
    glRotatef(-45.0, 1.0, 0.0, 0.0);
    glScalef(1.0, 4.0, 2.0);
    if (texswitch)
        cubeS(tb1[Wall]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stairs 3
    glPushMatrix();
    glTranslatef(-1.5, 2.5, -1.4);
    if (texswitch)
        cubeS(tb1[Wall]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //stairs 4
    glPushMatrix();
    glTranslatef(-0.8, 2.9, -1.4);
    glRotatef(-45.0, 0.0, 0.0, 1.0);
    glScalef(1.0, 1.5, 1.0);
    if (texswitch)
        cubeS(tb1[Wall]);
    else
        glutSolidCube(1.0);
    glPopMatrix();

    //window 1
    glPushMatrix();
    glTranslatef(1.8, 1.8, -1.0);
    //side 1
    glPushMatrix();
    glTranslatef(0.0, 0.7, 0.0);
    glScalef(0.1, 0.1, 0.9);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 2
    glPushMatrix();
    glTranslatef(0.0, -0.7, 0.0);
    glScalef(0.1, 0.1, 0.9);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 3
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.4);
    glScalef(0.1, 1.35, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 4
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.4);
    glScalef(0.1, 1.35, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //glass
    glPushMatrix();
    glScalef(0.08, 1.3, 0.8);
    if (texswitch)
        cubeS(tb1[Glass]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix(); //window 1

    //window 2
    glPushMatrix();
    glTranslatef(1.9, 1.8, -0.2);
    //side 1
    glPushMatrix();
    glTranslatef(0.0, 0.7, 0.0);
    glScalef(0.1, 0.1, 0.9);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 2
    glPushMatrix();
    glTranslatef(0.0, -0.7, 0.0);
    glScalef(0.1, 0.1, 0.9);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 3
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.4);
    glScalef(0.1, 1.35, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 4
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.4);
    glScalef(0.1, 1.35, 0.1);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //glass
    glPushMatrix();
    glScalef(0.08, 1.3, 0.8);
    if (texswitch)
        cubeS(tb1[Glass]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    glPopMatrix(); //window 2

    //door
    glPushMatrix();
    glTranslatef(0.4, 1.0, 1.9);
    //side 1
    glPushMatrix();
    glTranslatef(0.0, 1.8, 0.05);
    glScalef(3.0, 0.2, 0.2);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 2
    glPushMatrix();
    glTranslatef(0.0, -1.8, 0.05);
    glScalef(3.0, 0.2, 0.2);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 3
    glPushMatrix();
    glTranslatef(1.4, 0.0, 0.05);
    glScalef(0.2, 3.5, 0.2);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 4
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.05);
    glScalef(0.2, 3.5, 0.2);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();
    //side 5
    glPushMatrix();
    glTranslatef(-1.4, 0.0, 0.05);
    glScalef(0.2, 3.5, 0.2);
    if (texswitch)
        cubeS(tb1[Bookshell1]);
    else
        glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();//door

    glPopMatrix();
    glutSwapBuffers();
    glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h)
{
    screenW = w;
    screenH = h;
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        gluPerspective(60.0, h / w, 0.5, 100);
    else
        gluPerspective(60.0, w / h, 0.5, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case '0':
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glutPostRedisplay();
        break;
    case '1':
        glEnable(GL_LIGHT0);
        glutPostRedisplay();
        break;
    case '2':
        glEnable(GL_LIGHT1);
        glutPostRedisplay();
        break;
    case '3':
        glEnable(GL_LIGHT2);
        glutPostRedisplay();
        break;
    case 't':
        texswitch = !texswitch;
        glutPostRedisplay();
        break;
    case 's':
        Simple_Smooth = !Simple_Smooth;
        unsigned char* data1;
        int width, height, channel;
        if (Simple_Smooth) {
            char resource[][20] = { "low_wall.jpg","low_floor.jpg","low_glass.jpg","low_pillow.jpg","low_wood3.jpg","low_blackgray.jpg","low_wood1.jpg","low_bed.jpg" };
            for (int i = 0; i < (sizeof(tb1) / sizeof(GLuint)); i++) {
                glBindTexture(GL_TEXTURE_2D, tb1[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                data1 = stbi_load(resource[i], &width, &height, &channel, 0);

                if (data1) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
                    std::cout << "이미지 " << i << " 로드 성공! 폭: " << width << " 높이: " << height << "\n";
                }
                else {
                    std::cout << "이미지 로드 실패\n";
                }
                stbi_image_free(data1);
            }
        }
        else {
            char resource[][20] = { "wall.jpg","floor.jpg","roughglass.jpg","pillow.jpg","bookshell3.jpg","blackgray.jpg","bookshell1.jpg","bed.jpg" };
            for (int i = 0; i < (sizeof(tb1) / sizeof(GLuint)); i++) {
                glBindTexture(GL_TEXTURE_2D, tb1[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                data1 = stbi_load(resource[i], &width, &height, &channel, 0);

                if (data1) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
                    std::cout << "이미지 " << i << " 로드 성공! 폭: " << width << " 높이: " << height << "\n";
                }
                else {
                    std::cout << "이미지 로드 실패\n";
                }
                stbi_image_free(data1);
            }
        }
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    }
    
}

void mouseStatus(int button, int state, int x, int y)
{
    // button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
    // state: GLUT_DOWN, GLUT_UP
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        startMovement = GL_TRUE;
        CamState = Rotate;
        mouseCurPositionX = x; // record mouse position
        mouseCurPositionY = y;
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        startMovement = GL_TRUE;
        CamState = Translate;
        mouseCurPositionX = x; // record mouse position
        mouseCurPositionY = y;
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        startMovement = GL_TRUE;
        CamState = Zoom;
        mouseCurPositionX = x; // record mouse position
        mouseCurPositionY = y;
    }
    else if ((button == GLUT_RIGHT_BUTTON|| button == GLUT_RIGHT_BUTTON|| button == GLUT_MIDDLE_BUTTON) && state == GLUT_UP) {
        startMovement = GL_FALSE;
    }

}

void mouseMotion(int x, int y)
{
    
    if (startMovement == GL_TRUE)
    {
        switch(CamState){
        case Rotate:
            if (mouseCurPositionX != x || mouseCurPositionY != y) {
                int dx = mouseCurPositionX - x;
                int dy = mouseCurPositionY - y;
                Xangle += dy / 3.6;
                Yangle += dx / 3.6;
            }
            break;
        case Translate:
            if (mouseCurPositionX != x || mouseCurPositionY != y) {
                int dx = mouseCurPositionX - x;
                int dy = mouseCurPositionY - y;
                CamPos[0] += dx / 60.0;
                
                CamPos[2] += dy / 60.0;
            }
            break;
        case Zoom:
            if (mouseCurPositionX != x) {
                int dx = mouseCurPositionX - x;
                fovy += dx / 10;
                if (fovy < 10)fovy = 10.0;
                else if (fovy > 100)fovy = 100.0;
                glViewport(0, 0, (GLsizei)screenW, (GLsizei)screenH);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(fovy, screenH / screenW, 0.5, 100);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
            }
            break;
        }

        mouseCurPositionX = x; // record mouse position
        mouseCurPositionY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenW, screenH);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("15010996 이진호");
    init();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouseStatus);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
