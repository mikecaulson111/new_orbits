/*

Code Author: Michael Caulson

NOTE:  MUCH OF THE BOILERPLATE CODE WAS WRITTEN BY MY PROFESSOR:
Willem A. (Vlakkies) Schreuder

this includes all other .c files in this repository

All code pertaining to the orbits was original and written by myself


*/




#include "CSCIx229.h"
#include <time.h>

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

int inside = 1; // This is the boolean of being inside the house


//for textures
int mode1 = 0;

double asp = 1;
int fov = 55;

// Many lighting necessities with help from Professor:
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float Ylight = 3;
int sunset = 0; // activates sunset mode

int light = 1;

int w = 500; // width
int h = 500; // height

// Controls viewing mode:
int mode = 1; // 0=perspective, 1=Fist Person

double Ex = 0;   // Eye position in the x axis
double Ez = 9;   // Eye position in the z axis
double dt = 0.1; // Time step

// To control speed:
double tvals [10] = {0.001, 0.01, 0.05, 0.1, 0.5, 1, 1.5, 2.0, 2.5, 5.0};
int tnum = 3; // start at dt = 0.1

int axes = 0;

float eyeposy = 0;
float accy = -0.001;
float vely = 0;

const float Emission[]  = {0.0,0.0,0.0,1.0};
const float Ambient[]   = {0.5,0.5,0.5,1.0};
// const float Ambient[] = {0.03,0.03,0.03,1.0}; // USE THIS IN THE END
const float Diffuse[]   = {1.0,1.0,1.0,1.0};
const float Specular[]  = {0.1,0.1,0.1,1.0};
const float Direction[] = {0.0,-1.0,0.0,1.0};
const float Shinyness[] = {16};

float posz = 15.0;


// View single item at a time
int single = 4;

// for fun
int size = 1;

int party = 1;

unsigned int texture[21];

double Cy; // just for now


int shader[] = {0,0,0,0,0};
// changes the shader mode:
int shademode = 2;

// Controls if lights are swinging
int swing = 0;

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

    // // float yellow[] = {1,1,0,1};
    // float Emission[] = {0,0,0,1};
    // // float Emission[] = {0,0,1,0};
    const int d = 15;

    // glMaterialf(GL_FRONT,GL_SHININESS,shiny);
    // // glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    // if(spec == 0) {
    //     glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    // }

    glPushMatrix();

    glTranslated(x,y,z);
    glScaled(r,r,r);

    // glColor3f(1,1,1);
    // SetColor(1,1,1);
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

double squarer(double number)
{
    return number * number;
}

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

double minner(double r1, double r2)
{
    if ( r1 < r2 )
    {
        return r1;
    }
    return r2;
}

// R in general will have coordinates = m1*r1 + m2*r2 where r1 and r2 are vectors
typedef struct tag_Rvec {
    double X;
    double Y;
    double Z;
    double M;
} Rvec;

int orbit = 0;
double G = 0.00001;

// update for spheres;
void update_sphere(spheres *s1, spheres *s2)
{
    // do update here

    Rvec *R = NULL;
    R = malloc(sizeof(Rvec));

    // f = ma:
    // a = f/m, m= 1
    // a = ( G * m1 * m2 / r^2) / m1
    double x1 = s2->x - s1->x;
    double y1 = s2->y - s1->y;
    double z1 = s2->z - s1->z;
    // double min = minner(s1->r, s2->r);
    // // double min = 0.001;
    // double rx = ( squarer(x1));
    // double ry = ( squarer(y1));
    // double rz = ( squarer(z1));
    // r = sqrt(r);



    double r = sqrt(x1*x1 + y1*y1 + z1*z1);
    // R->M = s1->m + s2->m;
    // R->X = (s2->m*s2->x - s1->m*s1->x) / R->M;
    // R->Y = (s2->m*s2->y - s1->m*s1->y) / R->M;
    // R->Z = (s2->m*s2->z - s1->m*s1->z) / R->M;





    if(orbit == 1)
    {
        // if ( rx > 0.05)
        // {
        //     s1->ax = (x1 < 0 ? -1 : 1) * (0.000005 * s2->m / (rx < min ? min : rx)); // G * m2 / r^2
        // }
        // if ( ry > 0.05)
        // {
        //     s1->ay = (y1 < 0 ? -1 : 1) * (0.000005 * s2->m / (ry < min ? min : ry)); // G * m2 / r^2
        // }
        // if ( rz > 0.05)
        // {
        //     s1->az = (z1 < 0 ? -1 : 1) * (0.000005 * s2->m / (rz < min ? min : rz)); // G * m2 / r^2
        // }

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
    // sphere(0,0,5,0.3,0);
    // // sphere(0,0,5.0,0.3,0);
    // sphere(2, 0, 0, 0.5, 0);

    // spheres here

    for ( int i = 0; i < 2; i++ )
    {
        sphere(all_spheres[i].x, all_spheres[i].y, all_spheres[i].z, all_spheres[i].r, 0);
    }

    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 0; j < 2; j++) // can optimize
        {
            if( i != j )
            {
                update_sphere(&all_spheres[i], &all_spheres[j]);
            }
        }
    }

    glBegin(GL_LINES);
    glVertex3d(all_spheres[0].x, all_spheres[0].y, all_spheres[0].z);
    glVertex3d(all_spheres[1].x, all_spheres[1].y, all_spheres[1].z);
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
    // glBegin(GL_QUADS);
    // glVertex2d(-1,-1);
    // glVertex2d(1,-1);
    // glVertex2d(1,1);
    // glVertex2d(-1,1);
    // glEnd();
    sphere(0,0,0,0.5,1);

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
    // else if (key == GLUT_KEY_UP) {
    //     if(ph < 90) {
    //         ph += 5;
    //     }
    // } else if (key == GLUT_KEY_DOWN) {
    //     if(ph > -85) {
    //         ph -= 5;
    //     }
    // } 

    ph %= 360;
    th %= 360;

    // // if(mode == 0) {
    // //     Project(0,asp,dim);
    // // } else {
    // //     Project(fov,asp,dim);
    // // }
    // Project(fov,asp,dim);
    Project(0,asp,dim); // want 2d view so I think this is correct

    glutPostRedisplay();
}

void keyDown(unsigned char ch, int x, int y) {
    switch (ch) {
        case 27:
            exit(0);
            break;
        case ' ':
            vely = 0.055;
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
            printf("%lf\n",posz);
            break;
            // dim += 0.1;
            // this does nothing lol
        case 'p':
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
    }
}

void keyUp(unsigned char ch, int x, int y) {
    // printf("%u",ch);
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

// Reshape function
void reshape(int width,int height)
{
   //  Set the viewport to the entire window
    asp = (height>0) ? (double)width/height : 1;
    glViewport(0,0, RES*width,RES*height);
    // Calls the Project to deal with different projections:
    // if(mode == 0) {
    //     Project(0,asp,dim);
    // } else {
    //     Project(fov,asp,dim);
    // }
    Project(0,asp,dim);

    w = width;
    h = height;
}

// Display the scene
void display() {
    // glClearColor(0.3,0.5,1.0,1);
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
            glWindowPos2i(5,20);
            Print("Cy=%f",Cy);
        }
    }

    ErrCheck("Display");
    glFlush();
    glutSwapBuffers();
}


void idle()
{
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

    glutIdleFunc(idle);

    //  Switch font to nearest
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   //  Create Shader Programs
//    shader[1] = CreateShaderProg("toon.vert","toon.frag");
//    shader[2] = CreateShaderProg("pixlight.vert","pixlight.frag");
//    shader[3] = CreateShaderProg("pixtex.vert","pixtex.frag");
// //    shader[4] = CreateShaderProg("pixlightspot.vert","pixlightspot.frag");
//     shader[4] = CreateShaderProg("pixlightspotup.vert","pixlightspotup.frag");

#ifndef USEGLEW
    shader[1] = CreateShaderProg("pixtex.vert","pixtex.frag");
#endif
    // shader[2] = CreateShaderProg("pixlightspotup.vert","pixlightspotup.frag");
    // shader[3] = CreateShaderProg("pixlightspot.vert","pixlightspot.frag");


    // Loads Textures
    // texture[0] = LoadTexBMP("brick.bmp");
    // texture[0] = LoadTexBMP("brick.bmp");
    // texture[1] = LoadTexBMP("roof1.bmp");
    // texture[2] = LoadTexBMP("door.bmp");
    // texture[3] = LoadTexBMP("newBlanket.bmp");
    // texture[4] = LoadTexBMP("newOak.bmp");
    // texture[5] = LoadTexBMP("newercobble.bmp");
    // texture[6] = LoadTexBMP("stone.bmp");
    // texture[7] = LoadTexBMP("newbark.bmp");
    // texture[8] = LoadTexBMP("Steel.bmp");
    // texture[9] = LoadTexBMP("shutter.bmp");
    // texture[10] = LoadTexBMP("webb.bmp");
    // texture[11] = LoadTexBMP("ceiling.bmp");
    // texture[12] = LoadTexBMP("doodle.bmp");
    // texture[13] = LoadTexBMP("spookyhouse.bmp");
    // texture[14] = LoadTexBMP("spookyboy.bmp");
    // texture[15] = LoadTexBMP("ghost1.bmp");
    // texture[16] = LoadTexBMP("ghost2.bmp");
    // texture[17] = LoadTexBMP("ghost3.bmp");
    // texture[18] = LoadTexBMP("woman.bmp");
    // texture[19] = LoadTexBMP("exitsign.bmp");
    // texture[20] = LoadTexBMP("cub.bmp");

    for ( int i = 0; i < 10; i++ )
    {
        intiSphere(&all_spheres[i]);
    }

    all_spheres[0].x = 5;
    all_spheres[0].y = 0;
    all_spheres[0].z = 0;
    all_spheres[0].r = 0.2;
    all_spheres[0].spec = 0;
    all_spheres[0].vx = 0;
    // all_spheres[0].vy = 100;
    all_spheres[0].vy = 0.05;
    all_spheres[0].vz = 0.0;
    all_spheres[0].ax = 0;
    all_spheres[0].ay = 0;
    all_spheres[0].az = 0;
    all_spheres[0].m = 20;

    all_spheres[1].x = 0;
    all_spheres[1].y = 0;
    all_spheres[1].z = 1;
    all_spheres[1].r = 0.5;
    all_spheres[1].spec = 0;
    all_spheres[1].vx = 0;
    all_spheres[1].vy = 0;
    all_spheres[1].vz = 0;
    all_spheres[1].ax = 0;
    all_spheres[1].ay = 0;
    all_spheres[1].az = 0;
    all_spheres[1].m = 4000;


    Player[0] = 0.0;
    Player[1] = 0.25;

    orbit = 1;


    ErrCheck("Main");

    glutMainLoop();

    return 0;
}