/* 
    Date: June 2, 2023 
    Author: Bani Somo 
*/

/*
    TODO:
    - fix edge cases
        - remove last task
        - "please enter a number between 1 and 0"
        - calling save when list is empty
        - swapping when only 1 task
        - look for the others that I am certainly missing
        - errors involving the json file (doesn't exist, messed up format, etc)
    - look over method of dealing with trailing comma inconsistencies
    - toodo list -u & -a, strikethru text
    - help function
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

const char* FILE_NAME = "tasks.json";
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
void check(Task_List *tasks, int task_number);
void uncheck(Task_List *tasks, int task_number);
void remove_task(Task_List *tasks, int task_number);
void clear(Task_List *tasks, FILE* fp);
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
        printf("Failed to open %s file.\n", FILE_NAME);
        return 1; 
    }

    if (file_size(fp) != 0) {
        restore_data(fp, &tasks);
    }
    fclose(fp);

    // check which command is being executed
    if (argc == 1 || strcmp(argv[1], "list") == 0) {
        print_tasks(tasks);

    } else if (strcmp("-h", argv[1]) == 0) {
        help();

    } else if (strcmp("add", argv[1]) == 0) {
        if (argc > 3) {
            printf("Too many arguments. Note that the task description must be in \"double quotes\".\n");
            return 1;
        } else if (argc < 3) {
            printf("Error: missing argument.\n");
            return 1;
        }
        add_task(&tasks, argv[2]);
        save_data(tasks);

    } else if (strcmp("check", argv[1]) == 0) {
        if (argc >= 3) {
            check(&tasks, atoi(argv[2]));
        }
        else {
            check(&tasks, CHOOSE_LATER);
        }
        save_data(tasks);

    } else if (strcmp("uncheck", argv[1]) == 0) {
        if (argc >= 3) {
            uncheck(&tasks, atoi(argv[2]));
        }
        else {
            uncheck(&tasks, CHOOSE_LATER);
        }
        save_data(tasks);

    } else if (strcmp("remove", argv[1]) == 0) {
        if (argc >= 3) {
            remove_task(&tasks, atoi(argv[2]));
        }
        else {
            remove_task(&tasks, CHOOSE_LATER);
        }
        save_data(tasks);

    } else if (strcmp("clear", argv[1]) == 0) {
        clear(&tasks, fp);

    } else if (strcmp("swap", argv[1]) == 0) {
        if (argc >= 4)
            swap(&tasks, atoi(argv[2]), atoi(argv[3]));
        else if (argc == 3)
            swap(&tasks, atoi(argv[2]), CHOOSE_LATER);
        else
            swap(&tasks, CHOOSE_LATER, CHOOSE_LATER);
        save_data(tasks);

    } else {
        printf("Invalid arguments. Try \"todo -h\" for help.\n");
        return 1;
    }

    return 0;
}

// Function Definitions
// user operations
void help() {

}

void add_task(Task_List *tasks, const char* new_task_msg) {
    if (tasks->num_tasks >= MAX_TASKS) {
        printf("Your list is full. Try getting something done.\n");
        return;
    }

    Task new_task;
    strncpy(new_task.description, new_task_msg, DESC_LEN);
    new_task.completed = false;
    tasks->list[tasks->num_tasks++] = new_task;
}

void check(Task_List *tasks, int task_number) {
    if (task_number < 0 || task_number > tasks->num_tasks) {
        printf("Please enter a number between 1 and %d.\n\n", tasks->num_tasks);
        return;
    }

    if (task_number == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the number of the task you have completed (enter 0 to cancel): ");
        scanf("%d", &task_number);
        while (task_number < 0 || task_number > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d, or 0 to cancel: ", tasks->num_tasks);
            scanf("%d", &task_number);
        }
        if (task_number == 0)
            return;
    }

    tasks->list[task_number - 1].completed = true;
}

void uncheck(Task_List *tasks, int task_number) {
    if (task_number < 0 || task_number > tasks->num_tasks) {
        printf("Please enter a number between 1 and %d.\n\n", tasks->num_tasks);
        return;
    }

    if (task_number == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the number of the task you want to uncheck (enter 0 to cancel): ");
        scanf("%d", &task_number);
        while (task_number < 0 || task_number > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d, or 0 to cancel: ", tasks->num_tasks);
            scanf("%d", &task_number);
        }
        if (task_number == 0)
            return;
    }

    tasks->list[task_number - 1].completed = false;
}

void remove_task(Task_List *tasks, int task_number) {
    if (task_number < 0 || task_number > tasks->num_tasks) {
        printf("Please enter a number between 1 and %d.\n\n", tasks->num_tasks);
        return;
    }

    if (task_number == CHOOSE_LATER) {
        print_tasks(*tasks);

        printf("Enter the number of the task you would like to remove (enter 0 to cancel): ");
        scanf("%d", &task_number);
        while (task_number < 0 || task_number > tasks->num_tasks) {
            printf("Please enter a number between 1 and %d, or 0 to cancel: ", tasks->num_tasks);
            scanf("%d", &task_number);
        }
        if (task_number == 0)
            return;
    }

    for (int i = task_number - 1; i < tasks->num_tasks - 1; i++) {
        strcpy(tasks->list[i].description, tasks->list[i + 1].description);
    }
    tasks->num_tasks--;
}

void clear(Task_List *tasks, FILE* fp) {
    tasks->num_tasks = 0;

    fopen(FILE_NAME, "w");
    fclose(fp);
}

void print_tasks(Task_List tasks) {
    // make it look better
    for(int i = 0; i < tasks.num_tasks; i++)
        printf("%d: %s\n", i + 1, tasks.list[i].description);
    
    printf("\n");
}

void swap(Task_List *tasks, int task1_number, int task2_number) {
    if (task1_number < 0 || task2_number < 0 || task1_number > tasks->num_tasks || task2_number > tasks->num_tasks) {
        printf("Please enter two numbers between 1 and %d.\n\n", tasks->num_tasks);
        return;
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
}