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
-- Reach specials
--
insert into objectives values(0, 'reach_minetn',	"Reach Minetown",		1000, 5, NOW());
insert into objectives values(0, 'reach_soko4',		"Reach Sokoban",		 500, 5, NOW());
insert into objectives values(0, 'reach_minend',	"Reach Mines' end",		 500, 5, NOW());
insert into objectives values(0, 'reach_knox',		"Reach Ludios",			1000, 5, NOW());
insert into objectives values(0, 'reach_strt',		"Reach Quest",			1000, 5, NOW());
insert into objectives values(0, 'reach_medusa',	"Reach Medusa",			1000, 5, NOW());
insert into objectives values(0, 'reach_castle',	"Reach Castle",			1000, 5, NOW());
insert into objectives values(0, 'reach_juiblex',	"Reach Juiblex",		1000, 5, NOW());
insert into objectives values(0, 'reach_orcus',		"Reach Orcus",			1000, 5, NOW());
insert into objectives values(0, 'reach_tower1',	"Reach Vlad's Tower",	1000, 5, NOW());
insert into objectives values(0, 'reach_sanctum',	"Reach Sanctum",		1000, 5, NOW());
insert into objectives values(0, 'reach_earth',		"Reach Earth Plane",	1000, 5, NOW());
insert into objectives values(0, 'reach_air',		"Reach Air Plane",		1000, 5, NOW());
insert into objectives values(0, 'reach_fire',		"Reach Fire Plane",		1000, 5, NOW());
insert into objectives values(0, 'reach_water',		"Reach Water Plane",	1000, 5, NOW());
insert into objectives values(0, 'reach_astral',	"Reach Astral Plane",	1000, 5, NOW());
insert into objectives values(0, 'reach_vinst1',	"Reach Oracle Quest 1",	1000, 5, NOW());
insert into objectives values(0, 'reach_vinst2',	"Reach Oracle Quest 2",	1000, 5, NOW());
insert into objectives values(0, 'reach_vinst3',	"Reach Oracle Quest 3",	1000, 5, NOW());

--
-- Misc goals
--
insert into objectives values(0, 'complete_soko',	"Complete Sokoban",		1000, 5, NOW());


--
-- Curses
--
insert into objectives values(0, 'curse_clan',		"Curse another clan",     4000, 0, NOW());
insert into objectives values(0, 'curse_kill',		"Kill with curse",         250, 0, NOW());
--
-- Team play
--
insert into objectives values(0, 'team_send', "Warp an item", 100, 0, NOW());


--
-- Kill normal mobs
--
insert into objectives values(0, 'kill_woodchuck',			"Kill a Woodchuck",				500,	5, NOW());
insert into objectives values(0, 'kill_gridbug',			"Kill a Grid bug",				500,	5, NOW());
insert into objectives values(0, 'kill_floatingeye',		"Kill a Floating Eye",			500,	5, NOW());
insert into objectives values(0, 'kill_littledog',			"Kill a Little Dog",			500,	5, NOW());
insert into objectives values(0, 'kill_gnomelord',			"Kill a Gnome Lord",			500,	5, NOW());
insert into objectives values(0, 'kill_smallmimic',			"Kill a Small Mimic",			500,	5, NOW());
insert into objectives values(0, 'kill_largemimic',			"Kill a Large Mimic",			500,	5, NOW());
insert into objectives values(0, 'kill_giantmimic',			"Kill a Giant Mimic",			500,	5, NOW());
insert into objectives values(0, 'kill_wererat',			"Kill a Wererat",				500,	5, NOW());
insert into objectives values(0, 'kill_soldierant',			"Kill a Soldier Ant",			500,	5, NOW());
insert into objectives values(0, 'kill_mumak',				"Kill a Mumak",					500,	5, NOW());
insert into objectives values(0, 'kill_cockatrice',			"Kill a Cockatrice",			1000,	5, NOW());
insert into objectives values(0, 'kill_warhorse',			"Kill a Warhorse",				1000,	5, NOW());
insert into objectives values(0, 'kill_blackdragon',		"Kill a Black Dragon",			1500,	5, NOW());
insert into objectives values(0, 'kill_mastermindflayer',	"Kill a Master Mind Flayer",	1500,	5, NOW());
insert into objectives values(0, 'kill_archlich',			"Kill an Archlich",				1500,	5, NOW());
insert into objectives values(0, 'kill_archon',				"Kill an Archon",				1500,	5, NOW());
insert into objectives values(0, 'kill_oracle',				"Kill the evil Oracle",			7500,	10, NOW());

--
-- Kill specials
--
insert into objectives values(0, 'kill_nem',		"Kill Nemesis",			1000, 5, NOW());
insert into objectives values(0, 'kill_medusa',		"Kill Medusa",			1000, 5, NOW());
insert into objectives values(0, 'kill_juiblex',	"Kill Juiblex",			1000, 5, NOW());
insert into objectives values(0, 'kill_orcus',		"Kill Orcus",			1000, 5, NOW());
insert into objectives values(0, 'kill_vlad',		"Kill Vlad",			1000, 5, NOW());
insert into objectives values(0, 'kill_wizard',		"Kill Wizard",			1000, 5, NOW());
insert into objectives values(0, 'kill_demogorgon', "Kill Demogorgon",		1000, 5, NOW());


--
-- Reach dungeon depth
--
insert into objectives values(0, 'reach_3',		"Reach dlvl 3",		500,	5, NOW());
insert into objectives values(0, 'reach_5',		"Reach dlvl 5",		500,	5, NOW());
insert into objectives values(0, 'reach_10',	"Reach dlvl 10",	500,	5, NOW());
insert into objectives values(0, 'reach_15',	"Reach dlvl 15",	500,	5, NOW());
insert into objectives values(0, 'reach_20',	"Reach dlvl 20",	500,	5, NOW());
insert into objectives values(0, 'reach_25',	"Reach dlvl 25",	500,	5, NOW());
insert into objectives values(0, 'reach_30',	"Reach dlvl 30",	500,	5, NOW());
insert into objectives values(0, 'reach_35',	"Reach dlvl 35",	500,	5, NOW());
insert into objectives values(0, 'reach_40',	"Reach dlvl 40",	500,	5, NOW());
insert into objectives values(0, 'reach_45',	"Reach dlvl 45",	500,	5, NOW());
insert into objectives values(0, 'reach_50',	"Reach dlvl 50",	500,	5, NOW());

--
-- Reach xlevel...
--
insert into objectives values(0, 'level_5',		"Get xlvl 5",	500,	5, NOW());
insert into objectives values(0, 'level_10',	"Get xlvl 10",	1000,	5, NOW());
insert into objectives values(0, 'level_14',	"Get xlvl 14",	1000,	5, NOW());
insert into objectives values(0, 'level_20',	"Get xlvl 20",	1000,	5, NOW());
insert into objectives values(0, 'level_30',	"Get xlvl 30",	2000,	5, NOW());

--
-- Ascension
--
insert into objectives values(0, 'get_aoy',			"Get Amulet of Yendor",	1000, 5, NOW());
insert into objectives values(0, 'ascend',			"Ascend",				20000, 5, NOW());
insert into objectives values(0, 'curse_ascend',	"Ascend with curse item", 2000, 0, NOW());


--
-- System user
--
insert into players values(0, "system", "", "prutt@korv.se", FALSE, null);


--
-- Hackshit
--
--- breggs hamps pellsson
--- niss steff marcs 
--- kalle kalle
--- erik, martin, filip
--- magnus
--- viktor
--- jens


-- pellsson karl b
-- breggan bjass
-- steff marco 
-- jens kalle
-- erik filip
-- magnus martin
-- hampus viktor



insert into players values(0, "pellsson", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "aransentin", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "valen", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "balmer00", md5("bajskorv"), "prutt@korv.se", FALSE, null);

insert into clans values(0, 1, 2, "Pantbanken", null);
insert into clan_members values(0, 1, 2, 'filled', null);
insert into clan_members values(0, 1, 3, 'filled', null);
insert into clan_members values(0, 1, 4, 'filled', null);
insert into clan_members values(0, 1, 5, 'filled', null);

insert into players values(0, "breggan", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "gorbiz", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "bJazz", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "balmer01", md5("bajskorv"), "prutt@korv.se", FALSE, null);

insert into clans values(0, 1, 2, "Nisses Lag", null);
insert into clan_members values(0, 2, 6, 'filled', null);
insert into clan_members values(0, 2, 7, 'filled', null);
insert into clan_members values(0, 2, 8, 'filled', null);
insert into clan_members values(0, 2, 9, 'filled', null);

insert into clans values(0, 1, 4, "Ronald McDonald Trump", null);
insert into players values(0, "Herde", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "Erik2", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "menvafan", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "CeleryMan", md5("12"), "prutt@korv.se", FALSE, null);
insert into players values(0, "balmer02", md5("bajskorv"), "prutt@korv.se", FALSE, null);

insert into clan_members values(0, 3, 10, 'filled', null);
insert into clan_members values(0, 3, 11, 'filled', null);
insert into clan_members values(0, 3, 12, 'filled', null);
insert into clan_members values(0, 3, 13, 'filled', null);
insert into clan_members values(0, 3, 14, 'filled', null);

-- insert into players values(0, "Najarana", md5("12"), "prutt@korv.se", FALSE, null);

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
	curse_remain int unsigned,
	wish_remain int unsigned,
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
	added timestamp,
	primary key(id));

create table questoffer(
	id int primary key auto_increment,
	clan_id int,
	otyp int,
	CONSTRAINT offershit UNIQUE (clan_id, otyp)
);

create table questticket(
	id int primary key auto_increment,
	player_id int,
	UNIQUE(player_id)
);

create table sharedbag(
	id int primary key auto_increment,
	clan_id int,
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
	removed int default 0,
	added timestamp default NOW(),
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

create table questions(
	id int not null auto_increment,
	q varchar(256) not null,
	a varchar(256) not null,
	primary key(id)
);

insert into questions (q,a) values("Which is heavier, a blessed horse or a cursed carrot?", "(blessed )?horse");
insert into questions (q,a) values("What monster symbol does a grid bug have?", "[xX]");
insert into questions (q,a) values("What object do you seek?", "(the |a |an )?amulet( of yendor)?");
insert into questions (q,a) values("There is a name so powerful that when engraved, monsters flee. What name?", "elbereth");
insert into questions (q,a) values("Name one of the four riders?", "(famine|death|pestilence|pestillence|war)");
insert into questions (q,a) values("", "ja");
insert into questions (q,a) values("", "ja");
insert into questions (q,a) values("What character has the fastest turn and real-time ascension on NAO?", "swaggin[zs]?");
insert into questions (q,a) values("Fisk?", ".*");
insert into questions (q,a) values("Fisk?", ".*");

--
-- Hack shit
--
insert into clan_state values(0, 1, 0, 3, 3, 1);
insert into clan_state values(0, 2, 0, 3, 3, 1);
insert into clan_state values(0, 3, 0, 3, 3, 1);

insert into player_state values(0, 1, 0, 0);
insert into player_state values(0, 2, 0, 0);
insert into player_state values(0, 3, 0, 0);
insert into player_state values(0, 4, 0, 0);
insert into player_state values(0, 5, 0, 0);
insert into player_state values(0, 6, 0, 0);
insert into player_state values(0, 7, 0, 0);
insert into player_state values(0, 8, 0, 0);
insert into player_state values(0, 9, 0, 0);
insert into player_state values(0, 10, 0, 0);
insert into player_state values(0, 11, 0, 0);


