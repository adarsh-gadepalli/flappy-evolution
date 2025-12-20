#include <iostream>

struct Bird {
    int x, y;
    int vx, vy;
};

struct Pipe {
    int x;
    int gap;
};

int main() {

    Bird bird;
    bird.x = 0;
    bird.y = 50;
    bird.vx = 1;
    bird.vy = -1;
    Pipe pipe;
    

    int score = 0;

    for (int t = 0; t < 1000; t++ ) {

        // update bird position
        bird.y -= bird.vy;
        bird.x += bird.vx;

        // check bird bounds
        if (bird.y < 0 || bird.y > 100) {
            std::cout << "Bird out of bounds at tick " << t << "\n";
            break;
        };
            
    }

    return 0;
    
}