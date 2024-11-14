#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <windows.h>

typedef struct Node {
    int x;
    int y;
    int prevX;
    int prevY;
    struct Node* next;
} SnakeNode;

SnakeNode* createSnake(int length, int origX, int origY) {
    SnakeNode* head = (SnakeNode*) malloc(sizeof(SnakeNode));
    if (head == NULL) {
        return NULL;
    }
    
    head->x = origX;
    head->y = origY;
    SnakeNode* prev = head;
    int cnt = 1;
    while (cnt < length) {
        SnakeNode* next = (SnakeNode*) malloc(sizeof(SnakeNode));
        if (next == NULL) {
            SnakeNode* temp = head;
            SnakeNode* del = NULL;
            while (temp != NULL) {
                del = temp;
                temp = temp->next;
                free(del);
            }
            
            return NULL;
        }
        
        next->x = prev->x - 1;
        next->y = prev->y;
        prev->next = next;
        
        prev = next;
        
        cnt++;
    }
    
    prev->next = NULL;
    
    return head;
}

void destroySnake(SnakeNode* snake) {
    SnakeNode* prev = NULL;
    SnakeNode* curr = snake;
    
    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
}

typedef struct {
    int x;
    int y;
} Apple;

Apple* createApple() {
    Apple* res = (Apple*) malloc(sizeof(Apple));
    if (res == NULL) {
        return NULL;
    }
    
    return res;
}

void destroyApple(Apple* apple) {
    free(apple);
}

void clearConsole(void) {
    system("cls");
}

// Function to set the cursor position
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Function to hide the cursor
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = FALSE;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// Function to show the cursor
void showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = TRUE; // Set cursor visibility to TRUE
    cursorInfo.dwSize = 1; // Set cursor size (1 is the minimum size)
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

int foldover(int num, int min, int max) {
    if (num < min) {
        num = max - (min - num);
    } else {
        num %= max;
    }
    
    return num;
}

int clearBuffer(char* buffer, int size) {
    memset(buffer, ' ', (sizeof(char) * ((size * 2) * (size * 2))));
    
    return 0;
}

int setTextRed(char* buffer) {
    return sprintf(buffer, "\033[31m");
}

int setTextBlue(char* buffer) {
    return sprintf(buffer, "\033[34m");
}

int setTextGreen(char* buffer) {
    return sprintf(buffer, "\033[32m");
}

int resetText(char* buffer) {
    return sprintf(buffer, "\033[0m");
}

int render(char* buffer, SnakeNode* snake, Apple* apple, int size, int** grid, int score, char headChar, char bodyChar, char appleChar, int playing) {
    clearBuffer(buffer, size);
    int cnt = 0;
    
    cnt += sprintf(buffer + cnt, "Score: %05d\n\n", score);
    
    //cnt += setTextBlue(buffer + cnt);
    
    for (int i = 0; i < size + 2; i++) {
        cnt += sprintf(buffer + cnt, "# ");
    }
    
    cnt += sprintf(buffer + cnt, "\n");
    
    for (int i = 0; i < size; i++) {
        cnt += sprintf(buffer + cnt, "# ");
        
        for (int j = 0; j < size; j++) {
            //cnt += resetText(buffer + cnt);
            if (grid[i][j] == 1) {
                if (snake->y == i && snake->x == j) {
                    if (playing == 0) {
                        //cnt += setTextRed(buffer + cnt);
                    }
                    cnt += sprintf(buffer + cnt, "%c ", headChar);
                } else {
                    cnt += sprintf(buffer + cnt, "%c ", bodyChar);
                }
            } else {
                if (apple->y == i && apple->x == j) {
                    //cnt += setTextGreen(buffer + cnt);
                    cnt += sprintf(buffer + cnt, "%c ", appleChar);
                } else {
                    cnt += sprintf(buffer + cnt, "  ");
                }
            }
        }
        
        //cnt += setTextBlue(buffer + cnt);
        
        cnt += sprintf(buffer + cnt, "#\n");
    }
    
    for (int i = 0; i < size + 2; i++) {
        cnt += sprintf(buffer + cnt, "# ");
    }
    
    //cnt += resetText(buffer + cnt);
    
    buffer[cnt] = '\0';
    
    //clearConsole();
    setCursorPosition(0, 0);
    printf("%s", buffer);
    
    return 0;
}

void updateGrid(int** grid, SnakeNode* snake, int size) {
    for (int i = 0; i < size; i++) {
        memset(grid[i], 0, sizeof(int) * size);
    }
    
    //printf("grid set.\n");
    
    SnakeNode* curr = snake;
    
    while (curr != NULL) {
        //printf("pos = (%d, %d)\n", curr->x, curr->y);
        grid[curr->y][curr->x] = 1;
        
        curr = curr->next;
    }
    
    //printf("done.\n");
}

int growSnake(SnakeNode* snake) {
    SnakeNode* curr = snake; SnakeNode* prev = NULL;
    
    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    
    SnakeNode* newPart = (SnakeNode*) malloc(sizeof(SnakeNode));
    
    if (newPart == NULL) { return -1; }
    
    newPart->x = prev->prevX;
    newPart->y = prev->prevY;
    prev->next = newPart;
    newPart->next = NULL;
    
    return 0;
}

void relocateApple(Apple* apple, int** grid, int size) {
    int accepted = 0; int x; int y;
    
    while (!accepted) {
        x = rand() % size;
        y = rand() % size;
        
        if (grid[y][x] == 0) {
            accepted = 1;
        }
    }
    
    apple->x = x;
    apple->y = y;
}

int changeDir(int dir, int newDir) {
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            switch (ch) {
                case 72:
                    //UP
                    if (dir != 1) {
                        return 3;
                    }
                    break;
                case 80:
                    //DOWN
                    if (dir != 3) {
                        return 1;
                    }
                    break;
                case 75:
                    //LEFT
                    if (dir != 0) {
                        return 2;
                    }
                    break;
                case 77:
                    //RIGHT
                    if (dir != 2) {
                        return 0;
                    }
                    break;
            }
        }
    }
    
    return newDir;
}

int moveSnake(SnakeNode* snake, Apple* apple, int dir, int size, int** grid, int* score) {
    //printf("checked press. dir = %d\n", dir);
    
    snake->prevX = snake->x;
    snake->prevY = snake->y;
    
    switch (dir) {
        case 0:
            (snake->x)++;
            break;
        case 1:
            (snake->y)++;
            break;
        case 2:
            (snake->x)--;
            break;
        case 3:
            (snake->y)--;
            break;
    }
    
    snake->x = foldover(snake->x, 0, size);
    snake->y = foldover(snake->y, 0, size);
    
    SnakeNode* curr = snake->next;
    SnakeNode* prev = snake;
    
    while (curr != NULL) {
        curr->prevX = curr->x;
        curr->prevY = curr->y;
        curr->x = prev->prevX;
        curr->y = prev->prevY;
        
        if (curr->x == snake->x && curr->y == snake->y) {
            return 1;
        }
        
        prev = curr;
        curr = curr->next;
    }
    
    if (snake->x == apple->x && snake->y == apple->y) {
        (*score) += 100;
        
        int res = growSnake(snake);
        
        if (res == -1) { return -1; }
        
        updateGrid(grid, snake, size);
        relocateApple(apple, grid, size);
    } else {
        updateGrid(grid, snake, size);
    }
    
    return 0;
}

int main(void) {
    srand((unsigned)time(NULL));
    
    showCursor();
    clearConsole();
    
    int size = 10;
    char bodyChar = 'H';
    char headChar = 'D';
    char appleChar = 'o';
    double updateTime = 0.125;
    
    printf("       __\n"
           "      {0O}\n"
           "      \\__/\n"
           "      /^/\n"
           "     ( (             \n"
           "     \\_\\_____\n"
           "     (_______)\n"
           "    (_________()Oo\n"
           " \n");
    printf("   _____  _   _            _  __ ______ \n"
           "  / ____|| \\ | |    /\\    | |/ /|  ____|\n"
           " | (___  |  \\| |   /  \\   | \' / | |__   \n"
           "  \\___ \\ | . ` |  / /\\ \\  |  <  |  __|  \n"
           "  ____) || |\\  | / ____ \\ | . \\ | |____ \n"
           " |_____/ |_| \\_|/_/    \\_\\|_|\\_\\|______|\n");
    printf("\n\tPress ENTER to start game.");
    
    while (getchar() != '\n');
    
    startGame:
    
    int choice = 0;
    
    printf("\n\nChoose the side lengths of the game area by inputting the corresponding number:\n1) 10\n2) 20\n3) 30\n-> ");
    
    while (choice < 1 || choice > 3) {
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                size = 10;
                break;
            case 2:
                size = 20;
                break;
            case 3:
                size = 30;
                break;
            default:
                printf("Option not available. Choose again\n-> ");
                break;
        }
    }
    
    while (getchar() != '\n');
    
    printf("You've chosen side lengths of size %d.\n\n"
           "Press ENTER to start playing.", size);
    
    while (getchar() != '\n');
    
    int score = 0;
    int running = 1;
    int snakeDir = 0;
    int newDir = 0;
    clock_t lastUpdate = clock();
    
    char* renderBuff = (char*) malloc(sizeof(char) * ((size * 2) * (size * 2)));
    if (renderBuff == NULL) {
        printf("ERROR ALLOCATING MEMORY.");
        return 0;
    }
    
    int** grid = (int**) malloc(sizeof(int*) * size);
    if (grid == NULL) { printf("ERROR ALLOCATING MEMORY."); free(renderBuff); return 0; }
    
    //printf("Grid alloc.\n");
    
    for (int i = 0; i < size; i++) {
        grid[i] = (int*) malloc(sizeof(int) * size);
        
        if (grid[i] == NULL) {
            printf("ERROR ALLOCATING MEMORY.");
            
            for (int j = 0; j < i; j++) {
                free(grid[j]);
            }
            
            free(grid);
            free(renderBuff);
            
            return 0;
        }
        
        memset(grid[i], 0, sizeof(int) * size);
    }
    
    //printf("grid[i] alloc.\n");
    
    SnakeNode* snake = createSnake(3, size / 3, size / 2);
    if (snake == NULL) { 
        printf("ERROR ALLOCATING MEMORY.");
        
        for (int i = 0; i < size; i++) {
            free(grid[i]);
        }
        
        free(grid);
        free(renderBuff);
        
        return 0; 
    }
    
    //printf("snake alloc.\n");
    
    Apple* apple = createApple();
    if (apple == NULL) { 
        printf("ERROR ALLOCATING MEMORY.");
        
        for (int i = 0; i < size; i++) {
            free(grid[i]);
        }
        
        free(grid);
        free(renderBuff);
        destroySnake(snake);
        
        return 0;
    }
    
    //printf("apple alloc.\n");
    
    updateGrid(grid, snake, size);
    
    //printf("grid updated.\n");
    
    
    relocateApple(apple, grid, size);
    
    //printf("apple placed.\n");
    
    clearConsole();
    hideCursor();
    
    while (running) {
        clock_t current = clock();
        double elapsed = (double) (current - lastUpdate) / CLOCKS_PER_SEC;
        
        newDir = changeDir(snakeDir, newDir);
        
        if (elapsed >= updateTime) {
            //printf("Update.\n");
            
            snakeDir = newDir;
            
            int res = moveSnake(snake, apple, snakeDir, size, grid, &score);
            
            //printf("Moved.\n");
            
            switch (res) {
                case 1:
                    // GAMEOVER
                    running = 0;
                    break;
                case -1:
                    // ERROR
                    printf("FATAL ERROR.\n");
                    destroyApple(apple);
                    destroySnake(snake);
                    for (int i = 0; i < size; i++) {
                        free(grid[i]);
                    }
                    free(grid);
                    return 0;
                    break;
            }
            
            //printf("rendering.\n");
            
            //while (getchar() != '\n');
            
            res = render(renderBuff, snake, apple, size, grid, score, headChar, bodyChar, appleChar, running);
            
            //while (getchar() != '\n');
            
            //printf("rendered.\n");
            
            if (res == -1) {
                printf("FATAL ERROR.\n");
                destroyApple(apple);
                destroySnake(snake);
                for (int i = 0; i < size; i++) {
                    free(grid[i]);
                }
                free(grid);
                free(renderBuff);
                return 0;
            }
            
            lastUpdate = clock();
        }
    }
    
    printf("\n\n\033[0;31mGAME OVER!\n"
           "\033[0mYour score was \033[0;32m%05d!\n\n"
           "\033[0mPress \033[0;32mENTER \033[0mto \033[0;32mplay again\033[0m or hit \033[0;31mq \033[0mto \033[0;31mquit\033[0m.", score);
    
    char c;
    while((c = (char)_getch()) != 'q' && c != '\r');
    
    showCursor();
    
    destroySnake(snake);
    
    destroyApple(apple);
    
    for (int i = 0; i < size; i++) {
        free(grid[i]);
    }
    
    free(grid);
    free(renderBuff);
    
    if (c == '\r') {
        goto startGame;
    }
    
    return 0;
}