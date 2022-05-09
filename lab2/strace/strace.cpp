#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sched.h>

#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/syscall.h>

#include <sys/reg.h>
#include <stdio.h>

#include <signal.h>

#include <ctype.h>

#include <map>

using namespace std;

char bufstr[4096];

int main(int argc, char *argv[])
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        exit(1);
    case 0:
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execvp(argv[1], argv + 1);
        exit(1);
    }
    waitpid(pid, 0, 0);
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
    ptrace(PTRACE_SYSCALL, pid, 0, 0);
    waitpid(pid, 0, 0);
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    long syscall = regs.orig_rax;
    fprintf(stderr, "%ld(%ld, %ld, %ld, %ld, %ld, %ld)", syscall,
            (long)regs.rdi, (long)regs.rsi, (long)regs.rdx,
            (long)regs.r10, (long)regs.r8, (long)regs.r9);
    ptrace(PTRACE_SYSCALL, pid, 0, 0);
    waitpid(pid, 0, 0);
}