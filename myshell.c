#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_STRINGS 100
#define MAX_STRING_LENGTH 100

typedef struct
{
    char data[MAX_STRINGS][MAX_STRING_LENGTH]; // 2D array for storing strings
    int front;                                 // Index of the front element
    int rear;                                  // Index of the last element
    int count;                                 // Number of items in the queue
} StringQueue;

void initializeQueue(StringQueue *q)
{
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isFull(StringQueue *q)
{
    return q->count == MAX_STRINGS;
}

int isEmpty(StringQueue *q)
{
    return q->count == 0;
}

void enqueue(StringQueue *q, const char *str)
{
    if (isFull(q))
    {
        // If the queue is full, we increment the front index (FIFO logic)
        q->front = (q->front + 1) % MAX_STRINGS;
    }
    else
    {
        q->count++;
    }
    // Move rear to the next spot, which could be the start if it's circular
    q->rear = (q->rear + 1) % MAX_STRINGS;
    strncpy(q->data[q->rear], str, MAX_STRING_LENGTH);
    q->data[q->rear][MAX_STRING_LENGTH - 1] = '\0'; // Ensure null termination
}

void dequeue(StringQueue *q, char *str)
{
    if (!isEmpty(q))
    {
        strncpy(str, q->data[q->front], MAX_STRING_LENGTH);
        q->front = (q->front + 1) % MAX_STRINGS;
        q->count--;
    }
    else
    {
        str[0] = '\0'; // Return an empty string if the queue is empty
    }
}

void printQueue(StringQueue *q)
{
    int i = q->front;
    int items = q->count;

    while (items > 0)
    {
        printf("%s\n", q->data[i]);
        i = (i + 1) % MAX_STRINGS;
        items--;
    }
}

void get_args(char ***splittedCommand, char *cmd)
{
    int spaceCount = 0;
    // Count the number of spaces to estimate the number of tokens
    for (int i = 0; cmd[i]; i++)
    {
        if (cmd[i] == ' ')
            spaceCount++;
    }

    // Allocate memory for pointers; add 1 for potential last token + 1 for NULL
    *splittedCommand = malloc((spaceCount + 2) * sizeof(char *));
    if (*splittedCommand == NULL)
    {
        perror("malloc failed");
        return;
    }

    char *token = strtok(cmd, " ");
    int i = 0;
    while (token != NULL)
    {
        (*splittedCommand)[i++] = token;
        token = strtok(NULL, " ");
    }
    (*splittedCommand)[i] = NULL; // Null-terminate the array of strings
}

int change_directory(char *path)
{
    if (chdir(path) != 0)
    {
        perror("chdir failed");
        return 1;
    }
    return 0;
}

int print_working_directory()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("pwd failed");
        exit(1);
    }
    printf("%s\n", cwd);
    return 0;
}

void execute_linux_cmd(char **args)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) < 0)
        {
            perror("execvp failed");
            exit(1);
        }
        exit(0);
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

int process_cmd(char *cmd, StringQueue *commands)
{
    char *cpyCmd = strcpy(malloc(strlen(cmd) + 1), cmd);
    char **args;
    get_args(&args, cpyCmd);
    if (args[0] == NULL)
    {
        return 1;
    }
    if (strcmp(args[0], "exit") == 0)
    {
        return 0;
    }
    else if (strcmp(args[0], "history") == 0)
    {
        enqueue(commands, cmd);
        printQueue(commands);
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        enqueue(commands, cmd);
        change_directory(args[1]);
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        enqueue(commands, args[0]);
        print_working_directory();
    }
    else
    {
        enqueue(commands, cmd);
        execute_linux_cmd(args);
    }
    return 1;
}

void add_to_path(int argc, char **argv)
{
    char *path = getenv("PATH");
    if (path == NULL)
    {
        perror("Failed to get PATH environment variable");
        return;
    }
    for (int i = 1; i < argc; i++)
    {
        char *new_path = argv[i];
        // Allocate memory for the new PATH environment variable (old_path:new_path\0
        size_t path_len = strlen(path) + strlen(new_path) + 2; // +2 for ':' and '\0'
        char *new_path_env = malloc(path_len);
        snprintf(new_path_env, path_len, "%s:%s", path, new_path);
        if (setenv("PATH", new_path_env, 1) != 0)
        {
            perror("Failed to set PATH environment variable");
        }
        free(new_path_env);
    }
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        add_to_path(argc, argv);
    }
    char input[100];
    StringQueue commands;
    initializeQueue(&commands);
    int res = 1;

    while (res)
    {
        printf("$ ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("fgets failed");
            exit(1);
        }
        input[strcspn(input, "\n")] = 0; // Remove trailing newline
        res = process_cmd(input, &commands);
    }

    return 0;
}
