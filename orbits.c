/*

Code Author: Michael Caulson

NOTE:  MUCH OF THE BOILERPLATE CODE WAS WRITTEN BY MY PROFESSOR:
Willem A. (Vlakkies) Schreuder

this includes all other .c files in this repository

All code pertaining to the orbits was original and written by myself


*/




#include "CSCIx229.h"
#include <time.h>
#include <stdbool.h>

#define ATan(y,x) (atan2(y,x)*180/3.14159265)




// l -> turn on line mode

double dim = 5;

// For angles overall
int th = 0; //20
int ph = 0; //60
int th1 = 0;
int ph1 = 0;

int ph2 = 0;
int th2 = 0;

double asp = 1;
int fov = 55;

int light = 1;

int w = 500; // width
int h = 500; // height

// Controls viewing mode:
int mode = 1; // 0=perspective, 1=Fist Person

int axes = 0;

const float Emission[]  = {0.0,0.0,0.0,1.0};
const float Ambient[]   = {0.5,0.5,0.5,1.0};
const float Diffuse[]   = {1.0,1.0,1.0,1.0};
const float Specular[]  = {0.1,0.1,0.1,1.0};
const float Direction[] = {0.0,-1.0,0.0,1.0};
const float Shinyness[] = {16};

float posz = 15.0;

// unsused at this time:
// unsigned int texture[21];

//Number of bodies to show:
int num_bodies = 2;


int shader[] = {0,0,0,0,0};
// changes the shader mode:
int shademode = 2;

int lines = 0;

#ifdef GL_VERSION_4_0
#define MODE 11
int shader4[] = {0,0,0}; //  OpenGL 4 shaders
#else
#define MODE 10
#endif

//  Transformation matrixes
float ProjectionMatrix[16];
float ViewMatrix[16];
//  Set lighting parameters using uniforms
float Position[4];

int onoff = 1; // Controls if light is on
int counteronoff = 0; // counts frames between turning on and off

float l1x = 0.0;
float l1y = 20.0;
float l1z = 0.0;

double leftRight = 0.0; // to be changed in key down and key up;


int onner = 1;

float Player[2];


static void SetColor(float x, float y, float z) {
    float color[] = {x,y,z,1.0};
    float dark[] = {x/20.,y/20.,z/20.,1.0};
    // float dark[] = {x*0.1,y*0.1,z*0.1,1.0};
    glColor3f(x,y,z);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,dark);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,color);

}

float Abs(float x) {
    if(x < 0) {
        return -x;
    } else {
        return x;
    }
}


// Using sphere2 from example 8 from class
static void sphere(double x, double y, double z, double r,int spec) {
    const int d = 15;

    glPushMatrix();

    glTranslated(x,y,z);
    glScaled(r,r,r);

    for(int ph = -90; ph < 90; ph += d) {
        glBegin(GL_QUAD_STRIP);
        for(int th = 0; th <= 360; th += d) {
            float p1[] = {Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph)};
            float p2[] = {Sin(th)*Cos(ph+d) , Sin(ph+d) , Cos(th)*Cos(ph+d)};

            // With normals:
            // glNormal3f(p1[0],p1[1],p1[2]);
            glNormal3f(0,0,1);
            glTexCoord2d(p1[0],p1[1]); glVertex3f(p1[0],p1[1],p1[2]);

            // glNormal3f(p2[0],p2[1],p2[2]);
            glTexCoord2d(p2[0],p2[1]); glVertex3f(p2[0],p2[1],p2[2]);
        }
        glEnd();
    }

    // glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    glPopMatrix();
    // glDisable(GL_TEXTURE_2D);
}


void drawPlayer() {
    SetColor(1,0,0);
    glBegin(GL_QUADS);
    glVertex2f(Player[0]-0.25,Player[1]-0.25);
    glVertex2f(Player[0]+0.25,Player[1]-0.25);
    glVertex2f(Player[0]+0.25,Player[1]+0.25);
    glVertex2f(Player[0]-0.25,Player[1]+0.25);
    glEnd();
}

typedef struct tag_spheres { // mjc should probably put this at the top
    double x, y, z, r, spec, vx, vy, vz, ax, ay, az, m;
} spheres;
// should probably make a create and desytroy function too

void update_velocity(spheres *sphere)
{
    // update veloctiy based on acceleration, then update position
    sphere->vx += sphere->ax;
    sphere->vy += sphere->ay;
    sphere->vz += sphere->az;

    sphere->x += sphere->vx;
    sphere->y += sphere->vy;
    sphere->z += sphere->vz;
}

int orbit = 0;
double G = 0.00001;

// update for spheres;
void update_sphere(spheres *s1, spheres *s2)
{
    // do update here

    double x1 = s2->x - s1->x;
    double y1 = s2->y - s1->y;
    double z1 = s2->z - s1->z;

    double r = sqrt(x1*x1 + y1*y1 + z1*z1);

    if(orbit == 1)
    {
        s1->ax = G * s2->m * (x1 / (r * r * r));
        s1->ay = G * s2->m * (y1 / (r * r * r));
        s1->az = G * s2->m * (z1 / (r * r * r));
    }

    update_velocity(s1);
}


spheres all_spheres[10];


/*
 * Draw scene using the OpenGL2 style
 */
void Scene2()
{
    if(lines) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
   glLoadIdentity();

    glRotated(th,0,1,0);

    glColor3f(1,1,1);

    // spheres here

    for ( int i = 0; i < num_bodies; i++ )
    {
        sphere(all_spheres[i].x, all_spheres[i].y, all_spheres[i].z, all_spheres[i].r, 0);
    }

    for ( int i = 0; i < num_bodies; i++ )
    {
        for ( int j = i+1; j < num_bodies; j++)
        {
            update_sphere(&all_spheres[i], &all_spheres[j]);
            update_sphere(&all_spheres[j], &all_spheres[i]);
        }
    }

    glBegin(GL_LINES);
    for ( int i = 0; i < num_bodies - 1; i++)
    {
        glVertex3d(all_spheres[i].x, all_spheres[i].y, all_spheres[i].z);
        glVertex3d(all_spheres[i+1].x, all_spheres[i+1].y, all_spheres[i+1].z);
    }
    glEnd();

   // Second light for fun
   //  OpenGL should normalize normal vectors
    float pos[3] = {0.0,0.0,15.0};
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
    // glLightfv(GL_LIGHT0,GL_POSITION,Position);
    glLightfv(GL_LIGHT0,GL_POSITION, pos);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,Direction);
    // }
    ErrCheck("Lighting");

   //  Set materials
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

   //
   //  Draw scene
   //


    SetColor(1,1,0);
    // glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,red);
    glNormal3f(0,0,1);

    // sphere(0,0,0,0.5,1);

    ErrCheck("Scene");

   //  Revert to fixed pipeline
   glUseProgram(0);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
//    glColor3f(1,1,1);
    SetColor(1,1,1);
   if (axes)
   {
      const double len=2.0;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
}


// Controls the rotations of the scene
void special(int key, int x, int y) {
    if(key == GLUT_KEY_RIGHT) {
        th += 15;
        th2+=15;
    } else if (key == GLUT_KEY_LEFT) {
        th -= 15;
        th2-=15;
    } 

    ph %= 360;
    th %= 360;

    Project(0,asp,dim); // want 2d view so I think this is correct

    glutPostRedisplay();
}

int opposite(int num)
{
    int opposite = 0;
    if ( num == 0)
    {
        opposite = 1;
    }

    return opposite;
}

void keyDown(unsigned char ch, int x, int y) {
    switch (ch) {
        case 27:
            exit(0);
            break;
        case 'D':
        case 'd':
            leftRight += 0.055;
            break;
        case 'a':
        case 'A':
            leftRight -= 0.055;
            break;
        case 's':
        case 'S':
            posz -= 0.1;
            break;
        case ' ':
            if (orbit)
            {
                orbit = 0;
                for ( int i = 0; i < 2; i++)
                {
                    all_spheres[i].ax = 0;
                    all_spheres[i].ay = 0;
                    all_spheres[i].az = 0;
                }
            }
            else
            {
                orbit = 1;
            }
            break;
        case 'u':
            dim += 1;
            Project(0,asp,dim);
            break;
        case 'y':
            dim -= 1;
            Project(0,asp,dim);
            break;
        case 'o':
            axes = opposite(axes);
            break;
        case 'l':
            lines = opposite(lines);
            break;
    }
}

void keyUp(unsigned char ch, int x, int y) {
    switch (ch) {
        case 'D':
        case 'd':
            leftRight -= 0.055;
            break;
        case 'A':
        case 'a':
            leftRight += 0.055;
            break;
    }
}

void mouse(int button, int state, int x, int y)
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        double tx = ((double)x / (double)w) - 0.5;
        double ty = ((double)y / (double)h) - 0.5;
        all_spheres[num_bodies].x = (tx) * dim * 2;
        all_spheres[num_bodies].y = (ty) * -dim * 2;
        all_spheres[num_bodies].z = 0;
        all_spheres[num_bodies].vx = 0;
        all_spheres[num_bodies].vy = 0;
        all_spheres[num_bodies].vz = 0;
        all_spheres[num_bodies].r = 0.3;
        all_spheres[num_bodies].m = 50;
        all_spheres[num_bodies].ax = 0;
        all_spheres[num_bodies].ay = 0;
        all_spheres[num_bodies].az = 0;

        printf("x:%d, y:%d\n", x, y);
        printf("xx: %f, yy: %f\n", all_spheres[num_bodies].x, all_spheres[num_bodies].y);

        num_bodies += 1;
    }
}

// Reshape function
void reshape(int width,int height)
{
   //  Set the viewport to the entire window
    asp = (height>0) ? (double)width/height : 1;
    glViewport(0,0, RES*width,RES*height);
    Project(0,asp,dim);

    w = width;
    h = height;
}

// Display the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Scene2();

    if(axes) {
        if(mode == 0) {
            glWindowPos2i(5,5);
            Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s, k=%d",th,ph,dim,fov,"Perpective",shademode);
        } else {
            glWindowPos2i(5,5);
            Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s, k=%d",th,ph,dim,fov,"First Person",shademode);
        }
    }

    ErrCheck("Display");
    glFlush();
    glutSwapBuffers();
}


void idle()
{
    // printf("mass0: x: %f, y: %f\n", all_spheres[0].x, all_spheres[0].y);
   glutPostRedisplay();
}

/*
 *  Read text file
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

void intiSphere(spheres *sphere)
{
    sphere = malloc(sizeof(spheres));
}




int main(int argc, char* argv[]) {

    // Initialize the whole scene with my name in the window
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(500,500);

    glutCreateWindow("Orbits - Michael Caulson");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif


    srand(time(0));
    glutSpecialFunc(special);

    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);// only added to make holding down work

    glutKeyboardFunc(keyDown);

    glutKeyboardUpFunc(keyUp);

    glutDisplayFunc(display);

    glutReshapeFunc(reshape);

    glutMouseFunc(mouse);

    glutIdleFunc(idle);

    //  Switch font to nearest
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   //  Create Shader Programs

#ifndef USEGLEW
    shader[1] = CreateShaderProg("pixtex.vert","pixtex.frag");
#endif

    for ( int i = 0; i < 10; i++ )
    {
        intiSphere(&all_spheres[i]);
    }

    // all_spheres[0].x = 5;
    // all_spheres[0].y = 0;
    // all_spheres[0].z = 0;
    // all_spheres[0].r = 0.2;
    // all_spheres[0].spec = 0;
    // all_spheres[0].vx = 0;
    // // all_spheres[0].vy = 100;
    // all_spheres[0].vy = 0.05;
    // all_spheres[0].vz = 0.0;
    // all_spheres[0].ax = 0;
    // all_spheres[0].ay = 0;
    // all_spheres[0].az = 0;
    // all_spheres[0].m = 20;

    // all_spheres[1].x = 0;
    // all_spheres[1].y = 0;
    // all_spheres[1].z = 1;
    // all_spheres[1].r = 0.5;
    // all_spheres[1].spec = 0;
    // all_spheres[1].vx = 0;
    // all_spheres[1].vy = 0;
    // all_spheres[1].vz = 0;
    // all_spheres[1].ax = 0;
    // all_spheres[1].ay = 0;
    // all_spheres[1].az = 0;
    // all_spheres[1].m = 4000;


    // good for colliding together
    // all_spheres[0].x = 2;
    // all_spheres[0].y = 0;
    // all_spheres[0].z = 0;
    // all_spheres[0].r = 0.5;
    // all_spheres[0].spec = 0;
    // all_spheres[0].vx = 0;
    // all_spheres[0].vy = 0.01;
    // all_spheres[0].vz = 0.0;
    // all_spheres[0].ax = 0;
    // all_spheres[0].ay = 0;
    // all_spheres[0].az = 0;
    // all_spheres[0].m = 200;

    // all_spheres[1].x = -2;
    // all_spheres[1].y = 0;
    // all_spheres[1].z = 0;
    // all_spheres[1].r = 0.5;
    // all_spheres[1].spec = 0;
    // all_spheres[1].vx = 0;
    // all_spheres[1].vy = -0.01;
    // all_spheres[1].vz = 0;
    // all_spheres[1].ax = 0;
    // all_spheres[1].ay = 0;
    // all_spheres[1].az = 0;
    // all_spheres[1].m = 200;


    // good for nearly perfect orbit, just remove the third set:
    all_spheres[0].x = 2;
    all_spheres[0].y = 0;
    all_spheres[0].z = 0;
    all_spheres[0].r = 0.5;
    all_spheres[0].spec = 0;
    all_spheres[0].vx = 0;
    all_spheres[0].vy = 0.015;
    all_spheres[0].vz = 0.0;
    all_spheres[0].ax = 0;
    all_spheres[0].ay = 0;
    all_spheres[0].az = 0;
    all_spheres[0].m = 200;

    all_spheres[1].x = -2;
    all_spheres[1].y = 0;
    all_spheres[1].z = 0;
    all_spheres[1].r = 0.5;
    all_spheres[1].spec = 0;
    all_spheres[1].vx = 0;
    all_spheres[1].vy = -0.015;
    all_spheres[1].vz = 0;
    all_spheres[1].ax = 0;
    all_spheres[1].ay = 0;
    all_spheres[1].az = 0;
    all_spheres[1].m = 200;

    // all_spheres[2].x = 5;
    // all_spheres[2].y = 0;
    // all_spheres[2].z = 0;
    // all_spheres[2].r = 0.2;
    // all_spheres[2].spec = 0;
    // all_spheres[2].vx = 0;
    // all_spheres[2].vy = 0.03;
    // all_spheres[2].vz = 0;
    // all_spheres[2].ax = 0;
    // all_spheres[2].ay = 0;
    // all_spheres[2].az = 0;
    // all_spheres[2].m = 40;


    Player[0] = 0.0;
    Player[1] = 0.25;

    orbit = 1;


    ErrCheck("Main");

    glutMainLoop();

    return 0;
}