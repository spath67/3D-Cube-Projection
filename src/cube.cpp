#include <iostream>
#include <cmath>
#include <SDL2/SDL.h> 

#define WIDTH  512
#define HEIGHT 512

using namespace std;
 
typedef struct {
    double x, y, z;
} point3D;

class Renderer {
    public:
        SDL_Window* window;
        SDL_Renderer* renderer;

        Renderer(string name, unsigned int width, unsigned int height);
        void clearScreen();
        void drawLine(SDL_Point p1, SDL_Point p2);
        void updateScreen();
        void quit();
};

Renderer::Renderer(string name, unsigned int width, unsigned int height) {
    SDL_Init(SDL_INIT_EVERYTHING);
            
    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (window == NULL) {
        cout << "Problem creating window: " << SDL_GetError() << endl;
    }
    if (renderer == NULL) {
        cout << "Problem creating renderer: " << SDL_GetError() << endl;
    }
}

void Renderer::clearScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

void Renderer::drawLine(SDL_Point p1, SDL_Point p2) {
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
}

void Renderer::updateScreen() {
    SDL_RenderPresent(renderer);
}

void Renderer::quit() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

class Cube3D {
    public:
        point3D* points;
        point3D newpoints[8];
        SDL_Point* p2d;
        unsigned int scalefactorx, scalefactory;

        Cube3D(point3D ps[8], unsigned int sfx, unsigned int sfy);
        SDL_Point* convertTo2D(unsigned int scalefactorx, unsigned int scalefactory);
        void renderCube(Renderer renderer);
        void rotateXZ();
        unsigned int degturnt;
};

Cube3D::Cube3D(point3D ps[8], unsigned int sfx, unsigned int sfy) {
    points = ps;
    for (unsigned int i = 0; i < 8; i++) newpoints[i] = points[i];
    degturnt = 0;
}

SDL_Point* Cube3D::convertTo2D(unsigned int scalefactorx, unsigned int scalefactory) {
    p2d = new SDL_Point[8];
    for (unsigned int i = 0; i < 8; i++) {
        p2d[i] = {
            (int) round((1 + (newpoints[i].x / newpoints[i].z))/2 * scalefactorx), 
            (int) round((1 + (newpoints[i].y / newpoints[i].z))/2 * scalefactory)
        };
    }
    return p2d;
}

void Cube3D::renderCube(Renderer renderer) {
    SDL_SetRenderDrawColor(renderer.renderer, 255, 255, 255, 255);
    for (unsigned int i = 0; i < 4; i++) {
        renderer.drawLine(p2d[i], p2d[(i+1) % 4]);
        renderer.drawLine(p2d[i+4], p2d[(i+1) % 4 + 4]);
        renderer.drawLine(p2d[i], p2d[i+4]);
    }
}

void Cube3D::rotateXZ() {
    double orgin[2] = {(points[0].x + points[6].x)/2, (points[0].z + points[6].z)/2};
    degturnt++;
    for (unsigned int i = 0; i < 8; i++) {
        newpoints[i].x = ((points[i].x-orgin[0])*cos(degturnt*M_PI/180)) - ((points[i].z-orgin[1])*sin(degturnt*M_PI/180))+orgin[0];
        newpoints[i].z = ((points[i].x-orgin[0])*sin(degturnt*M_PI/180)) + ((points[i].z-orgin[1])*cos(degturnt*M_PI/180))+orgin[1];
        if (degturnt == 360) degturnt = 0;
    }
}

#define SCALEFACTORX WIDTH
#define SCALEFACTORY HEIGHT

int main() {
    Renderer renderer("3D Cube", WIDTH, HEIGHT);

    // Points have to be sorted such that points that are adjacent to each other have to be next
    // Also separate the front and back face
    point3D points[8] = {
         {1, -1, 3}, { 1,  1, 3}, {-1,  1, 3}, {-1, -1, 3}, 
         {1, -1, 5}, { 1,  1, 5}, {-1,  1, 5}, {-1, -1, 5}
    }; 

    Cube3D cube(points, SCALEFACTORX, SCALEFACTORY);

    renderer.clearScreen();

    while (1) {
        cube.convertTo2D(SCALEFACTORX, SCALEFACTORY);
        cube.renderCube(renderer);
        cube.rotateXZ();
        SDL_Delay(10);
        renderer.updateScreen();
        renderer.clearScreen();
    }
    
    renderer.quit();
    return 0;
}
