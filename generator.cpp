#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include "utils.h"
#include <sstream>

void saveToFile(std::string path, const std::vector<Point>& vertices){
    std::ofstream f(path);
    if(f.is_open()){
        f << vertices.size() << "\n";
        for (const auto& v:vertices){
            f << v.x << " " << v.y << " " << v.z << "\n";
        }
        f.close();
    }
}

std::vector<Point> sphere (float raio, int slices, int stack){
    std::vector<Point> v;
    float alpha_step = 2 * M_PI / slices;
    float beta_step = M_PI / stack;

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stack; j++) {
            float a1 = i * alpha_step;
            float a2 = (i + 1) * alpha_step;
            float b1 = j * beta_step - M_PI/2;
            float b2 = (j + 1) * beta_step - M_PI/2;

            // Ponto 1: (r*cos(b)*sin(a), r*sin(b), r*cos(b)*cos(a))
            Point p1 = { raio * cos(b1) * sin(a1), raio * sin(b1), raio * cos(b1) * cos(a1) };
            Point p2 = { raio * cos(b1) * sin(a2), raio * sin(b1), raio * cos(b1) * cos(a2) };
            Point p3 = { raio * cos(b2) * sin(a1), raio * sin(b2), raio * cos(b2) * cos(a1) };
            Point p4 = { raio * cos(b2) * sin(a2), raio * sin(b2), raio * cos(b2) * cos(a2) };

            //v.push_back(p1); v.push_back(p2); v.push_back(p4);
            //v.push_back(p1); v.push_back(p4); v.push_back(p3);

            v.push_back(p1); v.push_back(p2); v.push_back(p3);
            v.push_back(p3); v.push_back(p2); v.push_back(p4);
        }
    }
    return v;
}

std::vector<Point> box (float tamanho, int grid){
    std::vector<Point> v;
    float delta = tamanho / (float)grid;
    float meio = tamanho / 2.0f;

    for (int i = 0; i < grid; i++) {
        for (int j = 0; j < grid; j++) {
            float a = -meio + i * delta;
            float b = -meio + j * delta;

            // Face Topo (y = meio)
            v.push_back({a, meio, b}); v.push_back({a, meio, b + delta}); v.push_back({a + delta, meio, b + delta});
            v.push_back({a, meio, b}); v.push_back({a + delta, meio, b + delta}); v.push_back({a + delta, meio, b});

            // Face Base (y = -meio)
            v.push_back({a, -meio, b}); v.push_back({a + delta, -meio, b + delta}); v.push_back({a, -meio, b + delta});
            v.push_back({a, -meio, b}); v.push_back({a + delta, -meio, b}); v.push_back({a + delta, -meio, b + delta});

            // Face Frente (z = meio)
            v.push_back({a, b, meio}); v.push_back({a + delta, b + delta, meio}); v.push_back({a, b + delta, meio});
            v.push_back({a, b, meio}); v.push_back({a + delta, b, meio}); v.push_back({a + delta, b + delta, meio});

            // Face Trás (z = -meio)
            v.push_back({a, b, -meio}); v.push_back({a, b + delta, -meio}); v.push_back({a + delta, b + delta, -meio});
            v.push_back({a, b, -meio}); v.push_back({a + delta, b + delta, -meio}); v.push_back({a + delta, b, -meio});

            // Face Direita (x = meio)
            v.push_back({meio, a, b}); v.push_back({meio, a + delta, b + delta}); v.push_back({meio, a, b + delta});
            v.push_back({meio, a, b}); v.push_back({meio, a + delta, b}); v.push_back({meio, a + delta, b + delta});

            // Face Esquerda (x = -meio)
            v.push_back({-meio, a, b}); v.push_back({-meio, a, b + delta}); v.push_back({-meio, a + delta, b + delta});
            v.push_back({-meio, a, b}); v.push_back({-meio, a + delta, b + delta}); v.push_back({-meio, a + delta, b});
        }
    }
    return v;
}

std::vector<Point> cone(float raio, float altura, int slices, int stack) {
    std::vector<Point> v;
    float angulo_passo = 2 * M_PI / slices;
    float h_passo = altura / stack;
    float r_passo = raio / stack;

    for (int i = 0; i < slices; i++) {
        float a1 = i * angulo_passo;
        float a2 = (i + 1) * angulo_passo;

        // 1. Base (Triângulos do centro para a borda no plano y=0)
        v.push_back({0, 0, 0});
        v.push_back({raio * sin(a2), 0, raio * cos(a2)});
        v.push_back({raio * sin(a1), 0, raio * cos(a1)});

        // 2. Lados (Corpos do cone divididos em stacks)
        for (int j = 0; j < stack; j++) {
            float y1 = j * h_passo;
            float y2 = (j + 1) * h_passo;
            float r1 = raio - (j * r_passo);
            float r2 = raio - ((j + 1) * r_passo);

            // Cada stack é um trapézio (2 triângulos), exceto o topo que é um triângulo
            Point p1 = {r1 * sin(a1), y1, r1 * cos(a1)};
            Point p2 = {r1 * sin(a2), y1, r1 * cos(a2)};
            Point p3 = {r2 * sin(a1), y2, r2 * cos(a1)};
            Point p4 = {r2 * sin(a2), y2, r2 * cos(a2)};

            v.push_back(p1); v.push_back(p2); v.push_back(p3);
            v.push_back(p3); v.push_back(p2); v.push_back(p4);
        }
    }
    return v;
}

std::vector<Point> plane(float length, int divisions){
    std::vector<Point> v;
    float inicio = -length/2.0f;
    float passo = length/(float)divisions;

    for (int i = 0; i < divisions; i++){
       for (int j = 0; j < divisions; j++) {
            float x1 = inicio + i * passo;
            float z1 = inicio + j * passo;
            float x2 = x1 + passo;
            float z2 = z1 + passo;

            v.push_back({x1, 0, z1});
            v.push_back({x1, 0, z2});
            v.push_back({x2, 0, z2});

            v.push_back({x1, 0, z1});
            v.push_back({x2, 0, z2});
            v.push_back({x2, 0, z1});
        }
    }
    return v;
}

// 1. Calcula os Polinómios de Bernstein (Os "pesos" que atraem a curva)
float bernstein(int i, float t) {
    if (i == 0) return pow(1.0f - t, 3.0f);
    if (i == 1) return 3.0f * t * pow(1.0f - t, 2.0f);
    if (i == 2) return 3.0f * pow(t, 2.0f) * (1.0f - t);
    if (i == 3) return pow(t, 3.0f);
    return 0.0f;
}

// 2. Calcula 1 Ponto exato na Superfície usando U e V (0 a 1)
Point getBezierPoint(float u, float v, const std::vector<Point>& patchPoints) {
    Point p = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float bu = bernstein(i, u);
            float bv = bernstein(j, v);
            Point cp = patchPoints[i * 4 + j];
            p.x += cp.x * bu * bv;
            p.y += cp.y * bu * bv;
            p.z += cp.z * bu * bv;
        }
    }
    return p;
}

// 3. Lê o .patch e gera os triângulos todos
std::vector<Point> generateBezierPatches(std::string patchFile, int tessellation) {
    std::vector<Point> finalVertices;
    std::ifstream file(patchFile);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro patch: " << patchFile << "\n";
        return finalVertices;
    }

    std::string line;
    
    std::getline(file, line);
    int numPatches = std::stoi(line);

    std::vector<std::vector<int>> patchIndices(numPatches);
    for (int i = 0; i < numPatches; i++) {
        std::getline(file, line);
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            patchIndices[i].push_back(std::stoi(token));
        }
    }

    std::getline(file, line);
    int numControlPoints = std::stoi(line);

    std::vector<Point> controlPoints(numControlPoints);
    for (int i = 0; i < numControlPoints; i++) {
        std::getline(file, line);
        std::stringstream ss(line);
        std::string tokenX, tokenY, tokenZ;
        std::getline(ss, tokenX, ',');
        std::getline(ss, tokenY, ',');
        std::getline(ss, tokenZ, ',');
        controlPoints[i] = {std::stof(tokenX), std::stof(tokenY), std::stof(tokenZ)};
    }
    file.close();

    float step = 1.0f / tessellation;
    for (int i = 0; i < numPatches; i++) {
        // Isolar os 16 pontos deste patch específico
        std::vector<Point> currentPatchPoints;
        for (int idx : patchIndices[i]) {
            currentPatchPoints.push_back(controlPoints[idx]);
        }

        // Criar a grelha
        for (int u = 0; u < tessellation; u++) {
            for (int v = 0; v < tessellation; v++) {
                float pu = u * step;
                float pv = v * step;
                float pu2 = (u + 1) * step;
                float pv2 = (v + 1) * step;

                // Calcular os 4 cantos do quadrado na superfície curva
                Point p0 = getBezierPoint(pu, pv, currentPatchPoints);
                Point p1 = getBezierPoint(pu, pv2, currentPatchPoints);
                Point p2 = getBezierPoint(pu2, pv, currentPatchPoints);
                Point p3 = getBezierPoint(pu2, pv2, currentPatchPoints);

                // Fechar o quadrado com 2 Triângulos (6 pontos)
                finalVertices.push_back(p0); finalVertices.push_back(p1); finalVertices.push_back(p2);
                finalVertices.push_back(p1); finalVertices.push_back(p3); finalVertices.push_back(p2);
            }
        }
    }
    return finalVertices;
}

int main(int argc, char** argv){
    std::string opt = argv[1];
    std::vector<Point> vertices;

    if (opt == "sphere"){
        float raio = std::stof(argv[2]);
        int slices = std::stoi(argv[3]);
        int stack = std::stoi(argv[4]);
        vertices = sphere(raio, slices, stack);
        saveToFile("3d/" + std::string(argv[5]), vertices);
    }
    else if(opt == "box"){
        float tamanho = std::stof(argv[2]);
        int grid = std::stoi(argv[3]);
        vertices = box(tamanho,grid);
        saveToFile("3d/" + std::string(argv[4]), vertices);
    }    
    else if(opt == "cone"){
        float raio = std::stof(argv[2]);
        float altura = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stack = std::stoi(argv[5]);
        vertices = cone(raio, altura, slices, stack);
        saveToFile("3d/" + std::string(argv[6]), vertices);
    }
    else if(opt == "plane"){
        float length = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        vertices = plane(length, divisions);
        saveToFile("3d/" + std::string(argv[4]), vertices);
    }
    else if(opt == "patch"){
        std::string patchFile = argv[2];
        int tessellation = std::stoi(argv[3]);
        vertices = generateBezierPatches(patchFile, tessellation);
        saveToFile("3d/" + std::string(argv[4]), vertices);
    }
    else{
        std::cout << "Inválido" << std::endl;
    }
    return 0;
}