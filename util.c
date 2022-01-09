#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>

#include "util.h"
#include "logger.h"

int open_path(char *proc_dir, char *path)
{
	char full_path[128];
	sprintf(full_path, "%s/%s", proc_dir, path);

	int fd = open(full_path, O_RDONLY);
	return fd;
}
ssize_t lineread(int fd, char *buf, size_t sz)
{

    int i=0;
    char temp[2]={0};
    while(buf[i-1]!='\n'&& i<sz)
    {

	int f=read(fd,temp,1);
    	if(f<=0){
		return i;
	}
	buf[i++]=*temp;
    }
    return i;
}
void draw_percbar(char *buf, double frac) {
	if(frac<=0.0)
		frac=0.0;
	else if(frac>=1.0)
		frac=1;
	else if(isnan(frac)!=0)
		frac=0.0;
	double value=frac*100;
	frac+=0.005;
	frac*=100;
	int temp=(int)frac;
	temp/=5;
	char *t=malloc(128);
	t[0]='[';
	for(int i=0;i<20;i++){
		if(i<temp)
			t[i+1]='#';
		else
			t[i+1]='-';
	}
	t[21]=']';
	t[22]='\0';
	sprintf(buf,"%s %.1f%%",t,value);
	free(t);

}

void uid_to_uname(char *name_buf, uid_t uid)
{
    int fd=open("/etc/passwd",O_RDONLY);
    bool truth=true;
    char *token=malloc(256);
    char *token2=token;
    int i=(int)uid;
    if(fd!=-1){
    	size_t temp;
    	while((temp=lineread(fd,token,255))>0){
		char *t=strsep(&token,":");
		strncpy(name_buf,t,15);
		strsep(&token,":");
		char *t2=strsep(&token,":");
		int j=atoi(t2);
		token=token2;
		memset(token,0,256);
		if(i==j){
			truth=false;
			break;
		}
    	}
    }
    close(fd);
    if(truth==true)
	sprintf(name_buf,"%u",uid);
    free(token2);
}
