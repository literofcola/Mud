-- MySQL dump 10.13  Distrib 5.7.20, for Win64 (x86_64)
--
-- Host: localhost    Database: mud
-- ------------------------------------------------------
-- Server version	5.7.20

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `areas`
--

DROP TABLE IF EXISTS `areas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `areas` (
  `id` int(11) NOT NULL,
  `name` varchar(45) DEFAULT NULL,
  `level_range_low` int(11) DEFAULT NULL,
  `level_range_high` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `areas`
--

LOCK TABLES `areas` WRITE;
/*!40000 ALTER TABLE `areas` DISABLE KEYS */;
INSERT INTO `areas` VALUES (1,'Northshire',-842150451,-842150451);
/*!40000 ALTER TABLE `areas` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `class_skills`
--

DROP TABLE IF EXISTS `class_skills`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `class_skills` (
  `class` int(11) NOT NULL,
  `skill` int(11) NOT NULL,
  `level` int(11) DEFAULT NULL,
  PRIMARY KEY (`skill`,`class`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `class_skills`
--

LOCK TABLES `class_skills` WRITE;
/*!40000 ALTER TABLE `class_skills` DISABLE KEYS */;
/*!40000 ALTER TABLE `class_skills` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `classes`
--

DROP TABLE IF EXISTS `classes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `classes` (
  `id` int(11) NOT NULL,
  `name` varchar(12) DEFAULT NULL,
  `color` varchar(5) DEFAULT NULL,
  `agility_per_level` int(11) DEFAULT NULL,
  `intellect_per_level` int(11) DEFAULT NULL,
  `strength_per_level` int(11) DEFAULT NULL,
  `vitality_per_level` int(11) DEFAULT NULL,
  `wisdom_per_level` int(11) DEFAULT NULL,
  `items` tinytext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `classes`
--

LOCK TABLES `classes` WRITE;
/*!40000 ALTER TABLE `classes` DISABLE KEYS */;
INSERT INTO `classes` VALUES (1,'Warrior','|M',1,0,2,3,0,'3;4;5;6;'),(2,'Rogue','|Y',3,0,1,2,0,''),(3,'Mage','|C',0,4,0,1,1,''),(4,'Cleric','|W',0,2,0,2,2,'');
/*!40000 ALTER TABLE `classes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `exits`
--

DROP TABLE IF EXISTS `exits`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `exits` (
  `from` int(11) NOT NULL,
  `direction` int(11) NOT NULL,
  `to` int(11) DEFAULT NULL,
  PRIMARY KEY (`from`,`direction`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `exits`
--

LOCK TABLES `exits` WRITE;
/*!40000 ALTER TABLE `exits` DISABLE KEYS */;
INSERT INTO `exits` VALUES (2,0,3),(2,6,4),(3,4,2),(4,2,2),(4,6,5),(5,0,6),(5,2,4),(5,6,7),(6,4,5),(7,2,5);
/*!40000 ALTER TABLE `exits` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `help`
--

DROP TABLE IF EXISTS `help`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `help` (
  `id` int(11) NOT NULL,
  `title` varchar(45) DEFAULT NULL,
  `search_string` tinytext,
  `text` mediumtext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `help`
--

LOCK TABLES `help` WRITE;
/*!40000 ALTER TABLE `help` DISABLE KEYS */;
/*!40000 ALTER TABLE `help` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `items`
--

DROP TABLE IF EXISTS `items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `items` (
  `id` int(11) NOT NULL,
  `name` varchar(100) DEFAULT NULL,
  `keywords` tinytext,
  `item_level` int(11) DEFAULT NULL,
  `char_level` int(11) DEFAULT NULL,
  `equip_location` int(11) DEFAULT NULL,
  `quality` int(11) DEFAULT NULL,
  `binds` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `quest` int(11) DEFAULT NULL,
  `armor` int(11) DEFAULT NULL,
  `durability` int(11) DEFAULT NULL,
  `unique` int(11) DEFAULT NULL,
  `damage_low` int(11) DEFAULT NULL,
  `damage_high` int(11) DEFAULT NULL,
  `value` int(11) DEFAULT NULL,
  `speed` decimal(10,0) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `items`
--

LOCK TABLES `items` WRITE;
/*!40000 ALTER TABLE `items` DISABLE KEYS */;
INSERT INTO `items` VALUES (1,'The One Ring','one ring',15,10,11,2,2,13,0,0,0,0,0,0,140,0),(2,'Test Item of Power','test item power',5,2,1,4,1,0,0,100,100,1,0,0,1000,0),(3,'Worn Greatsword','',1,1,16,1,0,4,0,0,25,0,1,3,4,4),(4,'Recruit\'s Vest','',1,1,5,1,0,3,0,10,55,0,0,0,2,0),(5,'Recruit\'s Pants','',1,1,9,1,0,3,0,1,40,0,0,0,2,0),(6,'Recruit\'s Boots','',1,1,10,1,0,3,0,8,25,0,0,0,1,0);
/*!40000 ALTER TABLE `items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_drops`
--

DROP TABLE IF EXISTS `npc_drops`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `npc_drops` (
  `npc` int(11) NOT NULL,
  `items` varchar(45) NOT NULL,
  `percent` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `npc_drops`
--

LOCK TABLES `npc_drops` WRITE;
/*!40000 ALTER TABLE `npc_drops` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_drops` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npc_skills`
--

DROP TABLE IF EXISTS `npc_skills`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `npc_skills` (
  `npc` int(11) NOT NULL,
  `skill` int(11) NOT NULL,
  PRIMARY KEY (`npc`,`skill`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `npc_skills`
--

LOCK TABLES `npc_skills` WRITE;
/*!40000 ALTER TABLE `npc_skills` DISABLE KEYS */;
/*!40000 ALTER TABLE `npc_skills` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `npcs`
--

DROP TABLE IF EXISTS `npcs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `npcs` (
  `id` int(11) NOT NULL,
  `name` varchar(100) NOT NULL,
  `keywords` tinytext NOT NULL,
  `level` int(11) NOT NULL,
  `gender` int(11) NOT NULL,
  `race` int(11) NOT NULL,
  `agility` int(11) NOT NULL,
  `intellect` int(11) NOT NULL,
  `strength` int(11) NOT NULL,
  `vitality` int(11) NOT NULL,
  `wisdom` int(11) NOT NULL,
  `health` int(11) NOT NULL,
  `mana` int(11) NOT NULL,
  `stamina` int(11) NOT NULL,
  `title` varchar(45) NOT NULL,
  `attack_speed` double NOT NULL,
  `damage_low` int(11) NOT NULL,
  `damage_high` int(11) NOT NULL,
  `flags` tinytext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `npcs`
--

LOCK TABLES `npcs` WRITE;
/*!40000 ALTER TABLE `npcs` DISABLE KEYS */;
INSERT INTO `npcs` VALUES (1,'Marshal McBride','',20,1,0,10,10,10,10,10,1227,100,800,'',2,1,1,'0;'),(2,'Blackrock Worg','',1,1,0,10,10,10,10,10,10,10,10,'',2,1,2,'1;');
/*!40000 ALTER TABLE `npcs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_active_quests`
--

DROP TABLE IF EXISTS `player_active_quests`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `player_active_quests` (
  `player` varchar(12) NOT NULL,
  `quest` int(11) NOT NULL,
  `objectives` tinytext,
  PRIMARY KEY (`player`,`quest`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_active_quests`
--

LOCK TABLES `player_active_quests` WRITE;
/*!40000 ALTER TABLE `player_active_quests` DISABLE KEYS */;
/*!40000 ALTER TABLE `player_active_quests` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_class_data`
--

DROP TABLE IF EXISTS `player_class_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `player_class_data` (
  `player` varchar(12) NOT NULL,
  `class` int(11) NOT NULL,
  `level` int(11) NOT NULL,
  PRIMARY KEY (`player`,`class`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_class_data`
--

LOCK TABLES `player_class_data` WRITE;
/*!40000 ALTER TABLE `player_class_data` DISABLE KEYS */;
INSERT INTO `player_class_data` VALUES ('Bob',1,40),('Bob',2,30),('Bob',3,81),('Bob',4,89),('Boris',1,479),('Monk',1,60),('Monk',2,120),('Monk',4,60),('Paladin',1,120),('Paladin',4,120),('Shaman',1,60),('Shaman',3,60),('Shaman',4,120);
/*!40000 ALTER TABLE `player_class_data` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_completed_quests`
--

DROP TABLE IF EXISTS `player_completed_quests`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `player_completed_quests` (
  `player` varchar(12) NOT NULL,
  `quest` int(11) NOT NULL,
  PRIMARY KEY (`player`,`quest`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_completed_quests`
--

LOCK TABLES `player_completed_quests` WRITE;
/*!40000 ALTER TABLE `player_completed_quests` DISABLE KEYS */;
/*!40000 ALTER TABLE `player_completed_quests` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_cooldowns`
--

DROP TABLE IF EXISTS `player_cooldowns`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `player_cooldowns` (
  `player` varchar(12) NOT NULL,
  `skill` int(11) NOT NULL,
  `timestamp` double DEFAULT NULL,
  PRIMARY KEY (`player`,`skill`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_cooldowns`
--

LOCK TABLES `player_cooldowns` WRITE;
/*!40000 ALTER TABLE `player_cooldowns` DISABLE KEYS */;
/*!40000 ALTER TABLE `player_cooldowns` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_inventory`
--

DROP TABLE IF EXISTS `player_inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `player_inventory` (
  `player` varchar(12) NOT NULL,
  `item` int(11) NOT NULL,
  `location` int(11) NOT NULL,
  KEY `player` (`player`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_inventory`
--

LOCK TABLES `player_inventory` WRITE;
/*!40000 ALTER TABLE `player_inventory` DISABLE KEYS */;
INSERT INTO `player_inventory` VALUES ('Boris',4,0),('Boris',5,0),('Boris',6,0),('Boris',3,0),('Boris',4,1),('Boris',3,1),('Bob',4,0),('Bob',5,0),('Bob',6,0),('Bob',3,0),('Paladin',4,0),('Paladin',5,0),('Paladin',6,0),('Paladin',3,0),('Monk',4,0),('Monk',5,0),('Monk',6,0),('Monk',3,0);
/*!40000 ALTER TABLE `player_inventory` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_spell_affects`
--

DROP TABLE IF EXISTS `player_spell_affects`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `player_spell_affects` (
  `player` varchar(12) NOT NULL,
  `caster` varchar(12) NOT NULL,
  `skill` int(11) NOT NULL,
  `ticks` int(11) DEFAULT NULL,
  `duration` double DEFAULT NULL,
  `timeleft` double DEFAULT NULL,
  `stackable` int(11) DEFAULT NULL,
  `hidden` int(11) DEFAULT NULL,
  `debuff` tinyint(4) DEFAULT NULL,
  `category` int(11) DEFAULT NULL,
  `auras` tinytext,
  `data` tinytext,
  PRIMARY KEY (`player`,`skill`,`caster`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_spell_affects`
--

LOCK TABLES `player_spell_affects` WRITE;
/*!40000 ALTER TABLE `player_spell_affects` DISABLE KEYS */;
/*!40000 ALTER TABLE `player_spell_affects` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `players`
--

DROP TABLE IF EXISTS `players`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `players` (
  `name` varchar(12) NOT NULL,
  `password` varchar(15) NOT NULL,
  `immlevel` int(11) NOT NULL,
  `title` varchar(45) NOT NULL,
  `experience` int(11) NOT NULL,
  `room` int(11) NOT NULL,
  `level` int(11) NOT NULL,
  `gender` int(11) NOT NULL,
  `race` int(11) NOT NULL,
  `agility` int(11) NOT NULL,
  `intellect` int(11) NOT NULL,
  `strength` int(11) NOT NULL,
  `vitality` int(11) NOT NULL,
  `wisdom` int(11) NOT NULL,
  `health` int(11) NOT NULL,
  `mana` int(11) NOT NULL,
  `stamina` int(11) NOT NULL,
  `class` int(11) NOT NULL,
  `recall` int(11) NOT NULL,
  `ghost` tinyint(4) NOT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `players`
--

LOCK TABLES `players` WRITE;
/*!40000 ALTER TABLE `players` DISABLE KEYS */;
INSERT INTO `players` VALUES ('Bob','\r\Z',0,'',16516800,2,240,1,0,139,512,118,446,269,2230,5120,1180,4,0,0),('Boris','\r\Z',10,'',16516800,2,240,1,7,249,10,488,727,10,3635,100,4880,1,0,0),('Monk','\r\Z',0,'',16516800,2,240,1,5,429,130,248,547,130,2735,1300,2480,2,0,0),('Paladin','\r\Z',0,'',16516800,2,240,1,4,129,250,248,607,250,3035,2500,2480,4,0,0),('Shaman','\r\Z',0,'',16516800,2,240,1,8,70,488,130,488,308,2440,4880,1300,1,0,0);
/*!40000 ALTER TABLE `players` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `quest_objectives`
--

DROP TABLE IF EXISTS `quest_objectives`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `quest_objectives` (
  `quest` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `id` int(11) NOT NULL,
  `count` int(11) DEFAULT NULL,
  `description` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`quest`,`type`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `quest_objectives`
--

LOCK TABLES `quest_objectives` WRITE;
/*!40000 ALTER TABLE `quest_objectives` DISABLE KEYS */;
/*!40000 ALTER TABLE `quest_objectives` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `quests`
--

DROP TABLE IF EXISTS `quests`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `quests` (
  `id` int(11) NOT NULL,
  `name` varchar(100) DEFAULT NULL,
  `short_description` text,
  `long_description` text,
  `progress_message` text,
  `completion_message` text,
  `level` int(11) DEFAULT NULL,
  `quest_requirement` int(11) DEFAULT NULL,
  `start` int(11) DEFAULT NULL,
  `end` int(11) DEFAULT NULL,
  `exp_reward` int(11) DEFAULT NULL,
  `money_reward` int(11) DEFAULT NULL,
  `shareable` int(11) DEFAULT NULL,
  `level_requirement` int(11) DEFAULT NULL,
  `quest_restriction` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `quests`
--

LOCK TABLES `quests` WRITE;
/*!40000 ALTER TABLE `quests` DISABLE KEYS */;
/*!40000 ALTER TABLE `quests` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `resets`
--

DROP TABLE IF EXISTS `resets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `resets` (
  `room_id` int(11) NOT NULL,
  `id` int(11) NOT NULL,
  `type` int(11) DEFAULT NULL,
  `target_id` int(11) DEFAULT NULL,
  `wander_dist` int(11) DEFAULT NULL,
  `leash_dist` int(11) DEFAULT NULL,
  `interval` int(11) DEFAULT NULL,
  PRIMARY KEY (`room_id`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `resets`
--

LOCK TABLES `resets` WRITE;
/*!40000 ALTER TABLE `resets` DISABLE KEYS */;
INSERT INTO `resets` VALUES (2,1,1,1,0,0,30),(6,1,1,2,0,0,15),(6,2,1,2,0,0,20),(7,1,1,2,0,0,20),(7,2,1,2,0,0,18);
/*!40000 ALTER TABLE `resets` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `rooms`
--

DROP TABLE IF EXISTS `rooms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `rooms` (
  `id` int(11) NOT NULL,
  `name` varchar(50) DEFAULT NULL,
  `description` text,
  `area` int(11) DEFAULT NULL,
  `flags` tinytext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `rooms`
--

LOCK TABLES `rooms` WRITE;
/*!40000 ALTER TABLE `rooms` DISABLE KEYS */;
INSERT INTO `rooms` VALUES (1,'The One Room','One Room to rule them all.\n\r',0,''),(2,'On the Steps of Northshire Abbey','',1,'0;'),(3,'Northshire Abbey','',1,''),(4,'Dermot\'s Wagon','',1,''),(5,'Northshire Valley','',1,''),(6,'Northshire Valley','',1,''),(7,'Northshire Valley','',1,'');
/*!40000 ALTER TABLE `rooms` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `skills`
--

DROP TABLE IF EXISTS `skills`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `skills` (
  `id` int(11) NOT NULL,
  `long_name` varchar(100) NOT NULL,
  `name` varchar(100) DEFAULT NULL,
  `cooldown` double DEFAULT NULL,
  `target_type` int(11) DEFAULT NULL,
  `affect_desc` text,
  `cast_time` double DEFAULT NULL,
  `function_name` varchar(100) DEFAULT NULL,
  `cast_script` text,
  `apply_script` text,
  `tick_script` text,
  `remove_script` text,
  `cost_script` text,
  PRIMARY KEY (`id`),
  UNIQUE KEY `long_name_UNIQUE` (`long_name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `skills`
--

LOCK TABLES `skills` WRITE;
/*!40000 ALTER TABLE `skills` DISABLE KEYS */;
INSERT INTO `skills` VALUES (1,'Recall','recall',1800,0,'Return to your preset location. Set with \"recall\" command.',10,'skill_recall','function skill_recall_cast(caster, target, skill)\n\n\n    caster:Message(\"|WYou have completed your spell.|X\", 0, null);\n\n    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.|X\", 3, null);\n\n    caster:SetCooldown(skill, \"\", false, -1);\n\n    caster:ConsumeMana(100)\n\n\n    caster:Message(\"\" .. caster:GetName() .. \" disappears in a flash of light.\", 3, null);\n\n\n    caster:ChangeRoomsID(caster:GetPlayer().recall);\n\n    caster:Message(\"\" .. caster:GetName() .. \" appears in a flash of light.\", 3, null);\n\n\n    cmd_look(caster, \"\");\n\n\n\n\n\nend\n\n\n','','','','function skill_recall_cost(caster, target, skill)\r\n\r\n    if(caster:GetPlayer().recall == 0) then\r\n\r\n        caster:Message(\"You must set your recall location first.\", 0, null);\r\n\r\n        return 0;\r\n\r\n\r\n    end\r\n\r\n    if(not caster:HasResource(2, 100)) then\r\n\r\n\r\n        caster:Message(\"You don\'t have enough mana.\", 0, null);\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n\r\n\r\n    return 1;\r\n\r\n\r\nend\r\n\r\n\r\n'),(2,'Fireball','fireball',0,2,'none',2.5,'skill_fireball','function skill_fireball_cast(caster, target, skill)\r\n\r\n    caster:Message(\"|WYou have completed your spell.|X\", 0, null);\r\n\r\n    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.|X\", 3, null);\r\n\r\n    caster:Send(\"Your fireball hits \" .. target:GetName() .. \" for 10 damage.\\n\\r\");\r\n\r\n    target:Send(caster:GetName() .. \"\'s fireball hits you for 10 damage.\\n\\r\");\r\n\r\n    caster:Message(caster:GetName() .. \"\'s fireball hits \" .. target:GetName() .. \" for 10 damage.\", 5, target);\r\n\r\n    caster:EnterCombat(target);\r\n\r\n\r\n    target:AdjustHealth(caster, -10);\r\n\r\n    caster:ConsumeMana(25)\r\n\r\n\r\n    caster:SetCooldown(skill, \"\", false, 0)\r\n\r\n\r\nend\r\n\r\n\r\n','','','','function skill_fireball_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\n\r\nend\r\n\r\n'),(3,'Heal','heal',0,4,'none',3,'skill_heal','function skill_heal_cast(caster, target, skill)\r\n\r\n    caster:Message(\"|WYou have completed your spell.|X\", 0, null);\r\n\r\n    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.|X\", 3, null);\r\n\r\n\r\n    if(caster:IsFighting(target)) then\r\n\r\n        caster:Send(\"You can\'t heal the target you\'re attacking.\\n\\r\")\r\n\r\n        return\r\n\r\n    end\r\n\r\n    if(caster:GetName() == target:GetName()) then\r\n\r\n        caster:Send(\"You heal yourself for 20 health.\\n\\r\");\r\n\r\n    else\r\n\r\n        caster:Send(\"You heal \" .. target:GetName() .. \" for 20 health.\\n\\r\");\r\n\r\n        target:Send(caster:GetName() .. \"\'s heal restores 20 health.\\n\\r\");\r\n\r\n    end\r\n\r\n\r\n    caster:Message(caster:GetName() .. \"\'s spell heals \" .. target:GetName() .. \" for 20 health.\", 5, target);\r\n\r\n    target:AdjustHealth(caster, 20);\r\n\r\n    caster:SetCooldown(skill, \"\", false, 0)\r\n\r\n    caster:AdjustMana(caster, -20)\r\n\r\nend\r\n\r\n\r\n','','','','function skill_heal_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 12 + math.ceil(2.53*caster:GetPlayer():GetClassLevel(4)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend'),(4,'Frost Nova','frost nova',25,2,'Frozen in place.',0,'skill_frost_nova','function skill_frost_nova_cast(caster, target, sk)\r\n\r\n    caster:Message(\"|WYou cast frost nova.|X\", 0, null);\r\n\r\n    target:AddSpellAffect(1, caster, \"Frost Nova\", false, false, 0, 8.0, 1, sk);\r\n\r\n    caster:EnterCombat(target);\r\n\r\n    caster:SetCooldown(sk, \"\", false, -1)\r\n\r\n    caster:AdjustMana(caster, -200)\r\n\r\n\r\n\r\nend\r\n\r\n\r\n','function skill_frost_nova_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by your frost nova.\\n\\r\");\r\n\r\n    target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s frost nova.|X\\n\\r\");\r\n\r\n    caster:Message(\"|W\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s frost nova.|X\", 5, target);\r\n\r\n    affect:ApplyAura(1, -100); --Movement speed minus 100%\r\n\r\nend\r\n\r\n','','function skill_frost_nova_remove(caster, target, affect)\r\n\r\n    target:Send(\"Frost nova fades from you.\\n\\r\");\r\n\r\n    if(caster ~= nil) then\r\n\r\n        caster:Send(\"Your frost nova fades from \" .. target:GetName() .. \".\\n\\r\");\r\n\r\n\r\n\r\n        caster:Message(caster:GetName() .. \"\'s frost nova fades from \" .. target:GetName() .. \".\", 5, target);\r\n\r\n    else\r\n\r\n        target:Message(affect:GetCasterName() .. \"\'s frost nova fades from \" .. target:GetName() .. \".\", 5, nil)\r\n\r\n    end\r\n\r\nend\r\n\r\n','function skill_frost_nova_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\n\r\nend\r\n\r\n'),(5,'Renew','renew',0,4,'Restores health every 3 seconds.',0,'skill_renew','function skill_renew_cast(caster, target, sk)\r\n\r\n\r\n    caster:Message(\"You cast renew.\", 0, null);\r\n\r\n    if(caster:GetName() ~= target:GetName()) then\r\n\r\n        target:Send(\"\" .. caster:GetName() .. \" casts renew on you.\\n\\r\");\r\n\r\n    end\r\n\r\n\r\n    caster:Message(\"\" .. caster:GetName() .. \" casts renew on \" .. target:GetName() .. \".\", 5, target);\r\n\r\n    target:AddSpellAffect(0, caster, \"Renew\", false, false, 5, 15.0, 1, sk);\r\n\r\n    caster:ConsumeMana(12 + math.ceil(2.53*caster:GetPlayer():GetClassLevel(4)))\r\n\r\n\r\n    caster:SetCooldown(sk, \"\", false, 0)\r\n\r\nend\r\n\r\n\r\n','function skill_renew_apply(caster, target, affect)\r\n    local healvalue = math.ceil(2.27 * caster:GetPlayer():GetClassLevel(4));\r\n\r\n    affect:SaveDataInt(\"healvalue\", healvalue);\r\n\r\n\r\n    if(caster:GetName() == target:GetName()) then\r\n        target:Send(\"You are affected by renew.\\n\\r\");\r\n    else\r\n        caster:Send(target:GetName() .. \" is affected by your renew.\\n\\r\");\r\n        target:Send(\"You are affected by \" .. caster:GetName() .. \"\'s renew.\\n\\r\");\r\n    end\r\nend\r\n\r\n','function skill_renew_tick(caster, target, affect)\r\n\r\n    local healvalue = affect:GetDataInt(\"healvalue\");\r\n\r\n\r\n    if(affect:GetCasterName() == target:GetName()) then\r\n\r\n        target:Send(\"Your renew heals you for \" .. healvalue .. \" health.\\n\\r\");\r\n\r\n    else\r\n\r\n        if(caster ~= nil) then\r\n\r\n            caster:Send(\"Your renew heals \" .. target:GetName() .. \" for \" .. healvalue .. \" health.\\n\\r\");\r\n\r\n        end\r\n\r\n        target:Send(affect:GetCasterName() .. \"\'s renew restores \" .. healvalue .. \" health.\\n\\r\");\r\n\r\n    end\r\n\r\n    target:Message(affect:GetCasterName() .. \"\'s renew heals \" .. target:GetName() .. \" for \" .. healvalue .. \" health.\", 5, caster)\r\n\r\n    target:AdjustHealth(caster, healvalue);\r\n\r\n\r\n\r\n\r\nend\r\n\r\n','function skill_renew_remove(caster, target, affect)\r\n    if(affect:GetCasterName() == target:GetName()) then\r\n        target:Send(\"Your renew fades.\\n\\r\");\r\n    else\r\n        if(caster ~= nil) then\r\n            caster:Send(\"Your renew fades from \" .. target:GetName() .. \".\\n\\r\");\r\n        end\r\n        target:Send(affect:GetCasterName() .. \"\'s renew fades from you.\\n\\r\");\r\n    end\r\n    target:Message(affect:GetCasterName() .. \"\'s renew fades from \" .. target:GetName() .. \".\", 5, caster)\r\nend\r\n\r\n\r\n','function skill_renew_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 12 + math.ceil(2.53*caster:GetPlayer():GetClassLevel(4)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(6,'Hamstring','hamstring',0,2,'Movement speed reduced by 50%',0,'skill_hamstring','function skill_hamstring_cast(caster, target, sk)\r\n\r\n    target:AddSpellAffect(1, caster, \"Hamstring\", false, false, 0, 15.0, 0, sk)\r\n\r\n    caster:EnterCombat(target)\r\n\r\n    caster:AdjustStamina(caster, -(10 + (2*caster:GetPlayer():GetClassLevel(1))))\r\n\r\n    caster:SetCooldown(sk, \"\", false, 0)\r\n\r\nend\r\n\r\n','function skill_hamstring_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by your hamstring.\\n\\r\")\r\n\r\n    target:Send(\"You are affected by \" .. caster:GetName() .. \"\'s hamstring.\\n\\r\")\r\n\r\n    caster:Message(\"\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s hamstring.\", 5, target);\r\n\r\n    affect:ApplyAura(1, -50);\r\n\r\n\r\nend\r\n\r\n','','function skill_hamstring_remove(caster, target, affect)\r\n\r\n\r\n    target:Send(\"Hamstring fades from you.\\n\\r\");\r\n\r\n    caster:Send(\"Your hamstring fades from \" .. target:GetName() .. \".\\n\\r\");\r\n\r\n\r\n    caster:Message(caster:GetName() .. \"\'s hamstring fades from \" .. target:GetName() .. \".\", 5, target);\r\n\r\n\r\nend\r\n\r\n','function skill_hamstring_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(3, 10 + (2*caster:GetPlayer():GetClassLevel(1)))) then\r\n\r\n\r\n        caster:Send(\"You don\'t have enough stamina.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n\r\n'),(7,'Greater Heal','greater heal',0,4,'none',2.5,'skill_greater_heal','function skill_greater_heal_cast(caster, target, skill)\r\n\r\n    local manacost = 20 + (4*caster:GetPlayer():GetClassLevel(4));\r\n\r\n    local healamount = 100 + (15 * caster:GetPlayer():GetClassLevel(4));\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n    caster:Message(\"You have completed your spell.\", 0, null);\r\n\r\n    caster:Message(\"\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.\", 3, null);\r\n\r\n\r\n\r\n    if(caster:GetName() == target:GetName()) then\r\n\r\n        caster:Send(\"You heal yourself for \" .. healamount .. \" health.\\n\\r\");\r\n\r\n\r\n    else\r\n\r\n        caster:Send(\"You heal \" .. target:GetName() .. \" for \" .. healamount .. \" health.\\n\\r\");\r\n\r\n        target:Send(caster:GetName() .. \"\'s heal restores \" .. healamount .. \" health.\\n\\r\");\r\n\r\n\r\n    end\r\n\r\n\r\n    caster:Message(caster:GetName() .. \"\'s spell heals \" .. target:GetName() .. \" for \" .. healamount .. \" health.\", 5, target);\r\n\r\n\r\n    target:AdjustHealth(caster, healamount);\r\n\r\n\r\n    caster:SetCooldown(skill, \"\", false, 0)\r\n\r\n    caster:ConsumeMana(manacost);\r\n\r\n\r\n\r\n\r\nend\r\n\r\n\r\n','','','','function skill_greater_heal_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 20 + (4*caster:GetPlayer():GetClassLevel(4)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(8,'Zot','zot',0,3,'Health, Mana, Stamina = 1',0,'skill_zot','function skill_zot_cast(caster, target, skill)\r\n\r\n    caster:Send(\"|YYou send a bolt of lightning to strike \" .. target:GetName() .. \".|X\\n\\r\");\r\n\r\n\r\n    target:Send(\"|YZOT! You are struck by a bolt of lightning!|X\\n\\r\");\r\n\r\n    target:Message(\"|YZOT! \" .. target:GetName() .. \" is struck by a bolt of lightning!|X\", 3, null);\r\n\r\n\r\n    target:AdjustHealth(caster, -(target:GetHealth() - 1));\r\n\r\n    target:AdjustMana(caster, -(target:GetMana() - 1));\r\n\r\n    target:AdjustStamina(caster, -(target:GetStamina() - 1));\r\n\r\nend\r\n','','','','function skill_zot_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(9,'Sprint','sprint',60,0,'Increases your movement speed by 70% for 8 seconds',0,'skill_sprint','function skill_sprint_cast(caster, target, sk)\r\n\r\n    caster:Message(\"You cast sprint.\", 0, null);\r\n\r\n    caster:Message(\"\" .. caster:GetName() .. \" casts sprint.\", 3, caster);\r\n\r\n    caster:AddSpellAffect(0, caster, \"Sprint\", false, false, 0, 8, 0, sk);\r\n\r\n    caster:SetCooldown(sk, \"\", false, -1);\r\n\r\n\r\nend\r\n\r\n','function skill_sprint_apply(caster, target, affect)\r\n\r\n    affect:ApplyAura(1, 70);\r\n\r\nend\r\n\r\n','','function skill_sprint_remove(caster, target, affect)\r\n\r\n    target:Send(\"You are no longer sprinting.\\n\\r\");\r\n\r\n    target:Message(affect:GetCasterName() .. \"\'s sprint fades from \" .. affect:GetCasterName() .. \".\", 3, target)\r\n\r\n\r\nend\r\n\r\n','function skill_sprint_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(10,'Lucifron\'s Curse','lucifrons curse',0,2,'Spell and ability costs are increased by 100%',0,'skill_lucifrons_curse','function skill_lucifrons_curse_cast(caster, target, skill)\r\n\r\n    caster:Message(\"You cast Lucifron\'s Curse.\", 0, null);\r\n\r\n    target:AddSpellAffect(1, caster, \"Lucifron\'s Curse\", false, false, 0, 300, 2, skill);\r\n\r\nend\r\n\r\n','function skill_lucifrons_curse_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by Lucifron\'s curse.\\n\\r\")\r\n\r\n    target:Send(\"You are affected by Lucifron\'s curse.\\n\\r\")\r\n\r\n    caster:Message(\"\" .. target:GetName() .. \" is affected by Lucifron\'s curse.\", 5, target);\r\n\r\n    affect:ApplyAura(2, 100);\r\n\r\nend\r\n\r\n','','function skill_lucifrons_curse_remove(caster, target, affect)\r\n\r\n\r\n\r\n    target:Send(\"Lucifron\'s curse fades from you.\\n\\r\");\r\n\r\n    caster:Send(\"Lucifron\'s curse fades from \" .. target:GetName() .. \".\\n\\r\");\r\n\r\n    caster:Message(\"Lucifron\'s curse fades from \" .. target:GetName() .. \".\", 5, target);\r\n\r\nend\r\n\r\n','function skill_lucifrons_curse_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(11,'Impending Doom','impending doom',0,2,'2000 Shadow damage inflicted after 10 sec.',0,'skill_impending_doom','function skill_impending_doom_cast(caster, target, skill)\r\n\r\n    caster:EnterCombat(target)\r\n\r\n    caster:Message(\"You cast Impending Doom.\", 0, null);\r\n\r\n    target:AddSpellAffect(1, caster, \"Impending Doom\", false, false, 1, 10, 1, skill);\r\n\r\nend\r\n\r\n','function skill_impending_doom_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by Impending Doom.\\n\\r\")\r\n\r\n    target:Send(\"You are affected by Impending Doom.\\n\\r\")\r\n\r\n    caster:Message(\"\" .. target:GetName() .. \" is affected by Impending Doom.\", 5, target);\r\n\r\nend\r\n\r\n','function skill_impending_doom_tick(caster, target, affect)\r\n\r\n    target:AdjustHealth(caster, -2000)\r\n\r\nend\r\n\r\n','function skill_impending_doom_remove(caster, target, affect)\r\n\r\n    target:Send(\"Impending Doom fades from you.\\n\\r\");\r\n\r\n    target:Message(\"Impending Doom fades from \" .. target:GetName() .. \".\", 3, target);\r\n\r\nend\r\n\r\n','function skill_impending_doom_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(12,'Remove Curse','remove curse',5,4,'Removes all curses from a friendly target',0,'skill_remove_curse','function skill_remove_curse_cast(caster, target, skill)\r\n\r\n    local removed = target:CleanseSpellAffect(caster, 2, -1);\r\n\r\n    caster:Send(\"Removed \" .. removed .. \" curses.\\n\\r\");\r\n\r\n    if(caster:GetName() ~= target:GetName()) then\r\n\r\n        target:Send(\"Removed \" .. removed .. \" curses.\\n\\r\");\r\n\r\n    end\r\n\r\nend\r\n\r\n','','','','function skill_remove_curse_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 250 + math.ceil(1.5*caster:GetPlayer():GetClassLevel(3)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(13,'Purify','purify',5,4,'Dispels harmful magic and disease affects.',0,'skill_purify','function skill_purify_cast(caster, target, skill)\r\n\r\n    local magic = target:CleanseSpellAffect(caster, 1, -1);\r\n\r\n    local disease = target:CleanseSpellAffect(caster, 4, -1);\r\n\r\n    caster:Send(\"Removed \" .. magic .. \" magic affects.\\n\\r\");\r\n\r\n    caster:Send(\"Removed \" .. disease .. \" diseases.\\n\\r\");\r\n\r\n    if(caster:GetName() ~= target:GetName()) then\r\n\r\n        target:Send(\"Removed \" .. magic .. \" magic affects.\\n\\r\");\r\n\r\n        target:Send(\"Removed \" .. disease .. \" diseases.\\n\\r\");\r\n\r\n    end\r\n\r\nend\r\n','','','','function skill_purify_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 150 + math.ceil(1.75*caster:GetPlayer():GetClassLevel(4)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(14,'Slow','slow',0,2,'Movement speed decreased by 80%, gradually increasing over spell duration',0,'skill_slow','function skill_slow_cast(caster, target, sk)\r\n\r\n    caster:Message(\"You cast slow on \" .. target:GetName() .. \".\", 0, null);\r\n\r\n    caster:Message(\"\" .. caster:GetName() .. \" casts slow on \" .. target:GetName() .. \".\", 3, caster);\r\n\r\n    target:AddSpellAffect(1, caster, \"Slow\", false, false, 5, 10, 0, sk);\r\n\r\n    caster:SetCooldown(sk, \"\", false, -1);\r\n\r\n    target:Send(\"You are affected by \" .. caster:GetName() .. \"\'s slow.\");\r\n\r\n    caster:EnterCombat(target);\r\n\r\n    caster:ConsumeMana(25)\r\n\r\nend\r\n\r\n\r\n\r\n','function skill_slow_apply(caster, target, affect)\r\n\r\n    affect:ApplyAura(1, -80);\r\n\r\n\r\nend\r\n\r\n','function skill_slow_tick(caster, target, affect)\r\n\r\n    -- need a RemoveAura function in affect\r\n\r\n    --need a GetRemainingDuration function in affect\r\n\r\n    -- need a GetCurrentTick() function in affect\r\n\r\n    -- todo: remove any movespeed aura, apply a movespeed of -80 + (15 * GetCurrentTick())\r\n\r\nend\r\n\r\n','function skill_slow_remove(caster, target, affect)\r\n\r\nend\r\n\r\n','function skill_slow_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n');
/*!40000 ALTER TABLE `skills` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `triggers`
--

DROP TABLE IF EXISTS `triggers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `triggers` (
  `parent_id` int(11) NOT NULL,
  `id` int(11) DEFAULT NULL,
  `parent_type` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `argument` varchar(45) DEFAULT NULL,
  `script` text,
  `function` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`parent_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `triggers`
--

LOCK TABLES `triggers` WRITE;
/*!40000 ALTER TABLE `triggers` DISABLE KEYS */;
/*!40000 ALTER TABLE `triggers` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-01-25  1:41:37
