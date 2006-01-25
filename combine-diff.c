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
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_struct
DECL|struct|path_list
struct|struct
name|path_list
block|{
DECL|member|next
name|struct
name|path_list
modifier|*
name|next
decl_stmt|;
DECL|member|len
name|int
name|len
decl_stmt|;
DECL|member|path
name|char
modifier|*
name|path
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|parent_sha1
name|unsigned
name|char
name|parent_sha1
index|[
name|FLEX_ARRAY
index|]
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|uninteresting
specifier|static
name|int
name|uninteresting
parameter_list|(
name|struct
name|diff_filepair
modifier|*
name|p
parameter_list|)
block|{
if|if
condition|(
name|diff_unmodified_pair
argument_list|(
name|p
argument_list|)
condition|)
return|return
literal|1
return|;
if|if
condition|(
operator|!
name|S_ISREG
argument_list|(
name|p
operator|->
name|one
operator|->
name|mode
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|p
operator|->
name|two
operator|->
name|mode
argument_list|)
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|intersect_paths
specifier|static
name|struct
name|path_list
modifier|*
name|intersect_paths
parameter_list|(
name|struct
name|path_list
modifier|*
name|curr
parameter_list|,
name|int
name|n
parameter_list|,
name|int
name|num_parent
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
name|struct
name|path_list
modifier|*
name|p
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|n
condition|)
block|{
name|struct
name|path_list
modifier|*
name|list
init|=
name|NULL
decl_stmt|,
modifier|*
modifier|*
name|tail
init|=
operator|&
name|list
decl_stmt|;
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
name|int
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
if|if
condition|(
name|uninteresting
argument_list|(
name|q
operator|->
name|queue
index|[
name|i
index|]
argument_list|)
condition|)
continue|continue;
name|path
operator|=
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|two
operator|->
name|path
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|p
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|p
argument_list|)
operator|+
name|len
operator|+
literal|1
operator|+
name|num_parent
operator|*
literal|20
argument_list|)
expr_stmt|;
name|p
operator|->
name|path
operator|=
operator|(
name|char
operator|*
operator|)
operator|&
operator|(
name|p
operator|->
name|parent_sha1
index|[
name|num_parent
index|]
index|[
literal|0
index|]
operator|)
expr_stmt|;
name|memcpy
argument_list|(
name|p
operator|->
name|path
argument_list|,
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|p
operator|->
name|path
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
name|p
operator|->
name|len
operator|=
name|len
expr_stmt|;
name|p
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|memcpy
argument_list|(
name|p
operator|->
name|sha1
argument_list|,
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|two
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|p
operator|->
name|parent_sha1
index|[
name|n
index|]
argument_list|,
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|one
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
operator|*
name|tail
operator|=
name|p
expr_stmt|;
name|tail
operator|=
operator|&
name|p
operator|->
name|next
expr_stmt|;
block|}
return|return
name|list
return|;
block|}
for|for
control|(
name|p
operator|=
name|curr
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
name|int
name|found
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|p
operator|->
name|len
condition|)
continue|continue;
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
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
name|uninteresting
argument_list|(
name|q
operator|->
name|queue
index|[
name|i
index|]
argument_list|)
condition|)
continue|continue;
name|path
operator|=
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|two
operator|->
name|path
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|==
name|p
operator|->
name|len
operator|&&
operator|!
name|memcmp
argument_list|(
name|path
argument_list|,
name|p
operator|->
name|path
argument_list|,
name|len
argument_list|)
condition|)
block|{
name|found
operator|=
literal|1
expr_stmt|;
name|memcpy
argument_list|(
name|p
operator|->
name|parent_sha1
index|[
name|n
index|]
argument_list|,
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|one
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
if|if
condition|(
operator|!
name|found
condition|)
name|p
operator|->
name|len
operator|=
literal|0
expr_stmt|;
block|}
return|return
name|curr
return|;
block|}
end_function
begin_struct
DECL|struct|lline
struct|struct
name|lline
block|{
DECL|member|next
name|struct
name|lline
modifier|*
name|next
decl_stmt|;
DECL|member|len
name|int
name|len
decl_stmt|;
DECL|member|parent_map
name|unsigned
name|long
name|parent_map
decl_stmt|;
DECL|member|line
name|char
name|line
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|sline
struct|struct
name|sline
block|{
DECL|member|lost_head
DECL|member|lost_tail
name|struct
name|lline
modifier|*
name|lost_head
decl_stmt|,
modifier|*
modifier|*
name|lost_tail
decl_stmt|;
DECL|member|bol
name|char
modifier|*
name|bol
decl_stmt|;
DECL|member|len
name|int
name|len
decl_stmt|;
DECL|member|flag
name|unsigned
name|long
name|flag
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|grab_blob
specifier|static
name|char
modifier|*
name|grab_blob
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|char
modifier|*
name|blob
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|null_sha1
argument_list|,
literal|20
argument_list|)
condition|)
block|{
comment|/* deleted blob */
operator|*
name|size
operator|=
literal|0
expr_stmt|;
return|return
name|xcalloc
argument_list|(
literal|1
argument_list|,
literal|1
argument_list|)
return|;
block|}
name|blob
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"blob"
argument_list|)
condition|)
name|die
argument_list|(
literal|"object '%s' is not a blob!"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|blob
return|;
block|}
end_function
begin_define
DECL|macro|TMPPATHLEN
define|#
directive|define
name|TMPPATHLEN
value|50
end_define
begin_define
DECL|macro|MAXLINELEN
define|#
directive|define
name|MAXLINELEN
value|10240
end_define
begin_function
DECL|function|write_to_temp_file
specifier|static
name|void
name|write_to_temp_file
parameter_list|(
name|char
modifier|*
name|tmpfile
parameter_list|,
name|void
modifier|*
name|blob
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|int
name|fd
init|=
name|git_mkstemp
argument_list|(
name|tmpfile
argument_list|,
name|TMPPATHLEN
argument_list|,
literal|".diff_XXXXXX"
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to create temp-file"
argument_list|)
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|blob
argument_list|,
name|size
argument_list|)
operator|!=
name|size
condition|)
name|die
argument_list|(
literal|"unable to write temp-file"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_temp_blob
specifier|static
name|void
name|write_temp_blob
parameter_list|(
name|char
modifier|*
name|tmpfile
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|;
name|void
modifier|*
name|blob
decl_stmt|;
name|blob
operator|=
name|grab_blob
argument_list|(
name|sha1
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
name|write_to_temp_file
argument_list|(
name|tmpfile
argument_list|,
name|blob
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|blob
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_num
specifier|static
name|int
name|parse_num
parameter_list|(
name|char
modifier|*
modifier|*
name|cp_p
parameter_list|,
name|unsigned
name|int
modifier|*
name|num_p
parameter_list|)
block|{
name|char
modifier|*
name|cp
init|=
operator|*
name|cp_p
decl_stmt|;
name|unsigned
name|int
name|num
init|=
literal|0
decl_stmt|;
name|int
name|read_some
decl_stmt|;
while|while
condition|(
literal|'0'
operator|<=
operator|*
name|cp
operator|&&
operator|*
name|cp
operator|<=
literal|'9'
condition|)
name|num
operator|=
name|num
operator|*
literal|10
operator|+
operator|*
name|cp
operator|++
operator|-
literal|'0'
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|read_some
operator|=
name|cp
operator|-
operator|*
name|cp_p
operator|)
condition|)
return|return
operator|-
literal|1
return|;
operator|*
name|cp_p
operator|=
name|cp
expr_stmt|;
operator|*
name|num_p
operator|=
name|num
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_hunk_header
specifier|static
name|int
name|parse_hunk_header
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
modifier|*
name|ob
parameter_list|,
name|unsigned
name|int
modifier|*
name|on
parameter_list|,
name|unsigned
name|int
modifier|*
name|nb
parameter_list|,
name|unsigned
name|int
modifier|*
name|nn
parameter_list|)
block|{
name|char
modifier|*
name|cp
decl_stmt|;
name|cp
operator|=
name|line
operator|+
literal|4
expr_stmt|;
if|if
condition|(
name|parse_num
argument_list|(
operator|&
name|cp
argument_list|,
name|ob
argument_list|)
condition|)
block|{
name|bad_line
label|:
return|return
name|error
argument_list|(
literal|"malformed diff output: %s"
argument_list|,
name|line
argument_list|)
return|;
block|}
if|if
condition|(
operator|*
name|cp
operator|==
literal|','
condition|)
block|{
name|cp
operator|++
expr_stmt|;
if|if
condition|(
name|parse_num
argument_list|(
operator|&
name|cp
argument_list|,
name|on
argument_list|)
condition|)
goto|goto
name|bad_line
goto|;
block|}
else|else
operator|*
name|on
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|*
name|cp
operator|++
operator|!=
literal|' '
operator|||
operator|*
name|cp
operator|++
operator|!=
literal|'+'
condition|)
goto|goto
name|bad_line
goto|;
if|if
condition|(
name|parse_num
argument_list|(
operator|&
name|cp
argument_list|,
name|nb
argument_list|)
condition|)
goto|goto
name|bad_line
goto|;
if|if
condition|(
operator|*
name|cp
operator|==
literal|','
condition|)
block|{
name|cp
operator|++
expr_stmt|;
if|if
condition|(
name|parse_num
argument_list|(
operator|&
name|cp
argument_list|,
name|nn
argument_list|)
condition|)
goto|goto
name|bad_line
goto|;
block|}
else|else
operator|*
name|nn
operator|=
literal|1
expr_stmt|;
return|return
operator|-
operator|!
operator|!
name|memcmp
argument_list|(
name|cp
argument_list|,
literal|" @@"
argument_list|,
literal|3
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|append_lost
specifier|static
name|void
name|append_lost
parameter_list|(
name|struct
name|sline
modifier|*
name|sline
parameter_list|,
name|int
name|n
parameter_list|,
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|struct
name|lline
modifier|*
name|lline
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|line
argument_list|)
decl_stmt|;
name|unsigned
name|long
name|this_mask
init|=
operator|(
literal|1UL
operator|<<
name|n
operator|)
decl_stmt|;
if|if
condition|(
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|len
operator|--
expr_stmt|;
comment|/* Check to see if we can squash things */
if|if
condition|(
name|sline
operator|->
name|lost_head
condition|)
block|{
name|struct
name|lline
modifier|*
name|last_one
init|=
name|NULL
decl_stmt|;
comment|/* We cannot squash it with earlier one */
for|for
control|(
name|lline
operator|=
name|sline
operator|->
name|lost_head
init|;
name|lline
condition|;
name|lline
operator|=
name|lline
operator|->
name|next
control|)
if|if
condition|(
name|lline
operator|->
name|parent_map
operator|&
name|this_mask
condition|)
name|last_one
operator|=
name|lline
expr_stmt|;
name|lline
operator|=
name|last_one
condition|?
name|last_one
operator|->
name|next
else|:
name|sline
operator|->
name|lost_head
expr_stmt|;
while|while
condition|(
name|lline
condition|)
block|{
if|if
condition|(
name|lline
operator|->
name|len
operator|==
name|len
operator|&&
operator|!
name|memcmp
argument_list|(
name|lline
operator|->
name|line
argument_list|,
name|line
argument_list|,
name|len
argument_list|)
condition|)
block|{
name|lline
operator|->
name|parent_map
operator||=
name|this_mask
expr_stmt|;
return|return;
block|}
name|lline
operator|=
name|lline
operator|->
name|next
expr_stmt|;
block|}
block|}
name|lline
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|lline
argument_list|)
operator|+
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|lline
operator|->
name|len
operator|=
name|len
expr_stmt|;
name|lline
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|lline
operator|->
name|parent_map
operator|=
name|this_mask
expr_stmt|;
name|memcpy
argument_list|(
name|lline
operator|->
name|line
argument_list|,
name|line
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|lline
operator|->
name|line
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
operator|*
name|sline
operator|->
name|lost_tail
operator|=
name|lline
expr_stmt|;
name|sline
operator|->
name|lost_tail
operator|=
operator|&
name|lline
operator|->
name|next
expr_stmt|;
block|}
end_function
begin_function
DECL|function|combine_diff
specifier|static
name|void
name|combine_diff
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|parent
parameter_list|,
specifier|const
name|char
modifier|*
name|ourtmp
parameter_list|,
name|struct
name|sline
modifier|*
name|sline
parameter_list|,
name|int
name|cnt
parameter_list|,
name|int
name|n
parameter_list|)
block|{
name|FILE
modifier|*
name|in
decl_stmt|;
name|char
name|parent_tmp
index|[
name|TMPPATHLEN
index|]
decl_stmt|;
name|char
name|cmd
index|[
name|TMPPATHLEN
operator|*
literal|2
operator|+
literal|1024
index|]
decl_stmt|;
name|char
name|line
index|[
name|MAXLINELEN
index|]
decl_stmt|;
name|unsigned
name|int
name|lno
decl_stmt|,
name|ob
decl_stmt|,
name|on
decl_stmt|,
name|nb
decl_stmt|,
name|nn
decl_stmt|;
name|unsigned
name|long
name|pmask
init|=
operator|~
operator|(
literal|1UL
operator|<<
name|n
operator|)
decl_stmt|;
name|struct
name|sline
modifier|*
name|lost_bucket
init|=
name|NULL
decl_stmt|;
name|write_temp_blob
argument_list|(
name|parent_tmp
argument_list|,
name|parent
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|cmd
argument_list|,
literal|"diff --unified=0 -La/x -Lb/x '%s' '%s'"
argument_list|,
name|parent_tmp
argument_list|,
name|ourtmp
argument_list|)
expr_stmt|;
name|in
operator|=
name|popen
argument_list|(
name|cmd
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|in
condition|)
return|return;
name|lno
operator|=
literal|1
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
name|in
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|line
argument_list|)
decl_stmt|;
if|if
condition|(
literal|5
operator|<
name|len
operator|&&
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
if|if
condition|(
name|parse_hunk_header
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
operator|&
name|ob
argument_list|,
operator|&
name|on
argument_list|,
operator|&
name|nb
argument_list|,
operator|&
name|nn
argument_list|)
condition|)
break|break;
name|lno
operator|=
name|nb
expr_stmt|;
if|if
condition|(
operator|!
name|nb
condition|)
block|{
comment|/* @@ -1,2 +0,0 @@ to remove the 				 * first two lines... 				 */
name|nb
operator|=
literal|1
expr_stmt|;
block|}
name|lost_bucket
operator|=
operator|&
name|sline
index|[
name|nb
operator|-
literal|1
index|]
expr_stmt|;
comment|/* sline is 0 based */
continue|continue;
block|}
if|if
condition|(
operator|!
name|lost_bucket
condition|)
continue|continue;
switch|switch
condition|(
name|line
index|[
literal|0
index|]
condition|)
block|{
case|case
literal|'-'
case|:
name|append_lost
argument_list|(
name|lost_bucket
argument_list|,
name|n
argument_list|,
name|line
operator|+
literal|1
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'+'
case|:
name|sline
index|[
name|lno
operator|-
literal|1
index|]
operator|.
name|flag
operator|&=
name|pmask
expr_stmt|;
name|lno
operator|++
expr_stmt|;
break|break;
block|}
block|}
name|fclose
argument_list|(
name|in
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|parent_tmp
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|context
specifier|static
name|unsigned
name|long
name|context
init|=
literal|3
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|combine_marker
specifier|static
name|char
name|combine_marker
init|=
literal|'@'
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|interesting
specifier|static
name|int
name|interesting
parameter_list|(
name|struct
name|sline
modifier|*
name|sline
parameter_list|,
name|unsigned
name|long
name|all_mask
parameter_list|)
block|{
return|return
operator|(
operator|(
name|sline
operator|->
name|flag
operator|&
name|all_mask
operator|)
operator|!=
name|all_mask
operator|||
name|sline
operator|->
name|lost_head
operator|)
return|;
block|}
end_function
begin_function
DECL|function|line_common_diff
specifier|static
name|unsigned
name|long
name|line_common_diff
parameter_list|(
name|struct
name|sline
modifier|*
name|sline
parameter_list|,
name|unsigned
name|long
name|all_mask
parameter_list|)
block|{
comment|/* 	 * Look at the line and see from which parents we have the 	 * same difference. 	 */
comment|/* Lower bits of sline->flag records if the parent had this 	 * line, so XOR with all_mask gives us on-bits for parents we 	 * have differences with. 	 */
name|unsigned
name|long
name|common_adds
init|=
operator|(
name|sline
operator|->
name|flag
operator|^
name|all_mask
operator|)
operator|&
name|all_mask
decl_stmt|;
name|unsigned
name|long
name|common_removes
init|=
name|all_mask
decl_stmt|;
comment|/* If all the parents have this line, that also counts as 	 * having the same difference. 	 */
if|if
condition|(
operator|!
name|common_adds
condition|)
name|common_adds
operator|=
name|all_mask
expr_stmt|;
if|if
condition|(
name|sline
operator|->
name|lost_head
condition|)
block|{
comment|/* Lost head list records the lines removed from 		 * the parents, and parent_map records from which 		 * parent the line was removed. 		 */
name|struct
name|lline
modifier|*
name|ll
decl_stmt|;
for|for
control|(
name|ll
operator|=
name|sline
operator|->
name|lost_head
init|;
name|ll
condition|;
name|ll
operator|=
name|ll
operator|->
name|next
control|)
block|{
name|common_removes
operator|&=
name|ll
operator|->
name|parent_map
expr_stmt|;
block|}
block|}
return|return
name|common_adds
operator|&
name|common_removes
return|;
block|}
end_function
begin_function
DECL|function|line_all_diff
specifier|static
name|unsigned
name|long
name|line_all_diff
parameter_list|(
name|struct
name|sline
modifier|*
name|sline
parameter_list|,
name|unsigned
name|long
name|all_mask
parameter_list|)
block|{
comment|/* 	 * Look at the line and see from which parents we have some difference. 	 */
name|unsigned
name|long
name|different
init|=
operator|(
name|sline
operator|->
name|flag
operator|^
name|all_mask
operator|)
operator|&
name|all_mask
decl_stmt|;
if|if
condition|(
name|sline
operator|->
name|lost_head
condition|)
block|{
comment|/* Lost head list records the lines removed from 		 * the parents, and parent_map records from which 		 * parent the line was removed. 		 */
name|struct
name|lline
modifier|*
name|ll
decl_stmt|;
for|for
control|(
name|ll
operator|=
name|sline
operator|->
name|lost_head
init|;
name|ll
condition|;
name|ll
operator|=
name|ll
operator|->
name|next
control|)
block|{
name|different
operator||=
name|ll
operator|->
name|parent_map
expr_stmt|;
block|}
block|}
return|return
name|different
return|;
block|}
end_function
begin_function
DECL|function|make_hunks
specifier|static
name|void
name|make_hunks
parameter_list|(
name|struct
name|sline
modifier|*
name|sline
parameter_list|,
name|unsigned
name|long
name|cnt
parameter_list|,
name|int
name|num_parent
parameter_list|,
name|int
name|dense
parameter_list|)
block|{
name|unsigned
name|long
name|all_mask
init|=
operator|(
literal|1UL
operator|<<
name|num_parent
operator|)
operator|-
literal|1
decl_stmt|;
name|unsigned
name|long
name|mark
init|=
operator|(
literal|1UL
operator|<<
name|num_parent
operator|)
decl_stmt|;
name|unsigned
name|long
name|i
decl_stmt|;
name|i
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|cnt
condition|)
block|{
if|if
condition|(
name|interesting
argument_list|(
operator|&
name|sline
index|[
name|i
index|]
argument_list|,
name|all_mask
argument_list|)
condition|)
block|{
name|unsigned
name|long
name|j
init|=
operator|(
name|context
operator|<
name|i
operator|)
condition|?
name|i
operator|-
name|context
else|:
literal|0
decl_stmt|;
while|while
condition|(
name|j
operator|<=
name|i
condition|)
name|sline
index|[
name|j
operator|++
index|]
operator|.
name|flag
operator||=
name|mark
expr_stmt|;
while|while
condition|(
operator|++
name|i
operator|<
name|cnt
condition|)
block|{
if|if
condition|(
operator|!
name|interesting
argument_list|(
operator|&
name|sline
index|[
name|i
index|]
argument_list|,
name|all_mask
argument_list|)
condition|)
break|break;
name|sline
index|[
name|i
index|]
operator|.
name|flag
operator||=
name|mark
expr_stmt|;
block|}
name|j
operator|=
operator|(
name|i
operator|+
name|context
operator|<
name|cnt
operator|)
condition|?
name|i
operator|+
name|context
else|:
name|cnt
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|j
condition|)
name|sline
index|[
name|i
operator|++
index|]
operator|.
name|flag
operator||=
name|mark
expr_stmt|;
continue|continue;
block|}
name|i
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|dense
condition|)
return|return;
comment|/* Look at each hunk, and if we have changes from only one 	 * parent, or the changes are the same from all but one 	 * parent, mark that uninteresting. 	 */
name|i
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|cnt
condition|)
block|{
name|int
name|j
decl_stmt|,
name|hunk_end
decl_stmt|,
name|same
decl_stmt|,
name|diff
decl_stmt|;
name|unsigned
name|long
name|same_diff
decl_stmt|,
name|all_diff
decl_stmt|,
name|this_diff
decl_stmt|;
while|while
condition|(
name|i
operator|<
name|cnt
operator|&&
operator|!
operator|(
name|sline
index|[
name|i
index|]
operator|.
name|flag
operator|&
name|mark
operator|)
condition|)
name|i
operator|++
expr_stmt|;
if|if
condition|(
name|cnt
operator|<=
name|i
condition|)
break|break;
comment|/* No more interesting hunks */
for|for
control|(
name|hunk_end
operator|=
name|i
operator|+
literal|1
init|;
name|hunk_end
operator|<
name|cnt
condition|;
name|hunk_end
operator|++
control|)
if|if
condition|(
operator|!
operator|(
name|sline
index|[
name|hunk_end
index|]
operator|.
name|flag
operator|&
name|mark
operator|)
condition|)
break|break;
comment|/* [i..hunk_end) are interesting.  Now does it have 		 * the same change with all but one parent? 		 */
name|same_diff
operator|=
name|all_mask
expr_stmt|;
name|all_diff
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|j
operator|=
name|i
init|;
name|j
operator|<
name|hunk_end
condition|;
name|j
operator|++
control|)
block|{
name|same_diff
operator|&=
name|line_common_diff
argument_list|(
name|sline
operator|+
name|j
argument_list|,
name|all_mask
argument_list|)
expr_stmt|;
name|all_diff
operator||=
name|line_all_diff
argument_list|(
name|sline
operator|+
name|j
argument_list|,
name|all_mask
argument_list|)
expr_stmt|;
block|}
name|diff
operator|=
name|same
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|num_parent
condition|;
name|j
operator|++
control|)
block|{
if|if
condition|(
name|same_diff
operator|&
operator|(
literal|1UL
operator|<<
name|j
operator|)
condition|)
name|same
operator|++
expr_stmt|;
if|if
condition|(
name|all_diff
operator|&
operator|(
literal|1UL
operator|<<
name|j
operator|)
condition|)
name|diff
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|num_parent
operator|-
literal|1
operator|<=
name|same
operator|)
operator|||
operator|(
name|diff
operator|==
literal|1
operator|)
condition|)
block|{
comment|/* This hunk is not that interesting after all */
for|for
control|(
name|j
operator|=
name|i
init|;
name|j
operator|<
name|hunk_end
condition|;
name|j
operator|++
control|)
name|sline
index|[
name|j
index|]
operator|.
name|flag
operator|&=
operator|~
name|mark
expr_stmt|;
block|}
name|i
operator|=
name|hunk_end
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|dump_sline
specifier|static
name|void
name|dump_sline
parameter_list|(
name|struct
name|sline
modifier|*
name|sline
parameter_list|,
name|int
name|cnt
parameter_list|,
name|int
name|num_parent
parameter_list|)
block|{
name|unsigned
name|long
name|mark
init|=
operator|(
literal|1UL
operator|<<
name|num_parent
operator|)
decl_stmt|;
name|int
name|i
decl_stmt|;
name|int
name|lno
init|=
literal|0
decl_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|struct
name|sline
modifier|*
name|sl
init|=
operator|&
name|sline
index|[
name|lno
index|]
decl_stmt|;
name|int
name|hunk_end
decl_stmt|;
while|while
condition|(
name|lno
operator|<
name|cnt
operator|&&
operator|!
operator|(
name|sline
index|[
name|lno
index|]
operator|.
name|flag
operator|&
name|mark
operator|)
condition|)
name|lno
operator|++
expr_stmt|;
if|if
condition|(
name|cnt
operator|<=
name|lno
condition|)
break|break;
for|for
control|(
name|hunk_end
operator|=
name|lno
operator|+
literal|1
init|;
name|hunk_end
operator|<
name|cnt
condition|;
name|hunk_end
operator|++
control|)
if|if
condition|(
operator|!
operator|(
name|sline
index|[
name|hunk_end
index|]
operator|.
name|flag
operator|&
name|mark
operator|)
condition|)
break|break;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<=
name|num_parent
condition|;
name|i
operator|++
control|)
name|putchar
argument_list|(
name|combine_marker
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|" +%d,%d "
argument_list|,
name|lno
operator|+
literal|1
argument_list|,
name|hunk_end
operator|-
name|lno
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<=
name|num_parent
condition|;
name|i
operator|++
control|)
name|putchar
argument_list|(
name|combine_marker
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
while|while
condition|(
name|lno
operator|<
name|hunk_end
condition|)
block|{
name|struct
name|lline
modifier|*
name|ll
decl_stmt|;
name|int
name|j
decl_stmt|;
name|sl
operator|=
operator|&
name|sline
index|[
name|lno
operator|++
index|]
expr_stmt|;
name|ll
operator|=
name|sl
operator|->
name|lost_head
expr_stmt|;
while|while
condition|(
name|ll
condition|)
block|{
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|num_parent
condition|;
name|j
operator|++
control|)
block|{
if|if
condition|(
name|ll
operator|->
name|parent_map
operator|&
operator|(
literal|1UL
operator|<<
name|j
operator|)
condition|)
name|putchar
argument_list|(
literal|'-'
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
block|}
name|puts
argument_list|(
name|ll
operator|->
name|line
argument_list|)
expr_stmt|;
name|ll
operator|=
name|ll
operator|->
name|next
expr_stmt|;
block|}
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|num_parent
condition|;
name|j
operator|++
control|)
block|{
if|if
condition|(
operator|(
literal|1UL
operator|<<
name|j
operator|)
operator|&
name|sl
operator|->
name|flag
condition|)
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|'+'
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"%.*s\n"
argument_list|,
name|sl
operator|->
name|len
argument_list|,
name|sl
operator|->
name|bol
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|show_combined_diff
specifier|static
name|void
name|show_combined_diff
parameter_list|(
name|struct
name|path_list
modifier|*
name|elem
parameter_list|,
name|int
name|num_parent
parameter_list|,
name|int
name|dense
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|,
name|cnt
decl_stmt|,
name|lno
decl_stmt|;
name|char
modifier|*
name|result
decl_stmt|,
modifier|*
name|cp
decl_stmt|,
modifier|*
name|ep
decl_stmt|;
name|struct
name|sline
modifier|*
name|sline
decl_stmt|;
comment|/* survived lines */
name|int
name|i
decl_stmt|;
name|char
name|ourtmp
index|[
name|TMPPATHLEN
index|]
decl_stmt|;
comment|/* Read the result of merge first */
name|result
operator|=
name|grab_blob
argument_list|(
name|elem
operator|->
name|sha1
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
name|write_to_temp_file
argument_list|(
name|ourtmp
argument_list|,
name|result
argument_list|,
name|size
argument_list|)
expr_stmt|;
for|for
control|(
name|cnt
operator|=
literal|0
operator|,
name|cp
operator|=
name|result
init|;
name|cp
operator|-
name|result
operator|<
name|size
condition|;
name|cp
operator|++
control|)
block|{
if|if
condition|(
operator|*
name|cp
operator|==
literal|'\n'
condition|)
name|cnt
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|result
index|[
name|size
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|cnt
operator|++
expr_stmt|;
comment|/* incomplete line */
name|sline
operator|=
name|xcalloc
argument_list|(
name|cnt
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|sline
argument_list|)
argument_list|)
expr_stmt|;
name|ep
operator|=
name|result
expr_stmt|;
name|sline
index|[
literal|0
index|]
operator|.
name|bol
operator|=
name|result
expr_stmt|;
for|for
control|(
name|lno
operator|=
literal|0
operator|,
name|cp
operator|=
name|result
init|;
name|cp
operator|-
name|result
operator|<
name|size
condition|;
name|cp
operator|++
control|)
block|{
if|if
condition|(
operator|*
name|cp
operator|==
literal|'\n'
condition|)
block|{
name|sline
index|[
name|lno
index|]
operator|.
name|lost_tail
operator|=
operator|&
name|sline
index|[
name|lno
index|]
operator|.
name|lost_head
expr_stmt|;
name|sline
index|[
name|lno
index|]
operator|.
name|len
operator|=
name|cp
operator|-
name|sline
index|[
name|lno
index|]
operator|.
name|bol
expr_stmt|;
name|sline
index|[
name|lno
index|]
operator|.
name|flag
operator|=
operator|(
literal|1UL
operator|<<
name|num_parent
operator|)
operator|-
literal|1
expr_stmt|;
name|lno
operator|++
expr_stmt|;
if|if
condition|(
name|lno
operator|<
name|cnt
condition|)
name|sline
index|[
name|lno
index|]
operator|.
name|bol
operator|=
name|cp
operator|+
literal|1
expr_stmt|;
block|}
block|}
if|if
condition|(
name|result
index|[
name|size
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
block|{
name|sline
index|[
name|cnt
operator|-
literal|1
index|]
operator|.
name|lost_tail
operator|=
operator|&
name|sline
index|[
name|cnt
operator|-
literal|1
index|]
operator|.
name|lost_head
expr_stmt|;
name|sline
index|[
name|cnt
operator|-
literal|1
index|]
operator|.
name|len
operator|=
name|size
operator|-
operator|(
name|sline
index|[
name|cnt
operator|-
literal|1
index|]
operator|.
name|bol
operator|-
name|result
operator|)
expr_stmt|;
name|sline
index|[
name|cnt
operator|-
literal|1
index|]
operator|.
name|flag
operator|=
operator|(
literal|1UL
operator|<<
name|num_parent
operator|)
operator|-
literal|1
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_parent
condition|;
name|i
operator|++
control|)
name|combine_diff
argument_list|(
name|elem
operator|->
name|parent_sha1
index|[
name|i
index|]
argument_list|,
name|ourtmp
argument_list|,
name|sline
argument_list|,
name|cnt
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|make_hunks
argument_list|(
name|sline
argument_list|,
name|cnt
argument_list|,
name|num_parent
argument_list|,
name|dense
argument_list|)
expr_stmt|;
name|dump_sline
argument_list|(
name|sline
argument_list|,
name|cnt
argument_list|,
name|num_parent
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|ourtmp
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|result
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
name|cnt
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|sline
index|[
name|i
index|]
operator|.
name|lost_head
condition|)
block|{
name|struct
name|lline
modifier|*
name|ll
init|=
name|sline
index|[
name|i
index|]
operator|.
name|lost_head
decl_stmt|;
while|while
condition|(
name|ll
condition|)
block|{
name|struct
name|lline
modifier|*
name|tmp
init|=
name|ll
decl_stmt|;
name|ll
operator|=
name|ll
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
block|}
block|}
block|}
name|free
argument_list|(
name|sline
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diff_tree_combined_merge
name|int
name|diff_tree_combined_merge
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|header
parameter_list|,
name|int
name|show_empty_merge
parameter_list|,
name|int
name|dense
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|diff_options
name|diffopts
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|struct
name|path_list
modifier|*
name|p
decl_stmt|,
modifier|*
name|paths
init|=
name|NULL
decl_stmt|;
name|int
name|num_parent
decl_stmt|,
name|i
decl_stmt|,
name|num_paths
decl_stmt|;
name|diff_setup
argument_list|(
operator|&
name|diffopts
argument_list|)
expr_stmt|;
name|diffopts
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_NO_OUTPUT
expr_stmt|;
name|diffopts
operator|.
name|recursive
operator|=
literal|1
expr_stmt|;
comment|/* count parents */
for|for
control|(
name|parents
operator|=
name|commit
operator|->
name|parents
operator|,
name|num_parent
operator|=
literal|0
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
operator|,
name|num_parent
operator|++
control|)
empty_stmt|;
comment|/* nothing */
comment|/* find set of paths that everybody touches */
for|for
control|(
name|parents
operator|=
name|commit
operator|->
name|parents
operator|,
name|i
operator|=
literal|0
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
operator|,
name|i
operator|++
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
name|diff_tree_sha1
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|""
argument_list|,
operator|&
name|diffopts
argument_list|)
expr_stmt|;
name|paths
operator|=
name|intersect_paths
argument_list|(
name|paths
argument_list|,
name|i
argument_list|,
name|num_parent
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|diffopts
argument_list|)
expr_stmt|;
block|}
comment|/* find out surviving paths */
for|for
control|(
name|num_paths
operator|=
literal|0
operator|,
name|p
operator|=
name|paths
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
name|p
operator|->
name|len
condition|)
name|num_paths
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|num_paths
operator|||
name|show_empty_merge
condition|)
block|{
name|puts
argument_list|(
name|header
argument_list|)
expr_stmt|;
for|for
control|(
name|p
operator|=
name|paths
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|p
operator|->
name|len
condition|)
continue|continue;
name|printf
argument_list|(
literal|"diff --%s "
argument_list|,
name|dense
condition|?
literal|"cc"
else|:
literal|"combined"
argument_list|)
expr_stmt|;
if|if
condition|(
name|quote_c_style
argument_list|(
name|p
operator|->
name|path
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
condition|)
name|quote_c_style
argument_list|(
name|p
operator|->
name|path
argument_list|,
name|NULL
argument_list|,
name|stdout
argument_list|,
literal|0
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|p
operator|->
name|path
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|show_combined_diff
argument_list|(
name|p
argument_list|,
name|num_parent
argument_list|,
name|dense
argument_list|)
expr_stmt|;
block|}
block|}
comment|/* Clean things up */
while|while
condition|(
name|paths
condition|)
block|{
name|struct
name|path_list
modifier|*
name|tmp
init|=
name|paths
decl_stmt|;
name|paths
operator|=
name|paths
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
