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
begin_define
DECL|macro|MAX_URI
define|#
directive|define
name|MAX_URI
value|(16)
end_define
begin_decl_stmt
DECL|variable|push_usage
specifier|static
specifier|const
name|char
name|push_usage
index|[]
init|=
literal|"git-push [--all] [--tags] [--exec=<git-receive-pack>] [--repo=all] [-f | --force] [-v] [<repository><refspec>...]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|all
DECL|variable|tags
DECL|variable|force
DECL|variable|thin
DECL|variable|verbose
specifier|static
name|int
name|all
decl_stmt|,
name|tags
decl_stmt|,
name|force
decl_stmt|,
name|thin
init|=
literal|1
decl_stmt|,
name|verbose
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|execute
specifier|static
specifier|const
name|char
modifier|*
name|execute
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|BUF_SIZE
define|#
directive|define
name|BUF_SIZE
value|(2084)
end_define
begin_decl_stmt
DECL|variable|buffer
specifier|static
name|char
name|buffer
index|[
name|BUF_SIZE
index|]
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
DECL|function|expand_one_ref
specifier|static
name|int
name|expand_one_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flag
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
comment|/* Ignore the "refs/" at the beginning of the refname */
name|ref
operator|+=
literal|5
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|ref
argument_list|,
literal|"tags/"
argument_list|,
literal|5
argument_list|)
condition|)
name|add_refspec
argument_list|(
name|xstrdup
argument_list|(
name|ref
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|expand_refspecs
specifier|static
name|void
name|expand_refspecs
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|all
condition|)
block|{
if|if
condition|(
name|refspec_nr
condition|)
name|die
argument_list|(
literal|"cannot mix '--all' and a refspec"
argument_list|)
expr_stmt|;
comment|/* 		 * No need to expand "--all" - we'll just use 		 * the "--all" flag to send-pack 		 */
return|return;
block|}
if|if
condition|(
operator|!
name|tags
condition|)
return|return;
name|for_each_ref
argument_list|(
name|expand_one_ref
argument_list|,
name|NULL
argument_list|)
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
if|if
condition|(
name|nr
condition|)
block|{
name|int
name|pass
decl_stmt|;
for|for
control|(
name|pass
operator|=
literal|0
init|;
name|pass
operator|<
literal|2
condition|;
name|pass
operator|++
control|)
block|{
comment|/* pass 0 counts and allocates, pass 1 fills */
name|int
name|i
decl_stmt|,
name|cnt
decl_stmt|;
for|for
control|(
name|i
operator|=
name|cnt
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
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"tag"
argument_list|,
name|refs
index|[
name|i
index|]
argument_list|)
condition|)
block|{
name|int
name|len
decl_stmt|;
name|char
modifier|*
name|tag
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
literal|"tag<tag> shorthand without<tag>"
argument_list|)
expr_stmt|;
if|if
condition|(
name|pass
condition|)
block|{
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
name|refspec
index|[
name|cnt
index|]
operator|=
name|tag
expr_stmt|;
block|}
name|cnt
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|pass
condition|)
name|refspec
index|[
name|cnt
index|]
operator|=
name|refs
index|[
name|i
index|]
expr_stmt|;
name|cnt
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|pass
condition|)
block|{
name|size_t
name|bytes
init|=
name|cnt
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
decl_stmt|;
name|refspec_nr
operator|=
name|cnt
expr_stmt|;
name|refspec
operator|=
name|xrealloc
argument_list|(
name|refspec
argument_list|,
name|bytes
argument_list|)
expr_stmt|;
block|}
block|}
block|}
name|expand_refspecs
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_remotes_uri
specifier|static
name|int
name|get_remotes_uri
parameter_list|(
specifier|const
name|char
modifier|*
name|repo
parameter_list|,
specifier|const
name|char
modifier|*
name|uri
index|[
name|MAX_URI
index|]
parameter_list|)
block|{
name|int
name|n
init|=
literal|0
decl_stmt|;
name|FILE
modifier|*
name|f
init|=
name|fopen
argument_list|(
name|git_path
argument_list|(
literal|"remotes/%s"
argument_list|,
name|repo
argument_list|)
argument_list|,
literal|"r"
argument_list|)
decl_stmt|;
name|int
name|has_explicit_refspec
init|=
name|refspec_nr
operator|||
name|all
operator|||
name|tags
decl_stmt|;
if|if
condition|(
operator|!
name|f
condition|)
return|return
operator|-
literal|1
return|;
while|while
condition|(
name|fgets
argument_list|(
name|buffer
argument_list|,
name|BUF_SIZE
argument_list|,
name|f
argument_list|)
condition|)
block|{
name|int
name|is_refspec
decl_stmt|;
name|char
modifier|*
name|s
decl_stmt|,
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
literal|"URL:"
argument_list|,
name|buffer
argument_list|,
literal|4
argument_list|)
condition|)
block|{
name|is_refspec
operator|=
literal|0
expr_stmt|;
name|s
operator|=
name|buffer
operator|+
literal|4
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
literal|"Push:"
argument_list|,
name|buffer
argument_list|,
literal|5
argument_list|)
condition|)
block|{
name|is_refspec
operator|=
literal|1
expr_stmt|;
name|s
operator|=
name|buffer
operator|+
literal|5
expr_stmt|;
block|}
else|else
continue|continue;
comment|/* Remove whitespace at the head.. */
while|while
condition|(
name|isspace
argument_list|(
operator|*
name|s
argument_list|)
condition|)
name|s
operator|++
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|s
condition|)
continue|continue;
comment|/* ..and at the end */
name|p
operator|=
name|s
operator|+
name|strlen
argument_list|(
name|s
argument_list|)
expr_stmt|;
while|while
condition|(
name|isspace
argument_list|(
name|p
index|[
operator|-
literal|1
index|]
argument_list|)
condition|)
operator|*
operator|--
name|p
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|is_refspec
condition|)
block|{
if|if
condition|(
name|n
operator|<
name|MAX_URI
condition|)
name|uri
index|[
name|n
operator|++
index|]
operator|=
name|xstrdup
argument_list|(
name|s
argument_list|)
expr_stmt|;
else|else
name|error
argument_list|(
literal|"more than %d URL's specified, ignoring the rest"
argument_list|,
name|MAX_URI
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|is_refspec
operator|&&
operator|!
name|has_explicit_refspec
condition|)
name|add_refspec
argument_list|(
name|xstrdup
argument_list|(
name|s
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|f
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|n
condition|)
name|die
argument_list|(
literal|"remote '%s' has no URL"
argument_list|,
name|repo
argument_list|)
expr_stmt|;
return|return
name|n
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|config_uri
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|config_uri
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|config_repo
specifier|static
specifier|const
name|char
modifier|*
name|config_repo
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|config_repo_len
specifier|static
name|int
name|config_repo_len
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|config_current_uri
specifier|static
name|int
name|config_current_uri
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|config_get_refspecs
specifier|static
name|int
name|config_get_refspecs
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|config_get_receivepack
specifier|static
name|int
name|config_get_receivepack
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|get_remote_config
specifier|static
name|int
name|get_remote_config
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|key
argument_list|,
literal|"remote."
argument_list|,
literal|7
argument_list|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|key
operator|+
literal|7
argument_list|,
name|config_repo
argument_list|,
name|config_repo_len
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|key
operator|+
literal|7
operator|+
name|config_repo_len
argument_list|,
literal|".url"
argument_list|)
condition|)
block|{
if|if
condition|(
name|config_current_uri
operator|<
name|MAX_URI
condition|)
name|config_uri
index|[
name|config_current_uri
operator|++
index|]
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
else|else
name|error
argument_list|(
literal|"more than %d URL's specified, ignoring the rest"
argument_list|,
name|MAX_URI
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|config_get_refspecs
operator|&&
operator|!
name|strcmp
argument_list|(
name|key
operator|+
literal|7
operator|+
name|config_repo_len
argument_list|,
literal|".push"
argument_list|)
condition|)
name|add_refspec
argument_list|(
name|xstrdup
argument_list|(
name|value
argument_list|)
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|config_get_receivepack
operator|&&
operator|!
name|strcmp
argument_list|(
name|key
operator|+
literal|7
operator|+
name|config_repo_len
argument_list|,
literal|".receivepack"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|execute
condition|)
block|{
name|char
modifier|*
name|ex
init|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|value
argument_list|)
operator|+
literal|8
argument_list|)
decl_stmt|;
name|sprintf
argument_list|(
name|ex
argument_list|,
literal|"--exec=%s"
argument_list|,
name|value
argument_list|)
expr_stmt|;
name|execute
operator|=
name|ex
expr_stmt|;
block|}
else|else
name|error
argument_list|(
literal|"more than one receivepack given, using the first"
argument_list|)
expr_stmt|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_config_remotes_uri
specifier|static
name|int
name|get_config_remotes_uri
parameter_list|(
specifier|const
name|char
modifier|*
name|repo
parameter_list|,
specifier|const
name|char
modifier|*
name|uri
index|[
name|MAX_URI
index|]
parameter_list|)
block|{
name|config_repo_len
operator|=
name|strlen
argument_list|(
name|repo
argument_list|)
expr_stmt|;
name|config_repo
operator|=
name|repo
expr_stmt|;
name|config_current_uri
operator|=
literal|0
expr_stmt|;
name|config_uri
operator|=
name|uri
expr_stmt|;
name|config_get_refspecs
operator|=
operator|!
operator|(
name|refspec_nr
operator|||
name|all
operator|||
name|tags
operator|)
expr_stmt|;
name|config_get_receivepack
operator|=
operator|(
name|execute
operator|==
name|NULL
operator|)
expr_stmt|;
name|git_config
argument_list|(
name|get_remote_config
argument_list|)
expr_stmt|;
return|return
name|config_current_uri
return|;
block|}
end_function
begin_function
DECL|function|get_branches_uri
specifier|static
name|int
name|get_branches_uri
parameter_list|(
specifier|const
name|char
modifier|*
name|repo
parameter_list|,
specifier|const
name|char
modifier|*
name|uri
index|[
name|MAX_URI
index|]
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|slash
init|=
name|strchr
argument_list|(
name|repo
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
name|int
name|n
init|=
name|slash
condition|?
name|slash
operator|-
name|repo
else|:
literal|1000
decl_stmt|;
name|FILE
modifier|*
name|f
init|=
name|fopen
argument_list|(
name|git_path
argument_list|(
literal|"branches/%.*s"
argument_list|,
name|n
argument_list|,
name|repo
argument_list|)
argument_list|,
literal|"r"
argument_list|)
decl_stmt|;
name|char
modifier|*
name|s
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|f
condition|)
return|return
literal|0
return|;
name|s
operator|=
name|fgets
argument_list|(
name|buffer
argument_list|,
name|BUF_SIZE
argument_list|,
name|f
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|f
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|s
condition|)
return|return
literal|0
return|;
while|while
condition|(
name|isspace
argument_list|(
operator|*
name|s
argument_list|)
condition|)
name|s
operator|++
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|s
condition|)
return|return
literal|0
return|;
name|p
operator|=
name|s
operator|+
name|strlen
argument_list|(
name|s
argument_list|)
expr_stmt|;
while|while
condition|(
name|isspace
argument_list|(
name|p
index|[
operator|-
literal|1
index|]
argument_list|)
condition|)
operator|*
operator|--
name|p
operator|=
literal|0
expr_stmt|;
name|len
operator|=
name|p
operator|-
name|s
expr_stmt|;
if|if
condition|(
name|slash
condition|)
name|len
operator|+=
name|strlen
argument_list|(
name|slash
argument_list|)
expr_stmt|;
name|p
operator|=
name|xmalloc
argument_list|(
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|p
argument_list|,
name|s
argument_list|)
expr_stmt|;
if|if
condition|(
name|slash
condition|)
name|strcat
argument_list|(
name|p
argument_list|,
name|slash
argument_list|)
expr_stmt|;
name|uri
index|[
literal|0
index|]
operator|=
name|p
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * Read remotes and branches file, fill the push target URI  * list.  If there is no command line refspecs, read Push: lines  * to set up the *refspec list as well.  * return the number of push target URIs  */
end_comment
begin_function
DECL|function|read_config
specifier|static
name|int
name|read_config
parameter_list|(
specifier|const
name|char
modifier|*
name|repo
parameter_list|,
specifier|const
name|char
modifier|*
name|uri
index|[
name|MAX_URI
index|]
parameter_list|)
block|{
name|int
name|n
decl_stmt|;
if|if
condition|(
operator|*
name|repo
operator|!=
literal|'/'
condition|)
block|{
name|n
operator|=
name|get_remotes_uri
argument_list|(
name|repo
argument_list|,
name|uri
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|>
literal|0
condition|)
return|return
name|n
return|;
name|n
operator|=
name|get_config_remotes_uri
argument_list|(
name|repo
argument_list|,
name|uri
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|>
literal|0
condition|)
return|return
name|n
return|;
name|n
operator|=
name|get_branches_uri
argument_list|(
name|repo
argument_list|,
name|uri
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|>
literal|0
condition|)
return|return
name|n
return|;
block|}
name|uri
index|[
literal|0
index|]
operator|=
name|repo
expr_stmt|;
return|return
literal|1
return|;
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
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|uri
index|[
name|MAX_URI
index|]
decl_stmt|;
name|int
name|i
decl_stmt|,
name|n
decl_stmt|;
name|int
name|common_argc
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|argv
decl_stmt|;
name|int
name|argc
decl_stmt|;
name|n
operator|=
name|read_config
argument_list|(
name|repo
argument_list|,
name|uri
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|<=
literal|0
condition|)
name|die
argument_list|(
literal|"bad repository '%s'"
argument_list|,
name|repo
argument_list|)
expr_stmt|;
name|argv
operator|=
name|xmalloc
argument_list|(
operator|(
name|refspec_nr
operator|+
literal|10
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
literal|"dummy-send-pack"
expr_stmt|;
name|argc
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|all
condition|)
name|argv
index|[
name|argc
operator|++
index|]
operator|=
literal|"--all"
expr_stmt|;
if|if
condition|(
name|force
condition|)
name|argv
index|[
name|argc
operator|++
index|]
operator|=
literal|"--force"
expr_stmt|;
if|if
condition|(
name|execute
condition|)
name|argv
index|[
name|argc
operator|++
index|]
operator|=
name|execute
expr_stmt|;
name|common_argc
operator|=
name|argc
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
name|int
name|err
decl_stmt|;
name|int
name|dest_argc
init|=
name|common_argc
decl_stmt|;
name|int
name|dest_refspec_nr
init|=
name|refspec_nr
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|dest_refspec
init|=
name|refspec
decl_stmt|;
specifier|const
name|char
modifier|*
name|dest
init|=
name|uri
index|[
name|i
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|sender
init|=
literal|"git-send-pack"
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|dest
argument_list|,
literal|"http://"
argument_list|,
literal|7
argument_list|)
operator|||
operator|!
name|strncmp
argument_list|(
name|dest
argument_list|,
literal|"https://"
argument_list|,
literal|8
argument_list|)
condition|)
name|sender
operator|=
literal|"git-http-push"
expr_stmt|;
elseif|else
if|if
condition|(
name|thin
condition|)
name|argv
index|[
name|dest_argc
operator|++
index|]
operator|=
literal|"--thin"
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|sender
expr_stmt|;
name|argv
index|[
name|dest_argc
operator|++
index|]
operator|=
name|dest
expr_stmt|;
while|while
condition|(
name|dest_refspec_nr
operator|--
condition|)
name|argv
index|[
name|dest_argc
operator|++
index|]
operator|=
operator|*
name|dest_refspec
operator|++
expr_stmt|;
name|argv
index|[
name|dest_argc
index|]
operator|=
name|NULL
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
name|dest
argument_list|)
expr_stmt|;
name|err
operator|=
name|run_command_v
argument_list|(
name|argv
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|err
condition|)
continue|continue;
switch|switch
condition|(
name|err
condition|)
block|{
case|case
operator|-
name|ERR_RUN_COMMAND_FORK
case|:
name|die
argument_list|(
literal|"unable to fork for %s"
argument_list|,
name|sender
argument_list|)
expr_stmt|;
case|case
operator|-
name|ERR_RUN_COMMAND_EXEC
case|:
name|die
argument_list|(
literal|"unable to exec %s"
argument_list|,
name|sender
argument_list|)
expr_stmt|;
case|case
operator|-
name|ERR_RUN_COMMAND_WAITPID
case|:
case|case
operator|-
name|ERR_RUN_COMMAND_WAITPID_WRONG_PID
case|:
case|case
operator|-
name|ERR_RUN_COMMAND_WAITPID_SIGNAL
case|:
case|case
operator|-
name|ERR_RUN_COMMAND_WAITPID_NOEXIT
case|:
name|die
argument_list|(
literal|"%s died with strange error"
argument_list|,
name|sender
argument_list|)
expr_stmt|;
default|default:
return|return
operator|-
name|err
return|;
block|}
block|}
return|return
literal|0
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
specifier|const
name|char
modifier|*
name|repo
init|=
literal|"origin"
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
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--repo="
argument_list|,
literal|7
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
name|all
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
literal|"--tags"
argument_list|)
condition|)
block|{
name|tags
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
name|force
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
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--exec="
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|execute
operator|=
name|arg
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
return|return
name|do_push
argument_list|(
name|repo
argument_list|)
return|;
block|}
end_function
end_unit
