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
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|struct
name|child_process
name|cp
decl_stmt|;
name|int
name|nogit
init|=
literal|0
decl_stmt|;
name|setup_git_directory_gently
argument_list|(
operator|&
name|nogit
argument_list|)
expr_stmt|;
if|if
condition|(
name|nogit
condition|)
name|die
argument_list|(
literal|"No git repo found"
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--setup-work-tree"
argument_list|)
condition|)
block|{
name|setup_work_tree
argument_list|()
expr_stmt|;
name|argv
operator|++
expr_stmt|;
block|}
name|memset
argument_list|(
operator|&
name|cp
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|cp
argument_list|)
argument_list|)
expr_stmt|;
name|cp
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|cp
operator|.
name|argv
operator|=
name|argv
operator|+
literal|1
expr_stmt|;
return|return
name|run_command
argument_list|(
operator|&
name|cp
argument_list|)
return|;
block|}
end_function
end_unit
