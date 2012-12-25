#include "../pchdef.h"
#include "PlayerbotAIConfig.h"
#include "Policies/SingletonImp.h"
#include "playerbot.h"
#include "RandomPlayerbotFactory.h"
#include "../AccountMgr.h"
#include "../../shared/SystemConfig.h"

using namespace std;

INSTANTIATE_SINGLETON_1(PlayerbotAIConfig);

PlayerbotAIConfig::PlayerbotAIConfig()
{
}

template <class T>
void LoadList(string value, T &list)
{
    vector<string> ids = split(value, ',');
    for (vector<string>::iterator i = ids.begin(); i != ids.end(); i++)
    {
        uint32 id = atoi((*i).c_str());
        if (!id)
            continue;

        list.push_back(id);
    }
}

bool PlayerbotAIConfig::Initialize()
{
    sLog.outString("Initializing AI Playerbot by ike3, based on the original Playerbot by blueboy");

    if (!config.SetSource("aiplayerbot.conf"))
    {
        sLog.outString("AI Playerbot is Disabled. Unable to open configuration file aiplayerbot.conf");
        return false;
    }

    enabled = config.GetBoolDefault("AiPlayerbot.Enabled", true);
    if (!enabled)
        sLog.outString("AI Playerbot is Disabled in aiplayerbot.conf");

    globalCoolDown = (uint32) config.GetIntDefault("AiPlayerbot.GlobalCooldown", 500);
    teleportDelay = config.GetIntDefault("AiPlayerbot.TeleportDelay", 10000);
    reactDelay = (uint32) config.GetIntDefault("AiPlayerbot.ReactDelay", 100);

    sightDistance = config.GetFloatDefault("AiPlayerbot.SightDistance", 50.0f);
    spellDistance = config.GetFloatDefault("AiPlayerbot.SpellDistance", 25.0f);
    reactDistance = config.GetFloatDefault("AiPlayerbot.ReactDistance", 100.0f);
    grindDistance = config.GetFloatDefault("AiPlayerbot.GrindDistance", 75.0f);
    lootDistance = config.GetFloatDefault("AiPlayerbot.LootDistance", 20.0f);
    fleeDistance = config.GetFloatDefault("AiPlayerbot.FleeDistance", 15.0f);
    tooCloseDistance = config.GetFloatDefault("AiPlayerbot.TooCloseDistance", 7.0f);
    meleeDistance = config.GetFloatDefault("AiPlayerbot.MeleeDistance", 2.0f);
    followDistance = config.GetFloatDefault("AiPlayerbot.FollowDistance", 2.0f);

    criticalHealth = config.GetIntDefault("AiPlayerbot.CriticalHealth", 25);
    lowHealth = config.GetIntDefault("AiPlayerbot.LowHealth", 45);
    mediumHealth = config.GetIntDefault("AiPlayerbot.MediumHealth", 65);
    almostFullHealth = config.GetIntDefault("AiPlayerbot.AlmostFullHealth", 85);
    lowMana = config.GetIntDefault("AiPlayerbot.LowMana", 15);
    mediumMana = config.GetIntDefault("AiPlayerbot.MediumMana", 40);

    randomGearLoweringChance = config.GetFloatDefault("AiPlayerbot.RandomGearLoweringChance", 0.15);

    iterationsPerTick = config.GetIntDefault("AiPlayerbot.IterationsPerTick", 10);

    allowGuildBots = config.GetBoolDefault("AiPlayerbot.AllowGuildBots", true);

    randomBotMapsAsString = config.GetStringDefault("AiPlayerbot.RandomBotMaps", "0,1,530,571");
    LoadList<vector<uint32> >(randomBotMapsAsString, randomBotMaps);
    LoadList<list<uint32> >(config.GetStringDefault("AiPlayerbot.RandomBotQuestItems", "6948,5175,5176,5177,5178"), randomBotQuestItems);
    LoadList<list<uint32> >(config.GetStringDefault("AiPlayerbot.RandomBotSpellIds", "54197"), randomBotSpellIds);

    randomBotAutologin = config.GetBoolDefault("AiPlayerbot.RandomBotAutologin", true);
    randomBotGrinding = config.GetBoolDefault("AiPlayerbot.RandomBotGrinding", true);
    minRandomBots = config.GetIntDefault("AiPlayerbot.MinRandomBots", 1);
    maxRandomBots = config.GetIntDefault("AiPlayerbot.MaxRandomBots", 10);
    randomBotUpdateInterval = config.GetIntDefault("AiPlayerbot.RandomBotUpdateInterval", 60);
    randomBotCountChangeInterval = config.GetIntDefault("AiPlayerbot.RandomBotCountChangeInterval", 5 * 24 * 3600);
    minRandomBotInWorldTime = config.GetIntDefault("AiPlayerbot.MinRandomBotInWorldTime", 2 * 3600);
    maxRandomBotInWorldTime = config.GetIntDefault("AiPlayerbot.MaxRandomBotInWorldTime", 14 * 24 * 3600);
    minRandomBotRandomizeTime = config.GetIntDefault("AiPlayerbot.MinRandomBotRandomizeTime", 2 * 3600);
    maxRandomRandomizeTime = config.GetIntDefault("AiPlayerbot.MaxRandomRandomizeTime", 14 * 24 * 3600);
    minRandomBotPvpTime = config.GetIntDefault("AiPlayerbot.MinRandomBotPvpTime", 120);
    maxRandomBotPvpTime = config.GetIntDefault("AiPlayerbot.MaxRandomBotPvpTime", 3 * 3600);
    randomBotTeleportDistance = config.GetIntDefault("AiPlayerbot.RandomBotTeleportDistance", 400);
    minRandomBotsPerInterval = config.GetIntDefault("AiPlayerbot.MinRandomBotsPerInterval", 1);
    maxRandomBotsPerInterval = config.GetIntDefault("AiPlayerbot.MaxRandomBotsPerInterval", 5);
    randomBotEnemyPercent = config.GetIntDefault("AiPlayerbot.RandomBotEnemyPercent", 30);

    CreateRandomBots();
    sLog.outString("AI Playerbot configuration loaded");

    return true;
}


bool PlayerbotAIConfig::IsInRandomAccountList(uint32 id)
{
    return find(randomBotAccounts.begin(), randomBotAccounts.end(), id) != randomBotAccounts.end();
}

bool PlayerbotAIConfig::IsInRandomQuestItemList(uint32 id)
{
    return find(randomBotQuestItems.begin(), randomBotQuestItems.end(), id) != randomBotQuestItems.end();
}

string PlayerbotAIConfig::GetValue(string name)
{
    ostringstream out;

    if (name == "GlobalCooldown")
        out << globalCoolDown;
    else if (name == "TeleportDelay")
        out << teleportDelay;
    else if (name == "ReactDelay")
        out << reactDelay;

    else if (name == "SightDistance")
        out << sightDistance;
    else if (name == "SpellDistance")
        out << spellDistance;
    else if (name == "ReactDistance")
        out << reactDistance;
    else if (name == "GrindDistance")
        out << grindDistance;
    else if (name == "LootDistance")
        out << lootDistance;
    else if (name == "FleeDistance")
        out << fleeDistance;

    else if (name == "CriticalHealth")
        out << criticalHealth;
    else if (name == "LowHealth")
        out << lowHealth;
    else if (name == "MediumHealth")
        out << mediumHealth;
    else if (name == "AlmostFullHealth")
        out << almostFullHealth;
    else if (name == "LowMana")
        out << lowMana;

    else if (name == "IterationsPerTick")
        out << iterationsPerTick;

    return out.str();
}

void PlayerbotAIConfig::SetValue(string name, string value)
{
    istringstream out(value, istringstream::in);

    if (name == "GlobalCooldown")
        out >> globalCoolDown;
    else if (name == "TeleportDelay")
        out >> teleportDelay;
    else if (name == "ReactDelay")
        out >> reactDelay;

    else if (name == "SightDistance")
        out >> sightDistance;
    else if (name == "SpellDistance")
        out >> spellDistance;
    else if (name == "ReactDistance")
        out >> reactDistance;
    else if (name == "GrindDistance")
        out >> grindDistance;
    else if (name == "LootDistance")
        out >> lootDistance;
    else if (name == "FleeDistance")
        out >> fleeDistance;

    else if (name == "CriticalHealth")
        out >> criticalHealth;
    else if (name == "LowHealth")
        out >> lowHealth;
    else if (name == "MediumHealth")
        out >> mediumHealth;
    else if (name == "AlmostFullHealth")
        out >> almostFullHealth;
    else if (name == "LowMana")
        out >> lowMana;

    else if (name == "IterationsPerTick")
        out >> iterationsPerTick;
}


void PlayerbotAIConfig::CreateRandomBots()
{
    string randomBotAccountPrefix = config.GetStringDefault("AiPlayerbot.RandomBotAccountPrefix", "rndbot");
    uint32 randomBotAccountCount = config.GetIntDefault("AiPlayerbot.RandomBotAccountCount", 50);

    for (int accountNumber = 0; accountNumber < randomBotAccountCount; ++accountNumber)
    {
        ostringstream out; out << randomBotAccountPrefix << accountNumber;
        string accountName = out.str();
        QueryResult *results = LoginDatabase.PQuery("SELECT id FROM account where username = '%s'", accountName.c_str());
        if (results)
        {
            delete results;
            continue;
        }

        string password = "";
        for (int i = 0; i < 10; i++)
        {
            password += (char)urand('!', 'z');
        }
        sAccountMgr.CreateAccount(accountName, password);

        sLog.outDetail("Account %s created for random bots", accountName.c_str());
    }

    LoginDatabase.PExecute("UPDATE account SET expansion = '%u'", MAX_EXPANSION);

    int totalRandomBotChars = 0;
    for (int accountNumber = 0; accountNumber < randomBotAccountCount; ++accountNumber)
    {
        ostringstream out; out << randomBotAccountPrefix << accountNumber;
        string accountName = out.str();

        QueryResult *results = LoginDatabase.PQuery("SELECT id FROM account where username = '%s'", accountName.c_str());
        if (!results)
            continue;

        Field* fields = results->Fetch();
        uint32 accountId = fields[0].GetUInt32();
        delete results;

        randomBotAccounts.push_back(accountId);

        int count = sAccountMgr.GetCharactersCount(accountId);
        if (count >= 10)
        {
            totalRandomBotChars += count;
            continue;
        }

        RandomPlayerbotFactory factory(accountId);
        while (count++ < 10)
        {
            factory.CreateRandomBot();
        }

        totalRandomBotChars += sAccountMgr.GetCharactersCount(accountId);
    }

    sLog.outBasic("%d random bot accounts with %d characters available", randomBotAccounts.size(), totalRandomBotChars);
}
