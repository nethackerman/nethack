drop database if exists nhtour;
create database nhtour;
use nhtour;

create table players(
	id int not null auto_increment,
	name varchar(32) unique,
	password_hash varchar(64),
	email varchar(255),
	banned boolean,
	added timestamp,
	primary key(id));

create table clans(
	id int not null auto_increment,
	game_id int,
	owner_id int,
	name varchar(64) unique,
	added timestamp,
	primary key(id));

create table clan_members(
	id int not null auto_increment,
	clan_id int,
	player_id int unique,
	slot_type ENUM('filled', 'open', 'request'),
	added timestamp,
	primary key(id));

create table clan_requests(
	id int not null auto_increment,
	clan_id int,
	player_id int,
	added timestamp,
	primary key(id),
	unique index(clan_id, player_id));

create table objectives(
	id int not null auto_increment,
	name varchar(32) not null,
	literal varchar(32) not null,
	score int,
	decrease_rate boolean,
	added timestamp,
	primary key(id));

create table games(
	id int not null auto_increment,
	name varchar(128),
	starts timestamp,
	ends timestamp,
	added timestamp,
	primary key(id));

create table game_participants(
	id int not null auto_increment,
	game_id int,
	clan_id int,
	added timestamp,
	primary key(id));

create table mails(
	id int not null auto_increment,
	to_player_id int,
	from_name varchar(64),
	body varchar(128),
	delivered boolean,
	added timestamp,
	primary key(id));

-- 
-- Main game quests
--
insert into objectives values(0, 'reach_minetn',	"Reach Minetown",		1000, 5, NOW());
insert into objectives values(0, 'reach_minend',	"Reach Mines' end",		 500, 5, NOW());
insert into objectives values(0, 'reach_soko4',		"Reach Sokoban",		 500, 5, NOW());
insert into objectives values(0, 'complete_soko',	"Complete Sokoban",		1000, 5, NOW());
insert into objectives values(0, 'reach_strt',		"Reach Quest",			1000, 5, NOW());
insert into objectives values(0, 'kill_nem',		"Kill Nemesis",			1000, 5, NOW());
insert into objectives values(0, 'reach_medusa',	"Reach Medusa",			1000, 5, NOW());
insert into objectives values(0, 'kill_medusa',		"Kill Medusa",			1000, 5, NOW());
insert into objectives values(0, 'reach_knox',		"Reach Ludios",			1000, 5, NOW());
insert into objectives values(0, 'reach_castle',	"Reach Castle",			1000, 5, NOW());
insert into objectives values(0, 'reach_juiblex',	"Reach Juiblex",		1000, 5, NOW());
insert into objectives values(0, 'kill_juiblex',	"Kill Juiblex",			1000, 5, NOW());
insert into objectives values(0, 'reach_orcus',		"Reach Orcus",			1000, 5, NOW());
insert into objectives values(0, 'kill_orcus',		"Kill Orcus",			1000, 5, NOW());
insert into objectives values(0, 'reach_tower1',	"Reach Vlad's Tower",	1000, 5, NOW());
insert into objectives values(0, 'kill_vlad',		"Kill Vlad",			1000, 5, NOW());
insert into objectives values(0, 'kill_wizard',		"Kill Wizard",			1000, 5, NOW());
insert into objectives values(0, 'kill_demogorgon', "Kill Demogorgon",		1000, 5, NOW());
insert into objectives values(0, 'reach_sanctum',	"Reach Sanctum",		1000, 5, NOW());
insert into objectives values(0, 'get_aoy',			"Get Amulet of Yendor",	1000, 5, NOW());
insert into objectives values(0, 'reach_earth',		"Reach Earth Plane",	1000, 5, NOW());
insert into objectives values(0, 'reach_air',		"Reach Air Plane",		1000, 5, NOW());
insert into objectives values(0, 'reach_fire',		"Reach Fire Plane",		1000, 5, NOW());
insert into objectives values(0, 'reach_water',		"Reach Water Plane",	1000, 5, NOW());
insert into objectives values(0, 'reach_astral',	"Reach Astral Plane",	1000, 5, NOW());
insert into objectives values(0, 'ascend',			"Ascend",				10000, 5, NOW());
--
-- Reach level...
--
insert into objectives values(0, 'level_5',		"Get xlvl 5",	500,	5, NOW());
insert into objectives values(0, 'level_10',	"Get xlvl 10",	500,	5, NOW());
insert into objectives values(0, 'level_14',	"Get xlvl 14",	500,	5, NOW());
insert into objectives values(0, 'level_30',	"Get xlvl 30",	2000,	5, NOW());
--
-- Curses
--
insert into objectives values(0, 'curse_clan',		"Curse another clan",     4000, 1, NOW());
insert into objectives values(0, 'curse_ascend',	"Ascend with curse item", 2000, 1, NOW());
insert into objectives values(0, 'curse_kill',		"Kill with curse",         250, 1, NOW());
--
-- Team play
--
insert into objectives values(0, 'team_send', "Send an item to team-mate", 100, 1, NOW());

--
-- System user
--
insert into players values(0, "system", "", "prutt@korv.se", FALSE, null);


--
-- Hackshit
--
insert into players values(0, "pellsson", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "breggan", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "wizard", md5("bajskorv"), "prutt@korv.se", FALSE, null);
insert into players values(0, "gorbiz", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "RALLERMANSSON", md5("bajskorv"), "prutt@korv.se", FALSE, null);

insert into clans values(0, 1, 2, "SUPERCLAN", null);
insert into clan_members values(0, 1, 2, 'filled', null);
insert into clan_members values(0, 1, 3, 'filled', null);
insert into clan_members values(0, 1, 4, 'filled', null);
insert into clan_members values(0, 1, 5, 'filled', null);
insert into clan_members values(0, 1, 6, 'filled', null);

insert into players values(0, "gorbiz2", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "gorbiz3", md5("12"), "prutt@korv.se", FALSE, null);

insert into clans values(0, 1, 7, "BAJSCLAN", null);
insert into clan_members values(0, 2, 7, 'filled', null);
insert into clan_members values(0, 2, 8, 'filled', null);

insert into players values(0, "gorbiz4", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "gorbiz5", md5("12"), "prutt@korv.se", FALSE, null);

insert into clans values(0, 1, 9, "KALLECLAN", null);
insert into clan_members values(0, 3, 9, 'filled', null);
insert into clan_members values(0, 3, 10, 'filled', null);

insert into games values(0, "Test Tournament", NOW(), DATE_ADD(NOW(), INTERVAL 2000 HOUR), NOW());
insert into game_participants values(0, 1, 1, NOW());
insert into game_participants values(0, 1, 2, NOW());
insert into game_participants values(0, 1, 3, NOW());

drop database if exists nhtour_game_1;
create database nhtour_game_1;
use nhtour_game_1;

create table clan_state(
	id int not null auto_increment,
	clan_id int,
	gold int,
	can_be_cursed boolean,
	primary key(id));

create table player_state(
	id int not null auto_increment,
	player_id int,
	ingame boolean,
	message_id_seen int,
	primary key(id));

create table deliveries(
	id int not null auto_increment,
	otyp int,
	quan int,
	spe int,
	oclass int,
	buc int,
	oeroded int,
	oeroded2 int,
	oerodeproof int,
	otrapped int,
	recharged int,
	greased int,
	corpsenm int,
	usecount int,
	oeaten int,
	age bigint,
	named varchar(128),
	delivered boolean,
	to_player_id int,
	from_player_id int,
	added timestamp,
	primary key(id));

create table messages(
	id int not null auto_increment,
	clan_id int,
	player_id int,
	msg_type int,
	message varchar(128),
	added timestamp,
	primary key(id));

create table curses(
	id int not null auto_increment,
	player_id int,
	from_clan_id int,
	turns_left int,
	added timestamp,
	primary key(id));

create table score(
	id int not null auto_increment,
	objective_id int,
	player_id int,
	clan_id int,
	score int,
	added timestamp,
	primary key(id));

--
-- Hack shit
--
insert into clan_state values(0, 1, 0, 1);
insert into clan_state values(0, 2, 0, 1);
insert into clan_state values(0, 3, 0, 1);

insert into player_state values(0, 1, 0, 0);
insert into player_state values(0, 2, 0, 0);
insert into player_state values(0, 3, 0, 0);
insert into player_state values(0, 4, 0, 0);
insert into player_state values(0, 5, 0, 0);
insert into player_state values(0, 6, 0, 0);
insert into player_state values(0, 7, 0, 0);
insert into player_state values(0, 8, 0, 0);
insert into player_state values(0, 9, 0, 0);


