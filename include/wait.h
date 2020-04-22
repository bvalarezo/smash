#ifndef WAIT_H
#define WAIT_H

/* includes */
#include "global.h"
#include <sys/types.h>
#include <sys/wait.h>

#include "jobs.h"
/* definitions */

/* prototypes */

/* wait for the specific job to reply */
int wait_job(struct job_node *j, int options);

/* refresh the jobs status */
int refresh_jobs(void);

/* update the job status by pid */
int update_job_status(pid_t pid, int status);

/* return TRUE or FALSE if the job status match */
int is_status(struct job_node *j, unsigned int process_status);

/* declarations */

#endif