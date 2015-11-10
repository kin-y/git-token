begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"transport.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_decl_stmt
DECL|variable|ls_remote_usage
specifier|static
specifier|const
name|char
name|ls_remote_usage
index|[]
init|=
literal|"git ls-remote [--heads] [--tags]  [--upload-pack=<exec>]\n"
literal|"                     [-q | --quiet] [--exit-code] [--get-url] [<repository> [<refs>...]]"
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * Is there one among the list of patterns that match the tail part  * of the path?  */
end_comment
begin_function
DECL|function|tail_match
specifier|static
name|int
name|tail_match
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|pattern
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
decl_stmt|;
name|char
name|pathbuf
index|[
name|PATH_MAX
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|pattern
condition|)
return|return
literal|1
return|;
comment|/* no restriction */
if|if
condition|(
name|snprintf
argument_list|(
name|pathbuf
argument_list|,
sizeof|sizeof
argument_list|(
name|pathbuf
argument_list|)
argument_list|,
literal|"/%s"
argument_list|,
name|path
argument_list|)
operator|>
sizeof|sizeof
argument_list|(
name|pathbuf
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"insanely long ref %.*s..."
argument_list|,
literal|20
argument_list|,
name|path
argument_list|)
return|;
while|while
condition|(
operator|(
name|p
operator|=
operator|*
operator|(
name|pattern
operator|++
operator|)
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
operator|!
name|wildmatch
argument_list|(
name|p
argument_list|,
name|pathbuf
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
condition|)
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_ls_remote
name|int
name|cmd_ls_remote
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
name|dest
init|=
name|NULL
decl_stmt|;
name|unsigned
name|flags
init|=
literal|0
decl_stmt|;
name|int
name|get_url
init|=
literal|0
decl_stmt|;
name|int
name|quiet
init|=
literal|0
decl_stmt|;
name|int
name|status
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|uploadpack
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|pattern
init|=
name|NULL
decl_stmt|;
name|struct
name|remote
modifier|*
name|remote
decl_stmt|;
name|struct
name|transport
modifier|*
name|transport
decl_stmt|;
specifier|const
name|struct
name|ref
modifier|*
name|ref
decl_stmt|;
if|if
condition|(
name|argc
operator|==
literal|2
operator|&&
operator|!
name|strcmp
argument_list|(
literal|"-h"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
condition|)
name|usage
argument_list|(
name|ls_remote_usage
argument_list|)
expr_stmt|;
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
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|starts_with
argument_list|(
name|arg
argument_list|,
literal|"--upload-pack="
argument_list|)
condition|)
block|{
name|uploadpack
operator|=
name|arg
operator|+
literal|14
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|starts_with
argument_list|(
name|arg
argument_list|,
literal|"--exec="
argument_list|)
condition|)
block|{
name|uploadpack
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
literal|"--tags"
argument_list|,
name|arg
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
literal|"-t"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|flags
operator||=
name|REF_TAGS
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"--heads"
argument_list|,
name|arg
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
literal|"-h"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|flags
operator||=
name|REF_HEADS
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"--refs"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|flags
operator||=
name|REF_NORMAL
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"--quiet"
argument_list|,
name|arg
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
literal|"-q"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|quiet
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
literal|"--get-url"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|get_url
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
literal|"--exit-code"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
comment|/* return this code if no refs are reported */
name|status
operator|=
literal|2
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|ls_remote_usage
argument_list|)
expr_stmt|;
block|}
name|dest
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
name|argv
index|[
name|i
index|]
condition|)
block|{
name|int
name|j
decl_stmt|;
name|pattern
operator|=
name|xcalloc
argument_list|(
name|argc
operator|-
name|i
operator|+
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
specifier|const
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
name|i
init|;
name|j
operator|<
name|argc
condition|;
name|j
operator|++
control|)
name|pattern
index|[
name|j
operator|-
name|i
index|]
operator|=
name|xstrfmt
argument_list|(
literal|"*/%s"
argument_list|,
name|argv
index|[
name|j
index|]
argument_list|)
expr_stmt|;
block|}
name|remote
operator|=
name|remote_get
argument_list|(
name|dest
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|remote
condition|)
block|{
if|if
condition|(
name|dest
condition|)
name|die
argument_list|(
literal|"bad repository '%s'"
argument_list|,
name|dest
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"No remote configured to list refs from."
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|remote
operator|->
name|url_nr
condition|)
name|die
argument_list|(
literal|"remote %s has no configured URL"
argument_list|,
name|dest
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_url
condition|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
operator|*
name|remote
operator|->
name|url
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|transport
operator|=
name|transport_get
argument_list|(
name|remote
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|uploadpack
operator|!=
name|NULL
condition|)
name|transport_set_option
argument_list|(
name|transport
argument_list|,
name|TRANS_OPT_UPLOADPACK
argument_list|,
name|uploadpack
argument_list|)
expr_stmt|;
name|ref
operator|=
name|transport_get_remote_refs
argument_list|(
name|transport
argument_list|)
expr_stmt|;
if|if
condition|(
name|transport_disconnect
argument_list|(
name|transport
argument_list|)
condition|)
return|return
literal|1
return|;
if|if
condition|(
operator|!
name|dest
operator|&&
operator|!
name|quiet
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"From %s\n"
argument_list|,
operator|*
name|remote
operator|->
name|url
argument_list|)
expr_stmt|;
for|for
control|(
init|;
name|ref
condition|;
name|ref
operator|=
name|ref
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|check_ref_type
argument_list|(
name|ref
argument_list|,
name|flags
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|tail_match
argument_list|(
name|pattern
argument_list|,
name|ref
operator|->
name|name
argument_list|)
condition|)
continue|continue;
name|printf
argument_list|(
literal|"%s	%s\n"
argument_list|,
name|oid_to_hex
argument_list|(
operator|&
name|ref
operator|->
name|old_oid
argument_list|)
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|status
operator|=
literal|0
expr_stmt|;
comment|/* we found something */
block|}
return|return
name|status
return|;
block|}
end_function
end_unit
