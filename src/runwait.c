#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static void usage(const char *a){
	fprintf(stderr,"Usage: %s <cmd> [args]\n",a); 
	exit(1);
}

static double d(struct timespec a, struct timespec b){
 return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;}

int main(int c,char**v){
	if (c < 2) usage(v[0]);
	struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
	
	pid_t child = fork();
    if (child < 0) {
        perror("fork failed");
        exit(1);
    }

	if (child == 0){
		execvp(v[1], &v[1]);
		perror("execvp failed");
		exit(1);
	}

	int status;
	if (waitpid(child, &status, 0) < 0) {
        perror("waitpid failed");
        exit(1);
    }

	clock_gettime(CLOCK_MONOTONIC, &t_end);
	double time = d(t_start, t_end);

	printf("pid=%d elapse=%.2f exit=%d\n",child, time, status);

	return 0;
}
