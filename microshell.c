# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>

void	print_err(char *str)
{
	while (*str)
		write(2, str++, 1);
}

int	cd(char **arv, int i)
{
	if (i != 2)
		return (print_err("error: bad arguments\n")), 1;
	if (chdir(arv[1]) == -1)
		return (print_err("error: cd: cannot change directory to "), print_err(arv[1]), print_err("\n")), 1;
	return (0);
}

void	set_pipe(int has_pipe, int *fd, int end)
{
	if (has_pipe && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		print_err("error: fatal\n"), exit(1);
}

int	exec(char **arv, int i, char **envp)
{
	int	has_pipe;
	int	fd[2];
	int	pid;
	int	status;

	has_pipe = arv[i] && !strcmp(arv[i], "|");
	if (!has_pipe && !strcmp(*arv, "cd"))
		return(cd(arv, i));
	if (has_pipe && pipe(fd) == -1)
		print_err("error: fatal\n"), exit(1);
	if ((pid = fork()) == -1)
		print_err("error: fatal\n"), exit(1);
	if (!pid)
	{
		arv[i] = 0;
		set_pipe(has_pipe, fd, 1);
		if (!strcmp(*arv, "cd"))
			exit(cd(arv, i));
		execve(*arv, arv, envp);
		print_err("error: cannot execute "), print_err(*arv), print_err("\n"), exit(1);
	}
	waitpid(pid, &status, 0);
	set_pipe(has_pipe, fd, 0);
	return (WIFEXITED(status) && WEXITSTATUS(status));

}
int	main (int arc, char **arv, char **envp)
{
	(void)arc;
	int	i = 0;
	int status = 0;

	while (arv[i])
	{
		arv += i + 1;
		i = 0;
		while (arv[i] && strcmp(arv[i], "|") && strcmp(arv[i], ";"))
			i++;
		if (i)
			status = exec(arv, i, envp);
	}
	return (status);
}