#ifndef _CCT_SYS_QUEUE_H_
#define _CCT_SYS_QUEUE_H_
#include <sys/types.h>
struct _cct_slist { struct _cct_slist *next; };
#define SLIST_HEAD(n, t) struct n { struct t *slh_first; }
#define SLIST_ENTRY(t) struct { struct t *sle_next; }
#define SLIST_INIT(h) do { (h)->slh_first = 0; } while (0)
#define SLIST_FIRST(h) ((h)->slh_first)
#define SLIST_EMPTY(h) ((h)->slh_first == 0)
#define SLIST_NEXT(e, f) ((e)->f.sle_next)
#define SLIST_FOREACH(v, h, f) for ((v) = (h)->slh_first; (v); (v) = (v)->f.sle_next)
#define SLIST_INSERT_HEAD(h, e, f) do { (e)->f.sle_next = (h)->slh_first; (h)->slh_first = (e); } while (0)
#define SLIST_REMOVE_HEAD(h, f) do { (h)->slh_first = (h)->slh_first->f.sle_next; } while (0)
#define STAILQ_HEAD(n, t) struct n { struct t *stqh_first; struct t **stqh_last; }
#define STAILQ_ENTRY(t) struct { struct t *stqe_next; }
#define STAILQ_INIT(h) do { (h)->stqh_first = 0; (h)->stqh_last = &(h)->stqh_first; } while (0)
#define STAILQ_FIRST(h) ((h)->stqh_first)
#define LIST_HEAD(n, t) struct n { struct t *lh_first; }
#define LIST_ENTRY(t) struct { struct t *le_next; struct t **le_prev; }
#define LIST_INIT(h) do { (h)->lh_first = 0; } while (0)
#define LIST_FIRST(h) ((h)->lh_first)
#define TAILQ_HEAD(n, t) struct n { struct t *tqh_first; struct t **tqh_last; }
#define TAILQ_ENTRY(t) struct { struct t *tqe_next; struct t **tqe_prev; }
#define TAILQ_INIT(h) do { (h)->tqh_first = 0; (h)->tqh_last = &(h)->tqh_first; } while (0)
#define TAILQ_FIRST(h) ((h)->tqh_first)
#endif
