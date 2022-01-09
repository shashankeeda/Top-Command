/**
 * @file
 *
 * Retrieves raw information from procfs, parses it, and provides easy-to-use
 * functions for displaying the data.
 */

#ifndef _PROCFS_H_
#define _PROCFS_H_

#include <sys/types.h>

/**
 *This is a struct load_avg that has three paramters to store load avg
 *
 *
 *
 *
 */
struct load_avg {
    double one;
    double five;
    double fifteen;
};

/**
 *This is a struct cpu_stats that has 2 parameters to calulate cpu usage
 *
 *
 *
 *
 */
struct cpu_stats {
    long idle;
    long total;
};

/**
 *This is a struct mem_stats which has two parameters to calculate memory usage
 *
 *
 *
 */
struct mem_stats {
    double used;
    double total;
};

/**
 *This is a struct task_stats which has 7 parameters to calculate number of tasks needed to be finished
 *
 *
 *
 */
struct task_stats {
    unsigned int total;
    unsigned int running;
    unsigned int waiting;
    unsigned int sleeping;
    unsigned int stopped;
    unsigned int zombie;

    struct task_info *active_tasks;
};

/**
 *This is a struct task_info which has 4 parameters stored to get info about active tasks
 *
 *
 */
struct task_info {
    pid_t pid;
    uid_t uid;
    char name[26];
    char state[13];
};

/**
 *Retrieves hostname of the machine
 *
 *@param proc_dir the location of procfs
 *@param hostname_buf where to store the hostname
 *@param buf_sz max size of the hostname_buf
 *
 *
 * @return 0 if it worked, return -1 on failure
 */
int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz);

/**
 *Retrives the kernel version being used
 *
 *@param proc_dir retrieves the locations of procfs
 *@param version_buf is where version is stores
 *@param buf_sz is max size needed to be read by version_buf
 *
 *
 *@return 0 on success and -1 on failure
 */
int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz);

/**
 *Retrives the cpu model being used
 *
 *@param proc_dir retrieves the locations of procfs
 *@param model_buf is where the version is stored
 *@param buf_sz is max size needed to be read by model_buf
 *
 *
 *@return 0 on success and -1 on failure
 */
int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz);

/**
 *Calculates number of cpu units there are
 *
 *@param proc_dir retrieves the location of procfs
 *
 *
 *@return number of cpu units on success and -1 on failure
 *
 *
 */
int pfs_cpu_units(char *proc_dir);

/**
 *Calculates the uptime in seconds
 *
 *@param proc_dir retrieves the location of procfs
 *
 *@return the uptime in seconds on success and -1 on failure
 *
 *
 */
double pfs_uptime(char *proc_dir);

/**
 *
 * Converts the uptime in seconds given to days, hours, minutes, and seconds
 *@param time is the time in seconds
 *@param uptime_buf is the char we're modifying by converting time into days, hours, minutes, and seconds
 *
 *@return 0 on success
 *
 */
int pfs_format_uptime(double time, char *uptime_buf);

/**
 *Returns the load average in the form of a struct
 *
 *@param proc_dir retrieves the location of procfs
 *
 *@return struct on success and struct on failure as well
 *
 */
struct load_avg pfs_load_avg(char *proc_dir);
/* Note: 'prev' is an input that represents the *last* CPU usage state. 'curr'
 * is an *output* that represents the current CPU usage state, and will be
 * passed back in to pfs_load_avg on its next call. */

/**
 *Returns the cpu usage by taking in two samples and getting the average
 *
 *@param proc_dir retrieves the location of proc
 *@param struct cpu_stats *prev is the first sample of cpu usage
 *@param struct cpu_stats *curr is the second sample of cpu usage
 *
 *@return cpu usage on success and returns -1 for any  failure
 *
 *
 */
double pfs_cpu_usage(
        char *procfs_dir, struct cpu_stats *prev, struct cpu_stats *curr);

/**
 *Returns the memory usage in form a struct
 *
 *@param proc_dir retrieves the location of the proc directory
 *
 *@return completed struct on success and empty struct on failure
 */
struct mem_stats pfs_mem_usage(char *procfs_dir);

/* Note: these two functions create and destroy task_stats structs. Depending on
 * your implementation, this might just be a malloc/free. */

/**
 *
 *Initializes a struct task_stats and its parameters as well
 *
 *@return completed struct on success and empty struct on failure
 *
 */
struct task_stats *pfs_create_tstats();

/**
 *
 *Frees the memory located within the struct
 *
 *@param tstats will be freed within the method
 */
void pfs_destroy_tstats(struct task_stats *tstats);

/**
 *Calculates the number of tasks and types of tasks there are
 *
 *@param char *proc_dir retrieves the loacation of the proc directory
 *@param struct task_stats *tstats is the struct we use to store all the information in
 *
 *@return number of tasks on success and 1 as failure
 *
 *
 */
int pfs_tasks(char *proc_dir, struct task_stats *tstats);

#endif
