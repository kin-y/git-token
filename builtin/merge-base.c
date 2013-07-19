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
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_function
DECL|function|show_merge_base
specifier|static
name|int
name|show_merge_base
parameter_list|(
name|struct
name|commit
modifier|*
modifier|*
name|rev
parameter_list|,
name|int
name|rev_nr
parameter_list|,
name|int
name|show_all
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|result
decl_stmt|;
name|result
operator|=
name|get_merge_bases_many
argument_list|(
name|rev
index|[
literal|0
index|]
argument_list|,
name|rev_nr
operator|-
literal|1
argument_list|,
name|rev
operator|+
literal|1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|result
condition|)
return|return
literal|1
return|;
while|while
condition|(
name|result
condition|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|result
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_all
condition|)
return|return
literal|0
return|;
name|result
operator|=
name|result
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|merge_base_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|merge_base_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git merge-base [-a|--all]<commit><commit>..."
argument_list|)
block|,
name|N_
argument_list|(
literal|"git merge-base [-a|--all] --octopus<commit>..."
argument_list|)
block|,
name|N_
argument_list|(
literal|"git merge-base --independent<commit>..."
argument_list|)
block|,
name|N_
argument_list|(
literal|"git merge-base --is-ancestor<commit><commit>"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|get_commit_reference
specifier|static
name|struct
name|commit
modifier|*
name|get_commit_reference
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
name|unsigned
name|char
name|revkey
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|r
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|revkey
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|r
operator|=
name|lookup_commit_reference
argument_list|(
name|revkey
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|r
condition|)
name|die
argument_list|(
literal|"Not a valid commit name %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
return|return
name|r
return|;
block|}
end_function
begin_function
DECL|function|handle_octopus
specifier|static
name|int
name|handle_octopus
parameter_list|(
name|int
name|count
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|args
parameter_list|,
name|int
name|reduce
parameter_list|,
name|int
name|show_all
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|revs
init|=
name|NULL
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|result
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|reduce
condition|)
name|show_all
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
name|count
operator|-
literal|1
init|;
name|i
operator|>=
literal|0
condition|;
name|i
operator|--
control|)
name|commit_list_insert
argument_list|(
name|get_commit_reference
argument_list|(
name|args
index|[
name|i
index|]
argument_list|)
argument_list|,
operator|&
name|revs
argument_list|)
expr_stmt|;
name|result
operator|=
name|reduce
condition|?
name|reduce_heads
argument_list|(
name|revs
argument_list|)
else|:
name|get_octopus_merge_bases
argument_list|(
name|revs
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|result
condition|)
return|return
literal|1
return|;
while|while
condition|(
name|result
condition|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|result
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_all
condition|)
return|return
literal|0
return|;
name|result
operator|=
name|result
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|handle_is_ancestor
specifier|static
name|int
name|handle_is_ancestor
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
name|commit
modifier|*
name|one
decl_stmt|,
modifier|*
name|two
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|2
condition|)
name|die
argument_list|(
literal|"--is-ancestor takes exactly two commits"
argument_list|)
expr_stmt|;
name|one
operator|=
name|get_commit_reference
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|two
operator|=
name|get_commit_reference
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|in_merge_bases
argument_list|(
name|one
argument_list|,
name|two
argument_list|)
condition|)
return|return
literal|0
return|;
else|else
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|cmd_merge_base
name|int
name|cmd_merge_base
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
name|commit
modifier|*
modifier|*
name|rev
decl_stmt|;
name|int
name|rev_nr
init|=
literal|0
decl_stmt|;
name|int
name|show_all
init|=
literal|0
decl_stmt|;
name|int
name|octopus
init|=
literal|0
decl_stmt|;
name|int
name|reduce
init|=
literal|0
decl_stmt|;
name|int
name|is_ancestor
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'a'
argument_list|,
literal|"all"
argument_list|,
operator|&
name|show_all
argument_list|,
name|N_
argument_list|(
literal|"output all common ancestors"
argument_list|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"octopus"
argument_list|,
operator|&
name|octopus
argument_list|,
name|N_
argument_list|(
literal|"find ancestors for a single n-way merge"
argument_list|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"independent"
argument_list|,
operator|&
name|reduce
argument_list|,
name|N_
argument_list|(
literal|"list revs not reachable from others"
argument_list|)
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"is-ancestor"
argument_list|,
operator|&
name|is_ancestor
argument_list|,
name|N_
argument_list|(
literal|"is the first one ancestor of the other?"
argument_list|)
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
name|merge_base_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|octopus
operator|&&
operator|!
name|reduce
operator|&&
name|argc
operator|<
literal|2
condition|)
name|usage_with_options
argument_list|(
name|merge_base_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_ancestor
operator|&&
operator|(
name|show_all
operator|||
name|octopus
operator|||
name|reduce
operator|)
condition|)
name|die
argument_list|(
literal|"--is-ancestor cannot be used with other options"
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_ancestor
condition|)
return|return
name|handle_is_ancestor
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|)
return|;
if|if
condition|(
name|reduce
operator|&&
operator|(
name|show_all
operator|||
name|octopus
operator|)
condition|)
name|die
argument_list|(
literal|"--independent cannot be used with other options"
argument_list|)
expr_stmt|;
if|if
condition|(
name|octopus
operator|||
name|reduce
condition|)
return|return
name|handle_octopus
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|reduce
argument_list|,
name|show_all
argument_list|)
return|;
name|rev
operator|=
name|xmalloc
argument_list|(
name|argc
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|rev
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
name|argc
operator|--
operator|>
literal|0
condition|)
name|rev
index|[
name|rev_nr
operator|++
index|]
operator|=
name|get_commit_reference
argument_list|(
operator|*
name|argv
operator|++
argument_list|)
expr_stmt|;
return|return
name|show_merge_base
argument_list|(
name|rev
argument_list|,
name|rev_nr
argument_list|,
name|show_all
argument_list|)
return|;
block|}
end_function
end_unit
