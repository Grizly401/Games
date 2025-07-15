#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "GameConstants.h"

class GameManager {
    //std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<Enemy*> enemies;
    int score;
    int wave;

public:
    GameManager();
    void SpawnWave();
    void Update();
    void AddScore() { score += 10; }
    void Render(char map[GameConstants::height][GameConstants::width]);
    int GetScore() const;
    std::vector<Enemy*> GetEnemies() { return enemies; }
};