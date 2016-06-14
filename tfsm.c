#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpc.h"
#include "tfsm.h"

static char *tfsm_strdup(const char *strng);
static char *strip_se(char *str);
static void tfsm_scan_dcl_source(mpc_result_t *r, const char *dcl_source_fn);

/* ******************************************************************************* *
 *                                  util functions                                 *
 * ******************************************************************************* */

static char *
strip_se(char *str) {
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
    
    tfsm->pending_fn_num = 0;
    TAILQ_INIT(&tfsm->states);
    return tfsm;
}

// XXX
void
tfsm_fsm_cleanup(tfsm_fsm_t *tfsm) {
    printf("Mock free tfsm");
}

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

tfsm_fsm_t *tfsm_fsm_fast_table(tfsm_fsm_t *tfsm) {
    tfsm_state_t *state;
    char *init_state = NULL;
    tfsm_transition_t *trs;
    int total_trss = 0;
    if(tfsm->pending_fn_num != 0) {
        fprintf(stderr, "Not all requiered functions injected.\n");
        exit(1);
    }
    TAILQ_INIT(&tfsm->trs_table);
    TAILQ_FOREACH(state, &tfsm->states, state_list) {
        switch(state->type) {
            case TFSM_STATE_T_INIT:
                if(init_state != NULL) {
                    fprintf(stderr, "Redeclaration of init state.");
                    exit(1);
                }
                init_state = tfsm_strdup(state->name);
                break;
            case TFSM_STATE_T_NODE:
                total_trss += state->transition_num;
                TAILQ_FOREACH(trs, &state->transitions, trs) {
                    TAILQ_INSERT_TAIL(&tfsm->trs_table, trs, tbl_trs);
                }
                break;
            // XXX SHIT
            default:
                break;
        }
    }
    if(init_state == NULL) {
        fprintf(stderr, "No init state found.");
        exit(1);
    }
    tfsm->init_state = init_state;
    return tfsm;
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

// XXX
void
tfsm_state_delete(tfsm_state_t *state) {
    printf("Mock free tfsm_state");
}

void
tfsm_state_set_type(tfsm_state_t *state, const char *type) {
    tfsm_state_type_t st_type;
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
tfsm_state_find(tfsm_fsm_t *tfsm, const char *name, tfsm_state_type_t flag){
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

tfsm_state_t *
tfsm_r_find_pairing(tfsm_fsm_t *tfsm, tfsm_state_t *state, char *ret_val) {
    tfsm_state_t *fstate;
    tfsm_transition_t *trs;
    trs = tfsm_trs_find(state, ret_val);
    fstate = tfsm_state_find(tfsm, trs->to_state, TFSM_STATE_T_NODE|TFSM_STATE_T_FINI);
    return fstate;
}

int
tfsm_r_finite(tfsm_state_t *state) {
    return (state->type == TFSM_STATE_T_FINI);
}

char *
tfsm_push(tfsm_fsm_t *tfsm, tfsm_context_t *ctx) {
    tfsm_state_t *state;
    char *ret_val;
    // find initial trans by name
    state = tfsm_state_find(tfsm, tfsm->init_state, TFSM_STATE_T_NODE);
    while(1) {
        ret_val = ((tfsm_state_fn)state->fn)(tfsm, ctx);
        state = tfsm_r_find_pairing(tfsm, state, ret_val);
        if (tfsm_r_finite(state)) { break; }
    }
    return state->name;
}

/* ******************************************************************************* *
 *                          scanning & lexing functions                            *
 * ******************************************************************************* */

tfsm_fsm_t *
tfsm_fsm_create_from_file(const char *dcl_filename) {
    mpc_result_t r;
    mpc_ast_t *ast, *_ast;
    tfsm_fsm_t *tfsm;
    tfsm_state_t *state;

    char *trans_ret_val;
    char *trans_next_state;

    tfsm_scan_dcl_source(&r, dcl_filename);
    ast = r.output;

    tfsm = tfsm_fsm_new();
    tfsm->source_name = tfsm_strdup(dcl_filename);
    for(int i = 1; i < ast->children_num; i++) {
        _ast = ast->children[i];
        if(strcmp(_ast->tag, "clause|>") == 0) {
            // first node of 'clause' is always 'declaration'  so, check second node
            state = tfsm_state_new();

            // every declaration node is simple declaration
            tfsm_state_set_type(state, _ast->children[0]->children[1]->contents);
            tfsm_state_set_name(state, _ast->children[0]->children[3]->contents);

            if(strcmp(_ast->children[1]->contents, ";") != 0) {
                // source is second node (source file, function name)
                tfsm_state_set_source(state, _ast->children[1]->children[1]->contents,
                                             _ast->children[1]->children[3]->contents);

                for(int j = 0; j < _ast->children[3]->children_num; j++) {
                    // get all statemaps
                    trans_ret_val = _ast->children[3]->children[j]->children[0]->contents;
                    trans_next_state = _ast->children[3]->children[j]->children[2]->contents;
                    tfsm_state_add_transition(state, trans_ret_val, trans_next_state);
                }
            }
            tfsm_fsm_add_state(tfsm, state);
        }
    }
    return tfsm;
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

    FILE *grammar = fopen(TFSM_GRAMMAR_SOURCE, "r");
    if (grammar == NULL) {
        fprintf(stderr, "No %s found!", TFSM_GRAMMAR_SOURCE);
        exit(1);
    }

    mpc_err_t *err = mpca_lang_file(MPCA_LANG_DEFAULT, grammar,
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
}

