#ifndef AULA_KEYMAP_CLI_H
#define AULA_KEYMAP_CLI_H
/* Handles --keymap, --remap and macro list/add/update/delete.
 * Return-1 if not a keymap command; otherwise a normal process exit code. */
int aula_keymap_cli(int argc,char **argv);
#endif
