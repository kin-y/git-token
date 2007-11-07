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
literal|"git-ls-remote [--upload-pack=<git-upload-pack>] [<host>:]<directory>"
decl_stmt|;
end_decl_stmt
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
name|int
name|nongit
init|=
literal|0
decl_stmt|;
name|unsigned
name|flags
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
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
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
operator|!
name|prefixcmp
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
operator|!
name|prefixcmp
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
break|break;
block|}
if|if
condition|(
operator|!
name|dest
operator|||
name|i
operator|!=
name|argc
operator|-
literal|1
condition|)
name|usage
argument_list|(
name|ls_remote_usage
argument_list|)
expr_stmt|;
name|remote
operator|=
name|nongit
condition|?
name|NULL
else|:
name|remote_get
argument_list|(
name|dest
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote
operator|&&
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
name|transport
operator|=
name|transport_get
argument_list|(
name|remote
argument_list|,
name|remote
condition|?
name|remote
operator|->
name|url
index|[
literal|0
index|]
else|:
name|dest
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
operator|!
name|ref
condition|)
return|return
literal|1
return|;
while|while
condition|(
name|ref
condition|)
block|{
if|if
condition|(
name|check_ref_type
argument_list|(
name|ref
argument_list|,
name|flags
argument_list|)
condition|)
name|printf
argument_list|(
literal|"%s	%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
