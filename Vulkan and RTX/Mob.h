#pragma once

struct MobTemplate
{
    std::string name;
    int32_t health;
    int32_t maxHealth;
    int32_t attackPower;
    int32_t defense;
    int32_t experienceReward;
};

const std::vector<MobTemplate> mobTemplates = {
    { "Slime",        12, 12, 3,  0,  5  },
    { "Goblin",       15, 15, 5,  1, 10  },
    { "Orc",          17, 17, 7,  2, 20  },
    { "Skeleton",     18, 18, 4,  0, 12  },
    { "Zombie",       25, 25, 6,  3, 15  },

    { "Bandit",       22, 22, 6,  1, 18  },
    { "Dark Mage",    16, 16, 9,  1, 25  },
    { "Wolf",         20, 20, 5,  2, 14  },
    { "Troll",        45, 45, 5,  4, 35  },
    { "Vampire",      28, 28, 7,  3, 40  },

    { "Elemental",    35, 35, 10, 2, 50  },
    { "Shadow Fiend", 40, 40, 11, 1, 60  },
    { "Lich",         30, 30, 13, 3, 75  },
    { "Demon Brute",  60, 60, 14, 5, 90  },
    { "Ancient Wyrm", 85, 85, 18, 7, 120 }
};

struct Mob
{
    std::string name;
    int32_t id;
    int32_t health;
    int32_t maxHealth;
    int32_t attackPower;
    int32_t defense;
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

    static Mob generateRandomMob(const glm::vec3& position, size_t floorIndex, float difficultyScale) {
        size_t templatesPerFloor = mobTemplates.size() / 3;
        size_t start = floorIndex * templatesPerFloor;
        size_t end = std::min(start + templatesPerFloor, mobTemplates.size());

        // Safety fallback in case of invalid floorIndex
        if (start >= mobTemplates.size()) {
            std::cout << "wrond floor index: " << floorIndex << "\n";
            start = 0;
            end = templatesPerFloor;
        }
        
        size_t mobIndex = randomInt(start, end - 1);
        const auto& mobTemplate = mobTemplates[mobIndex];

        float randomFactor = randomReal(0.9, 1.1);
        int32_t health  = static_cast<int32_t>(mobTemplate.health * difficultyScale * randomFactor);
        int32_t attack  = static_cast<int32_t>(mobTemplate.attackPower * difficultyScale * randomFactor);
        int32_t defense = static_cast<int32_t>(mobTemplate.defense * difficultyScale * randomFactor);
        int32_t exp     = static_cast<int32_t>(mobTemplate.experienceReward * difficultyScale * randomFactor);

        return Mob(position, mobTemplate.name, mobIndex, health, health, attack, defense, exp);
    }
};

