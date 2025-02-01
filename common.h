#define _GNU_SOURCE
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>

#define STAT_F "/proc/stat"
#define DISKSTAT_F "/proc/diskstats"
#define MEM_F "/proc/meminfo"
#define DEVNET_F "/proc/net/dev"
#define ERR_MSG_LEN 1024
#define EXIT_FAILURE_MALLOC 1
#define EXIT_FAILURE_FILE 2
#define DEBUG 1
#define MAX_STR_SIZE 128
#define MAX_DISK_NAME 32
#define MAX_HASH_SIZE 128
#define TRUE 1
#define FALSE 0
typedef unsigned long long int t_stat;
typedef uint8_t entries_t; 

struct cpu_stats {
    t_stat user;         // system spent in user mode
    t_stat nice;         // user mode with low priority (nice)
    t_stat system;       // system mode
    t_stat idle;         // idle tasks
    t_stat iowait;       // iowait time 
    t_stat irq;          // time servicing  interrupts
    t_stat softirq;      // time servicing softirqs
    t_stat steal;        // stolen time, which is the time spent in other operating systems when running in a virtualized environment
    t_stat guest;        // is the time spent running a virtual CPU for guest operating systems under the  control of the Linux kernel
};

typedef struct cpu_stats cpu_stat_t; 
typedef cpu_stat_t *  cpu_stat_p; 


struct common_stats {
    t_stat intr; // total number of interrupts 
    t_stat ctxt; // total number os context switches
    t_stat btime; // time since boot 
    t_stat processes; // total number of processes since boot
    t_stat procs_running; // total number of running processes 
    t_stat procs_blocked; // total numbe of blocked processes 
    t_stat softirq;  
};
typedef struct common_stats common_stats_t;
typedef common_stats_t * common_stats_p;

struct iostats { 
    t_stat rds; //  number of reads completed successfully 
    t_stat rds_merged; // Number of reads which are merged 
    t_stat rds_sectors; // Number of sectors read 
    t_stat rds_time; // Total Milliseconds spent on all reads 
    t_stat wrs; //  number of writes completed successfully 
    t_stat wrs_merged; // Number of writes which are merged 
    t_stat wrs_sectors; // Number of sectors written 
    t_stat wrs_time; // Total Milliseconds spent on all writes 
    t_stat io_current; // I/Os currently in progress
    t_stat io_time; // Milliseconds spent on I/O, increases as long as io_current is non_zero
    t_stat io_weighted_time;  // weighted # of milliseconds spent doing I/Os
};


typedef struct iostats  diskstats_t; 
typedef diskstats_t * diskstats_p; 



struct meminfo { 
    entries_t entries;       
    t_stat * statsL; 
};

typedef struct meminfo memstats_t; 
typedef memstats_t * memstats_p; 

struct disk_node { 
    char name[MAX_DISK_NAME]; 
    short major;  
    short minor; 
     diskstats_p stats; 
};
#define MEM_F "/proc/meminfo"

typedef struct disk_node disk_t; 
typedef disk_t * disk_p; 


struct devnetstats { 

    t_stat bytes;
    t_stat packets;
    t_stat errs;
    t_stat drop;
    t_stat fifo;
    t_stat frame; 
    t_stat compressed;
    t_stat multicast;
    t_stat tr_bytes;
    t_stat tr_packets;
    t_stat tr_errs;
    t_stat tr_drop;
    t_stat tr_fifo;
    t_stat tr_colls;
    t_stat tr_carrier;
    t_stat tr_compressed;

};

typedef struct devnetstats devnetstats_t;
typedef devnetstats_t * devnetstats_p;

struct dev_interface { 
    char *NAME;
    devnetstats_p stats;
};

typedef struct  dev_interface interface_t; 
typedef interface_t * interface_p;
