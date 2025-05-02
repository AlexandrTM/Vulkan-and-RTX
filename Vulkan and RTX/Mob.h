#pragma once

struct MobTemplate
{
    std::string name;
    int32_t health;
    int32_t maxHealth;
    int32_t attackPower;
    int32_t defence;
    int32_t experienceReward;
};

const std::vector<MobTemplate> mobTemplates = {
    { "Slime",        12, 12,  3,  0,   5 },
    { "Goblin",       15, 15,  5,  1,   7 },
    { "Orc",          20, 20,  4,  3,  17 },
    { "Skeleton",     18, 18,  6,  0,  10 },
    { "Zombie",       25, 25,  2,  4,  11 },
                                       
    { "Bandit",       22, 22,  6,  2,  18 },
    { "Dark Mage",    16, 16,  9,  2,  25 },
    { "Wolf",         20, 20,  5,  1,  14 },
    { "Troll",        40, 40,  3,  7,  35 },
    { "Vampire",      28, 28,  7,  3,  40 },
                                   
    { "Elemental",    35, 35, 10,  4,  50 },
    { "Shadow Fiend", 40, 40, 11,  5,  60 },
    { "Lich",         25, 25, 19,  3,  75 },
    { "Demon Brute",  60, 60,  8, 11,  90 },
    { "Ancient Wyrm", 85, 85, 15,  9, 120 }
};

struct Mob
{
    std::string name;
    int32_t id;
    int32_t health;
    int32_t maxHealth;
    int32_t attackPower;
    int32_t defence;
    int32_t experienceReward;

    glm::vec3 position;

    Mob(const glm::vec3& position, 
        const std::string& mobName,
        int32_t id,
        int32_t currentHealth,
        int32_t maxHealth,
        int32_t atk, 
        int32_t def, 
        int32_t exp
    )
        : 
        position(position), 
        name(mobName), 
        id(id),
        health(currentHealth),
        maxHealth(maxHealth),
        attackPower(atk), 
        defence(def), 
        experienceReward(exp)
    {
    }

    bool isAlive() const { return health > 0; }
    const glm::vec3& getPosition() const { return position; }

	void takeDamage(int32_t damage) {
		int32_t damageTaken = std::max(damage - defence, static_cast<int32_t>(0));
		health = std::max(health - damageTaken, 0);
	}

    static Mob generateRandomMob(const glm::vec3& position, int32_t floorNumber, float difficultyScale) {
        size_t templatesPerFloor = mobTemplates.size() / 3;
        size_t start = floorNumber * templatesPerFloor;
        size_t end = std::min(start + templatesPerFloor, mobTemplates.size());

        // Safety fallback in case of invalid floorNumber
        if (start >= mobTemplates.size()) {
            std::cout << "wrond floor number: " << floorNumber << "\n";
            start = 0;
            end = templatesPerFloor;
        }
        
        size_t mobIndex = randomInt(start, end - 1);
        const auto& mobTemplate = mobTemplates[mobIndex];

        float randomFactor = randomReal(0.9, 1.1);
        int32_t health  = static_cast<int32_t>(mobTemplate.health * difficultyScale * randomFactor);
        int32_t attack  = static_cast<int32_t>(mobTemplate.attackPower * difficultyScale * randomFactor);
        int32_t defence = static_cast<int32_t>(mobTemplate.defence * difficultyScale * randomFactor);
        int32_t exp     = static_cast<int32_t>(mobTemplate.experienceReward * randomFactor);

        /*std::cout << "Generated Mob: "
            << "Name: " << mobTemplate.name
            << ", MobIndex: " << mobIndex
            << ", Health: " << health
            << ", Attack: " << attack
            << ", Defense: " << defence
            << ", Exp: " << exp
            << "\n";*/

        return Mob(position, mobTemplate.name, mobIndex, health, health, attack, defence, exp);
    }
};

