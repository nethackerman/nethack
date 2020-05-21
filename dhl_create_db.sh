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
insert into objectives values(0, 'reach_minetn',        "Reach Minetown",               1000, 5, NOW());
insert into objectives values(0, 'reach_soko4',         "Reach Sokoban",                 500, 5, NOW());
insert into objectives values(0, 'reach_minend',        "Reach Mines' end",              500, 5, NOW());
insert into objectives values(0, 'reach_knox',          "Reach Ludios",                 1000, 5, NOW());
insert into objectives values(0, 'reach_strt',          "Reach Quest",                  1000, 5, NOW());
insert into objectives values(0, 'reach_medusa',        "Reach Medusa",                 1000, 5, NOW());
insert into objectives values(0, 'reach_castle',        "Reach Castle",                 1000, 5, NOW());
insert into objectives values(0, 'reach_juiblex',       "Reach Juiblex",                1000, 5, NOW());
insert into objectives values(0, 'reach_orcus',         "Reach Orcus",                  1000, 5, NOW());
insert into objectives values(0, 'reach_tower1',        "Reach Vlad's Tower",   1000, 5, NOW());
insert into objectives values(0, 'reach_sanctum',       "Reach Sanctum",                1000, 5, NOW());
insert into objectives values(0, 'reach_earth',         "Reach Earth Plane",    1000, 5, NOW());
insert into objectives values(0, 'reach_air',           "Reach Air Plane",              1000, 5, NOW());
insert into objectives values(0, 'reach_fire',          "Reach Fire Plane",             1000, 5, NOW());
insert into objectives values(0, 'reach_water',         "Reach Water Plane",    1000, 5, NOW());
insert into objectives values(0, 'reach_astral',        "Reach Astral Plane",   1000, 5, NOW());
insert into objectives values(0, 'reach_vinst1',        "Reach Oracle Quest 1", 1000, 5, NOW());
insert into objectives values(0, 'reach_vinst2',        "Reach Oracle Quest 2", 1000, 5, NOW());
insert into objectives values(0, 'reach_vinst3',        "Reach Oracle Quest 3", 1000, 5, NOW());

--
-- Misc goals
--
insert into objectives values(0, 'complete_soko',       "Complete Sokoban",             1000, 5, NOW());


--
-- Curses
--
insert into objectives values(0, 'curse_clan',          "Curse another clan",     4000, 0, NOW());
insert into objectives values(0, 'curse_kill',          "Kill with curse",         250, 0, NOW());
--
-- Team play
--
insert into objectives values(0, 'team_send', "Warp an item", 100, 0, NOW());


--
-- Kill normal mobs
--
insert into objectives values(0, 'kill_woodchuck',                      "Kill a Woodchuck",                             500,    5, NOW());
insert into objectives values(0, 'kill_gridbug',                        "Kill a Grid bug",                              500,    5, NOW());
insert into objectives values(0, 'kill_floatingeye',            "Kill a Floating Eye",                  500,    5, NOW());
insert into objectives values(0, 'kill_littledog',                      "Kill a Little Dog",                    500,    5, NOW());
insert into objectives values(0, 'kill_gnomelord',                      "Kill a Gnome Lord",                    500,    5, NOW());
insert into objectives values(0, 'kill_smallmimic',                     "Kill a Small Mimic",                   500,    5, NOW());
insert into objectives values(0, 'kill_largemimic',                     "Kill a Large Mimic",                   500,    5, NOW());
insert into objectives values(0, 'kill_giantmimic',                     "Kill a Giant Mimic",                   500,    5, NOW());
insert into objectives values(0, 'kill_wererat',                        "Kill a Wererat",                               500,    5, NOW());
insert into objectives values(0, 'kill_soldierant',                     "Kill a Soldier Ant",                   500,    5, NOW());
insert into objectives values(0, 'kill_mumak',                          "Kill a Mumak",                                 500,    5, NOW());
insert into objectives values(0, 'kill_cockatrice',                     "Kill a Cockatrice",                    1000,   5, NOW());
insert into objectives values(0, 'kill_warhorse',                       "Kill a Warhorse",                              1000,   5, NOW());
insert into objectives values(0, 'kill_blackdragon',            "Kill a Black Dragon",                  1500,   5, NOW());
insert into objectives values(0, 'kill_mastermindflayer',       "Kill a Master Mind Flayer",    1500,   5, NOW());
insert into objectives values(0, 'kill_archlich',                       "Kill an Archlich",                             1500,   5, NOW());
insert into objectives values(0, 'kill_archon',                         "Kill an Archon",                               1500,   5, NOW());
insert into objectives values(0, 'kill_oracle',                         "Kill the evil Oracle",                 7500,   10, NOW());

--
-- Kill specials
--
insert into objectives values(0, 'kill_nem',            "Kill Nemesis",                 1000, 5, NOW());
insert into objectives values(0, 'kill_medusa',         "Kill Medusa",                  1000, 5, NOW());
insert into objectives values(0, 'kill_juiblex',        "Kill Juiblex",                 1000, 5, NOW());
insert into objectives values(0, 'kill_orcus',          "Kill Orcus",                   1000, 5, NOW());
insert into objectives values(0, 'kill_vlad',           "Kill Vlad",                    1000, 5, NOW());
insert into objectives values(0, 'kill_wizard',         "Kill Wizard",                  1000, 5, NOW());
insert into objectives values(0, 'kill_demogorgon', "Kill Demogorgon",          1000, 5, NOW());


--
-- Reach dungeon depth
--
insert into objectives values(0, 'reach_3',     "Reach dlvl 3",         500,    5, NOW());
insert into objectives values(0, 'reach_5',     "Reach dlvl 5",         500,    5, NOW());
insert into objectives values(0, 'reach_10',    "Reach dlvl 10",        500,    5, NOW());
insert into objectives values(0, 'reach_15',    "Reach dlvl 15",        500,    5, NOW());
insert into objectives values(0, 'reach_20',    "Reach dlvl 20",        500,    5, NOW());
insert into objectives values(0, 'reach_25',    "Reach dlvl 25",        500,    5, NOW());
insert into objectives values(0, 'reach_30',    "Reach dlvl 30",        500,    5, NOW());
insert into objectives values(0, 'reach_35',    "Reach dlvl 35",        500,    5, NOW());
insert into objectives values(0, 'reach_40',    "Reach dlvl 40",        500,    5, NOW());
insert into objectives values(0, 'reach_45',    "Reach dlvl 45",        500,    5, NOW());
insert into objectives values(0, 'reach_50',    "Reach dlvl 50",        500,    5, NOW());

--
-- Reach xlevel...
--
insert into objectives values(0, 'level_5',     "Get xlvl 5",   500,    5, NOW());
insert into objectives values(0, 'level_10',    "Get xlvl 10",  1000,   5, NOW());
insert into objectives values(0, 'level_14',    "Get xlvl 14",  1000,   5, NOW());
insert into objectives values(0, 'level_20',    "Get xlvl 20",  1000,   5, NOW());
insert into objectives values(0, 'level_30',    "Get xlvl 30",  2000,   5, NOW());

--
-- Ascension
--
insert into objectives values(0, 'get_aoy',     "Get Amulet of Yendor", 1000, 5, NOW());
insert into objectives values(0, 'ascend',      "Ascend",               20000, 5, NOW());
insert into objectives values(0, 'curse_ascend', "Ascend with curse item", 2000, 0, NOW());


--
-- System user
--
insert into players values(0, "system", "", "prutt@korv.se", FALSE, null);
set @system_id = LAST_INSERT_ID();

--
-- Hackshit
--

-- Pellsson & Karl b
insert into players values(0, "pellsson", md5("12"), "prutt@korv.se", FALSE, null);
set @pellsson_id = LAST_INSERT_ID();

-- karl bergstrom
insert into players values(0, "kae", md5("12"), "prutt@korv.se", FALSE, null);
set @kae_id = LAST_INSERT_ID();

-- breggan
insert into players values(0, "breggan", md5("12"), "prutt@korv.se", FALSE, null);
set @breggan_id = LAST_INSERT_ID();

-- bajset
insert into players values(0, "bJazz", md5("12"), "prutt@korv.se", FALSE, null);
set @bjazz_id = LAST_INSERT_ID();

-- STEFANHELLSBORN
insert into players values(0, "CeleryMan", md5("12"), "prutt@korv.se", FALSE, null);
set @celeryman_id = LAST_INSERT_ID();

-- Patrick bror
insert into players values(0, "pumpkinman", md5("12"), "prutt@korv.se", FALSE, null);
set @pumpkinman_id = LAST_INSERT_ID();

-- Macroman
insert into players values(0, "Najarana", md5("12"), "prutt@korv.se", FALSE, null);
set @najarana_id = LAST_INSERT_ID();

-- Jensson
insert into players values(0, "Aransentin", md5("12"), "prutt@korv.se", FALSE, null);
set @aransentin_id = LAST_INSERT_ID();

-- Kalle kurt
insert into players values(0, "gorbiz", md5("12"), "prutt@korv.se", FALSE, null);
set @gorbiz_id = LAST_INSERT_ID();

-- Erik1
insert into players values(0, "menvafan", md5("12"), "prutt@korv.se", FALSE, null);
set @menvafan_id = LAST_INSERT_ID();

-- Erik2 (björn)
insert into players values(0, "erik2", md5("12"), "prutt@korv.se", FALSE, null);
set @erik2_id = LAST_INSERT_ID();

-- Magnus
insert into players values(0, "Magnum", md5("12"), "prutt@korv.se", FALSE, null);
set @magnum_id = LAST_INSERT_ID();

-- nils (björn)
insert into players values(0, "nils", md5("12"), "prutt@korv.se", FALSE, null);
set @nils_id = LAST_INSERT_ID();

-- Martin
insert into players values(0, "erik4", md5("12"), "prutt@korv.se", FALSE, null);
set @erik4_id = LAST_INSERT_ID();

-- DET AR HAMPOS
insert into players values(0, "Herde", md5("12"), "prutt@korv.se", FALSE, null);
set @herde_id = LAST_INSERT_ID();

-- viktor
insert into players values(0, "Viktor", md5("12"), "prutt@korv.se", FALSE, null);
set @viktor_id = LAST_INSERT_ID();

insert into players values(0, "balmer00", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer00 = LAST_INSERT_ID();
insert into players values(0, "balmer01", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer01 = LAST_INSERT_ID();
insert into players values(0, "balmer02", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer02 = LAST_INSERT_ID();
insert into players values(0, "balmer03", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer03 = LAST_INSERT_ID();
insert into players values(0, "balmer04", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer04 = LAST_INSERT_ID();
insert into players values(0, "balmer05", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer05 = LAST_INSERT_ID();
insert into players values(0, "balmer06", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer06 = LAST_INSERT_ID();
insert into players values(0, "balmer07", md5("bajskorv"), "prutt@korv.se", FALSE, null);
set @blamer07 = LAST_INSERT_ID();

insert into clans (game_id, owner_id, name, added) values (1, @pellsson_id, "The Vinnarna", NOW());
set @clan0_id = LAST_INSERT_ID();
insert into clan_members (clan_id, player_id, slot_type, added) values
        (@clan0_id, @pellsson_id, 'filled', NOW()),
        (@clan0_id, @kae_id, 'filled', NOW()),
        (@clan0_id, @aransentin_id, 'filled', NOW()),
        (@clan0_id, @balmer00_id, 'filled', NOW())
;

insert into clans (game_id, owner_id, name, added) values (1, @breggan_id, "Herdens stund", NOW());
set @clan1_id = LAST_INSERT_ID();
insert into clan_members (clan_id, player_id, slot_type, added) values
        (@clan1_id, @breggan_id, 'filled', NOW()),
        (@clan1_id, @herde_id, 'filled', NOW()),
        (@clan1_id, @viktor_id, 'filled', NOW()),
        (@clan1_id, @balmer01_id, 'filled', NOW())
;

insert into clans (game_id, owner_id, name, added) values (1, @menvafan_id, "Roffarna", NOW());
set @clan2_id = LAST_INSERT_ID();
insert into clan_members (clan_id, player_id, slot_type, added) values 
        (@clan2_id, @menvafan_id, 'filled', NOW()),
        (@clan2_id, @celeryman_id, 'filled', NOW()),
        (@clan2_id, @pumpkinman_id, 'filled', NOW()),
        (@clan2_id, @balmer02_id, 'filled', NOW())
;

insert into clans (game_id, owner_id, name, added) values (1, @erik2_id, "Loffarna", NOW());
set @clan3_id = LAST_INSERT_ID();
insert into clan_members (clan_id, player_id, slot_type, added) values 
        (@clan3_id, @erik2_id, 'filled', NOW()),
        (@clan3_id, @nils_id, 'filled', NOW()),
        (@clan3_id, @balmer03_id, 'filled', NOW())
;

---WTFFF ISTHis shit??:L::::;/vvvv


insert into games values(0, "Test Tournament", NOW(), DATE_ADD(NOW(), INTERVAL 2400 HOUR), NOW());
insert into game_participants (game_id, clan_id, added) values (1, @clan0_id, NOW());
insert into game_participants (game_id, clan_id, added) values (1, @clan1_id, NOW());
insert into game_participants (game_id, clan_id, added) values (1, @clan2_id, NOW());
insert into game_participants (game_id, clan_id, added) values (1, @clan3_id, NOW());
-- insert into game_participants (game_id, clan_id, added) values (1, @clan4_id, NOW());
-- insert into game_participants (game_id, clan_id, added) values (1, @clan5_id, NOW());
-- insert into game_participants (game_id, clan_id, added) values (1, @clan6_id, NOW());

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
        added timestamp default NOW());

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
insert into questions (q,a) values("What intrinsic (resistance) is helpful against an energy vortex?", "(lightning|shock)( resistance| resistant| intrinsic)?");
insert into questions (q,a) values("\"You hear the chime of a cash register\", why?", "shop");
insert into questions (q,a) values("What character has the fastest turn and real-time ascension on NAO?", "swaggin[zs]?");
insert into questions (q,a) values("What attribute affects your hitpoints?", "con?(stitution)?");
insert into questions (q,a) values("What is the highest possible strength?", "25");

--
-- Hack shit
--
insert into clan_state values(0, 1, 0, 3, 3, 1);
insert into clan_state values(0, 2, 0, 3, 3, 1);
insert into clan_state values(0, 3, 0, 3, 3, 1);
insert into clan_state values(0, 4, 0, 3, 3, 1);

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
