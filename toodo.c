/* 
    Date: June 2, 2023 
    Author: Bani Somo 
*/

/*
    TODO:
    - fix edge cases
        - look for the others that I am certainly missing
        - errors involving the json file (doesn't exist, messed up format, etc)
    - deal with unexpected user input
    - look over method of dealing with trailing comma inconsistencies
    - make the print list function look better
    - ability to change file path
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define DESC_LEN 50
#define MAX_TASKS 25
#define CHOOSE_LATER 0 // argument to function if user did not include program parameter

const char* FILE_NAME = "/home/bani/code/c/toodo_app/tasks.json";
const char* DATA_FORMAT_LINE_OUT = "\t{\n\t\t\"Completed\": %d,\n\t\t\"Description\": \"%s\"\n\t},\n";
const char* DATA_FORMAT_LINE_IN = "\t{\n\t\t\"Completed\": %d,\n\t\t\"Description\": \"%[^\"]\"\n\t}";

typedef struct Task {
    char description[DESC_LEN + 1];
    bool completed;
} Task;

typedef struct Task_List {
    Task list[MAX_TASKS]; // maybe use a different data structure
    int num_tasks;
} Task_List;

// Function Prototypes
// user operations
void help();
void add_task(Task_List *tasks, const char *new_task_msg);
void check_task(Task_List *tasks, int check_task_index);
void uncheck_task(Task_List *tasks, int uncheck_task_index);
void remove_task(Task_List *tasks, int removal_index);
void clear_list(Task_List *tasks);
void print_tasks(Task_List tasks);
void swap(Task_List *tasks, int task1_number, int task2_number);

// program operations
int file_size(FILE* fp);
void restore_data(FILE* fp, Task_List *tasks);
void save_data(Task_List tasks);

int main(int argc, const char *argv[]) {
    FILE* fp;
    Task_List tasks;
    tasks.num_tasks = 0;

    fp = fopen(FILE_NAME, "r");
    if (!fp) {
        printf("Failed to open %s file.\n\n", FILE_NAME);
        return 1; 
    }

    if (file_size(fp) != 0) {
        restore_data(fp, &tasks);
    }
    fclose(fp);

    // check which command is being executed
    if (argc == 1) {
        printf("No arguments provided. Try \"toodo -h\" for help.\n\n");
        return 1;

    } else if (strcmp(argv[1], "list") == 0) {
        print_tasks(tasks);
        return 0;
        
    } else if (strcmp("-h", argv[1]) == 0) {
        help();
        return 0;

    } else if (strcmp("add", argv[1]) == 0) {
        if (argc > 3) {
            printf("Too many arguments. Note that the task description must be in \"double quotes\".\n");
            return 1;
        } else if (argc < 3) {
            printf("Error: missing argument.\n");
            return 1;
        }
        add_task(&tasks, argv[2]);

    } else if (strcmp("check", argv[1]) == 0) {
        if (argc >= 3) {
            check_task(&tasks, atoi(argv[2]));
        }
        else {
            check_task(&tasks, CHOOSE_LATER);
        }

    } else if (strcmp("uncheck", argv[1]) == 0) {
        if (argc >= 3) {
            uncheck_task(&tasks, atoi(argv[2]));
        }
        else {
            uncheck_task(&tasks, CHOOSE_LATER);
        }

    } else if (strcmp("remove", argv[1]) == 0) {
        if (argc >= 3) {
            remove_task(&tasks, atoi(argv[2]));
        }
        else {
            remove_task(&tasks, CHOOSE_LATER);
        }

    } else if (strcmp("clear", argv[1]) == 0) {
        clear_list(&tasks);

    } else if (strcmp("swap", argv[1]) == 0) {
        if (argc >= 4)
            swap(&tasks, atoi(argv[2]), atoi(argv[3]));
        else if (argc == 3)
            swap(&tasks, atoi(argv[2]), CHOOSE_LATER);
        else
            swap(&tasks, CHOOSE_LATER, CHOOSE_LATER);

    } else {
        printf("Invalid arguments. Try \"todo -h\" for help.\n\n");
        return 1;
    }

    save_data(tasks);

    return 0;
}

// Function Definitions
// user operations
void help() {
    printf("Toodo is a todo list command line tool. These are the supported operations:\n\n");
    printf("* list                        - prints your list.\n");
    printf("* add \"some task description\" - adds a task to your list with the given description. Be sure to wrap the description in double quotes.\n");
    printf("* check <n>                   - marks the n'th task on your list as completed.\n");
    printf("* uncheck <n>                 - unmarks the n'th task on your list as completed.\n");
    printf("* remove <n>                  - removes the n'th task on your list.\n");
    printf("* clear                       - clears all tasks on your list.\n");
    printf("* swap <n> <m>                - swaps the n'th and m'th tasks on your list.\n");
    printf("\n");
}

void add_task(Task_List *tasks, const char* new_task_msg) {
    if (strlen(new_task_msg) > DESC_LEN) {
        printf("Your task description is too long. Task was not added.\n\n");
        exit(EXIT_FAILURE);
    }

    if (tasks->num_tasks >= MAX_TASKS) {
        printf("Your list is full. Try getting something done.\n\n");
        exit(EXIT_FAILURE);
    }

    Task new_task;
    strncpy(new_task.description, new_task_msg, DESC_LEN);
    new_task.completed = false;
    tasks->list[tasks->num_tasks++] = new_task;
}

void check_task(Task_List *tasks, int check_task_index) {
    if (tasks->num_tasks == 0) {
        printf("You have no tasks on you list to check off.\n\n");
        exit(EXIT_FAILURE);
    }

    if (check_task_index < 0 || check_task_index > tasks->num_tasks) {
        printf("Please enter a number between 1 and %d.\n\n", tasks->num_tasks);
        exit(EXIT_FAILURE);
    }

    if (check_task_index == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the number of the task you have completed (enter 0 to cancel): ");
        scanf("%d", &check_task_index);
        while (check_task_index < 0 || check_task_index > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d, or 0 to cancel: ", tasks->num_tasks);
            scanf("%d", &check_task_index);
        }
        if (check_task_index == 0)
            return;
    }

    tasks->list[check_task_index - 1].completed = true;
}

void uncheck_task(Task_List *tasks, int uncheck_task_index) {
    if (tasks->num_tasks == 0) {
        printf("You have no tasks on you list to uncheck.\n\n");
        exit(EXIT_FAILURE);
    }

    if (uncheck_task_index < 0 || uncheck_task_index > tasks->num_tasks) {
        printf("Please enter a number between 1 and %d.\n\n", tasks->num_tasks);
        exit(EXIT_FAILURE);
    }

    if (uncheck_task_index == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the number of the task you want to uncheck (enter 0 to cancel): ");
        scanf("%d", &uncheck_task_index);
        while (uncheck_task_index < 0 || uncheck_task_index > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d, or 0 to cancel: ", tasks->num_tasks);
            scanf("%d", &uncheck_task_index);
        }
        if (uncheck_task_index == 0)
            return;
    }

    tasks->list[uncheck_task_index - 1].completed = false;
}

void remove_task(Task_List *tasks, int removal_index) {
    if (tasks->num_tasks == 0) {
        printf("You have no tasks on your list to remove.\n\n");
        exit(EXIT_FAILURE);
    }

    if (removal_index < 0 || removal_index > tasks->num_tasks) {
        printf("Please enter a number between 1 and %d.\n\n", tasks->num_tasks);
        exit(EXIT_FAILURE);
    }

    if (removal_index == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the number of the task you would like to remove (enter 0 to cancel): ");
        scanf("%d", &removal_index);
        while (removal_index < 0 || removal_index > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d, or 0 to cancel: ", tasks->num_tasks);
            scanf("%d", &removal_index);
        }
        if (removal_index == 0)
            return;
    }

    for (int i = removal_index - 1; i < tasks->num_tasks - 1; i++) {
        // strcpy(tasks->list[i].description, tasks->list[i + 1].description);
        // tasks->list[i]
        tasks->list[i] = tasks->list[i + 1];
    }
    tasks->num_tasks--;
}

void clear_list(Task_List *tasks) {
    tasks->num_tasks = 0;
    fclose(fopen(FILE_NAME, "w"));
}

void print_tasks(Task_List tasks) {
    printf("###############\n");
    printf("#  TODO LIST  #\n");
    printf("###############\n");

    if (tasks.num_tasks == 0) {
        printf("\nNo current tasks!\n\n");
        return;
    }

    for(int i = 0; i < tasks.num_tasks; i++) {
        printf("(%d) %s %s\n", i + 1, (tasks.list[i].completed) ? "\u221A" : "\u25A1", tasks.list[i].description);
    }
    
    printf("\n");
}

void swap(Task_List *tasks, int task1_number, int task2_number) {
    if (tasks->num_tasks <= 1) {
        printf("You don't have enough tasks on you list to swap anything around.\n\n");
        exit(EXIT_FAILURE);
    }

    if (task1_number < 0 || task2_number < 0 || task1_number > tasks->num_tasks || task2_number > tasks->num_tasks) {
        printf("Please enter two numbers between 1 and %d.\n\n", tasks->num_tasks);
        exit(EXIT_FAILURE);
    }

    if (task1_number == CHOOSE_LATER && task2_number == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the numbers of the tasks you'd like to swap.\nFirst number: ");
        scanf("%d", &task1_number);
        while (task1_number <= 0 || task1_number > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d: ", tasks->num_tasks);
            scanf("%d", &task1_number);
        }

        printf("Second number: ");
        scanf("%d", &task2_number);
        while (task2_number <= 0 || task2_number > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d: ", tasks->num_tasks);
            scanf("%d", &task2_number);
        }

    } else if (task2_number == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the number of the task you'd like to swap with task %d: ", task1_number);
        scanf("%d", &task2_number);
        while (task2_number <= 0 || task2_number > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d: ", tasks->num_tasks);
            scanf("%d", &task2_number);
        }
    }

    // actual swap logic (；・∀・)
    Task temp = tasks->list[task1_number - 1];
    tasks->list[task1_number - 1] = tasks->list[task2_number - 1];
    tasks->list[task2_number - 1] = temp;
}

// program operations
int file_size(FILE* fp) {
    long file_size;

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return file_size;
}

void restore_data(FILE* fp, Task_List *tasks) {
    int i;
    fscanf(fp, "[\n\t%d,\n", &(tasks->num_tasks));

    for (i = 0; i < tasks->num_tasks - 1; i++) {
        fscanf(fp, DATA_FORMAT_LINE_IN, &tasks->list[i].completed, tasks->list[i].description);
        fscanf(fp, ",\n");
    }
    fscanf(fp, DATA_FORMAT_LINE_IN, &tasks->list[i].completed, tasks->list[i].description);
    fscanf(fp, "\n");
}

void save_data(Task_List tasks) {
    if (tasks.num_tasks == 0) {
        clear_list(&tasks);
        return;
    }

    FILE* fp = fopen(FILE_NAME, "w");

    fseek(fp, 0, SEEK_SET);

    fprintf(fp, "[\n\t%d,\n", tasks.num_tasks);

    for (int i = 0; i < tasks.num_tasks; i++) {
        fprintf(fp, DATA_FORMAT_LINE_OUT, tasks.list[i].completed, tasks.list[i].description);
    }

    // remove the trailing comma
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "\n");

    fprintf(fp, "]\n");

    fclose(fp);
    print_tasks(tasks);
}
