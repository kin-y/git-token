begin_unit
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
file|"log-tree.h"
end_include
begin_function
DECL|function|log_tree_diff_flush
name|int
name|log_tree_diff_flush
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|)
block|{
name|diffcore_std
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
if|if
condition|(
name|diff_queue_is_empty
argument_list|()
condition|)
block|{
name|int
name|saved_fmt
init|=
name|opt
operator|->
name|diffopt
operator|.
name|output_format
decl_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_NO_OUTPUT
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|saved_fmt
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|opt
operator|->
name|header
condition|)
block|{
if|if
condition|(
operator|!
name|opt
operator|->
name|no_commit_id
condition|)
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|opt
operator|->
name|header
argument_list|,
name|opt
operator|->
name|diffopt
operator|.
name|line_termination
argument_list|)
expr_stmt|;
name|opt
operator|->
name|header
operator|=
name|NULL
expr_stmt|;
block|}
name|diff_flush
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|diff_root_tree
specifier|static
name|int
name|diff_root_tree
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|int
name|retval
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|struct
name|tree_desc
name|empty
decl_stmt|,
name|real
decl_stmt|;
name|tree
operator|=
name|read_object_with_reference
argument_list|(
name|new
argument_list|,
name|tree_type
argument_list|,
operator|&
name|real
operator|.
name|size
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
name|die
argument_list|(
literal|"unable to read root tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|new
argument_list|)
argument_list|)
expr_stmt|;
name|real
operator|.
name|buf
operator|=
name|tree
expr_stmt|;
name|empty
operator|.
name|buf
operator|=
literal|""
expr_stmt|;
name|empty
operator|.
name|size
operator|=
literal|0
expr_stmt|;
name|retval
operator|=
name|diff_tree
argument_list|(
operator|&
name|empty
argument_list|,
operator|&
name|real
argument_list|,
name|base
argument_list|,
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|opt
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
begin_function
DECL|function|generate_header
specifier|static
specifier|const
name|char
modifier|*
name|generate_header
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|parent_sha1
parameter_list|,
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
specifier|static
name|char
name|this_header
index|[
literal|16384
index|]
decl_stmt|;
name|int
name|offset
decl_stmt|;
name|unsigned
name|long
name|len
decl_stmt|;
name|int
name|abbrev
init|=
name|opt
operator|->
name|diffopt
operator|.
name|abbrev
decl_stmt|;
specifier|const
name|char
modifier|*
name|msg
init|=
name|commit
operator|->
name|buffer
decl_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|verbose_header
condition|)
return|return
name|sha1_to_hex
argument_list|(
name|commit_sha1
argument_list|)
return|;
name|len
operator|=
name|strlen
argument_list|(
name|msg
argument_list|)
expr_stmt|;
name|offset
operator|=
name|sprintf
argument_list|(
name|this_header
argument_list|,
literal|"%s%s "
argument_list|,
name|opt
operator|->
name|header_prefix
argument_list|,
name|diff_unique_abbrev
argument_list|(
name|commit_sha1
argument_list|,
name|abbrev
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit_sha1
operator|!=
name|parent_sha1
condition|)
name|offset
operator|+=
name|sprintf
argument_list|(
name|this_header
operator|+
name|offset
argument_list|,
literal|"(from %s)\n"
argument_list|,
name|parent_sha1
condition|?
name|diff_unique_abbrev
argument_list|(
name|parent_sha1
argument_list|,
name|abbrev
argument_list|)
else|:
literal|"root"
argument_list|)
expr_stmt|;
else|else
name|offset
operator|+=
name|sprintf
argument_list|(
name|this_header
operator|+
name|offset
argument_list|,
literal|"(from parents)\n"
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|pretty_print_commit
argument_list|(
name|opt
operator|->
name|commit_format
argument_list|,
name|commit
argument_list|,
name|len
argument_list|,
name|this_header
operator|+
name|offset
argument_list|,
sizeof|sizeof
argument_list|(
name|this_header
argument_list|)
operator|-
name|offset
argument_list|,
name|abbrev
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|always_show_header
condition|)
block|{
name|puts
argument_list|(
name|this_header
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
name|this_header
return|;
block|}
end_function
begin_function
DECL|function|do_diff_combined
specifier|static
name|int
name|do_diff_combined
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|unsigned
specifier|const
name|char
modifier|*
name|sha1
init|=
name|commit
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
name|opt
operator|->
name|header
operator|=
name|generate_header
argument_list|(
name|opt
argument_list|,
name|sha1
argument_list|,
name|sha1
argument_list|,
name|commit
argument_list|)
expr_stmt|;
name|opt
operator|->
name|header
operator|=
name|diff_tree_combined_merge
argument_list|(
name|sha1
argument_list|,
name|opt
operator|->
name|header
argument_list|,
name|opt
operator|->
name|dense_combined_merges
argument_list|,
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|header
operator|&&
name|opt
operator|->
name|verbose_header
condition|)
name|opt
operator|->
name|header_prefix
operator|=
literal|"\ndiff-tree "
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|log_tree_commit
name|int
name|log_tree_commit
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|unsigned
specifier|const
name|char
modifier|*
name|sha1
init|=
name|commit
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
comment|/* Root commit? */
if|if
condition|(
name|opt
operator|->
name|show_root_diff
operator|&&
operator|!
name|commit
operator|->
name|parents
condition|)
block|{
name|opt
operator|->
name|header
operator|=
name|generate_header
argument_list|(
name|opt
argument_list|,
name|sha1
argument_list|,
name|NULL
argument_list|,
name|commit
argument_list|)
expr_stmt|;
name|diff_root_tree
argument_list|(
name|opt
argument_list|,
name|sha1
argument_list|,
literal|""
argument_list|)
expr_stmt|;
block|}
comment|/* More than one parent? */
if|if
condition|(
name|commit
operator|->
name|parents
operator|&&
name|commit
operator|->
name|parents
operator|->
name|next
condition|)
block|{
if|if
condition|(
name|opt
operator|->
name|ignore_merges
condition|)
return|return
literal|0
return|;
elseif|else
if|if
condition|(
name|opt
operator|->
name|combine_merges
condition|)
return|return
name|do_diff_combined
argument_list|(
name|opt
argument_list|,
name|commit
argument_list|)
return|;
block|}
for|for
control|(
name|parents
operator|=
name|commit
operator|->
name|parents
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
name|struct
name|commit
modifier|*
name|parent
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|unsigned
specifier|const
name|char
modifier|*
name|psha1
init|=
name|parent
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
name|opt
operator|->
name|header
operator|=
name|generate_header
argument_list|(
name|opt
argument_list|,
name|sha1
argument_list|,
name|psha1
argument_list|,
name|commit
argument_list|)
expr_stmt|;
name|diff_tree_sha1
argument_list|(
name|psha1
argument_list|,
name|sha1
argument_list|,
literal|""
argument_list|,
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|opt
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|header
operator|&&
name|opt
operator|->
name|verbose_header
condition|)
name|opt
operator|->
name|header_prefix
operator|=
literal|"\ndiff-tree "
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
