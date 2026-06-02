#include "catmull.h"
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

// Multiplicação de Matrizes M = A * B
void multMatrixVector(float* m, float* v, float* res) {
    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            
            res[j] += v[k] * m[k * 4 + j];
        }
    }
}

void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, float* pos, float* deriv) {
    // Matriz de Catmull-Rom
    float m[4][4] = {{-0.5f,  1.5f, -1.5f,  0.5f},
                     { 1.0f, -2.5f,  2.0f, -0.5f},
                     {-0.5f,  0.0f,  0.5f,  0.0f},
                     { 0.0f,  1.0f,  0.0f,  0.0f}};

    // Arrays para as coordenadas XYZ dos 4 pontos
    float Px[4] = {p0.x, p1.x, p2.x, p3.x};
    float Py[4] = {p0.y, p1.y, p2.y, p3.y};
    float Pz[4] = {p0.z, p1.z, p2.z, p3.z};

    // Arrays de Tempo e Derivada do Tempo: T = [t^3, t^2, t, 1], T' = [3t^2, 2t, 1, 0]
    float T[4]  = {t * t * t, t * t, t, 1};
    float Td[4] = {3 * t * t, 2 * t, 1, 0};

    // Resultados intermédios: A = T * M
    float A[4], Ad[4];
    multMatrixVector((float*)m, T, A);
    multMatrixVector((float*)m, Td, Ad);

    // Calcular Posição final (pos = A * P)
    pos[0] = A[0]*Px[0] + A[1]*Px[1] + A[2]*Px[2] + A[3]*Px[3];
    pos[1] = A[0]*Py[0] + A[1]*Py[1] + A[2]*Py[2] + A[3]*Py[3];
    pos[2] = A[0]*Pz[0] + A[1]*Pz[1] + A[2]*Pz[2] + A[3]*Pz[3];

    // Calcular Derivada final (deriv = Ad * P)
    deriv[0] = Ad[0]*Px[0] + Ad[1]*Px[1] + Ad[2]*Px[2] + Ad[3]*Px[3];
    deriv[1] = Ad[0]*Py[0] + Ad[1]*Py[1] + Ad[2]*Py[2] + Ad[3]*Py[3];
    deriv[2] = Ad[0]*Pz[0] + Ad[1]*Pz[1] + Ad[2]*Pz[2] + Ad[3]*Pz[3];
}

void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv, const std::vector<Point>& p) {
    int POINT_COUNT = p.size();
    float t = gt * POINT_COUNT; 
    int index = floor(t);       
    t = t - index;              

    // Os 4 pontos de controlo q
    int p0 = (index + POINT_COUNT - 1) % POINT_COUNT;
    int p1 = (index) % POINT_COUNT;
    int p2 = (index + 1) % POINT_COUNT;
    int p3 = (index + 2) % POINT_COUNT;

    getCatmullRomPoint(t, p[p0], p[p1], p[p2], p[p3], pos, deriv);
}

void renderCatmullRomCurve(const std::vector<Point>& p) {
    float pos[3];
    float deriv[3];
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glDisableClientState(GL_VERTEX_ARRAY); 
    
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINE_LOOP);
    for (float gt = 0.0f; gt < 1.0f; gt += 0.01f) {
        getGlobalCatmullRomPoint(gt, pos, deriv, p);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glEnableClientState(GL_VERTEX_ARRAY); 
}