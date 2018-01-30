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
INSERT INTO `class_skills` VALUES (1,1,1),(2,1,1),(3,1,1),(4,1,1),(4,5,1),(3,15,1),(1,16,1),(2,21,1),(2,22,1),(4,26,1),(4,29,1),(3,32,1),(2,33,1);
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
  `items` tinytext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `classes`
--

LOCK TABLES `classes` WRITE;
/*!40000 ALTER TABLE `classes` DISABLE KEYS */;
INSERT INTO `classes` VALUES (1,'Warrior','|M','3;4;5;6;'),(2,'Rogue','|Y','11;12;13;14;'),(3,'Mage','|C','7;8;9;10;'),(4,'Cleric','|W','15;16;17;10;');
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
INSERT INTO `exits` VALUES (2,0,3),(2,2,8),(2,4,9),(2,6,4),(3,1,11),(3,4,2),(3,7,10),(4,2,2),(4,6,5),(5,0,6),(5,2,4),(5,6,7),(6,4,5),(7,2,5),(8,6,2),(9,0,2),(10,3,3),(11,5,3);
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
  `name` varchar(100) NOT NULL,
  `keywords` tinytext NOT NULL,
  `item_level` int(11) NOT NULL,
  `char_level` int(11) NOT NULL,
  `equip_location` int(11) NOT NULL,
  `quality` int(11) NOT NULL,
  `binds` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `quest` int(11) NOT NULL,
  `armor` int(11) NOT NULL,
  `durability` int(11) NOT NULL,
  `unique` int(11) NOT NULL,
  `damage_low` int(11) NOT NULL,
  `damage_high` int(11) NOT NULL,
  `value` int(11) NOT NULL,
  `speed` decimal(10,1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `items`
--

LOCK TABLES `items` WRITE;
/*!40000 ALTER TABLE `items` DISABLE KEYS */;
INSERT INTO `items` VALUES (1,'The One Ring','one ring',15,10,11,2,2,13,0,0,0,0,0,0,140,0.0),(2,'Test Item of Power','test item power',5,2,1,4,1,0,0,100,100,1,0,0,1000,0.0),(3,'Worn Greatsword','',1,1,16,1,0,4,0,0,25,0,1,3,4,3.6),(4,'Recruit\'s Vest','',1,1,5,1,0,0,0,3,55,0,0,0,2,0.0),(5,'Recruit\'s Pants','',1,1,9,1,0,0,0,3,40,0,0,0,2,0.0),(6,'Recruit\'s Boots','',1,1,10,1,0,0,0,2,25,0,0,0,1,0.0),(7,'Apprentice\'s Boots','',1,1,10,1,0,0,0,2,25,0,0,0,2,0.0),(8,'Apprentice\'s Robe','',1,1,5,1,0,0,0,3,55,0,0,0,2,0.0),(9,'Apprentice\'s Pants','',1,1,9,1,0,0,0,3,40,0,0,0,2,0.0),(10,'Bent Staff','',1,1,16,1,0,9,0,0,25,0,2,3,4,3.6),(11,'Footpad\'s Pants','',1,1,9,1,0,0,0,2,40,0,0,0,2,0.0),(12,'Footpad\'s Vest','',1,1,5,1,0,0,0,3,55,0,0,0,2,0.0),(13,'Footpad\'s Shoes','',1,1,10,1,0,0,0,3,25,0,0,0,1,0.0),(14,'Worn Shortsword','',1,1,14,1,0,4,0,0,20,0,1,2,3,2.6),(15,'Neophyte\'s Pants','',1,1,9,1,0,0,0,3,40,0,0,0,2,0.0),(16,'Neophyte\'s Boots','',1,1,10,1,0,0,0,2,25,0,0,0,2,0.0),(17,'Neophyte\'s Robe','',1,1,5,1,0,0,0,3,55,0,0,0,2,0.0);
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
  `stamina` int(11) NOT NULL,
  `wisdom` int(11) NOT NULL,
  `health` int(11) NOT NULL,
  `mana` int(11) NOT NULL,
  `energy` int(11) NOT NULL,
  `rage` int(11) NOT NULL,
  `title` varchar(45) NOT NULL,
  `attack_speed` double NOT NULL,
  `damage_low` int(11) NOT NULL,
  `damage_high` int(11) NOT NULL,
  `flags` tinytext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `npcs`
--

LOCK TABLES `npcs` WRITE;
/*!40000 ALTER TABLE `npcs` DISABLE KEYS */;
INSERT INTO `npcs` VALUES (1,'Marshal McBride','',20,1,0,10,10,10,10,10,1227,100,0,0,'',2,1,1,'0;'),(2,'Blackrock Worg','',1,1,0,10,10,10,25,10,25,10,0,0,'',2,1,2,'1;');
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
INSERT INTO `player_class_data` VALUES ('Bob',1,40),('Bob',2,30),('Bob',3,81),('Bob',4,89),('Bobo',3,2),('Boris',1,59),('Boris',2,60),('Boris',3,60),('Boris',4,60),('Monk',1,60),('Monk',2,120),('Monk',4,60),('Paladin',1,120),('Paladin',4,120),('Shaman',1,60),('Shaman',3,60),('Shaman',4,120);
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
INSERT INTO `player_inventory` VALUES ('Paladin',4,0),('Paladin',5,0),('Paladin',6,0),('Paladin',3,0),('Monk',4,0),('Monk',5,0),('Monk',6,0),('Monk',3,0),('Bob',4,0),('Bob',5,0),('Bob',6,0),('Bob',3,0),('Bobo',8,0),('Bobo',9,0),('Bobo',7,0),('Bobo',10,0);
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
  `stamina` int(11) NOT NULL,
  `wisdom` int(11) NOT NULL,
  `health` int(11) NOT NULL,
  `mana` int(11) NOT NULL,
  `class` int(11) NOT NULL,
  `recall` int(11) NOT NULL,
  `ghost` tinyint(4) NOT NULL,
  `stat_points` int(11) NOT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `players`
--

LOCK TABLES `players` WRITE;
/*!40000 ALTER TABLE `players` DISABLE KEYS */;
INSERT INTO `players` VALUES ('Bob','\r\Z',0,'',16516800,2,240,1,0,139,512,118,446,269,2230,5120,4,0,0,0),('Bobo','\r\Z',0,'',1100,2,2,1,0,10,10,10,10,10,50,100,3,2,0,6),('Boris','\r\Z',10,'',16516800,2,240,1,7,249,110,488,727,10,3635,1100,2,2,0,4208),('Monk','\r\Z',0,'',16516800,2,240,1,5,429,130,248,547,130,2735,1300,2,0,0,0),('Paladin','\r\Z',0,'',16516800,2,240,1,4,129,250,248,607,250,3035,2500,4,0,0,0),('Shaman','\r\Z',0,'',16516800,2,240,1,8,70,488,130,488,308,2440,4880,1,0,0,0);
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
INSERT INTO `quest_objectives` VALUES (1,2,2,6,'Blackrock Worg slain');
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
INSERT INTO `quests` VALUES (1,'Beating Them Back!','Kill 6 Blackrock Worgs.','So you\'re the new recruit from Stormwind, eh? I\'m Marshal McBride, commander of this garrison. Glad to have you on board.\n\r\n\rYou\'ve arrived just in time. The Blackrock orcs have managed to sneak into Northshire through a break in the mountain. My soldiers are doing the best that they can to push them back, but I fear they will be overwhelmed soon.\n\r\n\rHead northwest into the forest and kill the attacking Blackrock worgs! Help my soldiers! \n\r','','You\'ve bought us a little time, but we\'ve got even bigger problems to deal with now.\n\r',3,0,1,1,500,50,0,1,0);
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
INSERT INTO `rooms` VALUES (1,'The One Room','One Room to rule them all.\n\r',0,''),(2,'On the Steps of Northshire Abbey','',1,'0;'),(3,'Northshire Abbey','',1,''),(4,'Dermot\'s Wagon','',1,''),(5,'Northshire Valley','',1,''),(6,'Northshire Valley','',1,''),(7,'Northshire Valley','',1,''),(8,'','',1,''),(9,'','',1,''),(10,'Northshire Abbey','',1,''),(11,'Northshire Abbey','',1,'');
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
  `name` varchar(100) NOT NULL,
  `cooldown` double DEFAULT NULL,
  `target_type` int(11) DEFAULT NULL,
  `description` tinytext,
  `cost_description` tinytext,
  `cast_time` double DEFAULT NULL,
  `interrupt_flags` varchar(15) DEFAULT NULL,
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
INSERT INTO `skills` VALUES (1,'Recall','recall',1800,0,'Return to your preset location. Set with \"recall\" command.','100 mana',10,'0;1;','skill_recall','function skill_recall_cast(caster, target, skill)\n\r    caster:Message(\"|WYou have completed your spell.|X\", 0, null);\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.|X\", 3, null);\n\r    caster:SetCooldown(skill, \"\", false, -1);\n\r    caster:ConsumeMana(100)\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" disappears in a flash of light.|X\", 3, null);\n\r    caster:ChangeRoomsID(caster:GetPlayer().recall);\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" appears in a flash of light.|X\", 3, null);\n\r    cmd_look(caster, \"\");\n\rend\n\r','','','','function skill_recall_cost(caster, target, skill)\n\r    if(caster:GetPlayer().recall == 0) then\n\r        caster:Message(\"You must set your recall location first.\", 0, null);\n\r        return 0;\n\r    end\n\r\n\r    if(not caster:HasResource(RESOURCE_MANA, 100)) then\n\r        caster:Message(\"You don\'t have enough mana.\", 0, null);\n\r        return 0;\n\r    end\n\r\n\r    return 1;\n\rend\n\r'),(2,'Fireball','fireball',0,2,'none',NULL,2.5,NULL,'skill_fireball','function skill_fireball_cast(caster, target, skill)\r\n\r\n    caster:Message(\"|WYou have completed your spell.|X\", 0, null);\r\n\r\n    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.|X\", 3, null);\r\n\r\n    caster:Send(\"Your fireball hits \" .. target:GetName() .. \" for 10 damage.\\n\\r\");\r\n\r\n    target:Send(caster:GetName() .. \"\'s fireball hits you for 10 damage.\\n\\r\");\r\n\r\n    caster:Message(caster:GetName() .. \"\'s fireball hits \" .. target:GetName() .. \" for 10 damage.\", 5, target);\r\n\r\n    caster:EnterCombat(target);\r\n\r\n\r\n    target:AdjustHealth(caster, -10);\r\n\r\n    caster:ConsumeMana(25)\r\n\r\n\r\n    caster:SetCooldown(skill, \"\", false, 0)\r\n\r\n\r\nend\r\n\r\n\r\n','','','','function skill_fireball_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\n\r\nend\r\n\r\n'),(3,'Heal','heal',0,4,'none',NULL,3,NULL,'skill_heal','function skill_heal_cast(caster, target, skill)\r\n\r\n    caster:Message(\"|WYou have completed your spell.|X\", 0, null);\r\n\r\n    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.|X\", 3, null);\r\n\r\n\r\n    if(caster:IsFighting(target)) then\r\n\r\n        caster:Send(\"You can\'t heal the target you\'re attacking.\\n\\r\")\r\n\r\n        return\r\n\r\n    end\r\n\r\n    if(caster:GetName() == target:GetName()) then\r\n\r\n        caster:Send(\"You heal yourself for 20 health.\\n\\r\");\r\n\r\n    else\r\n\r\n        caster:Send(\"You heal \" .. target:GetName() .. \" for 20 health.\\n\\r\");\r\n\r\n        target:Send(caster:GetName() .. \"\'s heal restores 20 health.\\n\\r\");\r\n\r\n    end\r\n\r\n\r\n    caster:Message(caster:GetName() .. \"\'s spell heals \" .. target:GetName() .. \" for 20 health.\", 5, target);\r\n\r\n    target:AdjustHealth(caster, 20);\r\n\r\n    caster:SetCooldown(skill, \"\", false, 0)\r\n\r\n    caster:AdjustMana(caster, -20)\r\n\r\nend\r\n\r\n\r\n','','','','function skill_heal_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 12 + math.ceil(2.53*caster:GetPlayer():GetClassLevel(4)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend'),(4,'Frost Nova','frost nova',25,2,'Frozen in place.',NULL,0,NULL,'skill_frost_nova','function skill_frost_nova_cast(caster, target, sk)\r\n\r\n    caster:Message(\"|WYou cast frost nova.|X\", 0, null);\r\n\r\n    target:AddSpellAffect(1, caster, \"Frost Nova\", false, false, 0, 8.0, 1, sk);\r\n\r\n    caster:EnterCombat(target);\r\n\r\n    caster:SetCooldown(sk, \"\", false, -1)\r\n\r\n    caster:AdjustMana(caster, -200)\r\n\r\n\r\n\r\nend\r\n\r\n\r\n','function skill_frost_nova_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by your frost nova.\\n\\r\");\r\n\r\n    target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s frost nova.|X\\n\\r\");\r\n\r\n    caster:Message(\"|W\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s frost nova.|X\", 5, target);\r\n\r\n    affect:ApplyAura(1, -100); --Movement speed minus 100%\r\n\r\nend\r\n\r\n','','function skill_frost_nova_remove(caster, target, affect)\r\n\r\n    target:Send(\"Frost nova fades from you.\\n\\r\");\r\n\r\n    if(caster ~= nil) then\r\n\r\n        caster:Send(\"Your frost nova fades from \" .. target:GetName() .. \".\\n\\r\");\r\n\r\n\r\n\r\n        caster:Message(caster:GetName() .. \"\'s frost nova fades from \" .. target:GetName() .. \".\", 5, target);\r\n\r\n    else\r\n\r\n        target:Message(affect:GetCasterName() .. \"\'s frost nova fades from \" .. target:GetName() .. \".\", 5, nil)\r\n\r\n    end\r\n\r\nend\r\n\r\n','function skill_frost_nova_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\n\r\nend\r\n\r\n'),(5,'Renew','renew',0,4,'Heals the target instantly for 62% of Spell Power, and then 310% of Spell Power over 15 seconds','2% of mana',0,'0;','skill_renew','function skill_renew_cast(caster, target, sk)\n\r    caster:ConsumeMana(math.ceil(caster:GetMaxMana() * .02))\n\r    local healvalue = math.ceil(0.62 * caster:GetIntellect())\n\r    if(caster:GetName() ~= target:GetName()) then\n\r        caster:Message(\"|WYou cast renew on \" .. target:GetName() .. \". Your renew heals \" .. target:GetName() .. \" for \" .. healvalue .. \" health.|X\", MSG_CHAR, null)\n\r        target:Send(\"|W\" .. caster:GetName() .. \" casts renew on you. \" .. caster:GetName() .. \"\'s renew restores \" .. healvalue .. \" health.|X\\n\\r\")\n\r    else\n\r        target:Send(\"|WYou cast renew. Your renew restores \" .. healvalue .. \" health.|X\\n\\r\")\n\r    end\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" casts renew on \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, target)\n\r    target:AdjustHealth(caster, healvalue)\n\r    target:AddSpellAffect(0, caster, \"Renew\", false, false, 5, 15.0, 1, sk, \"Healing \" .. healvalue .. \" health every 3 seconds\")\n\r    caster:SetCooldown(sk)\n\rend\n\r','function skill_renew_apply(caster, target, affect)\n\r    local healvalue = math.ceil(0.62 * caster:GetIntellect())\n\r    affect:SaveDataInt(\"healvalue\", healvalue)\n\r\n\r    if(caster:GetName() == target:GetName()) then\n\r        target:Send(\"|WYou are affected by renew.|X\\n\\r\")\n\r    else\n\r        caster:Send(\"|W\" .. target:GetName() .. \" is affected by your renew.|X\\n\\r\")\n\r        target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s renew.|X\\n\\r\")\n\r    end\n\rend\n\r','function skill_renew_tick(caster, target, affect)\n\r    local healvalue = affect:GetDataInt(\"healvalue\")\n\r    if(affect:GetCasterName() == target:GetName()) then\n\r        target:Send(\"|WYour renew restores \" .. healvalue .. \" health.|X\\n\\r\")\n\r    else\n\r        if(caster ~= nil) then\n\r            caster:Send(\"|WYour renew heals \" .. target:GetName() .. \" for \" .. healvalue .. \" health.|X\\n\\r\")\n\r        end\n\r        target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s renew restores \" .. healvalue .. \" health.|X\\n\\r\")\n\r    end\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s renew heals \" .. target:GetName() .. \" for \" .. healvalue .. \" health.|X\", MSG_ROOM_NOTCHARVICT, caster)\n\r    target:AdjustHealth(caster, healvalue)\n\rend\n\r','function skill_renew_remove(caster, target, affect)\n\r    if(affect:GetCasterName() == target:GetName()) then\n\r        target:Send(\"|WYour renew fades.|X\\n\\r\")\n\r    else\n\r        if(caster ~= nil) then\n\r            caster:Send(\"|WYour renew fades from \" .. target:GetName() .. \".|X\\n\\r\")\n\r        end\n\r        target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s renew fades from you.|X\\n\\r\")\n\r    end\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s renew fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, caster)\n\rend\n\r','function skill_renew_cost(caster, target, skill)\n\r    if(not caster:HasResource(RESOURCE_MANA, math.ceil(caster:GetMaxMana() * .02))) then\n\r        caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r        return 0\n\r    end\n\r    return 1\n\rend\n\r'),(6,'Hamstring','hamstring',0,2,'Movement speed reduced by 50%',NULL,0,NULL,'skill_hamstring','function skill_hamstring_cast(caster, target, sk)\r\n\r\n    target:AddSpellAffect(1, caster, \"Hamstring\", false, false, 0, 15.0, 0, sk)\r\n\r\n    caster:EnterCombat(target)\r\n\r\n    caster:AdjustStamina(caster, -(10 + (2*caster:GetPlayer():GetClassLevel(1))))\r\n\r\n    caster:SetCooldown(sk, \"\", false, 0)\r\n\r\nend\r\n\r\n','function skill_hamstring_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by your hamstring.\\n\\r\")\r\n\r\n    target:Send(\"You are affected by \" .. caster:GetName() .. \"\'s hamstring.\\n\\r\")\r\n\r\n    caster:Message(\"\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s hamstring.\", 5, target);\r\n\r\n    affect:ApplyAura(1, -50);\r\n\r\n\r\nend\r\n\r\n','','function skill_hamstring_remove(caster, target, affect)\r\n\r\n\r\n    target:Send(\"Hamstring fades from you.\\n\\r\");\r\n\r\n    caster:Send(\"Your hamstring fades from \" .. target:GetName() .. \".\\n\\r\");\r\n\r\n\r\n    caster:Message(caster:GetName() .. \"\'s hamstring fades from \" .. target:GetName() .. \".\", 5, target);\r\n\r\n\r\nend\r\n\r\n','function skill_hamstring_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(3, 10 + (2*caster:GetPlayer():GetClassLevel(1)))) then\r\n\r\n\r\n        caster:Send(\"You don\'t have enough stamina.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n\r\n'),(7,'Greater Heal','greater heal',0,4,'none',NULL,2.5,NULL,'skill_greater_heal','function skill_greater_heal_cast(caster, target, skill)\r\n\r\n    local manacost = 20 + (4*caster:GetPlayer():GetClassLevel(4));\r\n\r\n    local healamount = 100 + (15 * caster:GetPlayer():GetClassLevel(4));\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n    caster:Message(\"You have completed your spell.\", 0, null);\r\n\r\n    caster:Message(\"\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.\", 3, null);\r\n\r\n\r\n\r\n    if(caster:GetName() == target:GetName()) then\r\n\r\n        caster:Send(\"You heal yourself for \" .. healamount .. \" health.\\n\\r\");\r\n\r\n\r\n    else\r\n\r\n        caster:Send(\"You heal \" .. target:GetName() .. \" for \" .. healamount .. \" health.\\n\\r\");\r\n\r\n        target:Send(caster:GetName() .. \"\'s heal restores \" .. healamount .. \" health.\\n\\r\");\r\n\r\n\r\n    end\r\n\r\n\r\n    caster:Message(caster:GetName() .. \"\'s spell heals \" .. target:GetName() .. \" for \" .. healamount .. \" health.\", 5, target);\r\n\r\n\r\n    target:AdjustHealth(caster, healamount);\r\n\r\n\r\n    caster:SetCooldown(skill, \"\", false, 0)\r\n\r\n    caster:ConsumeMana(manacost);\r\n\r\n\r\n\r\n\r\nend\r\n\r\n\r\n','','','','function skill_greater_heal_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 20 + (4*caster:GetPlayer():GetClassLevel(4)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(8,'Zot','zot',0,3,'Health, Mana, Stamina = 1',NULL,0,NULL,'skill_zot','function skill_zot_cast(caster, target, skill)\r\n\r\n    caster:Send(\"|YYou send a bolt of lightning to strike \" .. target:GetName() .. \".|X\\n\\r\");\r\n\r\n\r\n    target:Send(\"|YZOT! You are struck by a bolt of lightning!|X\\n\\r\");\r\n\r\n    target:Message(\"|YZOT! \" .. target:GetName() .. \" is struck by a bolt of lightning!|X\", 3, null);\r\n\r\n\r\n    target:AdjustHealth(caster, -(target:GetHealth() - 1));\r\n\r\n    target:AdjustMana(caster, -(target:GetMana() - 1));\r\n\r\n    target:AdjustStamina(caster, -(target:GetStamina() - 1));\r\n\r\nend\r\n','','','','function skill_zot_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(9,'Sprint','sprint',60,0,'Increases your movement speed by 70% for 8 seconds',NULL,0,NULL,'skill_sprint','function skill_sprint_cast(caster, target, sk)\r\n\r\n    caster:Message(\"You cast sprint.\", 0, null);\r\n\r\n    caster:Message(\"\" .. caster:GetName() .. \" casts sprint.\", 3, caster);\r\n\r\n    caster:AddSpellAffect(0, caster, \"Sprint\", false, false, 0, 8, 0, sk);\r\n\r\n    caster:SetCooldown(sk, \"\", false, -1);\r\n\r\n\r\nend\r\n\r\n','function skill_sprint_apply(caster, target, affect)\r\n\r\n    affect:ApplyAura(1, 70);\r\n\r\nend\r\n\r\n','','function skill_sprint_remove(caster, target, affect)\r\n\r\n    target:Send(\"You are no longer sprinting.\\n\\r\");\r\n\r\n    target:Message(affect:GetCasterName() .. \"\'s sprint fades from \" .. affect:GetCasterName() .. \".\", 3, target)\r\n\r\n\r\nend\r\n\r\n','function skill_sprint_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(10,'Lucifron\'s Curse','lucifrons curse',0,2,'Spell and ability costs are increased by 100%',NULL,0,NULL,'skill_lucifrons_curse','function skill_lucifrons_curse_cast(caster, target, skill)\r\n\r\n    caster:Message(\"You cast Lucifron\'s Curse.\", 0, null);\r\n\r\n    target:AddSpellAffect(1, caster, \"Lucifron\'s Curse\", false, false, 0, 300, 2, skill);\r\n\r\nend\r\n\r\n','function skill_lucifrons_curse_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by Lucifron\'s curse.\\n\\r\")\r\n\r\n    target:Send(\"You are affected by Lucifron\'s curse.\\n\\r\")\r\n\r\n    caster:Message(\"\" .. target:GetName() .. \" is affected by Lucifron\'s curse.\", 5, target);\r\n\r\n    affect:ApplyAura(2, 100);\r\n\r\nend\r\n\r\n','','function skill_lucifrons_curse_remove(caster, target, affect)\r\n\r\n\r\n\r\n    target:Send(\"Lucifron\'s curse fades from you.\\n\\r\");\r\n\r\n    caster:Send(\"Lucifron\'s curse fades from \" .. target:GetName() .. \".\\n\\r\");\r\n\r\n    caster:Message(\"Lucifron\'s curse fades from \" .. target:GetName() .. \".\", 5, target);\r\n\r\nend\r\n\r\n','function skill_lucifrons_curse_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(11,'Impending Doom','impending doom',0,2,'2000 Shadow damage inflicted after 10 sec.',NULL,0,NULL,'skill_impending_doom','function skill_impending_doom_cast(caster, target, skill)\r\n\r\n    caster:EnterCombat(target)\r\n\r\n    caster:Message(\"You cast Impending Doom.\", 0, null);\r\n\r\n    target:AddSpellAffect(1, caster, \"Impending Doom\", false, false, 1, 10, 1, skill);\r\n\r\nend\r\n\r\n','function skill_impending_doom_apply(caster, target, affect)\r\n\r\n    caster:Send(target:GetName() .. \" is affected by Impending Doom.\\n\\r\")\r\n\r\n    target:Send(\"You are affected by Impending Doom.\\n\\r\")\r\n\r\n    caster:Message(\"\" .. target:GetName() .. \" is affected by Impending Doom.\", 5, target);\r\n\r\nend\r\n\r\n','function skill_impending_doom_tick(caster, target, affect)\r\n\r\n    target:AdjustHealth(caster, -2000)\r\n\r\nend\r\n\r\n','function skill_impending_doom_remove(caster, target, affect)\r\n\r\n    target:Send(\"Impending Doom fades from you.\\n\\r\");\r\n\r\n    target:Message(\"Impending Doom fades from \" .. target:GetName() .. \".\", 3, target);\r\n\r\nend\r\n\r\n','function skill_impending_doom_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(12,'Remove Curse','remove curse',5,4,'Removes all curses from a friendly target',NULL,0,NULL,'skill_remove_curse','function skill_remove_curse_cast(caster, target, skill)\r\n\r\n    local removed = target:CleanseSpellAffect(caster, 2, -1);\r\n\r\n    caster:Send(\"Removed \" .. removed .. \" curses.\\n\\r\");\r\n\r\n    if(caster:GetName() ~= target:GetName()) then\r\n\r\n        target:Send(\"Removed \" .. removed .. \" curses.\\n\\r\");\r\n\r\n    end\r\n\r\nend\r\n\r\n','','','','function skill_remove_curse_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 250 + math.ceil(1.5*caster:GetPlayer():GetClassLevel(3)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(13,'Purify','purify',5,4,'Dispels harmful magic and disease affects.',NULL,0,NULL,'skill_purify','function skill_purify_cast(caster, target, skill)\r\n\r\n    local magic = target:CleanseSpellAffect(caster, 1, -1);\r\n\r\n    local disease = target:CleanseSpellAffect(caster, 4, -1);\r\n\r\n    caster:Send(\"Removed \" .. magic .. \" magic affects.\\n\\r\");\r\n\r\n    caster:Send(\"Removed \" .. disease .. \" diseases.\\n\\r\");\r\n\r\n    if(caster:GetName() ~= target:GetName()) then\r\n\r\n        target:Send(\"Removed \" .. magic .. \" magic affects.\\n\\r\");\r\n\r\n        target:Send(\"Removed \" .. disease .. \" diseases.\\n\\r\");\r\n\r\n    end\r\n\r\nend\r\n','','','','function skill_purify_cost(caster, target, skill)\r\n\r\n    if(not caster:HasResource(2, 150 + math.ceil(1.75*caster:GetPlayer():GetClassLevel(4)))) then\r\n\r\n        caster:Send(\"You don\'t have enough mana.\\n\\r\");\r\n\r\n        return 0;\r\n\r\n    end\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(14,'Slow','slow',0,2,'Movement speed decreased by 80%, gradually increasing over spell duration',NULL,0,NULL,'skill_slow','function skill_slow_cast(caster, target, sk)\r\n\r\n    caster:Message(\"You cast slow on \" .. target:GetName() .. \".\", 0, null);\r\n\r\n    caster:Message(\"\" .. caster:GetName() .. \" casts slow on \" .. target:GetName() .. \".\", 3, caster);\r\n\r\n    target:AddSpellAffect(1, caster, \"Slow\", false, false, 5, 10, 0, sk);\r\n\r\n    caster:SetCooldown(sk, \"\", false, -1);\r\n\r\n    target:Send(\"You are affected by \" .. caster:GetName() .. \"\'s slow.\");\r\n\r\n    caster:EnterCombat(target);\r\n\r\n    caster:ConsumeMana(25)\r\n\r\nend\r\n\r\n\r\n\r\n','function skill_slow_apply(caster, target, affect)\r\n\r\n    affect:ApplyAura(1, -80);\r\n\r\n\r\nend\r\n\r\n','function skill_slow_tick(caster, target, affect)\r\n\r\n    -- need a RemoveAura function in affect\r\n\r\n    --need a GetRemainingDuration function in affect\r\n\r\n    -- need a GetCurrentTick() function in affect\r\n\r\n    -- todo: remove any movespeed aura, apply a movespeed of -80 + (15 * GetCurrentTick())\r\n\r\nend\r\n\r\n','function skill_slow_remove(caster, target, affect)\r\n\r\nend\r\n\r\n','function skill_slow_cost(caster, target, skill)\r\n\r\n    return 1;\r\n\r\nend\r\n\r\n'),(15,'Frostbolt','frostbolt',0,2,'Launches a bolt of frost at the enemy, causing 200% of Spell Power damage and slowing movement speed by 50% for 10 seconds','2% of mana',2,'0;','skill_frostbolt','function skill_frostbolt_cast(caster, target, sk)\n\r    caster:ConsumeMana(math.ceil(caster:GetMaxMana() * .02))\n\r    local damage = math.ceil(2 * caster:GetIntellect())\n\r    caster:Message(\"|WYou cast frostbolt on \" .. target:GetName() .. \". Your frostbolt hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_CHAR, null)\n\r    target:Send(\"|W\" .. caster:GetName() .. \" casts frostbolt on you. \" .. caster:GetName() .. \"\'s frostbolt hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" casts frostbolt on \" .. target:GetName() .. \". \" .. caster:GetName() .. \"\'s frostbolt hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r    target:AddSpellAffect(1, caster, \"Chilled\", false, false, 0, 10, AFFECT_MAGIC, sk, \"Movement speed reduced by 50%\")\n\r    caster:SetCooldown(sk, \"\", false, -1)\n\r    caster:EnterCombat(target)\n\r    target:AdjustHealth(caster, -damage)\n\rend\n\r','function skill_frostbolt_apply(caster, target, affect)\n\r        affect:ApplyAura(AURA_MOVE_SPEED, -50);\n\r        target:Send(\"|WYou are affected by Chilled.|X\\n\\r\")\n\r        target:Message(\"|W\" .. target:GetName() .. \" is affected by Chilled.|X\", MSG_ROOM_NOTCHAR, nil)\n\rend\n\r','','function skill_frostbolt_remove(caster, target, affect)\n\r    target:Send(\"|WChilled fades from you.|X\\n\\r\")\n\r    target:Message(\"|WChilled fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\rend\n\r','function skill_frostbolt_cost(caster, target, skill)\n\r    if(not caster:HasResource(RESOURCE_MANA, math.ceil(caster:GetMaxMana() * .02))) then\n\r        caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r        return 0\n\r    end\n\r    return 1\n\rend\n\r'),(16,'Slam','slam',0,2,'Slams an opponent, causing Physical damage','20 rage',0,'0;','skill_slam','','','','',''),(17,'Charge','',0,0,'',NULL,0,NULL,'','','','','',''),(18,'Victory Rush','',0,0,'',NULL,0,NULL,'','','','','',''),(19,'Execute','',0,0,'',NULL,0,NULL,'','','','','',''),(20,'Mortal Strike','',0,0,'',NULL,0,NULL,'','','','','',''),(21,'Sinister Strike','sinister strike',0,2,'An instant strike that causes Physical damage. Awards 1 combo point.',NULL,0,NULL,'skill_sinister_strike','function skill_sinister_strike_cast(caster, target, sk)\n\r    caster:EnterCombat(target)\n\r    caster:ConsumeEnergy(40)\n\r    caster:GenerateComboPoint(target)\n\r    caster:Send(\"|WYour Sinister Strike hits \" .. target:GetName() .. \" for 1 damage.|X\\n\\r\")\n\r    target:Send(\"|W\" .. caster:GetName() .. \"\'s Sinister Strike hits you for 1 damage.|X\\n\\r\")\n\r    caster:Message(\"|W\" .. caster:GetName() .. \"\'s Sinister Strike hits \" .. target:GetName() .. \" for 1 damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r\r\r    caster:SetCooldown(sk, \"\", false, 0)\n\r    target:AdjustHealth(caster, -1)\n\rend\n\r','','','','function skill_sinister_strike_cost(caster, target, skill)\n\r\n\r    if(not caster:HasResource(RESOURCE_ENERGY, 40)) then\n\r        caster:Send(\"You don\'t have enough energy.\\n\\r\")\n\r\n\r        return 0\n\r\n\r    end\n\r    return 1\n\r\n\rend\n\r'),(22,'Eviscerate','eviscerate',0,0,'','NULL',0,'0;','','','','','',''),(23,'Stealth','',0,0,'',NULL,0,NULL,'','','','','',''),(24,'Cheap Shot','',0,0,'',NULL,0,NULL,'','','','','',''),(25,'Backstab','',0,0,'',NULL,0,NULL,'','','','','',''),(26,'Smite','smite',0,2,'','NULL',2,'0;','skill_smite','','','','',''),(27,'Pain','',0,0,'',NULL,0,NULL,'','','','','',''),(28,'Flash Heal','',0,0,'',NULL,0,NULL,'','','','','',''),(29,'Shield','shield',0,4,'','NULL',0,'0;','skill_shield','','','','',''),(30,'Penance','',0,0,'',NULL,0,NULL,'','','','','',''),(31,'Fire Blast','',0,0,'',NULL,0,NULL,'','','','','',''),(32,'Ice Lance','ice lance',0,2,'','NULL',0,'0;','skill_ice_lance','','','','',''),(33,'Garrote','garrote',15,2,'Cause bleed damage over 18 seconds. Generates 1 combo point.',NULL,0,NULL,'skill_garrote','','','','',''),(34,'Bloodthirst','bloodthirst',4.5,2,'','Generates 20 rage',0,'0;','skill_bloodthirst','','','','',''),(35,'Rend','rend',0,2,'','30 rage',0,'0;','skill_rend','','','','',''),(36,'Mana Shield','mana shield',0,0,'','',0,'0;','','','','','','');
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

-- Dump completed on 2018-01-30  5:17:07
