#ifndef _H_TFSM
#define _H_TFSM

#include <sys/queue.h>

#include "mpc.h"

#ifndef TFSM_GRAMMAR_SOURCE
#define TFSM_GRAMMAR_SOURCE ("./grammar.mpc")
#endif

// use context for further extension
typedef struct tfsm_context_t {
    void *data;
} tfsm_context_t;

typedef enum {
    TFSM_STATE_T_FINI = 1 << 0,
    TFSM_STATE_T_INIT = 1 << 1,
    TFSM_STATE_T_NODE = 1 << 2
} tfsm_state_type_t;

typedef struct tfsm_transition_t {
    TAILQ_ENTRY(tfsm_transition_t) trs;
    TAILQ_ENTRY(tfsm_transition_t) tbl_trs; /* head  */
    char *from_state;
    char *ret_val;
    char *to_state;
} tfsm_transition_t;

typedef struct tfsm_fsm_t {
    TAILQ_HEAD(,tfsm_state_t) states;
    TAILQ_HEAD(,tfsm_transition_t) trs_table;
    const char *source_name;
    char *init_state;
    int states_num;
    int pending_fn_num;
} tfsm_fsm_t;

typedef struct tfsm_state_t {
    TAILQ_HEAD(,tfsm_transition_t) transitions;
    TAILQ_ENTRY(tfsm_state_t) state_list;
    tfsm_state_type_t type;
    void *fn;
    char *name;
    char *source_file;
    char *function_name;
    int transition_num;
} tfsm_state_t;

typedef char *(*tfsm_state_fn)(tfsm_fsm_t *tfsm, tfsm_context_t *ctx);

/* ******************************************************************************* *
 *                                  fsm interface                                  *
 * ******************************************************************************* */

tfsm_fsm_t *tfsm_fsm_new(void);
tfsm_fsm_t *tfsm_fsm_create_from_file(const char *filename);
void tfsm_fsm_cleanup(tfsm_fsm_t *tfsm);
void tfsm_fsm_add_state(tfsm_fsm_t *tfsm, tfsm_state_t *state);
void tfsm_fsm_print(tfsm_fsm_t *tfsm);
void tfsm_fsm_inject_fn(tfsm_fsm_t *tfsm, tfsm_state_fn fn, const char *fnname);
tfsm_fsm_t *tfsm_fsm_fast_table(tfsm_fsm_t *tfsm);
tfsm_state_t *tfsm_state_find(tfsm_fsm_t *tfsm, const char *name, tfsm_state_type_t flag);

/* ******************************************************************************* *
 *                                  fstate functions                               *
 * ******************************************************************************* */

tfsm_state_t *tfsm_state_new(void);
void tfsm_state_delete(tfsm_state_t *state);
void tfsm_state_set_type(tfsm_state_t *state, const char *type);
void tfsm_state_set_name(tfsm_state_t *state, const char *name);
void tfsm_state_set_source(tfsm_state_t *tfsm, const char *s, const char *f);
void tfsm_state_add_transition(tfsm_state_t *tfsm, const char *ret_val, const char *next_state);
void tfsm_state_print(tfsm_state_t *state);

/* ******************************************************************************* *
 *                              transition functions                               *
 * ******************************************************************************* */

tfsm_transition_t *tfsm_trs_new(void);


char *tfsm_push(tfsm_fsm_t *tfsm, tfsm_context_t *ctx);
#endif
