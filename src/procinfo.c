#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

static void usage(const char *a){
	fprintf(stderr,"Usage: %s <pid>\n",a); 
	exit(1);
}

static int isnum(const char*s){
	for(;*s;s++) 
		if(!isdigit(*s)) return 0; 
	return 1;
}

int main(int c,char**v){
	if(c!=2||!isnum(v[1])) usage(v[0]);
	int pid = atoi(v[1]);
	char stat[256];
	char status[256];
	char cmdline[256];

	//Paths
	sprintf(stat, "/proc/%d/stat", pid);
 	sprintf(status, "/proc/%d/status", pid);
 	sprintf(cmdline, "/proc/%d/cmdline", pid);
 
	//Parent PID, Process State, CPU time
 	FILE *fstat = fopen(stat, "r");
 	if (!fstat){
 		perror("stat file  error");
		return 1;
 	}	
	
	int ppid;
	char state;
	unsigned long utime, stime;
	fscanf(fstat, "%*d (%*[^)]) %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &state, &ppid, &utime, &stime);
	fclose(fstat);
	
	long ticks_per_sec = sysconf(_SC_CLK_TCK);
    double total_cpu_sec = (utime + stime) / (double)ticks_per_sec;

	//Resident memory usage (VmRSS)
	FILE *fstatus = fopen(status, "r");
    if (!fstatus) {
        perror("status file error");
        return 1;
    }
	
	char line[256];
	int vmrss = 0;
	while(fgets(line, sizeof(line), fstatus)){
		if (strncmp(line, "VmRSS:", 6) == 0) {
    		sscanf(line + 6, " %d", &vmrss);
			break;
		}
	}
	fclose(fstatus);

	//Command Line
	FILE *fcmd = fopen(cmdline, "r");
	if (!fcmd) {
        perror("cmdline file error");
 		return 1;
    }

	char cmd[1024];
	size_t n = fread(cmd, 1, sizeof(cmd)-1, fcmd);
	fclose(fcmd);

	for (size_t i=0; i<n; i++) if (cmd[i] == '\0') cmd[i] = ' ';
	cmd[n] = '\0';

	printf("PID: %d\n", pid);
    printf("State: %c\n", state);
    printf("PPID: %d\n", ppid);
    printf("Cmd: %s\n", cmd);
    printf("CPU: %.2f\n", total_cpu_sec);
    printf("VmRSS: %d\n", vmrss);

	return 0;
}
