begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
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
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"list-objects.h"
end_include
begin_function
DECL|function|process_blob
specifier|static
name|void
name|process_blob
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|struct
name|blob
modifier|*
name|blob
parameter_list|,
name|struct
name|object_array
modifier|*
name|p
parameter_list|,
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
operator|&
name|blob
operator|->
name|object
decl_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|blob_objects
condition|)
return|return;
if|if
condition|(
operator|!
name|obj
condition|)
name|die
argument_list|(
literal|"bad blob object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|SEEN
operator|)
condition|)
return|return;
name|obj
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|name
operator|=
name|xstrdup
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|add_object
argument_list|(
name|obj
argument_list|,
name|p
argument_list|,
name|path
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Processing a gitlink entry currently does nothing, since  * we do not recurse into the subproject.  *  * We *could* eventually add a flag that actually does that,  * which would involve:  *  - is the subproject actually checked out?  *  - if so, see if the subproject has already been added  *    to the alternates list, and add it if not.  *  - process the commit (or tag) the gitlink points to  *    recursively.  *  * However, it's unclear whether there is really ever any  * reason to see superprojects and subprojects as such a  * "unified" object pool (potentially resulting in a totally  * humongous pack - avoiding which was the whole point of  * having gitlinks in the first place!).  *  * So for now, there is just a note that we *could* follow  * the link, and how to do it. Whether it necessarily makes  * any sense what-so-ever to ever do that is another issue.  */
end_comment
begin_function
DECL|function|process_gitlink
specifier|static
name|void
name|process_gitlink
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|object_array
modifier|*
name|p
parameter_list|,
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
comment|/* Nothing to do */
block|}
end_function
begin_function
DECL|function|process_tree
specifier|static
name|void
name|process_tree
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
name|struct
name|object_array
modifier|*
name|p
parameter_list|,
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
operator|&
name|tree
operator|->
name|object
decl_stmt|;
name|struct
name|tree_desc
name|desc
decl_stmt|;
name|struct
name|name_entry
name|entry
decl_stmt|;
name|struct
name|name_path
name|me
decl_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|tree_objects
condition|)
return|return;
if|if
condition|(
operator|!
name|obj
condition|)
name|die
argument_list|(
literal|"bad tree object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|SEEN
operator|)
condition|)
return|return;
if|if
condition|(
name|parse_tree
argument_list|(
name|tree
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"bad tree object %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|obj
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|name
operator|=
name|xstrdup
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|add_object
argument_list|(
name|obj
argument_list|,
name|p
argument_list|,
name|path
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|me
operator|.
name|up
operator|=
name|path
expr_stmt|;
name|me
operator|.
name|elem
operator|=
name|name
expr_stmt|;
name|me
operator|.
name|elem_len
operator|=
name|strlen
argument_list|(
name|name
argument_list|)
expr_stmt|;
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
name|S_ISDIR
argument_list|(
name|entry
operator|.
name|mode
argument_list|)
condition|)
name|process_tree
argument_list|(
name|revs
argument_list|,
name|lookup_tree
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
argument_list|,
name|p
argument_list|,
operator|&
name|me
argument_list|,
name|entry
operator|.
name|path
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
name|process_gitlink
argument_list|(
name|revs
argument_list|,
name|entry
operator|.
name|sha1
argument_list|,
name|p
argument_list|,
operator|&
name|me
argument_list|,
name|entry
operator|.
name|path
argument_list|)
expr_stmt|;
else|else
name|process_blob
argument_list|(
name|revs
argument_list|,
name|lookup_blob
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
argument_list|,
name|p
argument_list|,
operator|&
name|me
argument_list|,
name|entry
operator|.
name|path
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|tree
operator|->
name|buffer
argument_list|)
expr_stmt|;
name|tree
operator|->
name|buffer
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|mark_edge_parents_uninteresting
specifier|static
name|void
name|mark_edge_parents_uninteresting
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|show_edge_fn
name|show_edge
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
for|for
control|(
name|parents
operator|=
name|commit
operator|->
name|parents
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
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
if|if
condition|(
operator|!
operator|(
name|parent
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
operator|)
condition|)
continue|continue;
name|mark_tree_uninteresting
argument_list|(
name|parent
operator|->
name|tree
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|edge_hint
operator|&&
operator|!
operator|(
name|parent
operator|->
name|object
operator|.
name|flags
operator|&
name|SHOWN
operator|)
condition|)
block|{
name|parent
operator|->
name|object
operator|.
name|flags
operator||=
name|SHOWN
expr_stmt|;
name|show_edge
argument_list|(
name|parent
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|mark_edges_uninteresting
name|void
name|mark_edges_uninteresting
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|,
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|show_edge_fn
name|show_edge
parameter_list|)
block|{
for|for
control|(
init|;
name|list
condition|;
name|list
operator|=
name|list
operator|->
name|next
control|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
block|{
name|mark_tree_uninteresting
argument_list|(
name|commit
operator|->
name|tree
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|mark_edge_parents_uninteresting
argument_list|(
name|commit
argument_list|,
name|revs
argument_list|,
name|show_edge
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|traverse_commit_list
name|void
name|traverse_commit_list
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|void
function_decl|(
modifier|*
name|show_commit
function_decl|)
parameter_list|(
name|struct
name|commit
modifier|*
parameter_list|)
parameter_list|,
name|void
function_decl|(
modifier|*
name|show_object
function_decl|)
parameter_list|(
name|struct
name|object_array_entry
modifier|*
parameter_list|)
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|object_array
name|objects
init|=
block|{
literal|0
block|,
literal|0
block|,
name|NULL
block|}
decl_stmt|;
while|while
condition|(
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
name|revs
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|process_tree
argument_list|(
name|revs
argument_list|,
name|commit
operator|->
name|tree
argument_list|,
operator|&
name|objects
argument_list|,
name|NULL
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|show_commit
argument_list|(
name|commit
argument_list|)
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
name|revs
operator|->
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object_array_entry
modifier|*
name|pending
init|=
name|revs
operator|->
name|pending
operator|.
name|objects
operator|+
name|i
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
init|=
name|pending
operator|->
name|item
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|pending
operator|->
name|name
decl_stmt|;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|SEEN
operator|)
condition|)
continue|continue;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
block|{
name|obj
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|add_object_array
argument_list|(
name|obj
argument_list|,
name|name
argument_list|,
operator|&
name|objects
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TREE
condition|)
block|{
name|process_tree
argument_list|(
name|revs
argument_list|,
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
argument_list|,
operator|&
name|objects
argument_list|,
name|NULL
argument_list|,
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_BLOB
condition|)
block|{
name|process_blob
argument_list|(
name|revs
argument_list|,
operator|(
expr|struct
name|blob
operator|*
operator|)
name|obj
argument_list|,
operator|&
name|objects
argument_list|,
name|NULL
argument_list|,
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"unknown pending object %s (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|,
name|name
argument_list|)
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
name|objects
operator|.
name|nr
condition|;
name|i
operator|++
control|)
name|show_object
argument_list|(
operator|&
name|objects
operator|.
name|objects
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|objects
operator|.
name|objects
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|pending
operator|.
name|nr
condition|)
block|{
name|free
argument_list|(
name|revs
operator|->
name|pending
operator|.
name|objects
argument_list|)
expr_stmt|;
name|revs
operator|->
name|pending
operator|.
name|nr
operator|=
literal|0
expr_stmt|;
name|revs
operator|->
name|pending
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
name|revs
operator|->
name|pending
operator|.
name|objects
operator|=
name|NULL
expr_stmt|;
block|}
block|}
end_function
end_unit
