#include <cstdlib>
#include <ctime>
#include "GameManager.h"
#include "GameConstants.h"

GameManager::GameManager() : score(0), wave(1) {
    std::srand(std::time(nullptr));
}

void GameManager::SpawnWave() {

    for (int i = 0; i < 1 + wave; i++) {

        int x = 5 + std::rand() % (GameConstants::width - 10);
        EnemyType type = static_cast<EnemyType>(std::rand() % 3);
        enemies.emplace_back(Enemy::CreateEnemy(type, x, 3));

    }

    wave++;
}

void GameManager::Update() {

    for (auto& enemy : enemies) {
        if (enemy->IsActive()) {
            enemy->Move();
            enemy->Shoot();
        }
    }

    // Удаление неактивных врагов
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),[](Enemy* e) { return !e->IsActive(); }),
        enemies.end()
    );

    if (enemies.empty()) {
        SpawnWave();
    }
}


void GameManager::Render(char map[GameConstants::height][GameConstants::width]) {
   
    for (auto& enemy : enemies) {
        enemy->Show(map);
    }

    // Вывод счета
    char scoreStr[20];
    sprintf_s(scoreStr, "Score: %d", score);
    for (int i = 0; i < strlen(scoreStr); i++) {
        map[0][i] = scoreStr[i];
    }
}

int GameManager::GetScore() const {
    return score;
}
