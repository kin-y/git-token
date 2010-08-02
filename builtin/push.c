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
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|push_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|push_usage
index|[]
init|=
block|{
literal|"git push [<options>] [<repository> [<refspec>...]]"
block|,
name|NULL
block|, }
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|thin
specifier|static
name|int
name|thin
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|deleterefs
specifier|static
name|int
name|deleterefs
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
DECL|variable|verbosity
specifier|static
name|int
name|verbosity
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|progress
specifier|static
name|int
name|progress
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
if|if
condition|(
name|deleterefs
condition|)
block|{
name|tag
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
name|tag
argument_list|,
literal|":refs/tags/"
argument_list|)
expr_stmt|;
block|}
else|else
block|{
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
block|}
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
elseif|else
if|if
condition|(
name|deleterefs
operator|&&
operator|!
name|strchr
argument_list|(
name|ref
argument_list|,
literal|':'
argument_list|)
condition|)
block|{
name|char
modifier|*
name|delref
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|ref
argument_list|)
operator|+
literal|1
decl_stmt|;
name|delref
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
name|delref
argument_list|,
literal|":"
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|delref
argument_list|,
name|ref
argument_list|)
expr_stmt|;
name|ref
operator|=
name|delref
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|deleterefs
condition|)
name|die
argument_list|(
literal|"--delete only accepts plain target ref names"
argument_list|)
expr_stmt|;
name|add_refspec
argument_list|(
name|ref
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|setup_push_tracking
specifier|static
name|void
name|setup_push_tracking
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|refspec
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|branch
modifier|*
name|branch
init|=
name|branch_get
argument_list|(
name|NULL
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|branch
condition|)
name|die
argument_list|(
literal|"You are not currently on a branch."
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|branch
operator|->
name|merge_nr
operator|||
operator|!
name|branch
operator|->
name|merge
condition|)
name|die
argument_list|(
literal|"The current branch %s is not tracking anything."
argument_list|,
name|branch
operator|->
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|branch
operator|->
name|merge_nr
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"The current branch %s is tracking multiple branches, "
literal|"refusing to push."
argument_list|,
name|branch
operator|->
name|name
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|refspec
argument_list|,
literal|"%s:%s"
argument_list|,
name|branch
operator|->
name|name
argument_list|,
name|branch
operator|->
name|merge
index|[
literal|0
index|]
operator|->
name|src
argument_list|)
expr_stmt|;
name|add_refspec
argument_list|(
name|refspec
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|setup_default_push_refspecs
specifier|static
name|void
name|setup_default_push_refspecs
parameter_list|(
name|void
parameter_list|)
block|{
switch|switch
condition|(
name|push_default
condition|)
block|{
default|default:
case|case
name|PUSH_DEFAULT_MATCHING
case|:
name|add_refspec
argument_list|(
literal|":"
argument_list|)
expr_stmt|;
break|break;
case|case
name|PUSH_DEFAULT_TRACKING
case|:
name|setup_push_tracking
argument_list|()
expr_stmt|;
break|break;
case|case
name|PUSH_DEFAULT_CURRENT
case|:
name|add_refspec
argument_list|(
literal|"HEAD"
argument_list|)
expr_stmt|;
break|break;
case|case
name|PUSH_DEFAULT_NOTHING
case|:
name|die
argument_list|(
literal|"You didn't specify any refspecs to push, and "
literal|"push.default is \"nothing\"."
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
end_function
begin_function
DECL|function|push_with_options
specifier|static
name|int
name|push_with_options
parameter_list|(
name|struct
name|transport
modifier|*
name|transport
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
name|int
name|err
decl_stmt|;
name|int
name|nonfastforward
decl_stmt|;
name|transport_set_verbosity
argument_list|(
name|transport
argument_list|,
name|verbosity
argument_list|,
name|progress
argument_list|)
expr_stmt|;
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
name|verbosity
operator|>
literal|0
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Pushing to %s\n"
argument_list|,
name|transport
operator|->
name|url
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
argument_list|,
operator|&
name|nonfastforward
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
operator|!=
literal|0
condition|)
name|error
argument_list|(
literal|"failed to push some refs to '%s'"
argument_list|,
name|transport
operator|->
name|url
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
return|return
literal|0
return|;
if|if
condition|(
name|nonfastforward
operator|&&
name|advice_push_nonfastforward
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"To prevent you from losing history, non-fast-forward updates were rejected\n"
literal|"Merge the remote changes (e.g. 'git pull') before pushing again.  See the\n"
literal|"'Note about fast-forwards' section of 'git push --help' for details.\n"
argument_list|)
expr_stmt|;
block|}
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
specifier|const
name|char
modifier|*
modifier|*
name|url
decl_stmt|;
name|int
name|url_nr
decl_stmt|;
if|if
condition|(
operator|!
name|remote
condition|)
block|{
if|if
condition|(
name|repo
condition|)
name|die
argument_list|(
literal|"bad repository '%s'"
argument_list|,
name|repo
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"No destination configured to push to."
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|remote
operator|->
name|mirror
condition|)
name|flags
operator||=
operator|(
name|TRANSPORT_PUSH_MIRROR
operator||
name|TRANSPORT_PUSH_FORCE
operator|)
expr_stmt|;
if|if
condition|(
operator|(
name|flags
operator|&
name|TRANSPORT_PUSH_ALL
operator|)
operator|&&
name|refspec
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
operator|*
name|refspec
argument_list|,
literal|"refs/tags/*"
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"--all and --tags are incompatible"
argument_list|)
return|;
return|return
name|error
argument_list|(
literal|"--all can't be combined with refspecs"
argument_list|)
return|;
block|}
if|if
condition|(
operator|(
name|flags
operator|&
name|TRANSPORT_PUSH_MIRROR
operator|)
operator|&&
name|refspec
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
operator|*
name|refspec
argument_list|,
literal|"refs/tags/*"
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"--mirror and --tags are incompatible"
argument_list|)
return|;
return|return
name|error
argument_list|(
literal|"--mirror can't be combined with refspecs"
argument_list|)
return|;
block|}
if|if
condition|(
operator|(
name|flags
operator|&
operator|(
name|TRANSPORT_PUSH_ALL
operator||
name|TRANSPORT_PUSH_MIRROR
operator|)
operator|)
operator|==
operator|(
name|TRANSPORT_PUSH_ALL
operator||
name|TRANSPORT_PUSH_MIRROR
operator|)
condition|)
block|{
return|return
name|error
argument_list|(
literal|"--all and --mirror are incompatible"
argument_list|)
return|;
block|}
if|if
condition|(
operator|!
name|refspec
operator|&&
operator|!
operator|(
name|flags
operator|&
name|TRANSPORT_PUSH_ALL
operator|)
condition|)
block|{
if|if
condition|(
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
elseif|else
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|TRANSPORT_PUSH_MIRROR
operator|)
condition|)
name|setup_default_push_refspecs
argument_list|()
expr_stmt|;
block|}
name|errs
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|remote
operator|->
name|pushurl_nr
condition|)
block|{
name|url
operator|=
name|remote
operator|->
name|pushurl
expr_stmt|;
name|url_nr
operator|=
name|remote
operator|->
name|pushurl_nr
expr_stmt|;
block|}
else|else
block|{
name|url
operator|=
name|remote
operator|->
name|url
expr_stmt|;
name|url_nr
operator|=
name|remote
operator|->
name|url_nr
expr_stmt|;
block|}
if|if
condition|(
name|url_nr
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|url_nr
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
name|url
index|[
name|i
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
name|push_with_options
argument_list|(
name|transport
argument_list|,
name|flags
argument_list|)
condition|)
name|errs
operator|++
expr_stmt|;
block|}
block|}
else|else
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
name|NULL
argument_list|)
decl_stmt|;
if|if
condition|(
name|push_with_options
argument_list|(
name|transport
argument_list|,
name|flags
argument_list|)
condition|)
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
name|flags
init|=
literal|0
decl_stmt|;
name|int
name|tags
init|=
literal|0
decl_stmt|;
name|int
name|rc
decl_stmt|;
specifier|const
name|char
modifier|*
name|repo
init|=
name|NULL
decl_stmt|;
comment|/* default repository */
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__VERBOSITY
argument_list|(
operator|&
name|verbosity
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"repo"
argument_list|,
operator|&
name|repo
argument_list|,
literal|"repository"
argument_list|,
literal|"repository"
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"all"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"push all refs"
argument_list|,
name|TRANSPORT_PUSH_ALL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"mirror"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"mirror all refs"
argument_list|,
operator|(
name|TRANSPORT_PUSH_MIRROR
operator||
name|TRANSPORT_PUSH_FORCE
operator|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"delete"
argument_list|,
operator|&
name|deleterefs
argument_list|,
literal|"delete refs"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"tags"
argument_list|,
operator|&
name|tags
argument_list|,
literal|"push tags (can't be used with --all or --mirror)"
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'n'
argument_list|,
literal|"dry-run"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"dry run"
argument_list|,
name|TRANSPORT_PUSH_DRY_RUN
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"porcelain"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"machine-readable output"
argument_list|,
name|TRANSPORT_PUSH_PORCELAIN
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'f'
argument_list|,
literal|"force"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"force updates"
argument_list|,
name|TRANSPORT_PUSH_FORCE
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"thin"
argument_list|,
operator|&
name|thin
argument_list|,
literal|"use thin pack"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"receive-pack"
argument_list|,
operator|&
name|receivepack
argument_list|,
literal|"receive-pack"
argument_list|,
literal|"receive pack program"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"exec"
argument_list|,
operator|&
name|receivepack
argument_list|,
literal|"receive-pack"
argument_list|,
literal|"receive pack program"
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'u'
argument_list|,
literal|"set-upstream"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"set upstream for git pull/status"
argument_list|,
name|TRANSPORT_PUSH_SET_UPSTREAM
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"progress"
argument_list|,
operator|&
name|progress
argument_list|,
literal|"force progress reporting"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|push_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|deleterefs
operator|&&
operator|(
name|tags
operator|||
operator|(
name|flags
operator|&
operator|(
name|TRANSPORT_PUSH_ALL
operator||
name|TRANSPORT_PUSH_MIRROR
operator|)
operator|)
operator|)
condition|)
name|die
argument_list|(
literal|"--delete is incompatible with --all, --mirror and --tags"
argument_list|)
expr_stmt|;
if|if
condition|(
name|deleterefs
operator|&&
name|argc
operator|<
literal|2
condition|)
name|die
argument_list|(
literal|"--delete doesn't make sense without any refs"
argument_list|)
expr_stmt|;
if|if
condition|(
name|tags
condition|)
name|add_refspec
argument_list|(
literal|"refs/tags/*"
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|0
condition|)
block|{
name|repo
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
name|set_refspecs
argument_list|(
name|argv
operator|+
literal|1
argument_list|,
name|argc
operator|-
literal|1
argument_list|)
expr_stmt|;
block|}
name|rc
operator|=
name|do_push
argument_list|(
name|repo
argument_list|,
name|flags
argument_list|)
expr_stmt|;
if|if
condition|(
name|rc
operator|==
operator|-
literal|1
condition|)
name|usage_with_options
argument_list|(
name|push_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
else|else
return|return
name|rc
return|;
block|}
end_function
end_unit
