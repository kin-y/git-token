begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
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
file|"diffcore.h"
end_include
begin_function
DECL|function|contains
specifier|static
name|unsigned
name|int
name|contains
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|one
parameter_list|,
specifier|const
name|char
modifier|*
name|needle
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|,
name|regex_t
modifier|*
name|regexp
parameter_list|)
block|{
name|unsigned
name|int
name|cnt
decl_stmt|;
name|unsigned
name|long
name|sz
decl_stmt|;
specifier|const
name|char
modifier|*
name|data
decl_stmt|;
if|if
condition|(
name|diff_populate_filespec
argument_list|(
name|one
argument_list|,
literal|0
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|len
condition|)
return|return
literal|0
return|;
name|sz
operator|=
name|one
operator|->
name|size
expr_stmt|;
name|data
operator|=
name|one
operator|->
name|data
expr_stmt|;
name|cnt
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|regexp
condition|)
block|{
name|regmatch_t
name|regmatch
decl_stmt|;
name|int
name|flags
init|=
literal|0
decl_stmt|;
name|assert
argument_list|(
name|data
index|[
name|sz
index|]
operator|==
literal|'\0'
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|data
operator|&&
operator|!
name|regexec
argument_list|(
name|regexp
argument_list|,
name|data
argument_list|,
literal|1
argument_list|,
operator|&
name|regmatch
argument_list|,
name|flags
argument_list|)
condition|)
block|{
name|flags
operator||=
name|REG_NOTBOL
expr_stmt|;
name|data
operator|+=
name|regmatch
operator|.
name|rm_eo
expr_stmt|;
if|if
condition|(
operator|*
name|data
operator|&&
name|regmatch
operator|.
name|rm_so
operator|==
name|regmatch
operator|.
name|rm_eo
condition|)
name|data
operator|++
expr_stmt|;
name|cnt
operator|++
expr_stmt|;
block|}
block|}
else|else
block|{
comment|/* Classic exact string match */
while|while
condition|(
name|sz
condition|)
block|{
specifier|const
name|char
modifier|*
name|found
init|=
name|memmem
argument_list|(
name|data
argument_list|,
name|sz
argument_list|,
name|needle
argument_list|,
name|len
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|found
condition|)
break|break;
name|sz
operator|-=
name|found
operator|-
name|data
operator|+
name|len
expr_stmt|;
name|data
operator|=
name|found
operator|+
name|len
expr_stmt|;
name|cnt
operator|++
expr_stmt|;
block|}
block|}
name|diff_free_filespec_data
argument_list|(
name|one
argument_list|)
expr_stmt|;
return|return
name|cnt
return|;
block|}
end_function
begin_function
DECL|function|diffcore_pickaxe
name|void
name|diffcore_pickaxe
parameter_list|(
specifier|const
name|char
modifier|*
name|needle
parameter_list|,
name|int
name|opts
parameter_list|)
block|{
name|struct
name|diff_queue_struct
modifier|*
name|q
init|=
operator|&
name|diff_queued_diff
decl_stmt|;
name|unsigned
name|long
name|len
init|=
name|strlen
argument_list|(
name|needle
argument_list|)
decl_stmt|;
name|int
name|i
decl_stmt|,
name|has_changes
decl_stmt|;
name|regex_t
name|regex
decl_stmt|,
modifier|*
name|regexp
init|=
name|NULL
decl_stmt|;
name|struct
name|diff_queue_struct
name|outq
decl_stmt|;
name|DIFF_QUEUE_CLEAR
argument_list|(
operator|&
name|outq
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|&
name|DIFF_PICKAXE_REGEX
condition|)
block|{
name|int
name|err
decl_stmt|;
name|err
operator|=
name|regcomp
argument_list|(
operator|&
name|regex
argument_list|,
name|needle
argument_list|,
name|REG_EXTENDED
operator||
name|REG_NEWLINE
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
block|{
comment|/* The POSIX.2 people are surely sick */
name|char
name|errbuf
index|[
literal|1024
index|]
decl_stmt|;
name|regerror
argument_list|(
name|err
argument_list|,
operator|&
name|regex
argument_list|,
name|errbuf
argument_list|,
literal|1024
argument_list|)
expr_stmt|;
name|regfree
argument_list|(
operator|&
name|regex
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"invalid pickaxe regex: %s"
argument_list|,
name|errbuf
argument_list|)
expr_stmt|;
block|}
name|regexp
operator|=
operator|&
name|regex
expr_stmt|;
block|}
if|if
condition|(
name|opts
operator|&
name|DIFF_PICKAXE_ALL
condition|)
block|{
comment|/* Showing the whole changeset if needle exists */
for|for
control|(
name|i
operator|=
name|has_changes
operator|=
literal|0
init|;
operator|!
name|has_changes
operator|&&
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
continue|continue;
comment|/* ignore unmerged */
comment|/* created */
if|if
condition|(
name|contains
argument_list|(
name|p
operator|->
name|two
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
condition|)
name|has_changes
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
if|if
condition|(
name|contains
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
condition|)
name|has_changes
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|diff_unmodified_pair
argument_list|(
name|p
argument_list|)
operator|&&
name|contains
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
operator|!=
name|contains
argument_list|(
name|p
operator|->
name|two
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
condition|)
name|has_changes
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|has_changes
condition|)
return|return;
comment|/* not munge the queue */
comment|/* otherwise we will clear the whole queue 		 * by copying the empty outq at the end of this 		 * function, but first clear the current entries 		 * in the queue. 		 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|diff_free_filepair
argument_list|(
name|q
operator|->
name|queue
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
else|else
comment|/* Showing only the filepairs that has the needle */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
name|has_changes
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
empty_stmt|;
comment|/* ignore unmerged */
comment|/* created */
elseif|else
if|if
condition|(
name|contains
argument_list|(
name|p
operator|->
name|two
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
condition|)
name|has_changes
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
condition|)
block|{
if|if
condition|(
name|contains
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
condition|)
name|has_changes
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|diff_unmodified_pair
argument_list|(
name|p
argument_list|)
operator|&&
name|contains
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
operator|!=
name|contains
argument_list|(
name|p
operator|->
name|two
argument_list|,
name|needle
argument_list|,
name|len
argument_list|,
name|regexp
argument_list|)
condition|)
name|has_changes
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|has_changes
condition|)
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
else|else
name|diff_free_filepair
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|opts
operator|&
name|DIFF_PICKAXE_REGEX
condition|)
block|{
name|regfree
argument_list|(
operator|&
name|regex
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|q
operator|->
name|queue
argument_list|)
expr_stmt|;
operator|*
name|q
operator|=
name|outq
expr_stmt|;
return|return;
block|}
end_function
end_unit
