#include <stdio.h>
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 450

int getNeighbours(bool grid[WIDTH][HEIGHT], Vector2 pos) {
    int res = 0;
    for (int y = pos.y-1; y <= pos.y+1; y++) {
        for (int x = pos.x-1; x <= pos.x+1; x++) {
            if (x == pos.x && y == pos.y) continue;
            if (0 > x || x > WIDTH-1 ||
                0 > y || y > HEIGHT-1) continue;
            if (grid[x][y]) res += 1;
        }
    }
    return res;
}

int main(int argc, char *argv[]) {
    SetTraceLogLevel(LOG_ERROR); // LOG_NONE
    srand(time(NULL));

    float density = (argc > 1) ? strtof(argv[1], NULL) : .08;

    bool grid[WIDTH][HEIGHT] = { false };
    for (int i = 0; i < (WIDTH*HEIGHT*density); i++) { // randomly set start population after density
        int x, y;
        do {
            x = rand() % WIDTH;
            y = rand() % HEIGHT;
        } while (grid[x][y]);
        grid[x][y] = true;
    }
    int generation = 0;
    char textField[64];

    InitWindow(WIDTH, HEIGHT, "Conway's Game of Life");

    while (!WindowShouldClose()) {
        if (generation > 0 || IsKeyDown(KEY_SPACE)) {
            // COMPUTE NEXT GENERATION
            bool nextGeneration[WIDTH][HEIGHT];
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    bool current = grid[x][y];
                    int neighbours = getNeighbours(grid, (Vector2){x,y});
                    if (neighbours < 2) nextGeneration[x][y] = false;                                         // dead - underpopulation
                    else if (current && (neighbours == 2 || neighbours == 3)) nextGeneration[x][y] = current; // lives on
                    else if (current && neighbours > 3) nextGeneration[x][y] = false;                         // dead - overpopulation
                    else if (!current && neighbours == 3) nextGeneration[x][y] = true;                        // live - reproduction
                }
            }

            generation += 1;
            memcpy(grid, nextGeneration, sizeof(grid)); // OVERWRITE grid
        }

        BeginDrawing();
            ClearBackground(BLACK);
            snprintf(textField, sizeof(textField), "Gen%d", generation);
            DrawText(textField, 0, 0, 20, LIGHTGRAY);

            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    if (grid[x][y]) {
                        DrawPixelV((Vector2){x,y}, RAYWHITE);
                    }
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
