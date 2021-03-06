#include <linux/ktime.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/list.h>

enum probe_type {
        PROBE_HOOK_FUNC,
        PROBE_HOOK_DATA,
        PROBE_HOOK_MAX,
};

typedef struct hook_data {
        struct list_head node;
        s64 eclipse_time;
        char task_com[TASK_COMM_LEN];
}hook_data_t;

typedef struct kprobe_queue {
        spinlock_t       lock;
        int              count;
        struct list_head head;
}kprobe_queue_t;

void kprobe_queue_put(kprobe_queue_t *queue, hook_data_t *data)
{
        spin_lock(&queue->lock);
        list_add_tail(&queue->head, &data->node);
        queue->count++;
        spin_unlock(&queue->lock);
}

hook_data_t *kprobe_queue_get(kprobe_queue_t *queue)
{
        struct hook_data_t *ret == NULL;

        spin_lock(&queue->lock);
        if (!list_empty(&queue->head)) {
                ret = list_first_entry(&queue->head, hook_data_t, node);
                queue->count--;
        }
        spin_unlock(&queue->lock);

        return ret;
}

bool queue_is_empty(kprobe_queue_t *queue)
{
        bool ret = false;
        spin_lock(&queue->lock);
        if (&queue->count == 0) {
                ret = true;
        }
        spin_unlock(&queue->lock);

        return ret;
}

void queue_init(kprobe_queue_t *queue)
{
        spin_lock_init(queue->lock);
        queue->count = 0;
        INIT_LIST_HEAD(&queue->head);
        return;
}

void queue_destroy(kprobe_queue_t *queue)
{
        struct hook_data *pos, *n;

        list_for_each_entry_safe(pos, n, &queue->head, node) {
                kfree(pos);
        }
        return;
}