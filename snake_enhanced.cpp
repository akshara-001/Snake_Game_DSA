
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <vector>
#include <climits>
#include<string>
using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Position {
    int x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

struct Snake {
    vector<Position> body;
    Direction dir;
    char headChar;
    char bodyChar;
    int colorCode;
    int score = 0;
};

struct Food {
    Position pos;
    char icon;
    int points;
    int colorCode;
};

Snake player, bot;
Food food;
bool gameOver = false, paused = false;
int speed = 120;

void SetCursor(int x, int y) {
    COORD pos = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void HideCursor() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void GenerateFood() {
    int r = rand() % 3;
    if (r == 0)      { food.icon = '@'; food.points = 10; food.colorCode = 31; }
    else if (r == 1) { food.icon = 'O'; food.points = 20; food.colorCode = 33; }
    else             { food.icon = '$'; food.points = 30; food.colorCode = 35; }
    food.pos.x = rand() % WIDTH;
    food.pos.y = rand() % HEIGHT;
}

void Setup() {
    gameOver = false;
    paused = false;
    speed = 120;
    player.body.clear();
    bot.body.clear();

    player.body.push_back({5, 5});
    player.dir = RIGHT;
    player.headChar = '@';
    player.bodyChar = 'o';
    player.colorCode = 32;
    player.score = 0;

    bot.body.push_back({WIDTH - 6, HEIGHT - 6});
    bot.dir = LEFT;
    bot.headChar = '#';
    bot.bodyChar = '+';
    bot.colorCode = 34;
    bot.score = 0;

    srand(time(0));
    GenerateFood();
}

void Draw() {
    SetCursor(0, 0);
    for (int i = 0; i <= WIDTH + 1; i++) cout << "#";
    cout << endl;

    for (int y = 0; y < HEIGHT; y++) {
        cout << "#";
        for (int x = 0; x < WIDTH; x++) {
            bool printed = false;

            if (x == food.pos.x && y == food.pos.y) {
                cout << "\033[1;" << food.colorCode << "m" << food.icon << "\033[0m";
                continue;
            }

            for (int i = 0; i < player.body.size(); i++) {
                if (player.body[i].x == x && player.body[i].y == y) {
                    cout << "\033[1;" << player.colorCode << "m" << (i == 0 ? player.headChar : player.bodyChar) << "\033[0m";
                    printed = true;
                    break;
                }
            }
            if (!printed) {
                for (int i = 0; i < bot.body.size(); i++) {
                    if (bot.body[i].x == x && bot.body[i].y == y) {
                        cout << "\033[1;" << bot.colorCode << "m" << (i == 0 ? bot.headChar : bot.bodyChar) << "\033[0m";
                        printed = true;
                        break;
                    }
                }
            }
            if (!printed) cout << " ";
        }
        cout << "#\n";
    }

    for (int i = 0; i <= WIDTH + 1; i++) cout << "#";
    cout << "\nPlayer Score: " << player.score << "\tBot Score: " << bot.score << endl;
    cout << "[P] Pause  [R] Restart  [X] Exit\n";
}

bool Collides(const Snake& s, Position next) {
    if (next.x < 0 || next.x >= WIDTH || next.y < 0 || next.y >= HEIGHT)
        return true;
    for (auto& p : s.body)
        if (p == next)
            return true;
    return false;
}

void Move(Snake& s, Snake& opponent) {
    Position head = s.body[0];
    Position next = head;
    switch (s.dir) {
        case LEFT: next.x--; break;
        case RIGHT: next.x++; break;
        case UP: next.y--; break;
        case DOWN: next.y++; break;
        default: return;
    }
    if (Collides(s, next) || Collides(opponent, next)) {
        gameOver = true;
        return;
    }
    s.body.insert(s.body.begin(), next);
    if (next == food.pos) {
        s.score += food.points;
        GenerateFood();
        if (speed > 50) speed -= 1;
    } else {
        s.body.pop_back();
    }
}

void BotLogic() {
    Position head = bot.body[0];
    int minDist = INT_MAX;

    Direction dirs[4] = {UP, DOWN, LEFT, RIGHT};
    for (Direction d : dirs) {
        Position next = head;
        switch (d) {
            case UP: next.y--; break;
            case DOWN: next.y++; break;
            case LEFT: next.x--; break;
            case RIGHT: next.x++; break;
            default: break;
        }
        if (!Collides(bot, next) && !Collides(player, next)) {
            int dist = abs(next.x - food.pos.x) + abs(next.y - food.pos.y);
            if (dist < minDist) {
                minDist = dist;
                bot.dir = d;
            }
        }
    }
}

void Input() {
    if (_kbhit()) {
        char key = _getch();
        if (!paused) {
            switch (key) {
                case 'w': if (player.dir != DOWN) player.dir = UP; break;
                case 's': if (player.dir != UP) player.dir = DOWN; break;
                case 'a': if (player.dir != RIGHT) player.dir = LEFT; break;
                case 'd': if (player.dir != LEFT) player.dir = RIGHT; break;
            }
        }
        if (key == 'p') paused = !paused;
        if (key == 'r') Setup();
        if (key == 'x') gameOver = true;
    }
}

void GameOver() {
    system("cls");
    cout << "\033[1;31m\n";
    cout << "============================\n";
    cout << "        GAME OVER           \n";
    cout << "============================\n";
    cout << "\033[0m\n";

    cout << "Player Score: " << player.score << "\n";
    cout << "Bot Score: " << bot.score << "\n";
    cout << "\nCreated by Akshara Chauhan (23BCS11410) & Ishika Chauhan (23BCS13591)\n";
    cout << "\nPress R to restart or X to exit...\n";
    while (true) {
        char c = _getch();
        if (c == 'r') { Setup(); break; }
        if (c == 'x') { gameOver = true; break; }
    }
}

int main() {
    HideCursor();
    Setup();
    while (!gameOver) {
        if (!paused) {
            Draw();
            Input();
            BotLogic();
            Move(player, bot);
            Move(bot, player);
            Sleep(speed);
        } else {
            Input();
        }
    }
    GameOver();
    return 0;
}
