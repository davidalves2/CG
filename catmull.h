#ifndef CATMULL_H
#define CATMULL_H

#include <vector>
#include "utils.h"

// Calcula um ponto específico na curva Catmull-Rom e a sua derivada 
void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, float* pos, float* deriv);

// Dado o tempo global 't' e a lista de pontos, descobre os 4 pontos corretos e chama a função acima
void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv, const std::vector<Point>& p);

// Desenha a linha da curva usando GL_LINE_LOOP
void renderCatmullRomCurve(const std::vector<Point>& p);

#endif