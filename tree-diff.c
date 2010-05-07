begin_unit
begin_comment
comment|/*  * Helper functions for tree diff generation  */
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
file|"tree.h"
end_include
begin_function
DECL|function|malloc_base
specifier|static
name|char
modifier|*
name|malloc_base
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
name|path
parameter_list|,
name|int
name|pathlen
parameter_list|)
block|{
name|char
modifier|*
name|newbase
init|=
name|xmalloc
argument_list|(
name|baselen
operator|+
name|pathlen
operator|+
literal|2
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|newbase
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|newbase
operator|+
name|baselen
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|newbase
operator|+
name|baselen
operator|+
name|pathlen
argument_list|,
literal|"/"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
return|return
name|newbase
return|;
block|}
end_function
begin_function
DECL|function|malloc_fullname
specifier|static
name|char
modifier|*
name|malloc_fullname
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
name|path
parameter_list|,
name|int
name|pathlen
parameter_list|)
block|{
name|char
modifier|*
name|fullname
init|=
name|xmalloc
argument_list|(
name|baselen
operator|+
name|pathlen
operator|+
literal|1
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|fullname
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|fullname
operator|+
name|baselen
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
name|fullname
index|[
name|baselen
operator|+
name|pathlen
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|fullname
return|;
block|}
end_function
begin_function_decl
specifier|static
name|void
name|show_entry
parameter_list|(
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|compare_tree_entry
specifier|static
name|int
name|compare_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t1
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|t2
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|unsigned
name|mode1
decl_stmt|,
name|mode2
decl_stmt|;
specifier|const
name|char
modifier|*
name|path1
decl_stmt|,
modifier|*
name|path2
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|,
modifier|*
name|sha2
decl_stmt|;
name|int
name|cmp
decl_stmt|,
name|pathlen1
decl_stmt|,
name|pathlen2
decl_stmt|;
name|char
modifier|*
name|fullname
decl_stmt|;
name|sha1
operator|=
name|tree_entry_extract
argument_list|(
name|t1
argument_list|,
operator|&
name|path1
argument_list|,
operator|&
name|mode1
argument_list|)
expr_stmt|;
name|sha2
operator|=
name|tree_entry_extract
argument_list|(
name|t2
argument_list|,
operator|&
name|path2
argument_list|,
operator|&
name|mode2
argument_list|)
expr_stmt|;
name|pathlen1
operator|=
name|tree_entry_len
argument_list|(
name|path1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|pathlen2
operator|=
name|tree_entry_len
argument_list|(
name|path2
argument_list|,
name|sha2
argument_list|)
expr_stmt|;
name|cmp
operator|=
name|base_name_compare
argument_list|(
name|path1
argument_list|,
name|pathlen1
argument_list|,
name|mode1
argument_list|,
name|path2
argument_list|,
name|pathlen2
argument_list|,
name|mode2
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
block|{
name|show_entry
argument_list|(
name|opt
argument_list|,
literal|"-"
argument_list|,
name|t1
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
block|{
name|show_entry
argument_list|(
name|opt
argument_list|,
literal|"+"
argument_list|,
name|t2
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|FIND_COPIES_HARDER
argument_list|)
operator|&&
operator|!
name|hashcmp
argument_list|(
name|sha1
argument_list|,
name|sha2
argument_list|)
operator|&&
name|mode1
operator|==
name|mode2
condition|)
return|return
literal|0
return|;
comment|/* 	 * If the filemode has changed to/from a directory from/to a regular 	 * file, we need to consider it a remove and an add. 	 */
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
operator|!=
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
block|{
name|show_entry
argument_list|(
name|opt
argument_list|,
literal|"-"
argument_list|,
name|t1
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|show_entry
argument_list|(
name|opt
argument_list|,
literal|"+"
argument_list|,
name|t2
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|RECURSIVE
argument_list|)
operator|&&
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
condition|)
block|{
name|int
name|retval
decl_stmt|;
name|char
modifier|*
name|newbase
init|=
name|malloc_base
argument_list|(
name|base
argument_list|,
name|baselen
argument_list|,
name|path1
argument_list|,
name|pathlen1
argument_list|)
decl_stmt|;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|TREE_IN_RECURSIVE
argument_list|)
condition|)
block|{
name|newbase
index|[
name|baselen
operator|+
name|pathlen1
index|]
operator|=
literal|0
expr_stmt|;
name|opt
operator|->
name|change
argument_list|(
name|opt
argument_list|,
name|mode1
argument_list|,
name|mode2
argument_list|,
name|sha1
argument_list|,
name|sha2
argument_list|,
name|newbase
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|newbase
index|[
name|baselen
operator|+
name|pathlen1
index|]
operator|=
literal|'/'
expr_stmt|;
block|}
name|retval
operator|=
name|diff_tree_sha1
argument_list|(
name|sha1
argument_list|,
name|sha2
argument_list|,
name|newbase
argument_list|,
name|opt
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|newbase
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
name|fullname
operator|=
name|malloc_fullname
argument_list|(
name|base
argument_list|,
name|baselen
argument_list|,
name|path1
argument_list|,
name|pathlen1
argument_list|)
expr_stmt|;
name|opt
operator|->
name|change
argument_list|(
name|opt
argument_list|,
name|mode1
argument_list|,
name|mode2
argument_list|,
name|sha1
argument_list|,
name|sha2
argument_list|,
name|fullname
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|fullname
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * Is a tree entry interesting given the pathspec we have?  *  * Return:  *  - 2 for "yes, and all subsequent entries will be"  *  - 1 for yes  *  - zero for no  *  - negative for "no, and no subsequent entries will be either"  */
end_comment
begin_function
DECL|function|tree_entry_interesting
specifier|static
name|int
name|tree_entry_interesting
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|unsigned
name|mode
decl_stmt|;
name|int
name|i
decl_stmt|;
name|int
name|pathlen
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
name|opt
operator|->
name|nr_paths
condition|)
return|return
literal|1
return|;
name|sha1
operator|=
name|tree_entry_extract
argument_list|(
name|desc
argument_list|,
operator|&
name|path
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
name|pathlen
operator|=
name|tree_entry_len
argument_list|(
name|path
argument_list|,
name|sha1
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
name|opt
operator|->
name|nr_paths
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|match
init|=
name|opt
operator|->
name|paths
index|[
name|i
index|]
decl_stmt|;
name|int
name|matchlen
init|=
name|opt
operator|->
name|pathlens
index|[
name|i
index|]
decl_stmt|;
name|int
name|m
init|=
operator|-
literal|1
decl_stmt|;
comment|/* signals that we haven't called strncmp() */
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
name|strncmp
argument_list|(
name|base
argument_list|,
name|match
argument_list|,
name|matchlen
argument_list|)
condition|)
continue|continue;
comment|/* 			 * If the base is a subdirectory of a path which 			 * was specified, all of them are interesting. 			 */
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
literal|2
return|;
comment|/* Just a random prefix match */
continue|continue;
block|}
comment|/* Does the base match? */
if|if
condition|(
name|strncmp
argument_list|(
name|base
argument_list|,
name|match
argument_list|,
name|baselen
argument_list|)
condition|)
continue|continue;
name|match
operator|+=
name|baselen
expr_stmt|;
name|matchlen
operator|-=
name|baselen
expr_stmt|;
if|if
condition|(
name|never_interesting
condition|)
block|{
comment|/* 			 * We have not seen any match that sorts later 			 * than the current path. 			 */
comment|/* 			 * Does match sort strictly earlier than path 			 * with their common parts? 			 */
name|m
operator|=
name|strncmp
argument_list|(
name|match
argument_list|,
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
continue|continue;
comment|/* 			 * If we come here even once, that means there is at 			 * least one pathspec that would sort equal to or 			 * later than the path we are currently looking at. 			 * In other words, if we have never reached this point 			 * after iterating all pathspecs, it means all 			 * pathspecs are either outside of base, or inside the 			 * base but sorts strictly earlier than the current 			 * one.  In either case, they will never match the 			 * subsequent entries.  In such a case, we initialized 			 * the variable to -1 and that is what will be 			 * returned, allowing the caller to terminate early. 			 */
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
continue|continue;
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
continue|continue;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
continue|continue;
block|}
if|if
condition|(
name|m
operator|==
operator|-
literal|1
condition|)
comment|/* 			 * we cheated and did not do strncmp(), so we do 			 * that here. 			 */
name|m
operator|=
name|strncmp
argument_list|(
name|match
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
comment|/* 		 * If common part matched earlier then it is a hit, 		 * because we rejected the case where path is not a 		 * leading directory and is shorter than match. 		 */
if|if
condition|(
operator|!
name|m
condition|)
return|return
literal|1
return|;
block|}
return|return
name|never_interesting
return|;
comment|/* No matches */
block|}
end_function
begin_comment
comment|/* A whole sub-tree went away or appeared */
end_comment
begin_function
DECL|function|show_tree
specifier|static
name|void
name|show_tree
parameter_list|(
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|)
block|{
name|int
name|all_interesting
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|desc
operator|->
name|size
condition|)
block|{
name|int
name|show
decl_stmt|;
if|if
condition|(
name|all_interesting
condition|)
name|show
operator|=
literal|1
expr_stmt|;
else|else
block|{
name|show
operator|=
name|tree_entry_interesting
argument_list|(
name|desc
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|opt
argument_list|)
expr_stmt|;
if|if
condition|(
name|show
operator|==
literal|2
condition|)
name|all_interesting
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|show
operator|<
literal|0
condition|)
break|break;
if|if
condition|(
name|show
condition|)
name|show_entry
argument_list|(
name|opt
argument_list|,
name|prefix
argument_list|,
name|desc
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
name|desc
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* A file entry went away or appeared */
end_comment
begin_function
DECL|function|show_entry
specifier|static
name|void
name|show_entry
parameter_list|(
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|)
block|{
name|unsigned
name|mode
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|tree_entry_extract
argument_list|(
name|desc
argument_list|,
operator|&
name|path
argument_list|,
operator|&
name|mode
argument_list|)
decl_stmt|;
name|int
name|pathlen
init|=
name|tree_entry_len
argument_list|(
name|path
argument_list|,
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|RECURSIVE
argument_list|)
operator|&&
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
name|char
modifier|*
name|newbase
init|=
name|malloc_base
argument_list|(
name|base
argument_list|,
name|baselen
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
decl_stmt|;
name|struct
name|tree_desc
name|inner
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|tree
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
operator|||
name|type
operator|!=
name|OBJ_TREE
condition|)
name|die
argument_list|(
literal|"corrupt tree sha %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|TREE_IN_RECURSIVE
argument_list|)
condition|)
block|{
name|newbase
index|[
name|baselen
operator|+
name|pathlen
index|]
operator|=
literal|0
expr_stmt|;
name|opt
operator|->
name|add_remove
argument_list|(
name|opt
argument_list|,
operator|*
name|prefix
argument_list|,
name|mode
argument_list|,
name|sha1
argument_list|,
name|newbase
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|newbase
index|[
name|baselen
operator|+
name|pathlen
index|]
operator|=
literal|'/'
expr_stmt|;
block|}
name|init_tree_desc
argument_list|(
operator|&
name|inner
argument_list|,
name|tree
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|show_tree
argument_list|(
name|opt
argument_list|,
name|prefix
argument_list|,
operator|&
name|inner
argument_list|,
name|newbase
argument_list|,
name|baselen
operator|+
literal|1
operator|+
name|pathlen
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|newbase
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|char
modifier|*
name|fullname
init|=
name|malloc_fullname
argument_list|(
name|base
argument_list|,
name|baselen
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
decl_stmt|;
name|opt
operator|->
name|add_remove
argument_list|(
name|opt
argument_list|,
name|prefix
index|[
literal|0
index|]
argument_list|,
name|mode
argument_list|,
name|sha1
argument_list|,
name|fullname
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|fullname
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|skip_uninteresting
specifier|static
name|void
name|skip_uninteresting
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|int
name|all_interesting
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|t
operator|->
name|size
condition|)
block|{
name|int
name|show
decl_stmt|;
if|if
condition|(
name|all_interesting
condition|)
name|show
operator|=
literal|1
expr_stmt|;
else|else
block|{
name|show
operator|=
name|tree_entry_interesting
argument_list|(
name|t
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|opt
argument_list|)
expr_stmt|;
if|if
condition|(
name|show
operator|==
literal|2
condition|)
name|all_interesting
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|show
condition|)
block|{
name|update_tree_entry
argument_list|(
name|t
argument_list|)
expr_stmt|;
continue|continue;
block|}
comment|/* Skip it all? */
if|if
condition|(
name|show
operator|<
literal|0
condition|)
name|t
operator|->
name|size
operator|=
literal|0
expr_stmt|;
return|return;
block|}
block|}
end_function
begin_function
DECL|function|diff_tree
name|int
name|diff_tree
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t1
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|t2
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|int
name|baselen
init|=
name|strlen
argument_list|(
name|base
argument_list|)
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|QUICK
argument_list|)
operator|&&
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|HAS_CHANGES
argument_list|)
condition|)
break|break;
if|if
condition|(
name|opt
operator|->
name|nr_paths
condition|)
block|{
name|skip_uninteresting
argument_list|(
name|t1
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|opt
argument_list|)
expr_stmt|;
name|skip_uninteresting
argument_list|(
name|t2
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|opt
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|t1
operator|->
name|size
condition|)
block|{
if|if
condition|(
operator|!
name|t2
operator|->
name|size
condition|)
break|break;
name|show_entry
argument_list|(
name|opt
argument_list|,
literal|"+"
argument_list|,
name|t2
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
name|t2
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|t2
operator|->
name|size
condition|)
block|{
name|show_entry
argument_list|(
name|opt
argument_list|,
literal|"-"
argument_list|,
name|t1
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
name|t1
argument_list|)
expr_stmt|;
continue|continue;
block|}
switch|switch
condition|(
name|compare_tree_entry
argument_list|(
name|t1
argument_list|,
name|t2
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|opt
argument_list|)
condition|)
block|{
case|case
operator|-
literal|1
case|:
name|update_tree_entry
argument_list|(
name|t1
argument_list|)
expr_stmt|;
continue|continue;
case|case
literal|0
case|:
name|update_tree_entry
argument_list|(
name|t1
argument_list|)
expr_stmt|;
comment|/* Fallthrough */
case|case
literal|1
case|:
name|update_tree_entry
argument_list|(
name|t2
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"git diff-tree: internal error"
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * Does it look like the resulting diff might be due to a rename?  *  - single entry  *  - not a valid previous file  */
end_comment
begin_function
DECL|function|diff_might_be_rename
specifier|static
specifier|inline
name|int
name|diff_might_be_rename
parameter_list|(
name|void
parameter_list|)
block|{
return|return
name|diff_queued_diff
operator|.
name|nr
operator|==
literal|1
operator|&&
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|diff_queued_diff
operator|.
name|queue
index|[
literal|0
index|]
operator|->
name|one
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|try_to_follow_renames
specifier|static
name|void
name|try_to_follow_renames
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t1
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|t2
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|struct
name|diff_options
name|diff_opts
decl_stmt|;
name|struct
name|diff_queue_struct
modifier|*
name|q
init|=
operator|&
name|diff_queued_diff
decl_stmt|;
name|struct
name|diff_filepair
modifier|*
name|choice
decl_stmt|;
specifier|const
name|char
modifier|*
name|paths
index|[
literal|1
index|]
decl_stmt|;
name|int
name|i
decl_stmt|;
comment|/* Remove the file creation entry from the diff queue, and remember it */
name|choice
operator|=
name|q
operator|->
name|queue
index|[
literal|0
index|]
expr_stmt|;
name|q
operator|->
name|nr
operator|=
literal|0
expr_stmt|;
name|diff_setup
argument_list|(
operator|&
name|diff_opts
argument_list|)
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|diff_opts
argument_list|,
name|RECURSIVE
argument_list|)
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|diff_opts
argument_list|,
name|FIND_COPIES_HARDER
argument_list|)
expr_stmt|;
name|diff_opts
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_NO_OUTPUT
expr_stmt|;
name|diff_opts
operator|.
name|single_follow
operator|=
name|opt
operator|->
name|paths
index|[
literal|0
index|]
expr_stmt|;
name|diff_opts
operator|.
name|break_opt
operator|=
name|opt
operator|->
name|break_opt
expr_stmt|;
name|paths
index|[
literal|0
index|]
operator|=
name|NULL
expr_stmt|;
name|diff_tree_setup_paths
argument_list|(
name|paths
argument_list|,
operator|&
name|diff_opts
argument_list|)
expr_stmt|;
if|if
condition|(
name|diff_setup_done
argument_list|(
operator|&
name|diff_opts
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to set up diff options to follow renames"
argument_list|)
expr_stmt|;
name|diff_tree
argument_list|(
name|t1
argument_list|,
name|t2
argument_list|,
name|base
argument_list|,
operator|&
name|diff_opts
argument_list|)
expr_stmt|;
name|diffcore_std
argument_list|(
operator|&
name|diff_opts
argument_list|)
expr_stmt|;
name|diff_tree_release_paths
argument_list|(
operator|&
name|diff_opts
argument_list|)
expr_stmt|;
comment|/* Go through the new set of filepairing, and see if we find a more interesting one */
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
comment|/* 		 * Found a source? Not only do we use that for the new 		 * diff_queued_diff, we will also use that as the path in 		 * the future! 		 */
if|if
condition|(
operator|(
name|p
operator|->
name|status
operator|==
literal|'R'
operator|||
name|p
operator|->
name|status
operator|==
literal|'C'
operator|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|p
operator|->
name|two
operator|->
name|path
argument_list|,
name|opt
operator|->
name|paths
index|[
literal|0
index|]
argument_list|)
condition|)
block|{
comment|/* Switch the file-pairs around */
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|=
name|choice
expr_stmt|;
name|choice
operator|=
name|p
expr_stmt|;
comment|/* Update the path we use from now on.. */
name|diff_tree_release_paths
argument_list|(
name|opt
argument_list|)
expr_stmt|;
name|opt
operator|->
name|paths
index|[
literal|0
index|]
operator|=
name|xstrdup
argument_list|(
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
name|diff_tree_setup_paths
argument_list|(
name|opt
operator|->
name|paths
argument_list|,
name|opt
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
comment|/* 	 * Then, discard all the non-relevant file pairs... 	 */
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
name|diff_free_filepair
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * .. and re-instate the one we want (which might be either the 	 * original one, or the rename/copy we found) 	 */
name|q
operator|->
name|queue
index|[
literal|0
index|]
operator|=
name|choice
expr_stmt|;
name|q
operator|->
name|nr
operator|=
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diff_tree_sha1
name|int
name|diff_tree_sha1
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|old
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
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|void
modifier|*
name|tree1
decl_stmt|,
modifier|*
name|tree2
decl_stmt|;
name|struct
name|tree_desc
name|t1
decl_stmt|,
name|t2
decl_stmt|;
name|unsigned
name|long
name|size1
decl_stmt|,
name|size2
decl_stmt|;
name|int
name|retval
decl_stmt|;
name|tree1
operator|=
name|read_object_with_reference
argument_list|(
name|old
argument_list|,
name|tree_type
argument_list|,
operator|&
name|size1
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree1
condition|)
name|die
argument_list|(
literal|"unable to read source tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|old
argument_list|)
argument_list|)
expr_stmt|;
name|tree2
operator|=
name|read_object_with_reference
argument_list|(
name|new
argument_list|,
name|tree_type
argument_list|,
operator|&
name|size2
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree2
condition|)
name|die
argument_list|(
literal|"unable to read destination tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|new
argument_list|)
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|t1
argument_list|,
name|tree1
argument_list|,
name|size1
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|t2
argument_list|,
name|tree2
argument_list|,
name|size2
argument_list|)
expr_stmt|;
name|retval
operator|=
name|diff_tree
argument_list|(
operator|&
name|t1
argument_list|,
operator|&
name|t2
argument_list|,
name|base
argument_list|,
name|opt
argument_list|)
expr_stmt|;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|FOLLOW_RENAMES
argument_list|)
operator|&&
name|diff_might_be_rename
argument_list|()
condition|)
block|{
name|init_tree_desc
argument_list|(
operator|&
name|t1
argument_list|,
name|tree1
argument_list|,
name|size1
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|t2
argument_list|,
name|tree2
argument_list|,
name|size2
argument_list|)
expr_stmt|;
name|try_to_follow_renames
argument_list|(
operator|&
name|t1
argument_list|,
operator|&
name|t2
argument_list|,
name|base
argument_list|,
name|opt
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|tree1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree2
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
begin_function
DECL|function|diff_root_tree_sha1
name|int
name|diff_root_tree_sha1
parameter_list|(
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
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
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
name|init_tree_desc
argument_list|(
operator|&
name|real
argument_list|,
name|tree
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|empty
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|)
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
name|opt
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
DECL|function|count_paths
specifier|static
name|int
name|count_paths
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|paths
parameter_list|)
block|{
name|int
name|i
init|=
literal|0
decl_stmt|;
while|while
condition|(
operator|*
name|paths
operator|++
condition|)
name|i
operator|++
expr_stmt|;
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|diff_tree_release_paths
name|void
name|diff_tree_release_paths
parameter_list|(
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|free
argument_list|(
name|opt
operator|->
name|pathlens
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diff_tree_setup_paths
name|void
name|diff_tree_setup_paths
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|p
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|opt
operator|->
name|nr_paths
operator|=
literal|0
expr_stmt|;
name|opt
operator|->
name|pathlens
operator|=
name|NULL
expr_stmt|;
name|opt
operator|->
name|paths
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|p
condition|)
block|{
name|int
name|i
decl_stmt|;
name|opt
operator|->
name|paths
operator|=
name|p
expr_stmt|;
name|opt
operator|->
name|nr_paths
operator|=
name|count_paths
argument_list|(
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|nr_paths
operator|==
literal|0
condition|)
block|{
name|opt
operator|->
name|pathlens
operator|=
name|NULL
expr_stmt|;
return|return;
block|}
name|opt
operator|->
name|pathlens
operator|=
name|xmalloc
argument_list|(
name|opt
operator|->
name|nr_paths
operator|*
sizeof|sizeof
argument_list|(
name|int
argument_list|)
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
name|opt
operator|->
name|nr_paths
condition|;
name|i
operator|++
control|)
name|opt
operator|->
name|pathlens
index|[
name|i
index|]
operator|=
name|strlen
argument_list|(
name|p
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
block|}
end_function
end_unit
