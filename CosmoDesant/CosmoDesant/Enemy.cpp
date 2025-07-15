#include "Enemy.h"
#include <iostream>
#include <string>

Enemy::Enemy(int startX, int startY, int hp, int dmg, float rate, const std::vector<std::string>& gfx)
    : x(startX), y(startY), health(hp), damage(dmg), fireRate(rate),
    graphic(gfx), active(true) {
}

void Enemy::Move() {
    
    y++;
       
    if (y >= GameConstants::height) active = false;
}

void Enemy::Shoot() {


}

void Enemy::TakeDamage(int amount) {

    health -= amount;
    if (health <= 0) active = false;
}

void Enemy::Show(char map[GameConstants::height][GameConstants::width]) const {
    if (!active) return;

    for (int dy = 0; dy < graphic.size(); ++dy) {
        for (int dx = 0; dx < graphic[dy].size(); ++dx) {
            int renderX = x + dx - graphic[dy].size() / 2;
            int renderY = y + dy - graphic.size() / 2;

            if (renderX >= 0 && renderX < GameConstants::width &&
                renderY >= 0 && renderY < GameConstants::height) {
                map[renderY][renderX] = graphic[dy][dx];
            }
        }
    }
}

RECT Enemy::GetRect() const {
    return { x - 2, y - 3, x + 2, y + 3 };
}

bool Enemy::IsActive() const {
    return active;
}



// Фабричный метод для создания врагов
Enemy* Enemy::CreateEnemy(EnemyType type, int x, int y) {
    switch (type) {
    case BASIC: return new Enemy(x, y, 3, 1, 0.5f, {
            " $ ",
            "$$$",
            " $ "
        });
    case FAST: return new Enemy(x, y, 1, 1, 1.5f, {
            " $ ",
            "$$$",
            " $ "
        });
    case TANK: return new Enemy(x, y, 10, 2, 0.3f, {
            " # ",
            " # ",
            "###"
        });
    case BOSS: return new Enemy(x, y, 30, 3, 0.2f, {
            "###",
            "###",
            "###"
        });
    default: return new Enemy(x, y, 3, 1, 0.5f, {
            " # ",
            "###",
            " # "
        });
    }
}