#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpc.h"
#include "tfsm.h"
static char *tfsm_strdup(const char *strng);
static char *strip_se(const char *str);
static void tfsm_scan_dcl_source(mpc_result_t *r, const char *dcl_source_fn);
static void tfsm_fsm_add_uniq_ret_val(tfsm_fsm_t *tfsm,  char *ret_val);
static void tfsm_fsm_set_table_lookup(tfsm_fsm_t *tfsm);

/* ******************************************************************************* *
 *                                  util functions                                 *
 * ******************************************************************************* */

static char *
strip_se(const char *str) {
    char *tmp;
    int i;
    int len = strlen(str);
    // add one for '\0', minus two for quotes
    tmp = malloc(len + 1 - 2);
    for(i = 0; i < len - 2; i++) {
        tmp[i] = str[i+1];
    }
    tmp[i] = '\0';
    return tmp;
}

static char *
tfsm_strdup(const char *strng) {
    return strcpy(malloc(strlen(strng) + 1), strng);
}

void
tfsm_state_print(tfsm_state_t *state) {
    tfsm_transition_t *trs;
    switch(state->type) {
        case TFSM_STATE_T_NODE:
            printf("node::%s\n", state->name);
            TAILQ_FOREACH(trs, &state->transitions, trs) {
                printf("    %s, %s => %s\n",
                        trs->from_state,
                        trs->ret_val,
                        trs->to_state);
            }
            break;
        case TFSM_STATE_T_FINI:
            printf("fini::%s\n", state->name);
            break;
        case TFSM_STATE_T_INIT:
            printf("init::%s\n", state->name);
            break;
        default:
            printf("Should never see this.\n");
    }
    putc('\n',stdout);
}

void
tfsm_fsm_print(tfsm_fsm_t *tfsm) {
    tfsm_state_t *state;
    printf("tfsm: %s\n", tfsm->source_name);
    printf("pending functions: %d", tfsm->pending_fn_num);
    TAILQ_FOREACH(state, &tfsm->states, state_list) {
        tfsm_state_print(state);
    }
}

/* ******************************************************************************* *
 *                                  fsm functions                                  *
 * ******************************************************************************* */

// XXX
tfsm_fsm_t *
tfsm_fsm_new(void) {
    tfsm_fsm_t *tfsm;
    tfsm = malloc(sizeof(tfsm_fsm_t));
    tfsm->known_ret_vals = calloc(TFSM_MAX_RETVAL_NUM, sizeof(char *));
    tfsm->pending_fn_num = 0;
    tfsm->states_num = 0;
    tfsm->ret_val_num = 0;
    TAILQ_INIT(&tfsm->states);
    return tfsm;
}

/* XXX
void
tfsm_fsm_cleanup(tfsm_fsm_t *tfsm) {
    printf("Mock free tfsm");
}
*/

// XXX
void
tfsm_fsm_add_state(tfsm_fsm_t *tfsm, tfsm_state_t *state) {
    TAILQ_INSERT_TAIL(&tfsm->states, state, state_list);
    tfsm->states_num++;
    if (state->type == TFSM_STATE_T_NODE && (strcmp(state->source_file,"<inject>") == 0)) {
        tfsm->pending_fn_num++;
    }
}

void
tfsm_fsm_inject_fn(tfsm_fsm_t *tfsm, tfsm_state_fn fn, const char *fnname) {
    tfsm_state_t *state;
    int memo_pending = tfsm->pending_fn_num;

    if(tfsm->pending_fn_num > 0) {
        TAILQ_FOREACH(state, &tfsm->states, state_list) {
            if(((state->type == TFSM_STATE_T_NODE) &&
                (strcmp(state->source_file, "<inject>") == 0) &&
                (strcmp(state->function_name, fnname) == 0))) {
                state->fn = (void *)fn;
                tfsm->pending_fn_num--;
                break;
            }
        }
        if (memo_pending == tfsm->pending_fn_num) {
            fprintf(stderr, "No state waiting for '%s' function. Skipping.", fnname);
        }
    } else {
        fprintf(stderr, "No free slot for  '%s' function. Skipping.\n", fnname);
    }
}

static void
tfsm_fsm_add_uniq_ret_val(tfsm_fsm_t *tfsm,  char *ret_val) {
    int i;
    for(i = 0; i < TFSM_MAX_RETVAL_NUM; i++) {
        if(tfsm->known_ret_vals[i] == NULL) {
            tfsm->known_ret_vals[i] = ret_val;
            tfsm->ret_val_num++;
            break;
        } else if ((strcmp(tfsm->known_ret_vals[i], ret_val) == 0)) {
            break;
        }
    }
}

/* ******************************************************************************* *
 *                                   state functions                               *
 * ******************************************************************************* */

// XXX
tfsm_state_t *
tfsm_state_new(void) {
    tfsm_state_t *state;
    state = malloc(sizeof(tfsm_state_t));
    state->transition_num = 0;
    TAILQ_INIT(&state->transitions);
    return state;
}

/* XXX
void
tfsm_state_delete(tfsm_state_t *state) {
    printf("Mock free tfsm_state");
}
*/

void
tfsm_state_set_type(tfsm_state_t *state, const char *type) {
    enum tfsm_state_type st_type;
    if((strcmp(type, "node") == 0)) {
        st_type = TFSM_STATE_T_NODE;
    } else if (strcmp(type, "fini") == 0) {
        st_type = TFSM_STATE_T_FINI;
    } else if (strcmp(type, "init") == 0) {
        st_type = TFSM_STATE_T_INIT;
    } else {
        fprintf(stderr, "Error. No such state type: %s\n", type);
        exit(1);
    }
    state->type = st_type;
}

// XXX
void
tfsm_state_set_name(tfsm_state_t *state, const char *name) {
    state->name = tfsm_strdup(name);
}

// XXX
void
tfsm_state_set_source(tfsm_state_t *state, const char *s, const char *fn) {
    state->source_file = strip_se(s);
    state->function_name = strip_se(fn);
}

// XXX
void
tfsm_state_add_transition(tfsm_state_t *state, const char *s, const char *f) {
    tfsm_transition_t *trs = tfsm_trs_new();
    trs->from_state = tfsm_strdup(state->name);
    trs->ret_val = tfsm_strdup(s);
    trs->to_state = tfsm_strdup(f);
    state->transition_num++;
    TAILQ_INSERT_TAIL(&state->transitions, trs, trs);
}

tfsm_state_t *
tfsm_state_find(tfsm_fsm_t *tfsm, const char *name, enum tfsm_state_type flag){
    tfsm_state_t *state;
    TAILQ_FOREACH(state, &tfsm->states, state_list) {
        if((flag & state->type) && (strcmp(state->name, name) == 0)) {
            return state;
        }
    }
    return NULL;
}

/* ******************************************************************************* *
 *                               transition functions                              *
 * ******************************************************************************* */

// XXX
tfsm_transition_t *
tfsm_trs_new(void) {
    return malloc(sizeof(tfsm_transition_t));
}

tfsm_transition_t *
tfsm_trs_find(tfsm_state_t *state, char *ret_val) {
    tfsm_transition_t *trss;
    TAILQ_FOREACH(trss, &state->transitions, trs) {
        if((strcmp(trss->ret_val, ret_val) == 0)) {
            return trss;
        }
    }
    return NULL;
}

/* ******************************************************************************* *
 *                               runtime functions                                 *
 * ******************************************************************************* */

int
tfsm_r_final(tfsm_state_t *state) {
    return (state->type == TFSM_STATE_T_FINI);
}

tfsm_retval *
tfsm_r_adopt_int(tfsm_ctx_t *ctx, int retval) {
    ctx->runtime_val->val.i = retval;
    return ctx->runtime_val;
}

tfsm_state_t *
tfsm_r_next_state(tfsm_fsm_t *tfsm, tfsm_state_t *state, tfsm_retval *rv) {
    return tfsm->next_state(tfsm, state, rv);
}

char *
tfsm_push(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    tfsm_state_t *state;
    tfsm_retval *ret_val;

    state = tfsm->init_state;
    while(1) {
        ret_val = ((tfsm_state_fn)state->fn)(tfsm, ctx);
        state = tfsm_r_next_state(tfsm, state, ret_val);
        if (state == NULL || tfsm_r_final(state)) { break; }
    }
    return state->name;
}

/* ******************************************************************************* *
 *                          scanning & lexing functions                            *
 * ******************************************************************************* */

tfsm_fsm_t *
tfsm_fsm_create_from_file(const char *dcl_filename, enum tfsm_fsm_type fsm_type) {
    mpc_result_t r;
    mpc_ast_t *ast, *_ast;
    tfsm_fsm_t *tfsm;
    tfsm_state_t *state;

    char *trans_ret_val;
    char *trans_next_state;
    char *init_state_name;

    int i, j;

    tfsm_scan_dcl_source(&r, dcl_filename);
    ast = r.output;

    tfsm = tfsm_fsm_new();
    tfsm->source_name = tfsm_strdup(dcl_filename);

    // foind all states
    for(i = 1; i < ast->children_num; i++) {
        _ast = ast->children[i];
        if(strcmp(_ast->tag, "clause|>") == 0) {
            // first node of 'clause' is always 'declaration'  so, check second node
            state = tfsm_state_new();

            // every declaration node is simple declaration
            tfsm_state_set_type(state, _ast->children[0]->children[1]->contents);
            tfsm_state_set_name(state, _ast->children[0]->children[3]->contents);

            // memorize initial state name from declaration
            if(state->type == TFSM_STATE_T_INIT) {
                init_state_name = state->name;
            }

            // in case of full definition - collect all transitions
            if(strcmp(_ast->children[1]->contents, ";") != 0) {
                // source is second node (source file, function name)
                tfsm_state_set_source(state, _ast->children[1]->children[1]->contents,
                                             _ast->children[1]->children[3]->contents);

                for(j = 0; j < _ast->children[3]->children_num; j++) {
                    // get all statemaps
                    trans_ret_val = _ast->children[3]->children[j]->children[0]->contents;
                    trans_next_state = _ast->children[3]->children[j]->children[2]->contents;
                    tfsm_fsm_add_uniq_ret_val(tfsm, trans_ret_val);
                    tfsm_state_add_transition(state, trans_ret_val, trans_next_state);
                }
            }
            // since i starts from 1
            state->idx = i - 1;
            tfsm_fsm_add_state(tfsm, state);
        }
    }

    // set initial state
    TAILQ_FOREACH(state, &tfsm->states, state_list) {
        if(strcmp(state->name, init_state_name) == 0) {
            tfsm->init_state = state;
        }
    }

    // create lookup object
    switch (fsm_type) {
        case TFSM_FSM_T_TABLE:
            tfsm_fsm_set_table_lookup(tfsm);
            break;
        case TFSM_FSM_T_LIST:
        case TFSM_FSM_T_NORMAL:
            break;
    }
    return tfsm;
}

int
tfsm_state_has_ret_val(tfsm_state_t *state, const char *ret_val_name) {
    tfsm_transition_t *trs;
    TAILQ_FOREACH(trs, &state->transitions, trs) {
        if (strcmp(trs->ret_val, ret_val_name) == 0) {
            return 1;
        }
    }
    return 0;
}

tfsm_state_t *
tfsm_fsm_next_state_from_table(tfsm_fsm_t *tfsm, tfsm_state_t *state, tfsm_retval *rv) {
    return tfsm->lookup_table[state->idx][rv->val.i];
}

static void
tfsm_fsm_set_table_lookup(tfsm_fsm_t *tfsm) {
    int i = 0;
    int j;
    tfsm_state_t *state;
    tfsm_state_t *next_state;
    // set table lookub function as main lookup function
    tfsm->next_state = &tfsm_fsm_next_state_from_table;

    // allocate 2d array of pointers to states
    if((tfsm->lookup_table = malloc(sizeof(tfsm_state_t **) * tfsm->states_num)) == NULL) {
        printf("Failed malloc\n");
        exit(1);
    }

    TAILQ_FOREACH(state, &tfsm->states, state_list) {
        if((tfsm->lookup_table[i] = malloc(sizeof(tfsm_state_t *) * tfsm->ret_val_num)) == NULL) {
            printf("Failed malloc\n");
            exit(1);
        }
        for(j = 0; j < tfsm->ret_val_num; j++){
            if(tfsm_state_has_ret_val(state, tfsm->known_ret_vals[j])) {
                next_state = tfsm_r_find_pairing(tfsm, state, tfsm->known_ret_vals[j]);
                tfsm->lookup_table[i][j] = next_state;
            } else {
                tfsm->lookup_table[i][j] = NULL;
            }
        }
        i++;
    }
}

tfsm_state_t *
tfsm_r_find_pairing(tfsm_fsm_t *tfsm, tfsm_state_t *state, char *ret_val) {
    tfsm_state_t *fstate;
    tfsm_transition_t *trs;
    trs = tfsm_trs_find(state, ret_val);
    fstate = tfsm_state_find(tfsm, trs->to_state, TFSM_STATE_T_NODE|TFSM_STATE_T_FINI);
    return fstate;
}

static void
tfsm_scan_dcl_source(mpc_result_t *r, const char *dcl_source_fn) {
    mpc_parser_t *Ident       = mpc_new("ident");
    mpc_parser_t *TypeIdent   = mpc_new("typeident");
    mpc_parser_t *Strng       = mpc_new("strng");
    mpc_parser_t *Decl        = mpc_new("declaration");
    mpc_parser_t *Source      = mpc_new("source");
    mpc_parser_t *StateMap    = mpc_new("statemap");
    mpc_parser_t *StateMents  = mpc_new("statements");
    mpc_parser_t *Clause      = mpc_new("clause");
    mpc_parser_t *FsmConf     = mpc_new("fsm");
    
    mpc_err_t *err = mpca_lang(MPCA_LANG_DEFAULT,
        "ident       : /[a-zA-Z_][a-zA-Z0-9_]*/ ;                      \n" 
        "typeident   : (\"init\" | \"fini\" | \"node\") ;                    \n"
        "strng       : /\"(\\\\.|[^\"])*\"/ ;                          \n"
        "declaration : \"def\" <typeident> \"::\" <ident> ;                \n"
        "source      : '(' <strng> ',' <strng> ')' ;                   \n"
        "statemap    : <ident> \"=>\" <ident> ';' ;                      \n"
        "statements  : <statemap> (<statemap>)* ;                      \n"
        "clause      : <declaration> ';'                               \n"
        "            | <declaration> <source> ':' <statements> \"end\" ; \n"
        "fsm         : /^/ <clause> (<clause>)* /$/ ;                  \n",
		Ident, TypeIdent, Strng, Decl, Source, StateMap, StateMents, Clause,
        FsmConf, NULL);


	if(err != NULL) {
		mpc_err_print(err);
		mpc_err_delete(err);
		exit(1);
	}

    if (!mpc_parse_contents(dcl_source_fn, FsmConf, r)) {
        mpc_err_print(r->error);
        mpc_err_delete(r->error);
        mpc_cleanup(9, Ident, TypeIdent, Strng, Decl, Source, StateMap, StateMents,
                Clause, FsmConf);
        exit(1);
    }
    mpc_cleanup(9, Ident, TypeIdent, Strng, Decl, Source, StateMap, StateMents,
                Clause, FsmConf);
}

/* ******************************************************************************* *
 *                               ctx functions                                     *
 * ******************************************************************************* */

tfsm_ctx_t *
tfsm_ctx_new(void) {
    tfsm_ctx_t *ctx;
    ctx = malloc(sizeof(ctx));
    ctx->runtime_val = calloc(1, sizeof(tfsm_retval));
    TAILQ_INIT(&ctx->params);
    return ctx;
}

void
tfsm_ctx_set(tfsm_ctx_t *ctx, const char *key, char *val) {
    tfsm_ctx_param_t *param;
    size_t key_len;
    TAILQ_FOREACH(param, &ctx->params, param_list) {
        if(strcmp(param->key, key) == 0) {
            if(param->val) {
                free(param->val);
            }
            param->val = val;
            return;
        }
    }
    key_len = strlen(key) + 1;
    // allocating one chunk of memory for struct and fields
    param = malloc(sizeof(*param) + key_len + 1 + strlen(val) + 1);
    param->key = (char *)(param + 1);
    param->val = param->key + key_len + 1;
    strcpy(param->key, key);
    strcpy(param->val, val);
    TAILQ_INSERT_TAIL(&ctx->params, param, param_list);
}

tfsm_ctx_param_t *
tfsm_ctx_get(tfsm_ctx_t *ctx, const char *key) {
    tfsm_ctx_param_t *param;

    TAILQ_FOREACH(param, &ctx->params, param_list) {
        if(strcmp(param->key, key) == 0) {
            return param;
        }
    }
    return NULL;
}

char *
tfsm_ctx_getstr(tfsm_ctx_t *ctx, const char *key) {
    return tfsm_ctx_get(ctx, key)->val;
}
