#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

#include "raylib.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const int MAX_FISH_SIZE = 50;
const int MIN_FISH_SIZE = 10;
const int MAX_FISH_SPEED = 10;
const int MIN_FISH_SPEED = 1;


Vector2 Vector2Add(Vector2 v1, Vector2 v2)
{
    Vector2 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

Vector2 Vector2Scale(Vector2 v, float scale)
{
    Vector2 result;
    result.x = v.x * scale;
    result.y = v.y * scale;
    return result;
}

Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
{
    Vector2 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

// Fish class
class Fish
{
public:
    Vector2 position;
    int size;
    int speed;
    Color color;
    std::vector<Vector2> wavingLines;

    // Constructor
    Fish(int x, int y, int size, int speed, Color color)
    {
        position = { (float)x, (float)y };
        this->size = size;
        this->speed = speed;
        this->color = color;

        // Initialize waving lines
        for (int i = 0; i < size; i++)
        {
            wavingLines.push_back({ (float)x, (float)y });
        }
    }

    // Move fish
    void move()
    {
        // Update position
        position.x += (float)(rand() % 3 - 1) * speed;
        position.y += (float)(rand() % 3 - 1) * speed;

        // Keep fish within screen bounds
        if (position.x < 0) position.x = 0;
        if (position.x > SCREEN_WIDTH) position.x = (float)SCREEN_WIDTH;
        if (position.y < 0) position.y = 0;
        if (position.y > SCREEN_HEIGHT) position.y = (float)SCREEN_HEIGHT;

        // Update waving lines
        for (int i = 0; i < wavingLines.size(); i++)
        {
            wavingLines[i].x += (float)(rand() % 3 - 1) * speed;
            wavingLines[i].y += (float)(rand() % 3 - 1) * speed;
        }
    }

    // Grow fish
    void grow(int size)
    {
        this->size += size;
    }
};

float distance(Vector2 p1, Vector2 p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}


// Simulation class
class Simulation
{
public:
    std::vector<Fish> fishList;
    int deadFishCount;

    // Constructor
    Simulation()
    {
        deadFishCount = 0;
    }

    // Update simulation by a single time step
    // Update simulation by a single time step
    void step()
    {
        // Move fish
        for (Fish& f : fishList)
        {
            f.move();
        }

        // Check for fish chasing and being chased
        for (int i = 0; i < fishList.size(); i++)
        {
            Fish& f1 = fishList[i];
            for (int j = i + 1; j < fishList.size(); j++)
            {
                Fish& f2 = fishList[j];
                if (f1.size > f2.size)
                {
                    // Larger fish chases smaller fish
                    if (distance(f1.position, f2.position) < f1.size + f2.size)
                    {
                        // Eat smaller fish
                        f1.grow(f2.size);
                        fishList.erase(fishList.begin() + j);
                        deadFishCount++;
                        break;
                    }
                }
                else if (f1.size < f2.size)
                {
                    // Smaller fish tries to escape larger fish
                    if (distance(f1.position, f2.position) < f1.size + f2.size)
                    {
                        f1.position = Vector2Add(f1.position, Vector2Scale(Vector2Subtract(f1.position, f2.position), f1.speed));
                    }
                }
            }
        }
    }
};

int main(int argc, char* argv[])
{
    // Initialize window and graphics
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fish Simulation");
    SetTargetFPS(60);
    srand(time(NULL));

    // Create simulation
    Simulation sim;

    // Create user-specified number of fish
    //std::cout << "Enter number of fish: ";
    int numFish = 20;
    
    for (int i = 0; i < numFish; i++)
    {
        int x = rand() % SCREEN_WIDTH;
        int y = rand() % SCREEN_HEIGHT;
        int size = MIN_FISH_SIZE + rand() % (MAX_FISH_SIZE - MIN_FISH_SIZE + 1);
        int speed = MIN_FISH_SPEED + rand() % (MAX_FISH_SPEED - MIN_FISH_SPEED + 1);
        Color color = { (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
        sim.fishList.push_back(Fish(x, y, size, speed, color));
    }

    // Run simulation loop
    while (!WindowShouldClose())
    {
        // Update
        sim.step();

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw fish
        for (Fish& f : sim.fishList)
        {
            DrawCircleV(f.position, f.size, f.color);
            for (Vector2& v : f.wavingLines)
            {
                DrawLineV(v, Vector2Add(v, Vector2Scale(f.position, 0.1f)), f.color);
            }
        }

        // Draw counters
        DrawText(std::string("Dead Fishies: " + std::to_string(sim.deadFishCount)).c_str(), 10, 10, 20, BLACK);
        DrawText(std::string("Fishies: " + std::to_string(sim.fishList.size())).c_str(), 10, 30, 20, BLACK);

        EndDrawing();
    }

    // Close window and terminate raylib
    CloseWindow();
    return 0;
}
