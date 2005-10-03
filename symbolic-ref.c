begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|git_symbolic_ref_usage
specifier|static
specifier|const
name|char
name|git_symbolic_ref_usage
index|[]
init|=
literal|"git-symbolic-ref name [ref]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|check_symref
specifier|static
name|int
name|check_symref
parameter_list|(
specifier|const
name|char
modifier|*
name|HEAD
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|git_HEAD
init|=
name|strdup
argument_list|(
name|git_path
argument_list|(
literal|"%s"
argument_list|,
name|HEAD
argument_list|)
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|git_refs_heads_master
init|=
name|resolve_ref
argument_list|(
name|git_HEAD
argument_list|,
name|sha1
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
name|git_refs_heads_master
condition|)
block|{
comment|/* we want to strip the .git/ part */
name|int
name|pfxlen
init|=
name|strlen
argument_list|(
name|git_HEAD
argument_list|)
operator|-
name|strlen
argument_list|(
name|HEAD
argument_list|)
decl_stmt|;
name|puts
argument_list|(
name|git_refs_heads_master
operator|+
name|pfxlen
argument_list|)
expr_stmt|;
block|}
else|else
name|die
argument_list|(
literal|"No such ref: %s"
argument_list|,
name|HEAD
argument_list|)
expr_stmt|;
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
name|setup_git_directory
argument_list|()
expr_stmt|;
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|2
case|:
name|check_symref
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
break|break;
case|case
literal|3
case|:
name|create_symref
argument_list|(
name|strdup
argument_list|(
name|git_path
argument_list|(
literal|"%s"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
argument_list|)
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
break|break;
default|default:
name|usage
argument_list|(
name|git_symbolic_ref_usage
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
