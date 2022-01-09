#include "logger.h"
#include "procfs.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
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
int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{
	
    int fd=open_path(proc_dir, "sys/kernel/hostname");
    if(fd==-1)
	    return -1;
    size_t bytes = read(fd, hostname_buf, buf_sz);
    hostname_buf[bytes - 1]='\0';
  
    close(fd);
    return 0;
}
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
int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{

    int fd=open_path(proc_dir, "version");
    char temp[128];
    int i=0;
    if(fd==-1)
	    return -1;
    read(fd,temp, 14);
    while(1){
	read(fd,temp,1);
	if(*temp=='-')
		break;
	version_buf[i]=*temp;
	i++;
    }	    
    version_buf[i]='\0';
    close(fd);
    return 0;
}

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
int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "cpuinfo");
    if(fd==-1)
	    return -1;
    char line[256]={0};
    ssize_t temp;

    char *search="model name";
    while((temp=lineread(fd,line,256))>0){
	if(strncmp(line,search,strlen(search))==0){
		size_t temp2= strcspn(line, ":")+2;
		size_t newline=strcspn(&line[temp2],"\n");
		if(newline>buf_sz)
			newline=buf_sz;
		strncpy(model_buf, &line[temp2], newline);
		break;
	}
    }
    if(temp==-1)
    	return -1;
    close(fd);
    return 0;
}
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
int pfs_cpu_units(char *proc_dir)
{
	    
    int fd = open_path(proc_dir, "cpuinfo");
    if(fd==-1)
	    return -1;
    char line[256]={0};

    int i=0;
    char *search="processor";
    while((lineread(fd,line,256))>0){
	    
	if(strncmp(line,search,strlen(search))==0){
		i+=1;
	}
    }
    close(fd);
    return i;
}

/**
 *Calculates the uptime in seconds
 *
 *@param proc_dir retrieves the location of procfs
 *
 *@return the uptime in seconds on success and -1 on failure
 *
 *
 */
double pfs_uptime(char *proc_dir)
{
    
    int fd=open_path(proc_dir, "uptime");
    char *temp=malloc(256);
    int i=0;
    if(fd==-1){
	    close(fd);
	    free(temp);
	    return -1;
    }    
    lineread(fd, temp, 256);
    while(isdigit(temp[i])||temp[i]=='.'){
	i++;
    }
    temp[i]='\0';
    
   
    close(fd);
    double d=atof(temp);
    free(temp);
    return d;
}

/**
 *
 * Formats the time in seconds to days, hours, minutes, and seconds
 *@param time is the time in seconds
 *@param uptime_buf is the char we're modifying by converting time into days, hours, minutes, and seconds
 *
 *@return 0 on success
 *
 */
int pfs_format_uptime(double time, char *uptime_buf)
{
    int t = (int) time;
    int days = (t % (86400 *30))/86400;
    int hours = (t%86400)/3600;
    int minutes = (t%3600)/60;
    int second = (t%60);
    sprintf(uptime_buf,"%d days, %d hours, %d minutes, %d seconds",days,hours,minutes,second);
    return 0;
}

/**
 *Returns the load average in the form of a struct
 *
 *@param proc_dir retrieves the location of procfs
 *
 *@return struct on success with the load values and empty struct on failure as well
 *
 */
struct load_avg pfs_load_avg(char *proc_dir)
{
   struct load_avg lavg = { 0 };
   int fd = open_path(proc_dir, "loadavg");
   char *temp=malloc(128);
   read(fd,temp,128);
   char *temp2=temp;
   int i;
   if(fd==-1){
	   close(fd);
	   free(temp2);
	   return lavg;
   }

   for(i=0;i<3;i++){
	char *token = strsep(&temp," ");
	if(token == NULL)
		break;
	if(i==0)
		lavg.one=atof(token);
	if(i==1)
		lavg.five=atof(token);
	if(i==2)
		lavg.fifteen=atof(token);
   }
   free(temp2);
   close(fd);
   return lavg;
}
/**
 *Returns the cpu usage by taking in two samples
 *
 *@param proc_dir retrieves the location of proc
 *@param struct cpu_stats *prev is the first sample of cpu usage
 *@param struct cpu_stats *curr is the second sample of cpu usage
 *
 *@return cpu usage on success and returns -1 for any  failure
 *
 *
 */
double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
	
    int fd=open_path(proc_dir,"stat");
    char *temp=malloc(128);
    char *temp2=temp;
    int i=0;
    if(fd==-1){
	close(fd);
	free(temp);
	return 0;
    }	    
    lineread(fd,temp,256);

    strsep(&temp,"  ");
    strsep(&temp, " ");
    curr->total=0;
    while(i<10){
	char *token=strsep(&temp," ");
	if(token==NULL){
		close(fd);
		free(temp);
		return 0;
	}
	long acc=atol(token);
        curr->total+=acc;
	if(i==3)
		curr->idle=acc;
	i++;
    }
    if(prev->idle==0&&prev->total==0){
    	close(fd);
    	free(temp2);
	return 0;
    }
    if(prev->idle>curr->idle&&prev->total>curr->total){
	close(fd);
	free(temp2);
	return 0;
    }
    double usage=1-(((double)curr->idle-(double)prev->idle)/((double)curr->total-(double)prev->total));
    if(isnan(usage)){
	free(temp2);
	close(fd);
	return 0;
    }
    free(temp);
    close(fd);

    return usage; 
}
/**
 *Returns the memory usage in form a struct
 *
 *@param proc_dir retrieves the location of the proc directory
 *
 *@return completed struct on success and empty struct on failure
 */
struct mem_stats pfs_mem_usage(char *proc_dir)
{

    int fd = open_path(proc_dir, "meminfo");

    struct mem_stats mstats={0};
    if(fd==-1){
	    close(fd);
	    return mstats;
    }
    char line[256]={0};
    char line2[256]={0};
    char line3[256]={0};
    ssize_t temp;

    int i=0;
    int j=0;
    int z=0;
    char *search="MemTotal";
    char *search2="MemAvailable";
    while((temp=lineread(fd,line,256))>0){
	if(strncmp(line,search,strlen(search))==0){
		while(i<temp){
			if(isdigit(line[i])!=0||line[i]=='.'){
				line2[j]=line[i];
				j++;
			}
			i++;
		}
		i=0;
		line2[j]='\0';
		if(z>0&&j>0)
			break;
	}
	if(strncmp(line,search2,strlen(search2))==0){
		while(i<temp){
			if(isdigit(line[i])!=0||line[i]=='.'){
				line3[z]=line[i];
				z++;
			}
			i++;
		}
		i=0;
		line3[z]='\0';
		if(z>0&&j>0)
			break;
	}
    }
    mstats.total=atof(line2);
 
    mstats.used=((mstats.total-atof(line3))/1024)/1024;

    mstats.total=(atof(line2)/1024)/1024;
    close(fd);
    return mstats;
}

/**
 *
 *Initializes a struct task_stats and its parameters as well
 *
 *@return completed struct on success and empty struct on failure
 *
 */
struct task_stats *pfs_create_tstats()
{
    struct task_stats *temp=calloc(1,sizeof(struct task_stats));
    if(temp!=NULL){
    	temp->active_tasks=malloc(sizeof(struct task_info));
    }
    return temp;
}

/**
 *
 *Frees the memory in struct task_stats *tstats
 *
 *@param tstats that is malloced with memory that needs to be freed
 */
void pfs_destroy_tstats(struct task_stats *tstats)
{
	free(tstats->active_tasks);
	free(tstats);
}

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
int pfs_tasks(char *proc_dir, struct task_stats *tstats)
{
	
    DIR *directory;
    if ((directory = opendir(proc_dir)) == NULL) {
        perror("opendir");
        return 1;
    }
    char token[300];
    char name[300];
    int size=1;
    int index=0;
    int sleeping=0;
    int running=0;
    int waiting=0;
    int stopped=0;
    int zombie=0;
    char *token2=malloc(256);
    char c[3];
    char *token3=token2;

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
	    if(isdigit(entry->d_name[0])!=0){
		sprintf(token,"%s/status",entry->d_name);
                int fd=open_path(proc_dir,token);
		if(fd!=-1){
	        	int i=0;
	    		int z=0;		
			lineread(fd,token2,256);
			strsep(&token2,":");
			while(i<strlen(token2)){
				if(isalpha(token2[i])!=0){
					int len=i+26;
					while(i<strlen(token2)&&token2[i]!='\0'&&i<len){
						name[z]=token2[i];
						z++;
						i++;
					}
					break;
				}
				i++;
			}
			name[z-1]='\0';
			z=0;
			i=0;
			while(name[i]!='\0'){
				if(isspace(name[i])==0)
				{

					name[z++]=name[i];
				}
				i++;
			}	
			name[z]='\0';
			i=0;
			token2=token3;
			memset(token2,0,256);
			lineread(fd,token2,256);
			lineread(fd,token2,256);
			strsep(&token2,":");
			while(isalpha(token2[i])==0){
				i++;
			}
			c[0]=token2[i];
			c[1]='\0';
			if(c[0]=='T'){
				strsep(&token2,"(");
				if(token2[0]=='t')
					c[0]='t';

			}
			if(c[0]=='S'||c[0]=='s'){
				sleeping++;
				memset(name,0,300);
			}
			else if(c[0]=='R'||c[0]=='r'){
				running++;
				tstats->active_tasks=realloc(tstats->active_tasks,sizeof(struct task_info)*size);
				strcpy(tstats->active_tasks[index].name,name);
				tstats->active_tasks[index].pid=atoi(entry->d_name);
				strcpy(tstats->active_tasks[index].state,"running");
				index++;
				size++;
				memset(name,0,300);
				token2=token3;

			}
			else if(c[0]=='T'||c[0]=='t'){
				stopped++;
				tstats->active_tasks=realloc(tstats->active_tasks,sizeof(struct task_info)*size);
				strcpy(tstats->active_tasks[index].name,name);
				tstats->active_tasks[index].pid=atoi(entry->d_name);
				if(c[0]=='T')
					strcpy(tstats->active_tasks[index].state,"stopped");
				else	
					strcpy(tstats->active_tasks[index].state,"tracing stop");
				index++;
				size++;
				memset(name,0,300);
				token2=token3;
			}
			else if(c[0]=='Z'||c[0]=='z'){
				zombie++;
				tstats->active_tasks=realloc(tstats->active_tasks,sizeof(struct task_info)*size);
				strcpy(tstats->active_tasks[index].name,name);
				tstats->active_tasks[index].pid=atoi(entry->d_name);
				strcpy(tstats->active_tasks[index].state,"zombie");
				index++;
				size++;
				memset(name,0,300);
				token2=token3;
			}
			else if(c[0]=='D'){
				waiting++;
				tstats->active_tasks=realloc(tstats->active_tasks,sizeof(struct task_info)*size);
				strcpy(tstats->active_tasks[index].name,name);
				tstats->active_tasks[index].pid=atoi(entry->d_name);
				strcpy(tstats->active_tasks[index].state,"disk sleep");
				index++;
				size++;
				memset(name,0,300);
				token2=token3;
			}
			if(c[0]!='S'&&c[0]!='s'){
				i=0;
				int j=0;
				char uid[20];
				lineread(fd,token2,256);
				lineread(fd,token2,256);
				lineread(fd,token2,256);
				lineread(fd,token2,256);
				lineread(fd,token2,256);
				lineread(fd,token2,256);
				strsep(&token2,":");
				while(i<strlen(token2)){
					if(isdigit(token2[i])!=0){
						while(isdigit(token2[i])!=0){
							uid[j]=token2[i];
							i++;
							j++;
						}
						break;
					}
					i++;
				}
				uid[j]='\0';
				tstats->active_tasks[index-1].uid=atoi(uid);
				memset(uid,0,20);
				token2=token3;
				memset(token2,0,256);
			}
	    	}
		close(fd);
	}
    }
    tstats->sleeping=sleeping;
    tstats->running=running;
    tstats->zombie=zombie;
    tstats->stopped=stopped;
    tstats->waiting=waiting;
    tstats->total=sleeping+running+zombie+stopped+waiting;

    free(token3);
    closedir(directory);
    return tstats->total;

}
