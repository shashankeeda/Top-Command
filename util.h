#ifndef _UTIL_H_
#define _UTIL_H_

#include <sys/types.h>

/**
 *Retrives a path to the file and stores it in an int
 *
 *@param char *proc_dir retrives a path to the proc directory
 *@param char *path is a path that will be fused with proc_dir to get data
 *
 *@return the int with the path to the file on success and -1 on failure
 */
int open_path(char *proc_dir, char *path);
/**
 *Reads one line in a file each time called
 *@param int fd opens is the int with the file stored inside
 *@param char *buf is the char pointer that is being read into
 *@param size_t sz is the maximum number of bytes being read
 *
 *@return ssize_t number of bytes read on success and -1 on failure
 *
 */
ssize_t lineread(int fd, char *buf, size_t sz);
/**
 *Prints out the cpu usage showing bars in char form
 *
 *@param char *buf is the paramter where we store our final answer
 *@param double frac is the number we will use to convert to cpu usage by converting to percentage
 *
 */
void draw_percbar(char *buf, double frac);

/**
 *Converts the uid to uname
 *@param char *name_buf is the parameter where we store our final answer
 *@param uid_t uid is the uid which we are converting to store in name_buf
 *
 *
 */
void uid_to_uname(char *name_buf, uid_t uid);

#endif
