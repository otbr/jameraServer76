
        worldId = 0
        gamePort = 7171


--- DIRECTORY PATH ---

    DataDir = "data/"

--- BANS SYSTEM ---

    NotationsToBan = 3
    WarningsToFinalBan = 4
    WarningsToDeletion = 5
    BanLength = 3 * 24 * 60 * 60
    FinalBanLength = 7 * 24 * 60 * 60
    IPBanishmentLength = 24 * 60 * 60
    KillsToBan = 7

--- COMBAT SYSTEM ---

    -- world type
    -- options: pvp, no-pvp, pvp-enforced
    WorldType = "pvp"

    -- exhausted time in ms (1000 = 1 second) for yelling
    Exhausted = 1000

    -- exhausted time in ms (1000 = 1 second) for aggressive spells/weapons
    FightExhausted = 1800

    -- exhausted time in ms (1000 = 1 second) for none-aggressive spells/weapons
    HealExhausted = 1500

    -- how many ms to add if the player is already exhausted and tries to yell (1000 = 1 second)
    ExhaustedAdd = 200

    -- how long does the player has to stay out of fight to get pz unlocked in ms (1000 = 1 second)
    PZLock = 60000

    -- how long a field belongs to a player before it no longer causes PZ lock for the owner
    FieldOwnershipDuration = 5000

    -- in mili seconds
    TimeToDecreaseFrags = 8 * 60 * 60 * 1000

    -- Time white skull will remain after killing a player, in minutes
    WhiteSkullTime = 10

    -- amount of kills that leads to red skull
    KillsToRedSkull = 3

    -- Remove ammunition
    -- If false, ammunition will not be removed when using distance weapons
    -- (or other weapons that use ammunition)
    RemoveAmmunition = true
	
    -- Remove rune charges
    -- This only applies to runes done using the default functions. 
    -- Custom runes made using actions will not be affected.
    RemoveRuneCharges = true

    -- Remove weapon charges
    -- Set to false to disable charges disappearing from weapons on use
    RemoveWeaponCharges = true

---- CONNECTION SYSTEM ----

    -- server ip (the ip that server listens on)
    IP = "149.56.108.250"

    -- server port (the port that server listens on)
    Port = "7171"

    -- how many logins attempts until ip is temporary disabled 
    -- set to 0 to disable
    LoginTries = 5

    -- how long the retry timeout until a new login can be made (without disabling the ip)
    RetryTimeout = 5000

    -- how long the player need to wait until the ip is allowed again
    LoginTimeout = 60 * 1000

    -- allow clones (multiple logins of the same char)
    AllowClones = false

    -- only one player online per account
    CheckAccounts = false

---- DATABASE ----

    -- SQL type
    -- options: mysql, sqlite, odbc or pgsql
    SQL_Type = "mysql"

    --- SQL connection part
    SQL_DB   = "ot_teste3"
    SQLDatabase = "ot_teste3"
    encryptionType = "plain"

    -- these settings are not used by SQLite
    SQL_Host = "jamera.com.br"
    SQL_Port = 3306
    SQL_User = "server"
    SQL_Pass = "159TibOt@"
	


---- HOUSES SYSTEM ----

    -- house rent period
    -- options: daily, weekly, monthly
    HouseRentPeriod = "weekly"

    -- House price
    HousePrice = 5000


--- INFO SYSTEM ---

    -- login message
    LoginMsg = "Welcome to Jamera OldSchool Server!"

    -- server name
    ServerName = "Jamera"
    serverName = "Jamera"

    -- world name
    WorldName = "Tibia World"

    -- server owner name
    OwnerName = "John/Bruno Of God"

    -- server owner email
    OwnerEmail = "contato@jamera.com.br"

    -- server url
    URL = "http://www.jamera.com.br/"

    -- server location
    Location = "Brazil"


---- ITEM USAGE SYSTEM ----

    -- minimum amount of time between actions ('Use') (1000 = 1 second)
    MinActionInterval = 200

    -- minimum amount of time between extended actions ('Use with...') (1000 = 1 second)
    MinActionExInterval = 200

---- MAP SYSTEM ----

    -- map location
	Map = "data/world/world.otbm"

    -- mapkind
    -- options: OTBM for binary map, XML for OTX map
    MapKind = "OTBM"

    -- Type of map storage, 
    -- 'relational' - Slower, but possible to run database queries to change all items to another id for example.
    -- 'binary' - Faster, but you cannot run DB queries.
    -- To switch, load server with the current type, change the type in config.lua 
    -- type /reload config and the save the server with /closeserver serversave
    MapStoreType = "binary"

---- RATES SYSTEM ----

    -- rates (experience, skill, magic level, loot and spawn)
    RateExp = 2
    RateSkill = 4
    RateMag = 5
    RateLoot = 1
    RateSpawn = 1


--- SPAWNS SYSTEM ---

    -- despawn configs
    -- how many floors can a monster go from his spawn before despawning
    DespawnRange = 2

    -- how many square metters can a monster be far from his spawn before despawning
    DespawnRadius = 50

--- STATUS SYSTEM ---

    -- motd (the message box that you sometimes get before you choose characters)
    MOTD = "Welcome to Jamera OldSchool Server.\n\nHave fun!"
    MOTD_Num = "1"

    -- max number of players allowed
    MaxPlayers = "600"

--- OTHERS ---

    -- accounts password type
    -- options: plain, md5, sha1
    PasswordType = "plain"

    -- max number of messages a player can say before getting muted (default 4), set to 0 to disable muting
    MaxMessageBuffer = 0

    -- save client debug assertion reports
    SaveClientDebug = true

    -- Should the server use account balance system or depot system for paying houses?
    UseAccBalance = true

    -- whether or not to check for premium accounts when using beds
    PremOnlyBeds = true

    -- Time after player will be kicked when idle, in minutes
    MaxIdleTime = 15

    -- Level on which player will get rooked
    LevelToRook = 5

    -- TownId to which player will be teleported
    RookTempleId = 11

    -- if your website is not showing player deaths, then keep this as 0
    StorePlayerDeaths = true

    -- ID of temple to which player will get teleported when his prem end out
    -- 0 to disable
    -- not tested yet
    FACCTempleID = 3
	
--[[ :::::::::::::::::::::::::::::::::
	 :: Configs added by ::
	 :: Nottinghster ::
	 ::::::::::::::::::::::::::::::::: ]]--
	
	-- Change sex
	changeSexPrice = 10000
	changeSexOnlyPremium = true
	
	-- How many seconds the player can push others
	PushDelayInterval = 500
	
	-- Use spell talking in orange?
	spellTalkInOrange = false
	
	-- Server Priority - options: "aboveNormal", "normal", "high" and "realtime"
	serverPriority = "high"
	
	-- Free premium for all players?
	freePremium = false
	
	-- Use Modified Exp Formula:
	-- The formula was modified, meaning that, with the original formula, you would need 4200 experience points to reach the level 8.
	-- With Tibianic Exp Formula, you will only need 238 experience points to reach the level 8!
	-- With MasterCores Exp Formula, you will only need 250 experience points to reach the level 8!
	-- Options: "original", "tibianic" and "mastercores".
	ExpFormulaType = "original"
	
	--- REAL TIBIA SAVE SYSTEM ---	
	
	-- Is it enabled?
	enableServerSave = true
	-- What hour will it happen?
	serverSaveHour = 16
	-- Will it just close the server or shutdown it? options: "closeserver" and "shutdown"
	serverSaveMode = "closeserver"
	-- Time to start warning players before the save (in minutes)
	saveWarnBefore = 20
	-- Clean server while saving?
	cleanServerAtSave = true
	-- Clean pz at save?
	cleanPzAtSave = true
	-- Clean corpses at save?
	cleanCorpsesAtSave = true
	-- Simple save frequence (in minutes)
	simpleSaveFrequence = 5
	-- Warn players about saving? (good for big servers)
	saveWarning = false
	-- save players? WARNING: YOU SHOULD JUST SET NO IF IT'S AN ENFORCED OTSERV
	savePlayers = true
	-- Database backup at save? (configurable at file backup.bat)
	DatabaseBackupAtSave = true
	
	--- CLEAN MAP SYSTEM ---	
	
	-- Auto clean frequence (in minutes)
	autoCleanFrequence = false
	-- Time to start warning players before the server clean (in minutes)
	cleanWarnBefore = 10
	-- Clean corpses/fields?
	cleanCorpses = true
	-- Clean pz area?
	cleanPz = true
