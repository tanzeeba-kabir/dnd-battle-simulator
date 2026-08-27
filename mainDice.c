
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dice.h"

// TODO: define the struct called Armor with the following fields
// - id: character array of 20
// - armor_class: an integer
struct Armor
{
    char id[20];
    int armor_class;
};

// TODO: define the struct called AttackType with the following fields
// - id: character array of 20
// - dice_count: should be an integer
// - dice_type: use the DiceType enum from dice.h
// - damage_bonus: an other integer
struct AttackType
{
    char id[20];
    int dice_count;
    DiceType dice_type;
    int damage_bonus;
};

// TODO: a struct called Creature with the following fields
// - id: character array of 20
// - hp: an integer holding the health points of the creature
// - attack_bonus: an other integer
// - armor: a field with type Armor
// - attack_type: a field with type AttackType
struct Creature
{
    char id[20];
    int hp;
    int attack_bonus;
    struct Armor armor;
    struct AttackType attack_type;
};

// TODO: implement the following function that takes a file handle and parses
// input in the format <id> <armor_class>. It should use the fscanf function for
// parsing and return an Armor. For example:
//
//   "leather 11"
//
// means a leather armor (id) with armor class 11. The id of the armor doesn't
// have whitespace characters in it.
struct Armor parse_armor(FILE *fp)
{
    struct Armor num1;
    fscanf(fp, "%s %d", num1.id, &num1.armor_class);
    return num1;
}

// TODO: implement the following function that takes a file handle and parses
// the input in <id> <dice_count> <dice_type> <bonus> format. It should use the
// fscanf function for parsing and return an AttackType. For example:
//
//   "dagger 1 D4 2"
//
// means a dagger (id) with damage defined as 1d4+2. The id of the attack type
// doesn't have whitespace characters in it.
struct AttackType parse_attack_type(FILE *fp)
{
    struct AttackType num2;
    char dice_s[50];
    fscanf(fp, "%s %d %s %d", num2.id, &num2.dice_count, dice_s, &num2.damage_bonus);
    int sides1 = 0;
    if (strcmp(dice_s, "D4") == 0)
        sides1 = 4;
    else if (strcmp(dice_s, "D6") == 0)
        sides1 = 6;
    else if (strcmp(dice_s, "D8") == 0)
        sides1 = 8;
    else if (strcmp(dice_s, "D10") == 0)
        sides1 = 10;
    else if (strcmp(dice_s, "D12") == 0)
        sides1 = 12;
    else if (strcmp(dice_s, "D100") == 0)
        sides1 = 100;

    num2.dice_type = (DiceType)sides1;
    return num2;
}

// TODO: implement the following function that takes a file handle and parses
// input in the format of <id> <hp> <attack_bonus> <armor_id> <attack_type_id>.
//
// The function should use fscanf to parse the line and return a Creature.
// For example the line
//
//   "Hero   30 6 leather    dagger"
//
// translates to a creature with id 'Hero' having 30 hp, and an attack bonus
// of 6. He is equipped with leather armor and a dagger. You can suppose that an
// armor with the id 'leather' is provided in the armors array, as well as an
// attack type with the id 'dagger'. You need to look them up by their ids; you
// can use the `strcmp` function for that. The id of the creature doesn't
// contain whitespace characters.
struct Creature parse_creature(
    FILE *fp,
    struct Armor armors[], int armor_count,
    struct AttackType attack_types[], int attack_type_count)
{
    struct Creature cr;
    char armor1_id[50];
    char attack1_id[50];

    fscanf(fp, "%s %d %d %s %s", cr.id, &cr.hp, &cr.attack_bonus, armor1_id, attack1_id);

    for (int i = 0; i < armor_count; i++)
    { // finding armor
        if (strcmp(armors[i].id, armor1_id) == 0)
        {
            cr.armor = armors[i];
            break;
        }
    }

    for (int i = 0; i < attack_type_count; i++)
    {
        if (strcmp(attack_types[i].id, attack1_id) == 0)
        { // finding attack type
            cr.attack_type = attack_types[i];
            break;
        }
    }

    return cr;
}

// TODO: using the `roll` function from dice.h implement the fight between
// two creatures as follows.
// - The fight is turn-based. c1 attacks first, then c2, and they
//   alternate until one of them is reduced to 0 hit points or below.
// - On its turn, the attacker makes an attack roll using a single D20 dice and
//   its own attack bonus. (Use the provided roll function).
// - The attack hits if the attack roll is greater than or equal to the
//   defender's armor class.
// - On a hit, damage is computed from the attacker's AttackType using the
//   dice rolling function again, but this time you 'roll' with the values
//   stored in the AttackType (dice type, count and damage_bonus).
//   The defender's hp is then reduced by this damage value.
// - If, after taking damage, a creature's hp reaches 0 the fight ends
// immediately
//   and the attacking creature is considered the winner.
// - The function returns the pointer to the winner.
struct Creature *fight(struct Creature *c1, struct Creature *c2)
{
    struct Creature *attacker;
    struct Creature *defender;

    attacker = c1;
    defender = c2;
    while (defender->hp > 0 && attacker->hp > 0)
    {

        int attack_roll = roll(D20, 1, attacker->attack_bonus);

        if (attack_roll >= defender->armor.armor_class)
        {

            int damage;
            damage = roll(
                attacker->attack_type.dice_type,
                attacker->attack_type.dice_count,
                attacker->attack_type.damage_bonus);

            defender->hp = defender->hp - damage;
        }
        if (defender->hp <= 0)
        {
            return attacker;
        }

        struct Creature *temp1; // swapping attacker and defender for next round

        temp1 = attacker;
        attacker = defender;
        defender = temp1;
    }

    return attacker;
}

// TODO: main should take a filename argument, open the file, parse
// its contents and simulate the fights. Print out the first character
// of the winner of each fight.
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        return 1;
    }

    FILE *fp;
    fp = fopen(argv[1], "r"); // reading a file

    if (fp == NULL)
    {
        return 1;
    }

    int armor_count;
    fscanf(fp, "%d", &armor_count);

    struct Armor armors[armor_count];
    for (int i = 0; i < armor_count; i++)
    {
        armors[i] = parse_armor(fp);
    }

    int attack_count;
    fscanf(fp, "%d", &attack_count);

    struct AttackType attack_types[attack_count];
    for (int i = 0; i < attack_count; i++)
    {
        attack_types[i] = parse_attack_type(fp);
    }

    int creature_count;
    fscanf(fp, "%d", &creature_count);

    struct Creature creatures[creature_count];
    for (int i = 0; i < creature_count; i++)
    {
        creatures[i] = parse_creature(fp, armors, armor_count, attack_types, attack_count);
    }

    fclose(fp);

    for (int i = 0; i < creature_count; i = i + 2)
    {
        struct Creature *winner;
        winner = fight(&creatures[i], &creatures[i + 1]);
        printf("%c", winner->id[0]);
    }

    printf("\n");

    return 0;
}
