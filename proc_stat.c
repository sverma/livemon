#include "common.h"

void * lmalloc( int size ) {
    void * p = malloc(size); 
    if(p==NULL) { 
        fprintf (stderr, "%s: Couldn't issue memory Malloc() failed%s\n",
            program_invocation_short_name, strerror (errno));
            exit (EXIT_FAILURE_MALLOC);
    }
    return p; 
}

void *open_r(char *file) { 
    FILE *fp =  fopen(file, "r");
    if (fp == NULL ) {
        fprintf (stderr, "%s: Couldn't open the File %s; %s\n",
            program_invocation_short_name, file, strerror (errno));
            exit (EXIT_FAILURE_FILE);
    }
    return fp;
}

disk_p init_disk_node (){
    disk_p new = (disk_p) lmalloc(sizeof(disk_t)) ; 
    new->stats = (diskstats_p) lmalloc(sizeof(diskstats_t));
    return (new);
}

interface_p init_interface_node (){
    interface_p new = (interface_p) lmalloc(sizeof(interface_t)) ; 
    new->stats = (devnetstats_p) lmalloc(sizeof(devnetstats_t));
    return (new);
}

disk_p init_disks(){ 
    return NULL;
}


void rd_proc_stats( cpu_stat_p cpu_all , common_stats_p common_stats , FILE * statfp ) {
    char *lineptr = NULL ; 
    ssize_t ret; 
    size_t n = 0 ;
    int ncpu = sysconf(_SC_NPROCESSORS_ONLN); 
    ret  = getline(&lineptr, &n , statfp ); 
    if ( ret == -1 ) {
          perror("Failed to read File" ); 
    }
    sscanf(lineptr,"cpu  %llu %llu %llu %llu %llu %llu %llu %llu %llu\n",&(cpu_all->user),&(cpu_all->nice),&(cpu_all->system),\
        &(cpu_all->idle),&(cpu_all->iowait), &(cpu_all->irq),&(cpu_all->softirq),&(cpu_all->steal),&(cpu_all->guest)); 
    short all_cpu_stats = 0; 
    if ( all_cpu_stats ) { 
        // section to calculate all cpus activities 
    } else { 
        int c = ncpu; 
        while(c > 0) {
            if ( getline(&lineptr,&n,statfp) != -1 )  {
                c--; 
            }
        }
    }
    // Calculate generic stats
    while( getline(&lineptr,&n,statfp) > 0 ) { 
        char * str; 
        str = (char *) lmalloc(MAX_STR_SIZE); 
        t_stat val; 
        sscanf(lineptr,"%s %llu",str,&val);
        if ( strcmp( str,"intr") == 0 ) 
            common_stats->intr = val;          
        else if ( strcmp (str,"ctxt") == 0 ) 
            common_stats->ctxt = val; 
        else if ( strcmp( str,"btime") == 0 ) 
            common_stats->btime = val; 
        else if ( strcmp(str,"processes") == 0 ) 
            common_stats->processes = val;
        else if ( strcmp (str,"procs_running")  == 0) 
            common_stats->procs_running = val;
        else if ( strcmp (str,"procs_blocked") == 0 ) 
            common_stats->procs_blocked = val; 
        free(str); 
    }

    free(lineptr);
}
short test_if_block_dev (short maj ){
    if ( ( maj == 3 ) || (maj == 8) )
        return TRUE; 
    else 
        return FALSE; 
}
void rd_diskstats (FILE * fp, disk_p ** dlpp , size_t * entries )  {
    char *lineptr = NULL; 
    size_t n = 0; 
    char * token;
    int cur = 0 ; 
    int init ; 
    ((*entries) == 0 ) ?  (init = TRUE ) : (init = FALSE ) ; 
    while ( getline(&lineptr,&n,fp) > 0 ) { 
        char * cp = strdup(lineptr);
        token = strtok(cp," ");
        short major  = atoi(token); 
        if ( ! test_if_block_dev(major) ) {
            free(cp);
            continue; 
        } 
        disk_p * dlp = *dlpp; 
        if (init == TRUE ) {
            *entries = cur+1; 
            dlp = (disk_p *) realloc ( dlp,  (*entries) * sizeof(disk_p));
            *dlpp = dlp;
            (*(dlp + cur)) = init_disk_node();
        }
        disk_p root = *(dlp+cur);
        root->minor = (short) atoi(strtok(NULL," "));
        token = strtok(NULL," "); 
        char *pos = strstr(lineptr,token);
        diskstats_p ptr;
        ptr = root->stats;
        sscanf(pos,"%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",\
            root->name,&(ptr->rds),&(ptr->rds_merged),&(ptr->rds_sectors),&(ptr->rds_time),\
            &(ptr->wrs),&(ptr->wrs_merged),&(ptr->wrs_sectors),&(ptr->wrs_time),\
            &(ptr->io_current),&(ptr->io_time),&(ptr->io_weighted_time) ) ; 
        printf("DEBUG: %s : wrs : %llu \n", root->name,ptr->wrs_sectors );
        cur++;
        if(cp != NULL )
            free(cp);
    }
    free(lineptr);       
}

void rd_devnet (FILE *fp, interface_p ** ilpp , size_t * entries ){

    char * lineptr = NULL; 
    size_t n = 0; 
    int cur = 0 ;
    int init; 
    char *token; 
    ((*entries) == 0 ) ? (init = TRUE ): (init = FALSE);
    getline(&lineptr,&n,fp);
    getline(&lineptr,&n,fp);
    while(getline(&lineptr,&n,fp) > 0){
        char *cp = strdup(lineptr);
        token = strtok(cp," "); 
        interface_p *ilp = *ilpp; 
        if (init == TRUE ){ 
            *entries = cur+1; 
            ilp = (interface_p *) realloc(ilp,(*entries) * sizeof(interface_p)); 
            *ilpp = ilp; 
            *(ilp+cur) = init_interface_node();
            interface_p p = *(ilp+cur);
            p->NAME = (char *) lmalloc((strlen(token))); 
            strcpy((p->NAME),token); 
        }
        interface_p root = *(ilp+cur); 
        devnetstats_p ptr; 
        token = strtok(NULL," "); 
        char *pos = strstr(lineptr,token);
        ptr = root->stats; 
        int i;
        sscanf(pos,"%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",&(ptr->bytes),&(ptr->packets),&(ptr->errs),\
            &(ptr->drop),&(ptr->fifo),&(ptr->frame),&(ptr->compressed),&(ptr->multicast),&(ptr->tr_bytes),&(ptr->tr_packets),\
            &(ptr->tr_errs),&(ptr->tr_drop),&(ptr->tr_fifo),&(ptr->tr_colls),&(ptr->tr_carrier),&(ptr->tr_compressed) ); 
        printf("DEBUG: %s : bytes : %llu \n", root->NAME,ptr->bytes);
        cur++; 
        if(cp != NULL)
            free(cp);
    }
    free(lineptr);
}
void rd_memstats (FILE *fp, memstats_p p ) {
    char * lineptr = NULL  ;
    size_t n = 0;
    int cur = 0;     
    int init; 
    (p->entries == 0 ) ?  (init = TRUE ) : (init = FALSE ) ;
    char * token;
    while(getline(&lineptr,&n,fp) > 0) {

        if (init == TRUE) {
            p->entries = cur+1;
            p->statsL = ( t_stat * ) realloc(p->statsL, p->entries * sizeof(t_stat)); 
        }
        char *cp = strdup(lineptr);
        token = strtok(cp," ");
        token = strtok(NULL," ");
        char *pos = strstr(lineptr,token); 
        sscanf(pos,"%llu KB\n",(p->statsL + cur) ) ;
        cur++; 
        free(cp);

    }
    free(lineptr);
}
memstats_p init_mem (){
    memstats_p new = (memstats_p) lmalloc(sizeof(memstats_t));
    new->entries = 0; 
    new->statsL = NULL; 
    return(new);
}


int main(int argc, char *argv[]) { 
    long hz_unit = sysconf(_SC_CLK_TCK); 
    
    cpu_stat_p cpu_all = (cpu_stat_p) lmalloc(sizeof(cpu_stat_t));
    common_stats_p common_stats = (common_stats_p) lmalloc(sizeof(common_stats_t)); 

    FILE * statfp =  open_r(STAT_F); 
    FILE * diskstatfp = open_r(DISKSTAT_F);
    FILE * memstatfp = open_r(MEM_F);
    FILE * devnetfp = open_r(DEVNET_F);

    disk_p * list = NULL; 
    size_t entries = 0 ;
    interface_p *list_dev = NULL; 
    size_t entries_dev = 0; 

    memstats_p memp = init_mem();


    while(1) { 
        struct timeval t_now ;
        gettimeofday(&t_now,NULL);
        unsigned long long int t_prev= t_now.tv_usec;

        rd_proc_stats(cpu_all,common_stats, statfp ); 
        rd_diskstats(diskstatfp,&list,&entries);
        rd_memstats(memstatfp,memp);
        rd_devnet(devnetfp,&list_dev,&entries_dev);
        gettimeofday(&t_now,NULL);
        printf("Time taken to parse %llu microseconds, %.3f ms \n",(t_now.tv_usec - t_prev),(double)((t_now.tv_usec - t_prev) / 1000 )); 
        int i;
        
        for(i = 0 ; i < entries ; i++ ) {
            disk_p root = *(list+i) ; 
            diskstats_p ptr = root->stats;
            printf("%s: rds:%llu, rds_merged:%llu, rds_sectors:%llu,\
    rds_time:%llu, wrs:%llu, wrs_merged:%llu, wrs_sectors:%llu wrs_time:%llu,\
    io_current:%llu io_time:%llu io_weighted_time:%llu \n",root->name,ptr->rds,\
            ptr->rds_merged,ptr->rds_sectors,ptr->rds_time,ptr->wrs,ptr->wrs_merged,\
            ptr->wrs_sectors,ptr->wrs_time,ptr->io_current,ptr->io_time,ptr->io_weighted_time\
        ); 
        for (i = 0; i < memp->entries ; i++) {
            printf("%s:%llu\n","Mem Stats",memp->statsL[i]);
        }
    } 
#ifdef DEBUG 
    printf("CPU_STATS: user:%llu, nice:%llu , system:%llu , iowait:%llu , irq:%llu , softirq:%llu , steal:%llu , guest: %llu \n",\
        cpu_all->user,cpu_all->nice, cpu_all->system , cpu_all->iowait,cpu_all->irq,cpu_all->softirq,cpu_all->steal,cpu_all->guest );
#endif

        rewind(statfp);
        rewind(diskstatfp);
        rewind(memstatfp);
        rewind(devnetfp);
        int interval = (argv[1]) ? atoi(argv[1]) : 100000 ;  
        usleep(interval); 
    }        

    fclose(statfp);
    fclose(diskstatfp);
    fclose(memstatfp);
    fclose(devnetfp);
}
