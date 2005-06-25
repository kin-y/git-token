begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|active_cache
name|struct
name|cache_entry
modifier|*
modifier|*
name|active_cache
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|active_nr
DECL|variable|active_alloc
DECL|variable|active_cache_changed
name|unsigned
name|int
name|active_nr
init|=
literal|0
decl_stmt|,
name|active_alloc
init|=
literal|0
decl_stmt|,
name|active_cache_changed
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * This only updates the "non-critical" parts of the directory  * cache, ie the parts that aren't tracked by GIT, and only used  * to validate the cache.  */
end_comment
begin_function
DECL|function|fill_stat_cache_info
name|void
name|fill_stat_cache_info
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
block|{
name|ce
operator|->
name|ce_ctime
operator|.
name|sec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_ctime
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mtime
operator|.
name|sec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_mtime
argument_list|)
expr_stmt|;
ifdef|#
directive|ifdef
name|USE_NSEC
name|ce
operator|->
name|ce_ctime
operator|.
name|nsec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_ctim
operator|.
name|tv_nsec
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mtime
operator|.
name|nsec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_mtim
operator|.
name|tv_nsec
argument_list|)
expr_stmt|;
endif|#
directive|endif
name|ce
operator|->
name|ce_dev
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_dev
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_ino
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_ino
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_uid
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_uid
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_gid
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_gid
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_size
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ce_match_stat
name|int
name|ce_match_stat
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
block|{
name|unsigned
name|int
name|changed
init|=
literal|0
decl_stmt|;
switch|switch
condition|(
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|&
name|S_IFMT
condition|)
block|{
case|case
name|S_IFREG
case|:
name|changed
operator||=
operator|!
name|S_ISREG
argument_list|(
name|st
operator|->
name|st_mode
argument_list|)
condition|?
name|TYPE_CHANGED
else|:
literal|0
expr_stmt|;
comment|/* We consider only the owner x bit to be relevant for "mode changes" */
if|if
condition|(
literal|0100
operator|&
operator|(
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|^
name|st
operator|->
name|st_mode
operator|)
condition|)
name|changed
operator||=
name|MODE_CHANGED
expr_stmt|;
break|break;
case|case
name|S_IFLNK
case|:
name|changed
operator||=
operator|!
name|S_ISLNK
argument_list|(
name|st
operator|->
name|st_mode
argument_list|)
condition|?
name|TYPE_CHANGED
else|:
literal|0
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"internal error: ce_mode is %o"
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|ce
operator|->
name|ce_mtime
operator|.
name|sec
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_mtime
argument_list|)
condition|)
name|changed
operator||=
name|MTIME_CHANGED
expr_stmt|;
if|if
condition|(
name|ce
operator|->
name|ce_ctime
operator|.
name|sec
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_ctime
argument_list|)
condition|)
name|changed
operator||=
name|CTIME_CHANGED
expr_stmt|;
ifdef|#
directive|ifdef
name|USE_NSEC
comment|/* 	 * nsec seems unreliable - not all filesystems support it, so 	 * as long as it is in the inode cache you get right nsec 	 * but after it gets flushed, you get zero nsec. 	 */
if|if
condition|(
name|ce
operator|->
name|ce_mtime
operator|.
name|nsec
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_mtim
operator|.
name|tv_nsec
argument_list|)
condition|)
name|changed
operator||=
name|MTIME_CHANGED
expr_stmt|;
if|if
condition|(
name|ce
operator|->
name|ce_ctime
operator|.
name|nsec
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_ctim
operator|.
name|tv_nsec
argument_list|)
condition|)
name|changed
operator||=
name|CTIME_CHANGED
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
name|ce
operator|->
name|ce_uid
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_uid
argument_list|)
operator|||
name|ce
operator|->
name|ce_gid
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_gid
argument_list|)
condition|)
name|changed
operator||=
name|OWNER_CHANGED
expr_stmt|;
if|if
condition|(
name|ce
operator|->
name|ce_ino
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_ino
argument_list|)
condition|)
name|changed
operator||=
name|INODE_CHANGED
expr_stmt|;
ifdef|#
directive|ifdef
name|USE_STDEV
comment|/* 	 * st_dev breaks on network filesystems where different 	 * clients will have different views of what "device" 	 * the filesystem is on 	 */
if|if
condition|(
name|ce
operator|->
name|ce_dev
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_dev
argument_list|)
condition|)
name|changed
operator||=
name|INODE_CHANGED
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
name|ce
operator|->
name|ce_size
operator|!=
name|htonl
argument_list|(
name|st
operator|->
name|st_size
argument_list|)
condition|)
name|changed
operator||=
name|DATA_CHANGED
expr_stmt|;
return|return
name|changed
return|;
block|}
end_function
begin_function
DECL|function|base_name_compare
name|int
name|base_name_compare
parameter_list|(
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|int
name|len1
parameter_list|,
name|int
name|mode1
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
name|int
name|len2
parameter_list|,
name|int
name|mode2
parameter_list|)
block|{
name|unsigned
name|char
name|c1
decl_stmt|,
name|c2
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
name|int
name|cmp
decl_stmt|;
name|cmp
operator|=
name|memcmp
argument_list|(
name|name1
argument_list|,
name|name2
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
condition|)
return|return
name|cmp
return|;
name|c1
operator|=
name|name1
index|[
name|len
index|]
expr_stmt|;
name|c2
operator|=
name|name2
index|[
name|len
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|c1
operator|&&
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
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
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
name|c2
operator|=
literal|'/'
expr_stmt|;
return|return
operator|(
name|c1
operator|<
name|c2
operator|)
condition|?
operator|-
literal|1
else|:
operator|(
name|c1
operator|>
name|c2
operator|)
condition|?
literal|1
else|:
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cache_name_compare
name|int
name|cache_name_compare
parameter_list|(
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|int
name|flags1
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
name|int
name|flags2
parameter_list|)
block|{
name|int
name|len1
init|=
name|flags1
operator|&
name|CE_NAMEMASK
decl_stmt|;
name|int
name|len2
init|=
name|flags2
operator|&
name|CE_NAMEMASK
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
name|int
name|cmp
decl_stmt|;
name|cmp
operator|=
name|memcmp
argument_list|(
name|name1
argument_list|,
name|name2
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
condition|)
return|return
name|cmp
return|;
if|if
condition|(
name|len1
operator|<
name|len2
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|len1
operator|>
name|len2
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|flags1
operator|<
name|flags2
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|flags1
operator|>
name|flags2
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
DECL|function|cache_name_pos
name|int
name|cache_name_pos
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|namelen
parameter_list|)
block|{
name|int
name|first
decl_stmt|,
name|last
decl_stmt|;
name|first
operator|=
literal|0
expr_stmt|;
name|last
operator|=
name|active_nr
expr_stmt|;
while|while
condition|(
name|last
operator|>
name|first
condition|)
block|{
name|int
name|next
init|=
operator|(
name|last
operator|+
name|first
operator|)
operator|>>
literal|1
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|next
index|]
decl_stmt|;
name|int
name|cmp
init|=
name|cache_name_compare
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|ntohs
argument_list|(
name|ce
operator|->
name|ce_flags
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
return|return
name|next
return|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
block|{
name|last
operator|=
name|next
expr_stmt|;
continue|continue;
block|}
name|first
operator|=
name|next
operator|+
literal|1
expr_stmt|;
block|}
return|return
operator|-
name|first
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/* Remove entry, return true if there are more entries to go.. */
end_comment
begin_function
DECL|function|remove_cache_entry_at
name|int
name|remove_cache_entry_at
parameter_list|(
name|int
name|pos
parameter_list|)
block|{
name|active_cache_changed
operator|=
literal|1
expr_stmt|;
name|active_nr
operator|--
expr_stmt|;
if|if
condition|(
name|pos
operator|>=
name|active_nr
condition|)
return|return
literal|0
return|;
name|memmove
argument_list|(
name|active_cache
operator|+
name|pos
argument_list|,
name|active_cache
operator|+
name|pos
operator|+
literal|1
argument_list|,
operator|(
name|active_nr
operator|-
name|pos
operator|)
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|cache_entry
operator|*
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|remove_file_from_cache
name|int
name|remove_file_from_cache
parameter_list|(
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|path
argument_list|,
name|strlen
argument_list|(
name|path
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|pos
operator|<
name|active_nr
operator|&&
operator|!
name|strcmp
argument_list|(
name|active_cache
index|[
name|pos
index|]
operator|->
name|name
argument_list|,
name|path
argument_list|)
condition|)
name|remove_cache_entry_at
argument_list|(
name|pos
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|ce_same_name
name|int
name|ce_same_name
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|a
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|b
parameter_list|)
block|{
name|int
name|len
init|=
name|ce_namelen
argument_list|(
name|a
argument_list|)
decl_stmt|;
return|return
name|ce_namelen
argument_list|(
name|b
argument_list|)
operator|==
name|len
operator|&&
operator|!
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
return|;
block|}
end_function
begin_comment
comment|/*  * Do we have another file that has the beginning components being a  * proper superset of the name we're trying to add?  */
end_comment
begin_function
DECL|function|has_file_name
specifier|static
name|int
name|has_file_name
parameter_list|(
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|pos
parameter_list|,
name|int
name|ok_to_replace
parameter_list|)
block|{
name|int
name|retval
init|=
literal|0
decl_stmt|;
name|int
name|len
init|=
name|ce_namelen
argument_list|(
name|ce
argument_list|)
decl_stmt|;
name|int
name|stage
init|=
name|ce_stage
argument_list|(
name|ce
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|ce
operator|->
name|name
decl_stmt|;
while|while
condition|(
name|pos
operator|<
name|active_nr
condition|)
block|{
name|struct
name|cache_entry
modifier|*
name|p
init|=
name|active_cache
index|[
name|pos
operator|++
index|]
decl_stmt|;
if|if
condition|(
name|len
operator|>=
name|ce_namelen
argument_list|(
name|p
argument_list|)
condition|)
break|break;
if|if
condition|(
name|memcmp
argument_list|(
name|name
argument_list|,
name|p
operator|->
name|name
argument_list|,
name|len
argument_list|)
condition|)
break|break;
if|if
condition|(
name|ce_stage
argument_list|(
name|p
argument_list|)
operator|!=
name|stage
condition|)
continue|continue;
if|if
condition|(
name|p
operator|->
name|name
index|[
name|len
index|]
operator|!=
literal|'/'
condition|)
continue|continue;
name|retval
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|ok_to_replace
condition|)
break|break;
name|remove_cache_entry_at
argument_list|(
operator|--
name|pos
argument_list|)
expr_stmt|;
block|}
return|return
name|retval
return|;
block|}
end_function
begin_comment
comment|/*  * Do we have another file with a pathname that is a proper  * subset of the name we're trying to add?  */
end_comment
begin_function
DECL|function|has_dir_name
specifier|static
name|int
name|has_dir_name
parameter_list|(
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|pos
parameter_list|,
name|int
name|ok_to_replace
parameter_list|)
block|{
name|int
name|retval
init|=
literal|0
decl_stmt|;
name|int
name|stage
init|=
name|ce_stage
argument_list|(
name|ce
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|ce
operator|->
name|name
decl_stmt|;
specifier|const
name|char
modifier|*
name|slash
init|=
name|name
operator|+
name|ce_namelen
argument_list|(
name|ce
argument_list|)
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|len
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
if|if
condition|(
operator|*
operator|--
name|slash
operator|==
literal|'/'
condition|)
break|break;
if|if
condition|(
name|slash
operator|<=
name|ce
operator|->
name|name
condition|)
return|return
name|retval
return|;
block|}
name|len
operator|=
name|slash
operator|-
name|name
expr_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|ntohs
argument_list|(
name|create_ce_flags
argument_list|(
name|len
argument_list|,
name|stage
argument_list|)
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|>=
literal|0
condition|)
block|{
name|retval
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|ok_to_replace
condition|)
break|break;
name|remove_cache_entry_at
argument_list|(
name|pos
argument_list|)
expr_stmt|;
continue|continue;
block|}
comment|/* 		 * Trivial optimization: if we find an entry that 		 * already matches the sub-directory, then we know 		 * we're ok, and we can exit. 		 */
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|pos
operator|<
name|active_nr
condition|)
block|{
name|struct
name|cache_entry
modifier|*
name|p
init|=
name|active_cache
index|[
name|pos
index|]
decl_stmt|;
if|if
condition|(
operator|(
name|ce_namelen
argument_list|(
name|p
argument_list|)
operator|<=
name|len
operator|)
operator|||
operator|(
name|p
operator|->
name|name
index|[
name|len
index|]
operator|!=
literal|'/'
operator|)
operator|||
name|memcmp
argument_list|(
name|p
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
condition|)
break|break;
comment|/* not our subdirectory */
if|if
condition|(
name|ce_stage
argument_list|(
name|p
argument_list|)
operator|==
name|stage
condition|)
comment|/* p is at the same stage as our entry, and 				 * is a subdirectory of what we are looking 				 * at, so we cannot have conflicts at our 				 * level or anything shorter. 				 */
return|return
name|retval
return|;
name|pos
operator|++
expr_stmt|;
block|}
block|}
return|return
name|retval
return|;
block|}
end_function
begin_comment
comment|/* We may be in a situation where we already have path/file and path  * is being added, or we already have path and path/file is being  * added.  Either one would result in a nonsense tree that has path  * twice when git-write-tree tries to write it out.  Prevent it.  *   * If ok-to-replace is specified, we remove the conflicting entries  * from the cache so the caller should recompute the insert position.  * When this happens, we return non-zero.  */
end_comment
begin_function
DECL|function|check_file_directory_conflict
specifier|static
name|int
name|check_file_directory_conflict
parameter_list|(
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|pos
parameter_list|,
name|int
name|ok_to_replace
parameter_list|)
block|{
comment|/* 	 * We check if the path is a sub-path of a subsequent pathname 	 * first, since removing those will not change the position 	 * in the array 	 */
name|int
name|retval
init|=
name|has_file_name
argument_list|(
name|ce
argument_list|,
name|pos
argument_list|,
name|ok_to_replace
argument_list|)
decl_stmt|;
comment|/* 	 * Then check if the path might have a clashing sub-directory 	 * before it. 	 */
return|return
name|retval
operator|+
name|has_dir_name
argument_list|(
name|ce
argument_list|,
name|pos
argument_list|,
name|ok_to_replace
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|add_cache_entry
name|int
name|add_cache_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|option
parameter_list|)
block|{
name|int
name|pos
decl_stmt|;
name|int
name|ok_to_add
init|=
name|option
operator|&
name|ADD_CACHE_OK_TO_ADD
decl_stmt|;
name|int
name|ok_to_replace
init|=
name|option
operator|&
name|ADD_CACHE_OK_TO_REPLACE
decl_stmt|;
name|int
name|skip_df_check
init|=
name|option
operator|&
name|ADD_CACHE_SKIP_DFCHECK
decl_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|ntohs
argument_list|(
name|ce
operator|->
name|ce_flags
argument_list|)
argument_list|)
expr_stmt|;
comment|/* existing match? Just replace it */
if|if
condition|(
name|pos
operator|>=
literal|0
condition|)
block|{
name|active_cache_changed
operator|=
literal|1
expr_stmt|;
name|active_cache
index|[
name|pos
index|]
operator|=
name|ce
expr_stmt|;
return|return
literal|0
return|;
block|}
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
comment|/* 	 * Inserting a merged entry ("stage 0") into the index 	 * will always replace all non-merged entries.. 	 */
if|if
condition|(
name|pos
operator|<
name|active_nr
operator|&&
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|==
literal|0
condition|)
block|{
while|while
condition|(
name|ce_same_name
argument_list|(
name|active_cache
index|[
name|pos
index|]
argument_list|,
name|ce
argument_list|)
condition|)
block|{
name|ok_to_add
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|remove_cache_entry_at
argument_list|(
name|pos
argument_list|)
condition|)
break|break;
block|}
block|}
if|if
condition|(
operator|!
name|ok_to_add
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|skip_df_check
operator|&&
name|check_file_directory_conflict
argument_list|(
name|ce
argument_list|,
name|pos
argument_list|,
name|ok_to_replace
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|ok_to_replace
condition|)
return|return
operator|-
literal|1
return|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|ntohs
argument_list|(
name|ce
operator|->
name|ce_flags
argument_list|)
argument_list|)
expr_stmt|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
block|}
comment|/* Make sure the array is big enough .. */
if|if
condition|(
name|active_nr
operator|==
name|active_alloc
condition|)
block|{
name|active_alloc
operator|=
name|alloc_nr
argument_list|(
name|active_alloc
argument_list|)
expr_stmt|;
name|active_cache
operator|=
name|xrealloc
argument_list|(
name|active_cache
argument_list|,
name|active_alloc
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|cache_entry
operator|*
argument_list|)
argument_list|)
expr_stmt|;
block|}
comment|/* Add it in.. */
name|active_nr
operator|++
expr_stmt|;
if|if
condition|(
name|active_nr
operator|>
name|pos
condition|)
name|memmove
argument_list|(
name|active_cache
operator|+
name|pos
operator|+
literal|1
argument_list|,
name|active_cache
operator|+
name|pos
argument_list|,
operator|(
name|active_nr
operator|-
name|pos
operator|-
literal|1
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|ce
argument_list|)
argument_list|)
expr_stmt|;
name|active_cache
index|[
name|pos
index|]
operator|=
name|ce
expr_stmt|;
name|active_cache_changed
operator|=
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|verify_hdr
specifier|static
name|int
name|verify_hdr
parameter_list|(
name|struct
name|cache_header
modifier|*
name|hdr
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|SHA_CTX
name|c
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|hdr
operator|->
name|hdr_signature
operator|!=
name|htonl
argument_list|(
name|CACHE_SIGNATURE
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"bad signature"
argument_list|)
return|;
if|if
condition|(
name|hdr
operator|->
name|hdr_version
operator|!=
name|htonl
argument_list|(
literal|2
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"bad index version"
argument_list|)
return|;
name|SHA1_Init
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|hdr
argument_list|,
name|size
operator|-
literal|20
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|sha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|sha1
argument_list|,
operator|(
name|void
operator|*
operator|)
name|hdr
operator|+
name|size
operator|-
literal|20
argument_list|,
literal|20
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"bad index file sha1 signature"
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_cache
name|int
name|read_cache
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|i
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|,
name|offset
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|;
name|struct
name|cache_header
modifier|*
name|hdr
decl_stmt|;
name|errno
operator|=
name|EBUSY
expr_stmt|;
if|if
condition|(
name|active_cache
condition|)
return|return
name|error
argument_list|(
literal|"more than one cachefile"
argument_list|)
return|;
name|errno
operator|=
name|ENOENT
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|get_index_file
argument_list|()
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
return|return
operator|(
name|errno
operator|==
name|ENOENT
operator|)
condition|?
literal|0
else|:
name|error
argument_list|(
literal|"open failed"
argument_list|)
return|;
name|size
operator|=
literal|0
expr_stmt|;
comment|// avoid gcc warning
name|map
operator|=
operator|(
name|void
operator|*
operator|)
operator|-
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|size
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|errno
operator|=
name|EINVAL
expr_stmt|;
if|if
condition|(
name|size
operator|>=
sizeof|sizeof
argument_list|(
expr|struct
name|cache_header
argument_list|)
operator|+
literal|20
condition|)
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|size
argument_list|,
name|PROT_READ
operator||
name|PROT_WRITE
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
operator|-
literal|1
operator|==
operator|(
name|int
operator|)
operator|(
name|long
operator|)
name|map
condition|)
return|return
name|error
argument_list|(
literal|"mmap failed"
argument_list|)
return|;
name|hdr
operator|=
name|map
expr_stmt|;
if|if
condition|(
name|verify_hdr
argument_list|(
name|hdr
argument_list|,
name|size
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|unmap
goto|;
name|active_nr
operator|=
name|ntohl
argument_list|(
name|hdr
operator|->
name|hdr_entries
argument_list|)
expr_stmt|;
name|active_alloc
operator|=
name|alloc_nr
argument_list|(
name|active_nr
argument_list|)
expr_stmt|;
name|active_cache
operator|=
name|calloc
argument_list|(
name|active_alloc
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|cache_entry
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|offset
operator|=
sizeof|sizeof
argument_list|(
operator|*
name|hdr
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
name|map
operator|+
name|offset
decl_stmt|;
name|offset
operator|=
name|offset
operator|+
name|ce_size
argument_list|(
name|ce
argument_list|)
expr_stmt|;
name|active_cache
index|[
name|i
index|]
operator|=
name|ce
expr_stmt|;
block|}
return|return
name|active_nr
return|;
name|unmap
label|:
name|munmap
argument_list|(
name|map
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
name|error
argument_list|(
literal|"verify header failed"
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|WRITE_BUFFER_SIZE
define|#
directive|define
name|WRITE_BUFFER_SIZE
value|8192
end_define
begin_decl_stmt
DECL|variable|write_buffer
specifier|static
name|unsigned
name|char
name|write_buffer
index|[
name|WRITE_BUFFER_SIZE
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|write_buffer_len
specifier|static
name|unsigned
name|long
name|write_buffer_len
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|ce_write
specifier|static
name|int
name|ce_write
parameter_list|(
name|SHA_CTX
modifier|*
name|context
parameter_list|,
name|int
name|fd
parameter_list|,
name|void
modifier|*
name|data
parameter_list|,
name|unsigned
name|int
name|len
parameter_list|)
block|{
while|while
condition|(
name|len
condition|)
block|{
name|unsigned
name|int
name|buffered
init|=
name|write_buffer_len
decl_stmt|;
name|unsigned
name|int
name|partial
init|=
name|WRITE_BUFFER_SIZE
operator|-
name|buffered
decl_stmt|;
if|if
condition|(
name|partial
operator|>
name|len
condition|)
name|partial
operator|=
name|len
expr_stmt|;
name|memcpy
argument_list|(
name|write_buffer
operator|+
name|buffered
argument_list|,
name|data
argument_list|,
name|partial
argument_list|)
expr_stmt|;
name|buffered
operator|+=
name|partial
expr_stmt|;
if|if
condition|(
name|buffered
operator|==
name|WRITE_BUFFER_SIZE
condition|)
block|{
name|SHA1_Update
argument_list|(
name|context
argument_list|,
name|write_buffer
argument_list|,
name|WRITE_BUFFER_SIZE
argument_list|)
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|write_buffer
argument_list|,
name|WRITE_BUFFER_SIZE
argument_list|)
operator|!=
name|WRITE_BUFFER_SIZE
condition|)
return|return
operator|-
literal|1
return|;
name|buffered
operator|=
literal|0
expr_stmt|;
block|}
name|write_buffer_len
operator|=
name|buffered
expr_stmt|;
name|len
operator|-=
name|partial
expr_stmt|;
name|data
operator|+=
name|partial
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|ce_flush
specifier|static
name|int
name|ce_flush
parameter_list|(
name|SHA_CTX
modifier|*
name|context
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
name|unsigned
name|int
name|left
init|=
name|write_buffer_len
decl_stmt|;
if|if
condition|(
name|left
condition|)
block|{
name|write_buffer_len
operator|=
literal|0
expr_stmt|;
name|SHA1_Update
argument_list|(
name|context
argument_list|,
name|write_buffer
argument_list|,
name|left
argument_list|)
expr_stmt|;
block|}
comment|/* Append the SHA1 signature at the end */
name|SHA1_Final
argument_list|(
name|write_buffer
operator|+
name|left
argument_list|,
name|context
argument_list|)
expr_stmt|;
name|left
operator|+=
literal|20
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|write_buffer
argument_list|,
name|left
argument_list|)
operator|!=
name|left
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_cache
name|int
name|write_cache
parameter_list|(
name|int
name|newfd
parameter_list|,
name|struct
name|cache_entry
modifier|*
modifier|*
name|cache
parameter_list|,
name|int
name|entries
parameter_list|)
block|{
name|SHA_CTX
name|c
decl_stmt|;
name|struct
name|cache_header
name|hdr
decl_stmt|;
name|int
name|i
decl_stmt|,
name|removed
decl_stmt|;
for|for
control|(
name|i
operator|=
name|removed
operator|=
literal|0
init|;
name|i
operator|<
name|entries
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
name|cache
index|[
name|i
index|]
operator|->
name|ce_mode
condition|)
name|removed
operator|++
expr_stmt|;
name|hdr
operator|.
name|hdr_signature
operator|=
name|htonl
argument_list|(
name|CACHE_SIGNATURE
argument_list|)
expr_stmt|;
name|hdr
operator|.
name|hdr_version
operator|=
name|htonl
argument_list|(
literal|2
argument_list|)
expr_stmt|;
name|hdr
operator|.
name|hdr_entries
operator|=
name|htonl
argument_list|(
name|entries
operator|-
name|removed
argument_list|)
expr_stmt|;
name|SHA1_Init
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|ce_write
argument_list|(
operator|&
name|c
argument_list|,
name|newfd
argument_list|,
operator|&
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|entries
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
name|cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|ce
operator|->
name|ce_mode
condition|)
continue|continue;
if|if
condition|(
name|ce_write
argument_list|(
operator|&
name|c
argument_list|,
name|newfd
argument_list|,
name|ce
argument_list|,
name|ce_size
argument_list|(
name|ce
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
name|ce_flush
argument_list|(
operator|&
name|c
argument_list|,
name|newfd
argument_list|)
return|;
block|}
end_function
end_unit
