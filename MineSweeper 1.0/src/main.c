#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CELL_SIZE 50
#define MINES 26


int countAdjacentMines(int y, int x, int mineLocations[][WINDOW_WIDTH / CELL_SIZE], int cellsY, int cellsX) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int ny = y + i;
            int nx = x + j;
            if (ny >= 0 && ny < cellsY && nx >= 0 && nx < cellsX) {
                count += mineLocations[ny][nx];
            }
        }
    }
    return count;
}


void floodFill(int cellStates[][WINDOW_WIDTH / CELL_SIZE], int mineLocations[][WINDOW_WIDTH / CELL_SIZE], int y, int x, int cellsY, int cellsX) {
    if (y < 0 || y >= cellsY || x < 0 || x >= cellsX || cellStates[y][x] != 1) {
        return;
    }

    int adjacentMines = countAdjacentMines(y, x, mineLocations, cellsY, cellsX);
    cellStates[y][x] = 0;

    if (adjacentMines == 0) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dy != 0 || dx != 0) {
                    floodFill(cellStates, mineLocations, y + dy, x + dx, cellsY, cellsX);
                }
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL2 : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Mine sweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        fprintf(stderr, "Failed to create a window : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Failed to create a renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    while (1) {
      if (getuid() == 0) {
	printf("\033[1;31m WARNING: It's unrecommended to run the program as root !\033[0m\n");
	break;
      } else {
	break;
      }
    }
      
    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2400) < 0) {
        fprintf(stderr, "Failed to initialize SDL mixer : %s\n", SDL_GetError());
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 1;
    }

    Mix_Music* reveal = Mix_LoadMUS("break.mp3");

    SDL_Surface* cellSurface = SDL_LoadBMP("textures/cube.bmp");
    SDL_Surface* flagSurface = SDL_LoadBMP("textures/flag.bmp");
    SDL_Surface* mineSurface = SDL_LoadBMP("textures/mine.bmp");
    SDL_Surface* oneSurface = SDL_LoadBMP("textures/one.bmp");
    SDL_Surface* twoSurface = SDL_LoadBMP("textures/two.bmp");
    SDL_Surface* threeSurface = SDL_LoadBMP("textures/three.bmp");
    SDL_Surface* fourSurface = SDL_LoadBMP("textures/four.bmp");
    SDL_Surface* fiveSurface = SDL_LoadBMP("textures/five.bmp");

    SDL_Texture* cell = SDL_CreateTextureFromSurface(renderer, cellSurface);
    SDL_FreeSurface(cellSurface);

    SDL_Texture* flag = SDL_CreateTextureFromSurface(renderer, flagSurface);
    SDL_FreeSurface(flagSurface);

    SDL_Texture* mine = SDL_CreateTextureFromSurface(renderer, mineSurface);
    SDL_FreeSurface(mineSurface);

    SDL_Texture* one = SDL_CreateTextureFromSurface(renderer, oneSurface);
    SDL_FreeSurface(oneSurface);

    SDL_Texture* two = SDL_CreateTextureFromSurface(renderer, twoSurface);
    SDL_FreeSurface(twoSurface);

    SDL_Texture* three = SDL_CreateTextureFromSurface(renderer, threeSurface);
    SDL_FreeSurface(threeSurface);

    SDL_Texture* four = SDL_CreateTextureFromSurface(renderer, fourSurface);
    SDL_FreeSurface(fourSurface);

    SDL_Texture* five = SDL_CreateTextureFromSurface(renderer, fiveSurface);
    SDL_FreeSurface(fiveSurface);

    if (!cell || !flag || !mine || !one || !two || !three || !four || !five) {
        fprintf(stderr, "Failed to load textures : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        Mix_FreeMusic(reveal);
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

	SDL_Surface* cursorSurface = SDL_LoadBMP("textures/cursor.bmp");

	SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
	SDL_FreeSurface(cursorSurface);

	SDL_SetCursor(cursor);

    int cellsX = WINDOW_WIDTH / CELL_SIZE;
    int cellsY = WINDOW_HEIGHT / CELL_SIZE;

    int cellStates[cellsY][cellsX];
    int mineLocations[cellsY][cellsX];
    int flaggedCells[cellsY][cellsX];
    memset(mineLocations, 0, sizeof(mineLocations));
    memset(flaggedCells, 0, sizeof(flaggedCells));


    for (int y = 0; y < cellsY; y++) {
        for (int x = 0; x < cellsX; x++) {
            cellStates[y][x] = 1;
        }
    }

    SDL_Rect rect;
    rect.w = CELL_SIZE;
    rect.h = CELL_SIZE;


    srand(time(NULL));
    int minesPlaced = 0;


    while (minesPlaced < MINES) {
        int x = rand() % cellsX;
        int y = rand() % cellsY;
        if (mineLocations[y][x] == 0) {
            mineLocations[y][x] = 1;
            minesPlaced++;
        }
    }

    int running = 1;
     while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                int cellX = mouseX / CELL_SIZE;
                int cellY = mouseY / CELL_SIZE;

                if (cellX >= 0 && cellX < cellsX && cellY >= 0 && cellY < cellsY) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (cellStates[cellY][cellX] != 2) {
                            if (mineLocations[cellY][cellX] == 1) {
                                rect.x = cellX * CELL_SIZE;
                                rect.y = cellY * CELL_SIZE;
                                SDL_RenderCopy(renderer, mine, NULL, &rect);
                                SDL_RenderPresent(renderer);

				SDL_Delay(3000);
				running = 0;
                            } else {
                                Mix_PlayMusic(reveal, 0);
                                int adjacentMines = countAdjacentMines(cellY, cellX, mineLocations, cellsY, cellsX);
                                if (adjacentMines == 0) {
                                    floodFill(cellStates, mineLocations, cellY, cellX, cellsY, cellsX);
                                } else {
                                    cellStates[cellY][cellX] = 0;

                                }
                            }
                        }
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        if (cellStates[cellY][cellX] == 1) {
                            cellStates[cellY][cellX] = 2;
                            flaggedCells[cellY][cellX] += 1;

                        } else if (cellStates[cellY][cellX] == 2) {
                            cellStates[cellY][cellX] = 1;
                            flaggedCells[cellY][cellX] -= 1;

                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 130, 130, 130, 130);
        SDL_RenderClear(renderer);

        for (int y = 0; y < cellsY; y++) {
            for (int x = 0; x < cellsX; x++) {
                rect.x = x * CELL_SIZE;
                rect.y = y * CELL_SIZE;

                if (cellStates[y][x] == 1) {
                    SDL_RenderCopy(renderer, cell, NULL, &rect);
                    if (flaggedCells[y][x] > 0) {
                        SDL_RenderCopy(renderer, flag, NULL, &rect);
                    }
				} else if (cellStates[y][x] == 2) {
					SDL_RenderCopy(renderer, flag, NULL, &rect);
                } else if (cellStates[y][x] == 0) {
                    int adjacentMines = countAdjacentMines(y, x, mineLocations, cellsY, cellsX);
                    SDL_Texture* numberTexture = NULL;
                    switch (adjacentMines) {
                        case 1: numberTexture = one; break;
                        case 2: numberTexture = two; break;
                        case 3: numberTexture = three; break;
                        case 4: numberTexture = four; break;
                        case 5: numberTexture = five; break;
                        default: break;
                    }
                    if (numberTexture) {
                        SDL_RenderCopy(renderer, numberTexture, NULL, &rect);
                    }
                }
            }
        }
        SDL_RenderPresent(renderer);
     }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(cell);
    SDL_DestroyTexture(mine);
    SDL_DestroyTexture(flag);
    SDL_DestroyTexture(one);
    SDL_DestroyTexture(two);
    SDL_DestroyTexture(three);
    SDL_DestroyTexture(four);
    SDL_DestroyTexture(five);
    Mix_FreeMusic(reveal);
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}
