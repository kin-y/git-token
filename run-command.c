begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_function
DECL|function|start_command
name|int
name|start_command
parameter_list|(
name|struct
name|child_process
modifier|*
name|cmd
parameter_list|)
block|{
name|cmd
operator|->
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|pid
operator|<
literal|0
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_FORK
return|;
if|if
condition|(
operator|!
name|cmd
operator|->
name|pid
condition|)
block|{
if|if
condition|(
name|cmd
operator|->
name|no_stdin
condition|)
block|{
name|int
name|fd
init|=
name|open
argument_list|(
literal|"/dev/null"
argument_list|,
name|O_RDWR
argument_list|)
decl_stmt|;
name|dup2
argument_list|(
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|stdout_to_stderr
condition|)
name|dup2
argument_list|(
literal|2
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|git_cmd
condition|)
block|{
name|execv_git_cmd
argument_list|(
name|cmd
operator|->
name|argv
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|execvp
argument_list|(
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|,
operator|(
name|char
operator|*
specifier|const
operator|*
operator|)
name|cmd
operator|->
name|argv
argument_list|)
expr_stmt|;
block|}
name|die
argument_list|(
literal|"exec %s failed."
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|finish_command
name|int
name|finish_command
parameter_list|(
name|struct
name|child_process
modifier|*
name|cmd
parameter_list|)
block|{
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|status
decl_stmt|,
name|code
decl_stmt|;
name|pid_t
name|waiting
init|=
name|waitpid
argument_list|(
name|cmd
operator|->
name|pid
argument_list|,
operator|&
name|status
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
name|waiting
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|EINTR
condition|)
continue|continue;
name|error
argument_list|(
literal|"waitpid failed (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID
return|;
block|}
if|if
condition|(
name|waiting
operator|!=
name|cmd
operator|->
name|pid
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID_WRONG_PID
return|;
if|if
condition|(
name|WIFSIGNALED
argument_list|(
name|status
argument_list|)
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID_SIGNAL
return|;
if|if
condition|(
operator|!
name|WIFEXITED
argument_list|(
name|status
argument_list|)
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID_NOEXIT
return|;
name|code
operator|=
name|WEXITSTATUS
argument_list|(
name|status
argument_list|)
expr_stmt|;
if|if
condition|(
name|code
condition|)
return|return
operator|-
name|code
return|;
return|return
literal|0
return|;
block|}
block|}
end_function
begin_function
DECL|function|run_command
name|int
name|run_command
parameter_list|(
name|struct
name|child_process
modifier|*
name|cmd
parameter_list|)
block|{
name|int
name|code
init|=
name|start_command
argument_list|(
name|cmd
argument_list|)
decl_stmt|;
if|if
condition|(
name|code
condition|)
return|return
name|code
return|;
return|return
name|finish_command
argument_list|(
name|cmd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|run_command_v_opt
name|int
name|run_command_v_opt
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|opt
parameter_list|)
block|{
name|struct
name|child_process
name|cmd
decl_stmt|;
name|memset
argument_list|(
operator|&
name|cmd
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|cmd
argument_list|)
argument_list|)
expr_stmt|;
name|cmd
operator|.
name|argv
operator|=
name|argv
expr_stmt|;
name|cmd
operator|.
name|no_stdin
operator|=
name|opt
operator|&
name|RUN_COMMAND_NO_STDIN
condition|?
literal|1
else|:
literal|0
expr_stmt|;
name|cmd
operator|.
name|git_cmd
operator|=
name|opt
operator|&
name|RUN_GIT_CMD
condition|?
literal|1
else|:
literal|0
expr_stmt|;
name|cmd
operator|.
name|stdout_to_stderr
operator|=
name|opt
operator|&
name|RUN_COMMAND_STDOUT_TO_STDERR
condition|?
literal|1
else|:
literal|0
expr_stmt|;
return|return
name|run_command
argument_list|(
operator|&
name|cmd
argument_list|)
return|;
block|}
end_function
end_unit
