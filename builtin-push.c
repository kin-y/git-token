begin_unit
begin_comment
comment|/*  * "git push"  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_include
include|#
directive|include
file|"transport.h"
end_include
begin_decl_stmt
DECL|variable|push_usage
specifier|static
specifier|const
name|char
name|push_usage
index|[]
init|=
literal|"git-push [--all] [--tags] [--receive-pack=<git-receive-pack>] [--repo=all] [-f | --force] [-v] [<repository><refspec>...]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|all
DECL|variable|thin
DECL|variable|verbose
specifier|static
name|int
name|all
decl_stmt|,
name|thin
decl_stmt|,
name|verbose
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|receivepack
specifier|static
specifier|const
name|char
modifier|*
name|receivepack
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|refspec
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|refspec
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|refspec_nr
specifier|static
name|int
name|refspec_nr
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_refspec
specifier|static
name|void
name|add_refspec
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|)
block|{
name|int
name|nr
init|=
name|refspec_nr
operator|+
literal|1
decl_stmt|;
name|refspec
operator|=
name|xrealloc
argument_list|(
name|refspec
argument_list|,
name|nr
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|refspec
index|[
name|nr
operator|-
literal|1
index|]
operator|=
name|ref
expr_stmt|;
name|refspec_nr
operator|=
name|nr
expr_stmt|;
block|}
end_function
begin_function
DECL|function|set_refspecs
specifier|static
name|void
name|set_refspecs
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|refs
parameter_list|,
name|int
name|nr
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|ref
init|=
name|refs
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"tag"
argument_list|,
name|ref
argument_list|)
condition|)
block|{
name|char
modifier|*
name|tag
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
name|nr
operator|<=
operator|++
name|i
condition|)
name|die
argument_list|(
literal|"tag shorthand without<tag>"
argument_list|)
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|refs
index|[
name|i
index|]
argument_list|)
operator|+
literal|11
expr_stmt|;
name|tag
operator|=
name|xmalloc
argument_list|(
name|len
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|tag
argument_list|,
literal|"refs/tags/"
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|tag
argument_list|,
name|refs
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|ref
operator|=
name|tag
expr_stmt|;
block|}
name|add_refspec
argument_list|(
name|ref
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|do_push
specifier|static
name|int
name|do_push
parameter_list|(
specifier|const
name|char
modifier|*
name|repo
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|errs
decl_stmt|;
name|struct
name|remote
modifier|*
name|remote
init|=
name|remote_get
argument_list|(
name|repo
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|remote
condition|)
name|die
argument_list|(
literal|"bad repository '%s'"
argument_list|,
name|repo
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|refspec
operator|&&
operator|!
name|all
operator|&&
name|remote
operator|->
name|push_refspec_nr
condition|)
block|{
name|refspec
operator|=
name|remote
operator|->
name|push_refspec
expr_stmt|;
name|refspec_nr
operator|=
name|remote
operator|->
name|push_refspec_nr
expr_stmt|;
block|}
name|errs
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|remote
operator|->
name|uri_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|transport
modifier|*
name|transport
init|=
name|transport_get
argument_list|(
name|remote
argument_list|,
name|remote
operator|->
name|uri
index|[
name|i
index|]
argument_list|,
literal|0
argument_list|)
decl_stmt|;
name|int
name|err
decl_stmt|;
if|if
condition|(
name|receivepack
condition|)
name|transport_set_option
argument_list|(
name|transport
argument_list|,
name|TRANS_OPT_RECEIVEPACK
argument_list|,
name|receivepack
argument_list|)
expr_stmt|;
if|if
condition|(
name|thin
condition|)
name|transport_set_option
argument_list|(
name|transport
argument_list|,
name|TRANS_OPT_THIN
argument_list|,
literal|"yes"
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Pushing to %s\n"
argument_list|,
name|remote
operator|->
name|uri
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|err
operator|=
name|transport_push
argument_list|(
name|transport
argument_list|,
name|refspec_nr
argument_list|,
name|refspec
argument_list|,
name|flags
argument_list|)
expr_stmt|;
name|err
operator||=
name|transport_disconnect
argument_list|(
name|transport
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|err
condition|)
continue|continue;
name|error
argument_list|(
literal|"failed to push to '%s'"
argument_list|,
name|remote
operator|->
name|uri
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|errs
operator|++
expr_stmt|;
block|}
return|return
operator|!
operator|!
name|errs
return|;
block|}
end_function
begin_function
DECL|function|cmd_push
name|int
name|cmd_push
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
name|int
name|i
decl_stmt|;
name|int
name|flags
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|repo
init|=
name|NULL
decl_stmt|;
comment|/* default repository */
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|!=
literal|'-'
condition|)
block|{
name|repo
operator|=
name|arg
expr_stmt|;
name|i
operator|++
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-v"
argument_list|)
condition|)
block|{
name|verbose
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--repo="
argument_list|)
condition|)
block|{
name|repo
operator|=
name|arg
operator|+
literal|7
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--all"
argument_list|)
condition|)
block|{
name|flags
operator||=
name|TRANSPORT_PUSH_ALL
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--tags"
argument_list|)
condition|)
block|{
name|add_refspec
argument_list|(
literal|"refs/tags/*"
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--force"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-f"
argument_list|)
condition|)
block|{
name|flags
operator||=
name|TRANSPORT_PUSH_FORCE
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--thin"
argument_list|)
condition|)
block|{
name|thin
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--no-thin"
argument_list|)
condition|)
block|{
name|thin
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--receive-pack="
argument_list|)
condition|)
block|{
name|receivepack
operator|=
name|arg
operator|+
literal|15
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--exec="
argument_list|)
condition|)
block|{
name|receivepack
operator|=
name|arg
operator|+
literal|7
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|push_usage
argument_list|)
expr_stmt|;
block|}
name|set_refspecs
argument_list|(
name|argv
operator|+
name|i
argument_list|,
name|argc
operator|-
name|i
argument_list|)
expr_stmt|;
if|if
condition|(
name|all
operator|&&
name|refspec
condition|)
name|usage
argument_list|(
name|push_usage
argument_list|)
expr_stmt|;
return|return
name|do_push
argument_list|(
name|repo
argument_list|,
name|flags
argument_list|)
return|;
block|}
end_function
end_unit
