begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_define
DECL|macro|MAX_ARGS
define|#
directive|define
name|MAX_ARGS
value|32
end_define
begin_decl_stmt
specifier|extern
name|char
modifier|*
modifier|*
name|environ
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|builtin_exec_path
specifier|static
specifier|const
name|char
modifier|*
name|builtin_exec_path
init|=
name|GIT_EXEC_PATH
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|argv_exec_path
specifier|static
specifier|const
name|char
modifier|*
name|argv_exec_path
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|git_set_argv_exec_path
name|void
name|git_set_argv_exec_path
parameter_list|(
specifier|const
name|char
modifier|*
name|exec_path
parameter_list|)
block|{
name|argv_exec_path
operator|=
name|exec_path
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Returns the highest-priority, location to look for git programs. */
end_comment
begin_function
DECL|function|git_exec_path
specifier|const
name|char
modifier|*
name|git_exec_path
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|env
decl_stmt|;
if|if
condition|(
name|argv_exec_path
condition|)
return|return
name|argv_exec_path
return|;
name|env
operator|=
name|getenv
argument_list|(
name|EXEC_PATH_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
name|env
operator|&&
operator|*
name|env
condition|)
block|{
return|return
name|env
return|;
block|}
return|return
name|builtin_exec_path
return|;
block|}
end_function
begin_function
DECL|function|execv_git_cmd
name|int
name|execv_git_cmd
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|char
name|git_command
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
name|int
name|i
decl_stmt|;
specifier|const
name|char
modifier|*
name|paths
index|[]
init|=
block|{
name|argv_exec_path
block|,
name|getenv
argument_list|(
name|EXEC_PATH_ENVIRONMENT
argument_list|)
block|,
name|builtin_exec_path
block|}
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|paths
argument_list|)
condition|;
operator|++
name|i
control|)
block|{
name|size_t
name|len
decl_stmt|;
name|int
name|rc
decl_stmt|;
specifier|const
name|char
modifier|*
name|exec_dir
init|=
name|paths
index|[
name|i
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|tmp
decl_stmt|;
if|if
condition|(
operator|!
name|exec_dir
operator|||
operator|!
operator|*
name|exec_dir
condition|)
continue|continue;
if|if
condition|(
operator|*
name|exec_dir
operator|!=
literal|'/'
condition|)
block|{
if|if
condition|(
operator|!
name|getcwd
argument_list|(
name|git_command
argument_list|,
sizeof|sizeof
argument_list|(
name|git_command
argument_list|)
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git: cannot determine "
literal|"current directory: %s\n"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
break|break;
block|}
name|len
operator|=
name|strlen
argument_list|(
name|git_command
argument_list|)
expr_stmt|;
comment|/* Trivial cleanup */
while|while
condition|(
operator|!
name|prefixcmp
argument_list|(
name|exec_dir
argument_list|,
literal|"./"
argument_list|)
condition|)
block|{
name|exec_dir
operator|+=
literal|2
expr_stmt|;
while|while
condition|(
operator|*
name|exec_dir
operator|==
literal|'/'
condition|)
name|exec_dir
operator|++
expr_stmt|;
block|}
name|rc
operator|=
name|snprintf
argument_list|(
name|git_command
operator|+
name|len
argument_list|,
sizeof|sizeof
argument_list|(
name|git_command
argument_list|)
operator|-
name|len
argument_list|,
literal|"/%s"
argument_list|,
name|exec_dir
argument_list|)
expr_stmt|;
if|if
condition|(
name|rc
operator|<
literal|0
operator|||
name|rc
operator|>=
sizeof|sizeof
argument_list|(
name|git_command
argument_list|)
operator|-
name|len
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git: command name given "
literal|"is too long.\n"
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
else|else
block|{
if|if
condition|(
name|strlen
argument_list|(
name|exec_dir
argument_list|)
operator|+
literal|1
operator|>
sizeof|sizeof
argument_list|(
name|git_command
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git: command name given "
literal|"is too long.\n"
argument_list|)
expr_stmt|;
break|break;
block|}
name|strcpy
argument_list|(
name|git_command
argument_list|,
name|exec_dir
argument_list|)
expr_stmt|;
block|}
name|len
operator|=
name|strlen
argument_list|(
name|git_command
argument_list|)
expr_stmt|;
name|rc
operator|=
name|snprintf
argument_list|(
name|git_command
operator|+
name|len
argument_list|,
sizeof|sizeof
argument_list|(
name|git_command
argument_list|)
operator|-
name|len
argument_list|,
literal|"/git-%s"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|rc
operator|<
literal|0
operator|||
name|rc
operator|>=
sizeof|sizeof
argument_list|(
name|git_command
argument_list|)
operator|-
name|len
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git: command name given is too long.\n"
argument_list|)
expr_stmt|;
break|break;
block|}
comment|/* argv[0] must be the git command, but the argv array 		 * belongs to the caller, and my be reused in 		 * subsequent loop iterations. Save argv[0] and 		 * restore it on error. 		 */
name|tmp
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|git_command
expr_stmt|;
name|trace_argv_printf
argument_list|(
name|argv
argument_list|,
operator|-
literal|1
argument_list|,
literal|"trace: exec:"
argument_list|)
expr_stmt|;
comment|/* execve() can only ever return if it fails */
name|execve
argument_list|(
name|git_command
argument_list|,
operator|(
name|char
operator|*
operator|*
operator|)
name|argv
argument_list|,
name|environ
argument_list|)
expr_stmt|;
name|trace_printf
argument_list|(
literal|"trace: exec failed: %s\n"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|tmp
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|execl_git_cmd
name|int
name|execl_git_cmd
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
modifier|...
parameter_list|)
block|{
name|int
name|argc
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[
name|MAX_ARGS
operator|+
literal|1
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|arg
decl_stmt|;
name|va_list
name|param
decl_stmt|;
name|va_start
argument_list|(
name|param
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|cmd
expr_stmt|;
name|argc
operator|=
literal|1
expr_stmt|;
while|while
condition|(
name|argc
operator|<
name|MAX_ARGS
condition|)
block|{
name|arg
operator|=
name|argv
index|[
name|argc
operator|++
index|]
operator|=
name|va_arg
argument_list|(
name|param
argument_list|,
name|char
operator|*
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|arg
condition|)
break|break;
block|}
name|va_end
argument_list|(
name|param
argument_list|)
expr_stmt|;
if|if
condition|(
name|MAX_ARGS
operator|<=
name|argc
condition|)
return|return
name|error
argument_list|(
literal|"too many args to run %s"
argument_list|,
name|cmd
argument_list|)
return|;
name|argv
index|[
name|argc
index|]
operator|=
name|NULL
expr_stmt|;
return|return
name|execv_git_cmd
argument_list|(
name|argv
argument_list|)
return|;
block|}
end_function
end_unit
