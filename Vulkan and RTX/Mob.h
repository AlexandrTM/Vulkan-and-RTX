#pragma once

struct Mob
{
    std::string name;
    int32_t health;
    int32_t maxHealth;
    int32_t attackPower;
    int32_t defense;
    int32_t experienceReward;

    glm::vec3 position;

    Mob(const glm::vec3& position, 
        const std::string& mobName,
        int32_t currentHealth,
        int32_t maxHealth,
        int32_t atk, 
        int32_t def, 
        int32_t exp
    )
        : position(position), 
        name(mobName), 
        health(currentHealth),
        maxHealth(maxHealth),
        attackPower(atk), 
        defense(def), 
        experienceReward(exp)
    {
    }

    bool isAlive() const { return health > 0; }
    const glm::vec3& getPosition() const { return position; }

	void takeDamage(int32_t damage) {
		int32_t damageTaken = std::max(damage - defense, static_cast<int32_t>(1));
		health = std::max(health - damageTaken, 0);
	}

};

