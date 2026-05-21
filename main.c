#include <stdio.h>
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    int width;
    int height;
} ScreenSet;

int getNeighbours(ScreenSet ss, bool (*grid)[ss.height], Vector2 pos) {
    int res = 0;
    for (int y = pos.y-1; y <= pos.y+1; y++) {
        for (int x = pos.x-1; x <= pos.x+1; x++) {
            if (x == pos.x && y == pos.y) continue;
            if (0 > x || x > ss.width-1 ||
                0 > y || y > ss.height-1) continue;
            if (grid[x][y]) res += 1;
        }
    }
    return res;
}

int main(int argc, char *argv[]) {
    SetTraceLogLevel(LOG_ERROR); // LOG_NONE
    srand(time(NULL));

    // 16:10  {2560, 1600} {1920, 1200} {1440, 900} {1280, 800}
    // 16:9   {2560, 1440} {1920, 1080}             {1280, 720}
    ScreenSet ss = {1920, 1080};
    InitWindow(ss.width, ss.height, "Conway's Game of Life");

    bool (*grid)[ss.height] = calloc(ss.width, sizeof(*grid));
    if (grid == NULL) {
        CloseWindow();
        return 1;
    }

    float density = (argc > 1) ? strtof(argv[1], NULL) : 0.08f;
    if (density > 1.0f) density = 1.0f;
    if (density < 0.001f) density = 0.001f;

    for (int i = 0; i < (int)(ss.width*ss.height*density); i++) { // randomly set start population after density
        int x, y;
        do {
            x = rand() % ss.width;
            y = rand() % ss.height;
            } while (grid[x][y]);
        grid[x][y] = true;
    }

    int generation = 0;
    char textField[64];

    while (!WindowShouldClose()) {
        if (generation > 0 || IsKeyDown(KEY_SPACE)) {
            // COMPUTE NEXT GENERATION
            bool (*nextGeneration)[ss.height] = calloc(ss.width, sizeof(*nextGeneration));
            for (int y = 0; y < ss.height; y++) {
                for (int x = 0; x < ss.width; x++) {
                    bool current = grid[x][y];
                    int neighbours = getNeighbours(ss, grid, (Vector2){x,y});
                    if (neighbours < 2) nextGeneration[x][y] = false;                                         // dead - underpopulation
                    else if (current && (neighbours == 2 || neighbours == 3)) nextGeneration[x][y] = current; // lives on
                    else if (current && neighbours > 3) nextGeneration[x][y] = false;                         // dead - overpopulation
                    else if (!current && neighbours == 3) nextGeneration[x][y] = true;                        // live - reproduction
                }
            }

            generation += 1;
            size_t total_bytes = ss.width * sizeof(*grid);
            memcpy(grid, nextGeneration, total_bytes); // OVERWRITE grid
            free(nextGeneration);
        }

        BeginDrawing();
            ClearBackground(BLACK);
            snprintf(textField, sizeof(textField), "Gen%d", generation);
            DrawText(textField, 0, 0, 20, LIGHTGRAY);

            for (int y = 0; y < ss.height; y++)
                for (int x = 0; x < ss.width; x++)
                    if (grid[x][y])
                        DrawPixelV((Vector2){x,y}, RAYWHITE);
        EndDrawing();
    }

    free(grid);
    CloseWindow();

    return 0;
}
