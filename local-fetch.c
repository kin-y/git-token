begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
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
file|"fetch.h"
end_include
begin_decl_stmt
DECL|variable|use_link
specifier|static
name|int
name|use_link
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|use_symlink
specifier|static
name|int
name|use_symlink
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|use_filecopy
specifier|static
name|int
name|use_filecopy
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|path
specifier|static
name|char
modifier|*
name|path
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|path
comment|/* "Remote" git repository */
end_comment
begin_function
DECL|function|prefetch
name|void
name|prefetch
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{ }
end_function
begin_decl_stmt
DECL|variable|packs
specifier|static
name|struct
name|packed_git
modifier|*
name|packs
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|setup_index
specifier|static
name|void
name|setup_index
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|packed_git
modifier|*
name|new_pack
decl_stmt|;
name|char
name|filename
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|strcpy
argument_list|(
name|filename
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|filename
argument_list|,
literal|"/objects/pack/pack-"
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|filename
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|filename
argument_list|,
literal|".idx"
argument_list|)
expr_stmt|;
name|new_pack
operator|=
name|parse_pack_index_file
argument_list|(
name|sha1
argument_list|,
name|filename
argument_list|)
expr_stmt|;
name|new_pack
operator|->
name|next
operator|=
name|packs
expr_stmt|;
name|packs
operator|=
name|new_pack
expr_stmt|;
block|}
end_function
begin_function
DECL|function|setup_indices
specifier|static
name|int
name|setup_indices
parameter_list|(
name|void
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|char
name|filename
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|sprintf
argument_list|(
name|filename
argument_list|,
literal|"%s/objects/pack/"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|dir
operator|=
name|opendir
argument_list|(
name|filename
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
return|return
operator|-
literal|1
return|;
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
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
decl_stmt|;
if|if
condition|(
name|namelen
operator|!=
literal|50
operator|||
name|strcmp
argument_list|(
name|de
operator|->
name|d_name
operator|+
name|namelen
operator|-
literal|5
argument_list|,
literal|".pack"
argument_list|)
condition|)
continue|continue;
name|get_sha1_hex
argument_list|(
name|de
operator|->
name|d_name
operator|+
literal|5
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|setup_index
argument_list|(
name|sha1
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
DECL|function|copy_file
specifier|static
name|int
name|copy_file
parameter_list|(
specifier|const
name|char
modifier|*
name|source
parameter_list|,
name|char
modifier|*
name|dest
parameter_list|,
specifier|const
name|char
modifier|*
name|hex
parameter_list|,
name|int
name|warn_if_not_exists
parameter_list|)
block|{
name|safe_create_leading_directories
argument_list|(
name|dest
argument_list|)
expr_stmt|;
if|if
condition|(
name|use_link
condition|)
block|{
if|if
condition|(
operator|!
name|link
argument_list|(
name|source
argument_list|,
name|dest
argument_list|)
condition|)
block|{
name|pull_say
argument_list|(
literal|"link %s\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* If we got ENOENT there is no point continuing. */
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
block|{
if|if
condition|(
name|warn_if_not_exists
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"does not exist %s\n"
argument_list|,
name|source
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
block|}
if|if
condition|(
name|use_symlink
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|stat
argument_list|(
name|source
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|warn_if_not_exists
operator|&&
name|errno
operator|==
name|ENOENT
condition|)
return|return
operator|-
literal|1
return|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cannot stat %s: %s\n"
argument_list|,
name|source
argument_list|,
name|strerror
argument_list|(
name|errno
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
name|symlink
argument_list|(
name|source
argument_list|,
name|dest
argument_list|)
condition|)
block|{
name|pull_say
argument_list|(
literal|"symlink %s\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
if|if
condition|(
name|use_filecopy
condition|)
block|{
name|int
name|ifd
decl_stmt|,
name|ofd
decl_stmt|,
name|status
init|=
literal|0
decl_stmt|;
name|ifd
operator|=
name|open
argument_list|(
name|source
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|ifd
operator|<
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|warn_if_not_exists
operator|&&
name|errno
operator|==
name|ENOENT
condition|)
return|return
operator|-
literal|1
return|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cannot open %s\n"
argument_list|,
name|source
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|ofd
operator|=
name|open
argument_list|(
name|dest
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|ofd
operator|<
literal|0
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cannot open %s\n"
argument_list|,
name|dest
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|status
operator|=
name|copy_fd
argument_list|(
name|ifd
argument_list|,
name|ofd
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|ofd
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cannot write %s\n"
argument_list|,
name|dest
argument_list|)
expr_stmt|;
else|else
name|pull_say
argument_list|(
literal|"copy %s\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
return|return
name|status
return|;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"failed to copy %s with given copy methods.\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|fetch_pack
specifier|static
name|int
name|fetch_pack
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|packed_git
modifier|*
name|target
decl_stmt|;
name|char
name|filename
index|[
name|PATH_MAX
index|]
decl_stmt|;
if|if
condition|(
name|setup_indices
argument_list|()
condition|)
return|return
operator|-
literal|1
return|;
name|target
operator|=
name|find_sha1_pack
argument_list|(
name|sha1
argument_list|,
name|packs
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|target
condition|)
return|return
name|error
argument_list|(
literal|"Couldn't find %s: not separate or in any pack"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|get_verbosely
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Getting pack %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" which contains %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|sprintf
argument_list|(
name|filename
argument_list|,
literal|"%s/objects/pack/pack-%s.pack"
argument_list|,
name|path
argument_list|,
name|sha1_to_hex
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|copy_file
argument_list|(
name|filename
argument_list|,
name|sha1_pack_name
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|filename
argument_list|,
literal|"%s/objects/pack/pack-%s.idx"
argument_list|,
name|path
argument_list|,
name|sha1_to_hex
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|copy_file
argument_list|(
name|filename
argument_list|,
name|sha1_pack_index_name
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|install_packed_git
argument_list|(
name|target
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fetch_file
specifier|static
name|int
name|fetch_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|int
name|object_name_start
init|=
operator|-
literal|1
decl_stmt|;
specifier|static
name|char
name|filename
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|char
modifier|*
name|hex
init|=
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|char
modifier|*
name|dest_filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|object_name_start
operator|<
literal|0
condition|)
block|{
name|strcpy
argument_list|(
name|filename
argument_list|,
name|path
argument_list|)
expr_stmt|;
comment|/* e.g. git.git */
name|strcat
argument_list|(
name|filename
argument_list|,
literal|"/objects/"
argument_list|)
expr_stmt|;
name|object_name_start
operator|=
name|strlen
argument_list|(
name|filename
argument_list|)
expr_stmt|;
block|}
name|filename
index|[
name|object_name_start
operator|+
literal|0
index|]
operator|=
name|hex
index|[
literal|0
index|]
expr_stmt|;
name|filename
index|[
name|object_name_start
operator|+
literal|1
index|]
operator|=
name|hex
index|[
literal|1
index|]
expr_stmt|;
name|filename
index|[
name|object_name_start
operator|+
literal|2
index|]
operator|=
literal|'/'
expr_stmt|;
name|strcpy
argument_list|(
name|filename
operator|+
name|object_name_start
operator|+
literal|3
argument_list|,
name|hex
operator|+
literal|2
argument_list|)
expr_stmt|;
return|return
name|copy_file
argument_list|(
name|filename
argument_list|,
name|dest_filename
argument_list|,
name|hex
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|fetch
name|int
name|fetch
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
else|else
return|return
name|fetch_file
argument_list|(
name|sha1
argument_list|)
operator|&&
name|fetch_pack
argument_list|(
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|fetch_ref
name|int
name|fetch_ref
parameter_list|(
name|char
modifier|*
name|ref
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|int
name|ref_name_start
init|=
operator|-
literal|1
decl_stmt|;
specifier|static
name|char
name|filename
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|static
name|char
name|hex
index|[
literal|41
index|]
decl_stmt|;
name|int
name|ifd
decl_stmt|;
if|if
condition|(
name|ref_name_start
operator|<
literal|0
condition|)
block|{
name|sprintf
argument_list|(
name|filename
argument_list|,
literal|"%s/refs/"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|ref_name_start
operator|=
name|strlen
argument_list|(
name|filename
argument_list|)
expr_stmt|;
block|}
name|strcpy
argument_list|(
name|filename
operator|+
name|ref_name_start
argument_list|,
name|ref
argument_list|)
expr_stmt|;
name|ifd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|ifd
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cannot open %s\n"
argument_list|,
name|filename
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|read
argument_list|(
name|ifd
argument_list|,
name|hex
argument_list|,
literal|40
argument_list|)
operator|!=
literal|40
operator|||
name|get_sha1_hex
argument_list|(
name|hex
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cannot read from %s\n"
argument_list|,
name|filename
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
name|pull_say
argument_list|(
literal|"ref %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|local_pull_usage
specifier|static
specifier|const
name|char
name|local_pull_usage
index|[]
init|=
literal|"git-local-fetch [-c] [-t] [-a] [-v] [-w filename] [--recover] [-l] [-s] [-n] commit-id path"
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * By default we only use file copy.  * If -l is specified, a hard link is attempted.  * If -s is specified, then a symlink is attempted.  * If -n is _not_ specified, then a regular file-to-file copy is done.  */
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
specifier|const
name|char
modifier|*
name|write_ref
init|=
name|NULL
decl_stmt|;
name|char
modifier|*
name|commit_id
decl_stmt|;
name|int
name|arg
init|=
literal|1
decl_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
while|while
condition|(
name|arg
operator|<
name|argc
operator|&&
name|argv
index|[
name|arg
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'t'
condition|)
name|get_tree
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'c'
condition|)
name|get_history
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'a'
condition|)
block|{
name|get_all
operator|=
literal|1
expr_stmt|;
name|get_tree
operator|=
literal|1
expr_stmt|;
name|get_history
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'l'
condition|)
name|use_link
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'s'
condition|)
name|use_symlink
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'n'
condition|)
name|use_filecopy
operator|=
literal|0
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'v'
condition|)
name|get_verbosely
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'w'
condition|)
name|write_ref
operator|=
name|argv
index|[
operator|++
name|arg
index|]
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|arg
index|]
argument_list|,
literal|"--recover"
argument_list|)
condition|)
name|get_recover
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|local_pull_usage
argument_list|)
expr_stmt|;
name|arg
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|<
name|arg
operator|+
literal|2
condition|)
name|usage
argument_list|(
name|local_pull_usage
argument_list|)
expr_stmt|;
name|commit_id
operator|=
name|argv
index|[
name|arg
index|]
expr_stmt|;
name|path
operator|=
name|argv
index|[
name|arg
operator|+
literal|1
index|]
expr_stmt|;
if|if
condition|(
name|pull
argument_list|(
literal|1
argument_list|,
operator|&
name|commit_id
argument_list|,
operator|&
name|write_ref
argument_list|,
name|path
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
end_unit
