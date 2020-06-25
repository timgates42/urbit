/* vere/lord.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <gmp.h>
#include <sigsegv.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <uv.h>
#include <errno.h>

#include "all.h"
#include "vere/vere.h"

/*
|%
::  +writ: from king to serf
::
+$  writ
  $%  $:  %live
          $%  [%exit cod=@]
              [%save eve=@]
              [%pack eve=@]
      ==  ==
      [%peek now=date lyc=gang pat=path]
      [%play eve=@ lit=(list ?((pair date ovum) *))]
      [%work mil=@ job=(pair date ovum)]
  ==
::  +plea: from serf to king
::
+$  plea
  $%  [%live ~]
      [%ripe [pro=@ hon=@ nok=@] eve=@ mug=@]
      [%slog pri=@ ?(cord tank)]
      [%peek dat=(unit (cask))]
      $:  %play
          $%  [%done mug=@]
              [%bail eve=@ mug=@ dud=goof]
      ==  ==
      $:  %work
          $%  [%done eve=@ mug=@ fec=(list ovum)]
              [%swap eve=@ mug=@ job=(pair date ovum) fec=(list ovum)]
              [%bail lud=(list goof)]
      ==  ==
  ==
--
*/

/* _lord_stop_cb(): finally all done.
*/
static void
_lord_stop_cb(void*       ptr_v,
              const c3_c* err_c)
{
  u3_lord* god_u = ptr_v;

  void (*exit_f)(void*) = god_u->cb_u.exit_f;
  void* exit_v = god_u->cb_u.vod_p;

  c3_free(god_u);

  if ( exit_f ) {
    exit_f(exit_v);
  }
}

/* _lord_writ_free(): dispose of pending writ.
*/
static void
_lord_writ_free(u3_writ* wit_u)
{
  switch ( wit_u->typ_e ) {
    default: c3_assert(0);

    case u3_writ_work: {
      //  XX confirm
      //
      u3_ovum* egg_u = wit_u->wok_u.egg_u;
      u3_auto_drop(egg_u->car_u, egg_u);
      u3z(wit_u->wok_u.job);
    } break;

    case u3_writ_peek: {
      u3z(wit_u->pek_u->now);
      u3z(wit_u->pek_u->gan);
      u3z(wit_u->pek_u->ful);
    } break;

    case u3_writ_play: {
      u3_fact* tac_u = wit_u->fon_u.ext_u;
      u3_fact* nex_u;

      while ( tac_u ) {
        nex_u = tac_u->nex_u;
        u3_fact_free(tac_u);
        tac_u = nex_u;
      }
    } break;

    case u3_writ_save:
    case u3_writ_pack:
    case u3_writ_exit: {
    } break;
  }

  c3_free(wit_u);
}

/* _lord_bail_noop(): ignore subprocess error on shutdown
*/
static void
_lord_bail_noop(void*       ptr_v,
                const c3_c* err_c)
{
}

/* _lord_stop(): close and dispose all resources.
*/
static void
_lord_stop(u3_lord* god_u)
{
  //  dispose outstanding writs
  //
  {
    u3_writ* wit_u = god_u->ext_u;
    u3_writ* nex_u;

    while ( wit_u ) {
      nex_u = wit_u->nex_u;
      _lord_writ_free(wit_u);
      wit_u = nex_u;
    }

    god_u->ent_u = god_u->ext_u = 0;
  }

  u3_newt_moat_stop(&god_u->out_u, _lord_stop_cb);
  u3_newt_mojo_stop(&god_u->inn_u, _lord_bail_noop);

  uv_close((uv_handle_t*)&god_u->cub_u, 0);
}

/* _lord_bail(): serf/lord error.
*/
static void
_lord_bail(u3_lord* god_u)
{
  void (*bail_f)(void*) = god_u->cb_u.bail_f;
  void* bail_v = god_u->cb_u.vod_p;

  u3_lord_halt(god_u);
  bail_f(bail_v);
}

/* _lord_writ_pop(): pop the writ stack.
*/
static u3_writ*
_lord_writ_pop(u3_lord* god_u)
{
  u3_writ* wit_u = god_u->ext_u;

  c3_assert( wit_u );

  if ( !wit_u->nex_u ) {
    god_u->ent_u = god_u->ext_u = 0;
  }
  else {
    god_u->ext_u = wit_u->nex_u;
    wit_u->nex_u = 0;
  }

  god_u->dep_w--;

  return wit_u;
}

/* _lord_writ_str(): writ labels for printing.
*/
static inline const c3_c*
_lord_writ_str(u3_writ_type typ_e)
{
  switch ( typ_e ) {
    default: c3_assert(0);

    case u3_writ_work: return "work";
    case u3_writ_peek: return "peek";
    case u3_writ_play: return "play";
    case u3_writ_save: return "save";
    case u3_writ_pack: return "pack";
    case u3_writ_exit: return "exit";
  }
}

/* _lord_writ_need(): require writ type.
*/
static u3_writ*
_lord_writ_need(u3_lord* god_u, u3_writ_type typ_e)
{
  u3_writ* wit_u = _lord_writ_pop(god_u);

  if ( typ_e != wit_u->typ_e ) {
    fprintf(stderr, "lord: unexpected %%%s, expected %%%s\r\n",
                    _lord_writ_str(typ_e),
                    _lord_writ_str(wit_u->typ_e));
    _lord_bail(god_u);
    return 0;
  }

  return wit_u;
}

/* _lord_plea_foul():
*/
static void
_lord_plea_foul(u3_lord* god_u, c3_m mot_m, u3_noun dat)
{
  if ( u3_blip == mot_m ) {
    fprintf(stderr, "lord: received invalid $plea\r\n");
  }
  else {
    fprintf(stderr, "lord: received invalid %%%.4s $plea\r\n", (c3_c*)&mot_m);
  }

  //  XX can't unconditionally print
  //
  // u3m_p("plea", dat);

  _lord_bail(god_u);
}

/* _lord_plea_live(): hear serf %live ack
*/
static void
_lord_plea_live(u3_lord* god_u, u3_noun dat)
{
  u3_writ* wit_u = _lord_writ_pop(god_u);

  if( u3_nul != dat ) {
    return _lord_plea_foul(god_u, c3__live, dat);
  }

  switch ( wit_u->typ_e ) {
    default: {
      return _lord_plea_foul(god_u, c3__live, dat);
    } break;

    case u3_writ_save: {
      god_u->cb_u.save_f(god_u->cb_u.vod_p);
    } break;

    case u3_writ_pack: {
      god_u->cb_u.pack_f(god_u->cb_u.vod_p);
    } break;
  }

  c3_free(wit_u);
}

/* _lord_plea_ripe(): hear serf startup state
*/
static void
_lord_plea_ripe(u3_lord* god_u, u3_noun dat)
{
  if ( c3y == god_u->liv_o ) {
    fprintf(stderr, "lord: received unexpected %%ripe\n");
    _lord_bail(god_u);
    return;
  }

  {
    u3_noun ver, pro, hon, noc, eve, mug;
    c3_y pro_y, hon_y, noc_y;
    c3_d eve_d;
    c3_l mug_l;

    if (  (c3n == u3r_trel(dat, &ver, &eve, &mug))
       || (c3n == u3r_trel(ver, &pro, &hon, &noc))
       || (c3n == u3r_safe_byte(pro, &pro_y))
       || (c3n == u3r_safe_byte(hon, &hon_y))
       || (c3n == u3r_safe_byte(noc, &noc_y))
       || (c3n == u3r_safe_chub(eve, &eve_d))
       || (c3n == u3r_safe_word(mug, &mug_l)) )
    {
      return _lord_plea_foul(god_u, c3__ripe, dat);
    }

    if ( 1 != pro_y ) {
      fprintf(stderr, "pier: unsupported ipc protocol version %u\r\n", pro_y);
      _lord_bail(god_u);
      return;
    }

    god_u->eve_d = eve_d;
    god_u->mug_l = mug_l;
    god_u->hon_y = hon_y;
    god_u->noc_y = noc_y;
  }

  god_u->liv_o = c3y;
  god_u->cb_u.live_f(god_u->cb_u.vod_p);

  u3z(dat);
}

/* _lord_plea_slog(): hear serf debug output
*/
static void
_lord_plea_slog(u3_lord* god_u, u3_noun dat)
{
  u3_noun pri, tan;
  c3_w pri_w;

  if (  (c3n == u3r_cell(dat, &pri, &tan))
     || (c3n == u3r_safe_word(pri, &pri_w)) )
  {
    return _lord_plea_foul(god_u, c3__slog, dat);
  }

  //  XX per-writ slog_f?
  //

  god_u->cb_u.slog_f(god_u->cb_u.vod_p, pri_w, u3k(tan));
  u3z(dat);
}

/* _lord_plea_peek(): hear serf %peek response
*/
static void
_lord_plea_peek(u3_lord* god_u, u3_noun dat)
{
  u3_peek* pek_u;
  {
    u3_writ* wit_u = _lord_writ_need(god_u, u3_writ_peek);
    pek_u = wit_u->pek_u;
    c3_free(wit_u);
  }

  //  XX cache [dat] (unless last)
  //
  pek_u->fun_f(pek_u->vod_p, dat);

  u3z(pek_u->now);
  u3z(pek_u->gan);
  u3z(pek_u->ful);
  c3_free(pek_u);
  // god_u->cb_u.peek_f(god_u->cb_u.vod_p, pek_u, dat);
}

/* _lord_plea_play_bail(): hear serf %play %bail
*/
static void
_lord_plea_play_bail(u3_lord* god_u, u3_info fon_u, u3_noun dat)
{
  u3_noun eve, mug, dud;
  c3_d eve_d;
  c3_l mug_l;

  if (  (c3n == u3r_trel(dat, &eve, &mug, &dud))
     || (c3n == u3r_safe_chub(eve, &eve_d))
     || (c3n == u3r_safe_word(mug, &mug_l))
     || (c3n == u3a_is_cell(dud)) )
  {
    fprintf(stderr, "lord: invalid %%play\r\n");
    return _lord_plea_foul(god_u, c3__bail, dat);
  }

  god_u->eve_d = (eve_d - 1ULL);
  god_u->mug_l = mug_l;

  god_u->cb_u.play_bail_f(god_u->cb_u.vod_p,
                          fon_u, mug_l, eve_d, u3k(dud));

  u3z(dat);
}
/* _lord_plea_play_done(): hear serf %play %done
*/
static void
_lord_plea_play_done(u3_lord* god_u, u3_info fon_u, u3_noun dat)
{
  c3_l mug_l;

  if ( c3n == u3r_safe_word(dat, &mug_l) ) {
    fprintf(stderr, "lord: invalid %%play\r\n");
    return _lord_plea_foul(god_u, c3__done, dat);
  }

  god_u->eve_d = fon_u.ent_u->eve_d;
  god_u->mug_l = mug_l;

  god_u->cb_u.play_done_f(god_u->cb_u.vod_p, fon_u, mug_l);

  u3z(dat);
}

/* _lord_plea_play(): hear serf %play response
*/
static void
_lord_plea_play(u3_lord* god_u, u3_noun dat)
{
  u3_info fon_u;
  {
    u3_writ* wit_u = _lord_writ_need(god_u, u3_writ_play);
    fon_u = wit_u->fon_u;
    c3_free(wit_u);
  }

  if ( c3n == u3a_is_cell(dat) ) {
    return _lord_plea_foul(god_u, c3__play, dat);
  }

  switch ( u3h(dat) ) {
    default: {
      return _lord_plea_foul(god_u, c3__play, dat);
    }

    case c3__done: {
      _lord_plea_play_done(god_u, fon_u, u3k(u3t(dat)));
    } break;

    case c3__bail: {
      _lord_plea_play_bail(god_u, fon_u, u3k(u3t(dat)));
    } break;
  }

  u3z(dat);
}

/* _lord_work_spin(): update spinner if more work is in progress.
 */
 static void
_lord_work_spin(u3_lord* god_u)
{
  u3_writ* wit_u = god_u->ext_u;

  //  complete spinner
  //
  c3_assert( c3y == god_u->pin_o );
  god_u->cb_u.spun_f(god_u->cb_u.vod_p);
  god_u->pin_o = c3n;

  //  restart spinner if more work
  //
  while ( wit_u ) {
    if ( u3_writ_work != wit_u->typ_e ) {
      wit_u = wit_u->nex_u;
    }
    else {
      u3_ovum* egg_u = wit_u->wok_u.egg_u;

      god_u->cb_u.spin_f(god_u->cb_u.vod_p,
                         egg_u->pin_u.lab,
                         egg_u->pin_u.del_o);
      god_u->pin_o = c3y;
      break;
    }
  }
}

/* _lord_work_done():
*/
static void
_lord_work_done(u3_lord* god_u,
                u3_ovum* egg_u,
                c3_d     eve_d,
                c3_l     mug_l,
                u3_noun    job,
                u3_noun    act)
{
  u3_fact* tac_u = u3_fact_init(eve_d, mug_l, job);
  tac_u->bug_l   = god_u->mug_l; // XX

  god_u->mug_l = mug_l;
  god_u->eve_d = eve_d;

  u3_gift* gif_u = u3_gift_init(eve_d, act);

  _lord_work_spin(god_u);

  god_u->cb_u.work_done_f(god_u->cb_u.vod_p, egg_u, tac_u, gif_u);
}


/* _lord_plea_work_bail(): hear serf %work %bail
*/
static void
_lord_plea_work_bail(u3_lord* god_u, u3_ovum* egg_u, u3_noun lud)
{
  _lord_work_spin(god_u);

  god_u->cb_u.work_bail_f(god_u->cb_u.vod_p, egg_u, lud);
}

/* _lord_plea_work_swap(): hear serf %work %swap
*/
static void
_lord_plea_work_swap(u3_lord* god_u, u3_ovum* egg_u, u3_noun dat)
{
  u3_noun eve, mug, job, act;
  c3_d eve_d;
  c3_l mug_l;

  if (  (c3n == u3r_qual(dat, &eve, &mug, &job, &act))
     || (c3n == u3r_safe_chub(eve, &eve_d))
     || (c3n == u3r_safe_word(mug, &mug_l))
     || (c3n == u3a_is_cell(job)) )
  {
    u3z(job);
    u3_ovum_free(egg_u);
    fprintf(stderr, "lord: invalid %%work\r\n");
    return _lord_plea_foul(god_u, c3__swap, dat);
  }
  else {
    u3k(job); u3k(act);
    u3z(dat);
    _lord_work_done(god_u, egg_u, eve_d, mug_l, job, act);
  }
}

/* _lord_plea_work_done(): hear serf %work %done
*/
static void
_lord_plea_work_done(u3_lord* god_u,
                     u3_ovum* egg_u,
                     u3_noun    job,
                     u3_noun    dat)
{
  u3_noun eve, mug, act;
  c3_d eve_d;
  c3_l mug_l;

  if (  (c3n == u3r_trel(dat, &eve, &mug, &act))
     || (c3n == u3r_safe_chub(eve, &eve_d))
     || (c3n == u3r_safe_word(mug, &mug_l)) )
  {
    u3z(job);
    u3_ovum_free(egg_u);
    fprintf(stderr, "lord: invalid %%work\r\n");
    return _lord_plea_foul(god_u, c3__done, dat);
  }
  else {
    u3k(act);
    u3z(dat);
    _lord_work_done(god_u, egg_u, eve_d, mug_l, job, act);
  }
}

/* _lord_plea_work(): hear serf %work response
*/
static void
_lord_plea_work(u3_lord* god_u, u3_noun dat)
{
  u3_ovum* egg_u;
  u3_noun    job;

  {
    u3_writ*  wit_u = _lord_writ_need(god_u, u3_writ_work);
    egg_u = wit_u->wok_u.egg_u;
    job   = wit_u->wok_u.job;
    c3_free(wit_u);
  }

  if ( c3n == u3a_is_cell(dat) ) {
    u3z(job);
    u3_ovum_free(egg_u);
    return _lord_plea_foul(god_u, c3__work, dat);
  }

  switch ( u3h(dat) ) {
    default: {
      u3z(job);
      u3_ovum_free(egg_u);
      return _lord_plea_foul(god_u, c3__work, dat);
    } break;

    case c3__done: {
      _lord_plea_work_done(god_u, egg_u, job, u3k(u3t(dat)));
    } break;

    case c3__swap: {
      u3z(job);
      _lord_plea_work_swap(god_u, egg_u, u3k(u3t(dat)));
    } break;

    case c3__bail: {
      u3z(job);
      _lord_plea_work_bail(god_u, egg_u, u3k(u3t(dat)));
    } break;
  }

  u3z(dat);
}

/* _lord_on_plea(): handle plea from serf.
*/
static void
_lord_on_plea(void* ptr_v, u3_noun mat)
{
  u3_lord* god_u = ptr_v;
  u3_noun    jar = u3ke_cue(mat);
  u3_noun    tag, dat;

  if ( c3n == u3r_cell(jar, &tag, &dat) ) {
    u3m_p("jar", jar);
    return _lord_plea_foul(god_u, 0, jar);
  }

  switch ( tag ) {
    default: {
      return _lord_plea_foul(god_u, 0, jar);
    }

    case c3__work: {
      _lord_plea_work(god_u, u3k(dat));
    } break;

    case c3__peek: {
      _lord_plea_peek(god_u, u3k(dat));
    } break;

    case  c3__slog: {
      _lord_plea_slog(god_u, u3k(dat));
    } break;

    case c3__play: {
      _lord_plea_play(god_u, u3k(dat));
    } break;

    case c3__live: {
      _lord_plea_live(god_u, u3k(dat));
    } break;

    case c3__ripe: {
      _lord_plea_ripe(god_u, u3k(dat));
    } break;
  }

  u3z(jar);
}

/* _lord_writ_new(): allocate a new writ.
*/
static u3_writ*
_lord_writ_new(u3_lord* god_u)
{
  u3_writ* wit_u = c3_calloc(sizeof(*wit_u));
  gettimeofday(&wit_u->tim_u, 0);
  return wit_u;
}

/* _lord_writ_jam(): serialize writ.
*/
static void
_lord_writ_jam(u3_lord* god_u, u3_writ* wit_u)
{
  if ( 0 == wit_u->mat ) {
    u3_noun msg;

    switch ( wit_u->typ_e ) {
      default: c3_assert(0);

      case u3_writ_work: {
        u3_noun mil = u3i_words(1, &wit_u->wok_u.egg_u->mil_w);
        msg = u3nt(c3__work, mil, u3k(wit_u->wok_u.job));
      } break;

      case u3_writ_peek: {
        msg = u3nq(c3__peek, u3k(wit_u->pek_u->now),
                             u3k(wit_u->pek_u->gan),
                             u3k(wit_u->pek_u->ful));
      } break;

      case u3_writ_play: {
        u3_fact* tac_u = wit_u->fon_u.ext_u;
        c3_d     eve_d = tac_u->eve_d;
        u3_noun    lit = u3_nul;

        while ( tac_u ) {
          lit   = u3nc(u3k(tac_u->job), lit);
          tac_u = tac_u->nex_u;
        }

        msg = u3nt(c3__play, u3i_chubs(1, &eve_d), u3kb_flop(lit));

      } break;

      case u3_writ_save: {
        msg = u3nt(c3__live, c3__save, u3i_chubs(1, &god_u->eve_d));
      } break;

      case u3_writ_pack: {
        msg = u3nt(c3__live, c3__pack, u3i_chubs(1, &god_u->eve_d));
      } break;

      case u3_writ_exit: {
        //  requested exit code is always 0
        //
        msg = u3nt(c3__live, c3__exit, 0);
      } break;
    }

    wit_u->mat = u3ke_jam(msg);
  }
}

/* _lord_writ_send(): send writ to serf.
*/
static void
_lord_writ_send(u3_lord* god_u, u3_writ* wit_u)
{
  //  exit expected
  //
  if ( u3_writ_exit == wit_u->typ_e ) {
    god_u->out_u.bal_f = _lord_bail_noop;
    god_u->inn_u.bal_f = _lord_bail_noop;
  }

  _lord_writ_jam(god_u, wit_u);
  u3_newt_write(&god_u->inn_u, wit_u->mat);
  wit_u->mat = 0;
}

/* _lord_writ_plan(): enqueue a writ and send.
*/
static void
_lord_writ_plan(u3_lord* god_u, u3_writ* wit_u)
{
  if ( !god_u->ent_u ) {
    c3_assert( !god_u->ext_u );
    c3_assert( !god_u->dep_w );
    god_u->dep_w = 1;
    god_u->ent_u = god_u->ext_u = wit_u;
  }
  else {
    god_u->dep_w++;
    god_u->ent_u->nex_u = wit_u;
    god_u->ent_u = wit_u;
  }

  _lord_writ_send(god_u, wit_u);
}

/* u3_lord_peek(): read namespace.
*/
void
u3_lord_peek(u3_lord*   god_u,
             u3_noun      gan,
             u3_noun      ful,
             void*      vod_p,
             u3_peek_cb fun_f)
{
  u3_writ* wit_u = _lord_writ_new(god_u);
  wit_u->typ_e = u3_writ_peek;
  wit_u->pek_u = c3_calloc(sizeof(*wit_u->pek_u));
  wit_u->pek_u->vod_p = vod_p;
  wit_u->pek_u->fun_f = fun_f;
  wit_u->pek_u->now   = u3_time_in_tv(&wit_u->tim_u);
  wit_u->pek_u->gan   = gan;
  wit_u->pek_u->ful   = ful;

  //  XX cache check
  //

  _lord_writ_plan(god_u, wit_u);
}

/* u3_lord_peek_mine(): read namespace, injecting ship (our).
*/
void
u3_lord_peek_mine(u3_lord*   god_u,
                  u3_noun      gan,
                  c3_m       car_m,
                  u3_noun      pax,
                  void*      vod_p,
                  u3_peek_cb fun_f)
{
  u3_writ* wit_u = _lord_writ_new(god_u);
  wit_u->typ_e = u3_writ_peek;
  wit_u->pek_u = c3_calloc(sizeof(*wit_u->pek_u));
  wit_u->pek_u->vod_p = vod_p;
  wit_u->pek_u->fun_f = fun_f;
  wit_u->pek_u->now   = u3_time_in_tv(&wit_u->tim_u);
  wit_u->pek_u->gan   = gan;

  {
    //  XX cache
    //
    u3_pier* pir_u = god_u->cb_u.vod_p;  //  XX do better
    u3_noun our = u3dc("scot", 'p', u3i_chubs(2, pir_u->who_d));
    wit_u->pek_u->ful = u3nt(car_m, our, pax);
  }

  //  XX cache check
  //

  _lord_writ_plan(god_u, wit_u);
}

/* u3_lord_peek_last(): read namespace, injecting ship (our) and case (now).
*/
void
u3_lord_peek_last(u3_lord*   god_u,
                  u3_noun      gan,
                  c3_m       car_m,
                  u3_atom      des,
                  u3_noun      pax,
                  void*      vod_p,
                  u3_peek_cb fun_f)
{
  u3_writ* wit_u = _lord_writ_new(god_u);
  wit_u->typ_e = u3_writ_peek;
  wit_u->pek_u = c3_calloc(sizeof(*wit_u->pek_u));
  wit_u->pek_u->vod_p = vod_p;
  wit_u->pek_u->fun_f = fun_f;
  wit_u->pek_u->now   = u3_time_in_tv(&wit_u->tim_u);
  wit_u->pek_u->gan   = gan;

  {
    //  XX cache
    //
    u3_pier* pir_u = god_u->cb_u.vod_p;  //  XX do better
    u3_noun our = u3dc("scot", 'p', u3i_chubs(2, pir_u->who_d));
    u3_noun cas = u3dc("scot", c3__da, u3k(wit_u->pek_u->now));

    wit_u->pek_u->ful = u3nc(car_m, u3nq(our, des, cas, pax));
  }

  //  NB, won't be cached, result shouldn't be
  //
  _lord_writ_plan(god_u, wit_u);
}

/* u3_lord_play(): recompute batch.
*/
void
u3_lord_play(u3_lord* god_u, u3_info fon_u)
{
  u3_writ* wit_u = _lord_writ_new(god_u);
  wit_u->typ_e = u3_writ_play;
  wit_u->fon_u = fon_u;

  //  XX wat do?
  //
  // c3_assert( !pay_u.ent_u->nex_u );

  _lord_writ_plan(god_u, wit_u);
}

/* u3_lord_work(): attempt work.
*/
void
u3_lord_work(u3_lord* god_u, u3_ovum* egg_u, u3_noun ovo)
{
  u3_writ* wit_u = _lord_writ_new(god_u);
  wit_u->typ_e = u3_writ_work;
  wit_u->wok_u.egg_u = egg_u;

  {
    u3_noun now = u3_time_in_tv(&wit_u->tim_u);
    wit_u->wok_u.job = u3nc(now, ovo);
  }

  //  if not spinning, start
  //
  if ( c3n == god_u->pin_o ) {
    god_u->cb_u.spin_f(god_u->cb_u.vod_p,
                       egg_u->pin_u.lab,
                       egg_u->pin_u.del_o);
    god_u->pin_o = c3y;
  }

  _lord_writ_plan(god_u, wit_u);
}

/* u3_lord_save(): save a snapshot.
*/
c3_o
u3_lord_save(u3_lord* god_u)
{
  if ( god_u->dep_w ) {
    return c3n;
  }
  else {
    u3_writ* wit_u = _lord_writ_new(god_u);
    wit_u->typ_e = u3_writ_save;
    _lord_writ_plan(god_u, wit_u);
    return c3y;
  }
}

/* u3_lord_pack(): save portable state.
*/
c3_o
u3_lord_pack(u3_lord* god_u)
{
  if ( god_u->dep_w ) {
    return c3n;
  }
  else {
    u3_writ* wit_u = _lord_writ_new(god_u);
    wit_u->typ_e = u3_writ_pack;
    _lord_writ_plan(god_u, wit_u);
    return c3y;
  }
}

/* u3_lord_exit(): shutdown gracefully.
*/
void
u3_lord_exit(u3_lord* god_u)
{
  u3_writ* wit_u = _lord_writ_new(god_u);
  wit_u->typ_e = u3_writ_exit;
  _lord_writ_plan(god_u, wit_u);

  //  XX set timer, then halt
}

/* u3_lord_stall(): send SIGINT
*/
void
u3_lord_stall(u3_lord* god_u)
{
  uv_process_kill(&god_u->cub_u, SIGINT);
}

/* u3_lord_halt(): shutdown immediately
*/
void
u3_lord_halt(u3_lord* god_u)
{
  //  no exit callback on halt
  //
  god_u->cb_u.exit_f = 0;

  uv_process_kill(&god_u->cub_u, SIGKILL);
  _lord_stop(god_u);
}

/* _lord_on_serf_exit(): handle subprocess exit.
*/
static void
_lord_on_serf_exit(uv_process_t* req_u,
                   c3_ds         sas_i,
                   c3_i          sig_i)
{

  u3_lord* god_u = (void*)req_u;

  if (  !god_u->ext_u
     || !(u3_writ_exit == god_u->ext_u->typ_e) )
  {
    fprintf(stderr, "pier: work exit: status %" PRId64 ", signal %d\r\n",
                  sas_i, sig_i);
    _lord_bail(god_u);
  }
  else {
    _lord_stop(god_u);
  }
}

/* _lord_on_serf_bail(): handle subprocess error.
*/
static void
_lord_on_serf_bail(void*       ptr_v,
                   const c3_c* err_c)
{
  u3_lord* god_u = ptr_v;
  u3l_log("pier: serf error: %s\r\n", err_c);
  _lord_bail(god_u);
}

/* u3_lord_info(): print status info.
*/
void
u3_lord_info(u3_lord* god_u)
{
  u3l_log("  lord: live=%s, event=%" PRIu64 ", mug=%x, queue=%u\n",
          ( c3y == god_u->liv_o ) ? "&" : "|",
          god_u->eve_d,
          god_u->mug_l,
          god_u->dep_w);
}

/* u3_lord_init(): instantiate child process.
*/
u3_lord*
u3_lord_init(c3_c* pax_c, c3_w wag_w, c3_d key_d[4], u3_lord_cb cb_u)
{
  u3_lord* god_u = c3_calloc(sizeof *god_u);
  god_u->liv_o = c3n;
  god_u->wag_w = wag_w;
  god_u->bin_c = u3_Host.wrk_c; //  XX strcopy
  god_u->pax_c = pax_c;  //  XX strcopy
  god_u->cb_u  = cb_u;

  god_u->key_d[0] = key_d[0];
  god_u->key_d[1] = key_d[1];
  god_u->key_d[2] = key_d[2];
  god_u->key_d[3] = key_d[3];

  //  spawn new process and connect to it
  //
  {
    c3_c* arg_c[6];
    c3_c  key_c[256];
    c3_c  wag_c[11];
    c3_i  err_i;

    sprintf(key_c, "%" PRIx64 ":%" PRIx64 ":%" PRIx64 ":%" PRIx64 "",
                   god_u->key_d[0],
                   god_u->key_d[1],
                   god_u->key_d[2],
                   god_u->key_d[3]);

    sprintf(wag_c, "%u", god_u->wag_w);

    arg_c[0] = god_u->bin_c;            //  executable
    arg_c[1] = god_u->pax_c;            //  path to checkpoint directory
    arg_c[2] = key_c;                   //  disk key
    arg_c[3] = wag_c;                   //  runtime config

    if ( u3_Host.ops_u.roc_c ) {
      //  XX validate
      //
      arg_c[4] = u3_Host.ops_u.roc_c;
    }
    else {
      arg_c[4] = 0;
    }

    arg_c[5] = 0;

    uv_pipe_init(u3L, &god_u->inn_u.pyp_u, 0);
    uv_timer_init(u3L, &god_u->out_u.tim_u);
    uv_pipe_init(u3L, &god_u->out_u.pyp_u, 0);

    god_u->cod_u[0].flags = UV_CREATE_PIPE | UV_READABLE_PIPE;
    god_u->cod_u[0].data.stream = (uv_stream_t *)&god_u->inn_u;

    god_u->cod_u[1].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
    god_u->cod_u[1].data.stream = (uv_stream_t *)&god_u->out_u;

    god_u->cod_u[2].flags = UV_INHERIT_FD;
    god_u->cod_u[2].data.fd = 2;

    god_u->ops_u.stdio = god_u->cod_u;
    god_u->ops_u.stdio_count = 3;

    god_u->ops_u.exit_cb = _lord_on_serf_exit;
    god_u->ops_u.file = arg_c[0];
    god_u->ops_u.args = arg_c;

    if ( (err_i = uv_spawn(u3L, &god_u->cub_u, &god_u->ops_u)) ) {
      fprintf(stderr, "spawn: %s: %s\r\n", arg_c[0], uv_strerror(err_i));

      return 0;
    }
  }

  //  start reading from proc
  //
  {
    god_u->out_u.ptr_v = god_u;
    god_u->out_u.pok_f = _lord_on_plea;
    god_u->out_u.bal_f = _lord_on_serf_bail;

    //  XX distinguish from out_u.bal_f ?
    //
    god_u->inn_u.ptr_v = god_u;
    god_u->inn_u.bal_f = _lord_on_serf_bail;

    u3_newt_read(&god_u->out_u);
  }
  return god_u;
}