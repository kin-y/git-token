begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_decl_stmt
DECL|variable|diff_files_usage
specifier|static
specifier|const
name|char
name|diff_files_usage
index|[]
init|=
literal|"git-diff-files [-q] [-0/-1/2/3 |-c|--cc] [<common diff options>] [<path>...]"
name|COMMON_DIFF_OPTIONS_HELP
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_diff_files
name|int
name|cmd_diff_files
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
name|int
name|silent
init|=
literal|0
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
comment|/* no "diff" UI options */
name|rev
operator|.
name|abbrev
operator|=
literal|0
expr_stmt|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
operator|<
name|argc
operator|&&
name|argv
index|[
literal|1
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--base"
argument_list|)
condition|)
name|rev
operator|.
name|max_count
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--ours"
argument_list|)
condition|)
name|rev
operator|.
name|max_count
operator|=
literal|2
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--theirs"
argument_list|)
condition|)
name|rev
operator|.
name|max_count
operator|=
literal|3
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-q"
argument_list|)
condition|)
name|silent
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|diff_files_usage
argument_list|)
expr_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|rev
operator|.
name|diffopt
operator|.
name|output_format
condition|)
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_RAW
expr_stmt|;
comment|/* 	 * Make sure there are NO revision (i.e. pending object) parameter, 	 * rev.max_count is reasonable (0<= n<= 3), 	 * there is no other revision filtering parameters. 	 */
if|if
condition|(
name|rev
operator|.
name|pending
operator|.
name|nr
operator|||
name|rev
operator|.
name|min_age
operator|!=
operator|-
literal|1
operator|||
name|rev
operator|.
name|max_age
operator|!=
operator|-
literal|1
condition|)
name|usage
argument_list|(
name|diff_files_usage
argument_list|)
expr_stmt|;
return|return
name|run_diff_files
argument_list|(
operator|&
name|rev
argument_list|,
name|silent
argument_list|)
return|;
block|}
end_function
end_unit
