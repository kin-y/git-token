begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  * Copyright (C) 2010 Google Inc.  */
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
begin_include
include|#
directive|include
file|"xdiff-interface.h"
end_include
begin_struct
DECL|struct|diffgrep_cb
struct|struct
name|diffgrep_cb
block|{
DECL|member|regexp
name|regex_t
modifier|*
name|regexp
decl_stmt|;
DECL|member|hit
name|int
name|hit
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|diffgrep_consume
specifier|static
name|void
name|diffgrep_consume
parameter_list|(
name|void
modifier|*
name|priv
parameter_list|,
name|char
modifier|*
name|line
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|)
block|{
name|struct
name|diffgrep_cb
modifier|*
name|data
init|=
name|priv
decl_stmt|;
name|regmatch_t
name|regmatch
decl_stmt|;
name|int
name|hold
decl_stmt|;
if|if
condition|(
name|line
index|[
literal|0
index|]
operator|!=
literal|'+'
operator|&&
name|line
index|[
literal|0
index|]
operator|!=
literal|'-'
condition|)
return|return;
if|if
condition|(
name|data
operator|->
name|hit
condition|)
comment|/* 		 * NEEDSWORK: we should have a way to terminate the 		 * caller early. 		 */
return|return;
comment|/* Yuck -- line ought to be "const char *"! */
name|hold
operator|=
name|line
index|[
name|len
index|]
expr_stmt|;
name|line
index|[
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
name|data
operator|->
name|hit
operator|=
operator|!
name|regexec
argument_list|(
name|data
operator|->
name|regexp
argument_list|,
name|line
operator|+
literal|1
argument_list|,
literal|1
argument_list|,
operator|&
name|regmatch
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|line
index|[
name|len
index|]
operator|=
name|hold
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fill_one
specifier|static
name|void
name|fill_one
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|one
parameter_list|,
name|mmfile_t
modifier|*
name|mf
parameter_list|,
name|struct
name|userdiff_driver
modifier|*
modifier|*
name|textconv
parameter_list|)
block|{
if|if
condition|(
name|DIFF_FILE_VALID
argument_list|(
name|one
argument_list|)
condition|)
block|{
operator|*
name|textconv
operator|=
name|get_textconv
argument_list|(
name|one
argument_list|)
expr_stmt|;
name|mf
operator|->
name|size
operator|=
name|fill_textconv
argument_list|(
operator|*
name|textconv
argument_list|,
name|one
argument_list|,
operator|&
name|mf
operator|->
name|ptr
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|memset
argument_list|(
name|mf
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|mf
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|diff_grep
specifier|static
name|int
name|diff_grep
parameter_list|(
name|struct
name|diff_filepair
modifier|*
name|p
parameter_list|,
name|regex_t
modifier|*
name|regexp
parameter_list|,
name|struct
name|diff_options
modifier|*
name|o
parameter_list|)
block|{
name|regmatch_t
name|regmatch
decl_stmt|;
name|struct
name|userdiff_driver
modifier|*
name|textconv_one
init|=
name|NULL
decl_stmt|;
name|struct
name|userdiff_driver
modifier|*
name|textconv_two
init|=
name|NULL
decl_stmt|;
name|mmfile_t
name|mf1
decl_stmt|,
name|mf2
decl_stmt|;
name|int
name|hit
decl_stmt|;
if|if
condition|(
name|diff_unmodified_pair
argument_list|(
name|p
argument_list|)
condition|)
return|return
literal|0
return|;
name|fill_one
argument_list|(
name|p
operator|->
name|one
argument_list|,
operator|&
name|mf1
argument_list|,
operator|&
name|textconv_one
argument_list|)
expr_stmt|;
name|fill_one
argument_list|(
name|p
operator|->
name|two
argument_list|,
operator|&
name|mf2
argument_list|,
operator|&
name|textconv_two
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|mf1
operator|.
name|ptr
condition|)
block|{
if|if
condition|(
operator|!
name|mf2
operator|.
name|ptr
condition|)
return|return
literal|0
return|;
comment|/* ignore unmerged */
comment|/* created "two" -- does it have what we are looking for? */
name|hit
operator|=
operator|!
name|regexec
argument_list|(
name|regexp
argument_list|,
name|p
operator|->
name|two
operator|->
name|data
argument_list|,
literal|1
argument_list|,
operator|&
name|regmatch
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|mf2
operator|.
name|ptr
condition|)
block|{
comment|/* removed "one" -- did it have what we are looking for? */
name|hit
operator|=
operator|!
name|regexec
argument_list|(
name|regexp
argument_list|,
name|p
operator|->
name|one
operator|->
name|data
argument_list|,
literal|1
argument_list|,
operator|&
name|regmatch
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* 		 * We have both sides; need to run textual diff and see if 		 * the pattern appears on added/deleted lines. 		 */
name|struct
name|diffgrep_cb
name|ecbdata
decl_stmt|;
name|xpparam_t
name|xpp
decl_stmt|;
name|xdemitconf_t
name|xecfg
decl_stmt|;
name|memset
argument_list|(
operator|&
name|xpp
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|xpp
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|xecfg
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|xecfg
argument_list|)
argument_list|)
expr_stmt|;
name|ecbdata
operator|.
name|regexp
operator|=
name|regexp
expr_stmt|;
name|ecbdata
operator|.
name|hit
operator|=
literal|0
expr_stmt|;
name|xecfg
operator|.
name|ctxlen
operator|=
name|o
operator|->
name|context
expr_stmt|;
name|xecfg
operator|.
name|interhunkctxlen
operator|=
name|o
operator|->
name|interhunkcontext
expr_stmt|;
name|xdi_diff_outf
argument_list|(
operator|&
name|mf1
argument_list|,
operator|&
name|mf2
argument_list|,
name|diffgrep_consume
argument_list|,
operator|&
name|ecbdata
argument_list|,
operator|&
name|xpp
argument_list|,
operator|&
name|xecfg
argument_list|)
expr_stmt|;
name|hit
operator|=
name|ecbdata
operator|.
name|hit
expr_stmt|;
block|}
if|if
condition|(
name|textconv_one
condition|)
name|free
argument_list|(
name|mf1
operator|.
name|ptr
argument_list|)
expr_stmt|;
if|if
condition|(
name|textconv_two
condition|)
name|free
argument_list|(
name|mf2
operator|.
name|ptr
argument_list|)
expr_stmt|;
return|return
name|hit
return|;
block|}
end_function
begin_function
DECL|function|diffcore_pickaxe_grep
specifier|static
name|void
name|diffcore_pickaxe_grep
parameter_list|(
name|struct
name|diff_options
modifier|*
name|o
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
name|int
name|i
decl_stmt|,
name|has_changes
decl_stmt|,
name|err
decl_stmt|;
name|regex_t
name|regex
decl_stmt|;
name|struct
name|diff_queue_struct
name|outq
decl_stmt|;
name|outq
operator|.
name|queue
operator|=
name|NULL
expr_stmt|;
name|outq
operator|.
name|nr
operator|=
name|outq
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
name|err
operator|=
name|regcomp
argument_list|(
operator|&
name|regex
argument_list|,
name|o
operator|->
name|pickaxe
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
literal|"invalid log-grep regex: %s"
argument_list|,
name|errbuf
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|o
operator|->
name|pickaxe_opts
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
name|diff_grep
argument_list|(
name|p
argument_list|,
operator|&
name|regex
argument_list|,
name|o
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
comment|/* do not munge the queue */
comment|/* 		 * Otherwise we will clear the whole queue by copying 		 * the empty outq at the end of this function, but 		 * first clear the current entries in the queue. 		 */
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
block|{
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
if|if
condition|(
name|diff_grep
argument_list|(
name|p
argument_list|,
operator|&
name|regex
argument_list|,
name|o
argument_list|)
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
block|}
name|regfree
argument_list|(
operator|&
name|regex
argument_list|)
expr_stmt|;
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
DECL|function|diffcore_pickaxe_count
specifier|static
name|void
name|diffcore_pickaxe_count
parameter_list|(
name|struct
name|diff_options
modifier|*
name|o
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|needle
init|=
name|o
operator|->
name|pickaxe
decl_stmt|;
name|int
name|opts
init|=
name|o
operator|->
name|pickaxe_opts
decl_stmt|;
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
name|regfree
argument_list|(
operator|&
name|regex
argument_list|)
expr_stmt|;
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
begin_function
DECL|function|diffcore_pickaxe
name|void
name|diffcore_pickaxe
parameter_list|(
name|struct
name|diff_options
modifier|*
name|o
parameter_list|)
block|{
comment|/* Might want to warn when both S and G are on; I don't care... */
if|if
condition|(
name|o
operator|->
name|pickaxe_opts
operator|&
name|DIFF_PICKAXE_KIND_G
condition|)
return|return
name|diffcore_pickaxe_grep
argument_list|(
name|o
argument_list|)
return|;
else|else
return|return
name|diffcore_pickaxe_count
argument_list|(
name|o
argument_list|)
return|;
block|}
end_function
end_unit
