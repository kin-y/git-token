begin_unit
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
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_decl_stmt
DECL|variable|worst_skew
name|unsigned
name|long
name|worst_skew
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|check_skew_recurse
specifier|static
name|void
name|check_skew_recurse
parameter_list|(
name|struct
name|commit
modifier|*
name|c
parameter_list|,
name|unsigned
name|long
name|when
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|c
operator|->
name|object
operator|.
name|flags
operator|&
name|SEEN
condition|)
return|return;
name|c
operator|->
name|object
operator|.
name|flags
operator||=
name|SEEN
expr_stmt|;
if|if
condition|(
name|parse_commit
argument_list|(
name|c
argument_list|)
operator|<
literal|0
condition|)
return|return;
if|if
condition|(
name|c
operator|->
name|date
operator|>
name|when
condition|)
block|{
name|unsigned
name|long
name|skew
init|=
name|c
operator|->
name|date
operator|-
name|when
decl_stmt|;
if|if
condition|(
name|skew
operator|>
name|worst_skew
condition|)
name|worst_skew
operator|=
name|skew
expr_stmt|;
block|}
for|for
control|(
name|p
operator|=
name|c
operator|->
name|parents
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
name|check_skew_recurse
argument_list|(
name|p
operator|->
name|item
argument_list|,
name|c
operator|->
name|date
operator|<
name|when
condition|?
name|c
operator|->
name|date
else|:
name|when
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|check_skew
specifier|static
name|void
name|check_skew
parameter_list|(
name|struct
name|commit
modifier|*
name|c
parameter_list|)
block|{
name|check_skew_recurse
argument_list|(
name|c
argument_list|,
name|time
argument_list|(
name|NULL
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_skew
name|int
name|cmd_skew
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
name|rev_info
name|revs
decl_stmt|;
name|int
name|i
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|revs
operator|.
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object
modifier|*
name|o
init|=
name|revs
operator|.
name|pending
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
decl_stmt|;
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
name|check_skew
argument_list|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|o
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"%lu\n"
argument_list|,
name|worst_skew
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
