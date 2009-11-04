begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"walker.h"
end_include
begin_include
include|#
directive|include
file|"http.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_function
DECL|function|get_refs
specifier|static
name|struct
name|ref
modifier|*
name|get_refs
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
specifier|const
name|char
modifier|*
name|url
parameter_list|)
block|{
name|struct
name|strbuf
name|buffer
init|=
name|STRBUF_INIT
decl_stmt|;
name|char
modifier|*
name|data
decl_stmt|,
modifier|*
name|start
decl_stmt|,
modifier|*
name|mid
decl_stmt|;
name|char
modifier|*
name|ref_name
decl_stmt|;
name|char
modifier|*
name|refs_url
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
name|int
name|http_ret
decl_stmt|;
name|struct
name|ref
modifier|*
name|refs
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
name|ref
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
name|last_ref
init|=
name|NULL
decl_stmt|;
name|refs_url
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|url
argument_list|)
operator|+
literal|11
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|refs_url
argument_list|,
literal|"%s/info/refs"
argument_list|,
name|url
argument_list|)
expr_stmt|;
name|http_ret
operator|=
name|http_get_strbuf
argument_list|(
name|refs_url
argument_list|,
operator|&
name|buffer
argument_list|,
name|HTTP_NO_CACHE
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|http_ret
condition|)
block|{
case|case
name|HTTP_OK
case|:
break|break;
case|case
name|HTTP_MISSING_TARGET
case|:
name|die
argument_list|(
literal|"%s not found: did you run git update-server-info on the"
literal|" server?"
argument_list|,
name|refs_url
argument_list|)
expr_stmt|;
default|default:
name|http_error
argument_list|(
name|refs_url
argument_list|,
name|http_ret
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"HTTP request failed"
argument_list|)
expr_stmt|;
block|}
name|data
operator|=
name|buffer
operator|.
name|buf
expr_stmt|;
name|start
operator|=
name|NULL
expr_stmt|;
name|mid
operator|=
name|data
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|buffer
operator|.
name|len
condition|)
block|{
if|if
condition|(
operator|!
name|start
condition|)
block|{
name|start
operator|=
operator|&
name|data
index|[
name|i
index|]
expr_stmt|;
block|}
if|if
condition|(
name|data
index|[
name|i
index|]
operator|==
literal|'\t'
condition|)
name|mid
operator|=
operator|&
name|data
index|[
name|i
index|]
expr_stmt|;
if|if
condition|(
name|data
index|[
name|i
index|]
operator|==
literal|'\n'
condition|)
block|{
name|data
index|[
name|i
index|]
operator|=
literal|0
expr_stmt|;
name|ref_name
operator|=
name|mid
operator|+
literal|1
expr_stmt|;
name|ref
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|ref
argument_list|)
operator|+
name|strlen
argument_list|(
name|ref_name
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|ref
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|ref
argument_list|)
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ref
operator|->
name|name
argument_list|,
name|ref_name
argument_list|)
expr_stmt|;
name|get_sha1_hex
argument_list|(
name|start
argument_list|,
name|ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|refs
condition|)
name|refs
operator|=
name|ref
expr_stmt|;
if|if
condition|(
name|last_ref
condition|)
name|last_ref
operator|->
name|next
operator|=
name|ref
expr_stmt|;
name|last_ref
operator|=
name|ref
expr_stmt|;
name|start
operator|=
name|NULL
expr_stmt|;
block|}
name|i
operator|++
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
name|ref
operator|=
name|alloc_ref
argument_list|(
literal|"HEAD"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|walker
operator|->
name|fetch_ref
argument_list|(
name|walker
argument_list|,
name|ref
argument_list|)
operator|&&
operator|!
name|resolve_remote_symref
argument_list|(
name|ref
argument_list|,
name|refs
argument_list|)
condition|)
block|{
name|ref
operator|->
name|next
operator|=
name|refs
expr_stmt|;
name|refs
operator|=
name|ref
expr_stmt|;
block|}
else|else
block|{
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|refs_url
argument_list|)
expr_stmt|;
return|return
name|refs
return|;
block|}
end_function
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
name|remote
modifier|*
name|remote
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|url
decl_stmt|;
name|struct
name|walker
modifier|*
name|walker
init|=
name|NULL
decl_stmt|;
name|int
name|nongit
decl_stmt|;
name|git_extract_argv0_path
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Remote needed\n"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|remote
operator|=
name|remote_get
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|2
condition|)
block|{
name|url
operator|=
name|argv
index|[
literal|2
index|]
expr_stmt|;
block|}
else|else
block|{
name|url
operator|=
name|remote
operator|->
name|url
index|[
literal|0
index|]
expr_stmt|;
block|}
do|do
block|{
if|if
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|buf
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|==
name|EOF
condition|)
break|break;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
literal|"fetch "
argument_list|)
condition|)
block|{
name|char
modifier|*
name|obj
init|=
name|buf
operator|.
name|buf
operator|+
name|strlen
argument_list|(
literal|"fetch "
argument_list|)
decl_stmt|;
if|if
condition|(
name|nongit
condition|)
name|die
argument_list|(
literal|"Fetch attempted without a local repo"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|walker
condition|)
name|walker
operator|=
name|get_http_walker
argument_list|(
name|url
argument_list|,
name|remote
argument_list|)
expr_stmt|;
name|walker
operator|->
name|get_all
operator|=
literal|1
expr_stmt|;
name|walker
operator|->
name|get_tree
operator|=
literal|1
expr_stmt|;
name|walker
operator|->
name|get_history
operator|=
literal|1
expr_stmt|;
name|walker
operator|->
name|get_verbosely
operator|=
literal|0
expr_stmt|;
name|walker
operator|->
name|get_recover
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|walker_fetch
argument_list|(
name|walker
argument_list|,
literal|1
argument_list|,
operator|&
name|obj
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
condition|)
name|die
argument_list|(
literal|"Fetch failed."
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
literal|"list"
argument_list|)
condition|)
block|{
name|struct
name|ref
modifier|*
name|refs
decl_stmt|;
name|struct
name|ref
modifier|*
name|posn
decl_stmt|;
if|if
condition|(
operator|!
name|walker
condition|)
name|walker
operator|=
name|get_http_walker
argument_list|(
name|url
argument_list|,
name|remote
argument_list|)
expr_stmt|;
name|refs
operator|=
name|get_refs
argument_list|(
name|walker
argument_list|,
name|url
argument_list|)
expr_stmt|;
for|for
control|(
name|posn
operator|=
name|refs
init|;
name|posn
condition|;
name|posn
operator|=
name|posn
operator|->
name|next
control|)
block|{
if|if
condition|(
name|posn
operator|->
name|symref
condition|)
name|printf
argument_list|(
literal|"@%s %s\n"
argument_list|,
name|posn
operator|->
name|symref
argument_list|,
name|posn
operator|->
name|name
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|posn
operator|->
name|old_sha1
argument_list|)
argument_list|,
name|posn
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
literal|"capabilities"
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"fetch\n"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
else|else
block|{
return|return
literal|1
return|;
block|}
name|strbuf_reset
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
literal|1
condition|)
do|;
return|return
literal|0
return|;
block|}
end_function
end_unit
