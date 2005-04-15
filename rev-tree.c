begin_unit
begin_define
DECL|macro|_XOPEN_SOURCE
define|#
directive|define
name|_XOPEN_SOURCE
end_define
begin_comment
DECL|macro|_XOPEN_SOURCE
comment|/* glibc2 needs this */
end_comment
begin_define
DECL|macro|_BSD_SOURCE
define|#
directive|define
name|_BSD_SOURCE
end_define
begin_comment
DECL|macro|_BSD_SOURCE
comment|/* for tm.tm_gmtoff */
end_comment
begin_include
include|#
directive|include
file|<time.h>
end_include
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
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_comment
comment|/*  * revision.h leaves the low 16 bits of the "flags" field of the  * revision data structure unused. We use it for a "reachable from  * this commit<N>" bitmask.  */
end_comment
begin_define
DECL|macro|MAX_COMMITS
define|#
directive|define
name|MAX_COMMITS
value|16
end_define
begin_decl_stmt
DECL|variable|show_edges
specifier|static
name|int
name|show_edges
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|basemask
specifier|static
name|int
name|basemask
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|parse_time
specifier|static
name|unsigned
name|long
name|parse_time
parameter_list|(
specifier|const
name|char
modifier|*
name|buf
parameter_list|)
block|{
name|char
name|c
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|char
name|buffer
index|[
literal|100
index|]
decl_stmt|;
name|struct
name|tm
name|tm
decl_stmt|;
specifier|const
name|char
modifier|*
name|formats
index|[]
init|=
block|{
literal|"%s"
block|,
literal|"%c"
block|,
literal|"%a %b %d %T %y"
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|fmt
init|=
name|formats
decl_stmt|;
name|p
operator|=
name|buffer
expr_stmt|;
while|while
condition|(
name|isspace
argument_list|(
name|c
operator|=
operator|*
name|buf
argument_list|)
condition|)
name|buf
operator|++
expr_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|buf
operator|++
operator|)
operator|!=
literal|'\n'
operator|&&
name|c
condition|)
operator|*
name|p
operator|++
operator|=
name|c
expr_stmt|;
operator|*
name|p
operator|++
operator|=
literal|0
expr_stmt|;
name|buf
operator|=
name|buffer
expr_stmt|;
name|memset
argument_list|(
operator|&
name|tm
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|tm
argument_list|)
argument_list|)
expr_stmt|;
do|do
block|{
specifier|const
name|char
modifier|*
name|next
init|=
name|strptime
argument_list|(
name|buf
argument_list|,
operator|*
name|fmt
argument_list|,
operator|&
name|tm
argument_list|)
decl_stmt|;
name|fmt
operator|++
expr_stmt|;
if|if
condition|(
name|next
condition|)
block|{
if|if
condition|(
operator|!
operator|*
name|next
condition|)
return|return
name|mktime
argument_list|(
operator|&
name|tm
argument_list|)
return|;
name|buf
operator|=
name|next
expr_stmt|;
block|}
block|}
do|while
condition|(
operator|*
name|buf
operator|&&
operator|*
name|fmt
condition|)
do|;
return|return
name|mktime
argument_list|(
operator|&
name|tm
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_commit_date
specifier|static
name|unsigned
name|long
name|parse_commit_date
parameter_list|(
specifier|const
name|char
modifier|*
name|buf
parameter_list|)
block|{
name|unsigned
name|long
name|time
decl_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|buf
argument_list|,
literal|"author"
argument_list|,
literal|6
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|buf
operator|++
operator|!=
literal|'\n'
condition|)
comment|/* nada */
empty_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|buf
argument_list|,
literal|"committer"
argument_list|,
literal|9
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|buf
operator|++
operator|!=
literal|'>'
condition|)
comment|/* nada */
empty_stmt|;
name|time
operator|=
name|strtoul
argument_list|(
name|buf
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|time
condition|)
name|time
operator|=
name|parse_time
argument_list|(
name|buf
argument_list|)
expr_stmt|;
return|return
name|time
return|;
block|}
end_function
begin_function
DECL|function|parse_commit
specifier|static
name|int
name|parse_commit
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|revision
modifier|*
name|rev
init|=
name|lookup_rev
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|rev
operator|->
name|flags
operator|&
name|SEEN
operator|)
condition|)
block|{
name|void
modifier|*
name|buffer
decl_stmt|,
modifier|*
name|bufptr
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|parent
index|[
literal|20
index|]
decl_stmt|;
name|rev
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|buffer
operator|=
name|bufptr
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
operator|||
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"commit"
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|bufptr
operator|+=
literal|46
expr_stmt|;
comment|/* "tree " + "hex sha1" + "\n" */
while|while
condition|(
operator|!
name|memcmp
argument_list|(
name|bufptr
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|bufptr
operator|+
literal|7
argument_list|,
name|parent
argument_list|)
condition|)
block|{
name|add_relationship
argument_list|(
name|rev
argument_list|,
name|parent
argument_list|)
expr_stmt|;
name|parse_commit
argument_list|(
name|parent
argument_list|)
expr_stmt|;
name|bufptr
operator|+=
literal|48
expr_stmt|;
comment|/* "parent " + "hex sha1" + "\n" */
block|}
name|rev
operator|->
name|date
operator|=
name|parse_commit_date
argument_list|(
name|bufptr
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_cache_file
specifier|static
name|void
name|read_cache_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|FILE
modifier|*
name|file
init|=
name|fopen
argument_list|(
name|path
argument_list|,
literal|"r"
argument_list|)
decl_stmt|;
name|char
name|line
index|[
literal|500
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|file
condition|)
name|die
argument_list|(
literal|"bad revtree cache file (%s)"
argument_list|,
name|path
argument_list|)
expr_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|file
argument_list|)
condition|)
block|{
name|unsigned
name|long
name|date
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|revision
modifier|*
name|rev
decl_stmt|;
specifier|const
name|char
modifier|*
name|buf
decl_stmt|;
if|if
condition|(
name|sscanf
argument_list|(
name|line
argument_list|,
literal|"%lu"
argument_list|,
operator|&
name|date
argument_list|)
operator|!=
literal|1
condition|)
break|break;
name|buf
operator|=
name|strchr
argument_list|(
name|line
argument_list|,
literal|' '
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
break|break;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|buf
operator|+
literal|1
argument_list|,
name|sha1
argument_list|)
condition|)
break|break;
name|rev
operator|=
name|lookup_rev
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|rev
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|rev
operator|->
name|date
operator|=
name|date
expr_stmt|;
comment|/* parents? */
while|while
condition|(
operator|(
name|buf
operator|=
name|strchr
argument_list|(
name|buf
operator|+
literal|1
argument_list|,
literal|' '
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|unsigned
name|char
name|parent
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|buf
operator|+
literal|1
argument_list|,
name|parent
argument_list|)
condition|)
break|break;
name|add_relationship
argument_list|(
name|rev
argument_list|,
name|parent
argument_list|)
expr_stmt|;
block|}
block|}
name|fclose
argument_list|(
name|file
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|mark_sha1_path
specifier|static
name|void
name|mark_sha1_path
parameter_list|(
name|struct
name|revision
modifier|*
name|rev
parameter_list|,
name|unsigned
name|int
name|mask
parameter_list|)
block|{
name|struct
name|parent
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|rev
operator|->
name|flags
operator|&
name|mask
condition|)
return|return;
name|rev
operator|->
name|flags
operator||=
name|mask
expr_stmt|;
name|p
operator|=
name|rev
operator|->
name|parent
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|mark_sha1_path
argument_list|(
name|p
operator|->
name|parent
argument_list|,
name|mask
argument_list|)
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * Some revisions are less interesting than others.  *  * For example, if we use a cache-file, that one may contain  * revisions that were never used. They are never interesting.  *  * And sometimes we're only interested in "edge" commits, ie  * places where the marking changes between parent and child.  */
end_comment
begin_function
DECL|function|interesting
specifier|static
name|int
name|interesting
parameter_list|(
name|struct
name|revision
modifier|*
name|rev
parameter_list|)
block|{
name|unsigned
name|mask
init|=
name|marked
argument_list|(
name|rev
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|mask
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|show_edges
condition|)
block|{
name|struct
name|parent
modifier|*
name|p
init|=
name|rev
operator|->
name|parent
decl_stmt|;
while|while
condition|(
name|p
condition|)
block|{
if|if
condition|(
name|mask
operator|!=
name|marked
argument_list|(
name|p
operator|->
name|parent
argument_list|)
condition|)
return|return
literal|1
return|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
if|if
condition|(
name|mask
operator|&
name|basemask
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * Usage: rev-tree [--edges] [--cache<cache-file>]<commit-id> [<commit-id2>]  *  * The cache-file can be quite important for big trees. This is an  * expensive operation if you have to walk the whole chain of  * parents in a tree with a long revision history.  */
end_comment
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
name|nr
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
name|MAX_COMMITS
index|]
index|[
literal|20
index|]
decl_stmt|;
comment|/* 	 * First - pick up all the revisions we can (both from 	 * caches and from commit file chains). 	 */
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
literal|"--cache"
argument_list|)
condition|)
block|{
name|read_cache_file
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|i
operator|++
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
literal|"--edges"
argument_list|)
condition|)
block|{
name|show_edges
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|==
literal|'^'
condition|)
block|{
name|arg
operator|++
expr_stmt|;
name|basemask
operator||=
literal|1
operator|<<
name|nr
expr_stmt|;
block|}
if|if
condition|(
name|nr
operator|>=
name|MAX_COMMITS
operator|||
name|get_sha1_hex
argument_list|(
name|arg
argument_list|,
name|sha1
index|[
name|nr
index|]
argument_list|)
condition|)
name|usage
argument_list|(
literal|"rev-tree [--edges] [--cache<cache-file>]<commit-id> [<commit-id>]"
argument_list|)
expr_stmt|;
name|parse_commit
argument_list|(
name|sha1
index|[
name|nr
index|]
argument_list|)
expr_stmt|;
name|nr
operator|++
expr_stmt|;
block|}
comment|/* 	 * Now we have the maximal tree. Walk the different sha files back to the root. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr
condition|;
name|i
operator|++
control|)
name|mark_sha1_path
argument_list|(
name|lookup_rev
argument_list|(
name|sha1
index|[
name|i
index|]
argument_list|)
argument_list|,
literal|1
operator|<<
name|i
argument_list|)
expr_stmt|;
comment|/* 	 * Now print out the results.. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_revs
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|revision
modifier|*
name|rev
init|=
name|revs
index|[
name|i
index|]
decl_stmt|;
name|struct
name|parent
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|interesting
argument_list|(
name|rev
argument_list|)
condition|)
continue|continue;
name|printf
argument_list|(
literal|"%lu %s:%d"
argument_list|,
name|rev
operator|->
name|date
argument_list|,
name|sha1_to_hex
argument_list|(
name|rev
operator|->
name|sha1
argument_list|)
argument_list|,
name|marked
argument_list|(
name|rev
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|=
name|rev
operator|->
name|parent
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|printf
argument_list|(
literal|" %s:%d"
argument_list|,
name|sha1_to_hex
argument_list|(
name|p
operator|->
name|parent
operator|->
name|sha1
argument_list|)
argument_list|,
name|marked
argument_list|(
name|p
operator|->
name|parent
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
