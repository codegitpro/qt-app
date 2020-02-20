package net.ai.tmp.cloudstore.implementation;

import net.ai.Task;
import net.ai.Worker;

import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class MainWorker extends Worker
{
    private final ThreadPoolExecutor shortTasks = new ThreadPoolExecutor(1, 1, 1, TimeUnit.SECONDS, new LinkedBlockingDeque<Runnable>());

    @Override
    public void schedule(final Task todo)
    {
        shortTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                todo.run();
            }
        });
    }
}
