#ifndef _H_TFSM
#define _H_TFSM

#include <sys/queue.h>

#include "mpc.h"

#ifndef TFSM_GRAMMAR_SOURCE
#define TFSM_GRAMMAR_SOURCE ("./grammar.mpc")
#endif

#ifndef TFSM_MAX_STATES_NUM
#define TFSM_MAX_STATES_NUM (1024)
#endif

#ifndef TFSM_MAX_RETVAL_NUM
#define TFSM_MAX_RETVAL_NUM (1024)
#endif

enum tfsm_status {
    TFSM_OK,
    TFSM_PENDING,
};

enum tfsm_ret_type {
    INT,
    STR,
};

enum tfsm_state_type {
    TFSM_STATE_T_FINI = 1 << 0,
    TFSM_STATE_T_INIT = 1 << 1,
    TFSM_STATE_T_NODE = 1 << 2
};

enum tfsm_fsm_type {
    // no additional operations
    TFSM_FSM_T_NORMAL,
    // create state table, fast, memmory inefficient
    TFSM_FSM_T_TABLE,
    // create indexed state list, faster than not modified
    // version but slower than table version
    TFSM_FSM_T_LIST
};

typedef struct tfsm_retval {
    enum tfsm_ret_type ret_val_type;
    union {
        int i;
        char *s;
    } val;
} tfsm_retval;

// use context for further extension
typedef struct tfsm_ctx_param_t {
    TAILQ_ENTRY(tfsm_ctx_param_t) param_list;
    char *key;
    char *val;
} tfsm_ctx_param_t;


struct tfsm_state_t;
typedef struct tfsm_ctx_t {
    TAILQ_HEAD(,tfsm_ctx_param_t) params;
    tfsm_retval *runtime_val;
    void *data;
    struct tfsm_state_t *cur_state;
} tfsm_ctx_t;


typedef struct tfsm_transition_t {
    TAILQ_ENTRY(tfsm_transition_t) trs;
    TAILQ_ENTRY(tfsm_transition_t) tbl_trs; /* head  */
    char *from_state;
    char *ret_val;
    char *to_state;
} tfsm_transition_t;

typedef struct tfsm_state_t {
    TAILQ_HEAD(,tfsm_transition_t) transitions;
    TAILQ_ENTRY(tfsm_state_t) state_list;
    enum tfsm_state_type type;
    int idx;
    void *fn;
    char *name;
    char *source_file;
    char *function_name;
    int transition_num;
} tfsm_state_t;

typedef struct tfsm_fsm_t {
    // storing here all states on parsing
    TAILQ_HEAD(,tfsm_state_t) states;
    // not used
    TAILQ_HEAD(,tfsm_transition_t) trs_table;
    // all known uniq ret_vals of states
    char **known_ret_vals;
    // name of source file
    const char *source_name;
    // name of initial state
    tfsm_state_t *init_state;
    // total number of states
    int states_num;
    // total number of known ret)vals
    int ret_val_num;
    // number of functions to be injected
    int pending_fn_num;
    // LOOKUP fields
    // Table - used if TFSM_FSM_T_TABLE
    tfsm_state_t ***lookup_table;
    // next state LOOKUP function
    tfsm_state_t *(*next_state)(struct tfsm_fsm_t *, struct tfsm_state_t *, struct tfsm_retval *);
} tfsm_fsm_t;

typedef tfsm_retval *(*tfsm_state_fn)(struct tfsm_fsm_t *tfsm, struct tfsm_ctx_t *ctx);

/* ******************************************************************************* *
 *                                  fsm interface                                  *
 * ******************************************************************************* */

tfsm_fsm_t *tfsm_fsm_new(void);
tfsm_fsm_t *tfsm_fsm_create_from_file(const char *filename, enum tfsm_fsm_type type);
//void tfsm_fsm_cleanup(tfsm_fsm_t *tfsm);
void tfsm_fsm_add_state(tfsm_fsm_t *tfsm, tfsm_state_t *state);
void tfsm_fsm_print(tfsm_fsm_t *tfsm);
void tfsm_fsm_inject_fn(tfsm_fsm_t *tfsm, tfsm_state_fn fn, const char *fnname);
tfsm_state_t *tfsm_state_find(tfsm_fsm_t *tfsm, const char *name, enum tfsm_state_type flag);

/* ******************************************************************************* *
 *                                  fstate functions                               *
 * ******************************************************************************* */

tfsm_state_t *tfsm_state_new(void);
//void tfsm_state_delete(tfsm_state_t *state);
void tfsm_state_set_type(tfsm_state_t *state, const char *type);
void tfsm_state_set_name(tfsm_state_t *state, const char *name);
void tfsm_state_set_source(tfsm_state_t *tfsm, const char *s, const char *f);
void tfsm_state_add_transition(tfsm_state_t *tfsm, const char *ret_val, const char *next_state);
void tfsm_state_print(tfsm_state_t *state);

/* ******************************************************************************* *
 *                              transition functions                               *
 * ******************************************************************************* */

tfsm_transition_t *tfsm_trs_new(void);


char *tfsm_push(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx);
void tfsm_ctx_set(tfsm_ctx_t *ctx, const char *key, char *val);
tfsm_ctx_param_t *tfsm_ctx_get(tfsm_ctx_t *ctx, const char *key);
char *tfsm_ctx_getstr(tfsm_ctx_t *ctx, const char *key);
tfsm_ctx_t *tfsm_ctx_new(void);
tfsm_retval *tfsm_r_adopt_int(tfsm_ctx_t *ctx, int retval);
tfsm_state_t *tfsm_r_find_pairing(tfsm_fsm_t *tfsm, tfsm_state_t *state, char *ret_val);
int tfsm_r_ready(tfsm_fsm_t *tfsm);
#endif
