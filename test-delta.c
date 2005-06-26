begin_unit
begin_comment
comment|/*  * test-delta.c: test code to exercise diff-delta.c and patch-delta.c  *  * (C) 2005 Nicolas Pitre<nico@cam.org>  *  * This code is free software; you can redistribute it and/or modify  * it under the terms of the GNU General Public License version 2 as  * published by the Free Software Foundation.  */
end_comment
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<sys/stat.h>
end_include
begin_include
include|#
directive|include
file|<sys/mman.h>
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_decl_stmt
DECL|variable|usage
specifier|static
specifier|const
name|char
modifier|*
name|usage
init|=
literal|"test-delta (-d|-p)<from_file><data_file><out_file>"
decl_stmt|;
end_decl_stmt
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
name|argv
index|[]
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|void
modifier|*
name|from_buf
decl_stmt|,
modifier|*
name|data_buf
decl_stmt|,
modifier|*
name|out_buf
decl_stmt|;
name|unsigned
name|long
name|from_size
decl_stmt|,
name|data_size
decl_stmt|,
name|out_size
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|5
operator|||
operator|(
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-d"
argument_list|)
operator|&&
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-p"
argument_list|)
operator|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Usage: %s\n"
argument_list|,
name|usage
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|fd
operator|=
name|open
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
operator|||
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|from_size
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|from_buf
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|from_size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|from_buf
operator|==
name|MAP_FAILED
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|argv
index|[
literal|3
index|]
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
operator|||
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|3
index|]
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|data_size
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|data_buf
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|data_size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|data_buf
operator|==
name|MAP_FAILED
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|3
index|]
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|argv
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'d'
condition|)
name|out_buf
operator|=
name|diff_delta
argument_list|(
name|from_buf
argument_list|,
name|from_size
argument_list|,
name|data_buf
argument_list|,
name|data_size
argument_list|,
operator|&
name|out_size
argument_list|,
operator|~
literal|0UL
argument_list|)
expr_stmt|;
else|else
name|out_buf
operator|=
name|patch_delta
argument_list|(
name|from_buf
argument_list|,
name|from_size
argument_list|,
name|data_buf
argument_list|,
name|data_size
argument_list|,
operator|&
name|out_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|out_buf
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"delta operation failed (returned NULL)\n"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|fd
operator|=
name|open
argument_list|(
name|argv
index|[
literal|4
index|]
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_TRUNC
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
operator|||
name|write
argument_list|(
name|fd
argument_list|,
name|out_buf
argument_list|,
name|out_size
argument_list|)
operator|!=
name|out_size
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|4
index|]
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
