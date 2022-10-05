#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

//  g++ main.cpp -I/opt/homebrew/Cellar/sfml/2.5.1_2/include -o prog -L/opt/homebrew/Cellar/sfml/2.5.1_2/lib -lsfml-graphics -lsfml-window -lsfml-system

const float SPEED = 400.0, DEFAULT_PLAYER_RADIUS = 15, DEFAULT_ENEMY_RADIUS = 30, UNIT_SIZE = 20.0, SHOT_CD = 0.420, ENEMY_SHOT_CD = 0.8, ENEMY_CD = 1, ATK_RANGE = 400;
const float PLAYER_PROJECTILE_SPEED = 800, ENEMY_PROJECTILE_SPEED = 700, ENEMY_SPEED = 420;
const int WIDTH = 1600, HEIGHT = 900;

int SCORE = 0;

using namespace sf;
using namespace std;



float max(float a, float b) {
    return a > b ? a : b;
}

struct PlayerProjectile {
    Vector2f pos;
    Vector2f velocity;
    double distTravelled = 0.0;
    double radius = DEFAULT_PLAYER_RADIUS;
};

struct EnemyProjectile {
    Vector2f pos;
    Vector2f velocity;
    double radius = DEFAULT_ENEMY_RADIUS;
};

struct Enemy {
    Vector2f pos;
};


// projectiles
vector <PlayerProjectile> playerProjectiles;
vector <EnemyProjectile> enemyProjectiles;
vector <Enemy> enemies;

// timer vars
float remainingShotCD = 0.0;
float enemyShotCD = 0.0;
float enemyCD = 0.0;

float len(Vector2f v) {
    return sqrt(v.x*v.x + v.y*v.y);
}


void addPlayerProjectile(Vector2f playerPos, Vector2f mousePos) {
    // scale velocity to bullet speed
    Vector2f velocity = mousePos - playerPos;
    velocity /= len(velocity);
    velocity *= PLAYER_PROJECTILE_SPEED;
    PlayerProjectile newProj;
    newProj.pos = playerPos;
    newProj.velocity = velocity;
    playerProjectiles.push_back(newProj);
}

void addEnemy() {
    // choose which of four sides it comes from
    Enemy e;
    if (rand() % 2 == 0) {
        // left or right
        if (rand() % 2 == 0) {
            // left
            e.pos.x = 0;
            e.pos.y = rand() % HEIGHT;
        } else {
            // right
            e.pos.x = WIDTH - 1;
            e.pos.y = rand() % HEIGHT;
        }
    } else {
        // top or bot
        if (rand() % 2 == 0) {
            // top
            e.pos.y = 0;
            e.pos.x = rand() % WIDTH;
        } else {
            //bot
            e.pos.y = HEIGHT - 1;
            e.pos.x = rand() % WIDTH;
        }
    }
    enemies.push_back(e);
}

void addEnemyShot(Vector2f playerPos) {
    EnemyProjectile e;
    if (rand() % 2 == 0) {
        // left or right
        if (rand() % 2 == 0) {
            // left
            e.pos.x = 0;
            e.pos.y = rand() % HEIGHT;
        } else {
            // right
            e.pos.x = WIDTH - 1;
            e.pos.y = rand() % HEIGHT;
        }
    } else {
        // top or bot
        if (rand() % 2 == 0) {
            // top
            e.pos.y = 0;
            e.pos.x = rand() % WIDTH;
        } else {
            //bot
            e.pos.y = HEIGHT - 1;
            e.pos.x = rand() % WIDTH;
        }
    }
    Vector2f v = playerPos - e.pos;
    v /= sqrt(v.x*v.x + v.y*v.y);
    v *= ENEMY_PROJECTILE_SPEED;
    e.velocity = v;
    enemyProjectiles.push_back(e);
}

void movePlayerProjectiles(float t) {
    for (int i = playerProjectiles.size() - 1; i >= 0; i--) {
        playerProjectiles[i].pos += playerProjectiles[i].velocity * t;
        playerProjectiles[i].distTravelled += len(playerProjectiles[i].velocity) * t;
        if(playerProjectiles[i].distTravelled > ATK_RANGE) {
            playerProjectiles.erase(playerProjectiles.begin() + i);
            continue;
        }
        // check for collisions (there should only be one bullet at a time but I used a vector in case any funny stuff happens)
        // so realistically even though this seems like O(# bullets * # enemies) its really just O(# enemies) cuz theres usually only one bullet
        // ^ staying true to the online simulator at loldodgegame.com or something
        for (int j = enemies.size(); j >= 0; j--) {
            if(len(enemies[j].pos - playerProjectiles[i].pos) <= DEFAULT_PLAYER_RADIUS + UNIT_SIZE) {
                // colliding
                enemies.erase(enemies.begin() + j);
                SCORE++;
                playerProjectiles.erase(playerProjectiles.begin() + i);
                break; // each bullet can only hit an enemy once
            }
        }
    }
}

void moveEnemyProjectiles(Vector2f playerPos, float t) {
    for (int i = enemyProjectiles.size() - 1; i >= 0; i--) {
        enemyProjectiles[i].pos += enemyProjectiles[i].velocity * t;
        // check if collides or off screen
        Vector2f pos = enemyProjectiles[i].pos;
        if(pos.x < 0 || pos.x > WIDTH || pos.y < 0 || pos.y > HEIGHT) {
            // delete
            enemyProjectiles.erase(enemyProjectiles.begin() + i);
            continue;
        }
        // check collision
        if(len(pos - playerPos) <= UNIT_SIZE + DEFAULT_ENEMY_RADIUS) {
            SCORE = 0;
            enemyProjectiles.erase(enemyProjectiles.begin() + i);
            continue;
        }
    }
}

void moveEnemies(Vector2f playerPos, float t) {
    for(int i = enemies.size() - 1; i >= 0; i--) {
        Vector2f v = (playerPos - enemies[i].pos);
        v /= len(v);
        v *= ENEMY_SPEED * t;
        enemies[i].pos += v;
        if(len(enemies[i].pos - playerPos) <= 2 * UNIT_SIZE) {
            SCORE = 0;
            enemies.erase(enemies.begin() + i);
            continue;
        }
    }
}

void drawEverything(RenderWindow &w) {
    // draw player projectiles
    for  (int i = 0; i < playerProjectiles.size(); i++) {
        CircleShape pp(DEFAULT_PLAYER_RADIUS);
        pp.setFillColor(Color::Cyan);
        pp.setPosition(playerProjectiles[i].pos);
        w.draw(pp);
    }

    // enemy proj
    for  (int i = 0; i < enemyProjectiles.size(); i++) {
        CircleShape pp(DEFAULT_ENEMY_RADIUS);
        pp.setFillColor(Color::Magenta);
        pp.setPosition(enemyProjectiles[i].pos);
        w.draw(pp);
    }

    // enemies
    for  (int i = 0; i < enemies.size(); i++) {
        CircleShape pp(UNIT_SIZE);
        pp.setFillColor(Color::Red);
        pp.setPosition(enemies[i].pos);
        w.draw(pp);
    }
}


int main()
{
    srand(1540);
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "", Style::Close | Style::None);
    CircleShape player(UNIT_SIZE);
    Clock clock;
    player.setFillColor(Color::Blue);
    Vector2f v = Vector2f(0.0f, 0.0f);
    Vector2f playerPos = Vector2f(100.0f, 100.0f);
    Vector2f mousePos = Vector2f(playerPos);
    Vector2f mouseMovePos = Vector2f(mousePos);
    player.setOrigin(20.0f, 20.0f);
    player.setPosition(playerPos);

    while (window.isOpen())
    {
        float t = (float)clock.getElapsedTime().asSeconds();
        remainingShotCD = max(0.0, remainingShotCD - t);
        enemyShotCD = max(0.0, enemyShotCD - t);
        enemyCD = max(0.0, enemyCD - t);
        float f = 1.f / t;
        Event event;
        while (window.pollEvent(event)){
            if (event.type == Event::Closed){
                window.close();
            }
        }
        //printf("Framerate: %f\n", f);
        // v = Vector2f(0.0f, 0.0f);
        // if(Keyboard::isKeyPressed(Keyboard::Key::A)) {
        //     //player.move(-speed * t, 0.0f);
        //     v.x-=1.0f;
        // }
        // if(Keyboard::isKeyPressed(Keyboard::Key::S)) {
        //     //player.move(0.0f, speed * t);
        //     v.y+=1.0f;
        // }
        // if(Keyboard::isKeyPressed(Keyboard::Key::D)) {
        //     //player.move(speed * t, 0.0f);
        //     v.x+=1.0f;
        // }
        // if(Keyboard::isKeyPressed(Keyboard::Key::W)) {
        //     //player.move(0.0f, -speed * t);
        //     v.y-=1.0f;
        // }
        if(Mouse::isButtonPressed(Mouse::Right)) {
            mousePos = (Vector2f)Mouse::getPosition(window);
            mouseMovePos = mousePos;
        }

        mousePos = (Vector2f)Mouse::getPosition(window);

        if(Keyboard::isKeyPressed(Keyboard::Key::S)) {
            // STOP
            mouseMovePos = playerPos;
            v.x = v.y = 0;
        }
        if(playerPos.x != mouseMovePos.x || playerPos.y != mouseMovePos.y) {
            v = mouseMovePos - playerPos;
            v/=sqrt(v.x*v.x+v.y*v.y);
        }
        // std::cout <<mousePos.x << ' ' << mousePos.y << ' ' << v.x << ' ' << v.y << endl;

        // UPDATE PLAYER
        playerPos += v*SPEED*t;
        player.setPosition(playerPos);

        // Check if player pressed projectile shoot button
        if (Keyboard::isKeyPressed(Keyboard::Key::Q)) {
            // shoot
            if(remainingShotCD == 0.0) {
                addPlayerProjectile(playerPos, mousePos);
                remainingShotCD = SHOT_CD;
            }
        }

        // ADD ENEMIES AND ENEMY PROJECTILES
        if (enemyShotCD == 0.0) {
            enemyShotCD = ENEMY_SHOT_CD;
            addEnemyShot(playerPos);
        }

        if (enemyCD == 0.0) {
            enemyCD = ENEMY_CD;
            addEnemy();
        }

        // UPDATE
        movePlayerProjectiles(t);
        moveEnemyProjectiles(playerPos, t);
        moveEnemies(playerPos, t);

        cout << SCORE << ' ' << remainingShotCD << endl;
        clock.restart();

        window.clear();
        window.draw(player);
        drawEverything(window);
        window.display();
    }

    return 0;
}
