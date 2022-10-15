Task 1:

Compilation command: make all
Run: ./sniffer
    or ./sniffer -p <directory name>

For your own help, create a directory for listener to monitor called "listenerdir" and run the program as ./sniffer -p ./listenerdir/

Manager.c: It's the main code for this task. It creates the listener using fork and then it runs the code for
the manager. Using dup2 (pipe), it connects the stdout of the listener with the stdin of the manager. Manager reads the output 
of the listener byte per byte and stores it into a string, whose size is by default 50 (taking into consideration that listener will never
send a bigger message). While() loop of reading from listener has 3 reasons to break: 1) read() returns -1, so it has nothing more to read, 
2) signal has arrived, so errno == EINTR, that's why we check immediately for the flags of handlers of SIGINT and SIGCHLD in case a worker has stopped 
or control + c has arrived and 3) manager has read the whole message from listener. At that case (3), manager has to decide whether he will create
a worker or receive someone from the queue. If queue is empty, he creates a worker process and a fifo(named pipe) with the name of the pid of this process. All
the named pipes are stored by default of my program into a directory called "fifos", so that there will be no conflict if listener monitors the current directory.
He writes the filename received from the listener (after the appropriate changes) into the named pipe for the worker to read. From the other hand, If
queue is not empty, he receives the first available worker, sends a signal to unblock him (via SIGCONT), opens the existing named pipe of this worker and 
writes the filename. This whole thing happens into a loop until a SIGINT signal arrives so that the manager and all the other processes can terminate. Workers finish their task and then they can terminate, that's why the check of the flag of sigint is at the end; 

Worker.c: It's the code for the worker. After its execution from the manager (using execvp), worker reads the filename from the pipe and opens the appropriate file.
Then, he creates a file, with name similar to the one he opened, together with ".out".Then, he reads the file and writes the protocols into the new file. After finishing, he blocks himself raising SIGSTOP signal and waits for the manager to unblock him.

myfunctions.c/ myfunctions.h (library): It's a file full of functions that i created in order to accomplish the tasks of the workers and the manager. path() function is used from the manager to split the message of the listener and receive only the path to the file (without the events). onlyfilename() is used from the worker to receive the filename in order to concat the ".out". outfile() is also used from the worker to combine the name, the directory and the ".out". Create a file named "workerdir" in order to store all the .out files of the workers. fifopath() is used from the manager to concat the name of a fifo with the directory "fifos" i reffered to before. At last, findProtocol() is the main function for the workers to find the protocols and write them into the .out file. In order to do that, i use a simple linked list to store via storeProtocol() function the urls and check via search() function if there is already the url into the list (in that case we just increase the counter). The main idea is that i store the whole file into a string, i split it by space, then i check if http:// exists, and if so, i check if www exists and split
the protocol with slash in order to receive only the adress after the http:// (or www if exists) and before the first slash. Storing each adress into the list, I just go through the list and write each adress with its counter into the .out file.

Queue.c/ Queue.h: It's the file with all the appropriate functions for a queue.



Task 2:

Run: bash finder.sh <tld> <tld> 

Give as arguments tlds, such as .com
The script prints the number of appearances of locations that finishes with the given tld.
Taking into consideration that we will search the already created files into the workerdir, the script, for each tld, reads each file, line per line, splits it with space, stores the first half into an array 
and checks if the tld exists at the end of the location. I so, it adds the second half of the line (the counter) into the current value s. After searching all the files, it prints the s value with an appropriate
message.


