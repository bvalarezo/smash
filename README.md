# SMASH
**Smash** (*Small Shell*) is an **sh**-compatible command language interpreter that executes commands from standard input or from a file. **Smash**  is meant to be a small and lightweight shell capable of the most basic functionality.

## Makefile

Use the Makefile to perform different actions.

```bash
make
make clean
make test
```
To clean up all object files, dependencies, and binaries, run...
```bash
make clean
```
To compile **Smash**, run...
```bash
make
```
To run the test scripts, run...
```bash
make test
```
## File Structure
.
├── build
│   └── *.o
├── include
│   ├── cmd.h
│   ├── debug.h
│   ├── global.h
│   ├── jobs.h
│   ├── parser.h
│   ├── sig.h
│   ├── smash.h
│   └── wait.h
├── Makefile
├── README
├── smash
├── src
│   ├── cmd.c
│   ├── jobs.c
│   ├── main.c
│   ├── parser.c
│   ├── sig.c
│   ├── smash.c
│   └── wait.c
└── tests
	├── rdump
	│   ├── *.txt
	│   └── *.log
	├── rsrc
	│   ├── one.sh
	│   └── wait.c
    ├── test1.sh
    ├── test2.sh
    └── test3.sh

## Usage
### Name
Smash -  SMAll SHell
### Synopsis
**smash** [options] [file]
###  Options
**-d**
Output debugging messages on standard error. Debugging messsages include

 - When a new command gets executed.
 - When a command finishes execution(also outputting the exit code).
 - When the _$?_ variable gets updated.
 - On every smash function enter and leave.

**-h**
Display a usage message on standard output and exit successfully.
### Built-in User Commands
**Smash** will support the following built-in commands...

jobs - list all background jobs.

	jobs
bg   - brings the suspended foreground job to the background and resumes execution.

	bg <job number>
fg   - brings background job to the foreground and resumes execution if suspended.

	fg <job number>
	
kill   - send a signal to a job.

	kill -<signal> <job number>
cd   - changes the  working directory. If `DIRECTORY` is empty, then `cd` will change to the user's `HOME` directory.

	cd DIRECTORY
pwd   - print name of current/working directory.

	pwd
echo   - display a line of text or simple variables.

	echo [STRING] [VARIABLE]
exit   - exit the shell session.

	exit

### Job Control
The `jobs` command will output information such as...

 - Job number  
 - Process ID
 - Process status (Running/Stopped/Done/Terminated)
 - Exit code
 - Command string

Jobs can be sent to the background by appending the `&` character to the command string

	sleep 60 &


^C will interrupt the foreground job.

^Z will suspend the foreground job and bring the job to the background.

### Redirection

**Smash** supports redirection of standard input, standard output, and standard error.

For standard input, use the 	`<` character.

	sort -nr < file.txt

For standard output, use the `>` character.

	ls -la > a.txt
	
For standard error, use the `2>` characters.

	 ls -? 2> err.log
All 3 can be redirection in a single command

	test.bin < in.txt > out.txt 2> err.log 

### Exit Codes
Exit codes follow POSIX standards.

    1 - Catchall for general errors
    2 - Misuse of shell builtins
    126 - Command invoked cannot execute
    127 - “command not found”
    128 - Invalid argument to exit
    128+n - Fatal error signal “n”
    130 - Script terminated by ^-C
    255\* - Exit status out of range
    
### Environment Variables
The `echo` command supports printing environment variables

	echo $PATH $HOME
The `$?` shell variable will print out the exit code of the recently finished process.

	smash> echo $?
	0 

### Non-Interactive(Batch) Mode
**Smash** can read commands from a file and run them in non-interactively. 

This can be done by passing in the file as an argument to  **Smash**.

	./smash foo.sh
Or by making the file executable and putting the path to **Smash** in it's shebang line.

`#!/path/to/smash`

	./foo.sh

#### Comments
The `#` character denotes the following text as a comment, terminated by `\n`. All text in the comment is ignored

>Note: Comments are also supported in interactive mode.
## Implementation

In interactive mode, **Smash** will use `readline(3)` to read in input from the terminal. 

In non-interactive mode, **Smash** will use `getline(3)` to read a line from a file.

**Smash** parses each command line and creates an argument vector of the command(similar to the `argv` parameter from `main`). **Smash** seperates each argument by whitespace as well as taking in account tokens, such as `&`, `<` , `>`,  `2>`, and `#`.  Smash then records this command in an `argument` struct, defined here.
```C
struct  argument
{
	char *line;
	int argc;
	char **argv;
	int fd_stdin, fd_stdout, fd_stderr;
	unsigned int background : 1;
};
```
Redirection is done by parsing the command string for the  `<` , `>`,  and `2>` tokens. It will open the file based on the provided pathname, record the file descriptor in the argument structure,  and use `dup2(2)` to duplicate the file descriptors and redirect input/output/error.
 
`dup2(fd_stdin, STDIN_FILENO)`
`dup2(fd_stdout, STDOUT_FILENO)`
`dup2(fd_stderr, STDERR_FILENO)`

Details can be found in `parser.c` and `smash.c`.   

>Note: Built-in commands do not use `dup2(2)` but instead use `dprintf(3)` to redirect standard output or standard error. Standard input will be duplicated, but as of now no built-in command supports it.

**Smash** will read `argv[0]` from this structure as the command the user wishes to execute. If the command is a built-in, **Smash**  will foward this structure to those functions (found in `cmd.c`). Otherwise,  **Smash** will call `fork(2)` to create a child process and have the child call `execvp(3)`  to execute the command.

The parent process creates a job record of the child process. This job is then added to a data structure(Linked List) for record keeping.  Jobs are defined by these structs. 

```C
struct job
{
	struct argument *arg;
	pid_t pid;
	unsigned int process_status : 2;
	int exit_code;
};

struct job_node
{
	unsigned int job_id;
	struct job data;
	struct job_node  *next;
	struct job_node  *prev;
};
```
The parent process will hang execution of the shell and  wait for the job to stop/finish via `waitpid(2)`
```C
int wait_job(struct job_node *j, int options)
{
    int status = 0;
    pid_t wpid = 0;
    do
    {
        /* wait for the job to reply */
        wpid = waitpid(j->data.pid, &status, options);

    } while ((!update_job_status(wpid, status) || is_status(j, PROCESS_RUNNING)) && options != WNOHANG);
    return EXIT_SUCCESS;
}
```
If the command had the `&` token set, then the parent process will not hang the shell and proceed as  normal. The `&` token activates the background bit.

^Z will also send a job to the background and mark the background bit.

Background jobs are handled in an asynchronus manner by overwriting the signal handler for `SIGCHLD` using `sigaction(2)`.  **Smash**'s handler will call `waitpid(2)` to reap the child process and return the resources back to the operating system.

```C
void sigchld_handler(int signal, siginfo_t *info, void *ucontext)
{
    pid_t wpid = 0;
    int status = 0;
    while ((wpid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (update_job_status(wpid, status))
            break;
    }
}
```
This handler will properly reap the finished job (preventing it from becoming a zombie), updates the job's process status (*Done* or *Terminated*), and updates the exit code of the job and of `$?`.

A job labeled *Done* means that the process exited normally.

A job labeled *Terminated* means that the process was interrupted by a signal.

More details can be found in `smash.c`, `sig.c`, `job.c`, and `wait.c`.

## Resources
Smash's implementation was inspired by these sources.

https://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html#Implementing-a-Shell
https://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html

## Contributing
Maintained by Bryan Valarezo

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
