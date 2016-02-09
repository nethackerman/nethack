#ifndef SQL_H
#define SQL_H

#define SQL_NAME_LENGTH 64
#define SQL_MAX_MEMBERS	5
#define SQL_MAX_HISTORY 24
#define SQL_MAX_CLANS	256

#define SQL_SYSTEM_PLAYER_ID 1

#define CHAT_TYPE_CLAN		0
#define CHAT_TYPE_ANNOUNCE	1
#define CHAT_TYPE_CRITICAL	2

#define SQL_CURSE_DELAY		20
#define SQL_GOLD_TO_CURSE	1000

struct member
{
	unsigned int player_id;
	char name[SQL_NAME_LENGTH];
};

struct player_state
{
	unsigned int message_id_seen;
	int is_ingame;
};

struct clan_state
{
	int can_be_cursed;
	unsigned int gold;
};

struct clan_info
{
	char name[128];
	unsigned int clan_id;
	int num_members;
	struct member members[SQL_MAX_MEMBERS];
};

struct chat_message
{
	unsigned int id;
	int type;
	char name[SQL_NAME_LENGTH];
	char msg[128];
	unsigned int added;
};

struct chat_history
{
	int num_messages;
	struct chat_message history[SQL_MAX_HISTORY];
};

const struct clan_info *sql_get_clans(int *count);

unsigned int sql_get_player_id(void);
const struct clan_info *sql_get_my_team(void);

int sql_initialize(const char *name);
int sql_send_item(unsigned int to_id, unsigned int from_id, const struct obj *obj);
int sql_consume_delivery(const struct member **from, struct obj *obj, char *name);
int sql_write_message(const char *format, ...);
int sql_write_announce(const char *format, ...);
int sql_write_critical(const char *format, ...);
int sql_get_messages(struct chat_history *history, unsigned int offset, unsigned int count);
int sql_get_unread(unsigned int *most_critical, unsigned int *count, int update);
int sql_set_player_ingame(int ingame);
int sql_next_mail(char *from, char *message, int consume);
int sql_make_donation(unsigned int gold);
int sql_curse_clan(unsigned int to_clan_id);
int sql_get_clan_state(unsigned int clan_id, struct clan_state *state);
int sql_get_player_state(unsigned int player_id, struct player_state *state);
int sql_decrement_curse_turns(int *turns_left, const char **by);
int sql_complete_objective(const char *category, const char *objective);

#endif



