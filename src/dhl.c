#include "hack.h"
#include "dhl.h"
#include "sql.h"
#include "mail.h"

static struct obj *get_item(char *from)
{
	const struct member *memb;

	struct obj *obj;
	struct obj delivery;
	char name[SQL_NAME_LENGTH + 1];

	if(0 != sql_consume_delivery(&memb, &delivery, name))
	{
		return NULL;
	}

	if(memb)
	{
		snprintf(from, SQL_NAME_LENGTH, "from %s", memb->name);
		from[SQL_NAME_LENGTH - 1] = 0;
	}
	else
	{
		*from = 0;
	}

	if(NULL == (obj = mksobj(delivery.otyp, FALSE, FALSE)))
	{
		if(WRATH_OF_DEMOGORGON == delivery.otyp)
		{
			unleash_the_wrath(NULL);
			return NULL;
		}
		else
		{
			pline("Playful sprites ate your item.");
			return NULL;
		}
	}

	obj->quan	= delivery.quan;
	obj->spe	= delivery.spe;
	obj->oclass	= delivery.oclass; // shouldnt have to set this right...
	if(delivery.cursed)
	{
		curse(obj);
	}
	else if(delivery.blessed)
	{
		bless(obj);
	}
	obj->oeroded		= delivery.oeroded;
	obj->oeroded2		= delivery.oeroded2;
	obj->oerodeproof	= delivery.oerodeproof;
	obj->otrapped		= delivery.otrapped;
	obj->recharged		= delivery.recharged;
	obj->greased		= delivery.greased;
	obj->corpsenm		= delivery.corpsenm;
	obj->usecount		= delivery.usecount;
	obj->oeaten			= delivery.oeaten;
	obj->age			= delivery.age;
	oname(obj, name);

	return obj;
}

void dhl_update(void)
{
	struct monst *dhl = NULL;
	coord start;
	coord stop;
	struct obj *package;
	struct obj *obj;
	char from[SQL_NAME_LENGTH + 1];

	if(NULL == (obj = get_item(from)))
	{
		return;
	}

	if(md_start(&start) && md_stop(&stop, &start))
	{
		if(NULL != (dhl = makemon(&mons[PM_DHL_DELIVERY_MAN], start.x, start.y, NO_MM_FLAGS)))
		{
			md_rush(dhl, stop.x, stop.y);
		}
	}

	if(WRATH_OF_DEMOGORGON != obj->otyp)
	{
		if(NULL != (package = mksobj(DHL_PACKAGE, TRUE, FALSE)))
		{
			(void)add_to_container(package, obj);
			obj = package;
		}

		if(from[0])
		{
			oname(obj, from);
		}

		place_object(obj, u.ux, u.uy);

		if(NULL == dhl)
		{
			pline("An object mysteriously appears at your feet.");
		}
		else
		{
			if(distu(dhl->mx, dhl->my) > 2)
			{
				verbalize("Hello sir! I've got a package for you. Catch!");
			}
			else
			{
				verbalize("Hello sir, I'll just place this package by your feet while you sign here.");
			}
		}
	}
	else
	{
		if(NULL == dhl)
		{
			pline("The Wrath of Demogorgon suddenly appears in your hands!");
		}
		else
		{
			verbalize("Uhm, I'm terribly sorry sir, but I have to deliver this to you.");
		}

		unleash_the_wrath(obj);
		hold_another_object(obj, "You are struck by such force that you drop the object.", (const char *)0, (const char *)0);
	}

	display_nhwindow(WIN_MESSAGE, FALSE);

	if(dhl)
	{
		(void)md_rush(dhl, start.x, start.y);
		mongone(dhl);
	}
}



