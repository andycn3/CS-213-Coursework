#include <stdlib.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"
#include "threadpool.h"

struct tpool
{
	uthread_t *workers;
	struct tpool_stack_elem *tasks_root;
	uthread_mutex_t tasks;
	uthread_cond_t has_tasks;
	uthread_cond_t none;
};

struct tpool_stack_elem
{
	void *arg;
	void (*task)(tpool_t, void *);
	struct tpool_stack_elem *next;
};

void push_task(tpool_t pool, void (*task)(tpool_t, void *), void *arg)
{
	struct tpool_stack_elem *to_add = malloc(sizeof(*to_add));
	to_add->next = pool->tasks_root;
	to_add->arg = arg;
	to_add->task = task;
	pool->tasks_root = to_add;
}

void pop_task(tpool_t pool, void (**to_execute_dst)(tpool_t, void *), void **arg_dst)
{
	*arg_dst = pool->tasks_root->arg;
	*to_execute_dst = pool->tasks_root->task;
	struct tpool_stack_elem *elem = pool->tasks_root;
	pool->tasks_root = pool->tasks_root->next;
	free(elem);
}

/**
 * Base procedure of every worker thread.  Calls available tasks
 * or blocks until a task becomes available.
 */
void *worker_thread(void *pool_v)
{
	tpool_t pool = (tpool_t)pool_v;

	uthread_mutex_lock(pool->tasks);
	while (1)
	{
		while (pool->tasks_root == NULL) {
			uthread_cond_wait(pool->has_tasks);
		}

		void (*task)(tpool_t, void *) = NULL;
		void *arg = NULL;
		pop_task(pool, &task, &arg);

		uthread_mutex_unlock(pool->tasks);
		task(pool, arg);
		uthread_mutex_lock(pool->tasks);

		if (pool->tasks_root == NULL)
			uthread_cond_signal(pool->none);
	}
	uthread_mutex_unlock(pool->tasks);

	return NULL; // TODO: replace this placehold return value
}

/**
 * Create a new thread pool with max_threads thread-count limit.
 */
tpool_t tpool_create(unsigned int max_threads)
{
	tpool_t tp = malloc(sizeof(*tp));
	tp->tasks_root = NULL;

	tp->tasks = uthread_mutex_create();
	tp->none = uthread_cond_create(tp->tasks);
	tp->has_tasks = uthread_cond_create(tp->tasks);

	tp->workers = malloc(sizeof(uthread_t) * max_threads);

	for (unsigned int i = 0; i < max_threads; ++i)
		tp->workers[i] = uthread_create(worker_thread, tp);

	return tp;
}

/**
 * Sechedule task f(arg) to be executed.
 */
void tpool_schedule_task(tpool_t pool, void (*f)(tpool_t, void *), void *arg)
{
	uthread_mutex_lock(pool->tasks);
	push_task(pool, f, arg);
	uthread_cond_signal(pool->has_tasks);
	uthread_mutex_unlock(pool->tasks);
}

/**
 * Wait (by blocking) until all tasks have completed and thread pool is thus idle
 */
void tpool_join(tpool_t pool)
{
	uthread_mutex_lock(pool->tasks);
	while (pool->tasks_root != NULL)
		uthread_cond_wait(pool->none);
	uthread_mutex_unlock(pool->tasks);
}