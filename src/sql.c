#include "hack.h"

#include <mysql/mysql.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "sql.h"

#define SQL_SERVER		"127.0.0.1" // by ip, to force tcp sockets
#define SQL_USER		"nh"
#define SQL_PASS		"nh12"
#define SQL_DATABASE		"nhtour"

static char game_database[128];
static const char *active_db = SQL_DATABASE;
static MYSQL *conn;
static unsigned int player_id = 0;
static int num_clans = 0;
static struct clan_info clans[SQL_MAX_CLANS];
static struct clan_info *we = NULL;

const char *my_player_name(void)
{
	unsigned int i;
	for(i = 0; i < we->num_members; ++i)
	{
		if(player_id == we->members[i].player_id)
		{
			return we->members[i].name;
		}
	}
	return "unknown_err";
}

unsigned int sql_get_player_id(void)
{
	return player_id;
}

const struct clan_info *sql_get_my_team(void)
{
	return we;
}

static int sql_connect(void)
{
	return mysql_real_connect(conn, SQL_SERVER, SQL_USER, SQL_PASS, active_db, 0, NULL, 0) ? 0 : 1;
}

static const struct member *get_team_member(unsigned int id)
{
	int i = 0;

	for(i = 0; i < we->num_members; ++i)
	{
		if(we->members[i].player_id == id)
		{
			return &we->members[i];
		}
	}
	return NULL;
}

static const struct clan_info *get_team(unsigned int id)
{
	int i = 0;

	for(i = 0; i < num_clans; ++i)
	{
		if(clans[i].clan_id == id)
		{
			return &clans[i];
		}
	}

	return NULL;
}

static char *make_string(const char *p)
{
	unsigned long len;
	char *to;

	if(NULL == p)
	{
		return strdup("NULL");
	}

	len = strlen(p);

	if(NULL != (to = malloc((len * 2) + 2 + 1)))
	{
		*to = '"';
		mysql_real_escape_string(conn, to + 1, p, len);
		len = strlen(to);
		to[len] = '"';
		to[len + 1] = 0;
	}
	return to;
}

static MYSQL_RES *sql_direct_query(const char *query)
{
	//pline("%s", query);
	if(0 != mysql_real_query(conn, query, strlen(query)))
	{
		if(0 != sql_connect())
		{
			return NULL;
		}

		if(0 != mysql_real_query(conn, query, strlen(query)))
		{
			return NULL;
		}
	}
	return mysql_store_result(conn);
}

static MYSQL_RES *sql_vquery(const char *query, va_list args)
{
	MYSQL_RES *res;
	int len;
	char *temp;
	va_list start;

	va_copy(start, args);
	len = vsnprintf(NULL, 0, query, args);

	if(len < 0)
	{
		return NULL;
	}

	if(NULL == (temp = malloc(len + 1)))
	{
		return NULL;
	}
	
	va_copy(args, start);
	vsnprintf(temp, len + 1, query, args);
	res = sql_direct_query(temp);
	free(temp);

	return res;
}

static MYSQL_RES *sql_query(const char *query, ...)
{
	MYSQL_RES *res;

	va_list args;
	va_start(args, query);
	res = sql_vquery(query, args);
	va_end(args);

	return res;
}

static boolean sql_query_exists(const char *query, ...)
{
	boolean exists = FALSE;
	MYSQL_RES *res;

	va_list args;
	va_start(args, query);
	res = sql_vquery(query, args);
	va_end(args);

	if(NULL != res)
	{
		exists = (NULL != mysql_fetch_row(res)) ? TRUE : FALSE;
		mysql_free_result(res);	
	}

	return exists;
}

static int load_clan_members(int clan_id, struct clan_info *info)
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	if(NULL == (res = sql_query("select id, name from players where id = ANY (select player_id from clan_members where clan_members.clan_id=%d) limit %d", clan_id, SQL_MAX_MEMBERS)))
	{
		return 1;		
	}

	info->num_members = 0;

	while(NULL != (row = mysql_fetch_row(res)))
	{
		struct member *m = &info->members[info->num_members];

		m->player_id = atoi(row[0]);

		strncpy(m->name, row[1], sizeof(m->name));
		m->name[sizeof(m->name) - 1] = 0;

		++info->num_members;
	}

	mysql_free_result(res);
	return 0;
}

static int load_clan(unsigned int id, int index)
{
	int rc = 1;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	struct clan_info *c = &clans[index];

	if(NULL == (res = sql_query("select name from clans where clans.id=%d", id)))
	{
		goto cleanup;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		c->clan_id = id;
		strncpy(c->name, row[0], sizeof(c->name));
		c->name[sizeof(c->name) - 1] = 0;
		rc = load_clan_members(id, c);
	}

cleanup:
	if(res)
	{
		mysql_free_result(res);
	}
	return rc;
}

static int load_clans(void)
{
	int i;
	MYSQL_ROW row;
	MYSQL_RES *res = sql_query("select clan_id from game_participants where game_id=%d limit %d", context.game_id, SQL_MAX_CLANS);

	if(NULL == res)
	{
		pline("Internal SQL is broken.");
		return 1;
	}

	while(NULL != (row = mysql_fetch_row(res)))
	{
		if(0 != load_clan((unsigned int)atoi(row[0]), num_clans))
		{
			pline("Failed to load a clan. This shouldn't happen.");
			mysql_free_result(res);
			return 1;
		}
		++num_clans;
	}

	mysql_free_result(res);

	for(i = 0; i < num_clans; ++i)
	{
		if(clans[i].clan_id == context.clan_id)
		{
			we = &clans[i];
			break;
		}
	}

	if(NULL == we)
	{
		pline("Clan missing from game? Internal error.");
		return 1;
	}

	return 0;
}

static int is_game_ongoing(void)
{
	if(sql_query_exists("select starts from games where starts < NOW() and ends < NOW() and id=%d", context.game_id))
	{
		return 1;
	}

	return sql_query_exists("select starts from games where NOW() >= starts and NOW() < ends and id=%d", context.game_id) ? 0 : -1;
}

static int load_game(void)
{
	int rc = 1;
	MYSQL_RES *res;
	MYSQL_ROW row;

	if(NULL == (res = sql_query("select clans.id, clans.game_id from clans"
								"    inner join clan_members on (player_id=%d)"
								"  where clans.id=clan_members.clan_id", player_id)))
	{
		pline("Must be part of a clan in order to play here. To play vanilla nethack, please see http://nethack.alt.org");
		return 1;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		int ongoing;
		unsigned int clan_id = (unsigned int)atoi(row[0]);
		unsigned int game_id = (unsigned int)atoi(row[1]);

		rc = -1;

		if(0 == context.clan_id)
		{
			context.clan_id = clan_id;
		}
		else if(clan_id != context.clan_id)
		{
			pline("You have changed clan, your on-going game has be abandoned.");
			goto cleanup;
		}

		if(0 == context.game_id)
		{
			context.game_id = game_id;

			if(0 == game_id)
			{
				pline("Your clan must register for a game before playing.");
				goto cleanup;
			}
		}
		else if(game_id != context.game_id)
		{
			pline("Your clan has changed game, your on-going game has been abandoned.");
			goto cleanup;
		}

		ongoing = is_game_ongoing();

		if(ongoing < 0)
		{
			pline("The game your clan is registered with has not yet started.");
			goto cleanup;
		}
		else if(ongoing > 0)
		{
			pline("The game your clan is registered with has ended.");
			goto cleanup;
		}

		rc = load_clans();
	}
	else
	{
		pline("Failed to fetch row for clan.");
	}
cleanup:
	mysql_free_result(res);
	return rc;
}

static int load_all(const char *name)
{
	int rc = 1;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char *esc_name = make_string(name);

	if(NULL == esc_name)
	{
		pline("Allocation for name failed.");
		return 1;
	}

	res = sql_query("select id, banned from players where name=%s", esc_name);
	free(esc_name);

	if(NULL == res)
	{
		pline("Failed to select player.");
		return 1;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		if(0 != atoi(row[1]))
		{
			pline("You are banned.");
			rc = -1;
		}
		else
		{
			player_id = (unsigned int)atoi(row[0]);
			rc = load_game();
		}
	}
	else
	{
		pline("Failed to fetch for for player.");
	}

	mysql_free_result(res);
	return rc;
}

int sql_initialize(const char *name)
{
	int rc;

	if(NULL == (conn = mysql_init(NULL)))
	{
		pline("Failed to initialize MySQL.");
		return 1;
	}

	if(0 != sql_connect())
	{
		pline("Failed to connect to MySQL during init.");
		return 2;
	}

	if(0 != (rc = load_all(name)))
	{
		return rc;
	}

	sprintf(game_database, "%s_game_%d", SQL_DATABASE, context.game_id);
	active_db = game_database;

	if(0 != mysql_select_db(conn, active_db))
	{
		pline("Failed to select game database (internal error).");
		return 4;
	}

	return 0;
}

int sql_send_item(unsigned int to_id, unsigned int from_id, const struct obj *obj)
{
	MYSQL_RES *res;
	char *esc_name = make_string(obj->oextra ? obj->oextra->oname : NULL);

	sql_query("insert into deliveries values("
		"0, %d, 1, %d, "
		"%d, %d, %d, %d, "
		"%d, %d, %d, %d, "
		"%d, %d, %d, %d, "
		"%s, "
		"FALSE, %d, %d, NULL)",
		obj->otyp, obj->spe,
		obj->oclass, (obj->cursed ? 0 : (obj->blessed ? 2 : 1)), obj->oeroded, obj->oeroded2,
		obj->oerodeproof, obj->otrapped, obj->recharged, obj->greased,
		obj->corpsenm, obj->usecount, obj->oeaten, obj->age,
		esc_name,
		to_id, from_id);

	free(esc_name);
	return 0;
}

int sql_consume_delivery(const struct member **from, struct obj *obj, char *name)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int rc = 1;

	if(NULL == (res = sql_query("select * from deliveries where delivered=FALSE and to_player_id=%d order by added asc limit 1", player_id)))
	{
		return rc;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		unsigned int delivery_id = atoi(row[0]);

		sql_query("update deliveries set delivered=TRUE where id=%d", delivery_id);

		obj->otyp	= atoi(row[1]);
		obj->quan	= atoi(row[2]);
		obj->spe	= atoi(row[3]);

		obj->oclass		= atoi(row[4]); // shouldnt have to set this right...
		obj->cursed		= 0 == atoi(row[5]);
		obj->blessed	= 2 == atoi(row[5]);
		obj->oeroded	= atoi(row[6]);
		obj->oeroded2	= atoi(row[7]);

		obj->oerodeproof	= atoi(row[8]);
		obj->otrapped		= atoi(row[9]);
		obj->recharged		= atoi(row[10]);
		obj->greased		= atoi(row[11]);
		
		obj->corpsenm	= atoi(row[12]);
		obj->usecount	= atoi(row[13]);
		obj->oeaten		= atoi(row[14]);
		obj->age		= atoi(row[15]);

		if(name)
		{
			if(row[16])
			{
				strncpy(name, row[16], SQL_NAME_LENGTH);
				name[SQL_NAME_LENGTH - 1] = 0;
			}
			else
			{
				*name = 0;
			}
		}

		*from = get_team_member((unsigned int)atoi(row[19]));
		rc = 0;
	}

	mysql_free_result(res);
	return rc;
}

static int write_message(int type, const char *format, va_list args)
{
	char local_msg[128 + 1];
	char *esc_msg;

	unsigned int from_id = SQL_SYSTEM_PLAYER_ID;
	unsigned int clan_id = 0;

	if(CHAT_TYPE_CLAN == type)
	{
		from_id	= player_id;
		clan_id	= context.clan_id;
	}

	vsnprintf(local_msg, sizeof(local_msg), format, args);
	local_msg[sizeof(local_msg) - 1] = 0;

	if(NULL == (esc_msg = make_string(local_msg)))
	{
		return 1;
	}

	sql_query("insert into messages values(0, %d, %d, %d, %s, NOW())", clan_id, player_id, type, esc_msg);
	free(esc_msg);
}

int sql_write_message(const char *format, ...)
{
	int rc;
	va_list args;
	va_start(args, format);
	rc = write_message(CHAT_TYPE_CLAN, format, args);
	va_end(args);

	return 0;
}

int sql_write_announce(const char *format, ...)
{
	int rc;
	va_list args;
	va_start(args, format);
	rc = write_message(CHAT_TYPE_ANNOUNCE, format, args);
	va_end(args);

	return rc;
}

int sql_write_critical(const char *format, ...)
{
	int rc;
	va_list args;
	va_start(args, format);
	rc = write_message(CHAT_TYPE_CRITICAL, format, args);
	va_end(args);

	return rc;
}

int sql_get_messages(struct chat_history *history, unsigned int offset, unsigned int count)
{
	unsigned int i;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char player_name[SQL_NAME_LENGTH];

	if(count > SQL_MAX_HISTORY)
	{
		count = SQL_MAX_HISTORY;
	}

	if(NULL == (res = sql_query("select id, player_id, message, msg_type, unix_timestamp(messages.added) as added"
					" from messages"
					"   where clan_id=%d or msg_type<>0"
					"   order by id desc limit %d, %d",
					context.clan_id, offset, count)))
	{
		return 1;
	}

	history->num_messages = 0;

	while(NULL != (row = mysql_fetch_row(res)))
	{
		const struct member *from;
		struct chat_message *const c = &history->history[history->num_messages++];

		c->id = (unsigned int)atoi(row[0]);

		from = get_team_member(atoi(row[1]));

		if(NULL != from)
		{
			strncpy(c->name, from->name, sizeof(c->name));
			c->name[sizeof(c->name) - 1] = 0;
		}
		else
		{
			c->name[0] = 0;
		}
		
		if(c->msg)
		{
			strncpy(c->msg, row[2], sizeof(c->msg));
			c->msg[sizeof(c->msg) - 1] = 0;
		}
		else
		{
			c->msg[0] = 0;
		}

		c->type = atoi(row[3]);
		c->added = (unsigned int)atoi(row[4]);
	}

	mysql_free_result(res);
	return 0;
}

static int get_unread(unsigned int msg_id, unsigned int *most_critical, unsigned int *count, int update)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
 
	res = sql_query("select count(*), ifnull(max(id), 0), ifnull(max(msg_type), 0) from messages"
					" where (clan_id=%d or msg_type<>0) and id>%d", context.clan_id, msg_id);

	if(NULL == res)
	{
		return 1;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		*count = atoi(row[0]);

		if(*count)
		{
			if(most_critical)
			{
				*most_critical = (unsigned int)atoi(row[2]);
			}

			if(update)
			{
				unsigned int high_id = (unsigned int)atoi(row[1]);
				sql_query("update player_state set message_id_seen=%d where player_id=%d", high_id, player_id);
			}
		}
	}

	mysql_free_result(res);
	return 0;
}

int sql_get_unread(unsigned int *most_critical, unsigned int *count, int update)
{
	MYSQL_ROW row;
	MYSQL_RES *res;
	int rc = 1;
	unsigned int from_msg_id = 0;

	res = sql_query("select message_id_seen from player_state where player_id=%d limit 1", player_id);

	if(NULL == res)
	{
		return 1;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		from_msg_id = (unsigned int)atoi(row[0]);
	}

	mysql_free_result(res);
	return get_unread(from_msg_id, most_critical, count, update);
}

int sql_set_player_ingame(int ingame)
{
	sql_query("update player_state set ingame=%d where player_id=%d", ingame ? 1 : 0, player_id);
	return 0;
}

int sql_next_mail(char *from, char *message, int consume)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int rc = 1;

	if(NULL == (res = sql_query("select id, from_name, body from nhtour.mails"
					"    where to_player_id=%d and delivered=FALSE"
					"    order by added asc limit 1", player_id)))
	{
		return 1;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		if(consume)
		{
			sql_query("update nhtour.mails set delivered=TRUE where id=%d", atoi(row[0]));
		}
		if(from)
		{
			strncpy(from, row[1], SQL_NAME_LENGTH);
			from[SQL_NAME_LENGTH - 1] = 0;			
		}
		if(message)
		{
			strncpy(message, row[2], 79);
			from[79] = 0;
		}
		rc = 0;
	}
	else
	{
		/* If this happens, we are probably stuck in a retarded forever loop */
		/* Might want to purge all mails to user here */
	}

	mysql_free_result(res);
	return rc;
}


int sql_make_donation(unsigned int gold)
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	sql_query("lock tables clan_state write");

	if(NULL == (res = sql_query("select gold from clan_state where clan_id=%d", context.clan_id)))
	{
		goto unlock_tables;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		unsigned int total = (unsigned int)atoi(row[0]) + gold;
		sql_write_critical("%s has donated %d g (total: %d)", we->name, gold, total);

		sql_query("update clan_state set gold=%d where clan_id=%d", total, context.clan_id);
	}

	mysql_free_result(res);
unlock_tables:
	sql_query("unlock tables");
	return 0;
}

const struct clan_info *sql_get_clans(int *count)
{
	*count = num_clans;
	return clans;	
}

static int purchase_item(unsigned int amount, const char *item)
{
	int rc = 1;
	MYSQL_RES *res;
	MYSQL_ROW row;

	sql_query("lock tables clans write");

	if(NULL == (res = sql_query("select gold, %s_remain from clan_state where clan_id=%d", item, context.clan_id)))
	{
		goto unlock_tables;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		unsigned int current_gold = (unsigned int)atoi(row[0]);
		unsigned int items_left = (unsigned int)atoi(row[1]);

		if(0 != items_left && current_gold >= amount)
		{
			sql_query("update clan_state set gold=%d, %s_remain=%d where clan_id=%d", 
				(current_gold - amount), item, (items_left - 1), context.clan_id);
			rc = 0;
		}
	}

	mysql_free_result(res);
unlock_tables:
	sql_query("unlock tables");
	return rc;
}

int sql_wish_clan(void)
{
	return purchase_item(SQL_GOLD_TO_WISH, "wish");
}

int sql_curse_clan(unsigned int to_clan_id)
{
	int i;
	const struct clan_info *c = get_team(to_clan_id);

	if(NULL == c)
	{
		return 1;
	}

	if(0 != purchase_item(SQL_GOLD_TO_CURSE, "curse"))
	{
		return 2;
	}

	for(i = 0; i < c->num_members; ++i)
	{
		sql_query("insert into curses values(0, %d, %d, %d, NOW())",
			c->members[i].player_id, context.clan_id, SQL_CURSE_DELAY);
	}
	return 0;	
}

int sql_decrement_curse_turns(int *turns_left, const char **by)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int rc = 1;

	const struct clan_info *cursed_by;

	*turns_left = -1;

	if(NULL == (res = sql_query("select id, turns_left, from_clan_id from curses where player_id=%d and turns_left>=0 order by turns_left asc limit 1", player_id)))
	{
		return 1;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		*turns_left = atoi(row[1]);

		if(NULL != (cursed_by = get_team(atoi(row[2]))))
		{
			*by = cursed_by->name;
		}
		else
		{
			*by = "an unknown source";
		}

		sql_query("update curses set turns_left=%d where id=%d", *turns_left - 1, atoi(row[0]));
		rc = 0;
	}

	mysql_free_result(res);
	return rc;
}

int sql_get_clan_state(unsigned int clan_id, struct clan_state *state)
{
	int rc = 1;
	MYSQL_RES *res = sql_query("select gold, can_be_cursed from clan_state where clan_id=%d limit 1", clan_id);
	MYSQL_ROW row;

	state->gold = 0;
	state->can_be_cursed = FALSE;

	if(res && NULL != (row = mysql_fetch_row(res)))
	{
		state->gold = atoi(row[0]);
		state->can_be_cursed = atoi(row[1]);
		rc = 0;
	}

	if(res)
	{
		mysql_free_result(res);
	}
	return 0;
}

int sql_get_player_state(unsigned int player_id, struct player_state *state)
{
	int rc = 1;
	MYSQL_RES *res = sql_query("select ingame, message_id_seen from player_state where player_id=%d limit 1", player_id);
	MYSQL_ROW row;

	state->is_ingame = 0;
	state->message_id_seen = 0;

	if(res && NULL != (row = mysql_fetch_row(res)))
	{
		state->is_ingame = atoi(row[0]);
		state->message_id_seen = atoi(row[1]);
		rc = 0;
	}

	if(res)
	{
		mysql_free_result(res);
	}
	return 0;
}

static unsigned int insert_score(unsigned int objective_id, unsigned int score, unsigned int decrease_rate)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	unsigned int value = 0;
	unsigned int times_completed = 0;
	unsigned int divider;

	res = sql_query("select count(*) from score"
					" where objective_id=%d and player_id=%d and clan_id=%d",
					objective_id, player_id, we->clan_id);

	if(NULL == res)
	{
		return 0;
	}
	
	if(NULL != (row = mysql_fetch_row(res)))
	{
		times_completed = (unsigned int)atoi(row[0]);
		divider = (0 == decrease_rate)
			? 1
			: (decrease_rate * times_completed);
		
		value = (0 == times_completed)
			? score
			: (score / divider);

		sql_query("insert into score values(0, %d, %d, %d, %d, NOW())",
			objective_id, player_id, we->clan_id, value);
	}

	mysql_free_result(res);
	return (0 == times_completed) ? value : 0;
}

int sql_complete_objective(const char *category, const char *objective)
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	char full[256];
	char *esc_full;

	if(!category)
	{
		return 3;
	}

	if(objective)
	{
		sprintf(full, "%s_%s", category, objective);
	}
	else
	{
		strncpy(full, category, sizeof(full));
		full[sizeof(full) - 1] = 0;
	}

	if(NULL == (esc_full = make_string(full)))
	{
		return 1;
	}

	res = sql_query("select id, literal, score, decrease_rate from nhtour.objectives where name=%s", esc_full);
	free(esc_full);

	if(NULL == res)
	{
		return 2;
	}

	if(NULL != (row = mysql_fetch_row(res)))
	{
		unsigned int id				= atoi(row[0]);
		const char *literal			= row[1];
		unsigned int score			= atoi(row[2]);
		unsigned int decrease_rate	= atoi(row[3]);

		unsigned int value = insert_score(id, score, decrease_rate);
		
		if(0 != value)
		{
			sql_write_announce("Player '%s' completed '%s' for %d points!", my_player_name(), literal, value);
		}
	}

	mysql_free_result(res);
	return 0;
}

int sql_quest_completed(void)
{
	int completed = 0;
	MYSQL_RES *res = sql_query("select distinct player_id from score where clan_id=%d and objective_id=(select id from nhtour.objectives where name='kill_oracle' limit 1) group by player_id", we->clan_id);

	if(NULL == res)
	{
		return 0;
	}

	completed = (((int)mysql_num_rows(res)) >= we->num_members);
	mysql_free_result(res);

	return completed;
}

int sql_add_bag_item(struct obj *obj)
{
	MYSQL_RES *res;
	char *esc_name = make_string(obj->oextra ? obj->oextra->oname : NULL);

	res = sql_query("insert into sharedbag ("
		"clan_id, otyp, quan, spe, "
		"oclass, buc, oeroded, oeroded2, "
		"oerodeproof, otrapped, recharged, greased, "
		"corpsenm, usecount, oeaten, age, named) values("
		"%d, %d, %d, %d, "
		"%d, %d, %d, %d, "
		"%d, %d, %d, %d, "
		"%d, %d, %d, %d, "
		"%s)",
		we->clan_id, obj->otyp, obj->quan, obj->spe,
		obj->oclass, (obj->cursed ? 0 : (obj->blessed ? 2 : 1)), obj->oeroded, obj->oeroded2,
		obj->oerodeproof, obj->otrapped, obj->recharged, obj->greased,
		obj->corpsenm, obj->usecount, obj->oeaten, obj->age,
		esc_name);

	obj->dbid = (int)mysql_insert_id(conn);
	// pline("ID: %d", obj->dbid);

	if(res)
	{
		mysql_free_result(res);
	}

	free(esc_name);	
}

#define next_col row[row_idx++]

int sql_sync_bag_content(struct obj *bag)
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	struct obj *otmp;

	if(NULL == (res = sql_query("SELECT "
		"id, otyp, quan, spe, "
		"oclass, buc, oeroded, oeroded2, "
		"oerodeproof, otrapped, recharged, greased, "
		"corpsenm, usecount, oeaten, age, named FROM sharedbag where clan_id=%d and removed=0", we->clan_id)))
	{
		return -1;
	}

    while((otmp = bag->cobj) != 0) {
    	otmp->dbid = 0;
        obj_extract_self(otmp);
        obfree(otmp, 0);
    }

	while(NULL != (row = mysql_fetch_row(res)))
	{
		int row_idx = 0;
		int has_it = 0;
		unsigned int dbid = atoi(next_col);

		if(NULL == (otmp = mksobj(atoi(next_col), FALSE, FALSE)))
		{
			// Log this shit....
			continue;
		}
		
		otmp->dbid		= dbid;
		otmp->quan		= atoi(next_col);
		otmp->spe		= atoi(next_col);
		otmp->oclass	= atoi(next_col);
		
		switch(atoi(next_col))
		{
			case 0:
				curse(otmp);
				break;
			case 1:
				uncurse(otmp);
				unbless(otmp);
				break;
			case 2:
			default:
				bless(otmp);
				break;
		}

		otmp->oeroded		= atoi(next_col);
		otmp->oeroded2		= atoi(next_col);
		otmp->oerodeproof	= atoi(next_col);
		otmp->otrapped		= atoi(next_col);
		otmp->recharged		= atoi(next_col);
		otmp->greased		= atoi(next_col);
		otmp->corpsenm		= atoi(next_col);
		otmp->usecount		= atoi(next_col);
		otmp->oeaten		= atoi(next_col);
		otmp->age			= atoi(next_col);
	
		const char *name = next_col;

		if(name)
		{
			oname(otmp, name);
		}

		otmp->owt = weight(otmp);

		otmp->where = OBJ_CONTAINED;
		otmp->ocontainer = bag;
		otmp->nobj = bag->cobj;
		bag->cobj = otmp;
	}

    bag->owt = weight(bag);
    pline("Bag weight: %d", bag->owt);
	mysql_free_result(res);
	return 0;
}

int sql_remove_bag_item(struct obj *obj)
{
	int rc = 0;
	MYSQL_RES *res;
	MYSQL_ROW row;

	sql_query("lock tables sharedbag");

	res = sql_query("SELECT quan FROM sharedbag WHERE id=%d AND removed=0 LIMIT 1", obj->dbid);
	
	if(res)
	{
		if(NULL != (row = mysql_fetch_row(res)))
		{
			obj->quan = atoi(row[0]);

			MYSQL_RES *kek = sql_query("UPDATE sharedbag SET removed=1 WHERE id=%d AND removed=0", obj->dbid);

			if(kek)
			{
				mysql_free_result(kek);
			}

			rc = 1;
		}
		mysql_free_result(res);
	}
	
	sql_query("unlock tables");
	return rc;
}

int sql_split_bag_item(unsigned int dbid, int adjust)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int rc = 0;

	sql_query("lock tables sharedbag");

	if(adjust < 0)
	{
		res = sql_query("SELECT quan FROM sharedbag WHERE id=%d AND clan_id=%d AND quan >= %d AND removed=0 LIMIT 1", dbid, we->clan_id, adjust * -1);	
	}
	else
	{
		res = sql_query("SELECT quan FROM sharedbag WHERE id=%d AND clan_id=%d AND removed=0 LIMIT 1", dbid, we->clan_id);	
	}

	if(res)
	{
		if(NULL != (row = mysql_fetch_row(res)))
		{
			int new_quan = atoi(row[0]) + adjust;
			MYSQL_RES *kek;

			if(new_quan <= 0) // cant be less?
			{
				kek = sql_query("UPDATE sharedbag SET removed=1 WHERE id=%d AND clan_id=%d", dbid, we->clan_id);
			}
			else
			{
				kek = sql_query("UPDATE sharedbag SET quan=%d WHERE id=%d AND clan_id=%d", new_quan, dbid, we->clan_id);	
			}

			if(kek)
			{
				mysql_free_result(kek);
			}

			rc = 1;
		}

		mysql_free_result(res);
	}

	sql_query("unlock tables");
	return rc;
}
