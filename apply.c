begin_unit
begin_comment
comment|/*  * apply.c  *  * Copyright (C) Linus Torvalds, 2005  *  * This applies patches on top of some (arbitrary) version of the SCM.  *  * NOTE! It does all its work in the index file, and only cares about  * the files in the working directory if you tell it to "merge" the  * patch apply.  *  * Even when merging it always takes the source from the index, and  * uses the working tree as a "branch" for a 3-way merge.  */
end_comment
begin_include
include|#
directive|include
file|<ctype.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_comment
comment|// We default to the merge behaviour, since that's what most people would
end_comment
begin_comment
comment|// expect
end_comment
begin_decl_stmt
DECL|variable|merge_patch
specifier|static
name|int
name|merge_patch
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|apply_usage
specifier|static
specifier|const
name|char
name|apply_usage
index|[]
init|=
literal|"git-apply<patch>"
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * Various "current state", notably line numbers and what  * file (and how) we're patching right now.. The "is_xxxx"  * things are flags, where -1 means "don't know yet".  */
end_comment
begin_decl_stmt
DECL|variable|linenr
specifier|static
name|int
name|linenr
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|old_mode
DECL|variable|new_mode
specifier|static
name|int
name|old_mode
decl_stmt|,
name|new_mode
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|old_name
DECL|variable|new_name
DECL|variable|def_name
specifier|static
name|char
modifier|*
name|old_name
decl_stmt|,
modifier|*
name|new_name
decl_stmt|,
modifier|*
name|def_name
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|is_rename
DECL|variable|is_copy
DECL|variable|is_new
DECL|variable|is_delete
specifier|static
name|int
name|is_rename
decl_stmt|,
name|is_copy
decl_stmt|,
name|is_new
decl_stmt|,
name|is_delete
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|CHUNKSIZE
define|#
directive|define
name|CHUNKSIZE
value|(8192)
end_define
begin_define
DECL|macro|SLOP
define|#
directive|define
name|SLOP
value|(16)
end_define
begin_function
DECL|function|read_patch_file
specifier|static
name|void
modifier|*
name|read_patch_file
parameter_list|(
name|int
name|fd
parameter_list|,
name|unsigned
name|long
modifier|*
name|sizep
parameter_list|)
block|{
name|unsigned
name|long
name|size
init|=
literal|0
decl_stmt|,
name|alloc
init|=
name|CHUNKSIZE
decl_stmt|;
name|void
modifier|*
name|buffer
init|=
name|xmalloc
argument_list|(
name|alloc
argument_list|)
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|nr
init|=
name|alloc
operator|-
name|size
decl_stmt|;
if|if
condition|(
name|nr
operator|<
literal|1024
condition|)
block|{
name|alloc
operator|+=
name|CHUNKSIZE
expr_stmt|;
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
argument_list|,
name|alloc
argument_list|)
expr_stmt|;
name|nr
operator|=
name|alloc
operator|-
name|size
expr_stmt|;
block|}
name|nr
operator|=
name|read
argument_list|(
name|fd
argument_list|,
name|buffer
operator|+
name|size
argument_list|,
name|nr
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|nr
condition|)
break|break;
if|if
condition|(
name|nr
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|EAGAIN
condition|)
continue|continue;
name|die
argument_list|(
literal|"git-apply: read returned %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|size
operator|+=
name|nr
expr_stmt|;
block|}
operator|*
name|sizep
operator|=
name|size
expr_stmt|;
comment|/* 	 * Make sure that we have some slop in the buffer 	 * so that we can do speculative "memcmp" etc, and 	 * see to it that it is NUL-filled. 	 */
if|if
condition|(
name|alloc
operator|<
name|size
operator|+
name|SLOP
condition|)
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
argument_list|,
name|size
operator|+
name|SLOP
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|buffer
operator|+
name|size
argument_list|,
literal|0
argument_list|,
name|SLOP
argument_list|)
expr_stmt|;
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|linelen
specifier|static
name|unsigned
name|long
name|linelen
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|unsigned
name|long
name|len
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|size
operator|--
condition|)
block|{
name|len
operator|++
expr_stmt|;
if|if
condition|(
operator|*
name|buffer
operator|++
operator|==
literal|'\n'
condition|)
break|break;
block|}
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|is_dev_null
specifier|static
name|int
name|is_dev_null
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|)
block|{
return|return
operator|!
name|memcmp
argument_list|(
literal|"/dev/null"
argument_list|,
name|str
argument_list|,
literal|9
argument_list|)
operator|&&
name|isspace
argument_list|(
name|str
index|[
literal|9
index|]
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|TERM_EXIST
define|#
directive|define
name|TERM_EXIST
value|1
end_define
begin_define
DECL|macro|TERM_SPACE
define|#
directive|define
name|TERM_SPACE
value|2
end_define
begin_define
DECL|macro|TERM_TAB
define|#
directive|define
name|TERM_TAB
value|4
end_define
begin_function
DECL|function|name_terminate
specifier|static
name|int
name|name_terminate
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|namelen
parameter_list|,
name|int
name|c
parameter_list|,
name|int
name|terminate
parameter_list|)
block|{
if|if
condition|(
name|c
operator|==
literal|' '
operator|&&
operator|!
operator|(
name|terminate
operator|&
name|TERM_SPACE
operator|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|c
operator|==
literal|'\t'
operator|&&
operator|!
operator|(
name|terminate
operator|&
name|TERM_TAB
operator|)
condition|)
return|return
literal|0
return|;
comment|/* 	 * Do we want an existing name? Return false and 	 * continue if it's not there. 	 */
if|if
condition|(
name|terminate
operator|&
name|TERM_EXIST
condition|)
return|return
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
operator|>=
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|find_name
specifier|static
name|char
modifier|*
name|find_name
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|char
modifier|*
name|def
parameter_list|,
name|int
name|p_value
parameter_list|,
name|int
name|terminate
parameter_list|)
block|{
name|int
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|start
init|=
name|line
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|char
name|c
init|=
operator|*
name|line
decl_stmt|;
if|if
condition|(
name|isspace
argument_list|(
name|c
argument_list|)
condition|)
block|{
if|if
condition|(
name|c
operator|==
literal|'\n'
condition|)
break|break;
if|if
condition|(
name|name_terminate
argument_list|(
name|start
argument_list|,
name|line
operator|-
name|start
argument_list|,
name|c
argument_list|,
name|terminate
argument_list|)
condition|)
break|break;
block|}
name|line
operator|++
expr_stmt|;
if|if
condition|(
name|c
operator|==
literal|'/'
operator|&&
operator|!
operator|--
name|p_value
condition|)
name|start
operator|=
name|line
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|start
condition|)
return|return
name|def
return|;
name|len
operator|=
name|line
operator|-
name|start
expr_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
return|return
name|def
return|;
comment|/* 	 * Generally we prefer the shorter name, especially 	 * if the other one is just a variation of that with 	 * something else tacked on to the end (ie "file.orig" 	 * or "file~"). 	 */
if|if
condition|(
name|def
condition|)
block|{
name|int
name|deflen
init|=
name|strlen
argument_list|(
name|def
argument_list|)
decl_stmt|;
if|if
condition|(
name|deflen
operator|<
name|len
operator|&&
operator|!
name|strncmp
argument_list|(
name|start
argument_list|,
name|def
argument_list|,
name|deflen
argument_list|)
condition|)
return|return
name|def
return|;
block|}
name|name
operator|=
name|xmalloc
argument_list|(
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|name
argument_list|,
name|start
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|name
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
name|free
argument_list|(
name|def
argument_list|)
expr_stmt|;
return|return
name|name
return|;
block|}
end_function
begin_comment
comment|/*  * Get the name etc info from the --/+++ lines of a traditional patch header  *  * NOTE! This hardcodes "-p1" behaviour in filename detection.  *  * FIXME! The end-of-filename heuristics are kind of screwy. For existing  * files, we can happily check the index for a match, but for creating a  * new file we should try to match whatever "patch" does. I have no idea.  */
end_comment
begin_function
DECL|function|parse_traditional_patch
specifier|static
name|int
name|parse_traditional_patch
parameter_list|(
specifier|const
name|char
modifier|*
name|first
parameter_list|,
specifier|const
name|char
modifier|*
name|second
parameter_list|)
block|{
name|int
name|p_value
init|=
literal|1
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|;
name|first
operator|+=
literal|4
expr_stmt|;
comment|// skip "--- "
name|second
operator|+=
literal|4
expr_stmt|;
comment|// skip "+++ "
if|if
condition|(
name|is_dev_null
argument_list|(
name|first
argument_list|)
condition|)
block|{
name|is_new
operator|=
literal|1
expr_stmt|;
name|name
operator|=
name|find_name
argument_list|(
name|second
argument_list|,
name|def_name
argument_list|,
name|p_value
argument_list|,
name|TERM_SPACE
operator||
name|TERM_TAB
argument_list|)
expr_stmt|;
name|new_name
operator|=
name|name
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|is_dev_null
argument_list|(
name|second
argument_list|)
condition|)
block|{
name|is_delete
operator|=
literal|1
expr_stmt|;
name|name
operator|=
name|find_name
argument_list|(
name|first
argument_list|,
name|def_name
argument_list|,
name|p_value
argument_list|,
name|TERM_EXIST
operator||
name|TERM_SPACE
operator||
name|TERM_TAB
argument_list|)
expr_stmt|;
name|old_name
operator|=
name|name
expr_stmt|;
block|}
else|else
block|{
name|name
operator|=
name|find_name
argument_list|(
name|first
argument_list|,
name|def_name
argument_list|,
name|p_value
argument_list|,
name|TERM_EXIST
operator||
name|TERM_SPACE
operator||
name|TERM_TAB
argument_list|)
expr_stmt|;
name|name
operator|=
name|find_name
argument_list|(
name|second
argument_list|,
name|name
argument_list|,
name|p_value
argument_list|,
name|TERM_EXIST
operator||
name|TERM_SPACE
operator||
name|TERM_TAB
argument_list|)
expr_stmt|;
name|old_name
operator|=
name|new_name
operator|=
name|name
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|name
condition|)
name|die
argument_list|(
literal|"unable to find filename in patch at line %d"
argument_list|,
name|linenr
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|gitdiff_hdrend
specifier|static
name|int
name|gitdiff_hdrend
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_oldname
specifier|static
name|int
name|gitdiff_oldname
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
if|if
condition|(
operator|!
name|old_name
operator|&&
operator|!
name|is_new
condition|)
name|old_name
operator|=
name|find_name
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_newname
specifier|static
name|int
name|gitdiff_newname
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
if|if
condition|(
operator|!
name|new_name
operator|&&
operator|!
name|is_delete
condition|)
name|new_name
operator|=
name|find_name
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_oldmode
specifier|static
name|int
name|gitdiff_oldmode
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|old_mode
operator|=
name|strtoul
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|8
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_newmode
specifier|static
name|int
name|gitdiff_newmode
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|new_mode
operator|=
name|strtoul
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|8
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_delete
specifier|static
name|int
name|gitdiff_delete
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|is_delete
operator|=
literal|1
expr_stmt|;
return|return
name|gitdiff_oldmode
argument_list|(
name|line
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_newfile
specifier|static
name|int
name|gitdiff_newfile
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|is_new
operator|=
literal|1
expr_stmt|;
return|return
name|gitdiff_newmode
argument_list|(
name|line
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_copysrc
specifier|static
name|int
name|gitdiff_copysrc
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|is_copy
operator|=
literal|1
expr_stmt|;
name|old_name
operator|=
name|find_name
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_copydst
specifier|static
name|int
name|gitdiff_copydst
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|is_copy
operator|=
literal|1
expr_stmt|;
name|new_name
operator|=
name|find_name
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_renamesrc
specifier|static
name|int
name|gitdiff_renamesrc
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|is_rename
operator|=
literal|1
expr_stmt|;
name|old_name
operator|=
name|find_name
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_renamedst
specifier|static
name|int
name|gitdiff_renamedst
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|is_rename
operator|=
literal|1
expr_stmt|;
name|new_name
operator|=
name|find_name
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|gitdiff_similarity
specifier|static
name|int
name|gitdiff_similarity
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * This is normal for a diff that doesn't change anything: we'll fall through  * into the next diff. Tell the parser to break out.  */
end_comment
begin_function
DECL|function|gitdiff_unrecognized
specifier|static
name|int
name|gitdiff_unrecognized
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/* Verify that we recognize the lines following a git header */
end_comment
begin_function
DECL|function|parse_git_header
specifier|static
name|int
name|parse_git_header
parameter_list|(
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|int
name|size
parameter_list|)
block|{
name|unsigned
name|long
name|offset
decl_stmt|;
comment|/* A git diff has explicit new/delete information, so we don't guess */
name|is_new
operator|=
literal|0
expr_stmt|;
name|is_delete
operator|=
literal|0
expr_stmt|;
name|line
operator|+=
name|len
expr_stmt|;
name|size
operator|-=
name|len
expr_stmt|;
name|linenr
operator|++
expr_stmt|;
for|for
control|(
name|offset
operator|=
name|len
init|;
name|size
operator|>
literal|0
condition|;
name|offset
operator|+=
name|len
operator|,
name|size
operator|-=
name|len
operator|,
name|line
operator|+=
name|len
operator|,
name|linenr
operator|++
control|)
block|{
specifier|static
specifier|const
struct|struct
name|opentry
block|{
specifier|const
name|char
modifier|*
name|str
decl_stmt|;
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
block|}
name|optable
index|[]
init|=
block|{
block|{
literal|"@@ -"
block|,
name|gitdiff_hdrend
block|}
block|,
block|{
literal|"--- "
block|,
name|gitdiff_oldname
block|}
block|,
block|{
literal|"+++ "
block|,
name|gitdiff_newname
block|}
block|,
block|{
literal|"old mode "
block|,
name|gitdiff_oldmode
block|}
block|,
block|{
literal|"new mode "
block|,
name|gitdiff_newmode
block|}
block|,
block|{
literal|"deleted file mode "
block|,
name|gitdiff_delete
block|}
block|,
block|{
literal|"new file mode "
block|,
name|gitdiff_newfile
block|}
block|,
block|{
literal|"copy from "
block|,
name|gitdiff_copysrc
block|}
block|,
block|{
literal|"copy to "
block|,
name|gitdiff_copydst
block|}
block|,
block|{
literal|"rename from "
block|,
name|gitdiff_renamesrc
block|}
block|,
block|{
literal|"rename to "
block|,
name|gitdiff_renamedst
block|}
block|,
block|{
literal|"similarity index "
block|,
name|gitdiff_similarity
block|}
block|,
block|{
literal|""
block|,
name|gitdiff_unrecognized
block|}
block|, 		}
struct|;
name|int
name|i
decl_stmt|;
name|len
operator|=
name|linelen
argument_list|(
name|line
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|len
operator|||
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
break|break;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
sizeof|sizeof
argument_list|(
name|optable
argument_list|)
operator|/
sizeof|sizeof
argument_list|(
name|optable
index|[
literal|0
index|]
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|struct
name|opentry
modifier|*
name|p
init|=
name|optable
operator|+
name|i
decl_stmt|;
name|int
name|oplen
init|=
name|strlen
argument_list|(
name|p
operator|->
name|str
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<
name|oplen
operator|||
name|memcmp
argument_list|(
name|p
operator|->
name|str
argument_list|,
name|line
argument_list|,
name|oplen
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|p
operator|->
name|fn
argument_list|(
name|line
operator|+
name|oplen
argument_list|)
operator|<
literal|0
condition|)
return|return
name|offset
return|;
break|break;
block|}
block|}
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|parse_num
specifier|static
name|int
name|parse_num
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|,
name|int
name|offset
parameter_list|,
specifier|const
name|char
modifier|*
name|expect
parameter_list|,
name|unsigned
name|long
modifier|*
name|p
parameter_list|)
block|{
name|char
modifier|*
name|ptr
decl_stmt|;
name|int
name|digits
decl_stmt|,
name|ex
decl_stmt|;
if|if
condition|(
name|offset
operator|<
literal|0
operator|||
name|offset
operator|>=
name|len
condition|)
return|return
operator|-
literal|1
return|;
name|line
operator|+=
name|offset
expr_stmt|;
name|len
operator|-=
name|offset
expr_stmt|;
if|if
condition|(
operator|!
name|isdigit
argument_list|(
operator|*
name|line
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
operator|*
name|p
operator|=
name|strtoul
argument_list|(
name|line
argument_list|,
operator|&
name|ptr
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|digits
operator|=
name|ptr
operator|-
name|line
expr_stmt|;
name|offset
operator|+=
name|digits
expr_stmt|;
name|line
operator|+=
name|digits
expr_stmt|;
name|len
operator|-=
name|digits
expr_stmt|;
name|ex
operator|=
name|strlen
argument_list|(
name|expect
argument_list|)
expr_stmt|;
if|if
condition|(
name|ex
operator|>
name|len
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|memcmp
argument_list|(
name|line
argument_list|,
name|expect
argument_list|,
name|ex
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|offset
operator|+
name|ex
return|;
block|}
end_function
begin_comment
comment|/*  * Parse a unified diff fragment header of the  * form "@@ -a,b +c,d @@"  */
end_comment
begin_function
DECL|function|parse_fragment_header
specifier|static
name|int
name|parse_fragment_header
parameter_list|(
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|long
modifier|*
name|pos
parameter_list|)
block|{
name|int
name|offset
decl_stmt|;
if|if
condition|(
operator|!
name|len
operator|||
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
return|return
operator|-
literal|1
return|;
comment|/* Figure out the number of lines in a fragment */
name|offset
operator|=
name|parse_num
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
literal|4
argument_list|,
literal|","
argument_list|,
name|pos
argument_list|)
expr_stmt|;
name|offset
operator|=
name|parse_num
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|offset
argument_list|,
literal|" +"
argument_list|,
name|pos
operator|+
literal|1
argument_list|)
expr_stmt|;
name|offset
operator|=
name|parse_num
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|offset
argument_list|,
literal|","
argument_list|,
name|pos
operator|+
literal|2
argument_list|)
expr_stmt|;
name|offset
operator|=
name|parse_num
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|offset
argument_list|,
literal|" @@"
argument_list|,
name|pos
operator|+
literal|3
argument_list|)
expr_stmt|;
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|find_header
specifier|static
name|int
name|find_header
parameter_list|(
name|char
modifier|*
name|line
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|int
modifier|*
name|hdrsize
parameter_list|)
block|{
name|unsigned
name|long
name|offset
decl_stmt|,
name|len
decl_stmt|;
name|is_rename
operator|=
name|is_copy
operator|=
literal|0
expr_stmt|;
name|is_new
operator|=
name|is_delete
operator|=
operator|-
literal|1
expr_stmt|;
name|old_mode
operator|=
name|new_mode
operator|=
literal|0
expr_stmt|;
name|def_name
operator|=
name|old_name
operator|=
name|new_name
operator|=
name|NULL
expr_stmt|;
for|for
control|(
name|offset
operator|=
literal|0
init|;
name|size
operator|>
literal|0
condition|;
name|offset
operator|+=
name|len
operator|,
name|size
operator|-=
name|len
operator|,
name|line
operator|+=
name|len
operator|,
name|linenr
operator|++
control|)
block|{
name|unsigned
name|long
name|nextlen
decl_stmt|;
name|len
operator|=
name|linelen
argument_list|(
name|line
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
break|break;
comment|/* Testing this early allows us to take a few shortcuts.. */
if|if
condition|(
name|len
operator|<
literal|6
condition|)
continue|continue;
comment|/* 		 * Make sure we don't find any unconnected patch fragmants. 		 * That's a sign that we didn't find a header, and that a 		 * patch has become corrupted/broken up. 		 */
if|if
condition|(
operator|!
name|memcmp
argument_list|(
literal|"@@ -"
argument_list|,
name|line
argument_list|,
literal|4
argument_list|)
condition|)
block|{
name|unsigned
name|long
name|pos
index|[
literal|4
index|]
decl_stmt|;
if|if
condition|(
name|parse_fragment_header
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|pos
argument_list|)
operator|<
literal|0
condition|)
continue|continue;
name|error
argument_list|(
literal|"patch fragment without header at line %d: %.*s"
argument_list|,
name|linenr
argument_list|,
name|len
operator|-
literal|1
argument_list|,
name|line
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|size
operator|<
name|len
operator|+
literal|6
condition|)
break|break;
comment|/* 		 * Git patch? It might not have a real patch, just a rename 		 * or mode change, so we handle that specially 		 */
if|if
condition|(
operator|!
name|memcmp
argument_list|(
literal|"diff --git "
argument_list|,
name|line
argument_list|,
literal|11
argument_list|)
condition|)
block|{
name|int
name|git_hdr_len
init|=
name|parse_git_header
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|git_hdr_len
operator|<
literal|0
condition|)
continue|continue;
operator|*
name|hdrsize
operator|=
name|git_hdr_len
expr_stmt|;
return|return
name|offset
return|;
block|}
comment|/** --- followed by +++ ? */
if|if
condition|(
name|memcmp
argument_list|(
literal|"--- "
argument_list|,
name|line
argument_list|,
literal|4
argument_list|)
operator|||
name|memcmp
argument_list|(
literal|"+++ "
argument_list|,
name|line
operator|+
name|len
argument_list|,
literal|4
argument_list|)
condition|)
continue|continue;
comment|/* 		 * We only accept unified patches, so we want it to 		 * at least have "@@ -a,b +c,d @@\n", which is 14 chars 		 * minimum 		 */
name|nextlen
operator|=
name|linelen
argument_list|(
name|line
operator|+
name|len
argument_list|,
name|size
operator|-
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|size
operator|<
name|nextlen
operator|+
literal|14
operator|||
name|memcmp
argument_list|(
literal|"@@ -"
argument_list|,
name|line
operator|+
name|len
operator|+
name|nextlen
argument_list|,
literal|4
argument_list|)
condition|)
continue|continue;
comment|/* Ok, we'll consider it a patch */
name|parse_traditional_patch
argument_list|(
name|line
argument_list|,
name|line
operator|+
name|len
argument_list|)
expr_stmt|;
operator|*
name|hdrsize
operator|=
name|len
operator|+
name|nextlen
expr_stmt|;
name|linenr
operator|+=
literal|2
expr_stmt|;
return|return
name|offset
return|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * Parse a unified diff. Note that this really needs  * to parse each fragment separately, since the only  * way to know the difference between a "---" that is  * part of a patch, and a "---" that starts the next  * patch is to look at the line counts..  */
end_comment
begin_function
DECL|function|apply_fragment
specifier|static
name|int
name|apply_fragment
parameter_list|(
name|char
modifier|*
name|line
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|int
name|len
init|=
name|linelen
argument_list|(
name|line
argument_list|,
name|size
argument_list|)
decl_stmt|,
name|offset
decl_stmt|;
name|unsigned
name|long
name|pos
index|[
literal|4
index|]
decl_stmt|,
name|oldlines
decl_stmt|,
name|newlines
decl_stmt|;
name|offset
operator|=
name|parse_fragment_header
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|pos
argument_list|)
expr_stmt|;
if|if
condition|(
name|offset
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|oldlines
operator|=
name|pos
index|[
literal|1
index|]
expr_stmt|;
name|newlines
operator|=
name|pos
index|[
literal|3
index|]
expr_stmt|;
if|if
condition|(
name|is_new
operator|<
literal|0
operator|&&
operator|(
name|pos
index|[
literal|0
index|]
operator|||
name|oldlines
operator|)
condition|)
name|is_new
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|is_delete
operator|<
literal|0
operator|&&
operator|(
name|pos
index|[
literal|1
index|]
operator|||
name|newlines
operator|)
condition|)
name|is_delete
operator|=
literal|0
expr_stmt|;
comment|/* Parse the thing.. */
name|line
operator|+=
name|len
expr_stmt|;
name|size
operator|-=
name|len
expr_stmt|;
name|linenr
operator|++
expr_stmt|;
for|for
control|(
name|offset
operator|=
name|len
init|;
name|size
operator|>
literal|0
condition|;
name|offset
operator|+=
name|len
operator|,
name|size
operator|-=
name|len
operator|,
name|line
operator|+=
name|len
operator|,
name|linenr
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|oldlines
operator|&&
operator|!
name|newlines
condition|)
break|break;
name|len
operator|=
name|linelen
argument_list|(
name|line
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|len
operator|||
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
return|return
operator|-
literal|1
return|;
switch|switch
condition|(
operator|*
name|line
condition|)
block|{
default|default:
return|return
operator|-
literal|1
return|;
case|case
literal|' '
case|:
name|oldlines
operator|--
expr_stmt|;
name|newlines
operator|--
expr_stmt|;
break|break;
case|case
literal|'-'
case|:
name|oldlines
operator|--
expr_stmt|;
break|break;
case|case
literal|'+'
case|:
name|newlines
operator|--
expr_stmt|;
break|break;
block|}
block|}
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|apply_single_patch
specifier|static
name|int
name|apply_single_patch
parameter_list|(
name|char
modifier|*
name|line
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|unsigned
name|long
name|offset
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|size
operator|>
literal|4
operator|&&
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"@@ -"
argument_list|,
literal|4
argument_list|)
condition|)
block|{
name|int
name|len
init|=
name|apply_fragment
argument_list|(
name|line
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<=
literal|0
condition|)
name|die
argument_list|(
literal|"corrupt patch at line %d"
argument_list|,
name|linenr
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"applying fragment:\n%.*s\n\n"
argument_list|,
name|len
argument_list|,
name|line
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|len
expr_stmt|;
name|line
operator|+=
name|len
expr_stmt|;
name|size
operator|-=
name|len
expr_stmt|;
block|}
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|apply_chunk
specifier|static
name|int
name|apply_chunk
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|int
name|hdrsize
decl_stmt|,
name|patchsize
decl_stmt|;
name|int
name|offset
init|=
name|find_header
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|,
operator|&
name|hdrsize
argument_list|)
decl_stmt|;
name|char
modifier|*
name|header
decl_stmt|,
modifier|*
name|patch
decl_stmt|;
if|if
condition|(
name|offset
operator|<
literal|0
condition|)
return|return
name|offset
return|;
name|header
operator|=
name|buffer
operator|+
name|offset
expr_stmt|;
name|printf
argument_list|(
literal|"Found header:\n%.*s\n\n"
argument_list|,
name|hdrsize
argument_list|,
name|header
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"Rename: %d\n"
argument_list|,
name|is_rename
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"Copy:   %d\n"
argument_list|,
name|is_copy
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"New:    %d\n"
argument_list|,
name|is_new
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"Delete: %d\n"
argument_list|,
name|is_delete
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"Mode:   %o:%o\n"
argument_list|,
name|old_mode
argument_list|,
name|new_mode
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"Name:   '%s':'%s'\n"
argument_list|,
name|old_name
argument_list|,
name|new_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|old_name
operator|&&
name|cache_name_pos
argument_list|(
name|old_name
argument_list|,
name|strlen
argument_list|(
name|old_name
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"file %s does not exist"
argument_list|,
name|old_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_name
operator|&&
operator|(
name|is_new
operator||
name|is_rename
operator||
name|is_copy
operator|)
condition|)
block|{
if|if
condition|(
name|cache_name_pos
argument_list|(
name|new_name
argument_list|,
name|strlen
argument_list|(
name|new_name
argument_list|)
argument_list|)
operator|>=
literal|0
condition|)
name|die
argument_list|(
literal|"file %s already exists"
argument_list|,
name|new_name
argument_list|)
expr_stmt|;
block|}
name|patch
operator|=
name|header
operator|+
name|hdrsize
expr_stmt|;
name|patchsize
operator|=
name|apply_single_patch
argument_list|(
name|patch
argument_list|,
name|size
operator|-
name|offset
operator|-
name|hdrsize
argument_list|)
expr_stmt|;
return|return
name|offset
operator|+
name|hdrsize
operator|+
name|patchsize
return|;
block|}
end_function
begin_function
DECL|function|apply_patch
specifier|static
name|int
name|apply_patch
parameter_list|(
name|int
name|fd
parameter_list|)
block|{
name|unsigned
name|long
name|offset
decl_stmt|,
name|size
decl_stmt|;
name|char
modifier|*
name|buffer
init|=
name|read_patch_file
argument_list|(
name|fd
argument_list|,
operator|&
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
return|return
operator|-
literal|1
return|;
name|offset
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|size
operator|>
literal|0
condition|)
block|{
name|int
name|nr
init|=
name|apply_chunk
argument_list|(
name|buffer
operator|+
name|offset
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|nr
operator|<
literal|0
condition|)
break|break;
name|offset
operator|+=
name|nr
expr_stmt|;
name|size
operator|-=
name|nr
expr_stmt|;
block|}
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
literal|0
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
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|read_stdin
init|=
literal|1
decl_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to read index file"
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
name|int
name|fd
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-"
argument_list|)
condition|)
block|{
name|apply_patch
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|read_stdin
operator|=
literal|0
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
literal|"--no-merge"
argument_list|)
condition|)
block|{
name|merge_patch
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
name|fd
operator|=
name|open
argument_list|(
name|arg
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
name|usage
argument_list|(
name|apply_usage
argument_list|)
expr_stmt|;
name|read_stdin
operator|=
literal|0
expr_stmt|;
name|apply_patch
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|read_stdin
condition|)
name|apply_patch
argument_list|(
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
