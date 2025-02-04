#include <iostream>
#include <unistd.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/syscall.h>
//#include <time.h>
#include <ctime>
#include <cmath>

using namespace std;

typedef unsigned long long u64;
typedef struct task_params {
	char* mibench_cmd;
	__u64 runtime;  // ns
	__u64 period;   // ns
}task_params;

struct task_params task_params_instances[20];

struct sched_attr {
	__u32 size;
	__u32 sched_policy;
	__u64 sched_flags;
	__s32 sched_nice;
	__u32 sched_priority;
	__u64 sched_runtime;
	__u64 sched_deadline;
	__u64 sched_period;
};

int sched_setattr(pid_t pid,
		const struct sched_attr *attr,
		unsigned int flags)
{
	return syscall(__NR_sched_setattr, pid, attr, flags);
}

int sched_getattr(pid_t pid,
		struct sched_attr *attr,
		unsigned int size,
		unsigned int flags)
{
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
}

void busy_loop_us(unsigned int us) {
	//unsigned i, j, k;
	volatile double i, j, k;
	//std::srand(std::time(0));
	//k=std::rand();
	k=1;
	for (i=1;i<=us;i++) {
		for (j=1;j<=120;j++)
			k=i+j;
			//k=(i+j)*k;
			//k=sqrt(k*M_PI*(i+j));
			//k=sqrt(M_PI*k);
	}
	i = k;  // this is to mute compiler's warnings.
}

void busy_wait_us_clock(long microseconds) {
    struct timespec start, now;
    long elapsed_ns = 0;
    long target_ns = microseconds * 1000L;

    // Get the current time using a high-precision clock
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while (elapsed_ns < target_ns) {
        // Get the current time again
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);

        // Calculate the elapsed time in nanoseconds
        elapsed_ns = (now.tv_sec - start.tv_sec) * 1000000000L + (now.tv_nsec - start.tv_nsec);
    }
}

int main(int argc, char **argv)
{
	int cycle = 0;
	struct sched_attr attr;
	struct timespec timeBegin, timeEnd;

	if (argc < 5) {
		cout << "Usage: ./RT-apps [duration_cycles] [runtime] [period] [deadline]" << endl;
		return 0;
	}

	cycle = atoi(argv[1]);

	attr.size = sizeof(attr);
	attr.sched_flags = 0;
	attr.sched_nice = 0;
	attr.sched_priority = 0;
	attr.sched_policy = 6;
	attr.sched_runtime = strtoull(argv[2], NULL, 10);
	attr.sched_runtime *= (1000*1000);
	attr.sched_period = strtoull(argv[3], NULL, 10);
	attr.sched_period *= (1000*1000);
	attr.sched_deadline = strtoull(argv[4], NULL, 10);
	attr.sched_deadline *= (1000*1000);

	int ret = sched_setattr(0, &attr, 0);
	if (ret < 0) {
		perror("sched_setattr");
		return ret;
	}

	int busy_loop_us_time = attr.sched_runtime/1000;
	int busy_loop_us_time_08 = busy_loop_us_time*0.9999;

	while (cycle>0) {
		clock_gettime(CLOCK_MONOTONIC, &timeBegin);
		//busy_loop_us(busy_loop_us_time_08);
		busy_wait_us_clock(busy_loop_us_time_08);
		clock_gettime(CLOCK_MONOTONIC, &timeEnd);
		printf("Testing busy_loop_us(%d): actual time elapsed = %ld us \r\n", 
				busy_loop_us_time, ((timeEnd.tv_sec - timeBegin.tv_sec) * 1000000) + 
				(timeEnd.tv_nsec-timeBegin.tv_nsec)/1000);
		sched_yield();
		cycle--;
	}

	return 0;
}
