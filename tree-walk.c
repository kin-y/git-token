begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"unpack-trees.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_function
DECL|function|get_mode
specifier|static
specifier|const
name|char
modifier|*
name|get_mode
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|unsigned
name|int
modifier|*
name|modep
parameter_list|)
block|{
name|unsigned
name|char
name|c
decl_stmt|;
name|unsigned
name|int
name|mode
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|*
name|str
operator|==
literal|' '
condition|)
return|return
name|NULL
return|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|str
operator|++
operator|)
operator|!=
literal|' '
condition|)
block|{
if|if
condition|(
name|c
operator|<
literal|'0'
operator|||
name|c
operator|>
literal|'7'
condition|)
return|return
name|NULL
return|;
name|mode
operator|=
operator|(
name|mode
operator|<<
literal|3
operator|)
operator|+
operator|(
name|c
operator|-
literal|'0'
operator|)
expr_stmt|;
block|}
operator|*
name|modep
operator|=
name|mode
expr_stmt|;
return|return
name|str
return|;
block|}
end_function
begin_function
DECL|function|decode_tree_entry
specifier|static
name|void
name|decode_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|,
name|len
decl_stmt|;
if|if
condition|(
name|size
operator|<
literal|24
operator|||
name|buf
index|[
name|size
operator|-
literal|21
index|]
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
name|path
operator|=
name|get_mode
argument_list|(
name|buf
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|path
operator|||
operator|!
operator|*
name|path
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
operator|+
literal|1
expr_stmt|;
comment|/* Initialize the descriptor entry */
name|desc
operator|->
name|entry
operator|.
name|path
operator|=
name|path
expr_stmt|;
name|desc
operator|->
name|entry
operator|.
name|mode
operator|=
name|mode
expr_stmt|;
name|desc
operator|->
name|entry
operator|.
name|sha1
operator|=
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
operator|(
name|path
operator|+
name|len
operator|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|init_tree_desc
name|void
name|init_tree_desc
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|desc
operator|->
name|buffer
operator|=
name|buffer
expr_stmt|;
name|desc
operator|->
name|size
operator|=
name|size
expr_stmt|;
if|if
condition|(
name|size
condition|)
name|decode_tree_entry
argument_list|(
name|desc
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fill_tree_descriptor
name|void
modifier|*
name|fill_tree_descriptor
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
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
init|=
literal|0
decl_stmt|;
name|void
modifier|*
name|buf
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|sha1
condition|)
block|{
name|buf
operator|=
name|read_object_with_reference
argument_list|(
name|sha1
argument_list|,
name|tree_type
argument_list|,
operator|&
name|size
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
name|die
argument_list|(
literal|"unable to read tree %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|init_tree_desc
argument_list|(
name|desc
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_function
DECL|function|entry_clear
specifier|static
name|void
name|entry_clear
parameter_list|(
name|struct
name|name_entry
modifier|*
name|a
parameter_list|)
block|{
name|memset
argument_list|(
name|a
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|a
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|entry_extract
specifier|static
name|void
name|entry_extract
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
name|struct
name|name_entry
modifier|*
name|a
parameter_list|)
block|{
operator|*
name|a
operator|=
name|t
operator|->
name|entry
expr_stmt|;
block|}
end_function
begin_function
DECL|function|update_tree_entry
name|void
name|update_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|)
block|{
specifier|const
name|void
modifier|*
name|buf
init|=
name|desc
operator|->
name|buffer
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|end
init|=
name|desc
operator|->
name|entry
operator|.
name|sha1
operator|+
literal|20
decl_stmt|;
name|unsigned
name|long
name|size
init|=
name|desc
operator|->
name|size
decl_stmt|;
name|unsigned
name|long
name|len
init|=
name|end
operator|-
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|buf
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|len
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
name|buf
operator|=
name|end
expr_stmt|;
name|size
operator|-=
name|len
expr_stmt|;
name|desc
operator|->
name|buffer
operator|=
name|buf
expr_stmt|;
name|desc
operator|->
name|size
operator|=
name|size
expr_stmt|;
if|if
condition|(
name|size
condition|)
name|decode_tree_entry
argument_list|(
name|desc
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|tree_entry
name|int
name|tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
name|struct
name|name_entry
modifier|*
name|entry
parameter_list|)
block|{
if|if
condition|(
operator|!
name|desc
operator|->
name|size
condition|)
return|return
literal|0
return|;
operator|*
name|entry
operator|=
name|desc
operator|->
name|entry
expr_stmt|;
name|update_tree_entry
argument_list|(
name|desc
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|setup_traverse_info
name|void
name|setup_traverse_info
parameter_list|(
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|int
name|pathlen
init|=
name|strlen
argument_list|(
name|base
argument_list|)
decl_stmt|;
specifier|static
name|struct
name|traverse_info
name|dummy
decl_stmt|;
name|memset
argument_list|(
name|info
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|info
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathlen
operator|&&
name|base
index|[
name|pathlen
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|pathlen
operator|--
expr_stmt|;
name|info
operator|->
name|pathlen
operator|=
name|pathlen
condition|?
name|pathlen
operator|+
literal|1
else|:
literal|0
expr_stmt|;
name|info
operator|->
name|name
operator|.
name|path
operator|=
name|base
expr_stmt|;
name|info
operator|->
name|name
operator|.
name|sha1
operator|=
operator|(
name|void
operator|*
operator|)
operator|(
name|base
operator|+
name|pathlen
operator|+
literal|1
operator|)
expr_stmt|;
if|if
condition|(
name|pathlen
condition|)
name|info
operator|->
name|prev
operator|=
operator|&
name|dummy
expr_stmt|;
block|}
end_function
begin_function
DECL|function|make_traverse_path
name|char
modifier|*
name|make_traverse_path
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
specifier|const
name|struct
name|name_entry
modifier|*
name|n
parameter_list|)
block|{
name|int
name|len
init|=
name|tree_entry_len
argument_list|(
name|n
operator|->
name|path
argument_list|,
name|n
operator|->
name|sha1
argument_list|)
decl_stmt|;
name|int
name|pathlen
init|=
name|info
operator|->
name|pathlen
decl_stmt|;
name|path
index|[
name|pathlen
operator|+
name|len
index|]
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|memcpy
argument_list|(
name|path
operator|+
name|pathlen
argument_list|,
name|n
operator|->
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pathlen
condition|)
break|break;
name|path
index|[
operator|--
name|pathlen
index|]
operator|=
literal|'/'
expr_stmt|;
name|n
operator|=
operator|&
name|info
operator|->
name|name
expr_stmt|;
name|len
operator|=
name|tree_entry_len
argument_list|(
name|n
operator|->
name|path
argument_list|,
name|n
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|info
operator|=
name|info
operator|->
name|prev
expr_stmt|;
name|pathlen
operator|-=
name|len
expr_stmt|;
block|}
return|return
name|path
return|;
block|}
end_function
begin_struct
DECL|struct|tree_desc_skip
struct|struct
name|tree_desc_skip
block|{
DECL|member|prev
name|struct
name|tree_desc_skip
modifier|*
name|prev
decl_stmt|;
DECL|member|ptr
specifier|const
name|void
modifier|*
name|ptr
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|tree_desc_x
struct|struct
name|tree_desc_x
block|{
DECL|member|d
name|struct
name|tree_desc
name|d
decl_stmt|;
DECL|member|skip
name|struct
name|tree_desc_skip
modifier|*
name|skip
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|name_compare
specifier|static
name|int
name|name_compare
parameter_list|(
specifier|const
name|char
modifier|*
name|a
parameter_list|,
name|int
name|a_len
parameter_list|,
specifier|const
name|char
modifier|*
name|b
parameter_list|,
name|int
name|b_len
parameter_list|)
block|{
name|int
name|len
init|=
operator|(
name|a_len
operator|<
name|b_len
operator|)
condition|?
name|a_len
else|:
name|b_len
decl_stmt|;
name|int
name|cmp
init|=
name|memcmp
argument_list|(
name|a
argument_list|,
name|b
argument_list|,
name|len
argument_list|)
decl_stmt|;
if|if
condition|(
name|cmp
condition|)
return|return
name|cmp
return|;
return|return
operator|(
name|a_len
operator|-
name|b_len
operator|)
return|;
block|}
end_function
begin_function
DECL|function|check_entry_match
specifier|static
name|int
name|check_entry_match
parameter_list|(
specifier|const
name|char
modifier|*
name|a
parameter_list|,
name|int
name|a_len
parameter_list|,
specifier|const
name|char
modifier|*
name|b
parameter_list|,
name|int
name|b_len
parameter_list|)
block|{
comment|/* 	 * The caller wants to pick *a* from a tree or nothing. 	 * We are looking at *b* in a tree. 	 * 	 * (0) If a and b are the same name, we are trivially happy. 	 * 	 * There are three possibilities where *a* could be hiding 	 * behind *b*. 	 * 	 * (1) *a* == "t",   *b* == "ab"  i.e. *b* sorts earlier than *a* no 	 *                                matter what. 	 * (2) *a* == "t",   *b* == "t-2" and "t" is a subtree in the tree; 	 * (3) *a* == "t-2", *b* == "t"   and "t-2" is a blob in the tree. 	 * 	 * Otherwise we know *a* won't appear in the tree without 	 * scanning further. 	 */
name|int
name|cmp
init|=
name|name_compare
argument_list|(
name|a
argument_list|,
name|a_len
argument_list|,
name|b
argument_list|,
name|b_len
argument_list|)
decl_stmt|;
comment|/* Most common case first -- reading sync'd trees */
if|if
condition|(
operator|!
name|cmp
condition|)
return|return
name|cmp
return|;
if|if
condition|(
literal|0
operator|<
name|cmp
condition|)
block|{
comment|/* a comes after b; it does not matter if it is case (3) 		if (b_len< a_len&& !memcmp(a, b, b_len)&& a[b_len]< '/') 			return 1; 		*/
return|return
literal|1
return|;
comment|/* keep looking */
block|}
comment|/* b comes after a; are we looking at case (2)? */
if|if
condition|(
name|a_len
operator|<
name|b_len
operator|&&
operator|!
name|memcmp
argument_list|(
name|a
argument_list|,
name|b
argument_list|,
name|a_len
argument_list|)
operator|&&
name|b
index|[
name|a_len
index|]
operator|<
literal|'/'
condition|)
return|return
literal|1
return|;
comment|/* keep looking */
return|return
operator|-
literal|1
return|;
comment|/* a cannot appear in the tree */
block|}
end_function
begin_comment
comment|/*  * From the extended tree_desc, extract the first name entry, while  * paying attention to the candidate "first" name.  Most importantly,  * when looking for an entry, if there are entries that sorts earlier  * in the tree object representation than that name, skip them and  * process the named entry first.  We will remember that we haven't  * processed the first entry yet, and in the later call skip the  * entry we processed early when update_extended_entry() is called.  *  * E.g. if the underlying tree object has these entries:  *  *    blob    "t-1"  *    blob    "t-2"  *    tree    "t"  *    blob    "t=1"  *  * and the "first" asks for "t", remember that we still need to  * process "t-1" and "t-2" but extract "t".  After processing the  * entry "t" from this call, the caller will let us know by calling  * update_extended_entry() that we can remember "t" has been processed  * already.  */
end_comment
begin_function
DECL|function|extended_entry_extract
specifier|static
name|void
name|extended_entry_extract
parameter_list|(
name|struct
name|tree_desc_x
modifier|*
name|t
parameter_list|,
name|struct
name|name_entry
modifier|*
name|a
parameter_list|,
specifier|const
name|char
modifier|*
name|first
parameter_list|,
name|int
name|first_len
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
name|int
name|len
decl_stmt|;
name|struct
name|tree_desc
name|probe
decl_stmt|;
name|struct
name|tree_desc_skip
modifier|*
name|skip
decl_stmt|;
comment|/* 	 * Extract the first entry from the tree_desc, but skip the 	 * ones that we already returned in earlier rounds. 	 */
while|while
condition|(
literal|1
condition|)
block|{
if|if
condition|(
operator|!
name|t
operator|->
name|d
operator|.
name|size
condition|)
block|{
name|entry_clear
argument_list|(
name|a
argument_list|)
expr_stmt|;
break|break;
comment|/* not found */
block|}
name|entry_extract
argument_list|(
operator|&
name|t
operator|->
name|d
argument_list|,
name|a
argument_list|)
expr_stmt|;
for|for
control|(
name|skip
operator|=
name|t
operator|->
name|skip
init|;
name|skip
condition|;
name|skip
operator|=
name|skip
operator|->
name|prev
control|)
if|if
condition|(
name|a
operator|->
name|path
operator|==
name|skip
operator|->
name|ptr
condition|)
break|break;
comment|/* found */
if|if
condition|(
operator|!
name|skip
condition|)
break|break;
comment|/* We have processed this entry already. */
name|update_tree_entry
argument_list|(
operator|&
name|t
operator|->
name|d
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|first
operator|||
operator|!
name|a
operator|->
name|path
condition|)
return|return;
comment|/* 	 * The caller wants "first" from this tree, or nothing. 	 */
name|path
operator|=
name|a
operator|->
name|path
expr_stmt|;
name|len
operator|=
name|tree_entry_len
argument_list|(
name|a
operator|->
name|path
argument_list|,
name|a
operator|->
name|sha1
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|check_entry_match
argument_list|(
name|first
argument_list|,
name|first_len
argument_list|,
name|path
argument_list|,
name|len
argument_list|)
condition|)
block|{
case|case
operator|-
literal|1
case|:
name|entry_clear
argument_list|(
name|a
argument_list|)
expr_stmt|;
case|case
literal|0
case|:
return|return;
default|default:
break|break;
block|}
comment|/* 	 * We need to look-ahead -- we suspect that a subtree whose 	 * name is "first" may be hiding behind the current entry "path". 	 */
name|probe
operator|=
name|t
operator|->
name|d
expr_stmt|;
while|while
condition|(
name|probe
operator|.
name|size
condition|)
block|{
name|entry_extract
argument_list|(
operator|&
name|probe
argument_list|,
name|a
argument_list|)
expr_stmt|;
name|path
operator|=
name|a
operator|->
name|path
expr_stmt|;
name|len
operator|=
name|tree_entry_len
argument_list|(
name|a
operator|->
name|path
argument_list|,
name|a
operator|->
name|sha1
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|check_entry_match
argument_list|(
name|first
argument_list|,
name|first_len
argument_list|,
name|path
argument_list|,
name|len
argument_list|)
condition|)
block|{
case|case
operator|-
literal|1
case|:
name|entry_clear
argument_list|(
name|a
argument_list|)
expr_stmt|;
case|case
literal|0
case|:
return|return;
default|default:
name|update_tree_entry
argument_list|(
operator|&
name|probe
argument_list|)
expr_stmt|;
break|break;
block|}
comment|/* keep looking */
block|}
name|entry_clear
argument_list|(
name|a
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|update_extended_entry
specifier|static
name|void
name|update_extended_entry
parameter_list|(
name|struct
name|tree_desc_x
modifier|*
name|t
parameter_list|,
name|struct
name|name_entry
modifier|*
name|a
parameter_list|)
block|{
if|if
condition|(
name|t
operator|->
name|d
operator|.
name|entry
operator|.
name|path
operator|==
name|a
operator|->
name|path
condition|)
block|{
name|update_tree_entry
argument_list|(
operator|&
name|t
operator|->
name|d
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* we have returned this entry early */
name|struct
name|tree_desc_skip
modifier|*
name|skip
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|skip
argument_list|)
argument_list|)
decl_stmt|;
name|skip
operator|->
name|ptr
operator|=
name|a
operator|->
name|path
expr_stmt|;
name|skip
operator|->
name|prev
operator|=
name|t
operator|->
name|skip
expr_stmt|;
name|t
operator|->
name|skip
operator|=
name|skip
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|free_extended_entry
specifier|static
name|void
name|free_extended_entry
parameter_list|(
name|struct
name|tree_desc_x
modifier|*
name|t
parameter_list|)
block|{
name|struct
name|tree_desc_skip
modifier|*
name|p
decl_stmt|,
modifier|*
name|s
decl_stmt|;
for|for
control|(
name|s
operator|=
name|t
operator|->
name|skip
init|;
name|s
condition|;
name|s
operator|=
name|p
control|)
block|{
name|p
operator|=
name|s
operator|->
name|prev
expr_stmt|;
name|free
argument_list|(
name|s
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|traverse_trees
name|int
name|traverse_trees
parameter_list|(
name|int
name|n
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|)
block|{
name|int
name|ret
init|=
literal|0
decl_stmt|;
name|int
name|error
init|=
literal|0
decl_stmt|;
name|struct
name|name_entry
modifier|*
name|entry
init|=
name|xmalloc
argument_list|(
name|n
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|entry
argument_list|)
argument_list|)
decl_stmt|;
name|int
name|i
decl_stmt|;
name|struct
name|tree_desc_x
modifier|*
name|tx
init|=
name|xcalloc
argument_list|(
name|n
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|tx
argument_list|)
argument_list|)
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
name|tx
index|[
name|i
index|]
operator|.
name|d
operator|=
name|t
index|[
name|i
index|]
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|long
name|mask
decl_stmt|,
name|dirmask
decl_stmt|;
specifier|const
name|char
modifier|*
name|first
init|=
name|NULL
decl_stmt|;
name|int
name|first_len
init|=
literal|0
decl_stmt|;
name|struct
name|name_entry
modifier|*
name|e
decl_stmt|;
name|int
name|len
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
name|e
operator|=
name|entry
operator|+
name|i
expr_stmt|;
name|extended_entry_extract
argument_list|(
name|tx
operator|+
name|i
argument_list|,
name|e
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
comment|/* 		 * A tree may have "t-2" at the current location even 		 * though it may have "t" that is a subtree behind it, 		 * and another tree may return "t".  We want to grab 		 * all "t" from all trees to match in such a case. 		 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
name|e
operator|=
name|entry
operator|+
name|i
expr_stmt|;
if|if
condition|(
operator|!
name|e
operator|->
name|path
condition|)
continue|continue;
name|len
operator|=
name|tree_entry_len
argument_list|(
name|e
operator|->
name|path
argument_list|,
name|e
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|first
condition|)
block|{
name|first
operator|=
name|e
operator|->
name|path
expr_stmt|;
name|first_len
operator|=
name|len
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|name_compare
argument_list|(
name|e
operator|->
name|path
argument_list|,
name|len
argument_list|,
name|first
argument_list|,
name|first_len
argument_list|)
operator|<
literal|0
condition|)
block|{
name|first
operator|=
name|e
operator|->
name|path
expr_stmt|;
name|first_len
operator|=
name|len
expr_stmt|;
block|}
block|}
if|if
condition|(
name|first
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
name|e
operator|=
name|entry
operator|+
name|i
expr_stmt|;
name|extended_entry_extract
argument_list|(
name|tx
operator|+
name|i
argument_list|,
name|e
argument_list|,
name|first
argument_list|,
name|first_len
argument_list|)
expr_stmt|;
comment|/* Cull the ones that are not the earliest */
if|if
condition|(
operator|!
name|e
operator|->
name|path
condition|)
continue|continue;
name|len
operator|=
name|tree_entry_len
argument_list|(
name|e
operator|->
name|path
argument_list|,
name|e
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|name_compare
argument_list|(
name|e
operator|->
name|path
argument_list|,
name|len
argument_list|,
name|first
argument_list|,
name|first_len
argument_list|)
condition|)
name|entry_clear
argument_list|(
name|e
argument_list|)
expr_stmt|;
block|}
block|}
comment|/* Now we have in entry[i] the earliest name from the trees */
name|mask
operator|=
literal|0
expr_stmt|;
name|dirmask
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|entry
index|[
name|i
index|]
operator|.
name|path
condition|)
continue|continue;
name|mask
operator||=
literal|1ul
operator|<<
name|i
expr_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|entry
index|[
name|i
index|]
operator|.
name|mode
argument_list|)
condition|)
name|dirmask
operator||=
literal|1ul
operator|<<
name|i
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|mask
condition|)
break|break;
name|ret
operator|=
name|info
operator|->
name|fn
argument_list|(
name|n
argument_list|,
name|mask
argument_list|,
name|dirmask
argument_list|,
name|entry
argument_list|,
name|info
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
block|{
name|error
operator|=
name|ret
expr_stmt|;
if|if
condition|(
operator|!
name|info
operator|->
name|show_all_errors
condition|)
break|break;
block|}
name|mask
operator|&=
name|ret
expr_stmt|;
name|ret
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|mask
operator|&
operator|(
literal|1ul
operator|<<
name|i
operator|)
condition|)
name|update_extended_entry
argument_list|(
name|tx
operator|+
name|i
argument_list|,
name|entry
operator|+
name|i
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|entry
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
name|n
condition|;
name|i
operator|++
control|)
name|free_extended_entry
argument_list|(
name|tx
operator|+
name|i
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tx
argument_list|)
expr_stmt|;
return|return
name|error
return|;
block|}
end_function
begin_function
DECL|function|find_tree_entry
specifier|static
name|int
name|find_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|,
name|unsigned
modifier|*
name|mode
parameter_list|)
block|{
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
while|while
condition|(
name|t
operator|->
name|size
condition|)
block|{
specifier|const
name|char
modifier|*
name|entry
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|int
name|entrylen
decl_stmt|,
name|cmp
decl_stmt|;
name|sha1
operator|=
name|tree_entry_extract
argument_list|(
name|t
argument_list|,
operator|&
name|entry
argument_list|,
name|mode
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
name|t
argument_list|)
expr_stmt|;
name|entrylen
operator|=
name|tree_entry_len
argument_list|(
name|entry
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|entrylen
operator|>
name|namelen
condition|)
continue|continue;
name|cmp
operator|=
name|memcmp
argument_list|(
name|name
argument_list|,
name|entry
argument_list|,
name|entrylen
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
continue|continue;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
break|break;
if|if
condition|(
name|entrylen
operator|==
name|namelen
condition|)
block|{
name|hashcpy
argument_list|(
name|result
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|name
index|[
name|entrylen
index|]
operator|!=
literal|'/'
condition|)
continue|continue;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
operator|*
name|mode
argument_list|)
condition|)
break|break;
if|if
condition|(
operator|++
name|entrylen
operator|==
name|namelen
condition|)
block|{
name|hashcpy
argument_list|(
name|result
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|get_tree_entry
argument_list|(
name|sha1
argument_list|,
name|name
operator|+
name|entrylen
argument_list|,
name|result
argument_list|,
name|mode
argument_list|)
return|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|get_tree_entry
name|int
name|get_tree_entry
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|tree_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
modifier|*
name|mode
parameter_list|)
block|{
name|int
name|retval
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|struct
name|tree_desc
name|t
decl_stmt|;
name|unsigned
name|char
name|root
index|[
literal|20
index|]
decl_stmt|;
name|tree
operator|=
name|read_object_with_reference
argument_list|(
name|tree_sha1
argument_list|,
name|tree_type
argument_list|,
operator|&
name|size
argument_list|,
name|root
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|name
index|[
literal|0
index|]
operator|==
literal|'\0'
condition|)
block|{
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|root
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|init_tree_desc
argument_list|(
operator|&
name|t
argument_list|,
name|tree
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|retval
operator|=
name|find_tree_entry
argument_list|(
operator|&
name|t
argument_list|,
name|name
argument_list|,
name|sha1
argument_list|,
name|mode
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
begin_function
DECL|function|match_entry
specifier|static
name|int
name|match_entry
parameter_list|(
specifier|const
name|struct
name|name_entry
modifier|*
name|entry
parameter_list|,
name|int
name|pathlen
parameter_list|,
specifier|const
name|char
modifier|*
name|match
parameter_list|,
name|int
name|matchlen
parameter_list|,
name|int
modifier|*
name|never_interesting
parameter_list|)
block|{
name|int
name|m
init|=
operator|-
literal|1
decl_stmt|;
comment|/* signals that we haven't called strncmp() */
if|if
condition|(
operator|*
name|never_interesting
condition|)
block|{
comment|/* 		 * We have not seen any match that sorts later 		 * than the current path. 		 */
comment|/* 		 * Does match sort strictly earlier than path 		 * with their common parts? 		 */
name|m
operator|=
name|strncmp
argument_list|(
name|match
argument_list|,
name|entry
operator|->
name|path
argument_list|,
operator|(
name|matchlen
operator|<
name|pathlen
operator|)
condition|?
name|matchlen
else|:
name|pathlen
argument_list|)
expr_stmt|;
if|if
condition|(
name|m
operator|<
literal|0
condition|)
return|return
literal|0
return|;
comment|/* 		 * If we come here even once, that means there is at 		 * least one pathspec that would sort equal to or 		 * later than the path we are currently looking at. 		 * In other words, if we have never reached this point 		 * after iterating all pathspecs, it means all 		 * pathspecs are either outside of base, or inside the 		 * base but sorts strictly earlier than the current 		 * one.  In either case, they will never match the 		 * subsequent entries.  In such a case, we initialized 		 * the variable to -1 and that is what will be 		 * returned, allowing the caller to terminate early. 		 */
operator|*
name|never_interesting
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|pathlen
operator|>
name|matchlen
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|matchlen
operator|>
name|pathlen
condition|)
block|{
if|if
condition|(
name|match
index|[
name|pathlen
index|]
operator|!=
literal|'/'
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|entry
operator|->
name|mode
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
if|if
condition|(
name|m
operator|==
operator|-
literal|1
condition|)
comment|/* 		 * we cheated and did not do strncmp(), so we do 		 * that here. 		 */
name|m
operator|=
name|strncmp
argument_list|(
name|match
argument_list|,
name|entry
operator|->
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
comment|/* 	 * If common part matched earlier then it is a hit, 	 * because we rejected the case where path is not a 	 * leading directory and is shorter than match. 	 */
if|if
condition|(
operator|!
name|m
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
DECL|function|match_dir_prefix
specifier|static
name|int
name|match_dir_prefix
parameter_list|(
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
specifier|const
name|char
modifier|*
name|match
parameter_list|,
name|int
name|matchlen
parameter_list|)
block|{
if|if
condition|(
name|strncmp
argument_list|(
name|base
argument_list|,
name|match
argument_list|,
name|matchlen
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* 	 * If the base is a subdirectory of a path which 	 * was specified, all of them are interesting. 	 */
if|if
condition|(
operator|!
name|matchlen
operator|||
name|base
index|[
name|matchlen
index|]
operator|==
literal|'/'
operator|||
name|match
index|[
name|matchlen
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
return|return
literal|1
return|;
comment|/* Just a random prefix match */
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * Is a tree entry interesting given the pathspec we have?  *  * Pre-condition: baselen == 0 || base[baselen-1] == '/'  *  * Return:  *  - 2 for "yes, and all subsequent entries will be"  *  - 1 for yes  *  - zero for no  *  - negative for "no, and no subsequent entries will be either"  */
end_comment
begin_function
DECL|function|tree_entry_interesting
name|int
name|tree_entry_interesting
parameter_list|(
specifier|const
name|struct
name|name_entry
modifier|*
name|entry
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|base
parameter_list|,
specifier|const
name|struct
name|pathspec
modifier|*
name|ps
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|pathlen
decl_stmt|,
name|baselen
init|=
name|base
operator|->
name|len
decl_stmt|;
name|int
name|never_interesting
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
operator|!
name|ps
operator|->
name|nr
condition|)
block|{
if|if
condition|(
operator|!
name|ps
operator|->
name|recursive
operator|||
name|ps
operator|->
name|max_depth
operator|==
operator|-
literal|1
condition|)
return|return
literal|2
return|;
return|return
operator|!
operator|!
name|within_depth
argument_list|(
name|base
operator|->
name|buf
argument_list|,
name|baselen
argument_list|,
operator|!
operator|!
name|S_ISDIR
argument_list|(
name|entry
operator|->
name|mode
argument_list|)
argument_list|,
name|ps
operator|->
name|max_depth
argument_list|)
return|;
block|}
name|pathlen
operator|=
name|tree_entry_len
argument_list|(
name|entry
operator|->
name|path
argument_list|,
name|entry
operator|->
name|sha1
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
name|ps
operator|->
name|nr
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
block|{
specifier|const
name|struct
name|pathspec_item
modifier|*
name|item
init|=
name|ps
operator|->
name|items
operator|+
name|i
decl_stmt|;
specifier|const
name|char
modifier|*
name|match
init|=
name|item
operator|->
name|match
decl_stmt|;
name|int
name|matchlen
init|=
name|item
operator|->
name|len
decl_stmt|;
if|if
condition|(
name|baselen
operator|>=
name|matchlen
condition|)
block|{
comment|/* If it doesn't match, move along... */
if|if
condition|(
operator|!
name|match_dir_prefix
argument_list|(
name|base
operator|->
name|buf
argument_list|,
name|baselen
argument_list|,
name|match
argument_list|,
name|matchlen
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|ps
operator|->
name|recursive
operator|||
name|ps
operator|->
name|max_depth
operator|==
operator|-
literal|1
condition|)
return|return
literal|2
return|;
return|return
operator|!
operator|!
name|within_depth
argument_list|(
name|base
operator|->
name|buf
operator|+
name|matchlen
operator|+
literal|1
argument_list|,
name|baselen
operator|-
name|matchlen
operator|-
literal|1
argument_list|,
operator|!
operator|!
name|S_ISDIR
argument_list|(
name|entry
operator|->
name|mode
argument_list|)
argument_list|,
name|ps
operator|->
name|max_depth
argument_list|)
return|;
block|}
comment|/* Does the base match? */
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|base
operator|->
name|buf
argument_list|,
name|match
argument_list|,
name|baselen
argument_list|)
condition|)
block|{
if|if
condition|(
name|match_entry
argument_list|(
name|entry
argument_list|,
name|pathlen
argument_list|,
name|match
operator|+
name|baselen
argument_list|,
name|matchlen
operator|-
name|baselen
argument_list|,
operator|&
name|never_interesting
argument_list|)
condition|)
return|return
literal|1
return|;
block|}
block|}
return|return
name|never_interesting
return|;
comment|/* No matches */
block|}
end_function
end_unit
