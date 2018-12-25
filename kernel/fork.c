/*
 *  linux/kernel/fork.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 *  'fork.c' contains the help-routines for the 'fork' system call
 * (see also system_call.s).
 * Fork is rather simple, once you get the hang of it, but the memory
 * management can be a bitch. See 'mm/mm.c': 'copy_page_tables()'
 */

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/segment.h>
#include <linux/ptrace.h>
#include <linux/malloc.h>
#include <linux/ldt.h>

#include <asm/segment.h>
#include <asm/system.h>

asmlinkage void ret_from_sys_call(void) __asm__("ret_from_sys_call");

/* These should maybe be in <linux/tasks.h> */

#define MAX_TASKS_PER_USER (NR_TASKS/2)
#define MIN_TASKS_LEFT_FOR_ROOT 4

extern int shm_fork(struct task_struct *, struct task_struct *);
long last_pid=0;

static int find_empty_process(void)
{
	int free_task;
	int i, tasks_free;
	int this_user_tasks;

repeat:
	if ((++last_pid) & 0xffff8000)
		last_pid=1;
	this_user_tasks = 0;
	tasks_free = 0;
	free_task = -EAGAIN;
	i = NR_TASKS;
	while (--i > 0) {
		if (!task[i]) {
			free_task = i;
			tasks_free++;
			continue;
		}
		if (task[i]->uid == current->uid)
			this_user_tasks++;
		if (task[i]->pid == last_pid || task[i]->pgrp == last_pid ||
		    task[i]->session == last_pid)
			goto repeat;
	}
	if (tasks_free <= MIN_TASKS_LEFT_FOR_ROOT ||
	    this_user_tasks > MAX_TASKS_PER_USER)
		if (current->uid)
			return -EAGAIN;
	return free_task;
}

static struct file * copy_fd(struct file * old_file)
{
	struct file * new_file = get_empty_filp();
	int error;

	if (new_file) {
		memcpy(new_file,old_file,sizeof(struct file));
		new_file->f_count = 1;
		if (new_file->f_inode)
			new_file->f_inode->i_count++;
		if (new_file->f_op && new_file->f_op->open) {
			error = new_file->f_op->open(new_file->f_inode,new_file);
			if (error) {
				iput(new_file->f_inode);
				new_file->f_count = 0;
				new_file = NULL;
			}
		}
	}
	return new_file;
}

int dup_mmap(struct task_struct * tsk)
{
	struct vm_area_struct * mpnt, **p, *tmp;

	tsk->mmap = NULL;
	tsk->stk_vma = NULL;
	p = &tsk->mmap;
	for (mpnt = current->mmap ; mpnt ; mpnt = mpnt->vm_next) {
		tmp = (struct vm_area_struct *) kmalloc(sizeof(struct vm_area_struct), GFP_KERNEL);
		if (!tmp)
			return -ENOMEM;
		*tmp = *mpnt;
		tmp->vm_task = tsk;
		tmp->vm_next = NULL;
		if (tmp->vm_inode)
			tmp->vm_inode->i_count++;
		*p = tmp;
		p = &tmp->vm_next;
		if (current->stk_vma == mpnt)
			tsk->stk_vma = tmp;
	}
	return 0;
}

#define IS_CLONE (regs.orig_eax == __NR_clone)
#define copy_vm(p) ((clone_flags & COPYVM)?copy_page_tables(p):clone_page_tables(p))

/*
 *  Ok, this is the main fork-routine. It copies the system process
 * information (task[nr]) and sets up the necessary registers. It
 * also copies the data segment in its entirety.
 */
asmlinkage int sys_fork(struct pt_regs regs)
{
	struct pt_regs * childregs;
	struct task_struct *p;
	int i,nr;
	struct file *f;
	unsigned long clone_flags = COPYVM | SIGCHLD;
printk("fork %d\n",__LINE__);
	if(!(p = (struct task_struct*)__get_free_page(GFP_KERNEL)))
		goto bad_fork;
printk("fork %d\n",__LINE__);
	nr = find_empty_process();
printk("fork %d\n",__LINE__);
	if (nr < 0)
		goto bad_fork_free;
printk("fork %d\n",__LINE__);
	task[nr] = p;
printk("fork %d\n",__LINE__);
	*p = *current;
	p->did_exec = 0;
	p->kernel_stack_page = 0;
	p->state = TASK_UNINTERRUPTIBLE;
	p->flags &= ~(PF_PTRACED|PF_TRACESYS);
printk("fork %d\n",__LINE__);
	p->pid = last_pid;
	p->swappable = 1;
	p->p_pptr = p->p_opptr = current;
	p->p_cptr = NULL;
printk("fork %d\n",__LINE__);
	SET_LINKS(p);
printk("fork %d\n",__LINE__);
	p->signal = 0;
	p->it_real_value = p->it_virt_value = p->it_prof_value = 0;
	p->it_real_incr = p->it_virt_incr = p->it_prof_incr = 0;
	p->leader = 0;		/* process leadership doesn't inherit */
	p->utime = p->stime = 0;
	p->cutime = p->cstime = 0;
	p->min_flt = p->maj_flt = 0;
	p->cmin_flt = p->cmaj_flt = 0;
	p->start_time = jiffies;
/*
 * set up new TSS and kernel stack
 */
printk("fork %d  kernel_stack_page:%x\n",__LINE__,p->kernel_stack_page);
	p->kernel_stack_page = get_free_page(GFP_KERNEL);
printk("fork %d  kernel_stack_page:%x\n",__LINE__,p->kernel_stack_page);
	if (!p->kernel_stack_page)
		goto bad_fork_cleanup;
printk("fork %d p:%x  p->kernel_stack_page:%x\n",__LINE__,p,p->kernel_stack_page);
	*(unsigned long *)p->kernel_stack_page = STACK_MAGIC;
printk("fork %d\n",__LINE__);
	p->tss.es = KERNEL_DS;
	p->tss.cs = KERNEL_CS;
	p->tss.ss = KERNEL_DS;
	p->tss.ds = KERNEL_DS;
	p->tss.fs = USER_DS;
	p->tss.gs = KERNEL_DS;
	p->tss.ss0 = KERNEL_DS;
printk("fork %d\n",__LINE__);
	p->tss.esp0 = p->kernel_stack_page + PAGE_SIZE;
	p->tss.tr = _TSS(nr);
	childregs = ((struct pt_regs *) (p->kernel_stack_page + PAGE_SIZE)) - 1;
	p->tss.esp = (unsigned long) childregs;
	p->tss.eip = (unsigned long) ret_from_sys_call;
printk("fork %d\n",__LINE__);
	*childregs = regs;
	childregs->eax = 0;
	p->tss.back_link = 0;
	p->tss.eflags = regs.eflags & 0xffffcfff;	/* iopl is always 0 for a new process */
printk("fork %d\n",__LINE__);
	if (IS_CLONE) {
		if (regs.ebx)
			childregs->esp = regs.ebx;
		clone_flags = regs.ecx;
printk("fork %d\n",__LINE__);
		if (childregs->esp == regs.esp)
			clone_flags |= COPYVM;
	}
printk("fork %d\n",__LINE__);
	p->exit_signal = clone_flags & CSIGNAL;
	p->tss.ldt = _LDT(nr);
printk("fork %d\n",__LINE__);
	if (p->ldt) {
		p->ldt = (struct desc_struct*) vmalloc(LDT_ENTRIES*LDT_ENTRY_SIZE);
		if (p->ldt != NULL)
			memcpy(p->ldt, current->ldt, LDT_ENTRIES*LDT_ENTRY_SIZE);
	}
	p->tss.bitmap = offsetof(struct tss_struct,io_bitmap);
printk("fork %d\n",__LINE__);
	for (i = 0; i < IO_BITMAP_SIZE+1 ; i++) /* IO bitmap is actually SIZE+1 */
		p->tss.io_bitmap[i] = ~0;
	if (last_task_used_math == current)
		__asm__("clts ; fnsave %0 ; frstor %0":"=m" (p->tss.i387));
	p->semun = NULL; p->shm = NULL;
printk("fork %d\n",__LINE__);
	if (copy_vm(p) || shm_fork(current, p))
		goto bad_fork_cleanup;
	if (clone_flags & COPYFD) {
		for (i=0; i<NR_OPEN;i++)
			if ((f = p->filp[i]) != NULL)
				p->filp[i] = copy_fd(f);
	} else {
		for (i=0; i<NR_OPEN;i++)
			if ((f = p->filp[i]) != NULL)
				f->f_count++;
	}
	if (current->pwd)
		current->pwd->i_count++;
	if (current->root)
		current->root->i_count++;
	if (current->executable)
		current->executable->i_count++;
	dup_mmap(p);
	set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY,&(p->tss));
	if (p->ldt)
		set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,p->ldt, 512);
	else
		set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,&default_ldt, 1);

	p->counter = current->counter >> 1;
	p->state = TASK_RUNNING;	/* do this last, just in case */
	return p->pid;
bad_fork_cleanup:
	task[nr] = NULL;
printk("fork %d\n",__LINE__);
	REMOVE_LINKS(p);
	free_page(p->kernel_stack_page);
bad_fork_free:
	free_page((long) p);
printk("fork %d\n",__LINE__);
bad_fork:
	return -EAGAIN;
}
