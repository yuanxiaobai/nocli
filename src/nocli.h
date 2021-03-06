//
// nocli.h
//
// Basic command-line interpreter.
// 
// # Usage
// Instantiate and fill in required sections of `struct Nocli`; see `example.c` for a simple
// example.
// 
// # Configuration
// Buffer sizes, etc. can be adjusted by defining `NOCLI_CONFIG_*` options in `nocli_config.h`.
//
#pragma once
#include "nocli_config.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Depth of command history
#if !defined(NOCLI_CONFIG_HISTORY_DEPTH)
#define NOCLI_CONFIG_HISTORY_DEPTH (5)
#endif

// Maximum command length
#if !defined(NOCLI_CONFIG_MAX_COMMAND_LENGTH)
#define NOCLI_CONFIG_MAX_COMMAND_LENGTH (128)
#endif

// Maximum number of tokens in a single command (eg "command option1 option2" is 3 tokens)
#if !defined(NOCLI_CONFIG_MAX_COMMAND_TOKENS)
#define NOCLI_CONFIG_MAX_COMMAND_TOKENS (10)
#endif

// Commands are always terminated by LF, '\n'. Output line end can be configured here
#if !defined(NOCLI_CONFIG_ENDLINE_STRING)
#define NOCLI_CONFIG_ENDLINE_STRING ("\n")
#endif

// Built in help command, enabled by default, set to 0 to disable
#if !defined(NOCLI_CONFIG_HELP_COMMAND)
#define NOCLI_CONFIG_HELP_COMMAND (1)
#endif

// Module error type
enum NocliErrors {
    kNocliOK = 0,
    kNocliError,
    kNocliOutOfMemory,
};

// Command structure
struct NocliCommand {
    const char *name;    // command name string, eg "cd"
    void (*function)(int argc, char **argv);    // command function
};

// Nocli context, intantiated by calling code. All the memory used by nocli exists here.
//  Fields are divided into 3 classes:
//  1. configured prior to setup
//  2. configured prior to setup; reconfigurable at runtime
//  3. internally managed; obfuscated block
#define NOCLI_PRIVATE_CONTEXT_SIZE (NOCLI_CONFIG_MAX_COMMAND_LENGTH +\
    (NOCLI_CONFIG_HISTORY_DEPTH * (NOCLI_CONFIG_MAX_COMMAND_LENGTH + sizeof(void*))))
struct Nocli {
    // 1. configured prior to setup
    void (*output_stream)(char *, size_t);    // nocli uses this for stdout
    
    // 2. reconfiguratble at runtime
    const struct NocliCommand *command_table;    // table of commands
    const size_t command_table_length;    // length of command table; must match command table
    char *prefix_string;    // leading string for prompt (eg "$ ")
    char *error_string;    // print this if there's an error
    bool echo_on;    // enable or disable echo
    
    // 3. private context space
    uint8_t private[NOCLI_PRIVATE_CONTEXT_SIZE];
};

// Initialize context.
// Must be called before calling `Nocli_Feed`. Will reset any context state and print prompt.
enum NocliErrors Nocli_Init(struct Nocli *nocli);

// Feed data in.
// Passed memory may be modified by nocli, and must remain valid until this function returns.
enum NocliErrors Nocli_Feed(struct Nocli *nocli, char *input, size_t length);
