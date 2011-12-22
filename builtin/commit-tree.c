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
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_decl_stmt
DECL|variable|commit_tree_usage
specifier|static
specifier|const
name|char
name|commit_tree_usage
index|[]
init|=
literal|"git commit-tree [(-p<sha1>)...] [-m<message>] [-F<file>]<sha1><changelog"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|new_parent
specifier|static
name|void
name|new_parent
parameter_list|(
name|struct
name|commit
modifier|*
name|parent
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|parents_p
parameter_list|)
block|{
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|parent
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
for|for
control|(
name|parents
operator|=
operator|*
name|parents_p
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
control|)
block|{
if|if
condition|(
name|parents
operator|->
name|item
operator|==
name|parent
condition|)
block|{
name|error
argument_list|(
literal|"duplicate parent %s ignored"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return;
block|}
name|parents_p
operator|=
operator|&
name|parents
operator|->
name|next
expr_stmt|;
block|}
name|commit_list_insert
argument_list|(
name|parent
argument_list|,
name|parents_p
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_commit_tree
name|int
name|cmd_commit_tree
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
decl_stmt|,
name|got_tree
init|=
literal|0
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|NULL
decl_stmt|;
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|commit_sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|strbuf
name|buffer
init|=
name|STRBUF_INIT
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
operator|||
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-h"
argument_list|)
condition|)
name|usage
argument_list|(
name|commit_tree_usage
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
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-p"
argument_list|)
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|argc
operator|<=
operator|++
name|i
condition|)
name|usage
argument_list|(
name|commit_tree_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name %s"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|assert_sha1_type
argument_list|(
name|sha1
argument_list|,
name|OBJ_COMMIT
argument_list|)
expr_stmt|;
name|new_parent
argument_list|(
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
argument_list|,
operator|&
name|parents
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
literal|"-m"
argument_list|)
condition|)
block|{
if|if
condition|(
name|argc
operator|<=
operator|++
name|i
condition|)
name|usage
argument_list|(
name|commit_tree_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer
operator|.
name|len
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|buffer
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buffer
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|strbuf_complete_line
argument_list|(
operator|&
name|buffer
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
literal|"-F"
argument_list|)
condition|)
block|{
name|int
name|fd
decl_stmt|;
if|if
condition|(
name|argc
operator|<=
operator|++
name|i
condition|)
name|usage
argument_list|(
name|commit_tree_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer
operator|.
name|len
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|buffer
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"-"
argument_list|)
condition|)
name|fd
operator|=
literal|0
expr_stmt|;
else|else
block|{
name|fd
operator|=
name|open
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"git commit-tree: failed to open '%s'"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|strbuf_read
argument_list|(
operator|&
name|buffer
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"git commit-tree: failed to read '%s'"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|&&
name|close
argument_list|(
name|fd
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"git commit-tree: failed to close '%s'"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|strbuf_complete_line
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|tree_sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
if|if
condition|(
name|got_tree
condition|)
name|die
argument_list|(
literal|"Cannot give more than one trees"
argument_list|)
expr_stmt|;
name|got_tree
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|buffer
operator|.
name|len
condition|)
block|{
if|if
condition|(
name|strbuf_read
argument_list|(
operator|&
name|buffer
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"git commit-tree: failed to read"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|commit_tree
argument_list|(
operator|&
name|buffer
argument_list|,
name|tree_sha1
argument_list|,
name|parents
argument_list|,
name|commit_sha1
argument_list|,
name|NULL
argument_list|)
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
