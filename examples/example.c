#include "../tfsm.h"


char *get_retval(tfsm_fsm_t *asdd, const char * asd) {
    return "1";
}

char *test_photo(tfsm_fsm_t *tfsm, tfsm_context_t *ctx) {
    double h_metric = 75L; 
    if (h_metric > 85) {
        return "NO";
    } else {
        return "YES";
    } 
}

char *test_text(tfsm_fsm_t *tfsm, tfsm_context_t *ctx) {
    long h_metric = 14L;
    if (h_metric > 13) {
        return "UNDER_15";
    } else {
        return "UNDER_80";
    } 
}

char *test_company(tfsm_fsm_t *tfsm, tfsm_context_t *ctx) {
    long h_metric = 15L;
    if (h_metric > 13) {
        return "YES";
    } else {
        return "NO";
    } 
}


int
main(void) {
    tfsm_context_t ctx = {};
    
    tfsm_fsm_t *tfsm = tfsm_fsm_create_from_file("dcl_states.txt");
    tfsm_fsm_print(tfsm);
    tfsm_fsm_inject_fn(tfsm, test_photo, "test_photo");
    tfsm_fsm_inject_fn(tfsm, test_text, "test_text");
    tfsm_fsm_inject_fn(tfsm, test_company, "test_company");
    tfsm_fsm_print(tfsm);
    tfsm = tfsm_fsm_fast_table(tfsm);

    for(int k = 0; k < 100; k++) {    
        printf("%s\n",tfsm_push(tfsm, &ctx));
    }

    return 0;
}
