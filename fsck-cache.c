begin_unit
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<dirent.h>
end_include
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
file|"tag.h"
end_include
begin_define
DECL|macro|REACHABLE
define|#
directive|define
name|REACHABLE
value|0x0001
end_define
begin_decl_stmt
DECL|variable|show_root
specifier|static
name|int
name|show_root
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_tags
specifier|static
name|int
name|show_tags
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_unreachable
specifier|static
name|int
name|show_unreachable
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|keep_cache_objects
specifier|static
name|int
name|keep_cache_objects
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|head_sha1
specifier|static
name|unsigned
name|char
name|head_sha1
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|check_connectivity
specifier|static
name|void
name|check_connectivity
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
comment|/* Look up all the requirements, warn about missing objects.. */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_objs
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|objs
index|[
name|i
index|]
decl_stmt|;
name|struct
name|object_list
modifier|*
name|refs
decl_stmt|;
if|if
condition|(
operator|!
name|obj
operator|->
name|parsed
condition|)
block|{
name|printf
argument_list|(
literal|"missing %s %s\n"
argument_list|,
name|obj
operator|->
name|type
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
continue|continue;
block|}
for|for
control|(
name|refs
operator|=
name|obj
operator|->
name|refs
init|;
name|refs
condition|;
name|refs
operator|=
name|refs
operator|->
name|next
control|)
block|{
if|if
condition|(
name|refs
operator|->
name|item
operator|->
name|parsed
condition|)
continue|continue;
name|printf
argument_list|(
literal|"broken link from %7s %s\n"
argument_list|,
name|obj
operator|->
name|type
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"              to %7s %s\n"
argument_list|,
name|refs
operator|->
name|item
operator|->
name|type
argument_list|,
name|sha1_to_hex
argument_list|(
name|refs
operator|->
name|item
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|show_unreachable
operator|&&
operator|!
operator|(
name|obj
operator|->
name|flags
operator|&
name|REACHABLE
operator|)
condition|)
block|{
name|printf
argument_list|(
literal|"unreachable %s %s\n"
argument_list|,
name|obj
operator|->
name|type
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|obj
operator|->
name|used
condition|)
block|{
name|printf
argument_list|(
literal|"dangling %s %s\n"
argument_list|,
name|obj
operator|->
name|type
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_comment
comment|/*  * The entries in a tree are ordered in the _path_ order,  * which means that a directory entry is ordered by adding  * a slash to the end of it.  *  * So a directory called "a" is ordered _after_ a file  * called "a.c", because "a/" sorts after "a.c".  */
end_comment
begin_define
DECL|macro|TREE_UNORDERED
define|#
directive|define
name|TREE_UNORDERED
value|(-1)
end_define
begin_define
DECL|macro|TREE_HAS_DUPS
define|#
directive|define
name|TREE_HAS_DUPS
value|(-2)
end_define
begin_function
DECL|function|verify_ordered
specifier|static
name|int
name|verify_ordered
parameter_list|(
name|struct
name|tree_entry_list
modifier|*
name|a
parameter_list|,
name|struct
name|tree_entry_list
modifier|*
name|b
parameter_list|)
block|{
name|int
name|len1
init|=
name|strlen
argument_list|(
name|a
operator|->
name|name
argument_list|)
decl_stmt|;
name|int
name|len2
init|=
name|strlen
argument_list|(
name|b
operator|->
name|name
argument_list|)
decl_stmt|;
name|int
name|len
init|=
name|len1
operator|<
name|len2
condition|?
name|len1
else|:
name|len2
decl_stmt|;
name|unsigned
name|char
name|c1
decl_stmt|,
name|c2
decl_stmt|;
name|int
name|cmp
decl_stmt|;
name|cmp
operator|=
name|memcmp
argument_list|(
name|a
operator|->
name|name
argument_list|,
name|b
operator|->
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
return|return
name|TREE_UNORDERED
return|;
comment|/* 	 * Ok, the first<len> characters are the same. 	 * Now we need to order the next one, but turn 	 * a '\0' into a '/' for a directory entry. 	 */
name|c1
operator|=
name|a
operator|->
name|name
index|[
name|len
index|]
expr_stmt|;
name|c2
operator|=
name|b
operator|->
name|name
index|[
name|len
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|c1
operator|&&
operator|!
name|c2
condition|)
comment|/* 		 * git-write-tree used to write out a nonsense tree that has 		 * entries with the same name, one blob and one tree.  Make 		 * sure we do not have duplicate entries. 		 */
return|return
name|TREE_HAS_DUPS
return|;
if|if
condition|(
operator|!
name|c1
operator|&&
name|a
operator|->
name|directory
condition|)
name|c1
operator|=
literal|'/'
expr_stmt|;
if|if
condition|(
operator|!
name|c2
operator|&&
name|b
operator|->
name|directory
condition|)
name|c2
operator|=
literal|'/'
expr_stmt|;
return|return
name|c1
operator|<
name|c2
condition|?
literal|0
else|:
name|TREE_UNORDERED
return|;
block|}
end_function
begin_function
DECL|function|fsck_tree
specifier|static
name|int
name|fsck_tree
parameter_list|(
name|struct
name|tree
modifier|*
name|item
parameter_list|)
block|{
name|int
name|has_full_path
init|=
literal|0
decl_stmt|;
name|struct
name|tree_entry_list
modifier|*
name|entry
decl_stmt|,
modifier|*
name|last
decl_stmt|;
name|last
operator|=
name|NULL
expr_stmt|;
for|for
control|(
name|entry
operator|=
name|item
operator|->
name|entries
init|;
name|entry
condition|;
name|entry
operator|=
name|entry
operator|->
name|next
control|)
block|{
if|if
condition|(
name|strchr
argument_list|(
name|entry
operator|->
name|name
argument_list|,
literal|'/'
argument_list|)
condition|)
name|has_full_path
operator|=
literal|1
expr_stmt|;
switch|switch
condition|(
name|entry
operator|->
name|mode
condition|)
block|{
comment|/* 		 * Standard modes..  		 */
case|case
name|S_IFREG
operator||
literal|0755
case|:
case|case
name|S_IFREG
operator||
literal|0644
case|:
case|case
name|S_IFLNK
case|:
case|case
name|S_IFDIR
case|:
break|break;
comment|/* 		 * This is nonstandard, but we had a few of these 		 * early on when we honored the full set of mode 		 * bits.. 		 */
case|case
name|S_IFREG
operator||
literal|0664
case|:
break|break;
default|default:
name|printf
argument_list|(
literal|"tree %s has entry %o %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|,
name|entry
operator|->
name|mode
argument_list|,
name|entry
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|last
condition|)
block|{
switch|switch
condition|(
name|verify_ordered
argument_list|(
name|last
argument_list|,
name|entry
argument_list|)
condition|)
block|{
case|case
name|TREE_UNORDERED
case|:
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"tree %s not ordered\n"
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
expr_stmt|;
return|return
operator|-
literal|1
return|;
case|case
name|TREE_HAS_DUPS
case|:
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"tree %s has duplicate entries for '%s'\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|,
name|entry
operator|->
name|name
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
default|default:
break|break;
block|}
block|}
name|last
operator|=
name|entry
expr_stmt|;
block|}
if|if
condition|(
name|has_full_path
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"warning: git-fsck-cache: tree %s "
literal|"has full pathnames in it\n"
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
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fsck_commit
specifier|static
name|int
name|fsck_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|free
argument_list|(
name|commit
operator|->
name|buffer
argument_list|)
expr_stmt|;
name|commit
operator|->
name|buffer
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|->
name|tree
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|commit
operator|->
name|parents
operator|&&
name|show_root
condition|)
name|printf
argument_list|(
literal|"root %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|->
name|date
condition|)
name|printf
argument_list|(
literal|"bad commit date in %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fsck_tag
specifier|static
name|int
name|fsck_tag
parameter_list|(
name|struct
name|tag
modifier|*
name|tag
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|tagged
init|=
name|tag
operator|->
name|tagged
decl_stmt|;
if|if
condition|(
operator|!
name|tagged
condition|)
block|{
name|printf
argument_list|(
literal|"bad object in tag %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|tag
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|show_tags
condition|)
return|return
literal|0
return|;
name|printf
argument_list|(
literal|"tagged %s %s"
argument_list|,
name|tagged
operator|->
name|type
argument_list|,
name|sha1_to_hex
argument_list|(
name|tagged
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|" (%s) in %s\n"
argument_list|,
name|tag
operator|->
name|tag
argument_list|,
name|sha1_to_hex
argument_list|(
name|tag
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fsck_sha1
specifier|static
name|int
name|fsck_sha1
parameter_list|(
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
if|if
condition|(
operator|!
name|obj
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|blob_type
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|tree_type
condition|)
return|return
name|fsck_tree
argument_list|(
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
argument_list|)
return|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|commit_type
condition|)
return|return
name|fsck_commit
argument_list|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
argument_list|)
return|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|tag_type
condition|)
return|return
name|fsck_tag
argument_list|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|obj
argument_list|)
return|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * This is the sorting chunk size: make it reasonably  * big so that we can sort well..  */
end_comment
begin_define
DECL|macro|MAX_SHA1_ENTRIES
define|#
directive|define
name|MAX_SHA1_ENTRIES
value|(1024)
end_define
begin_struct
DECL|struct|sha1_entry
struct|struct
name|sha1_entry
block|{
DECL|member|ino
name|unsigned
name|long
name|ino
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
specifier|static
struct|struct
block|{
DECL|member|nr
name|unsigned
name|long
name|nr
decl_stmt|;
DECL|member|entry
name|struct
name|sha1_entry
modifier|*
name|entry
index|[
name|MAX_SHA1_ENTRIES
index|]
decl_stmt|;
block|}
DECL|variable|sha1_list
name|sha1_list
struct|;
end_struct
begin_function
DECL|function|ino_compare
specifier|static
name|int
name|ino_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|_a
parameter_list|,
specifier|const
name|void
modifier|*
name|_b
parameter_list|)
block|{
specifier|const
name|struct
name|sha1_entry
modifier|*
name|a
init|=
name|_a
decl_stmt|,
modifier|*
name|b
init|=
name|_b
decl_stmt|;
name|unsigned
name|long
name|ino1
init|=
name|a
operator|->
name|ino
decl_stmt|,
name|ino2
init|=
name|b
operator|->
name|ino
decl_stmt|;
return|return
name|ino1
operator|<
name|ino2
condition|?
operator|-
literal|1
else|:
name|ino1
operator|>
name|ino2
condition|?
literal|1
else|:
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fsck_sha1_list
specifier|static
name|void
name|fsck_sha1_list
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|nr
init|=
name|sha1_list
operator|.
name|nr
decl_stmt|;
name|qsort
argument_list|(
name|sha1_list
operator|.
name|entry
argument_list|,
name|nr
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|sha1_entry
operator|*
argument_list|)
argument_list|,
name|ino_compare
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
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|sha1_entry
modifier|*
name|entry
init|=
name|sha1_list
operator|.
name|entry
index|[
name|i
index|]
decl_stmt|;
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|entry
operator|->
name|sha1
decl_stmt|;
name|sha1_list
operator|.
name|entry
index|[
name|i
index|]
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|fsck_sha1
argument_list|(
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"bad sha1 entry '%s'\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|entry
argument_list|)
expr_stmt|;
block|}
name|sha1_list
operator|.
name|nr
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_sha1_list
specifier|static
name|void
name|add_sha1_list
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|long
name|ino
parameter_list|)
block|{
name|struct
name|sha1_entry
modifier|*
name|entry
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|entry
argument_list|)
argument_list|)
decl_stmt|;
name|int
name|nr
decl_stmt|;
name|entry
operator|->
name|ino
operator|=
name|ino
expr_stmt|;
name|memcpy
argument_list|(
name|entry
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|nr
operator|=
name|sha1_list
operator|.
name|nr
expr_stmt|;
if|if
condition|(
name|nr
operator|==
name|MAX_SHA1_ENTRIES
condition|)
block|{
name|fsck_sha1_list
argument_list|()
expr_stmt|;
name|nr
operator|=
literal|0
expr_stmt|;
block|}
name|sha1_list
operator|.
name|entry
index|[
name|nr
index|]
operator|=
name|entry
expr_stmt|;
name|sha1_list
operator|.
name|nr
operator|=
operator|++
name|nr
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fsck_dir
specifier|static
name|int
name|fsck_dir
parameter_list|(
name|int
name|i
parameter_list|,
name|char
modifier|*
name|path
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
block|{
return|return
name|error
argument_list|(
literal|"missing sha1 directory '%s'"
argument_list|,
name|path
argument_list|)
return|;
block|}
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|char
name|name
index|[
literal|100
index|]
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
init|=
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
decl_stmt|;
switch|switch
condition|(
name|len
condition|)
block|{
case|case
literal|2
case|:
if|if
condition|(
name|de
operator|->
name|d_name
index|[
literal|1
index|]
operator|!=
literal|'.'
condition|)
break|break;
case|case
literal|1
case|:
if|if
condition|(
name|de
operator|->
name|d_name
index|[
literal|0
index|]
operator|!=
literal|'.'
condition|)
break|break;
continue|continue;
case|case
literal|38
case|:
name|sprintf
argument_list|(
name|name
argument_list|,
literal|"%02x"
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|name
operator|+
literal|2
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
break|break;
name|add_sha1_list
argument_list|(
name|sha1
argument_list|,
name|de
operator|->
name|d_ino
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"bad sha1 file: %s/%s\n"
argument_list|,
name|path
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_sha1_reference
specifier|static
name|int
name|read_sha1_reference
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|char
name|hexname
index|[
literal|60
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|fd
init|=
name|open
argument_list|(
name|path
argument_list|,
name|O_RDONLY
argument_list|)
decl_stmt|,
name|len
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|len
operator|=
name|read
argument_list|(
name|fd
argument_list|,
name|hexname
argument_list|,
sizeof|sizeof
argument_list|(
name|hexname
argument_list|)
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|40
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|hexname
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|obj
operator|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
return|return
name|error
argument_list|(
literal|"%s: invalid sha1 pointer %.40s"
argument_list|,
name|path
argument_list|,
name|hexname
argument_list|)
return|;
name|obj
operator|->
name|used
operator|=
literal|1
expr_stmt|;
name|mark_reachable
argument_list|(
name|obj
argument_list|,
name|REACHABLE
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|find_file_objects
specifier|static
name|int
name|find_file_objects
parameter_list|(
specifier|const
name|char
modifier|*
name|base
parameter_list|,
specifier|const
name|char
modifier|*
name|name
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
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
name|char
modifier|*
name|path
init|=
name|xmalloc
argument_list|(
name|baselen
operator|+
name|namelen
operator|+
literal|2
argument_list|)
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|memcpy
argument_list|(
name|path
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|path
index|[
name|baselen
index|]
operator|=
literal|'/'
expr_stmt|;
name|memcpy
argument_list|(
name|path
operator|+
name|baselen
operator|+
literal|1
argument_list|,
name|name
argument_list|,
name|namelen
operator|+
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|stat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
return|return
literal|0
return|;
comment|/* 	 * Recurse into directories 	 */
if|if
condition|(
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|int
name|count
init|=
literal|0
decl_stmt|;
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|path
argument_list|)
decl_stmt|;
if|if
condition|(
name|dir
condition|)
block|{
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
name|de
operator|->
name|d_name
index|[
literal|0
index|]
operator|==
literal|'.'
condition|)
continue|continue;
name|count
operator|+=
name|find_file_objects
argument_list|(
name|path
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
return|return
name|count
return|;
block|}
if|if
condition|(
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
return|return
name|read_sha1_reference
argument_list|(
name|path
argument_list|)
operator|==
literal|0
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_default_heads
specifier|static
name|void
name|get_default_heads
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|git_dir
init|=
name|gitenv
argument_list|(
name|GIT_DIR_ENVIRONMENT
argument_list|)
condition|?
else|:
name|DEFAULT_GIT_DIR_ENVIRONMENT
decl_stmt|;
name|int
name|count
init|=
name|find_file_objects
argument_list|(
name|git_dir
argument_list|,
literal|"refs"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|count
condition|)
name|die
argument_list|(
literal|"No default references"
argument_list|)
expr_stmt|;
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
decl_stmt|,
name|heads
decl_stmt|;
name|char
modifier|*
name|sha1_dir
decl_stmt|;
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
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--unreachable"
argument_list|)
condition|)
block|{
name|show_unreachable
operator|=
literal|1
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
literal|"--tags"
argument_list|)
condition|)
block|{
name|show_tags
operator|=
literal|1
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
literal|"--root"
argument_list|)
condition|)
block|{
name|show_root
operator|=
literal|1
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
literal|"--cache"
argument_list|)
condition|)
block|{
name|keep_cache_objects
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
name|usage
argument_list|(
literal|"git-fsck-cache [--tags] [[--unreachable] [--cache]<head-sha1>*]"
argument_list|)
expr_stmt|;
block|}
name|sha1_dir
operator|=
name|get_object_directory
argument_list|()
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|256
condition|;
name|i
operator|++
control|)
block|{
specifier|static
name|char
name|dir
index|[
literal|4096
index|]
decl_stmt|;
name|sprintf
argument_list|(
name|dir
argument_list|,
literal|"%s/%02x"
argument_list|,
name|sha1_dir
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|fsck_dir
argument_list|(
name|i
argument_list|,
name|dir
argument_list|)
expr_stmt|;
block|}
name|fsck_sha1_list
argument_list|()
expr_stmt|;
name|heads
operator|=
literal|0
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
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
continue|continue;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|head_sha1
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object
argument_list|(
name|head_sha1
argument_list|)
decl_stmt|;
comment|/* Error is printed by lookup_object(). */
if|if
condition|(
operator|!
name|obj
condition|)
continue|continue;
name|obj
operator|->
name|used
operator|=
literal|1
expr_stmt|;
name|mark_reachable
argument_list|(
name|obj
argument_list|,
name|REACHABLE
argument_list|)
expr_stmt|;
name|heads
operator|++
expr_stmt|;
continue|continue;
block|}
name|error
argument_list|(
literal|"expected sha1, got %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * If we've not been given any explicit head information, do the 	 * default ones from .git/refs. We also consider the index file 	 * in this case (ie this implies --cache). 	 */
if|if
condition|(
operator|!
name|heads
condition|)
block|{
name|get_default_heads
argument_list|()
expr_stmt|;
name|keep_cache_objects
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|keep_cache_objects
condition|)
block|{
name|int
name|i
decl_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|blob
modifier|*
name|blob
init|=
name|lookup_blob
argument_list|(
name|active_cache
index|[
name|i
index|]
operator|->
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
if|if
condition|(
operator|!
name|blob
condition|)
continue|continue;
name|obj
operator|=
operator|&
name|blob
operator|->
name|object
expr_stmt|;
name|obj
operator|->
name|used
operator|=
literal|1
expr_stmt|;
name|mark_reachable
argument_list|(
name|obj
argument_list|,
name|REACHABLE
argument_list|)
expr_stmt|;
block|}
block|}
name|check_connectivity
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
