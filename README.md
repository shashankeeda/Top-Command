# Project 1: System Inspector
About This Project:
In this project we are creating a utility that goes through the data on our system and then prints it out to us, basically we are creating
a very similar function to the top command. We are making a lot of system calls through methods that we implement to read data being given to us in the proc files and then process them to print out the information that we want it. The main program we are running is the inspector file which will output something similar to what the top command outputs assuming that the methods in procfs.c and util.c are implementedcorrectly. 

How The Program Work:
The main program inspector will basically be running the main methods that give us our desired output which will use the methods from display.c which will use methods from procfs.c and util.c. The method pfs_hostname will go to one of the proc directories ending in hostname whcih will return the first line which will give us our hostname. The method pfs_kernel_version will go to the version directory and store the version number one there into a buffer anf will return 0 on success and -1 on failure. The pfs_cpu_model method will go into the cpuinfo directory which will store the cpu model into a buffer and will return the appropriate value depending on success. The pfs_cpu_units will also go into the cpuinfo directory and return the number of processors there are by going through every line and will return -1 on failure. The pfs_uptime function will go into the uptime directory and return the value given in seconds and then pfs_format_uptime will format that uptime we got into days, hours, minutes, and seconds. The pfs_load_avg function will then go into the loadavg directory which will return the first 3 values representing the loadavg and then store them in a struct which it will return regardless of failure. The pfs_cpu_usage function will then calculate the cpu usage by taking in two samples, the way it takes in two samples is because the function gets called twice by the display.c file and then the second time it is called it will return the cpu usage by using the data from the two samples. The pfs_mem_usage will go into the meminfo directory which will get the total memory available and the total memory that is being used and the store those two things in a struct and return them. The method pfs_create_tstats will accolacte memory for the struct and the variables within it and the the method pfs_destroy_tstats will just just free the memory allocated by pfs_create_tstats. The method pfs_tasks will then go throug every numbered directory in the /proc file and see how many of them are sleeping, running, zombie, stopped, and disk sleeped, it will then return the total number of processes there and store the active processes within another struct. We then have the draw_percbar function which will print the cpu bar based on the percentage of cpu being used and then we also have the uid_to_uname function which will go through all the uids returned by our pfs_tasks function and then see which uname it belongs to by storing it in the buffer given to us. All the functions which we wrote in procfs.c and util.c are used by display.c which organizes our methods usage propery and then returns values which are then used by the inspector file which will use the display methods to print out the information properly to us.
____________________________________________________________________
The output of the program will look something like this:

Hostname: shashanke | Kernel Version: 5.10.11
CPU: AMD EPYC Processor (with IBPB), Processing Units: 2
Uptime: 15 days, 5 hours, 0 minutes, 2 seconds

Load Average (1/5/15 min): 0.07 0.15 0.14
CPU Usage:    [--------------------] 0.0%
Memory Usage: [###-----------------] 18.3% (0.3 / 1.4 GB)

Tasks: 75 total
1 running, 0 waiting, 74 sleeping, 0 stopped, 0 zombie

      PID |                 Task Name |        State |            User
----------+---------------------------+--------------+-----------------
   126981 |                 inspector |      running |          sreeda

______________________________________________________________________

To compile and run:

```bash
make
./inspector
```

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```
