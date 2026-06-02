#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "TinyXML/tinyxml2.h" 
#include "utils.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h> 
#include <GL/glut.h>
#endif

#include "catmull.h" 

float initAlpha;
float initBeta;
float initRadius;

void buildRotMatrix(float *x, float *y, float *z, float *m) {
    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void crossProduct(float *a, float *b, float *res) {
    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

void normalize(float *a) {
    float l = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
    if (l > 0) { a[0] = a[0]/l; a[1] = a[1]/l; a[2] = a[2]/l; }
}

// Códigos de cores
#define RED 1.0f,0.0f,0.0f
#define GREEN 0.0f,1.0f,0.0f
#define BLUE 0.0f,0.0f,1.0f
#define YELLOW 1.0f, 1.0f, 0.0f
#define CYAN 0.0f, 1.0f, 1.0f
#define WHITE 1.0f, 1.0f, 1.0f

struct Model {
    GLuint vboID;      
    int vertexCount;   
};

enum TransformType { TRANSLATE, ROTATE, SCALE };

struct Transformacao {
    TransformType type;
    float angle, x, y, z;
 
    float time = 0.0f;          
    bool align = false;         
    std::vector<Point> points;   
};

struct Grupo {
    std::vector<Transformacao> transformacoes;

    std::vector<Model> models;
    std::vector<Grupo*> children;
};

// Definições para não dar erros a iniciar o programa
float camX;
float camY;
float camZ;
float lookatx;
float lookaty;
float lookatz;
float upx;
float upy;
float upz;
float fov = 60.0f;
float near = 1.0f;
float far = 1000.0f;
int width;
int heigth;
float radius = 5.0f;
float alpha = M_PI/2;
float beta = M_PI/2;
float vel = 0.1f;
int tracking = 0;
int startX, startY;
Grupo* root = nullptr;



void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(fov ,ratio, near, far);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


// Meter funçao para desenhar figuras

Model loadModel(std::string filename) {
    std::ifstream file("3d/" + filename);
    Model m;
    if (!file.is_open()) {
        std::cerr << "Falha ao carregar ficheiro: " << filename << "\n";
        return m;
    }

    int numVertices;
    file >> numVertices; 
    m.vertexCount = numVertices; 

    std::vector<float> vertexData; 
    Point p;
    while (file >> p.x >> p.y >> p.z) {
        vertexData.push_back(p.x);
        vertexData.push_back(p.y);
        vertexData.push_back(p.z);
    }
    file.close();

    glGenBuffers(1, &m.vboID);
    
    glBindBuffer(GL_ARRAY_BUFFER, m.vboID);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);

    return m;
}

void parseGrupo(tinyxml2::XMLElement* grupo, Grupo* atual) {

    tinyxml2::XMLElement* transform = grupo->FirstChildElement("transform");

    if (transform) {
        for (tinyxml2::XMLElement* t = transform->FirstChildElement(); t; t = t->NextSiblingElement()) {
            std::string name = t->Value();
			Transformacao tr;
            if (name == "translate") {
                tr.type = TRANSLATE;
                // Verificamos se é uma translação animada (tem atributo 'time')
                if (t->Attribute("time")) {
                    tr.time = t->FloatAttribute("time");
                    tr.align = t->BoolAttribute("align", false);
                    
                    // Ler todos os sub-pontos da curva
                    for (tinyxml2::XMLElement* pt = t->FirstChildElement("point"); pt; pt = pt->NextSiblingElement("point")) {
                        Point p = { pt->FloatAttribute("x"), pt->FloatAttribute("y"), pt->FloatAttribute("z") };
                        tr.points.push_back(p);
                    }
                } else {
                    // Translação Estática normal 
                    tr.x = t->FloatAttribute("x");
                    tr.y = t->FloatAttribute("y");
                    tr.z = t->FloatAttribute("z");
                }
                atual->transformacoes.push_back(tr);
            }
            else if (name == "rotate") {
                tr.type = ROTATE;
                // Verificamos se é uma rotação animada
                if (t->Attribute("time")) {
                    tr.time = t->FloatAttribute("time");
                } else {
                    tr.angle = t->FloatAttribute("angle");
                }
                tr.x = t->FloatAttribute("x");
                tr.y = t->FloatAttribute("y");
                tr.z = t->FloatAttribute("z");
                atual->transformacoes.push_back(tr);
            }
            else if (name == "scale") {
                tr.type = SCALE;
                tr.angle = 0.0f;
                tr.x = t->FloatAttribute("x");
                tr.y = t->FloatAttribute("y");
                tr.z = t->FloatAttribute("z");
                atual->transformacoes.push_back(tr);
            }
        }
    }

    tinyxml2::XMLElement* models = grupo->FirstChildElement("models");
    if (models) {
        for (tinyxml2::XMLElement* mod = models->FirstChildElement("model"); mod; mod = mod->NextSiblingElement("model")) {
            const char* file = mod->Attribute("file");
            if (file) atual->models.push_back(loadModel(file));
        }
    }

    for (tinyxml2::XMLElement* child = grupo->FirstChildElement("group"); child; child = child->NextSiblingElement("group")) {
        Grupo* filho = new Grupo();
        atual->children.push_back(filho);
        parseGrupo(child, filho);
    }
}

void loadXML(const char* path){
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path) != tinyxml2::XML_SUCCESS) return;

	tinyxml2::XMLElement* world = doc.FirstChildElement("world");
    if (!world) return;

	tinyxml2::XMLElement* win = world->FirstChildElement("window");
    if (win) {
        width = win->IntAttribute("width");
        heigth = win->IntAttribute("height");
    }

	tinyxml2::XMLElement* camera = world->FirstChildElement("camera");
    if (camera) {
        tinyxml2::XMLElement* pos = camera->FirstChildElement("position");
        if (pos) {
            camX = pos->FloatAttribute("x");
            camY = pos->FloatAttribute("y");
            camZ = pos->FloatAttribute("z");
        }
        tinyxml2::XMLElement* look = camera->FirstChildElement("lookAt");
		if (look) {
			lookatx = look->FloatAttribute("x");
			lookaty = look->FloatAttribute("y");
			lookatz = look->FloatAttribute("z");
		}
		tinyxml2::XMLElement* up = camera->FirstChildElement("up");
		if (up) {
			upx = up->FloatAttribute("x");
			upy = up->FloatAttribute("y");
			upz = up->FloatAttribute("z");
		}
		tinyxml2::XMLElement* p = camera->FirstChildElement("projection");
		if (p){
			fov = p->FloatAttribute("fov");
			near = p->FloatAttribute("near");
			far = p->FloatAttribute("far");
		}
    }

	tinyxml2::XMLElement* group = world->FirstChildElement("group");
	if (group){
		root = new Grupo();
		parseGrupo(group, root);
	}

    radius = sqrt(camX * camX + camY * camY + camZ * camZ);
    beta = asin(camY / radius);
    alpha = atan2(camX, camZ);
    
    initAlpha = alpha;
    initBeta = beta;
    initRadius = radius;
}

void renderGrupo(Grupo* g) {
    glPushMatrix(); 

    for (const auto& t : g->transformacoes) {
        switch (t.type) {
            case TRANSLATE:
                if (t.time > 0.0f && t.points.size() >= 4) { // Animação
                    // 1. Desenhar a linha do percurso para vermos o caminho
                    renderCatmullRomCurve(t.points);

                    // 2. Calcular o tempo e a posição do objeto
                    float elapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; 
                    float gt = fmod(elapsedTime / t.time, 1.0f);
                    
                    float pos[3], deriv[3];
                    getGlobalCatmullRomPoint(gt, pos, deriv, t.points);
                    
                    // 3. Mover para a posição correta na curva
                    glTranslatef(pos[0], pos[1], pos[2]);

                    // 4. Se align="True", rodar o objeto para a direção do movimento
                    if (t.align) {
                        float x_axis[3] = {deriv[0], deriv[1], deriv[2]};
                        normalize(x_axis); 
                        
                        float up_axis[3] = {0.0f, 1.0f, 0.0f}; 
                        float z_axis[3];
                        
                        crossProduct(x_axis, up_axis, z_axis);
                        normalize(z_axis); 
                        
                        float y_axis[3];
                        crossProduct(z_axis, x_axis, y_axis);
                        normalize(y_axis); 

                        float m[16];
                        buildRotMatrix(x_axis, y_axis, z_axis, m);
                        glMultMatrixf(m); 
                    }
                } else { // Estático
                    glTranslatef(t.x, t.y, t.z);
                }
                break;
                
            case ROTATE:
                if (t.time > 0.0f) { // Animação de Rotação
                    float elapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
                    float dynamicAngle = (elapsedTime / t.time) * 360.0f; // Quanto tempo para 360º?
                    glRotatef(dynamicAngle, t.x, t.y, t.z);
                } else { // Estático
                    glRotatef(t.angle, t.x, t.y, t.z);
                }
                break;
                
            case SCALE:
                glScalef(t.x, t.y, t.z);
                break;
        }
    }

    for (const auto& model : g->models) {

    glBindBuffer(GL_ARRAY_BUFFER, model.vboID);
    
    glVertexPointer(3, GL_FLOAT, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
	}


    for (auto child : g->children) {
        renderGrupo(child);
    }

    glPopMatrix(); 
}


void renderScene(void) {

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // set the camera
    glLoadIdentity();
    gluLookAt(camX,camY,camZ, 
              lookatx,lookaty,lookatz,
              upx,upy,upz);

    // put drawing instructions here
    // axis lines
    glBegin(GL_LINES);
        // X axis in red
        glColor3f(RED);
        glVertex3f(-250.0f, 0.0f, 0.0f);
        glVertex3f( 250.0f, 0.0f, 0.0f);
        // Y Axis in Green
        glColor3f(GREEN);
        glVertex3f(0.0f,-250.0f, 0.0f);
        glVertex3f(0.0f, 250.0f, 0.0f);
        // Z Axis in Blue
        glColor3f(BLUE);
        glVertex3f(0.0f, 0.0f,-250.0f);
        glVertex3f(0.0f, 0.0f, 250.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnableClientState(GL_VERTEX_ARRAY); 
    
    if (root) renderGrupo(root);          
    
    glDisableClientState(GL_VERTEX_ARRAY); 
    
    glutSwapBuffers();

    glutPostRedisplay();
}

void processKeys(unsigned char c, int xx, int yy) {

	switch(c){
        case('w') : {beta+=vel;} break;
        case('s') : {beta-=vel;} break;
        case('a') : {alpha-=vel;} break;
        case('d') : {alpha+=vel;} break;
        
        case('r') : {
            alpha = initAlpha;
            beta = initBeta;
            radius = initRadius;
        } break;
    }

	if(beta>M_PI/2+vel) {beta = M_PI/2+vel;};
	if(beta<-M_PI/2+vel) {beta = -M_PI/2+vel;};

	camX = radius*sin(alpha)*cos(beta);
	camY = radius*sin(beta);
	camZ = radius*cos(alpha)*cos(beta);
	glutPostRedisplay();
}

void processMouseButtons(int button, int state, int xx, int yy){
	if(state == GLUT_DOWN){
		if(button == GLUT_LEFT_BUTTON) {
			startX = xx; 
			startY = yy;
			tracking = 1;
		};
	}
	else if (state == GLUT_UP) {
		if(tracking == 1) {
			alpha += (xx-startX);
			beta += (yy-startY);
			tracking = 0;
		}
	}
}

void processMouseMotion(int xx, int yy){
	if(!tracking) return;

	float deltaX = xx-startX;
	float deltaY = yy-startY;
	alpha += deltaX * 0.005f;
    beta  -= deltaY * 0.005f;

	if(beta>M_PI/2+vel) {beta = M_PI/2+vel;};
	if(beta<-M_PI/2+vel) {beta = -M_PI/2+vel;};

	camX = radius*sin(alpha)*cos(beta);
	camY = radius*sin(beta);
	camZ = radius*cos(alpha)*cos(beta);

	startX = xx;
	startY = yy;
	glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    
    glutCreateWindow("CG FASE 3 - VBOs");

#ifndef __APPLE__
    glewInit();
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    loadXML(argv[1]);

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    glutMainLoop();
    
    return 1;
}
