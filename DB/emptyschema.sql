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
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-01-25  1:41:35
