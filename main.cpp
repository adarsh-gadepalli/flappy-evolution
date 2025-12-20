#include <iostream>
#include <random>

struct Bird {
    int x, y;
    int vx, vy;
};

struct Pipe {
    int x;
    int gap;
};

int main() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(30, 70);

    Bird bird;
    bird.x = 0;
    bird.y = 50;
    bird.vx = 1;
    bird.vy = -1;
    

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
        
        // generate new pipe every 50 ticks
        if (t % 50 == 0) {
            Pipe pipe;
            pipe.x = bird.x + 100;
            pipe.gap = dis(gen);
        }


    }

    std::cout << "Final score: " << score << "\n";
    return 0;
    
}