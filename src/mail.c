/* NetHack 3.6	mail.c	$NHDT-Date: 1451955080 2016/01/05 00:51:20 $  $NHDT-Branch: NetHack-3.6.0 $:$NHDT-Revision: 1.25 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "sql.h"

#include "mail.h"

/*
 * Notify user when new mail has arrived.  Idea by Merlyn Leroy.
 *
 * The mail daemon can move with less than usual restraint.  It can:
 *      - move diagonally from a door
 *      - use secret and closed doors
 *      - run through a monster ("Gangway!", etc.)
 *      - run over pools & traps
 *
 * Possible extensions:
 *      - Open the file MAIL and do fstat instead of stat for efficiency.
 *        (But sh uses stat, so this cannot be too bad.)
 *      - Examine the mail and produce a scroll of mail named "From somebody".
 *      - Invoke MAILREADER in such a way that only this single mail is read.
 *      - Do something to the text when the scroll is enchanted or cancelled.
 *      - Make the daemon always appear at a stairwell, and have it find a
 *        path to the hero.
 *
 * Note by Olaf Seibert: On the Amiga, we usually don't get mail.  So we go
 *                       through most of the effects at 'random' moments.
 * Note by Paul Winner:  The MSDOS port also 'fakes' the mail daemon at
 *                       random intervals.
 */

STATIC_DCL void FDECL(newmail, (struct mail_info *));

extern char *viz_rmin, *viz_rmax; /* line-of-sight limits (vision.c) */

/*
 * Pick coordinates for a starting position for the mail daemon.  Called
 * from newmail() and newphone().
 */
boolean
md_start(startp)
coord *startp;
{
    coord testcc;     /* scratch coordinates */
    int row;          /* current row we are checking */
    int lax;          /* if TRUE, pick a position in sight. */
    int dd;           /* distance to current point */
    int max_distance; /* max distance found so far */

    /*
     * If blind and not telepathic, then it doesn't matter what we pick ---
     * the hero is not going to see it anyway.  So pick a nearby position.
     */
    if (Blind && !Blind_telepat) {
        if (!enexto(startp, u.ux, u.uy, (struct permonst *) 0))
            return FALSE; /* no good positions */
        return TRUE;
    }

    /*
     * Arrive at an up or down stairwell if it is in line of sight from the
     * hero.
     */
    if (couldsee(upstair.sx, upstair.sy)) {
        startp->x = upstair.sx;
        startp->y = upstair.sy;
        return TRUE;
    }
    if (couldsee(dnstair.sx, dnstair.sy)) {
        startp->x = dnstair.sx;
        startp->y = dnstair.sy;
        return TRUE;
    }

    /*
     * Try to pick a location out of sight next to the farthest position away
     * from the hero.  If this fails, try again, just picking the farthest
     * position that could be seen.  What we really ought to be doing is
     * finding a path from a stairwell...
     *
     * The arrays viz_rmin[] and viz_rmax[] are set even when blind.  These
     * are the LOS limits for each row.
     */
    lax = 0; /* be picky */
    max_distance = -1;
retry:
    for (row = 0; row < ROWNO; row++) {
        if (viz_rmin[row] < viz_rmax[row]) {
            /* There are valid positions on this row. */
            dd = distu(viz_rmin[row], row);
            if (dd > max_distance) {
                if (lax) {
                    max_distance = dd;
                    startp->y = row;
                    startp->x = viz_rmin[row];

                } else if (enexto(&testcc, (xchar) viz_rmin[row], row,
                                  (struct permonst *) 0)
                           && !cansee(testcc.x, testcc.y)
                           && couldsee(testcc.x, testcc.y)) {
                    max_distance = dd;
                    *startp = testcc;
                }
            }
            dd = distu(viz_rmax[row], row);
            if (dd > max_distance) {
                if (lax) {
                    max_distance = dd;
                    startp->y = row;
                    startp->x = viz_rmax[row];

                } else if (enexto(&testcc, (xchar) viz_rmax[row], row,
                                  (struct permonst *) 0)
                           && !cansee(testcc.x, testcc.y)
                           && couldsee(testcc.x, testcc.y)) {
                    max_distance = dd;
                    *startp = testcc;
                }
            }
        }
    }

    if (max_distance < 0) {
        if (!lax) {
            lax = 1; /* just find a position */
            goto retry;
        }
        return FALSE;
    }

    return TRUE;
}

/*
 * Try to choose a stopping point as near as possible to the starting
 * position while still adjacent to the hero.  If all else fails, try
 * enexto().  Use enexto() as a last resort because enexto() chooses
 * its point randomly, which is not what we want.
 */
boolean
md_stop(stopp, startp)
coord *stopp;  /* stopping position (we fill it in) */
coord *startp; /* starting position (read only) */
{
    int x, y, distance, min_distance = -1;

    for (x = u.ux - 1; x <= u.ux + 1; x++)
        for (y = u.uy - 1; y <= u.uy + 1; y++) {
            if (!isok(x, y) || (x == u.ux && y == u.uy))
                continue;

            if (accessible(x, y) && !MON_AT(x, y)) {
                distance = dist2(x, y, startp->x, startp->y);
                if (min_distance < 0 || distance < min_distance
                    || (distance == min_distance && rn2(2))) {
                    stopp->x = x;
                    stopp->y = y;
                    min_distance = distance;
                }
            }
        }

    /* If we didn't find a good spot, try enexto(). */
    if (min_distance < 0 && !enexto(stopp, u.ux, u.uy, &mons[PM_MAIL_DAEMON]))
        return FALSE;

    return TRUE;
}

/* Let the mail daemon have a larger vocabulary. */
static NEARDATA const char *mail_text[] = { "Gangway!", "Look out!",
                                            "Pardon me!" };
#define md_exclamations() (mail_text[rn2(3)])

/*
 * Make the mail daemon run through the dungeon.  The daemon will run over
 * any monsters that are in its path, but will replace them later.  Return
 * FALSE if the md gets stuck in a position where there is a monster.  Return
 * TRUE otherwise.
 */
boolean
md_rush(md, tx, ty)
struct monst *md;
register int tx, ty; /* destination of mail daemon */
{
    struct monst *mon;            /* displaced monster */
    register int dx, dy;          /* direction counters */
    int fx = md->mx, fy = md->my; /* current location */
    int nfx = fx, nfy = fy,       /* new location */
        d1, d2;                   /* shortest distances */

    /*
     * It is possible that the monster at (fx,fy) is not the md when:
     * the md rushed the hero and failed, and is now starting back.
     */
    if (m_at(fx, fy) == md) {
        remove_monster(fx, fy); /* pick up from orig position */
        newsym(fx, fy);
    }

    /*
     * At the beginning and exit of this loop, md is not placed in the
     * dungeon.
     */
    while (1) {
        /* Find a good location next to (fx,fy) closest to (tx,ty). */
        d1 = dist2(fx, fy, tx, ty);
        for (dx = -1; dx <= 1; dx++)
            for (dy = -1; dy <= 1; dy++)
                if ((dx || dy) && isok(fx + dx, fy + dy)
                    && !IS_STWALL(levl[fx + dx][fy + dy].typ)) {
                    d2 = dist2(fx + dx, fy + dy, tx, ty);
                    if (d2 < d1) {
                        d1 = d2;
                        nfx = fx + dx;
                        nfy = fy + dy;
                    }
                }

        /* Break if the md couldn't find a new position. */
        if (nfx == fx && nfy == fy)
            break;

        fx = nfx; /* this is our new position */
        fy = nfy;

        /* Break if the md reaches its destination. */
        if (fx == tx && fy == ty)
            break;

        if ((mon = m_at(fx, fy)) != 0) /* save monster at this position */
            verbalize1(md_exclamations());
        else if (fx == u.ux && fy == u.uy)
            verbalize("Excuse me.");

        place_monster(md, fx, fy); /* put md down */
        newsym(fx, fy);            /* see it */
        flush_screen(0);           /* make sure md shows up */
        delay_output();            /* wait a little bit */

        /* Remove md from the dungeon.  Restore original mon, if necessary. */
        if (mon) {
            if ((mon->mx != fx) || (mon->my != fy))
                place_worm_seg(mon, fx, fy);
            else
                place_monster(mon, fx, fy);
        } else
            remove_monster(fx, fy);
        newsym(fx, fy);
    }

    /*
     * Check for a monster at our stopping position (this is possible, but
     * very unlikely).  If one exists, then have the md leave in disgust.
     */
    if ((mon = m_at(fx, fy)) != 0) {
        place_monster(md, fx, fy); /* display md with text below */
        newsym(fx, fy);
        verbalize("This place's too crowded.  I'm outta here.");

        if ((mon->mx != fx) || (mon->my != fy)) /* put mon back */
            place_worm_seg(mon, fx, fy);
        else
            place_monster(mon, fx, fy);

        newsym(fx, fy);
        return FALSE;
    }

    place_monster(md, fx, fy); /* place at final spot */
    newsym(fx, fy);
    flush_screen(0);
    delay_output(); /* wait a little bit */

    return TRUE;
}

/* Deliver a scroll of mail. */
/*ARGSUSED*/
STATIC_OVL void
newmail(info)
struct mail_info *info;
{
    struct monst *md;
    coord start, stop;
    boolean message_seen = FALSE;

    /* Try to find good starting and stopping places. */
    if (!md_start(&start) || !md_stop(&stop, &start))
        goto give_up;

    /* Make the daemon.  Have it rush towards the hero. */
    if (!(md = makemon(&mons[PM_MAIL_DAEMON], start.x, start.y, NO_MM_FLAGS)))
        goto give_up;
    if (!md_rush(md, stop.x, stop.y))
        goto go_back;

    message_seen = TRUE;
    verbalize("%s, %s!  %s.", Hello(md), plname, info->display_txt);

    if (info->message_typ) {
        struct obj *obj = mksobj(SCR_MAIL, FALSE, FALSE);

        if (info->object_nam)
            obj = oname(obj, info->object_nam);
        if (info->response_cmd)
            new_omailcmd(obj, info->response_cmd);

        if (distu(md->mx, md->my) > 2)
            verbalize("Catch!");
        display_nhwindow(WIN_MESSAGE, FALSE);
        obj = hold_another_object(obj, "Oops!", (const char *) 0,
                                  (const char *) 0);
    }

/* zip back to starting location */
go_back:
    (void) md_rush(md, start.x, start.y);
    mongone(md);
/* deliver some classes of messages even if no daemon ever shows up */
give_up:
    if (!message_seen && info->message_typ == MSG_OTHER)
        pline("Hark!  \"%s.\"", info->display_txt);
}

static unsigned long last_check = 0;

void
ckmailstatus()
{
    if(last_check < get_ms())
    {
        if(0 == sql_next_mail(NULL, NULL, 0))
        {
            static struct mail_info deliver = {
                MSG_MAIL, "I have some mail for you",
                0, 0
            };
            newmail(&deliver);
        }

        last_check = get_ms() + 3000;
    }
}

void
readmail(otmp)
struct obj *otmp UNUSED;
{
    int first = 1;
    char from[SQL_NAME_LENGTH];
    char message[128];

    while(0 == sql_next_mail(from, message, 1))
    {
        pline(first
            ? "There is a message on this scroll."
            : "There is another message on this scroll.");
        pline("This message is from '%s'.", from);
        pline("It reads: \"%s\".", message);
        first = 0;
    }
}

