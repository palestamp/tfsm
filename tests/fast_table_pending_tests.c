#include <limits.h>
#include <errno.h>

#include "tfsm.h"
#include "dcl_states.h"
#include "dbg.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

tfsm_retval *
test_is_new_user(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int new_user = atoi(tfsm_ctx_getstr(ctx, "is_new_user"));
    int answer = new_user ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_has_photo(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int has_photo = atoi(tfsm_ctx_getstr(ctx, "has_photo"));
    int answer = has_photo ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_is_photo_nude(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int is_photo_nude = atoi(tfsm_ctx_getstr(ctx, "is_photo_nude"));
    int answer = is_photo_nude ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_photo_duplicate(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int has_photo_duplicates = atoi(tfsm_ctx_getstr(ctx, "has_photo_duplicates"));
    int answer = has_photo_duplicates ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_text_duplicate(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int has_text_duplicates = atoi(tfsm_ctx_getstr(ctx, "has_text_duplicates"));
    int answer = has_text_duplicates ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_is_private_ad(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int is_private_ad = atoi(tfsm_ctx_getstr(ctx, "is_private_ad"));
    int answer = is_private_ad ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_is_looks_like_company(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int looks_like_company = atoi(tfsm_ctx_getstr(ctx, "looks_like_company"));
    int answer = looks_like_company ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_is_from_partner_and_in_autoaccept(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int from_partner = atoi(tfsm_ctx_getstr(ctx, "is_from_partner"));
    int in_autoaccept_list = atoi(tfsm_ctx_getstr(ctx, "in_autoaccept_list"));
    int answer = (from_partner && in_autoaccept_list) ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}

tfsm_retval *
test_is_in_autoaccept_category(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int category = atoi(tfsm_ctx_getstr(ctx, "category"));
    switch(category / 1000) {
        case 1000:
        case 2000:
        case 3000:
        case 4000:
        case 5000:
        case 6000: return tfsm_r_adopt_int(ctx, R_yes);
        case 7000:
        case 8000:
        case 9000:
        case 10000:
        case 11000:
        case 12000:
        case 13000:
        default: return tfsm_r_adopt_int(ctx, R_no); 
    }
}

tfsm_retval *
test_is_user_in_whitelist(tfsm_fsm_t *tfsm, tfsm_ctx_t *ctx) {
    int in_white_list = atoi(tfsm_ctx_getstr(ctx, "user_in_whitelist"));
    int answer = in_white_list ? R_yes : R_no; 
    return tfsm_r_adopt_int(ctx, answer);
}


int
main(void) {
    tfsm_ctx_t *ctx0 = tfsm_ctx_new();
    tfsm_ctx_t *ctx1 = tfsm_ctx_new();
    tfsm_ctx_t *ctx2 = tfsm_ctx_new();
    tfsm_ctx_t *ctx3 = tfsm_ctx_new();

    tfsm_ctx_set(ctx0, "is_new_user", "0");
    tfsm_ctx_set(ctx0, "has_photo", "1");
    tfsm_ctx_set(ctx0, "is_photo_nude", "1");  // HERE TO MANUAL
    tfsm_ctx_set(ctx0, "has_photo_duplicates", "0");
    tfsm_ctx_set(ctx0, "has_text_duplicates", "0");
    tfsm_ctx_set(ctx0, "is_private_ad", "1");
    tfsm_ctx_set(ctx0, "looks_like_company", "0");
    tfsm_ctx_set(ctx0, "is_from_partner", "1");
    tfsm_ctx_set(ctx0, "in_autoaccept_list", "1");
    tfsm_ctx_set(ctx0, "category", "13000");
    tfsm_ctx_set(ctx0, "user_in_whitelist", "1");
    
    tfsm_ctx_set(ctx1, "is_new_user", "0");
    tfsm_ctx_set(ctx1, "has_photo", "0");
    tfsm_ctx_set(ctx1, "is_photo_nude", "0");
    tfsm_ctx_set(ctx1, "has_photo_duplicates", "0");
    tfsm_ctx_set(ctx1, "has_text_duplicates", "1"); // HERE REFUSE
    tfsm_ctx_set(ctx1, "is_private_ad", "1");
    tfsm_ctx_set(ctx1, "looks_like_company", "0");
    tfsm_ctx_set(ctx1, "is_from_partner", "1");
    tfsm_ctx_set(ctx1, "in_autoaccept_list", "1");
    tfsm_ctx_set(ctx1, "category", "13000");
    tfsm_ctx_set(ctx1, "user_in_whitelist", "1");
    
    tfsm_ctx_set(ctx2, "is_new_user", "0");
    tfsm_ctx_set(ctx2, "has_photo", "1");
    tfsm_ctx_set(ctx2, "is_photo_nude", "0");
    tfsm_ctx_set(ctx2, "has_photo_duplicates", "0");
    tfsm_ctx_set(ctx2, "has_text_duplicates", "0");
    tfsm_ctx_set(ctx2, "is_private_ad", "1");
    tfsm_ctx_set(ctx2, "looks_like_company", "0");
    tfsm_ctx_set(ctx2, "is_from_partner", "1");
    tfsm_ctx_set(ctx2, "in_autoaccept_list", "1"); // HERE TO AUTOACCEPT
    tfsm_ctx_set(ctx2, "category", "13000");
    tfsm_ctx_set(ctx2, "user_in_whitelist", "1");

    tfsm_ctx_set(ctx3, "is_new_user", "0");
    tfsm_ctx_set(ctx3, "has_photo", "0");
    tfsm_ctx_set(ctx3, "is_photo_nude", "0");
    tfsm_ctx_set(ctx3, "has_photo_duplicates", "0");
    tfsm_ctx_set(ctx3, "has_text_duplicates", "0");
    tfsm_ctx_set(ctx3, "is_private_ad", "1");
    tfsm_ctx_set(ctx3, "looks_like_company", "0");
    tfsm_ctx_set(ctx3, "is_from_partner", "1");
    tfsm_ctx_set(ctx3, "in_autoaccept_list", "0");
    tfsm_ctx_set(ctx3, "category", "13000");
    tfsm_ctx_set(ctx3, "user_in_whitelist", "0"); // HERE TO MANUAL

    char buf[PATH_MAX];
    realpath("tests/dcl_states.tfsm", buf);
    tfsm_fsm_t *tfsm = tfsm_fsm_create_from_file(buf, TFSM_FSM_T_TABLE);

    tfsm_fsm_inject_fn(tfsm, test_is_new_user, "test_is_new_user");
    tfsm_fsm_inject_fn(tfsm, test_has_photo, "test_has_photo");
    tfsm_fsm_inject_fn(tfsm, test_is_photo_nude, "test_is_photo_nude");
    tfsm_fsm_inject_fn(tfsm, test_photo_duplicate, "test_photo_duplicate");
    tfsm_fsm_inject_fn(tfsm, test_text_duplicate, "test_text_duplicate");
    tfsm_fsm_inject_fn(tfsm, test_is_private_ad, "test_is_private_ad");
    tfsm_fsm_inject_fn(tfsm, test_is_looks_like_company, "test_is_looks_like_company");
    tfsm_fsm_inject_fn(tfsm, test_is_from_partner_and_in_autoaccept, "test_is_from_partner_and_in_autoaccept");
    tfsm_fsm_inject_fn(tfsm, test_is_in_autoaccept_category, "test_is_in_autoaccept_category");
//    tfsm_fsm_inject_fn(tfsm, test_is_user_in_whitelist, "test_is_user_in_whitelist");
    
    check(tfsm_push(tfsm, ctx0) == NULL, "Failed on first push");
    check(tfsm_push(tfsm, ctx1) == NULL, "Failed on second push");
    check(tfsm_push(tfsm, ctx2) == NULL, "Failed on third push");
    check(tfsm_push(tfsm, ctx3) == NULL, "Failed on fourth push");
    check(tfsm_r_ready(tfsm) == TFSM_PENDING, "Failed, tfsm not ready");  

    return 0;
error:
    return 1;
}
