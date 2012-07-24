begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_decl_stmt
DECL|variable|tree_type
specifier|const
name|char
modifier|*
name|tree_type
init|=
literal|"tree"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|read_one_entry_opt
specifier|static
name|int
name|read_one_entry_opt
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
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|unsigned
name|mode
parameter_list|,
name|int
name|stage
parameter_list|,
name|int
name|opt
parameter_list|)
block|{
name|int
name|len
decl_stmt|;
name|unsigned
name|int
name|size
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
return|return
name|READ_TREE_RECURSIVE
return|;
name|len
operator|=
name|strlen
argument_list|(
name|pathname
argument_list|)
expr_stmt|;
name|size
operator|=
name|cache_entry_size
argument_list|(
name|baselen
operator|+
name|len
argument_list|)
expr_stmt|;
name|ce
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mode
operator|=
name|create_ce_mode
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_flags
operator|=
name|create_ce_flags
argument_list|(
name|stage
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_namelen
operator|=
name|baselen
operator|+
name|len
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
operator|+
name|baselen
argument_list|,
name|pathname
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|opt
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_one_entry
specifier|static
name|int
name|read_one_entry
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
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|unsigned
name|mode
parameter_list|,
name|int
name|stage
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
return|return
name|read_one_entry_opt
argument_list|(
name|sha1
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|pathname
argument_list|,
name|mode
argument_list|,
name|stage
argument_list|,
name|ADD_CACHE_OK_TO_ADD
operator||
name|ADD_CACHE_SKIP_DFCHECK
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * This is used when the caller knows there is no existing entries at  * the stage that will conflict with the entry being added.  */
end_comment
begin_function
DECL|function|read_one_entry_quick
specifier|static
name|int
name|read_one_entry_quick
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
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|unsigned
name|mode
parameter_list|,
name|int
name|stage
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
return|return
name|read_one_entry_opt
argument_list|(
name|sha1
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|pathname
argument_list|,
name|mode
argument_list|,
name|stage
argument_list|,
name|ADD_CACHE_JUST_APPEND
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_tree_1
specifier|static
name|int
name|read_tree_1
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
name|struct
name|strbuf
modifier|*
name|base
parameter_list|,
name|int
name|stage
parameter_list|,
name|struct
name|pathspec
modifier|*
name|pathspec
parameter_list|,
name|read_tree_fn_t
name|fn
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
name|struct
name|tree_desc
name|desc
decl_stmt|;
name|struct
name|name_entry
name|entry
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|len
decl_stmt|,
name|oldlen
init|=
name|base
operator|->
name|len
decl_stmt|;
name|enum
name|interesting
name|retval
init|=
name|entry_not_interesting
decl_stmt|;
if|if
condition|(
name|parse_tree
argument_list|(
name|tree
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|init_tree_desc
argument_list|(
operator|&
name|desc
argument_list|,
name|tree
operator|->
name|buffer
argument_list|,
name|tree
operator|->
name|size
argument_list|)
expr_stmt|;
while|while
condition|(
name|tree_entry
argument_list|(
operator|&
name|desc
argument_list|,
operator|&
name|entry
argument_list|)
condition|)
block|{
if|if
condition|(
name|retval
operator|!=
name|all_entries_interesting
condition|)
block|{
name|retval
operator|=
name|tree_entry_interesting
argument_list|(
operator|&
name|entry
argument_list|,
name|base
argument_list|,
literal|0
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
if|if
condition|(
name|retval
operator|==
name|all_entries_not_interesting
condition|)
break|break;
if|if
condition|(
name|retval
operator|==
name|entry_not_interesting
condition|)
continue|continue;
block|}
switch|switch
condition|(
name|fn
argument_list|(
name|entry
operator|.
name|sha1
argument_list|,
name|base
operator|->
name|buf
argument_list|,
name|base
operator|->
name|len
argument_list|,
name|entry
operator|.
name|path
argument_list|,
name|entry
operator|.
name|mode
argument_list|,
name|stage
argument_list|,
name|context
argument_list|)
condition|)
block|{
case|case
literal|0
case|:
continue|continue;
case|case
name|READ_TREE_RECURSIVE
case|:
break|break;
default|default:
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|S_ISDIR
argument_list|(
name|entry
operator|.
name|mode
argument_list|)
condition|)
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|entry
operator|.
name|sha1
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISGITLINK
argument_list|(
name|entry
operator|.
name|mode
argument_list|)
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|commit
operator|=
name|lookup_commit
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
name|die
argument_list|(
literal|"Commit %s in submodule path %s%s not found"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
argument_list|,
name|base
operator|->
name|buf
argument_list|,
name|entry
operator|.
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
condition|)
name|die
argument_list|(
literal|"Invalid commit %s in submodule path %s%s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
argument_list|,
name|base
operator|->
name|buf
argument_list|,
name|entry
operator|.
name|path
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|commit
operator|->
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
block|}
else|else
continue|continue;
name|len
operator|=
name|tree_entry_len
argument_list|(
operator|&
name|entry
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|base
argument_list|,
name|entry
operator|.
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|base
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|retval
operator|=
name|read_tree_1
argument_list|(
name|lookup_tree
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|base
argument_list|,
name|stage
argument_list|,
name|pathspec
argument_list|,
name|fn
argument_list|,
name|context
argument_list|)
expr_stmt|;
name|strbuf_setlen
argument_list|(
name|base
argument_list|,
name|oldlen
argument_list|)
expr_stmt|;
if|if
condition|(
name|retval
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_tree_recursive
name|int
name|read_tree_recursive
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|int
name|stage
parameter_list|,
name|struct
name|pathspec
modifier|*
name|pathspec
parameter_list|,
name|read_tree_fn_t
name|fn
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|sb
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|ret
operator|=
name|read_tree_1
argument_list|(
name|tree
argument_list|,
operator|&
name|sb
argument_list|,
name|stage
argument_list|,
name|pathspec
argument_list|,
name|fn
argument_list|,
name|context
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|cmp_cache_name_compare
specifier|static
name|int
name|cmp_cache_name_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce1
decl_stmt|,
modifier|*
name|ce2
decl_stmt|;
name|ce1
operator|=
operator|*
operator|(
operator|(
specifier|const
expr|struct
name|cache_entry
operator|*
operator|*
operator|)
name|a_
operator|)
expr_stmt|;
name|ce2
operator|=
operator|*
operator|(
operator|(
specifier|const
expr|struct
name|cache_entry
operator|*
operator|*
operator|)
name|b_
operator|)
expr_stmt|;
return|return
name|cache_name_stage_compare
argument_list|(
name|ce1
operator|->
name|name
argument_list|,
name|ce1
operator|->
name|ce_namelen
argument_list|,
name|ce_stage
argument_list|(
name|ce1
argument_list|)
argument_list|,
name|ce2
operator|->
name|name
argument_list|,
name|ce2
operator|->
name|ce_namelen
argument_list|,
name|ce_stage
argument_list|(
name|ce2
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_tree
name|int
name|read_tree
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
name|int
name|stage
parameter_list|,
name|struct
name|pathspec
modifier|*
name|match
parameter_list|)
block|{
name|read_tree_fn_t
name|fn
init|=
name|NULL
decl_stmt|;
name|int
name|i
decl_stmt|,
name|err
decl_stmt|;
comment|/* 	 * Currently the only existing callers of this function all 	 * call it with stage=1 and after making sure there is nothing 	 * at that stage; we could always use read_one_entry_quick(). 	 * 	 * But when we decide to straighten out git-read-tree not to 	 * use unpack_trees() in some cases, this will probably start 	 * to matter. 	 */
comment|/* 	 * See if we have cache entry at the stage.  If so, 	 * do it the original slow way, otherwise, append and then 	 * sort at the end. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
operator|!
name|fn
operator|&&
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|==
name|stage
condition|)
name|fn
operator|=
name|read_one_entry
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|fn
condition|)
name|fn
operator|=
name|read_one_entry_quick
expr_stmt|;
name|err
operator|=
name|read_tree_recursive
argument_list|(
name|tree
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
name|stage
argument_list|,
name|match
argument_list|,
name|fn
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|fn
operator|==
name|read_one_entry
operator|||
name|err
condition|)
return|return
name|err
return|;
comment|/* 	 * Sort the cache entry -- we need to nuke the cache tree, though. 	 */
name|cache_tree_free
argument_list|(
operator|&
name|active_cache_tree
argument_list|)
expr_stmt|;
name|qsort
argument_list|(
name|active_cache
argument_list|,
name|active_nr
argument_list|,
sizeof|sizeof
argument_list|(
name|active_cache
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|cmp_cache_name_compare
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|lookup_tree
name|struct
name|tree
modifier|*
name|lookup_tree
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
return|return
name|create_object
argument_list|(
name|sha1
argument_list|,
name|OBJ_TREE
argument_list|,
name|alloc_tree_node
argument_list|()
argument_list|)
return|;
if|if
condition|(
operator|!
name|obj
operator|->
name|type
condition|)
name|obj
operator|->
name|type
operator|=
name|OBJ_TREE
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|!=
name|OBJ_TREE
condition|)
block|{
name|error
argument_list|(
literal|"Object %s is a %s, not a tree"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|typename
argument_list|(
name|obj
operator|->
name|type
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
return|;
block|}
end_function
begin_function
DECL|function|parse_tree_buffer
name|int
name|parse_tree_buffer
parameter_list|(
name|struct
name|tree
modifier|*
name|item
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|item
operator|->
name|object
operator|.
name|parsed
operator|=
literal|1
expr_stmt|;
name|item
operator|->
name|buffer
operator|=
name|buffer
expr_stmt|;
name|item
operator|->
name|size
operator|=
name|size
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_tree
name|int
name|parse_tree
parameter_list|(
name|struct
name|tree
modifier|*
name|item
parameter_list|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|item
operator|->
name|object
operator|.
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
name|buffer
condition|)
return|return
name|error
argument_list|(
literal|"Could not read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|type
operator|!=
name|OBJ_TREE
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"Object %s not a tree"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
block|}
return|return
name|parse_tree_buffer
argument_list|(
name|item
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_tree_indirect
name|struct
name|tree
modifier|*
name|parse_tree_indirect
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
do|do
block|{
if|if
condition|(
operator|!
name|obj
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TREE
condition|)
return|return
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
return|;
elseif|else
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
name|obj
operator|=
operator|&
operator|(
operator|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
operator|)
operator|->
name|tree
operator|->
name|object
operator|)
expr_stmt|;
elseif|else
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
name|obj
operator|=
operator|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|obj
operator|)
operator|->
name|tagged
expr_stmt|;
else|else
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|obj
operator|->
name|parsed
condition|)
name|parse_object
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
literal|1
condition|)
do|;
block|}
end_function
end_unit
