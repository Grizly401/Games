#pragma once
#include <windows.h>
#include <chrono>
#include <vector>
#include <string>
#include "GameConstants.h"

enum EnemyType { BASIC, FAST, TANK, BOSS };

class Enemy {
protected:
    int x, y;
    int health;
    int damage;
    float fireRate;
    char appearance;
    bool active;
    std::vector<std::string> graphic;

public:
    Enemy(int startX, int startY, int hp, int dmg, float rate, const std::vector<std::string>& gfx);
    virtual ~Enemy() = default;

    virtual void Move();
    virtual void Shoot();
    void TakeDamage(int amount);
    void Show(char map[GameConstants::height][GameConstants::width]) const;
    RECT GetRect() const;
    bool IsActive() const;

    static Enemy* CreateEnemy(EnemyType type, int x, int y);
};
