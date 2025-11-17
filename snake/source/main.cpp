#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <deque>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <switch.h>
#include <SDL.h>
#include <SDL_mixer.h>

// Game constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int GRID_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;
const int INITIAL_SPEED = 150; // milliseconds per move

// Colors
const SDL_Color COLOR_BACKGROUND = {20, 20, 30, 255};
const SDL_Color COLOR_SNAKE = {50, 200, 50, 255};
const SDL_Color COLOR_SNAKE_HEAD = {100, 255, 100, 255};
const SDL_Color COLOR_FOOD = {255, 50, 50, 255};
const SDL_Color COLOR_GRID = {40, 40, 50, 255};

// Direction enum
enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

// Position struct
struct Position {
    int x;
    int y;
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// Snake class
class Snake {
private:
    std::deque<Position> body;
    Direction direction;
    Direction nextDirection;
    bool growing;

public:
    Snake(int startX, int startY) {
        body.push_back({startX, startY});
        body.push_back({startX - 1, startY});
        body.push_back({startX - 2, startY});
        direction = DIR_RIGHT;
        nextDirection = DIR_RIGHT;
        growing = false;
        SDL_Log("[Snake] Initialized at position (%d, %d) with 3 segments\n", startX, startY);
    }

    void setDirection(Direction dir) {
        // Prevent 180 degree turns
        if ((dir == DIR_UP && direction != DIR_DOWN) ||
            (dir == DIR_DOWN && direction != DIR_UP) ||
            (dir == DIR_LEFT && direction != DIR_RIGHT) ||
            (dir == DIR_RIGHT && direction != DIR_LEFT)) {
            const char* dirNames[] = {"UP", "DOWN", "LEFT", "RIGHT"};
            SDL_Log("[Snake] Direction changed: %s -> %s\n", dirNames[direction], dirNames[dir]);
            nextDirection = dir;
        }
    }

    bool move() {
        direction = nextDirection;
        
        Position newHead = body.front();
        
        switch (direction) {
            case DIR_UP:    newHead.y--; break;
            case DIR_DOWN:  newHead.y++; break;
            case DIR_LEFT:  newHead.x--; break;
            case DIR_RIGHT: newHead.x++; break;
        }

        // Check wall collision
        if (newHead.x < 0 || newHead.x >= GRID_WIDTH ||
            newHead.y < 0 || newHead.y >= GRID_HEIGHT) {
            SDL_Log("[Snake] Wall collision detected at (%d, %d)\n", newHead.x, newHead.y);
            return false;
        }

        // Check self collision
        for (const auto& segment : body) {
            if (segment == newHead) {
                SDL_Log("[Snake] Self collision detected at (%d, %d)\n", newHead.x, newHead.y);
                return false;
            }
        }

        body.push_front(newHead);
        
        if (!growing) {
            body.pop_back();
        } else {
            growing = false;
        }

        return true;
    }

    void grow() {
        growing = true;
        SDL_Log("[Snake] Growing... Current length: %d\n", (int)body.size());
    }

    Position getHead() const {
        return body.front();
    }

    const std::deque<Position>& getBody() const {
        return body;
    }

    int getLength() const {
        return body.size();
    }
};

// Food class
class Food {
private:
    Position pos;

public:
    Food() {
        respawn();
    }

    void respawn() {
        pos.x = rand() % GRID_WIDTH;
        pos.y = rand() % GRID_HEIGHT;
        SDL_Log("[Food] Respawned at position (%d, %d)\n", pos.x, pos.y);
    }

    Position getPosition() const {
        return pos;
    }

    bool checkCollision(const Position& head) const {
        return pos == head;
    }
};

// Game class
class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Snake* snake;
    Food* food;
    Mix_Chunk* eatSound;
    Mix_Chunk* gameOverSound;
    Mix_Music* bgm;
    int score;
    bool gameOver;
    bool paused;
    Uint32 lastMoveTime;
    int moveDelay;

public:
    Game() : window(nullptr), renderer(nullptr), snake(nullptr), food(nullptr),
             eatSound(nullptr), gameOverSound(nullptr), bgm(nullptr), score(0), 
             gameOver(false), paused(false), lastMoveTime(0), moveDelay(INITIAL_SPEED) {
        SDL_Log("[Game] Game object created\n");
    }

    bool init() {
        // Initialize network sockets for nxlink logging
        // This enables stdout/stderr redirection to nxlink or Ryujinx
        socketInitializeDefault();
        nxlinkStdio();
        
        printf("\n");
        printf("========================================\n");
        printf("  Snake Game - Debug Build\n");
        printf("  Logging enabled via nxlink/socket\n");
        printf("========================================\n");
        printf("\n");
        
        SDL_Log("[Game] Starting initialization...\n");
        
        // Initialize romfs for accessing audio files
        Result rc = romfsInit();
        if (R_FAILED(rc)) {
            SDL_Log("[Game] romfsInit failed: 0x%x\n", rc);
            // Continue anyway, game will work without audio
        } else {
            SDL_Log("[Game] romfsInit succeeded\n");
        }

        // Initialize SDL
        SDL_Log("[Game] Initializing SDL subsystems...\n");
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
            SDL_Log("[Game] SDL_Init failed: %s\n", SDL_GetError());
            return false;
        }
        SDL_Log("[Game] SDL initialized successfully\n");

        // Create window
        SDL_Log("[Game] Creating window (%dx%d)...\n", SCREEN_WIDTH, SCREEN_HEIGHT);
        window = SDL_CreateWindow("Snake Game", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (!window) {
            SDL_Log("[Game] SDL_CreateWindow failed: %s\n", SDL_GetError());
            return false;
        }
        SDL_Log("[Game] Window created successfully\n");

        // Create renderer
        SDL_Log("[Game] Creating renderer...\n");
        renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            SDL_Log("[Game] SDL_CreateRenderer failed: %s\n", SDL_GetError());  
            return false;
        }
        SDL_Log("[Game] Renderer created successfully\n");

        // Initialize MP3 support for background music
        SDL_Log("[Game] Initializing audio mixer...\n");
        int mixFlags = MIX_INIT_MP3;
        if ((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
            SDL_Log("[Game] Mix_Init failed: %s\n", Mix_GetError());
            // Continue anyway, might still work with WAV files
        } else {
            SDL_Log("[Game] Audio mixer MP3 support initialized\n");
        }

        // Initialize SDL_mixer
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0) {
            SDL_Log("[Game] Mix_OpenAudio failed: %s\n", Mix_GetError());
            // Continue anyway, game will work without audio
        } else {
            SDL_Log("[Game] Audio initialized successfully (44100Hz)\n");
        }

        // Load sound effects (if they exist)
        SDL_Log("[Game] Loading sound effects...\n");
        eatSound = Mix_LoadWAV("romfs:/eat.wav");
        if (!eatSound) {
            SDL_Log("[Game] Failed to load eat sound: %s\n", Mix_GetError());
        } else {
            SDL_Log("[Game] Eat sound loaded successfully\n");
        }

        gameOverSound = Mix_LoadWAV("romfs:/gameover.wav");
        if (!gameOverSound) {
            SDL_Log("[Game] Failed to load game over sound: %s\n", Mix_GetError());
        } else {
            SDL_Log("[Game] Game over sound loaded successfully\n");
        }

        // Load background music (if it exists)
        bgm = Mix_LoadMUS("romfs:/bgm.mp3");
        if (!bgm) {
            SDL_Log("[Game] Failed to load BGM: %s\n", Mix_GetError());
        } else {
            SDL_Log("[Game] BGM loaded successfully\n");
            // Play background music in loop
            Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Set to 50% volume
            if (Mix_PlayMusic(bgm, -1) < 0) {
                SDL_Log("[Game] Failed to play BGM: %s\n", Mix_GetError());
            } else {
                SDL_Log("[Game] BGM playing at 50%% volume (looping)\n");
            }
        }

        // Open joysticks
        SDL_Log("[Game] Opening joysticks...\n");
        for (int i = 0; i < 2; i++) {
            if (SDL_JoystickOpen(i)) {
                SDL_Log("[Game] Joystick %d opened successfully\n", i);
            }
        }

        // Initialize game objects
        SDL_Log("[Game] Initializing game objects...\n");
        srand(time(NULL));
        snake = new Snake(GRID_WIDTH / 2, GRID_HEIGHT / 2);
        food = new Food();

        SDL_Log("[Game] Initialization complete! Grid: %dx%d, Initial speed: %dms\n", 
                GRID_WIDTH, GRID_HEIGHT, INITIAL_SPEED);
        return true;
    }

    void handleInput(SDL_Event& event) {
        if (event.type == SDL_JOYBUTTONDOWN) {
            if (event.jbutton.which == 0) {
                SDL_Log("[Input] Joy button pressed: %d\n", event.jbutton.button);
                // D-Pad Up (button 13)
                if (event.jbutton.button == 13) {
                    snake->setDirection(DIR_UP);
                }
                // D-Pad Down (button 15)
                else if (event.jbutton.button == 15) {
                    snake->setDirection(DIR_DOWN);
                }
                // D-Pad Left (button 12)
                else if (event.jbutton.button == 12) {
                    snake->setDirection(DIR_LEFT);
                }
                // D-Pad Right (button 14)
                else if (event.jbutton.button == 14) {
                    snake->setDirection(DIR_RIGHT);
                }
                // A button - restart game
                else if (event.jbutton.button == 0 && gameOver) {
                    SDL_Log("[Input] Restart game requested\n");
                    restart();
                }
                // X button - pause
                else if (event.jbutton.button == 2) {
                    paused = !paused;
                    SDL_Log("[Input] Game %s\n", paused ? "PAUSED" : "RESUMED");
                    if (paused) {
                        Mix_PauseMusic();
                    } else {
                        Mix_ResumeMusic();
                    }
                }
            }
        }
        // Also support keyboard for testing
        else if (event.type == SDL_KEYDOWN) {
            SDL_Log("[Input] Key pressed: %s\n", SDL_GetKeyName(event.key.keysym.sym));
            switch (event.key.keysym.sym) {
                case SDLK_UP:    snake->setDirection(DIR_UP); break;
                case SDLK_DOWN:  snake->setDirection(DIR_DOWN); break;
                case SDLK_LEFT:  snake->setDirection(DIR_LEFT); break;
                case SDLK_RIGHT: snake->setDirection(DIR_RIGHT); break;
                case SDLK_SPACE:
                    paused = !paused;
                    SDL_Log("[Input] Game %s\n", paused ? "PAUSED" : "RESUMED");
                    if (paused) {
                        Mix_PauseMusic();
                    } else {
                        Mix_ResumeMusic();
                    }
                    break;
                case SDLK_r:
                    if (gameOver) {
                        SDL_Log("[Input] Restart game requested\n");
                        restart();
                    }
                    break;
            }
        }
    }

    void update() {
        if (gameOver || paused) {
            return;
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastMoveTime >= (Uint32)moveDelay) {
            lastMoveTime = currentTime;

            if (!snake->move()) {
                gameOver = true;
                SDL_Log("[Game] GAME OVER! Final score: %d, Snake length: %d\n", 
                        score, snake->getLength());
                Mix_HaltMusic(); // Stop BGM when game is over
                if (gameOverSound) {
                    Mix_PlayChannel(-1, gameOverSound, 0);
                }
                return;
            }

            // Check food collision
            if (food->checkCollision(snake->getHead())) {
                snake->grow();
                food->respawn();
                score += 10;
                
                SDL_Log("[Game] Food eaten! Score: %d, Snake length: %d\n", 
                        score, snake->getLength());
                
                // Increase speed slightly
                if (moveDelay > 50) {
                    moveDelay -= 2;
                    SDL_Log("[Game] Speed increased! New delay: %dms\n", moveDelay);
                }

                if (eatSound) {
                    Mix_PlayChannel(-1, eatSound, 0);
                }
            }
        }
    }

    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, 
                               COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
        SDL_RenderClear(renderer);

        // Draw grid lines
        SDL_SetRenderDrawColor(renderer, COLOR_GRID.r, COLOR_GRID.g, 
                               COLOR_GRID.b, COLOR_GRID.a);
        for (int x = 0; x <= SCREEN_WIDTH; x += GRID_SIZE) {
            SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
        }
        for (int y = 0; y <= SCREEN_HEIGHT; y += GRID_SIZE) {
            SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
        }

        // Draw food
        SDL_SetRenderDrawColor(renderer, COLOR_FOOD.r, COLOR_FOOD.g, 
                               COLOR_FOOD.b, COLOR_FOOD.a);
        Position foodPos = food->getPosition();
        SDL_Rect foodRect = {foodPos.x * GRID_SIZE + 2, foodPos.y * GRID_SIZE + 2, 
                            GRID_SIZE - 4, GRID_SIZE - 4};
        SDL_RenderFillRect(renderer, &foodRect);

        // Draw snake
        const auto& body = snake->getBody();
        bool isHead = true;
        for (const auto& segment : body) {
            if (isHead) {
                SDL_SetRenderDrawColor(renderer, COLOR_SNAKE_HEAD.r, COLOR_SNAKE_HEAD.g, 
                                      COLOR_SNAKE_HEAD.b, COLOR_SNAKE_HEAD.a);
                isHead = false;
            } else {
                SDL_SetRenderDrawColor(renderer, COLOR_SNAKE.r, COLOR_SNAKE.g, 
                                      COLOR_SNAKE.b, COLOR_SNAKE.a);
            }
            
            SDL_Rect segmentRect = {segment.x * GRID_SIZE + 1, segment.y * GRID_SIZE + 1, 
                                   GRID_SIZE - 2, GRID_SIZE - 2};
            SDL_RenderFillRect(renderer, &segmentRect);
        }

        // Draw UI text (score, game over, etc.)
        if (gameOver) {
            SDL_Color redColor = {255, 100, 100, 255};
            SDL_Color whiteColor = {255, 255, 255, 255};
            // Large "GAME OVER" text (fontSize = 3)
            drawText("GAME OVER", SCREEN_WIDTH / 2 - 108, SCREEN_HEIGHT / 2 - 60, redColor, 3);
            // Normal size instruction (fontSize = 2)
            drawText("Press A to restart", SCREEN_WIDTH / 2 - 144, SCREEN_HEIGHT / 2 + 20, whiteColor, 2);
        } else if (paused) {
            SDL_Color yellowColor = {255, 255, 100, 255};
            SDL_Color grayColor = {150, 150, 150, 255};
            // Large "PAUSED" text (fontSize = 3)
            drawText("PAUSED", SCREEN_WIDTH / 2 - 72, SCREEN_HEIGHT / 2 - 40, yellowColor, 3);
            // Normal size instruction (fontSize = 2)
            drawText("Press X to resume", SCREEN_WIDTH / 2 - 136, SCREEN_HEIGHT / 2 + 10, yellowColor, 2);
            // Small copyright text (fontSize = 1)
            drawText("Powered by Feizhu @2025", SCREEN_WIDTH / 2 - 92, SCREEN_HEIGHT / 2 + 35, grayColor, 1);
        }

        // Draw score (fontSize = 2)
        char scoreText[32];
        snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
        SDL_Color scoreColor = {100, 255, 100, 255};
        drawText(scoreText, 10, 10, scoreColor, 2);

        SDL_RenderPresent(renderer);
    }

    void drawChar(char c, int x, int y, SDL_Color color, int scale = 2) {
        // Simple 5x7 bitmap font for basic ASCII characters
        static const uint8_t font[][7] = {
            // '0'
            {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E},
            // '1'
            {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F},
            // '2'
            {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F},
            // '3'
            {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E},
            // '4'
            {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78},
            // '5'
            {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E},
            // '6'
            {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E},
            // '7'
            {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C},
            // '8'
            {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E},
            // '9'
            {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E},
            // 'A'
            {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33},
            // 'B'
            {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F},
            // 'C'
            {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C},
            // 'D'
            {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F},
            // 'E'
            {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F},
            // 'G'
            {0x3C, 0x66, 0x03, 0x3B, 0x33, 0x66, 0x5C},
            // 'M'
            {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63},
            // 'O'
            {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C},
            // 'P'
            {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F},
            // 'R'
            {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67},
            // 'S'
            {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E},
            // 'T'
            {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E},
            // 'U'
            {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F},
            // 'V'
            {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C},
            // 'a'
            {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E},
            // 'c'
            {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E},
            // 'd'
            {0x30, 0x30, 0x3E, 0x33, 0x33, 0x33, 0x6E},
            // 'e'
            {0x00, 0x00, 0x1E, 0x33, 0x3F, 0x03, 0x1E},
            // 'i'
            {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E},
            // 'l'
            {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0F},
            // 'n'
            {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33},
            // 'o'
            {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E},
            // 'r'
            {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F},
            // 's'
            {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F},
            // 't'
            {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18},
            // 'u'
            {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E},
            // 'w'
            {0x00, 0x00, 0x33, 0x33, 0x33, 0x3F, 0x36},
            // 'F'
            {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F},
            // 'H'
            {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33},
            // 'I'
            {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E},
            // 'L'
            {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F},
            // 'N'
            {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63},
            // 'W'
            {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63},
            // 'h'
            {0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x67},
            // 'k'
            {0x06, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67},
            // 'y'
            {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30},
            // 'z'
            {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F},
            // 'b'
            {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B},
            // 'f'
            {0x1C, 0x36, 0x06, 0x0F, 0x06, 0x06, 0x0F},
            // 'g'
            {0x00, 0x00, 0x3E, 0x33, 0x33, 0x3E, 0x30},
            // 'm'
            {0x00, 0x00, 0x37, 0x7F, 0x6B, 0x63, 0x63},
            // 'p'
            {0x00, 0x00, 0x1F, 0x33, 0x33, 0x1F, 0x03},
            // 'v'
            {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C},
            // '@'
            {0x1E, 0x33, 0x3B, 0x3B, 0x3B, 0x03, 0x1E},
            // ':'
            {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00},
            // ' ' (space)
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        };
        
        int charIndex = -1;
        if (c >= '0' && c <= '9') charIndex = c - '0';
        else if (c >= 'A' && c <= 'E') charIndex = 10 + (c - 'A');
        else if (c == 'F') charIndex = 37;
        else if (c == 'G') charIndex = 15;
        else if (c == 'H') charIndex = 38;
        else if (c == 'I') charIndex = 39;
        else if (c == 'L') charIndex = 40;
        else if (c == 'M') charIndex = 16;
        else if (c == 'N') charIndex = 41;
        else if (c == 'O') charIndex = 17;
        else if (c == 'P') charIndex = 18;
        else if (c == 'R') charIndex = 19;
        else if (c == 'S') charIndex = 20;
        else if (c == 'T') charIndex = 21;
        else if (c == 'U') charIndex = 22;
        else if (c == 'V') charIndex = 23;
        else if (c == 'W') charIndex = 42;
        else if (c == 'a') charIndex = 24;
        else if (c == 'b') charIndex = 47;
        else if (c == 'c') charIndex = 25;
        else if (c == 'd') charIndex = 26;
        else if (c == 'e') charIndex = 27;
        else if (c == 'f') charIndex = 48;
        else if (c == 'g') charIndex = 49;
        else if (c == 'h') charIndex = 43;
        else if (c == 'i') charIndex = 28;
        else if (c == 'k') charIndex = 44;
        else if (c == 'l') charIndex = 29;
        else if (c == 'm') charIndex = 50;
        else if (c == 'n') charIndex = 30;
        else if (c == 'o') charIndex = 31;
        else if (c == 'p') charIndex = 51;
        else if (c == 'r') charIndex = 32;
        else if (c == 's') charIndex = 33;
        else if (c == 't') charIndex = 34;
        else if (c == 'u') charIndex = 35;
        else if (c == 'v') charIndex = 52;
        else if (c == 'w') charIndex = 36;
        else if (c == 'y') charIndex = 45;
        else if (c == 'z') charIndex = 46;
        else if (c == '@') charIndex = 53;
        else if (c == ':') charIndex = 54;
        else if (c == ' ') charIndex = 55;
        
        if (charIndex < 0) return;
        
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        
        for (int row = 0; row < 7; row++) {
            uint8_t line = font[charIndex][row];
            for (int col = 0; col < 8; col++) {
                if (line & (1 << col)) {
                    SDL_Rect pixel = {x + col * scale, y + row * scale, scale, scale};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
    }
    
    void drawText(const char* text, int x, int y, SDL_Color color = {255, 255, 255, 255}, int fontSize = 2) {
        int curX = x;
        int curY = y;
        const int charSpacing = 8 * fontSize;     // 8 pixels base width * scale
        const int lineHeight = 7 * fontSize + 4;  // 7 pixels base height * scale + spacing
        
        for (size_t i = 0; i < strlen(text); i++) {
            if (text[i] == '\n') {
                // Handle newline
                curX = x;
                curY += lineHeight;
            } else {
                drawChar(text[i], curX, curY, color, fontSize);
                curX += charSpacing;
            }
        }
    }

    void restart() {
        SDL_Log("[Game] Restarting game...\n");
        delete snake;
        delete food;
        
        snake = new Snake(GRID_WIDTH / 2, GRID_HEIGHT / 2);
        food = new Food();
        score = 0;
        gameOver = false;
        paused = false;
        moveDelay = INITIAL_SPEED;
        lastMoveTime = SDL_GetTicks();
        
        SDL_Log("[Game] Game restarted successfully\n");
        
        // Restart BGM
        if (bgm) {
            Mix_PlayMusic(bgm, -1);
        }
    }

    void cleanup() {
        SDL_Log("[Game] Starting cleanup...\n");
        fflush(stdout);
        fflush(stderr);
        
        if (eatSound) Mix_FreeChunk(eatSound);
        if (gameOverSound) Mix_FreeChunk(gameOverSound);
        if (bgm) {
            Mix_HaltMusic();
            Mix_FreeMusic(bgm);
        }
        
        delete snake;
        delete food;
        
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        
        Mix_Quit();
        Mix_CloseAudio();
        SDL_Quit();
        
        // Clean up romfs
        romfsExit();
        
        SDL_Log("[Game] Cleanup complete. Goodbye!\n");
        fflush(stdout);
        
        // Clean up sockets
        socketExit();
    }

    bool shouldQuit(SDL_Event& event) {
        if (event.type == SDL_QUIT) {
            return true;
        }
        if (event.type == SDL_JOYBUTTONDOWN) {
            if (event.jbutton.which == 0 && event.jbutton.button == 10) {
                return true; // + button
            }
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            return true;
        }
        return false;
    }

    void run() {
        SDL_Event event;
        bool quit = false;
        lastMoveTime = SDL_GetTicks();
        int frameCount = 0;
        Uint32 lastLogTime = SDL_GetTicks();

        SDL_Log("[Game] Starting main game loop\n");

        while (!quit) {
            while (SDL_PollEvent(&event)) {
                if (shouldQuit(event)) {
                    SDL_Log("[Game] Quit event received\n");
                    quit = true;
                    break;
                }
                handleInput(event);
            }

            update();
            render();
            
            frameCount++;
            // Log FPS every 5 seconds
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastLogTime >= 5000) {
                float fps = frameCount / 5.0f;
                SDL_Log("[Game] Performance: %.1f FPS, Score: %d, Snake length: %d, Speed: %dms\n", 
                        fps, score, snake->getLength(), moveDelay);
                frameCount = 0;
                lastLogTime = currentTime;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    // Note: Socket initialization moved to Game::init() to ensure proper setup
    printf("\n*** Snake Game Starting ***\n");
    printf("Preparing to initialize logging...\n\n");
    
    SDL_Log("===========================================\n");
    SDL_Log("[Main] Snake Game Starting...\n");
    SDL_Log("[Main] Version: 1.0 with Debug Logging\n");
    SDL_Log("===========================================\n");
    
    Game game;

    if (!game.init()) {
        SDL_Log("[Main] Game initialization failed!\n");
        return -1;
    }

    game.run();
    game.cleanup();

    SDL_Log("[Main] Game exited normally\n");
    return 0;
}
