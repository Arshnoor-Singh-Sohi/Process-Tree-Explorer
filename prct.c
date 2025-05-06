#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h> // For kill() function
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


// Function to check if process exists or not
int does_process_exist(int pid)
{
    // First, we will make a string to store the file path
    char file_path[100]; // Make it big enough to hold the path

    // Creating the path string like "/proc/1234/status"
    sprintf(file_path, "/proc/%d/status", pid);

    // Try to open the file
    FILE *file = fopen(file_path, "r"); // "r" means read mode
    if (file == NULL)
    {
        return 0; // File didn't open, so process doesn't exist
    }

    // If we got here, file opened okay
    fclose(file); // close file after reading

    // Process exists
    return 1;
}

// Function to get pid of the parent process
int get_parent_pid(int pid)
{
    // Make a string to store file path
    char file_path[100];

    // Make a string to store each line we read
    char line[256];

    // Create the path to status file
    sprintf(file_path, "/proc/%d/status", pid);

    // Try to open the file
    FILE *file = fopen(file_path, "r");

    // Check if file opened okay
    if (file == NULL)
    {
        printf("Cannot open status file for process %d\n", pid);
        return -1; // Return -1 if we couldn't open file
    }

    // Variable to store parent pid when we find it
    int ppid = -1;

    // Read file line by line
    while (fgets(line, sizeof(line), file))
    {
        // Check if this line starts with "PPid:"
        if (strncmp(line, "PPid:", 5) == 0)
        {
            // Found the line with parent PID
            // Convert the text after "PPid:" to a number
            sscanf(line, "PPid: %d", &ppid);
            break; // Exit the loop, we found what we needed
        }
    }

    // printf("PPid: %d\n", ppid);

    // Close the file
    fclose(file);

    // Return the parent pid we found
    return ppid;
}

int is_process_in_tree(int root_process, int process_id)
{
    // First check if both processes exist
    if (!does_process_exist(root_process) || !does_process_exist(process_id))
    {
        printf("One or both processes don't exist\n");
        return 0;
    }

    // Start with the process we want to check
    int current_pid = process_id;

    // Keep going up the tree until we either find root_process or reach init
    while (current_pid > 1)
    { // 1 is the init process
        // If we found the root_process, we're done!
        if (current_pid == root_process)
        {
            return 1;
        }

        // Get the parent of current process
        int parent_pid = get_parent_pid(current_pid);

        // If we couldn't get parent PID, something went wrong
        if (parent_pid == -1)
        {
            printf("Couldn't get parent for process %d\n", current_pid);
            return 0;
        }

        // Move up to the parent
        current_pid = parent_pid;
    }

    // If we got here, we reached init without finding root_process
    return 0;
}

// Helper function to verify if a process is root of its tree
int is_root_process(int pid)
{
    char status_path[256];
    sprintf(status_path, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL)
    {
        return 0;
    }

    char line[256];
    int ppid = -1;

    // Get parent PID
    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "PPid:", 5) == 0)
        {
            sscanf(line, "PPid: %d", &ppid);
            break;
        }
    }
    fclose(status_file);

    // Check if parent is a bash process
    char parent_cmdline[256];
    sprintf(parent_cmdline, "/proc/%d/cmdline", ppid);
    FILE *cmdline_file = fopen(parent_cmdline, "r");
    if (cmdline_file == NULL)
    {
        return 0;
    }

    char cmd[256];
    fgets(cmd, sizeof(cmd), cmdline_file);
    fclose(cmdline_file);

    // Check if parent is bash
    return (strstr(cmd, "bash") != NULL);
}

// function to print immediate descendants of a process
void list_immediate_descendants(int process_id)
{
    // Make space for storing the file path
    char children_path[256];

    // Create path to children file
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);

    // Try to open the file
    FILE *child_file = fopen(children_path, "r");

    // Check if we could open the file
    if (child_file == NULL)
    {
        printf("Could not open children file for process %d\n", process_id);
        return;
    }

    // Variable to store child pid
    int child_pid;
    int found_children = 0; // Flag to track if we found any children

    // Read child PIDs one by one
    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        // If this is first child found, print header
        if (!found_children)
        {
            printf("Immediate descendants of %d:\n", process_id);
            found_children = 1;
        }
        printf("%d\n", child_pid);
    }

    // If no children were found
    if (!found_children)
    {
        printf("No immediate descendants found for process %d\n", process_id);
    }

    // Close the file
    fclose(child_file);
}

void list_non_direct_descendants(int process_id)
{
    // First get immediate childrem
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        printf("No non-direct descendants found\n");
        return;
    }

    // Read immediate children first
    int immediate_child;
    int found_non_direct = 0; // Flag for non direct descendants

    // For each immediate child, find their children (which are non-direct for original process)
    while (fscanf(child_file, "%d", &immediate_child) > 0)
    {
        // Now check children for this immediate child
        char grandchild_path[256];
        sprintf(grandchild_path, "/proc/%d/task/%d/children", immediate_child, immediate_child);

        FILE *grandchild_file = fopen(grandchild_path, "r");
        if (grandchild_file != NULL)
        {
            int grandchild;
            while (fscanf(grandchild_file, "%d", &grandchild) > 0)
            {
                if (!found_non_direct)
                {
                    printf("Non-direct descendants of %d: \n", process_id);
                    found_non_direct = 1;
                }
                printf("%d\n", grandchild);
            }
            fclose(grandchild_file);
        }
    }
    fclose(child_file);
    if (!found_non_direct)
    {
        printf("No non-direct descendants found.\n");
    }
}

void list_siblings(int process_id)
{
    // First get the parent ID of our process
    int parent_pid = get_parent_pid(process_id);

    // If we couldn't get parent, return
    if (parent_pid == -1)
    {
        printf("Could not find parent of process %d\n", process_id);
        return;
    }

    // Now look for all children of this parent (these are siblings)
    char sibling_path[256];
    sprintf(sibling_path, "/proc/%d/task/%d/children", parent_pid, parent_pid);

    FILE *sibling_file = fopen(sibling_path, "r");
    if (sibling_file == NULL)
    {
        printf("No siblings found\n");
        return;
    }

    // Read all siblings
    int sibling_pid;
    int found_siblings = 0;

    while (fscanf(sibling_file, "%d", &sibling_pid) > 0)
    {
        // Don't list the process itself as its sibling
        if (sibling_pid != process_id)
        {
            // Print header only when first sibling is found
            if (!found_siblings)
            {
                printf("Siblings of process %d:\n", process_id);
                found_siblings = 1;
            }
            printf("%d\n", sibling_pid);
        }
    }

    // If no siblings were found (except itself)
    if (!found_siblings)
    {
        printf("No siblings found for process %d\n", process_id);
    }

    fclose(sibling_file);
}

void list_grandchildren(int process_id)
{
    // First get immediate children
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        printf("No grandchildren found (no children)\n");
        return;
    }

    int child_pid;
    int found_grandchildren = 0; // Flag to track if we found any grandchildren

    // For each child, find their children (our grandchildren)
    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        // Look for children of this child (grandchildren)
        char grandchild_path[256];
        sprintf(grandchild_path, "/proc/%d/task/%d/children", child_pid, child_pid);

        FILE *grandchild_file = fopen(grandchild_path, "r");
        if (grandchild_file != NULL)
        {
            int grandchild_pid;

            // Read each grandchild
            while (fscanf(grandchild_file, "%d", &grandchild_pid) > 0)
            {
                // Print header only for first grandchild
                if (!found_grandchildren)
                {
                    printf("Grandchildren of process %d:\n", process_id);
                    found_grandchildren = 1;
                }
                printf("%d\n", grandchild_pid);
            }
            fclose(grandchild_file);
        }
    }

    fclose(child_file);

    // If no grandchildren were found
    if (!found_grandchildren)
    {
        printf("No grandchildren found for process %d\n", process_id);
    }
}

void check_if_defunct(int process_id)
{
    // Make space for file path
    char status_path[256];
    sprintf(status_path, "/proc/%d/status", process_id);

    // Try to open status file
    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL)
    {
        printf("Cannot open status file for process %d\n", process_id);
        return;
    }

    // Read file line by line
    char line[256];
    int is_defunct = 0; // Flag to track if process is defunct

    while (fgets(line, sizeof(line), status_file))
    {
        // Look for line starting with "State:"
        if (strncmp(line, "State:", 6) == 0)
        {
            // Check if state contains 'Z' (zombie/defunct)
            if (strchr(line, 'Z') != NULL)
            {
                is_defunct = 1;
            }
            break; // Found state line, no need to read more
        }
    }

    // Print result
    if (is_defunct)
    {
        printf("Defunct\n");
    }
    else
    {
        printf("Not defunct\n");
    }

    // Close file
    fclose(status_file);
}

void list_defunct_siblings(int process_id)
{
    // First get the parent ID of our process
    int parent_pid = get_parent_pid(process_id);

    // If we couldn't get parent, return
    if (parent_pid == -1)
    {
        printf("Could not find parent of process %d\n", process_id);
        return;
    }

    // Now look for all children of this parent (these are siblings)
    char sibling_path[256];
    sprintf(sibling_path, "/proc/%d/task/%d/children", parent_pid, parent_pid);

    FILE *sibling_file = fopen(sibling_path, "r");
    if (sibling_file == NULL)
    {
        printf("No defunct siblings found\n");
        return;
    }

    // Read all siblings
    int sibling_pid;
    int found_defunct_siblings = 0;

    while (fscanf(sibling_file, "%d", &sibling_pid) > 0)
    {
        // Don't check the process itself
        if (sibling_pid != process_id)
        {
            // Check if this sibling is defunct
            char status_path[256];
            sprintf(status_path, "/proc/%d/status", sibling_pid);

            FILE *status_file = fopen(status_path, "r");
            if (status_file != NULL)
            {
                char line[256];
                while (fgets(line, sizeof(line), status_file))
                {
                    // Look for State line
                    if (strncmp(line, "State:", 6) == 0)
                    {
                        // Check if zombie
                        if (strchr(line, 'Z') != NULL)
                        {
                            // Print header only for first defunct sibling
                            if (!found_defunct_siblings)
                            {
                                printf("Defunct siblings of process %d:\n", process_id);
                                found_defunct_siblings = 1;
                            }
                            printf("%d\n", sibling_pid);
                        }
                        break; // Found state line, no need to read more
                    }
                }
                fclose(status_file);
            }
        }
    }

    // If no defunct siblings were found
    if (!found_defunct_siblings)
    {
        printf("No defunct siblings found for process %d\n", process_id);
    }

    fclose(sibling_file);
}

// Helper function to check if a process is defunct
int is_defunct(int pid)
{
    char status_path[256];
    sprintf(status_path, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL)
    {
        return 0;
    }

    char line[256];
    int is_defunct = 0;

    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
            if (strchr(line, 'Z') != NULL)
            {
                is_defunct = 1;
            }
            break;
        }
    }
    fclose(status_file);
    return is_defunct;
}

// Recursive function to check all descendants
void check_descendants_recursive(int pid, int *count, int print_pids)
{
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", pid, pid);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        return;
    }

    int child_pid;
    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        // Check if this child is defunct
        if (is_defunct(child_pid))
        {
            (*count)++; // Increment counter
            if (print_pids)
            { // Only print if -df option
                if (*count == 1)
                { // Print header only once
                    printf("Defunct descendants:\n");
                }
                printf("%d\n", child_pid);
            }
        }
        // Recursively check this child's descendants
        check_descendants_recursive(child_pid, count, print_pids);
    }

    fclose(child_file);
}

// Function for -df option
void list_defunct_descendants(int process_id)
{
    int count = 0;
    check_descendants_recursive(process_id, &count, 1); // 1 means print PIDs

    if (count == 0)
    {
        printf("No defunct descendants found for process %d\n", process_id);
    }
}

// Function for -dc option
void count_defunct_descendants(int process_id)
{
    // printf("[DEBUG] Entering count_defunct_descendants for PID: %d\n", process_id);
    int count = 0;
    check_descendants_recursive(process_id, &count, 0); // 0 means don't print PIDs
    printf("%d\n", count);                              // Print the count
    // printf("[DEBUG] Defunct descendant count for PID %d: %d\n", process_id, count);
}

void check_if_orphan(int process_id)
{
    // Get original parent PID
    int original_ppid = get_parent_pid(process_id);

    // printf("\nIn orphan checking\n");
    // printf("Got original parent PID: %d\n", original_ppid);

    // If parent is init (PID 1), process is orphan
    if (original_ppid == 1)
    {
        printf("Orphan\n");
        return;
    }

    // Try to open parent's status file
    char parent_path[256];
    sprintf(parent_path, "/proc/%d/status", original_ppid);

    FILE *parent_file = fopen(parent_path, "r");
    if (parent_file == NULL)
    {
        // Can't open parent's status file - parent doesn't exist
        printf("Orphan\n");
        return;
    }

    fclose(parent_file);
    printf("Not Orphan\n");
}

// Helper function to check if a process is orphan
int is_orphan(int pid)
{
    char status_path[256];
    sprintf(status_path, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL)
    {
        printf("Failed to open status file for PID %d\n", pid); // Debug print
        return 0;                                               // Process doesn't exist
    }

    char line[256];
    int ppid = -1;

    // Read the status file to find the parent PID (PPid)
    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "PPid:", 5) == 0)
        {
            sscanf(line, "PPid: %d", &ppid);
            break;
        }
    }
    fclose(status_file);

    printf("PID: %d, PPid: %d\n", pid, ppid); // Debug print

    // A process is orphaned if its parent PID is 1 (adopted by init)
    return (ppid == 1);
}

// Recursive function to list all orphan descendants
void list_orphan_descendants(int process_id)
{
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        printf("Failed to open children file for PID %d\n", process_id); // Debug print
        return;
    }

    int child_pid;

    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        printf("Traversing child PID: %d of parent PID: %d\n", child_pid, process_id); // Debug print

        // Recursively check descendants of this child
        list_orphan_descendants(child_pid);

        // Check if this child is an orphan
        if (is_orphan(child_pid))
        {
            printf("Orphaned descendant: %d\n", child_pid);
        }
    }

    fclose(child_file);
}

// Helper function to check if a process is zombie
int is_zombie(int pid)
{
    char status_path[256];
    sprintf(status_path, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL)
    {
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
            fclose(status_file);
            return (strchr(line, 'Z') != NULL); // Zombie state
        }
    }
    fclose(status_file);
    return 0;
}

// Helper function to get parent PID
int get_parent_pid_new(int pid)
{
    char status_path[256];
    sprintf(status_path, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL)
    {
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "PPid:", 5) == 0)
        {
            int ppid;
            sscanf(line, "PPid: %d", &ppid);
            fclose(status_file);
            return ppid;
        }
    }
    fclose(status_file);
    return -1;
}

// Recursive function to kill parents of all zombie descendants
void kill_parents_of_zombies(int process_id)
{
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        return;
    }

    int child_pid;
    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        // Recursively handle descendants of this child
        kill_parents_of_zombies(child_pid);

        // Check if this child is a zombie
        if (is_zombie(child_pid))
        {
            // Get the parent PID of the zombie process
            int parent_pid = get_parent_pid(child_pid);

            // Kill the parent of the zombie
            if (parent_pid > 1 && kill(parent_pid, SIGKILL) == -1)
            {
                perror("Failed to kill parent");
            }
            else
            {
                printf("Killed parent %d of zombie process %d\n", parent_pid, child_pid);

                // Wait for init to clean up the zombie
                sleep(1); // Short delay to allow cleanup

                // Verify if the zombie is removed
                if (!does_process_exist(child_pid))
                {
                    printf("Zombie process %d has been cleaned up\n", child_pid);
                }
                else
                {
                    printf("Zombie process %d still exists\n", child_pid);
                }
            }
        }
    }

    fclose(child_file);
}

// Recursive function to kill all descendants
void kill_all_descendants(int process_id)
{
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);
    
    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        return;
    }

    int child_pid;
    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        
        // Recursively kill descendants of this child
        kill_all_descendants(child_pid);

        // Check if child is a zombie
        if (is_zombie(child_pid))
        {
            printf("Zombie process detected: PID = %d (Cannot kill zombie directly using SIGKILL)\n", child_pid);
            // Get the parent PID of the zombie
            // int parent_pid = get_parent_pid_new(child_pid);
            // // Kill the parent of the zombie
            // if (parent_pid > 1 && kill(parent_pid, SIGKILL) == -1)
            // {
            //     perror("Failed to kill parent");
            // }
            // else
            // {
            //     printf("Killed parent %d of zombie process %d\n", parent_pid, child_pid);
            // }

        }
        else
        {
            // Kill non-zombie processes directly
            if (kill(child_pid, SIGKILL) == -1)
            {
                printf("Failed to kill process");
            }
            else
            {
                printf("Killed process: %d\n", child_pid);
            }
        }

        usleep(50000); // Small delay to ensure signal delivery
    }

    fclose(child_file);
}

// Recursive function to send SIGSTOP to all descendants
void stop_all_descendants(int process_id)
{
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        return;
    }

    int child_pid;
    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        // Recursively stop descendants of this child
        stop_all_descendants(child_pid);

        // Check if the process is a zombie
        char status_path[256];
        sprintf(status_path, "/proc/%d/status", child_pid);

        FILE *status_file = fopen(status_path, "r");
        if (status_file == NULL)
        {
            continue;
        }

        char line[256];
        int is_zombie = 0;
        while (fgets(line, sizeof(line), status_file))
        {
            if (strncmp(line, "State:", 6) == 0 && strchr(line, 'Z') != NULL)
            {
                is_zombie = 1;
                break;
            }
        }
        fclose(status_file);

        // Skip zombie processes
        if (is_zombie)
        {
            printf("Process %d is a zombie and cannot be stopped\n", child_pid);
            continue;
        }

        // Send SIGSTOP to non-zombie processes
        if (kill(child_pid, SIGSTOP) == -1)
        {
            perror("Failed to stop process");
        }
        else
        {
            printf("Stopped process: %d\n", child_pid);
        }
    }

    fclose(child_file);
}

// Recursive function to send SIGCONT to all paused descendants
void continue_all_paused_descendants(int process_id)
{
    char children_path[256];
    sprintf(children_path, "/proc/%d/task/%d/children", process_id, process_id);

    FILE *child_file = fopen(children_path, "r");
    if (child_file == NULL)
    {
        return;
    }

    int child_pid;
    while (fscanf(child_file, "%d", &child_pid) > 0)
    {
        // Recursively continue descendants of this child
        continue_all_paused_descendants(child_pid);

        // Check if the process is paused (T state)
        char status_path[256];
        sprintf(status_path, "/proc/%d/status", child_pid);

        FILE *status_file = fopen(status_path, "r");
        if (status_file == NULL)
        {
            continue;
        }

        char line[256];
        int is_paused = 0;
        while (fgets(line, sizeof(line), status_file))
        {
            if (strncmp(line, "State:", 6) == 0 && strchr(line, 'T') != NULL)
            {
                is_paused = 1;
                break;
            }
        }
        fclose(status_file);

        // Send SIGCONT to paused processes
        if (is_paused)
        {
            if (kill(child_pid, SIGCONT) == -1)
            {
                perror("Failed to continue process");
            }
            else
            {
                printf("Resumed process: %d\n", child_pid);
            }
        }
    }

    fclose(child_file);
}

// Function to kill a specific process and handle zombies
void kill_root_process(int root_process, int process_id)
{
    // Ensure the root process is valid
    if (!is_root_process(root_process))
    {
        printf("Error: %d is not a root process\n", root_process);
        return;
    }

    // Check if the process_id belongs to the tree rooted at root_process
    if (!is_process_in_tree(root_process, process_id))
    {
        printf("Error: Process %d does not belong to the tree rooted at %d\n", process_id, root_process);
        return;
    }

    // Check if the process is a zombie
    if (is_zombie(process_id))
    {
        // Get the parent PID of the zombie process
        int parent_pid = get_parent_pid(process_id);

        // Kill the parent of the zombie
        if (parent_pid > 1 && kill(parent_pid, SIGKILL) == -1)
        {
            perror("Failed to kill parent");
        }
        else
        {
            printf("Killed parent %d of zombie process %d\n", parent_pid, process_id);
        }

        // Zombie will be cleaned up by init after killing its parent
    }
    else
    {
        // Kill non-zombie processes directly
        if (kill(process_id, SIGKILL) == -1)
        {
            perror("Failed to kill process");
        }
        else
        {
            printf("Killed process: %d\n", process_id);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("ERROR:Number of arguments are less than required\n");
        exit(EXIT_FAILURE);
    }

    pid_t root_process = atoi(argv[1]);
    pid_t process_id = atoi(argv[2]);
    char *option = argc == 4 ? argv[3] : NULL;

    // Valid Inputs
    if (process_id <= 0 || root_process <= 0)
    {
        printf("ERROR:Process IDs should be a positive number\n");
        exit(EXIT_FAILURE);
    }

    // First check if process exists
    if (!does_process_exist(process_id))
    {
        printf("Process %d doesn't exist!\n", process_id);
        return EXIT_FAILURE;
    }

    // Special handling for -so option
    if (argc == 4 && strcmp(argv[3], "-so") == 0)
    {
        check_if_orphan(process_id);
        return EXIT_SUCCESS;
    }
    // If -op option is provided
    if (argc == 4 && strcmp(argv[3], "-op") == 0)
    {
        list_orphan_descendants(process_id);
    }

    // Now check tree membership for all other options
    if (!is_process_in_tree(root_process, process_id))
    {
        printf("Process %d does not belong to the tree rooted at %d\n", process_id, root_process);
        // printf("Orphan\n");
        return EXIT_FAILURE;
    }

    if (!is_root_process(root_process))
    {
        printf("Process %d is not the root process\n", process_id);
        return EXIT_FAILURE;
    }

    // If no option provided (argc == 3), print PID and PPID
    if (argc == 3)
    {
        printf("Process %d exists!\n", process_id);
        printf("PPid: %d\n", get_parent_pid(process_id));
        printf("PID %d belongs to tree rooted at %d\n", process_id, root_process);
        return EXIT_SUCCESS;
    }

    // If -id option is provided
    if (argc == 4 && strcmp(argv[3], "-id") == 0)
    {
        list_immediate_descendants(process_id);
    }

    // If -ds option is provided
    if (argc == 4 && strcmp(argv[3], "-ds") == 0)
    {
        list_non_direct_descendants(process_id);
    }

    // If -lg option is provided
    if (argc == 4 && strcmp(argv[3], "-lg") == 0)
    {
        list_siblings(process_id);
    }

    // If -lg option is provided
    if (argc == 4 && strcmp(argv[3], "-gc") == 0)
    {
        list_grandchildren(process_id);
    }

    // If -do option is provided
    if (argc == 4 && strcmp(argv[3], "-do") == 0)
    {
        check_if_defunct(process_id);
    }

    // If -lz option is provided
    if (argc == 4 && strcmp(argv[3], "-lz") == 0)
    {
        list_defunct_siblings(process_id);
    }

    // If -df option is provided
    if (argc == 4 && strcmp(argv[3], "-df") == 0)
    {
        list_defunct_descendants(process_id);
    }

    // If -dc option is provided
    if (argc == 4 && strcmp(argv[3], "-dc") == 0)
    {
        // printf("[DEBUG] Executing -dc option\n");
        count_defunct_descendants(atoi(argv[2]));
    }

    // If -so option is provided
    if (argc == 4 && strcmp(argv[3], "-so") == 0)
    {
        check_if_orphan(process_id);
    }

    // If -op option is provided
    if (argc == 4 && strcmp(argv[3], "-op") == 0)
    {
        list_orphan_descendants(process_id);
    }

    // If -pz option is provided
    if (argc == 4 && strcmp(argv[3], "--pz") == 0)
    {
        kill_parents_of_zombies(process_id);
    }

    // If -sk option is provided
    if (argc == 4 && strcmp(argv[3], "-sk") == 0)
    {
        if (!is_root_process(root_process))
        {
            printf("Error: %d is not a root process\n", root_process);
            return EXIT_FAILURE;
        }

        // if (process_id != root_process) {
        //     printf("Only root process can kill all descendants\n");
        //     return EXIT_FAILURE;
        // }

        kill_all_descendants(process_id);
    }

    // If -st option is provide
    if (argc == 4 && strcmp(argv[3], "-st") == 0)
    {
        if (!is_root_process(root_process))
        {
            printf("Error: %d is not a root process\n", root_process);
            return EXIT_FAILURE;
        }

        // if (process_id != root_process) {
        //     printf("Only root process can send SIGSTOP to all descendants\n");
        //     return EXIT_FAILURE;
        // }

        stop_all_descendants(process_id);
    }

    // If -sc option is provide
    if (argc == 4 && strcmp(argv[3], "-dt") == 0)
    {
        if (!is_root_process(root_process))
        {
            printf("Error: %d is not a root process\n", root_process);
            return EXIT_FAILURE;
        }

        // if (process_id != root_process) {
        //     printf("Only root process can send SIGCONT to all paused descendants\n");
        //     return EXIT_FAILURE;
        // }

        continue_all_paused_descendants(process_id);
    }

    // If -rp option is provide
    if (argc == 4 && strcmp(argv[3], "-rp") == 0)
    {
        
        if (process_id != root_process) {
            printf("Only root process have to be killed using -rp\n");
            return EXIT_FAILURE;
        }
        kill_root_process(root_process, root_process);
    }

    return 0;
}
