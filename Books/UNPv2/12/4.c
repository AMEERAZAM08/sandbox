#include	"unpipc.h"

#define	SEM_NAME	"mysem"

int
main(int argc, char **argv)
{
	int		fd, i, nloop, zero = 0;
	int		*ptr;
	sem_t	*mutex;

	if (argc != 3)
		err_quit("usage: incr2 <pathname> <#loops>");
	nloop = atoi(argv[2]);

		/* 4open file, initialize to 0, map into memory */
	fd = Open(argv[1], O_RDWR | O_CREAT, FILE_MODE);
	Write(fd, &zero, sizeof(int));
	ptr = Mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	Close(fd);

		/* 4create, initialize, and unlink semaphore */
	mutex = Sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1);
	Sem_unlink(SEM_NAME);

	setbuf(stdout, NULL);	/* stdout is unbuffered */
	if (Fork() == 0) {		/* child */
		for (i = 0; i < nloop; i++) {
			Sem_wait(mutex);
			printf("child: %d\n", (*ptr)++);
			Sem_post(mutex);
		}
		exit(0);
	}

		/* 4parent */
	for (i = 0; i < nloop; i++) {
		Sem_wait(mutex);
		printf("parent: %d\n", (*ptr)++);
		Sem_post(mutex);
	}


    /* MAP_PRIVATE
              Create a private copy-on-write mapping.  Updates to the mapping
              are not visible to other processes mapping the same file, and are
              not carried through to the underlying file.  It is unspecified
              whether changes made to the file after the mmap() call are visible
              in the mapped region. */
    char cmd[128] = { 0 };
    snprintf(cmd, sizeof(cmd), "od -D %s", argv[1]);
    system(cmd);

	exit(0);
}
