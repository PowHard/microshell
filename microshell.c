#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int error(char *str)
{
	while (*str)
		write(2, str++, 1);
	return (1);
}
int	cd(char **av, int i)
{
	if (i != 2)
		return error ("error: cd: bad arguments\n");
	else if (chdir(av[1]) -1)
		return error("error: cd: cannot change directory to "), error(av[1]), error("\n");
	return (0);
}
int	set_pipe(int is_pipe, int *fd, int i)
{
	if (is_pipe && (dup2(fd[i], i) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return (1);// check compatibility whith return in exec
	return (0);
}

int exec(char **av, char **env, int i)
{
	int fd[2];
	int status;
	int is_pipe = av[i] && !strcmp(av[i], "|");

	if (is_pipe && pipe(fd) == -1)
		return (error("error: fatal\n"));
	
	int pid = fork();
	if (!pid)
	{
		av[i] = 0;
		if (set_pipe(is_pipe, fd, 1) == 1)
			return (error("error: fatal\n"));
		execve(*av, av, env);
		return (error("error: cannot execute "), error(*av), error("\n"));
	}
	waitpid(pid, &status, 0);
	set_pipe(is_pipe, fd, 0);
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int main (int ac, char **av, char **envp)
{
	int	i = 0;
	int	status = 0;

	if (ac > 1)
	{
		while (av[i] && av[++i])
		{
			av += i;
			i = 0;
			while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
				i++;
			if (!strcmp(*av, "cd"))
				status = cd(av, i);
			else if (i)
			status = exec(av, envp, i);
		}
	}
	return (status);
}