#include <mach/cthreads.h>


/*
 * Initialization.
 */
static void PyThread__init_thread _P0()
{
	cthread_init();
}

/*
 * Thread support.
 */
int PyThread_start_new_thread _P2(func, void (*func) _P((void *)), arg, void *arg)
{
	int success = 0;	/* init not needed when SOLARIS_THREADS and */
				/* C_THREADS implemented properly */

	dprintf(("PyThread_start_new_thread called\n"));
	if (!initialized)
		PyThread_init_thread();
	/* looks like solaris detaches the thread to never rejoin
	 * so well do it here
	 */
	cthread_detach(cthread_fork((cthread_fn_t) func, arg));
	return success < 0 ? 0 : 1;
}

long PyThread_get_thread_ident _P0()
{
	if (!initialized)
		PyThread_init_thread();
	return (long) cthread_self();
}

static void do_PyThread_exit_thread _P1(no_cleanup, int no_cleanup)
{
	dprintf(("PyThread_exit_thread called\n"));
	if (!initialized)
		if (no_cleanup)
			_exit(0);
		else
			exit(0);
	cthread_exit(0);
}

void PyThread_exit_thread _P0()
{
	do_PyThread_exit_thread(0);
}

void PyThread__exit_thread _P0()
{
	do_PyThread_exit_thread(1);
}

#ifndef NO_EXIT_PROG
static void do_PyThread_exit_prog _P2(status, int status, no_cleanup, int no_cleanup)
{
	dprintf(("PyThread_exit_prog(%d) called\n", status));
	if (!initialized)
		if (no_cleanup)
			_exit(status);
		else
			exit(status);
	if (no_cleanup)
		_exit(status);
	else
		exit(status);
}

void PyThread_exit_prog _P1(status, int status)
{
	do_PyThread_exit_prog(status, 0);
}

void PyThread__exit_prog _P1(status, int status)
{
	do_PyThread_exit_prog(status, 1);
}
#endif /* NO_EXIT_PROG */

/*
 * Lock support.
 */
PyThread_type_lock PyThread_allocate_lock _P0()
{
	mutex_t lock;

	dprintf(("PyThread_allocate_lock called\n"));
	if (!initialized)
		PyThread_init_thread();

	lock = mutex_alloc();
	if (mutex_init(lock)) {
		perror("mutex_init");
		free((void *) lock);
		lock = 0;
	}
	dprintf(("PyThread_allocate_lock() -> %lx\n", (long)lock));
	return (PyThread_type_lock) lock;
}

void PyThread_free_lock _P1(lock, PyThread_type_lock lock)
{
	dprintf(("PyThread_free_lock(%lx) called\n", (long)lock));
	mutex_free(lock);
}

int PyThread_acquire_lock _P2(lock, PyThread_type_lock lock, waitflag, int waitflag)
{
	int success = FALSE;

	dprintf(("PyThread_acquire_lock(%lx, %d) called\n", (long)lock, waitflag));
	if (waitflag) { 	/* blocking */
		mutex_lock(lock);
		success = TRUE;
	} else {		/* non blocking */
		success = mutex_try_lock(lock);
	}
	dprintf(("PyThread_acquire_lock(%lx, %d) -> %d\n", (long)lock, waitflag, success));
	return success;
}

void PyThread_release_lock _P1(lock, PyThread_type_lock lock)
{
	dprintf(("PyThread_release_lock(%lx) called\n", (long)lock));
	mutex_unlock((mutex_t )lock);
}

/*
 * Semaphore support.
 *
 * This implementation is ripped directly from the pthreads implementation.
 * Which is to say that it is 100% non-functional at this time.
 *
 * Assuming the page is still up, documentation can be found at:
 *
 * http://www.doc.ic.ac.uk/~mac/manuals/solaris-manual-pages/solaris/usr/man/man2/_lwp_sema_wait.2.html
 *
 * Looking at the man page, it seems that one could easily implement a
 * semaphore using a condition.
 *
 */
PyThread_type_sema PyThread_allocate_sema _P1(value, int value)
{
	char *sema = 0;
	dprintf(("PyThread_allocate_sema called\n"));
	if (!initialized)
		PyThread_init_thread();

	dprintf(("PyThread_allocate_sema() -> %lx\n", (long) sema));
	return (PyThread_type_sema) sema;
}

void PyThread_free_sema _P1(sema, PyThread_type_sema sema)
{
	dprintf(("PyThread_free_sema(%lx) called\n", (long) sema));
}

int PyThread_down_sema _P2(sema, PyThread_type_sema sema, waitflag, int waitflag)
{
	dprintf(("PyThread_down_sema(%lx, %d) called\n", (long) sema, waitflag));
	dprintf(("PyThread_down_sema(%lx) return\n", (long) sema));
	return -1;
}

void PyThread_up_sema _P1(sema, PyThread_type_sema sema)
{
	dprintf(("PyThread_up_sema(%lx)\n", (long) sema));
}
