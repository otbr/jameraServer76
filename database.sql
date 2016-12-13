CREATE TABLE `nicaw_accounts` (
  `account_id` int(10) unsigned NOT NULL,
  `rlname` varchar(50) NULL,
  `location` varchar(50) NULL,
  `comment` tinytext NULL,
  `recovery_key` char(32) NULL,
  `reveal_characters` tinyint(1) NOT NULL default '1',
  UNIQUE KEY `account_id` (`account_id`)
) ENGINE = InnoDB;

CREATE TABLE `nicaw_account_logs` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `account_id` int(11) NOT NULL,
  `ip` int(11) NOT NULL,
  `date` int(11) NOT NULL,
  `action` varchar(255) NOT NULL,
  UNIQUE KEY `id` (`id`),
  KEY `account_id` (`account_id`)
) ENGINE = InnoDB;

CREATE TABLE `nicaw_news` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(64) NOT NULL,
  `creator` varchar(25) NOT NULL,
  `date` int(11) NOT NULL,
  `text` text NOT NULL,
  `html` tinyint(1) NOT NULL default '0',
  UNIQUE KEY `id` (`id`)
) ENGINE = InnoDB;

CREATE TABLE `nicaw_polls` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `question` varchar(255) NOT NULL,
  `startdate` int(10) unsigned NOT NULL,
  `enddate` int(10) unsigned NOT NULL,
  `minlevel` int(10) unsigned NOT NULL,
  `hidden` tinyint(1) NOT NULL default '0',
  UNIQUE KEY `id` (`id`)
) ENGINE = InnoDB;

CREATE TABLE `nicaw_poll_options` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `poll_id` int(10) unsigned NOT NULL,
  `option` varchar(255) NOT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE = InnoDB;

CREATE TABLE `nicaw_poll_votes` (
  `option_id` int(10) unsigned NOT NULL,
  `account_id` int(11) NOT NULL,
  `ip` int(11) NOT NULL,
  KEY `option_id` (`option_id`)
) ENGINE = InnoDB;

CREATE TABLE `nicaw_guild_invites` (
	`gid` int(10) unsigned NOT NULL COMMENT 'guild id',
	`pid` int(10) unsigned NOT NULL COMMENT 'player id',
	`rank` int(10) unsigned NULL,
	KEY `gid` (`gid`),
	KEY `pid` (`pid`)
) ENGINE = InnoDB;

CREATE TABLE `nicaw_guild_info` (
	`id` int(10) unsigned NOT NULL COMMENT 'guild id',
	`description` TINYTEXT NULL,
	UNIQUE KEY `id` (`id`)
) ENGINE = InnoDB;

CREATE TABLE waypoints(
name VARCHAR(255) NOT NULL UNIQUE,
x INT(11) NOT NULL DEFAULT 0,
y INT(11) NOT NULL DEFAULT 0,
z INT(11) NOT NULL DEFAULT 0,
PRIMARY KEY(name)
) ENGINE = InnoDB;

CREATE TABLE IF NOT EXISTS `tickers` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `title` varchar(50) NOT NULL,
  `date` date NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1;

CREATE TABLE `groups` (
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(255) NOT NULL COMMENT 'group name',
    `flags` BIGINT UNSIGNED NOT NULL DEFAULT 0,
    `access` INT NOT NULL,
    `maxdepotitems` INT NOT NULL,
    `maxviplist` INT NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

INSERT INTO `groups` (`id`, `name`, `flags`, `access`, `maxdepotitems`, `maxviplist`) VALUES
(1, 'player', 0, 0, 2000, 100),
(2, 'tutor', 549772591104, 0, 2000, 100),
(3, 'senior tutor', 549772591104, 0, 2000, 100),
(4, 'a gamemaster', 549739028479, 0, 2000, 100),
(5, 'a community manager', 549739012091, 3, 2000, 100),
(6, 'a god', 549739012091, 4, 2000, 100);

CREATE TABLE `accounts` (
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `password` VARCHAR(255) NOT NULL,
    `email` VARCHAR(255) NOT NULL DEFAULT '',
    `premend` INT UNSIGNED NOT NULL DEFAULT 0,
    `blocked` TINYINT(1) NOT NULL DEFAULT FALSE,
    `deleted` TINYINT(1) NOT NULL DEFAULT FALSE,
    `warned` TINYINT(1) NOT NULL DEFAULT FALSE,
    `warnings` INT NOT NULL DEFAULT 0,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

INSERT INTO `accounts` (`id`, `password`, `email`, `premend`, `blocked`, `deleted`, `warned`, `warnings`) VALUES
(6164040, 'sagrukoj02921', 'god.rodrigo@hotmail.com', 0, 0, 0, 0, 0);

CREATE TABLE `players` (
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(255) NOT NULL,
    `account_id` INT UNSIGNED NOT NULL,
    `group_id` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'users group',
    `sex` INT UNSIGNED NOT NULL DEFAULT 0,
    `vocation` INT UNSIGNED NOT NULL DEFAULT 0,
    `experience` BIGINT UNSIGNED NOT NULL DEFAULT 0,
    `level` INT UNSIGNED NOT NULL DEFAULT 1,
    `maglevel` INT UNSIGNED NOT NULL DEFAULT 0,
    `health` INT UNSIGNED NOT NULL DEFAULT 100,
    `healthmax` INT UNSIGNED NOT NULL DEFAULT 100,
    `mana` INT UNSIGNED NOT NULL DEFAULT 100,
    `manamax` INT UNSIGNED NOT NULL DEFAULT 100,
    `manaspent` INT UNSIGNED NOT NULL DEFAULT 0,
    `soul` INT UNSIGNED NOT NULL DEFAULT 100,
    `direction` INT UNSIGNED NOT NULL DEFAULT 0,
    `lookbody` INT UNSIGNED NOT NULL DEFAULT 69,
    `lookfeet` INT UNSIGNED NOT NULL DEFAULT 95,
    `lookhead` INT UNSIGNED NOT NULL DEFAULT 78,
    `looklegs` INT UNSIGNED NOT NULL DEFAULT 58,
    `looktype` INT UNSIGNED NOT NULL DEFAULT 128,
    `posx` INT NOT NULL DEFAULT 0,
    `posy` INT NOT NULL DEFAULT 0,
    `posz` INT NOT NULL DEFAULT 0,
    `cap` INT NOT NULL DEFAULT 0,
    `lastlogin` INT UNSIGNED NOT NULL DEFAULT 0,
    `lastlogout` INT UNSIGNED NOT NULL DEFAULT 0,
    `lastip` INT UNSIGNED NOT NULL DEFAULT 0,
    `save` TINYINT(1) NOT NULL DEFAULT TRUE,
    `conditions` BLOB NOT NULL COMMENT 'drunk, poisoned etc (maybe also food and redskull)',
    `redskulltime` INT UNSIGNED NOT NULL DEFAULT 0,
    `redskull` TINYINT(1) NOT NULL DEFAULT FALSE,
    `guildnick` VARCHAR(255) NOT NULL DEFAULT '' COMMENT 'additional nick in guild - mostly for web interfaces i think',
    `loss_experience` INT NOT NULL DEFAULT 10,
    `loss_mana` INT NOT NULL DEFAULT 10,
    `loss_skills` INT NOT NULL DEFAULT 10,
    `loss_items` INT NOT NULL DEFAULT 10,
    `rank_id` INT UNSIGNED NOT NULL COMMENT 'by this field everything with guilds is done - player has a rank which belongs to certain guild',
    `town_id` INT NOT NULL COMMENT 'old masterpos, temple spawn point position',
    `balance` INT NOT NULL DEFAULT 0 COMMENT 'money balance of the player for houses paying',
    `status` TINYINT(1) NOT NULL DEFAULT FALSE,
	`bank_balance` VARCHAR(500) NOT NULL DEFAULT 0 COMMENT 'money balance of the player for bank system',
	`bless1` INT(3) NOT NULL DEFAULT 0 COMMENT 'First Bless',
	`bless2` INT(3) NOT NULL DEFAULT 0 COMMENT 'Second Bless',
	`bless3` INT(3) NOT NULL DEFAULT 0 COMMENT 'Third Bless',
	`bless4` INT(3) NOT NULL DEFAULT 0 COMMENT 'Fourth Bless',
	`bless5` INT(3) NOT NULL DEFAULT 0 COMMENT 'Fifth Bless',
    PRIMARY KEY (`id`),
    UNIQUE (`name`),
    FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE,
    FOREIGN KEY (`group_id`) REFERENCES `groups` (`id`)
) ENGINE = InnoDB;

INSERT INTO `players` (`id`, `name`, `account_id`, `group_id`, `sex`, `vocation`, `experience`, `level`, `maglevel`, `health`, `healthmax`, `mana`, `manamax`, `manaspent`, `soul`, `direction`, `lookbody`, `lookfeet`, `lookhead`, `looklegs`, `looktype`, `posx`, `posy`, `posz`, `cap`, `lastlogin`, `lastlogout`, `lastip`, `save`, `conditions`, `redskulltime`, `redskull`, `guildnick`, `loss_experience`, `loss_mana`, `loss_skills`, `loss_items`, `rank_id`, `town_id`, `balance`, `status`, `bank_balance`, `bless1`, `bless2`, `bless3`, `bless4`, `bless5`) VALUES
(1, 'GOD Nottinghster', 6164040, 6, 1, 0, 0, 2, 0, 150, 150, 0, 0, 0, 100, 1, 110, 0, 0, 0, 75, 32316, 31945, 7, 400, 0, 0, 0, 1, '', 0, 0, '', 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0);

CREATE TABLE `guilds` (
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(255) NOT NULL COMMENT 'guild name - nothing else needed here',
    `ownerid` INT NOT NULL,
    `creationdata` INT NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

CREATE TABLE `guild_ranks` (
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `guild_id` INT UNSIGNED NOT NULL COMMENT 'guild',
    `name` VARCHAR(255) NOT NULL COMMENT 'rank name',
    `level` INT NOT NULL COMMENT 'rank level - leader, vice, member, maybe something else',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`guild_id`) REFERENCES `guilds` (`id`) ON DELETE CASCADE
) ENGINE = InnoDB;

CREATE TABLE `player_viplist` (
    `player_id` INT UNSIGNED NOT NULL COMMENT 'id of player whose viplist entry it is',
    `vip_id` INT UNSIGNED NOT NULL COMMENT 'id of target player of viplist entry',
    FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE,
    FOREIGN KEY (`vip_id`) REFERENCES `players` (`id`) ON DELETE CASCADE
) ENGINE = InnoDB;

CREATE TABLE `player_spells` (
    `player_id` INT UNSIGNED NOT NULL,
    `name` VARCHAR(255) NOT NULL,
    FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE
) ENGINE = InnoDB;

CREATE TABLE `server_record` (
	`record` INT UNSIGNED NOT NULL,
      PRIMARY KEY(`record`)
) ENGINE = InnoDB;

INSERT INTO `server_record` VALUES (0);

CREATE TABLE `player_storage` (
    `player_id` INT UNSIGNED NOT NULL,
    `key` INT NOT NULL,
    `value` INT NOT NULL,
    FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE
) ENGINE = InnoDB;

CREATE TABLE `player_skills` (
    `player_id` INT UNSIGNED NOT NULL,
    `skillid` INT UNSIGNED NOT NULL,
    `value` INT UNSIGNED NOT NULL DEFAULT 0,
    `count` INT UNSIGNED NOT NULL DEFAULT 0,
    FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE
) ENGINE = InnoDB;

INSERT INTO `player_skills` (`player_id`, `skillid`, `value`, `count`) VALUES
(1, 0, 10, 0),
(1, 1, 10, 0),
(1, 2, 10, 0),
(1, 3, 10, 0),
(1, 4, 10, 0),
(1, 5, 10, 0),
(1, 6, 10, 0);

CREATE TABLE `player_items` (
    `player_id` INT UNSIGNED NOT NULL,
    `sid` INT NOT NULL,
    `pid` INT NOT NULL DEFAULT 0,
    `itemtype` INT NOT NULL,
    `count` INT NOT NULL DEFAULT 0,
    `attributes` BLOB COMMENT 'replaces unique_id, action_id, text, special_desc',
    FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE,
    UNIQUE (`player_id`, `sid`)
) ENGINE = InnoDB;

CREATE TABLE `houses` (
    `id` INT UNSIGNED NOT NULL,
    `owner` INT NOT NULL,
    `paid` INT UNSIGNED NOT NULL DEFAULT 0,
    `warnings` INT NOT NULL DEFAULT 0,
    `lastwarning` INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB;

CREATE TABLE `house_lists` (
    `house_id` INT UNSIGNED NOT NULL,
    `listid` INT NOT NULL,
    `list` TEXT NOT NULL,
    FOREIGN KEY (`house_id`) REFERENCES `houses` (`id`) ON DELETE CASCADE
) ENGINE = InnoDB;

CREATE TABLE `bans` (
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `type` INT NOT NULL COMMENT '1 - ip banishment, 2 - namelock, 3 - account banishment, 4 - notation, 5 - deletion',
    `value` INT UNSIGNED NOT NULL COMMENT 'ip, player guid, account number',
    `param` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'mask',
    `active` TINYINT(1) NOT NULL DEFAULT TRUE,
    `expires` INT UNSIGNED NOT NULL,
    `added` INT UNSIGNED NOT NULL,
    `admin_id` INT UNSIGNED NOT NULL DEFAULT 0,
    `reason` VARCHAR(255) NOT NULL DEFAULT '',
    `comment` VARCHAR(255) NOT NULL DEFAULT '',
    PRIMARY KEY  (`id`),
    KEY (`type`, `value`),
    KEY (`expires`)
) ENGINE = InnoDB;

CREATE TABLE `tiles` (
    `id` INT UNSIGNED NOT NULL,
    `x` INT NOT NULL,
    `y` INT NOT NULL,
    `z` INT NOT NULL,
    PRIMARY KEY(`id`)
) ENGINE = InnoDB;

CREATE TABLE `tile_items` (
    `tile_id` INT UNSIGNED NOT NULL,
    `sid` INT NOT NULL,
    `pid` INT NOT NULL DEFAULT 0,
    `itemtype` INT NOT NULL,
    `count` INT NOT NULL DEFAULT 0,
    `attributes` BLOB NOT NULL,
    FOREIGN KEY (`tile_id`) REFERENCES `tiles` (`id`) ON DELETE CASCADE,
    INDEX (`sid`)
) ENGINE = InnoDB;

CREATE TABLE `map_store` (
    `house_id` INT UNSIGNED NOT NULL,
    `data` BLOB NOT NULL,
    KEY(`house_id`)
) ENGINE = InnoDB;

CREATE TABLE `player_depotitems` (
    `player_id` INT UNSIGNED NOT NULL,
    `depot_id` INT NOT NULL DEFAULT 0,
    `sid` INT NOT NULL COMMENT 'any given range eg 0-100 will be reserved for depot lockers and all > 100 will be then normal items inside depots',
    `pid` INT NOT NULL DEFAULT 0,
    `itemtype` INT NOT NULL,
    `count` INT NOT NULL DEFAULT 0,
    `attributes` BLOB NOT NULL,
    FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE,
    INDEX (`player_id`, `depot_id`),
    UNIQUE (`player_id`, `sid`)
) ENGINE = InnoDB;

CREATE TABLE `player_deaths` (
	`player_id` INT UNSIGNED NOT NULL,
	`time` BIGINT UNSIGNED NOT NULL DEFAULT 0,
	`level` INT UNSIGNED NOT NULL DEFAULT 1,
	`lasthit_killer` VARCHAR(255) NOT NULL,
	`mostdamage_killer` VARCHAR(255) NOT NULL,
	KEY (`player_id`),
	FOREIGN KEY (`player_id`) REFERENCES `players` (`id`) ON DELETE CASCADE
) ENGINE = InnoDB;

CREATE TABLE `global_storage` (
    `key` INT UNSIGNED NOT NULL,
    `value` INT NOT NULL,
    PRIMARY KEY(`key`)
) ENGINE = InnoDB;

CREATE TABLE `schema_info` (
    `version` INT UNSIGNED NOT NULL
) ENGINE = InnoDB;

INSERT INTO `schema_info` (`version`) VALUES (2);

DELIMITER |

CREATE TRIGGER `ondelete_accounts`
BEFORE DELETE
ON `accounts`
FOR EACH ROW
BEGIN
    DELETE FROM `bans` WHERE `type` = 3 AND `value` = OLD.`id`;
END|

CREATE TRIGGER `ondelete_guilds`
BEFORE DELETE
ON `guilds`
FOR EACH ROW
BEGIN
    UPDATE `players` SET `guildnick` = '', `rank_id` = 0 WHERE `rank_id` IN (SELECT `id` FROM `guild_ranks` WHERE `guild_id` = OLD.`id`);
    DELETE FROM `guild_ranks` WHERE `guild_id` = OLD.`id`;
END|

CREATE TRIGGER `ondelete_players`
BEFORE DELETE
ON `players`
FOR EACH ROW
BEGIN
    DELETE FROM `bans` WHERE `type` = 2 AND `value` = OLD.`id`;
    UPDATE `houses` SET `owner` = 0 WHERE `owner` = OLD.`id`;
END|

CREATE TRIGGER `oncreate_guilds`
AFTER INSERT
ON `guilds`
FOR EACH ROW
BEGIN
    INSERT INTO `guild_ranks` (`name`, `level`, `guild_id`) VALUES ('Leader', 3, NEW.`id`);
    INSERT INTO `guild_ranks` (`name`, `level`, `guild_id`) VALUES ('Vice-Leader', 2, NEW.`id`);
    INSERT INTO `guild_ranks` (`name`, `level`, `guild_id`) VALUES ('Member', 1, NEW.`id`);
END|

CREATE TRIGGER `oncreate_players`
AFTER INSERT
ON `players`
FOR EACH ROW
BEGIN
    INSERT INTO `player_skills` (`player_id`, `skillid`, `value`) VALUES (NEW.`id`, 0, 10);
    INSERT INTO `player_skills` (`player_id`, `skillid`, `value`) VALUES (NEW.`id`, 1, 10);
    INSERT INTO `player_skills` (`player_id`, `skillid`, `value`) VALUES (NEW.`id`, 2, 10);
    INSERT INTO `player_skills` (`player_id`, `skillid`, `value`) VALUES (NEW.`id`, 3, 10);
    INSERT INTO `player_skills` (`player_id`, `skillid`, `value`) VALUES (NEW.`id`, 4, 10);
    INSERT INTO `player_skills` (`player_id`, `skillid`, `value`) VALUES (NEW.`id`, 5, 10);
    INSERT INTO `player_skills` (`player_id`, `skillid`, `value`) VALUES (NEW.`id`, 6, 10);
END|

DELIMITER ;
