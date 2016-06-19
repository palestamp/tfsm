#include <stdio.h>

#include "tfsm.h"


void 
usage() {
    printf(
        "gen_fsm_enum - automatic enum generator for tfsm\n"
        "Usage:                                          \n"
        "      dcl_fsm_file                              \n"
        "      prefix for enum                           \n"
        "      out file name                             \n"
    );
}

void 
write_states_enum(tfsm_fsm_t *tfsm, FILE *enum_out, char *prefix) {
    char name_swap_buf[1024] = {0};
    char *tmp_name;
    tfsm_state_t *state;
    const char start[] = "typedef enum {\n";
    const char end[] = "} states;\n";
    size_t ident_name_len;
    size_t plen = strlen(prefix);
    memcpy(name_swap_buf, prefix, plen);
    name_swap_buf[plen] = '_';

    fwrite(start, 1, strlen(start), enum_out);
    TAILQ_FOREACH(state, &tfsm->states, state_list) {
        if(state->type != TFSM_STATE_T_INIT) {
            ident_name_len = strlen(state->name);
            tmp_name = strcat(name_swap_buf, state->name);

            fwrite("    ", 1, 4, enum_out);
            fwrite(tmp_name, 1, ident_name_len + 1 + plen, enum_out);
            fwrite(",\n", 1, 2, enum_out);

            memset((name_swap_buf + 1 + plen), 0, ident_name_len);
        }
    }
    fwrite(end, 1, strlen(end), enum_out);
}

void
write_ret_val_enum(tfsm_fsm_t *tfsm, FILE *enum_out, char *prefix) {
    int i;
    char name_swap_buf[1024] = {0};
    char *tmp_name;
    const char start[] = "typedef enum {\n";
    const char end[] = "} ret_val;\n";

    size_t ident_name_len;
    size_t plen = strlen(prefix);
    memcpy(name_swap_buf, prefix, plen);
    name_swap_buf[plen] = '_';

    fwrite(start, 1, strlen(start), enum_out);
    for(i = 0; i < 1024 && tfsm->known_ret_vals[i]; i++) {
            ident_name_len = strlen(tfsm->known_ret_vals[i]);
            tmp_name = strcat(name_swap_buf, tfsm->known_ret_vals[i]);

            fwrite("    ", 1, 4, enum_out);
            fwrite(tmp_name, 1, ident_name_len + 1 + plen, enum_out);
            fwrite(",\n", 1, 2, enum_out);
            memset((name_swap_buf + 1 + plen), 0, ident_name_len);
    }
    fwrite(end, 1, strlen(end), enum_out);
}

int
main(int argc, char **argv) {
    if(argc < 4) {
        usage();
        exit(1);
    }
    FILE *enum_out = fopen(argv[3], "w");
    if(enum_out == NULL) {
        printf("Cant create file!\n");
        exit(1);
    }
    tfsm_fsm_t *tfsm = tfsm_fsm_create_from_file(argv[1], TFSM_FSM_T_TABLE);
    write_states_enum(tfsm, enum_out, argv[2]);
    fwrite("\n\n", 1, 2, enum_out);
    write_ret_val_enum(tfsm, enum_out, argv[2]);


    fclose(enum_out);
    return 0;
}
