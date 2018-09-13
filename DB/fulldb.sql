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
  `name` varchar(45) NOT NULL,
  `pvp` int(11) NOT NULL,
  `death_room` int(11) NOT NULL,
  `level_range_low` int(11) NOT NULL,
  `level_range_high` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `areas`
--

LOCK TABLES `areas` WRITE;
/*!40000 ALTER TABLE `areas` DISABLE KEYS */;
INSERT INTO `areas` VALUES (1,'Northshire',0,121,1,240),(2,'Elwynn Forest',0,0,1,240),(3,'Blackrock Depths',3,123,230,240);
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
INSERT INTO `class_skills` VALUES (1,1,1),(2,1,1),(3,1,1),(4,1,1),(4,2,1),(3,4,60),(4,5,2),(1,6,50),(4,7,180),(2,9,90),(3,15,1),(1,16,1),(2,21,1),(2,22,2),(4,26,1),(4,29,5),(3,32,2),(2,33,5),(1,34,2),(1,35,5),(3,36,5),(1,40,1);
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
  `armor` tinytext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `classes`
--

LOCK TABLES `classes` WRITE;
/*!40000 ALTER TABLE `classes` DISABLE KEYS */;
INSERT INTO `classes` VALUES (1,'Warrior','|M','3,1;4,1;5,1;6,1;19,10;18,10;','0,1;1,60;2,120;3,180;'),(2,'Rogue','|Y','11,1;12,1;13,1;14,1;19,10;18,10;','0,1;1,120;'),(3,'Mage','|C','7,1;8,1;9,1;10,1;19,10;18,10;','0,1;'),(4,'Cleric','|W','15,1;16,1;17,1;10,1;19,10;18,10;','0,1;');
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
INSERT INTO `exits` VALUES (2,0,3),(2,2,8),(2,4,9),(2,6,4),(3,1,11),(3,4,2),(3,7,10),(4,0,122),(4,2,2),(4,4,18),(4,6,5),(5,0,6),(5,2,4),(5,4,24),(5,6,7),(6,0,34),(6,2,122),(6,4,5),(6,6,32),(7,0,32),(7,2,5),(7,4,25),(7,6,31),(8,0,121),(8,2,81),(8,4,64),(8,6,2),(9,0,2),(9,2,64),(9,4,19),(9,6,18),(10,0,13),(10,2,12),(10,3,3),(11,0,14),(11,5,3),(11,6,12),(12,0,15),(12,2,11),(12,6,10),(13,4,10),(14,4,11),(15,4,12),(15,8,16),(16,8,17),(16,9,15),(17,9,16),(18,0,4),(18,2,9),(18,4,26),(18,6,24),(19,0,9),(19,2,65),(19,4,20),(19,6,26),(20,0,19),(20,3,21),(20,5,22),(21,5,23),(21,7,20),(22,1,20),(22,3,23),(23,1,21),(23,7,22),(24,0,5),(24,2,18),(24,4,27),(24,6,25),(25,0,7),(25,2,24),(25,4,28),(25,6,29),(26,0,18),(26,2,19),(26,6,27),(27,0,24),(27,2,26),(27,6,28),(28,0,25),(28,2,27),(28,6,30),(29,0,31),(29,2,25),(29,4,30),(30,0,29),(30,2,28),(31,0,33),(31,2,7),(31,4,29),(32,0,35),(32,2,6),(32,4,7),(32,6,33),(33,0,36),(33,2,32),(33,4,31),(34,0,37),(34,4,6),(34,6,35),(35,0,38),(35,2,34),(35,4,32),(35,6,36),(36,0,39),(36,2,35),(36,4,33),(37,0,41),(37,4,34),(37,6,38),(38,0,42),(38,2,37),(38,4,35),(38,6,39),(39,0,43),(39,2,38),(39,4,36),(39,6,40),(40,0,44),(40,2,39),(41,0,45),(41,2,51),(41,4,37),(41,6,42),(42,0,46),(42,2,41),(42,4,38),(42,6,43),(43,0,47),(43,2,42),(43,4,39),(43,6,44),(44,0,48),(44,2,43),(44,4,40),(45,0,74),(45,2,72),(45,4,41),(45,6,46),(46,0,73),(46,2,45),(46,4,42),(46,6,47),(47,0,67),(47,2,46),(47,4,43),(47,6,48),(48,0,49),(48,2,47),(48,4,44),(49,0,50),(49,2,67),(49,4,48),(50,0,109),(50,4,49),(51,0,72),(51,2,52),(51,6,41),(52,0,71),(52,2,53),(52,6,51),(53,0,70),(53,2,54),(53,6,52),(54,0,69),(54,2,55),(54,4,84),(54,6,53),(55,0,68),(55,2,56),(55,4,87),(55,6,54),(56,0,66),(56,2,99),(56,4,57),(56,6,55),(57,0,56),(57,2,98),(57,4,58),(57,6,87),(58,0,57),(58,2,91),(58,4,59),(58,6,86),(59,0,58),(59,2,88),(59,4,60),(59,6,85),(60,0,59),(60,2,89),(60,4,61),(60,6,80),(61,0,60),(61,2,90),(61,6,62),(62,0,80),(62,2,61),(62,6,63),(63,0,81),(63,2,62),(63,6,64),(64,0,8),(64,2,63),(64,4,65),(64,6,9),(65,0,64),(65,6,19),(66,4,56),(66,6,68),(67,2,73),(67,4,47),(67,6,49),(68,0,79),(68,2,66),(68,4,55),(68,6,69),(69,0,78),(69,2,68),(69,4,54),(69,6,70),(70,0,77),(70,2,69),(70,4,53),(70,6,71),(71,0,76),(71,2,70),(71,4,52),(71,6,72),(72,0,75),(72,2,71),(72,4,51),(72,6,45),(73,2,74),(73,4,46),(73,6,67),(74,2,75),(74,4,45),(74,6,73),(75,2,76),(75,4,72),(75,6,74),(76,2,77),(76,4,71),(76,6,75),(77,2,78),(77,4,70),(77,6,76),(78,2,79),(78,4,69),(78,6,77),(79,4,68),(79,6,78),(80,0,85),(80,2,60),(80,4,62),(80,6,81),(81,0,82),(81,2,80),(81,4,63),(81,6,8),(82,0,83),(82,2,85),(82,4,81),(82,6,121),(83,0,84),(83,2,86),(83,4,82),(84,0,54),(84,2,87),(84,4,83),(85,0,86),(85,2,59),(85,4,80),(85,6,82),(86,0,87),(86,2,58),(86,4,85),(86,6,83),(87,0,55),(87,2,57),(87,4,86),(87,6,84),(88,0,91),(88,2,107),(88,4,89),(88,6,59),(89,0,88),(89,2,105),(89,4,90),(89,6,60),(90,0,89),(90,2,100),(90,6,61),(91,0,98),(91,2,92),(91,4,88),(91,6,58),(92,0,96),(92,2,93),(92,4,107),(92,6,91),(93,0,95),(93,2,94),(93,4,108),(93,6,92),(94,4,104),(94,6,93),(95,4,93),(95,6,96),(96,0,97),(96,2,95),(96,4,92),(96,6,98),(97,4,96),(97,6,99),(98,0,99),(98,2,96),(98,4,91),(98,6,57),(99,2,97),(99,4,98),(99,6,56),(100,0,105),(100,2,101),(100,6,90),(101,0,106),(101,2,102),(101,6,100),(102,0,103),(102,6,101),(103,0,104),(103,4,102),(103,6,106),(104,0,94),(104,4,103),(104,6,108),(105,0,107),(105,2,106),(105,4,100),(105,6,89),(106,0,108),(106,2,103),(106,4,101),(106,6,105),(107,0,92),(107,2,108),(107,4,105),(107,6,88),(108,0,93),(108,2,104),(108,4,106),(108,6,107),(109,0,112),(109,4,50),(109,6,110),(110,2,109),(110,6,111),(111,2,110),(112,4,109),(112,7,113),(113,1,114),(113,3,112),(113,6,115),(114,5,113),(114,9,116),(115,0,117),(115,2,113),(116,8,114),(117,1,118),(117,4,115),(118,5,117),(118,9,119),(119,0,120),(119,8,118),(120,4,119),(121,2,82),(121,4,8),(122,4,4),(122,6,6),(123,0,124),(123,2,125),(123,4,126),(124,0,127),(124,4,123),(125,2,131),(125,6,123),(126,0,123),(127,0,128),(127,2,129),(127,4,124),(128,0,132),(128,1,133),(128,4,127),(128,7,134),(129,2,130),(129,6,127),(130,6,129),(131,6,125),(132,2,133),(132,4,128),(132,6,134),(133,5,128),(133,6,132),(134,2,132),(134,3,128);
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
  `inroom_name` varchar(100) NOT NULL,
  `item_level` int(11) NOT NULL,
  `char_level` int(11) NOT NULL,
  `equip_location` int(11) NOT NULL,
  `quality` int(11) NOT NULL,
  `binds` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `skill_id` int(11) NOT NULL,
  `quest` int(11) NOT NULL,
  `armor` int(11) NOT NULL,
  `durability` int(11) NOT NULL,
  `unique` int(11) NOT NULL,
  `damage_low` int(11) NOT NULL,
  `damage_high` int(11) NOT NULL,
  `value` int(11) NOT NULL,
  `speed` decimal(10,1) NOT NULL,
  `agility` int(11) NOT NULL,
  `intellect` int(11) NOT NULL,
  `strength` int(11) NOT NULL,
  `stamina` int(11) NOT NULL,
  `wisdom` int(11) NOT NULL,
  `spirit` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `items`
--

LOCK TABLES `items` WRITE;
/*!40000 ALTER TABLE `items` DISABLE KEYS */;
INSERT INTO `items` VALUES (1,'The One Ring','one ring','',15,10,11,2,2,13,0,0,0,0,0,0,0,140,0.0,0,0,0,0,0,0),(2,'Test Item of Power','test item power','',5,2,1,4,1,0,0,0,100,100,1,0,0,1000,0.0,0,0,0,0,0,0),(3,'Worn Greatsword','','',1,1,16,1,0,4,0,0,0,25,0,2,3,4,2.4,0,0,0,0,0,0),(4,'Recruit\'s Vest','','',1,1,5,1,0,0,0,0,3,55,0,0,0,2,0.0,0,0,0,0,0,0),(5,'Recruit\'s Pants','','',1,1,9,1,0,0,0,0,3,40,0,0,0,2,0.0,0,0,0,0,0,0),(6,'Recruit\'s Boots','','',1,1,10,1,0,0,0,0,2,25,0,0,0,1,0.0,0,0,0,0,0,0),(7,'Apprentice\'s Boots','','',1,1,10,1,0,0,0,0,2,25,0,0,0,2,0.0,0,0,0,0,0,0),(8,'Apprentice\'s Robe','','',1,1,5,1,0,0,0,0,3,55,0,0,0,2,0.0,0,0,0,0,0,0),(9,'Apprentice\'s Pants','','',1,1,9,1,0,0,0,0,3,40,0,0,0,2,0.0,0,0,0,0,0,0),(10,'Bent Staff','','',1,1,16,1,0,9,0,0,0,25,0,2,3,4,2.3,0,0,0,0,0,0),(11,'Footpad\'s Pants','','',1,1,9,1,0,0,0,0,2,40,0,0,0,2,0.0,0,0,0,0,0,0),(12,'Footpad\'s Vest','','',1,1,5,1,0,0,0,0,3,55,0,0,0,2,0.0,0,0,0,0,0,0),(13,'Footpad\'s Shoes','','',1,1,10,1,0,0,0,0,3,25,0,0,0,1,0.0,0,0,0,0,0,0),(14,'Worn Shortsword','worn shortsword sword','',1,1,14,1,0,4,0,0,0,20,0,1,3,3,1.9,0,0,0,0,0,0),(15,'Neophyte\'s Pants','','',1,1,9,1,0,0,0,0,3,40,0,0,0,2,0.0,0,0,0,0,0,0),(16,'Neophyte\'s Boots','','',1,1,10,1,0,0,0,0,2,25,0,0,0,2,0.0,0,0,0,0,0,0),(17,'Neophyte\'s Robe','','',1,1,5,1,0,0,0,0,3,55,0,0,0,2,0.0,0,0,0,0,0,0),(18,'Refreshing Spring Water','','',1,1,0,1,0,11,38,0,0,0,0,0,0,1,0.0,0,0,0,0,0,0),(19,'Shiny Red Apple','','',1,1,0,1,0,11,37,0,0,0,0,0,0,1,0.0,0,0,0,0,0,0),(20,'Tough Wolf Meat','','',1,0,0,1,0,13,0,1,0,0,0,0,0,0,0.0,0,0,0,0,0,0),(21,'Red Burlap Bandana','','',1,0,0,1,0,13,0,1,0,0,0,0,0,0,0.0,0,0,0,0,0,0),(22,'Milly\'s Harvest','milly harvest grapes bucket','A large bucket of grapes is here.',1,0,0,1,0,13,0,1,0,0,0,0,0,0,0.0,0,0,0,0,0,0),(23,'Grape Manifest','','',1,1,0,1,1,13,0,1,0,0,1,0,0,0,0.0,0,0,0,0,0,0),(24,'Marshal McBride\'s Documents','','',1,1,0,1,1,13,0,1,0,0,1,0,0,0,0.0,0,0,0,0,0,0),(25,'Garrick\'s Head','','',1,1,0,1,1,13,0,1,0,0,1,0,0,0,0.0,0,0,0,0,0,0),(26,'Soft Fur-lined Shoes','soft fur-lined shoes','',5,1,10,1,1,0,0,0,7,20,0,0,0,7,0.0,0,0,0,0,0,0),(27,'Wolfskin Bracers','wolfskin bracers','',5,1,6,2,1,0,0,0,7,16,0,0,0,6,0.0,0,0,0,0,0,0),(28,'Morning Glory Dew','','',230,1,0,1,0,11,38,0,0,0,0,0,0,2,0.0,0,0,0,0,0,0),(29,'Lagrave\'s Seal','ring lagraves seal','',234,1,11,2,1,13,0,0,0,0,0,0,0,90,0.0,0,7,0,7,0,7),(30,'Essence of the Elements','','',1,1,0,1,0,13,0,1,0,0,0,0,0,0,0.0,0,0,0,0,0,0),(31,'Burning Pitch','','',1,1,0,0,0,13,0,0,0,0,0,0,0,5,0.0,0,0,0,0,0,0),(32,'Sunborne Cape','','',236,1,4,2,1,0,0,0,36,0,0,0,0,104,0.0,0,3,0,0,0,5),(33,'Nightfall Gloves','','',236,1,7,2,1,1,0,0,91,30,0,0,0,87,0.0,12,0,12,0,0,0),(34,'Crypt Demon Bracers','','',236,1,6,2,1,2,0,0,132,35,0,0,0,104,0.0,14,0,0,0,0,0),(35,'Stalwart Clutch','','',236,1,8,2,1,3,0,0,300,40,0,0,0,70,0.0,12,0,0,12,0,0),(36,'Arcanite Reaper','axe arcanite reaper','',241,238,16,3,2,7,0,0,0,100,1,153,256,730,3.8,0,0,0,13,0,0),(37,'Golem Skull Helm','','',234,225,1,3,1,3,0,0,447,80,1,0,0,118,0.0,0,0,18,18,0,0),(38,'Ebonsteel Spaulders','','',236,226,3,3,1,3,0,0,553,80,0,0,0,143,0.0,7,0,6,16,0,0),(39,'Verek\'s Collar','','',233,224,2,3,1,13,0,0,0,0,0,0,0,146,0.0,0,0,6,7,0,0),(40,'The Emperor\'s New Cape','','',240,230,4,3,1,0,0,0,43,0,0,0,0,145,0.0,7,0,0,16,0,0),(41,'Savage Gladiator Chain','','',234,223,5,4,1,2,0,0,369,140,0,0,0,335,0.0,14,0,13,13,0,0),(42,'Emberplate Armguards','','',236,224,6,3,1,3,0,0,261,45,0,0,0,86,0.0,0,0,10,10,0,0),(43,'Stonegrip Gauntlets','','',238,230,7,3,2,3,0,0,392,45,0,0,0,104,0.0,0,0,9,14,0,0),(44,'Warmaster Legguards','','',241,237,9,3,1,3,0,0,575,100,0,0,0,231,0.0,0,0,13,15,0,0),(45,'Shalehusk Boots','','',236,224,10,3,1,3,0,0,417,65,0,0,0,137,0.0,0,0,0,5,0,0),(46,'Rock Golem Bulwark','','',236,222,13,3,1,14,0,0,1994,100,0,0,0,290,0.0,0,0,0,10,0,0),(47,'Hurley\'s Tankard','','',234,222,14,3,1,6,0,0,0,90,0,71,132,414,2.7,5,0,0,12,0,0);
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
INSERT INTO `npc_drops` VALUES (1,'1;',100),(1,'2;',100),(1,'3;',100),(1,'10;',100),(1,'18;',100),(1,'20;',100),(1,'36;35;34;33;',100),(2,'20;',100),(3,'20;',100),(18,'25;',100),(22,'21;',100),(23,'21;',100),(24,'28;',2),(25,'30;',100),(25,'31;',38),(28,'30;',100),(29,'28;',2);
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
INSERT INTO `npc_skills` VALUES (1,10);
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
  `health` int(11) NOT NULL,
  `mana` int(11) NOT NULL,
  `energy` int(11) NOT NULL,
  `rage` int(11) NOT NULL,
  `title` varchar(45) NOT NULL,
  `attack_speed` double NOT NULL,
  `damage_low` int(11) NOT NULL,
  `damage_high` int(11) NOT NULL,
  `flags` tinytext NOT NULL,
  `speechtext` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `npcs`
--

LOCK TABLES `npcs` WRITE;
/*!40000 ALTER TABLE `npcs` DISABLE KEYS */;
INSERT INTO `npcs` VALUES (1,'The Lurker Below','',240,1,0,10000,50,0,0,'',2.5,10,20,'1;',''),(2,'Diseased Young Wolf','',1,1,0,18,0,0,0,'',2,2,3,'1;',''),(3,'Diseased Timber Wolf','',1,1,0,18,0,0,0,'',2,2,3,'1;',''),(4,'Marshal McBride','',20,1,1,50,50,0,0,'',2,1,1,'0;','Hey, citizen! You look like a stout one. We guards are spread a little thin out here, and I could use your help...'),(5,'Eagan Peltskinner','',3,1,1,50,50,0,0,'',2,1,1,'0;',''),(6,'Deputy Willem','',18,1,1,50,50,0,0,'',2,1,1,'0;','Normally I\'d be out on the beat looking after the folk of Stormwind, but a lot of the Stormwind guards are fighting in the other lands. So here I am, deputized and offering bounties when I\'d rather be on patrol...'),(7,'Milly Osworth','',2,1,1,50,50,0,0,'',2,1,1,'0;',''),(8,'Kobold Vermin','',2,1,0,20,20,0,0,'',2,2,4,'1;',''),(9,'Kobold Worker','',3,1,0,60,20,0,0,'',2,5,7,'1;',''),(10,'Kobold Laborer','',5,1,0,100,20,0,0,'',2,9,11,'1;',''),(11,'Falkhaan Isenstrider','',10,1,1,50,50,0,0,'',2,1,1,'0;',''),(12,'Brother Neals','',50,1,1,50,50,0,0,'',2,1,1,'0;',''),(13,'Janos Hammerknuckle','',5,1,1,50,50,0,0,'Weaponsmith',2,1,1,'0;4;5;',''),(14,'Godric Rothgar','',5,1,1,50,50,0,0,'Armorer & Shieldcrafter',2,1,1,'0;4;5;',''),(15,'Brother Danil','',5,1,1,50,50,0,0,'General Supplies',2,1,1,'0;4;',''),(16,'Dermot Johns','',5,1,1,50,50,0,0,'Cloth & Leather Armor Merchant',2,1,1,'0;4;5;',''),(17,'Marshal Dughan','',1,1,0,50,50,100,0,'',2,1,1,'0;','Ach, it\'s hard enough keeping order around here without all these new troubles popping up! I hope you have good news...'),(18,'Garrick Padfoot','',5,1,0,50,50,100,0,'',2,12,14,'2;',''),(19,'Innkeeper Farley','',30,1,0,50,50,100,0,'',2,1,1,'0;','Welcome to my Inn, weary traveler. What can I do for you?'),(20,'Brother Paxton','',5,1,0,50,50,100,0,'',2,1,1,'0;',''),(21,'Brother Sammuel','',5,1,0,50,50,100,0,'',2,1,1,'0;',''),(22,'Defias Thug','',4,1,0,100,50,100,0,'',2,9,11,'2;',''),(23,'Defias Thug','',3,2,0,60,50,100,0,'',2,6,8,'2;',''),(24,'Anvilrage Overseer','',229,1,0,6500,50,100,100,'',1.26,269,346,'2;',''),(25,'Blazing Fireguard','',233,1,0,5900,3900,100,100,'',1.22,198,256,'2;',''),(26,'Shadowmage Vivian Lagrave','',238,2,0,4484,2289,100,100,'',2,106,136,'0;',''),(27,'Jalinda Sprig','',237,2,0,4356,5340,100,100,'',2,102,131,'0;',''),(28,'Lord Incendius','',235,1,0,15000,50,100,100,'',1.2,449,533,'2;',''),(29,'Anvilrage Soldier','',233,1,0,7300,50,100,100,'',3.2,1000,1100,'2;',''),(30,'Anvilrage Medic','',232,1,0,5000,9000,100,100,'',2.6,441,534,'2;',''),(31,'Anvilrage Officer','',233,1,0,6000,4000,100,100,'',2.6,689,760,'2;',''),(32,'Anvilrage Footman','',232,1,0,6900,50,100,100,'',2,375,462,'2;','');
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
INSERT INTO `player_active_quests` VALUES ('Bobo',1,'4'),('Bobo',4,'8'),('Boris',15,'5'),('Boris',16,'0'),('Maikeru',5,'1'),('Momo',1,'0'),('Momo',4,'0'),('Tanktest',1,'0'),('Tanktest',4,'8'),('Tanktest',5,'0');
/*!40000 ALTER TABLE `player_active_quests` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `player_alias`
--

DROP TABLE IF EXISTS `player_alias`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `player_alias` (
  `player` varchar(12) NOT NULL,
  `word` varchar(25) NOT NULL,
  `substitution` varchar(512) NOT NULL,
  PRIMARY KEY (`player`,`word`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_alias`
--

LOCK TABLES `player_alias` WRITE;
/*!40000 ALTER TABLE `player_alias` DISABLE KEYS */;
INSERT INTO `player_alias` VALUES ('Bobo','evis','cast eviscerate'),('Bobo','ss','cast \'sinister strike\''),('Boris','evis','cast eviscerate'),('Boris','ff','cast \'frostbolt 1\''),('Boris','gh','cast \'greater heal 1\''),('Boris','ss','cast \'sinister strike\''),('Castertest','gh','cast \'greater heal 1\''),('Dpstest','ss','c \'sinister strike\''),('Tanktest','ss','c \'sinister strike\'');
/*!40000 ALTER TABLE `player_alias` ENABLE KEYS */;
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
INSERT INTO `player_class_data` VALUES ('Bob',1,40),('Bob',2,30),('Bob',3,81),('Bob',4,155),('Bobo',2,2),('Boris',1,59),('Boris',2,60),('Boris',3,60),('Boris',4,60),('Castertest',3,60),('Castertest',4,180),('Dpstest',2,120),('Dpstest',3,120),('Maikeru',2,2),('Momo',1,1),('Tanktest',1,180),('Tanktest',2,60),('Tash',1,2);
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
INSERT INTO `player_completed_quests` VALUES ('Bobo',6),('Bobo',8),('Boris',6),('Boris',8),('Maikeru',1),('Maikeru',4),('Maikeru',6),('Maikeru',8),('Momo',6),('Momo',8),('Tanktest',6),('Tanktest',8),('Tash',1),('Tash',6);
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
  `count` int(11) NOT NULL,
  `location` int(11) NOT NULL,
  KEY `player` (`player`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_inventory`
--

LOCK TABLES `player_inventory` WRITE;
/*!40000 ALTER TABLE `player_inventory` DISABLE KEYS */;
INSERT INTO `player_inventory` VALUES ('Tash',4,1,0),('Tash',5,1,0),('Tash',6,1,0),('Tash',3,1,0),('Tash',19,1,1),('Tash',19,1,1),('Tash',19,1,1),('Tash',19,1,1),('Tash',19,1,1),('Maikeru',12,1,0),('Maikeru',27,1,0),('Maikeru',11,1,0),('Maikeru',13,1,0),('Maikeru',14,1,0),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',19,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',18,1,1),('Maikeru',21,1,1),('Bob',4,1,0),('Bob',5,1,0),('Bob',6,1,0),('Bob',3,1,0),('Bob',21,8,1),('Bob',20,2,1),('Bob',22,3,1),('Momo',4,1,0),('Momo',5,1,0),('Momo',6,1,0),('Momo',3,1,0),('Momo',18,10,1),('Momo',19,10,1),('Bobo',2,1,0),('Bobo',12,1,0),('Bobo',11,1,0),('Bobo',13,1,0),('Bobo',14,1,0),('Bobo',10,1,1),('Bobo',3,1,1),('Bobo',20,11,1),('Bobo',18,11,1),('Bobo',19,10,1),('Castertest',17,1,0),('Castertest',15,1,0),('Castertest',16,1,0),('Castertest',36,1,0),('Castertest',33,1,1),('Castertest',10,3,1),('Castertest',3,2,1),('Castertest',31,1,1),('Castertest',1,5,1),('Castertest',18,12,1),('Castertest',19,10,1),('Castertest',20,2,1),('Castertest',2,3,1),('Dpstest',8,1,0),('Dpstest',9,1,0),('Dpstest',7,1,0),('Dpstest',10,1,0),('Dpstest',35,1,1),('Dpstest',10,1,1),('Dpstest',3,1,1),('Dpstest',2,1,1),('Dpstest',1,1,1),('Dpstest',18,11,1),('Dpstest',19,10,1),('Boris',37,1,0),('Boris',39,1,0),('Boris',38,1,0),('Boris',40,1,0),('Boris',41,1,0),('Boris',42,1,0),('Boris',43,1,0),('Boris',35,1,0),('Boris',44,1,0),('Boris',45,1,0),('Boris',1,1,0),('Boris',1,1,0),('Boris',46,1,0),('Boris',47,1,0),('Boris',31,1,1),('Boris',36,1,1),('Boris',3,1,1),('Boris',2,1,1),('Boris',18,4,1),('Boris',20,1,1),('Boris',30,5,1),('Tanktest',2,1,0),('Tanktest',4,1,0),('Tanktest',5,1,0),('Tanktest',6,1,0),('Tanktest',1,1,0),('Tanktest',1,1,0),('Tanktest',36,1,0),('Tanktest',36,1,1),('Tanktest',2,3,1),('Tanktest',1,2,1),('Tanktest',20,8,1),('Tanktest',18,14,1),('Tanktest',3,5,1),('Tanktest',10,4,1);
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
  `spirit` int(11) NOT NULL,
  `health` int(11) NOT NULL,
  `mana` int(11) NOT NULL,
  `class` int(11) NOT NULL,
  `recall` int(11) NOT NULL,
  `ghost` double NOT NULL,
  `corpse_room` int(11) NOT NULL,
  `stat_points` int(11) NOT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `players`
--

LOCK TABLES `players` WRITE;
/*!40000 ALTER TABLE `players` DISABLE KEYS */;
INSERT INTO `players` VALUES ('Bob','\r\Z',0,'',61309521,35,240,1,0,139,512,118,446,269,102,4460,2690,4,0,0,0,1758),('Bobo','\r\Z',0,'',1268,123,2,1,0,5,5,5,5,5,5,34,36,2,2,0,0,6),('Boris','\r\Z',10,'',61309521,126,240,1,7,296,261,565,877,285,261,8770,2850,2,2,0,0,3815),('Castertest','\r\Z',0,'',61309521,126,240,1,4,5,363,5,376,365,363,3760,3650,3,2,0,0,0),('Dpstest','\r\Z',0,'',61309521,126,240,1,6,818,5,5,209,202,5,2051,2020,2,2,0,0,220),('Maikeru','\0W',0,'',1356,121,2,1,0,5,5,5,11,5,5,0,50,4,2,1532997372,92,0),('Momo','\r\Z',0,'',300,33,1,1,0,5,5,5,5,5,5,50,50,1,2,0,0,0),('Tanktest','\r\Z',0,'',61309521,126,240,1,7,291,5,489,682,5,5,6820,50,2,2,0,0,72),('Tash','FCU',0,'',1451,3,2,1,2,5,5,5,5,5,0,50,100,1,2,0,0,6);
/*!40000 ALTER TABLE `players` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `quest_item_rewards`
--

DROP TABLE IF EXISTS `quest_item_rewards`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `quest_item_rewards` (
  `quest` int(11) NOT NULL,
  `item` int(11) NOT NULL,
  PRIMARY KEY (`quest`,`item`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `quest_item_rewards`
--

LOCK TABLES `quest_item_rewards` WRITE;
/*!40000 ALTER TABLE `quest_item_rewards` DISABLE KEYS */;
INSERT INTO `quest_item_rewards` VALUES (4,2),(4,26),(4,27),(15,29),(16,32),(16,33),(16,34),(16,35);
/*!40000 ALTER TABLE `quest_item_rewards` ENABLE KEYS */;
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
INSERT INTO `quest_objectives` VALUES (1,2,8,8,'Kobold Vermin'),(2,2,9,8,'Kobold Workers'),(3,2,10,8,'Kobold Laborers'),(4,3,20,8,'Tough Wolf Meat'),(5,3,21,8,'Red Burlap Bandana'),(9,3,22,8,'Milly\'s Harvest'),(11,2,18,1,'Garrick Padfoot slain'),(11,3,25,1,'Garrick\'s Head'),(12,3,23,1,'Grape Manifest'),(13,3,24,1,'Marshal McBride\'s Documents'),(15,3,30,10,'Essence of the Elements'),(16,2,28,1,'Lord Incendius slain');
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
  `name` varchar(100) NOT NULL,
  `short_description` text NOT NULL,
  `long_description` text NOT NULL,
  `progress_message` text NOT NULL,
  `completion_message` text NOT NULL,
  `level` int(11) NOT NULL,
  `quest_requirement` int(11) NOT NULL,
  `start` int(11) NOT NULL,
  `end` int(11) NOT NULL,
  `exp_reward` int(11) NOT NULL,
  `money_reward` int(11) NOT NULL,
  `shareable` int(11) NOT NULL,
  `level_requirement` int(11) NOT NULL,
  `quest_restriction` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `quests`
--

LOCK TABLES `quests` WRITE;
/*!40000 ALTER TABLE `quests` DISABLE KEYS */;
INSERT INTO `quests` VALUES (1,'Kobold Camp Cleanup','Kill 8 Kobold Vermin, then return to Marshal McBride.','Your first task is one of cleansing. A clan of kobolds have infested the \nwoods to the north. Go there and fight the Kobold Vermin you find. Reduce \ntheir numbers so that we may one day drive them from Northshire.','','Well done, citizen.  Those kobolds are thieves and cowards, but in large \nnumbers they pose a threat to us.  And the humans of Stormwind do not \nneed another threat.\n\nFor defeating them, you have my gratitude. ',2,6,4,4,170,10,1,1,0),(2,'Investigate Echo Ridge','Kill 8 Kobold Workers, then report back to Marshal McBride.','My scouts tell me that the kobold infestation is larger than we had thought. \nA group of Kobold Workers has camped near the Echo Ridge Mine to the north.\n\nGo to the mine and remove them. We know there are at least 8. Kill them, \nsee if there are more, then report back to me. ','','I don\'t like hearing of all these kobolds in our mine.  No good can come \nof this.  Here, take this as payment, and when you\'re ready, speak to me \nagain.  I would like you to go back to the mines one more time... ',3,1,4,4,250,20,1,1,0),(3,'Skirmish at Echo Ridge','Kill 8 Kobold Laborers, then return to Marshal McBride at Northshire Abbey.','Your previous investigations are proof that the Echo Ridge Mine needs \npurging. Return to the mine and help clear it of kobolds.\n\nWaste no time. The longer the kobolds are left unmolested in the mine, \nthe deeper a foothold they gain in Northshire. ','','Once again, you have earned my respect, and the gratitude of the \nStormwind Army.  There may yet be kobolds in the mine, but I will \nmarshal others against them.  We have further tasks for you. ',5,2,4,4,500,330,1,1,0),(4,'Wolves Across the Border','Bring 8 pieces of Tough Wolf Meat to Eagan Peltskinner outside Northshire Abbey.','I hate those nasty timber wolves! But I sure like eating wolf steaks... \nBring me tough wolf meat and I will exchange it for something you\'ll\n find useful.\n\nTough wolf meat is gathered from hunting the timber wolves and \nyoung wolves wandering the Northshire countryside.','I\'m getting hungry...did you get that tough wolf meat? ','You\'ve been busy!  I can\'t wait to cook up that wolf meat...\n\r',2,6,5,5,170,0,1,1,0),(5,'Brotherhood of Thieves','Bring 8 Red Burlap Bandanas to Deputy Willem outside the Northshire Abbey. ','Recently, a new group of thieves has been hanging around \nNorthshire. They call themselves the Defias Brotherhood, and have\nbeen seen across the river to the east.\n\nI don\'t know what they\'re up to, but I\'m sure it\'s not good! Bring me \nthe bandanas they wear, and I\'ll reward you with a weapon. ','Have you gathered those bandanas for me yet? ','Back with some bandanas, I see.  The Stormwind Army appreciates your help. ',4,6,6,6,360,0,1,2,0),(6,'A Threat Within','Speak with Marshal McBride. ','I hope you strapped your belt on tight, because there is work \n\rto do here in Northshire.\n\r\n\rAnd I don\'t mean farming.\n\r\n\rThe Stormwind guards are hard pressed to keep the peace here, \n\rwith so many of us in distant lands and so many threats pressing \n\rclose. And so we\'re enlisting the aid of anyone willing to defend \n\rtheir home. And their alliance.\n\r\n\rIf you\'re here to answer the call, then speak with my superior, \n\rMarshal McBride. He\'s inside the abbey to the north.\n\r','','Ah, good.  Another volunteer.  We\'re getting a lot of you these days.\n\nI hope it\'s enough.\n\nOur lands are threatened from without, and so many of our forces\nhave been marshaled abroad.  This, in turn, leaves room for corrupt \nand lawless groups to thrive within our borders.\n\nIt is a many-fronted battle we wage.  Gird yourself for a long campaign. ',1,0,6,4,50,0,0,1,0),(7,'Milly Osworth','Speak with Milly Osworth. ','You\'ve shown yourself to be dependable. Dependable, and not \nafraid to get your hands dirty, eh?\n\nI have a friend, Milly Osworth, who\'s in some trouble. She\'s over \nwith her wagon on the other side of the abbey, near the stable. I\'m \nsure she could use a pair of hands like yours. ','','Oh, Deputy Willem told you to speak with me?  He\'s a brave \nman and always willing to help, but his duties keep him stuck \nat Northshire Abbey and I\'m afraid the problem I have today is \nbeyond him.\n\nPerhaps you can help me? ',4,5,6,7,50,0,1,2,0),(8,'Eagan Peltskinner','Speak with Eagan Peltskinner.','Eagan Peltskinner is looking for someone to hunt wolves for him. \n\rThat\'s good news, because we\'re seeing a lot more wolves in \n\rNorthshire Valley lately.\n\r\n\rIf you\'re interested then speak with Eagan. He\'s around the \n\rside of the abbey, to the west.\n\r','','That\'s true. I\'m looking for someone to hunt me some wolves! \nAre you that person?',1,6,6,5,100,0,1,1,4),(9,'Milly\'s Harvest','Bring 8 crates of Milly\'s Harvest to Milly Osworth at Northshire Abbey. ','A gang of brigands, the Defias, moved into the Northshire Vineyards while I was \n\rharvesting! I reported it to the Northshire guards and they assured me they\'d \n\rtake care of things, but... I\'m afraid for my crop of grapes! If the Defias \n\rdon\'t steal them then I fear our guards will trample them when they chase away \n\rthe thugs.\n\r\n\rPlease, you must help me! I gathered most of my grapes into buckets, but I left \n\rthem in the vineyards to the southeast.\n\r\n\rBring me those buckets! Save my harvest! \n\r','Do you have my harvest?\n\n','Oh thank you! You saved my harvest!  And I hope you showed a few of those Defias that they can\'t cause trouble around here.\n\n\nWe might be short on guards these days, but we\'re lucky to have heroes like you to protect us! \n\n',4,7,7,7,200,0,1,2,0),(10,'Rest and Relaxation','Speak with Innkeeper Farley at the Lion\'s Pride Inn.','Every adventurer should rest when exhaustion sets in - and there is no finer place to get rest and relaxation than at the Lion\'s Pride Inn!\n\r\n\rMy best friend, Innkeeper Farley, runs the Lion\'s Pride. If you tell him I sent you, he may give you the special discounted rates on food and drink.\n\r\n\rTo find the Lion\'s Pride Inn, travel south along the road from here -- you can\'t miss it! \n\r','','Rest and relaxation for the tired and cold -- that\'s our motto! Please, take a seat by the fire and rest your weary bones.\n\r\n\rWould you like to try a sampling of some of our fine food and drink? \n\r',5,0,11,19,100,0,1,1,0),(11,'Bounty on Garrick Padfoot','Kill Garrick Padfoot and bring his head to Deputy Willem at Northshire Abbey.','Garrick Padfoot - a cutthroat who\'s plagued our farmers and merchants for weeks - was seen at a shack near the vineyards, which lies east of the Abbey and across the bridge. Bring me the villain\'s head, and earn his bounty!\n\r\n\rBut be wary. Garrick has gathered a gang of thugs around him. He will not be an easy man to reach.\n\r','Did you find Garrick\'s shack? Are we finally free of that villain?\n\r','Hah - you caught him! You\'ve done Elwynn a great service, and earned a nice bounty!\n\r',5,5,6,6,350,0,1,3,0),(12,'Grape Manifest','Bring the Grape Manifest to Brother Neals in Northshire Abbey.','Now that my crop is saved, take this Grape Manifest to Brother Neals. He manages the store of food and drink in Northshire, and I\'m sure he\'ll be delighted to hear that he has fresh grapes.\n\r\n\rYou\'ll find Brother Neals in the abbey, in the bell tower... where he likes to taste his wine.\n\r','You look to be in fine spirits! Come! Have a seat, and have a drink!\n\r','Let\'s see here...\n\r\n\rOh my! Milly\'s grapes have been saved! When she told me that brigands overran her vineyards I nearly despaired, but my faith in the Light did not waver!\n\r\n\rAnd through your bravery, we now have grapes for more wine! May the Light bless you, and keep you safe!\n\r',4,9,7,12,360,0,0,3,0),(13,'Report to Goldshire','Take Marshal McBride\'s Documents to Marshal Dughan in Goldshire.','You have proven interest in the security of Northshire. You are now tasked with the protection of the surrounding Elwynn Forest.\n\r\n\rIf you accept this duty, then I have prepared papers that must be delivered to Marshal Dughan in Goldshire. Goldshire lies along the southern road, past the border gates.\n\r','You have word from McBride? Northshire is a garden compared to Elwynn Forest, but I wonder what Marshal McBride has to report.\n\r\n\rHere, let me have his papers...\n\r','Well, it says here that you\'ve been awarded Acting Deputy Status with the Stormwind Marshals. Congratulations.\n\r\n\rAnd good luck - keeping Elwynn safe is no picnic... what with most the army busy doing who knows what for who knows which noble!\n\r\n\rIt\'s hard to keep track of politics in these dark days...\n\r',5,3,4,17,230,0,1,3,14),(14,'The Fargodeep Mine','Explore the Fargodeep Mine, then return to Marshal Dughan in Goldshire.','The mine in Northshire isn\'t the only one with problems! I have reports that the Fargodeep Mine in Elwynn has also become a haven for Kobolds.\n\r\n\rExplore the mine and confirm these reports, then return to me. The mine is almost due south of Goldshire, between the Stonefield and Maclure homesteads.\n\r','What do you have to report? Have you been to the Fargodeep Mine?\n\r','This is bad news. What\'s next, dragons?!? We\'ll have to increase our patrols near that mine. Thanks for your efforts. And hold a moment... I might have another task for you.\n\r',7,0,17,17,480,75,1,4,0),(15,'The Last Element','Travel to Blackrock Depths and recover 10 Essence of the Elements. Your first inclination is to search the golems and golem makers. You remember Vivian Lagrave also muttering something about elementals.','There is work to be had for those venturing into the depths.\n\r\n\rThe Dark Irons have mastered creation of extremely powerful golems.\n\r\n\rInitial reports from our spies indicate that the dwarves use a unique power source to give their creations incomparable killing power.\n\r\n\rJust imagine what we could do with our abominations if we could get our hands on this essence of the elements! Turn that city upside down if you must, but do not return until you have found the essence! Payment will be worth the risk.\n\r','Show them to me! \n\r','Wonderful! I will have these sent by courier to the Undercity at once!\n\r\n\rAs for you - here is payment, as promised. Keep the change, you filthy beast! \n\r',234,0,26,26,2180,245,0,228,0),(16,'Incendius!','Find Lord Incendius in Blackrock Depths and destroy him!','Are you sure Pyron said \'Incendius\' when he died?\n\r\n\r<Jalinda swallows hard.>\n\r\n\rLord Incendius is purported to be a minion of Ragnaros! Oh dear, oh dear... whatever will we do?\n\r\n\rDo you think you can handle another mission? I don\'t have anybody else to send!\n\r\n\rWe will never get a team in if Incendius continues to raise Pyron from the ashes.\n\r\n\rYou\'ll have to find and destroy Lord Incendius!\n\r','Is the fiend dead??\n\r','Oh dear! Are you sure Incendius said \'Ragnaros?\'\n\r\n\r<Jalinda hands you something and pats you on the wrist as she fades deep into thought.>\n\r',236,0,27,27,2320,85,0,228,0);
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
INSERT INTO `resets` VALUES (2,1,1,6,0,0,30),(3,1,1,4,0,0,30),(4,1,1,16,0,0,30),(4,2,1,14,0,0,30),(7,1,1,2,0,0,120),(7,2,1,3,0,0,120),(13,1,1,20,0,0,30),(14,1,1,21,0,0,30),(17,1,1,12,0,0,30),(18,1,1,15,0,0,30),(18,2,1,13,0,0,30),(23,1,1,11,0,0,30),(27,1,1,2,0,0,120),(28,1,1,2,0,0,120),(29,1,1,3,0,0,120),(30,1,1,3,0,0,120),(32,1,1,3,0,0,120),(33,1,1,2,0,0,120),(34,1,1,5,0,0,30),(35,1,1,2,0,0,120),(35,2,1,8,0,0,120),(36,1,1,8,0,0,120),(38,1,1,3,0,0,120),(40,1,1,9,0,0,120),(42,1,1,8,0,0,120),(43,1,1,8,0,0,120),(43,2,1,8,0,0,120),(46,1,1,9,0,0,120),(47,1,1,9,0,0,120),(48,1,1,9,0,0,120),(48,2,1,9,0,0,120),(49,1,1,10,0,0,120),(50,1,1,10,0,0,120),(62,1,1,2,0,0,120),(63,1,1,2,0,0,120),(65,1,1,2,0,0,120),(65,2,1,3,0,0,120),(68,1,1,8,0,0,120),(70,1,1,8,0,0,120),(72,1,1,8,0,0,120),(72,2,1,8,0,0,120),(74,1,1,9,0,0,120),(74,2,1,9,0,0,120),(76,1,1,9,0,0,120),(80,1,1,3,0,0,120),(84,1,1,7,0,0,30),(85,1,1,2,0,0,120),(86,1,1,3,0,0,120),(87,1,1,3,0,0,120),(88,1,1,23,0,0,120),(88,2,2,22,0,0,30),(92,1,1,23,0,0,120),(94,1,1,22,0,0,120),(95,1,1,18,0,0,120),(95,2,1,23,0,0,120),(96,1,1,22,0,0,120),(96,2,1,23,0,0,120),(97,1,1,22,0,0,120),(97,2,1,23,0,0,120),(98,1,1,22,0,0,120),(100,1,1,23,0,0,120),(102,1,1,22,0,0,120),(106,1,1,22,0,0,120),(108,1,1,22,0,0,120),(109,1,1,10,0,0,120),(110,1,1,10,0,0,120),(111,1,1,10,0,0,120),(112,1,1,10,0,0,120),(113,1,1,10,0,0,120),(115,1,1,10,0,0,120),(116,1,1,10,0,0,120),(117,1,1,10,0,0,120),(118,1,1,10,0,0,120),(120,1,1,10,0,0,120),(125,1,1,24,0,0,1800),(125,2,1,24,0,0,1800),(126,1,1,26,0,0,30),(126,2,1,27,0,0,30),(127,1,1,25,0,0,1800),(132,1,1,29,0,0,1800),(132,2,1,29,0,0,1800),(132,3,1,30,0,0,1800),(132,4,1,31,0,0,1800),(132,5,1,32,0,0,1800);
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
INSERT INTO `rooms` VALUES (1,'The One Room','One Room to rule them all.\n\r',0,''),(2,'On the Steps of Northshire Abbey','',1,'0;'),(3,'Northshire Abbey','',1,''),(4,'Dermot\'s Wagon','',1,''),(5,'Northshire Valley','',1,''),(6,'Northshire Valley','',1,''),(7,'Northshire Valley','',1,''),(8,'Northshire Valley','',1,''),(9,'Northshire Road','',1,''),(10,'Northshire Abbey','',1,''),(11,'Northshire Abbey','',1,''),(12,'Main Hall','',1,''),(13,'Library Wing','',1,''),(14,'Hall of Arms','',1,''),(15,'Main Hall','',1,''),(16,'Northshire Abbey','',1,''),(17,'Northshire Abbey Bell Tower','',1,''),(18,'Dermot\'s Wagon','',1,''),(19,'Northshire Road','',1,''),(20,'Northshire Road','',1,''),(21,'Northshire Valley Gate','',1,''),(22,'Northshire Valley Gate','',1,''),(23,'Elwynn Forest Road','',2,''),(24,'Northshire Valley','',1,''),(25,'Northshire Valley','',1,''),(26,'Northshire Valley','',1,''),(27,'Northshire Valley','',1,''),(28,'Northshire Valley','',1,''),(29,'Northshire Valley','',1,''),(30,'Northshire Valley','',1,''),(31,'Northshire Valley','',1,''),(32,'Northshire Valley','',1,''),(33,'Northshire Valley','',1,''),(34,'Northshire Valley','',1,''),(35,'Northshire Valley','',1,''),(36,'Northshire Valley','',1,''),(37,'Northshire Valley','',1,''),(38,'Northshire Valley','',1,''),(39,'Northshire Valley','',1,''),(40,'Northshire Valley','',1,''),(41,'Northshire Valley','',1,''),(42,'Northshire Valley','',1,''),(43,'Northshire Valley','',1,''),(44,'Northshire Valley','',1,''),(45,'Northshire Valley','',1,''),(46,'Northshire Valley','',1,''),(47,'Northshire Valley','',1,''),(48,'Northshire Valley','',1,''),(49,'Entrance to Echo Ridge Mine','',1,''),(50,'Echo Ridge Mine','',1,''),(51,'Behind Northshire Abbey','',1,''),(52,'Behind Northshire Abbey','',1,''),(53,'Behind Northshire Abbey','',1,''),(54,'Northshire Valley','',1,''),(55,'Northshire Valley','',1,''),(56,'Northshire River','',1,''),(57,'Northshire River','',1,''),(58,'Northshire River','',1,''),(59,'A Bridge Over Northshire River','',1,''),(60,'Northshire River','',1,''),(61,'Northshire River','',1,''),(62,'Northshire Valley','',1,''),(63,'Northshire Valley','',1,''),(64,'Northshire Valley','',1,''),(65,'Northshire Valley','',1,''),(66,'An Impassable Waterfall','',1,''),(67,'Northshire Valley','',1,''),(68,'Northshire Valley','',1,''),(69,'Northshire Valley','',1,''),(70,'Northshire Valley','',1,''),(71,'Northshire Valley','',1,''),(72,'Northshire Valley','',1,''),(73,'Northshire Valley','',1,''),(74,'Northshire Valley','',1,''),(75,'Northshire Valley','',1,''),(76,'Northshire Valley','',1,''),(77,'Northshire Valley','',1,''),(78,'Northshire Valley','',1,''),(79,'Northshire Valley','',1,''),(80,'Northshire Valley','',1,''),(81,'Northshire Valley','',1,''),(82,'Northshire Valley','',1,''),(83,'Northshire Valley','',1,''),(84,'Northshire Stables','',1,''),(85,'Northshire Valley','',1,''),(86,'Northshire Valley','',1,''),(87,'Northshire Valley','',1,''),(88,'Northshire Vineyards','',1,''),(89,'Northshire Vineyards','',1,''),(90,'Northshire Vineyards','',1,''),(91,'Northshire Vineyards','',1,''),(92,'Northshire Vineyards','',1,''),(93,'Northshire Vineyards','',1,''),(94,'Northshire Vineyards','',1,''),(95,'Defias Camp','',1,''),(96,'Defias Camp','',1,''),(97,'Defias Camp','',1,''),(98,'Northshire Valley','',1,''),(99,'Northshire Valley','',1,''),(100,'Northshire Vineyards','',1,''),(101,'Northshire Vineyards','',1,''),(102,'Northshire Vineyards','',1,''),(103,'Northshire Vineyards','',1,''),(104,'Northshire Vineyards','',1,''),(105,'Northshire Vineyards','',1,''),(106,'Northshire Vineyards','',1,''),(107,'Northshire Vineyards','',1,''),(108,'Northshire Vineyards','',1,''),(109,'Echo Ridge Mine','',1,''),(110,'Echo Ridge Mine','',1,''),(111,'Echo Ridge Mine','',1,''),(112,'Echo Ridge Mine','',1,''),(113,'Echo Ridge Mine','',1,''),(114,'Echo Ridge Mine','',1,''),(115,'Echo Ridge Mine','',1,''),(116,'Echo Ridge Mine','',1,''),(117,'Echo Ridge Mine','',1,''),(118,'Echo Ridge Mine','',1,''),(119,'Echo Ridge Mine','',1,''),(120,'Echo Ridge Mine','',1,''),(121,'A Small Cemetery','',1,''),(122,'Northshire Valley','',1,''),(123,'Detention Block','',3,''),(124,'Shadowforge Gate','',3,''),(125,'Detention Block','',3,''),(126,'Detention Block','',3,''),(127,'Shadowforge Gate','',3,''),(128,'Shadowforge Gate','',3,''),(129,'Shadowforge Gate','',3,''),(130,'Detention Block','',3,''),(131,'Detention Block','',3,''),(132,'Dark Iron Highway','',3,''),(133,'Dark Iron Highway','',3,''),(134,'Dark Iron Highway','',3,'');
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
  `cooldown` double NOT NULL,
  `flags` tinytext,
  `target_type` int(11) NOT NULL,
  `description` tinytext,
  `cost_description` tinytext,
  `cast_time` double NOT NULL,
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
INSERT INTO `skills` VALUES (1,'Recall','recall',1800,'',0,'Return to a preset location. Set with \"recall\" command.','50% of total mana',10,'0;1;','skill_recall','function skill_recall_cast(caster, target, skill)\n\r  caster:Message(\"|WYou have completed your spell.|X\", 0, nil)\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell.|X\", 3, nil)\n\r  caster:ConsumeMana(math.ceil(caster:GetMaxMana()/2))\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" disappears in a flash of light.|X\", 3, nil)\n\r  caster:ChangeRoomsID(caster:GetRecall())\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" appears in a flash of light.|X\", 3, nil)\n\r  caster:Look(\"\")\n\rend\n\r','','','','function skill_recall_cost(caster, target, skill)\n\r  if(caster:GetRecall() == 0) then\n\r      caster:Message(\"You must set your recall location first.\", 0, null)\n\r      return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, math.ceil(caster:GetMaxMana()/2))) then\n\r      caster:Message(\"You don\'t have enough mana.\", 0, null)\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(2,'Fireball 1','fireball 1',0,'',2,'Launches a ball of fire at the enemy, causing 14 + 22% of intellect damage','10 mana',2.5,'0;','skill_fireball_1','function skill_fireball_1_cast(caster, target, skill)\n\r  caster:ConsumeMana(10)\n\r  local damage = math.ceil(14 + 0.22 * caster:GetIntellect())\n\r  caster:Message(\"|WYou cast fireball on \" .. target:GetName() .. \". Your fireball hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_CHAR, null)\n\r  target:Send(\"|W\" .. caster:GetName() .. \" casts fireball on you. \" .. caster:GetName() .. \"\'s fireball hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" casts fireball on \" .. target:GetName() .. \". \" .. caster:GetName() .. \"\'s fireball hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:OneHit(target, damage)\n\rend\n\r\r','','','','function skill_fireball_1_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r\r\r  end\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 10)) then\n\r    caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(3,'Heal 1','heal 1',0,'',4,'Heal a friendly target for 30 + 40% of intellect','40 mana',3,'0;','skill_heal_1','function skill_heal_1_cast(caster, target, skill)\n\r  if(caster:IsFighting(target)) then\n\r    caster:Send(\"You can\'t heal the target you\'re attacking.\\n\\r\")\n\r    return\n\r  end\n\r\n\r  local healval = math.ceil(30 + 0.4 * caster:GetIntellect())\n\r\n\r  if(caster == target) then\n\r    if(healval > (caster:GetMaxHealth() - caster:GetHealth())) then\n\r      healval = caster:GetMaxHealth() - caster:GetHealth()\n\r    end\n\r    caster:Send(\"|WYou have completed your spell. You gain \" .. healval .. \" health.|X\\n\\r\")\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell. \" .. caster:GetName() .. \" gains \" .. healval .. \" health.|X\", MSG_ROOM_NOTCHAR, null)\n\r  else\n\r    if(healval > (target:GetMaxHealth() - target:GetHealth())) then\n\r      healval = target:GetMaxHealth() - target:GetHealth()\n\r    end\n\r    caster:Send(\"|WYou have completed your spell. \" .. target:GetName() .. \" gains \" .. healval .. \" health.|X\\n\\r\")\n\r    target:Send(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() .. \" spell. You gain \" .. healval .. \" health.|X\\n\\r\")\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell. \" .. target:GetName() .. \" gains \" .. healval .. \" health.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  end\n\r  caster:ConsumeMana(40)\n\r  caster:OneHeal(target, healval)\n\r\rend\n\r','','','','function skill_heal_1_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 40)) then\n\r    caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(4,'Frost Nova','frost nova',25,'',5,'All hostile targets in the room are unable to move for 8 seconds','200 mana',0,'0;','skill_frost_nova','function skill_frost_nova_cast(caster, target, sk)\n\r  caster:Send(\"|WYou cast Frost Nova.\\n\\r|X\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" casts Frost Nova.|X\", MSG_ROOM_NOTCHAR, nil)\n\r  caster:ConsumeMana(200)\n\r  for index, ch in pairs(caster:GetRoom().characters) do\n\r    if(caster:CanAttack(ch)) then\n\r      ch:AddSpellAffect(1, caster, \"Frost Nova\", false, false, 0, 8, AFFECT_MAGIC, sk, \"Frozen in place\")\n\r      --ch:Send(\"You are affected by \" .. caster:GetName() .. \"\'s frost nova and unable to move.\\n\\r\")\n\r      caster:Send(\"|W\" .. ch:GetName() .. \" is affected by your Frost Nova and unable to move.|X\\n\\r\")\n\r      caster:Message(\"|W\" .. ch:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s Frost Nova and unable to move.|X\", MSG_ROOM_NOTCHARVICT, ch)\n\r      caster:EnterCombat(ch)\n\r      ch:EnterCombat(caster)\n\r    end\n\r  end\n\r\rend\n\r','function skill_frost_nova_apply(caster, target, affect)\n\r  affect:ApplyAura(1, -999)\n\rend\n\r','','function skill_frost_nova_remove(caster, target, affect)\n\r  target:Send(\"|WFrost nova fades from you.|X\\n\\r\")\n\r  if(caster ~= nil) then\n\r    caster:Send(\"|WYour frost nova fades from \" .. target:GetName() .. \".|X\\n\\r\")\n\r    target:Message(\"|W\" .. caster:GetName() .. \"\'s frost nova fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\r  else\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s frost nova fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\r  end\n\rend\n\r','function skill_frost_nova_cost(caster, target, skill)\n\r  if(not caster:HasResource(RESOURCE_MANA, 200)) then\n\r    caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(5,'Renew 1','renew 1',0,'',4,'Heals the target for 45 health over 15 seconds','30 mana',0,'0;','skill_renew_1','function skill_renew_1_cast(caster, target, sk)\n\r  caster:ConsumeMana(30)\n\r  local healpertick = math.ceil(45 / 5)\n\r  if(caster ~= target) then\n\r      caster:Send(\"|WYou cast Renew on \" .. target:GetName() .. \".|X\\n\\r\")\n\r      target:Send(\"|W\" .. caster:GetName() .. \" casts Renew on you.|X\\n\\r\")\n\r  else\n\r      caster:Send(\"|WYou cast Renew on yourself.|X\\n\\r\")\n\r  end\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" casts Renew on \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  target:AddSpellAffect(0, caster, \"Renew\", false, false, 5, 15.0, AFFECT_MAGIC, sk, \"Healing \" .. healpertick .. \" health every 3 seconds\")\n\r\rend\n\r','function skill_renew_1_apply(caster, target, affect)\n\r  if(caster == target) then\n\r      target:Send(\"|WYou are affected by Renew.|X\\n\\r\")\n\r  else\n\r      caster:Send(\"|W\" .. target:GetName() .. \" is affected by your Renew.|X\\n\\r\")\n\r      target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s Renew.|X\\n\\r\")\n\r  end\n\rend\n\r','function skill_renew_1_tick(caster, target, affect)\n\r  local healpertick = math.ceil(45 / 5)\n\r  local actualheal = healpertick\n\r\n\r  if(healpertick > (target:GetMaxHealth() - target:GetHealth())) then\n\r    actualheal = target:GetMaxHealth() - target:GetHealth()\n\r  end\n\r\n\r  --caster may be nil in tick and remove functions\n\r  if(affect:GetCasterName() == target:GetName()) then\n\r      target:Send(\"|WYour Renew restores \" .. actualheal .. \" health.|X\\n\\r\")\n\r  else\n\r      if(caster ~= nil) then\n\r          caster:Send(\"|WYour Renew heals \" .. target:GetName() .. \" for \" .. actualheal .. \" health.|X\\n\\r\")\n\r      end\n\r      target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s Renew restores \" .. actualheal .. \" health.|X\\n\\r\")\n\r  end\n\r  target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s Renew heals \" .. target:GetName() .. \" for \" .. actualheal .. \" health.|X\", MSG_ROOM_NOTCHARVICT, caster)\n\r\n\r  if(caster ~= nil) then\n\r    caster:OneHeal(target, actualheal)\n\r  else\n\r    target:AdjustHealth(caster, actualheal)\n\r  end\n\rend\n\r','function skill_renew_1_remove(caster, target, affect)\n\r  --caster may be nil in tick and remove functions\n\r  if(affect:GetCasterName() == target:GetName()) then\n\r      target:Send(\"|WYour Renew fades.|X\\n\\r\")\n\r  else\n\r      if(caster ~= nil) then\n\r          caster:Send(\"|WYour Renew fades from \" .. target:GetName() .. \".|X\\n\\r\")\n\r      end\n\r      target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s Renew fades from you.|X\\n\\r\")\n\r  end\n\r  target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s Renew fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, caster)\n\rend\n\r','function skill_renew_1_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(caster:IsFighting(target)) then\n\r    caster:Send(\"You can\'t heal the target you\'re attacking.\\n\\r\")\n\r    return\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 30)) then\n\r    caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(6,'Hamstring','hamstring',0,'',2,'Reduces movement speed of the target by 50% for 15 seconds','10 rage',0,'0;','skill_hamstring','function skill_hamstring_cast(caster, target, sk)\n\r  target:AddSpellAffect(1, caster, \"Hamstring\", false, false, 0, 15, AFFECT_NONE, sk, \"Movement speed reduced by 50%\")\n\r  caster:Send(\"|W\" .. target:GetName() .. \" is affected by your hamstring.|X\\n\\r\")\n\r  target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s hamstring.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s hamstring.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:ConsumeRage(10)\n\r\rend\n\r','function skill_hamstring_apply(caster, target, affect)\n\r  affect:ApplyAura(AURA_MOVE_SPEED, -50)\n\rend\n\r','','function skill_hamstring_remove(caster, target, affect)\n\r  target:Send(\"|WHamstring fades from you.|X\\n\\r\")\n\r  if(caster ~= nil) then\n\r    caster:Send(\"|WYour hamstring fades from \" .. target:GetName() .. \".|X\\n\\r\")\n\r    caster:Message(\"|W\" .. affect:GetCasterName() .. \"\'s hamstring fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  else\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s hamstring fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\r  end\n\rend\n\r','function skill_hamstring_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_RAGE, 10)) then\n\r      caster:Send(\"You don\'t have enough rage.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(7,'Greater Heal 1','greater heal 1',0,'',4,'Heals a single target for 2100 + 100% of intellect','370 mana',3,'0;','skill_greater_heal_1','function skill_greater_heal_1_cast(caster, target, skill)\n\r  if(caster:IsFighting(target)) then\n\r    caster:Send(\"You can\'t heal the target you\'re attacking.\\n\\r\")\n\r    return\n\r  end\n\r\n\r  local healval = math.ceil(2100 + caster:GetIntellect())\n\r\n\r  if(caster == target) then\n\r    if(healval > (caster:GetMaxHealth() - caster:GetHealth())) then\n\r      healval = caster:GetMaxHealth() - caster:GetHealth()\n\r    end\n\r    caster:Send(\"|WYou have completed your spell. You gain \" .. healval .. \" health.|X\\n\\r\")\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell. \" .. caster:GetName() .. \" gains \" .. healval .. \" health.|X\", MSG_ROOM_NOTCHAR, null)\n\r  else\n\r    if(healval > (target:GetMaxHealth() - target:GetHealth())) then\n\r      healval = target:GetMaxHealth() - target:GetHealth()\n\r    end\n\r    caster:Send(\"|WYou have completed your spell. \" .. target:GetName() .. \" gains \" .. healval .. \" health.|X\\n\\r\")\n\r    target:Send(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() .. \" spell. You gain \" .. healval .. \" health.|X\\n\\r\")\n\r    caster:Message(\"|W\" .. caster:GetName() .. \" has completed \" .. caster:HisHer() ..\" spell. \" .. target:GetName() .. \" gains \" .. healval .. \" health.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  end\n\r  caster:ConsumeMana(370)\n\r  caster:OneHeal(target, healval)\n\r\r\rend\n\r\r','','','','function skill_greater_heal_1_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 370)) then\n\r    caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(8,'Zot','zot',0,'',3,'health, mana = 1','none',0,'0;','skill_zot','function skill_zot_cast(caster, target, skill)\n\r  caster:Send(\"|YYou send a bolt of lightning to strike \" .. target:GetName() .. \".|X\\n\\r\")\n\r  target:Send(\"|YZOT! You are struck by a bolt of lightning!|X\\n\\r\")\n\r  target:Message(\"|YZOT! \" .. target:GetName() .. \" is struck by a bolt of lightning!|X\", MSG_ROOM_NOTCHAR, nil)\n\r  target:AdjustHealth(caster, -(target:GetHealth() - 1))\n\r  target:AdjustMana(caster, -(target:GetMana() - 1))\n\rend\n\r','','','','function skill_zot_cost(caster, target, skill)\n\r  return 1\n\rend\n\r'),(9,'Sprint','sprint',240,'',0,'Increases your movement speed by 70% for 15 seconds','none',0,'0;','skill_sprint','function skill_sprint_cast(caster, target, sk)\n\r  caster:Send(\"|WYou cast sprint.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" casts sprint.|X\", MSG_ROOM_NOTCHAR, nil)\n\r  caster:AddSpellAffect(0, caster, \"Sprint\", false, false, 0, 15, AFFECT_NONE, sk, \"Movement speed increased by 70%\")\n\r\rend\n\r','function skill_sprint_apply(caster, target, affect)\n\r  affect:ApplyAura(AURA_MOVE_SPEED, 70)\n\rend\n\r','','function skill_sprint_remove(caster, target, affect)\n\r  target:Send(\"|WSprint fades from you.|X\\n\\r\")\n\r  target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s sprint fades from \" .. affect:GetCasterName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\rend\n\r','function skill_sprint_cost(caster, target, skill)\n\r  return 1\n\rend\n\r'),(10,'Lucifron\'s Curse','lucifrons curse',0,'',2,'Spell and ability costs are increased by 100% for 5 minutes','none',0,'0;','skill_lucifrons_curse','function skill_lucifrons_curse_cast(caster, target, skill)\n\r  caster:Send(\"|WYou cast Lucifron\'s Curse.|X\\n\\r\")\n\r  target:AddSpellAffect(1, caster, \"Lucifron\'s Curse\", false, false, 0, 300, AFFECT_CURSE, skill, \"Skill costs increased by 100% for 5 minutes\")\n\rend\n\r','function skill_lucifrons_curse_apply(caster, target, affect)\n\r  caster:Send(\"|W\" .. target:GetName() .. \" is affected by Lucifron\'s curse.|X\\n\\r\")\n\r  target:Send(\"|WYou are affected by Lucifron\'s curse.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. target:GetName() .. \" is affected by Lucifron\'s curse.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  affect:ApplyAura(AURA_RESOURCE_COST, 100)\n\rend\n\r','','function skill_lucifrons_curse_remove(caster, target, affect)\n\r  target:Send(\"|WLucifron\'s curse fades from you.|X\\n\\r\")\n\r  target:Message(\"|WLucifron\'s curse fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\rend\n\r','function skill_lucifrons_curse_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(11,'Impending Doom','impending doom',0,'',2,'2000 Shadow damage inflicted after 10 sec.','none',0,'0;','skill_impending_doom','function skill_impending_doom_cast(caster, target, skill)\n\r  caster:EnterCombat(target)\n\r  caster:Send(\"|WYou cast Impending Doom on \" .. target:GetName() .. \"|X\\n\\r\")\n\r  target:AddSpellAffect(1, caster, \"Impending Doom\", false, false, 1, 10, AFFECT_MAGIC, skill, \"2000 shadow damage inflicted after 10 seconds\")\n\rend\n\r','function skill_impending_doom_apply(caster, target, affect)\n\r  caster:Send(\"|W\" .. target:GetName() .. \" is affected by Impending Doom.|X\\n\\r\")\n\r  target:Send(\"|WYou are affected by Impending Doom.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. target:GetName() .. \" is affected by Impending Doom.|X\", MSG_ROOM_NOTCHARVICT, target)\n\rend\n\r','function skill_impending_doom_tick(caster, target, affect)\n\r  target:Send(\"|WYou suffer 2000 damage from Impending Doom.|X\\n\\r\")\n\r  target:Message(\"|W\" .. target:GetName() .. \" suffers 2000 damage from Impending Doom.|X\", MSG_ROOM_NOTCHAR, nil)\n\r  if(caster ~= nil) then\n\r    caster:OneHit(target, 2000)\n\r  else\n\r    target:OneHit(target, 2000)\n\r  end\n\rend\n\r','function skill_impending_doom_remove(caster, target, affect)\n\r  target:Send(\"|WImpending Doom fades from you.|X\\n\\r\")\n\r  target:Message(\"|WImpending Doom fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\rend\n\r','function skill_impending_doom_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(12,'Remove Curse','remove curse',0,'',4,'Dispels one curse from a friendly target','120 mana',0,'0;','skill_remove_curse','function skill_remove_curse_cast(caster, target, skill)\n\r  caster:ConsumeMana(120)\n\r  if(caster ~= target) then\n\r    caster:Send(\"|WYou cast remove curse on \" .. target:GetName() .. \".|X\\n\\r\")\n\r  else\n\r    caster:Send(\"|WYou cast remove curse.|X\\n\\r\")\n\r  end\n\r  \n\r  local removed = target:CleanseSpellAffect(caster, AFFECT_CURSE, 1)\n\r  caster:EnterCombatAssist(target)\n\rend\n\r\r','','','','function skill_remove_curse_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 120)) then\n\r      caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend  \n\r'),(13,'Purify','purify',0,'',4,'Purifies the friendly target, removing 1 disease effect and 1 poison effect','180 mana',0,'0;','skill_purify','function skill_purify_cast(caster, target, skill)\n\r  caster:ConsumeMana(180)\n\r  if(caster ~= target) then\n\r    caster:Send(\"|WYou cast purify on \" .. target:GetName() .. \".|X\\n\\r\")\n\r  else\n\r    caster:Send(\"|WYou cast purify.|X\\n\\r\")\n\r  end\n\r\n\r  local poison = target:CleanseSpellAffect(caster, AFFECT_POISON, 1)\n\r  local disease = target:CleanseSpellAffect(caster, AFFECT_DISEASE, 1)\n\r  caster:EnterCombatAssist(target)\n\rend\n\r','','','','function skill_purify_cost(caster, target, skill)\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 180)) then\n\r      caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(14,'Slow','slow',30,'',2,'Movement speed decreased by 80%, gradually increasing over spell duration','200 mana',0,'0;','skill_slow','function skill_slow_cast(caster, target, sk)\n\r  caster:Message(\"|WYou cast slow on \" .. target:GetName() .. \".|X\", MSG_CHAR, nil)\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" casts slow on \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, caster)\n\r  target:AddSpellAffect(1, caster, \"Slow\", false, false, 5, 10, AFFECT_MAGIC, sk, \"Movement speed decreased\")\n\r  target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s slow.|X\\n\\r\")\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:ConsumeMana(200)\n\rend\n\r','function skill_slow_apply(caster, target, affect)\n\r  affect:ApplyAura(AURA_MOVE_SPEED, -80)\n\r  affect:SaveDataInt(\"ticknum\", 1)\n\rend\n\r','function skill_slow_tick(caster, target, affect)\n\r  ticknum = affect:GetDataInt(\"ticknum\")\n\r  affect:RemoveAura(AURA_MOVE_SPEED)\n\r  affect:ApplyAura(AURA_MOVE_SPEED, -80 + (15 * ticknum))\n\r  affect:SaveDataInt(\"ticknum\", ticknum + 1)\n\rend\n\r','function skill_slow_remove(caster, target, affect)\n\r  if(caster ~= nil) then\n\r    caster:Send(\"|WYour slow fades from \" .. target:GetName() .. \".|X\\n\\r\")\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s slow fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, caster)\n\r  else\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s slow fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\r  end\n\r  target:Send(\"|WSlow fades from you.|X\\n\\r\")\n\rend\n\r','function skill_slow_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 200)) then\n\r      caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(15,'Frostbolt 1','frostbolt 1',0,'',2,'Launches a bolt of frost at the enemy, causing 10 + 20% of intellect damage and slowing movement speed by 50% for 5 seconds','10 mana',2,'0;','skill_frostbolt_1',' function skill_frostbolt_1_cast(caster, target, sk)\n\r   caster:ConsumeMana(10)\n\r   local damage = math.ceil(10 + 0.2 * caster:GetIntellect())\n\r   caster:Message(\"|WYou cast frostbolt on \" .. target:GetName() .. \". Your frostbolt hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_CHAR, null)\n\r   target:Send(\"|W\" .. caster:GetName() .. \" casts frostbolt on you. \" .. caster:GetName() .. \"\'s frostbolt hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r   caster:Message(\"|W\" .. caster:GetName() .. \" casts frostbolt on \" .. target:GetName() .. \". \" .. caster:GetName() .. \"\'s frostbolt hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r   target:AddSpellAffect(1, caster, \"Chilled\", false, false, 0, 5, AFFECT_MAGIC, sk, \"Movement speed reduced by 50%\")\n\r   --target:AddSpellAffect(1, caster, \"Frostbite\", false, false, 0, 4, AFFECT_MAGIC, sk, \"Frozen in place\")\n\r\r   caster:EnterCombat(target)\n\r   target:EnterCombat(caster)\n\r   caster:OneHit(target, damage)\n\r end\n\r','function skill_frostbolt_1_apply(caster, target, affect)\n\r  if(affect.name == \"Chilled\") then  \n\r    affect:ApplyAura(AURA_MOVE_SPEED, -50)\n\r    target:Send(\"|WYou are affected by Chilled.|X\\n\\r\")\n\r    target:Message(\"|W\" .. target:GetName() .. \" is affected by Chilled.|X\", MSG_ROOM_NOTCHAR, nil)\n\r  end\n\r  if(affect.name == \"Frostbite\") then\n\r    caster:Send(\"Frostbite!\\n\\r\")\n\r  end\n\rend\n\r','','function skill_frostbolt_1_remove(caster, target, affect)\n\r  if(affect.name == \"Chilled\") then\n\r    target:Send(\"|WChilled fades from you.|X\\n\\r\")\n\r    target:Message(\"|WChilled fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\r  end\n\rend\n\r','function skill_frostbolt_1_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not target:IsAlive()) then\n\r       caster:Send(\"That target is already dead!\\n\\r\")\n\r       return 0\n\r   end\n\r   if(not caster:HasResource(RESOURCE_MANA, 10)) then\n\r       caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r       return 0\n\r   end\n\r   return 1\n\r end\n\r'),(16,'Slam','slam',0,'',2,'Slams an opponent, causing 190% of mainhand weapon damage, plus 30% of strength','20 rage',0,'0;','skill_slam','function skill_slam_cast(caster, target, sk)\n\r  local damage = math.ceil(caster:GetMainhandDamagePerSecond() * 1.9 + (caster:GetStrength() * .3))\n\r  caster:ConsumeRage(20)\n\r  caster:Send(\"|WYour Slam hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\\n\\r\")\n\r  target:Send(\"|W\" .. caster:GetName() .. \"\'s Slam hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \"\'s Slam hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  caster:OneHit(target, damage)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\rend\n\r','','','',' function skill_slam_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not target:IsAlive()) then\n\r      caster:Send(\"That target is already dead!\\n\\r\")\n\r      return 0\n\r  end\n\r\n\r  if(not caster:HasResource(RESOURCE_RAGE, 20)) then\n\r      caster:Send(\"You don\'t have enough rage.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(17,'Charge','',0,'',0,'',NULL,0,NULL,'','','','','',''),(18,'Victory Rush','',0,'',0,'',NULL,0,NULL,'','','','','',''),(19,'Execute','',0,'',0,'',NULL,0,NULL,'','','','','',''),(20,'Mortal Strike','',0,'',0,'',NULL,0,NULL,'','','','','',''),(21,'Sinister Strike','sinister strike',0,'',2,'An instant strike that causes 200% mainhand weapon damage plus 30% of strength. Awards 1 combo point.','40 energy',0,'0;','skill_sinister_strike','function skill_sinister_strike_cast(caster, target, sk)\n\r  local damage = math.ceil(caster:GetMainhandDamagePerSecond() * 2 + (caster:GetStrength() * .3))\n\r  caster:ConsumeEnergy(40)\n\r  caster:GenerateComboPoint(target)\n\r  caster:OneHit(target, damage)\n\r  caster:Send(\"|WYour Sinister Strike hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\\n\\r\")\n\r  target:Send(\"|W\" .. caster:GetName() .. \"\'s Sinister Strike hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \"\'s Sinister Strike hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r\rend\n\r','','','','function skill_sinister_strike_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not target:IsAlive()) then\n\r    caster:Send(\"That target is already dead!\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_ENERGY, 40)) then\n\r      caster:Send(\"You don\'t have enough energy.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(22,'Eviscerate','eviscerate',0,'',2,'Finishing move causing weapon damage plus 30% of strength per combo point','35 Energy / 1 to 5 Combo Points',0,'0;','skill_eviscerate','function skill_eviscerate_cast(caster, target, sk)\n\r  local combos = caster:SpendComboPoints(target)\n\r  if(combos == 0) then\n\r      caster:Send(\"You don\'t have combo points on that target.\\n\\r\")\n\r      return\n\r  end\n\r  local damage = math.ceil((caster:GetMainhandDamagePerSecond() + caster:GetOffhandDamagePerSecond()\n\r                 + (caster:GetStrength() * .3)) * combos)\n\r  caster:ConsumeEnergy(35)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:Send(\"|WYour Eviscerate hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\\n\\r\")\n\r  target:Send(\"|W\" .. caster:GetName() .. \"\'s Eviscerate hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \"\'s Eviscerate hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r\r  caster:OneHit(target, damage)\n\rend\n\r','','','','function skill_eviscerate_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_ENERGY, 35)) then\n\r      caster:Send(\"You don\'t have enough energy.\\n\\r\")\n\r      return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_COMBO, 1)) then\n\r      caster:Send(\"You don\'t have any combo points.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(23,'Stealth','',0,'',0,'',NULL,0,NULL,'','','','','',''),(24,'Cheap Shot','',0,'',0,'',NULL,0,NULL,'','','','','',''),(25,'Backstab','',0,'',0,'',NULL,0,NULL,'','','','','',''),(26,'Smite 1','smite 1',0,'',2,'Smites an enemy for 15 + 10% intellect damage','15 mana',1.5,'0;','skill_smite_1','function skill_smite_1_cast(caster, target, sk)\n\r  caster:ConsumeMana(15)\n\r  local damage = 15 + math.ceil(0.1 * caster:GetIntellect())\n\r  caster:Message(\"|WYour smite hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_CHAR, nil)\n\r  target:Send(\"|W\" .. caster:GetName() .. \"\'s smite hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \"\'s smite hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:OneHit(target, damage)\n\rend\n\r','','','','function skill_smite_1_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_MANA, 15)) then\n\r      caster:Send(\"You don\'t have enough mana.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(27,'Shadow Word: Pain 1','shadow word pain 1',0,'',2,'','NULL',0,'0;','shadow_word_pain_1','','','','',''),(28,'Flash Heal','',0,'',0,'',NULL,0,NULL,'','','','','',''),(29,'Shield','shield',0,'',4,'','NULL',0,'0;','skill_shield','','','','',''),(30,'Penance','',0,'',0,'',NULL,0,NULL,'','','','','',''),(31,'Fire Blast','',0,'',0,'',NULL,0,NULL,'','','','','',''),(32,'Ice Lance','ice lance',0,'',2,'','NULL',0,'0;','skill_ice_lance','','','','',''),(33,'Garrote','garrote',15,'',2,'Garrote the enemy, causing 100% of strength as bleed damage over 18 sec. Awards 1 combo point','45 energy',0,'0;','skill_garrote','function skill_garrote_cast(caster, target, sk)\n\r  local damage = math.ceil(caster:GetStrength() / 9)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:ConsumeEnergy(45)\n\r  caster:GenerateComboPoint(target)\n\r  target:AddSpellAffect(1, caster, \"Garrote\", false, false, 9, 18, AFFECT_BLEED, sk, \"Suffering \" .. damage .. \" damage every 2 seconds\")\n\r\rend\n\r','function skill_garrote_apply(caster, target, affect)\n\r  local damage = math.ceil(caster:GetStrength() / 9)\n\r  affect:SaveDataInt(\"damage\", damage)\n\r  caster:Send(\"|W\" .. target:GetName() .. \" is affected by your Garrote.|X\\n\\r\")\n\r  target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s Garrote.|X\\n\\r\")\n\r  target:Message(\"|W\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s Garrote.|X\", MSG_ROOM_NOTCHARVICT, caster)\n\rend\n\r','function skill_garrote_tick(caster, target, affect)\n\r  local damage = affect:GetDataInt(\"damage\")\n\r  if(caster ~= nil) then\n\r      caster:Send(\"|WYour Garrote damages \" .. target:GetName() .. \" for \" .. damage .. \" health.|X\\n\\r\")\n\r  end\n\r  target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s Garrote damages you for \" .. damage .. \" health.|X\\n\\r\")\n\r  target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s Garrote damages \" .. target:GetName() .. \" for \" .. damage .. \" health.|X\", MSG_ROOM_NOTCHARVICT, caster)\n\r  if(caster ~= nil) then\n\r    caster:OneHit(target, damage)\n\r  else\n\r    target:AdjustHealth(nil, -damage)\n\r  end\n\rend\n\r','function skill_garrote_remove(caster, target, affect)\n\r    if(caster ~= nil) then\n\r        caster:Send(\"|WYour Garrote fades from \" .. target:GetName() .. \".|X\\n\\r\")\n\r    end\n\r    target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s Garrote fades from you.|X\\n\\r\")\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s Garrote fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, caster)\n\rend\n\r','function skill_garrote_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_ENERGY, 45)) then\n\r      caster:Send(\"You don\'t have enough energy.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(34,'Bloodthirst','bloodthirst',4.5,'',2,'Assault the target in a bloodthirsty craze, dealing mainhand weapon damage plus 30% strength. Restores 4% health.','Generates 10 rage',0,'0;','skill_bloodthirst','function skill_bloodthirst_cast(caster, target, sk)\n\r  local damage = math.ceil(caster:GetMainhandDamagePerSecond() + (caster:GetStrength() * .3))\n\r  local heal = math.ceil(caster:GetMaxHealth() * .04)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:AdjustRage(caster, 10)\n\r  caster:OneHeal(caster, heal)\n\r  caster:Send(\"|WYour Bloodthirst hits \" .. target:GetName() .. \" for \" .. damage .. \" damage, and heals you for \" .. heal .. \" health.|X\\n\\r\")\n\r  target:Send(\"|W\" .. caster:GetName() .. \"\'s Bloodthirst hits you for \" .. damage .. \" damage, and heals \" .. caster:GetName() .. \" for \" .. heal .. \" health.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \"\'s Bloodthirst hits \" .. target:GetName() .. \" for \" .. damage .. \" damage, and heals \" .. caster:GetName() .. \" for \" .. heal .. \" health.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r\r  caster:OneHit(target, damage)\n\rend\n\r','','','','function skill_bloodthirst_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r'),(35,'Rend','rend',0,'',2,'Wounds the target, causing 150% of weapon damage, then 150% of strength as Bleed damage over 16 seconds','30 rage',0,'0;','skill_rend','function skill_rend_cast(caster, target, sk)\n\r  local damage = math.ceil((caster:GetMainhandDamagePerSecond() + caster:GetOffhandDamagePerSecond()) * 1.5)\n\r  local bleed = math.ceil((caster:GetStrength() * 1.6) / 4)\n\r  caster:Send(\"|WYour Rend hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\\n\\r\")\n\r  target:Send(\"|W\" .. caster:GetName() .. \"\'s Rend hits you for \" .. damage .. \" damage.|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \"\'s Rend hits \" .. target:GetName() .. \" for \" .. damage .. \" damage.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  caster:OneHit(target, damage)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:ConsumeRage(30)\n\r  target:AddSpellAffect(1, caster, \"Rend\", false, false, 4, 16, AFFECT_BLEED, sk, \"Suffering \" .. bleed .. \" damage every 4 seconds\")\n\r\rend\n\r','function skill_rend_apply(caster, target, affect)\n\r  local bleed = math.ceil((caster:GetStrength() * 1.6) / 4)\n\r  affect:SaveDataInt(\"damage\", bleed)\n\r  caster:Send(\"|W\" .. target:GetName() .. \" is affected by your Rend.|X\\n\\r\")\n\r  target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s Rend.|X\\n\\r\")\n\r  target:Message(\"|W\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s Rend.|X\", MSG_ROOM_NOTCHARVICT, caster)\n\rend\n\r','function skill_rend_tick(caster, target, affect) \n\r  local damage = affect:GetDataInt(\"damage\") \n\r  target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s Rend damages you for \" .. damage .. \" health.|X\\n\\r\") \n\r  target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s Rend damages \" .. target:GetName() .. \" for \" .. damage .. \" health.|X\", MSG_ROOM_NOTCHARVICT, caster) \n\r  if(caster ~= nil) then \n\r    caster:Send(\"|WYour Rend damages \" .. target:GetName() .. \" for \" .. damage .. \" health.|X\\n\\r\") \n\r    caster:OneHit(target, damage) \n\r  end \n\r  if(caster == nil) then \n\r    target:AdjustHealth(caster, -damage) \n\r  end \n\rend \n\r','function skill_rend_remove(caster, target, affect)\n\r    if(caster ~= nil) then\n\r        caster:Send(\"|WYour Rend fades from \" .. target:GetName() .. \".|X\\n\\r\")\n\r    end\n\r    target:Send(\"|W\" .. affect:GetCasterName() .. \"\'s Rend fades from you.|X\\n\\r\")\n\r    target:Message(\"|W\" .. affect:GetCasterName() .. \"\'s Rend fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, caster)\n\rend\n\r','function skill_rend_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not caster:HasResource(RESOURCE_RAGE, 30)) then\n\r      caster:Send(\"You don\'t have enough rage.\\n\\r\")\n\r      return 0\n\r  end\n\r  return 1\n\rend\n\r'),(36,'Mana Shield','mana shield',0,'',0,'','',0,'0;','skill_mana_shield','','','','',''),(37,'Food','food',0,'',0,'Skill associated with the food spell affect used by generic food items','',0,'0;','item_basicfood','function item_basicfood_cast(caster, target, sk)\n\r    local totalheal = math.ceil((20*caster:GetLevel())+50)\n\r    local healpersecond = math.ceil(totalheal / 30)\n\r    target:AddSpellAffect(0, caster, \"Food\", false, false, 10, 30, AFFECT_NONE, sk, \"Restores \" .. healpersecond .. \" health per second.\")\n\rend\n\r','function item_basicfood_apply(caster, target, affect)\n\r    local totalheal = math.ceil((20*caster:GetLevel())+50)\n\r    affect:SaveDataInt(\"totalheal\", totalheal)\n\r    affect:ApplyAura(AURA_EATING, 1)\n\r\n\rend\n\r','function item_basicfood_tick(caster, target, affect)\n\r    local totalheal = affect:GetDataInt(\"totalheal\")\n\r    local healpertick = math.ceil(totalheal / 10)\n\r    target:Send(\"|WYou gain \" .. healpertick .. \" health.|X\\n\\r\")\n\r    target:AdjustHealth(caster, healpertick)\n\rend\n\r','function item_basicfood_remove(caster, target, affect)\n\r    \n\rend\n\r','function item_basicfood_cost(caster, target, skill)\n\r    if(caster:InCombat()) then\n\r        caster:Send(\"You can\'t do that while in combat.\\n\\r\")\n\r        return 0\n\r    end\n\r    return 1\n\rend\n\r'),(38,'Drink','drink',0,'',0,'Skill associated with the drink spell affect used by generic drink items','',0,'0;','item_basicdrink','function item_basicdrink_cast(caster, target, sk)\n\r    local totalmana = math.ceil((30*caster:GetLevel())+50)\n\r    local manapersecond = math.ceil(totalmana / 30)\n\r    target:AddSpellAffect(0, caster, \"Drink\", false, false, 10, 30, AFFECT_NONE, sk, \"Restores \" .. manapersecond .. \" mana per second.\")\n\rend\n\r','function item_basicdrink_apply(caster, target, affect)\n\r    local totalmana = math.ceil((30*caster:GetLevel())+50)\n\r    affect:SaveDataInt(\"totalmana\", totalmana)\n\r    affect:ApplyAura(AURA_EATING, 1)\n\rend\n\r','function item_basicdrink_tick(caster, target, affect)\n\r    local totalmana = affect:GetDataInt(\"totalmana\")\n\r    local manapertick = math.ceil(totalmana / 10)\n\r    target:Send(\"|WYou gain \" .. manapertick .. \" mana.|X\\n\\r\")\n\r    target:AdjustMana(caster, manapertick)\n\rend\n\r','function item_basicdrink_remove(caster, target, affect)\n\r    \n\rend\n\r','function item_basicdrink_cost(caster, target, skill)\n\r    if(caster:InCombat()) then\n\r        caster:Send(\"You can\'t do that while in combat.\\n\\r\")\n\r        return 0\n\r    end\n\r    return 1\n\rend\n\r'),(39,'Frostbite','frostbite',0,'',2,'Frozen in place and unable to move','Possible effect from mage frostbolt',0,'0;','skill_frostbite','function skill_frostbite_cast(caster, target, sk)\n\r  caster:Message(\"|W\" .. target:GetName() .. \" is affected by frostbite and unable to move.|X\", MSG_CHAR, null)\n\r  target:Send(\"|WYou are affected by \" .. caster:GetName() .. \"\'s frostbite and unable to move. |X\\n\\r\")\n\r  caster:Message(\"|W\" .. target:GetName() .. \" is affected by \" .. caster:GetName() .. \"\'s frostbite and unable to move.|X\", MSG_ROOM_NOTCHARVICT, target)\n\r  target:AddSpellAffect(1, caster, \"Frostbite\", false, false, 0, 4, AFFECT_MAGIC, sk, \"Unable to move\")\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\rend\n\r','function skill_frostbite_apply(caster, target, affect)\n\r      affect:ApplyAura(AURA_MOVE_SPEED, -1000)\n\r\n\rend\n\r','','function skill_frostbite_remove(caster, target, affect)\n\r  target:Send(\"|WFrostbite fades from you.|X\\n\\r\")\n\r  target:Message(\"|WFrostbite fades from \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHAR, nil)\n\rend\n\r','function skill_frostbite_cost(caster, target, skill)\n\r\r  return 1\n\rend\n\r'),(40,'Taunt 1','taunt 1',12,'0;1;',2,'Taunts the target to attack you, but has no effect if the target is already attacking you','none',0,'0;','skill_taunt_1','function skill_taunt_1_cast(caster, target, sk)\n\r  caster:EnterCombat(target)\n\r  target:EnterCombat(caster)\n\r  caster:Send(\"|WYou taunt \" .. target:GetName() .. \".|X\\n\\r\")\n\r  caster:Message(\"|W\" .. caster:GetName() .. \" taunts \" .. target:GetName() .. \".|X\", MSG_ROOM_NOTCHARVICT, target)\n\r\n\r  target:AddSpellAffect(1, caster, \"Taunt\", false, false, 0, 3, AFFECT_NONE, sk, \"Taunted\")\n\r\n\r  if(target:GetTarget() ~= nil) then\n\r    saveme = target:GetTarget()\n\r    threat_difference = target:GetThreat(saveme) - target:GetThreat(caster)\n\r    if(threat_difference > 0) then \n\r      target:UpdateThreat(caster, threat_difference, THREAT_OTHER)\n\r    end\n\r  end\n\rend\n\r\r','function skill_taunt_1_apply(caster, target, affect)\n\r  affect:ApplyAura(AURA_TAUNT, 1)\n\rend\n\r','','function skill_taunt_1_remove(caster, target, affect)\n\r    target:Message(\"|WTaunt fades from \" .. target:GetName() .. \".|X\", MSG_ROOM, nil)\n\rend\n\r','function skill_taunt_1_cost(caster, target, skill)\n\r  if(not caster:CanAttack(target)) then\n\r    caster:Send(\"You can\'t attack that target.\\n\\r\")\n\r    return 0\n\r  end\n\r  if(not target:IsNPC()) then\n\r    caster:Send(\"This skill has no effect on players.\\n\\r\")\n\r    return 0\n\r  end\n\r  return 1\n\rend\n\r');
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

-- Dump completed on 2018-09-12 20:18:51
