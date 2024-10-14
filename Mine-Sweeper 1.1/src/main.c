#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <GL/glew.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CELL_SIZE 50
#define MINES 26

const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}
)";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Error compiling shader: %s\n", infoLog);
    }

    return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "Error linking program: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


GLuint loadTexture(const char* filename) {
	SDL_Surface* surface = SDL_LoadBMP(filename);

	if (!surface) {
		fprintf(stderr, "Unable to load texture: %s\n", filename);
		return 1;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);

	SDL_FreeSurface(surface);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

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

void renderGrid(GLuint shaderProgram, GLuint vao, GLuint texture, int cellsX, int cellsY) {
    glUseProgram(shaderProgram);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    
    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    
    for (int y = 0; y < cellsY; y++) {
        for (int x = 0; x < cellsX; x++) {
            float posX = x * CELL_SIZE;
            float posY = y * CELL_SIZE;

            float model[16] = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                posX, posY, 0.0f, 1.0f
            };

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
    
    glBindVertexArray(0);
    glUseProgram(0);
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

void renderTexture(GLuint shaderProgram, GLuint vao, GLuint texture, SDL_Rect* rect) {
	glUseProgram(shaderProgram);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

	float model[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		(float)rect->x, (float)rect->y, 0.0f, 1.0f
	};

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model);
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glBindVertexArray(0);
	glUseProgram(0);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL2 : %s\n", SDL_GetError());
        return 1;
    }
	
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("Mine sweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		                           WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
	    fprintf(stderr, "SDL2: %s\n", SDL_GetError());
	    SDL_Quit();
	    return 1;
    }
 
    
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
	    fprintf(stderr, "OpenGL: %s\n", glewGetErrorString(glewError));
	    SDL_GL_DeleteContext(glContext);
	    SDL_DestroyWindow(window);
	    SDL_Quit();
	    return 1;
    }

    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2400) < 0) {
            fprintf(stderr, "SDL_mixer: %s\n", SDL_GetError());
	    SDL_GL_DeleteContext(glContext);
	    SDL_DestroyWindow(window);
	    SDL_Quit();
	    return 1;
    }

    Mix_Chunk* soundEffect[2];

    soundEffect[0] = Mix_LoadWAV("sfx/break.flac");
    soundEffect[1] = Mix_LoadWAV("sfx/boom.flac");
    
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported: %s\n", version);

    SDL_Surface* cursorSurface = SDL_LoadBMP("textures/cursor.bmp");
    SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
    SDL_FreeSurface(cursorSurface);
    SDL_SetCursor(cursor);

    GLuint cell = loadTexture("textures/cube.bmp");
    GLuint flag = loadTexture("textures/flag.bmp");
    GLuint mine = loadTexture("textures/mine.bmp");
    GLuint one = loadTexture("textures/one.bmp");
    GLuint two = loadTexture("textures/two.bmp");
    GLuint three = loadTexture("textures/three.bmp");
    GLuint four = loadTexture("textures/four.bmp");
    GLuint five = loadTexture("textures/five.bmp");

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

    
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    float vertices[] = {
        0.0f, 0.0f,       0.0f, 0.0f,
        CELL_SIZE, 0.0f,  1.0f, 0.0f,
        0.0f, CELL_SIZE,  0.0f, 1.0f,
        CELL_SIZE, CELL_SIZE, 1.0f, 1.0f,
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float projection[16] = {
        2.0f / WINDOW_WIDTH, 0.0f,               0.0f, 0.0f,
        0.0f,               -2.0f / WINDOW_HEIGHT, 0.0f, 0.0f,
        0.0f,               0.0f,               1.0f, 0.0f,
        -1.0f,              1.0f,               0.0f, 1.0f
    };

    GLint projLocation = glGetUniformLocation(shaderProgram, "projection");

    int running = 1;
    SDL_Event event;
	
    SDL_Rect rect;
    rect.w = CELL_SIZE;
    rect.h = CELL_SIZE;

    while (running) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    renderGrid(shaderProgram, vao, cell, cellsX, cellsY);
	    
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
					         Mix_PlayChannel(-1, soundEffect[0], 0);
						if (mineLocations[cellY][cellX] == 1) {
							rect.x = cellX * CELL_SIZE;
							rect.y = cellY * CELL_SIZE;
							renderTexture(shaderProgram, vao, mine, &rect);
							SDL_GL_SwapWindow(window);
							Mix_PlayChannel(-1, soundEffect[1], 0);

							SDL_Delay(3000);
							running = 0;

						} else {
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
	    glClearColor(0.51f, 0.51f, 0.51f, 0.51f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
	    for (int y = 0; y < cellsY; y++) {
		    for (int x = 0; x < cellsX; x++) {
			    rect.x = x * CELL_SIZE;
			    rect.y = y * CELL_SIZE;
		     		
			    if (cellStates[y][x] == 1) {
				    renderTexture(shaderProgram, vao, cell, &rect);
				    if (flaggedCells[y][x] > 0) {
					   renderTexture(shaderProgram, vao, flag, &rect);
				    }
			    } else if (cellStates[y][x] == 2) {
				   renderTexture(shaderProgram, vao, flag, &rect);
			    } else if (cellStates[y][x] == 0) {
				   int adjacentMines = countAdjacentMines(y, x, mineLocations, cellsY, cellsX);
				   GLuint numberTexture = 0;
				   switch (adjacentMines) {
					   case 1: numberTexture = one; break;
					   case 2: numberTexture = two; break;
				           case 3: numberTexture = three; break;
				           case 4: numberTexture = four; break;
				           case 5: numberTexture = five; break;
				           default: break;
				   }
				   if (numberTexture) {
					  renderTexture(shaderProgram, vao, numberTexture, &rect);
     
			           }
			    }
		   }
	    } 
	    glUseProgram(shaderProgram);
            glUniformMatrix4fv(projLocation, 1, GL_FALSE, projection);
	    
   	    SDL_GL_SwapWindow(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &cell);
    glDeleteTextures(1, &flag);
    glDeleteTextures(1, &mine);
    glDeleteTextures(1, &one);
    glDeleteTextures(1, &two);
    glDeleteTextures(1, &three);
    glDeleteTextures(1, &four);
    glDeleteTextures(1, &five);
    Mix_FreeChunk(soundEffect[0]);
    Mix_FreeChunk(soundEffect[1]);
    Mix_CloseAudio();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}



