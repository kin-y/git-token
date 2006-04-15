begin_unit
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_include
include|#
directive|include
file|<libgen.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<assert.h>
end_include
begin_include
include|#
directive|include
file|<math.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
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
file|"rabinpoly.h"
end_include
begin_include
include|#
directive|include
file|"gsimm.h"
end_include
begin_define
DECL|macro|MIN
define|#
directive|define
name|MIN
parameter_list|(
name|x
parameter_list|,
name|y
parameter_list|)
value|((y)<(x) ? (y) : (x))
end_define
begin_define
DECL|macro|MAX
define|#
directive|define
name|MAX
parameter_list|(
name|x
parameter_list|,
name|y
parameter_list|)
value|((y)>(x) ? (y) : (x))
end_define
begin_comment
comment|/* The RABIN_WINDOW_SIZE is the size of fingerprint window used by    Rabin algorithm. This is not a modifiable parameter.     The first RABIN_WINDOW_SIZE - 1 bytes are skipped, in order to ensure    fingerprints are good hashes. This does somewhat reduce the    influence of the first few bytes in the file (they're part of    fewer windows, like the last few bytes), but that actually isn't    so bad as files often start with fixed content that may bias comparisons. */
end_comment
begin_typedef
DECL|struct|fileinfo
typedef|typedef
struct|struct
name|fileinfo
DECL|member|name
block|{
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|length
name|size_t
name|length
decl_stmt|;
DECL|member|md
name|u_char
name|md
index|[
name|MD_LENGTH
index|]
decl_stmt|;
DECL|member|match
name|int
name|match
decl_stmt|;
block|}
DECL|typedef|File
name|File
typedef|;
end_typedef
begin_decl_stmt
DECL|variable|flag_verbose
name|int
name|flag_verbose
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|flag_debug
name|int
name|flag_debug
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|flag_relative
name|char
modifier|*
name|flag_relative
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|cmd
name|char
name|cmd
index|[
literal|12
index|]
init|=
literal|"        ..."
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|md_strbuf
name|char
name|md_strbuf
index|[
name|MD_LENGTH
operator|*
literal|2
operator|+
literal|1
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|relative_md
name|u_char
name|relative_md
index|[
name|MD_LENGTH
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|file
name|File
modifier|*
name|file
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|file_count
name|int
name|file_count
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|file_bytes
name|size_t
name|file_bytes
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|hex
name|char
name|hex
index|[
literal|17
index|]
init|=
literal|"0123456789abcdef"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|usage
name|void
name|usage
parameter_list|()
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"usage: %s [-dhvw] [-r fingerprint] file ...\n"
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" -d\tdebug output, repeate for more verbosity\n"
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" -h\tshow this usage information\n"
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" -r\tshow distance relative to fingerprint "
literal|"(%u hex digits)\n"
argument_list|,
name|MD_LENGTH
operator|*
literal|2
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" -v\tverbose output, repeat for even more verbosity\n"
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" -w\tenable warnings for suspect statistics\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|dist
name|int
name|dist
parameter_list|(
name|u_char
modifier|*
name|l
parameter_list|,
name|u_char
modifier|*
name|r
parameter_list|)
block|{
name|int
name|j
decl_stmt|,
name|k
decl_stmt|;
name|int
name|d
init|=
literal|0
decl_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|MD_LENGTH
condition|;
name|j
operator|++
control|)
block|{
name|u_char
name|ch
init|=
name|l
index|[
name|j
index|]
operator|^
name|r
index|[
name|j
index|]
decl_stmt|;
for|for
control|(
name|k
operator|=
literal|0
init|;
name|k
operator|<
literal|8
condition|;
name|k
operator|++
control|)
name|d
operator|+=
operator|(
operator|(
name|ch
operator|&
operator|(
literal|1
operator|<<
name|k
operator|)
operator|)
operator|>
literal|0
operator|)
expr_stmt|;
block|}
return|return
name|d
return|;
block|}
end_function
begin_function
DECL|function|md_to_str
name|char
modifier|*
name|md_to_str
parameter_list|(
name|u_char
modifier|*
name|md
parameter_list|)
block|{
name|int
name|j
decl_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|MD_LENGTH
condition|;
name|j
operator|++
control|)
block|{
name|u_char
name|ch
init|=
name|md
index|[
name|j
index|]
decl_stmt|;
name|md_strbuf
index|[
name|j
operator|*
literal|2
index|]
operator|=
name|hex
index|[
name|ch
operator|>>
literal|4
index|]
expr_stmt|;
name|md_strbuf
index|[
name|j
operator|*
literal|2
operator|+
literal|1
index|]
operator|=
name|hex
index|[
name|ch
operator|&
literal|0xF
index|]
expr_stmt|;
block|}
name|md_strbuf
index|[
name|j
operator|*
literal|2
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|md_strbuf
return|;
block|}
end_function
begin_function
DECL|function|process_file
name|void
name|process_file
parameter_list|(
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|struct
name|stat
name|fs
decl_stmt|;
name|u_char
modifier|*
name|data
decl_stmt|;
name|File
modifier|*
name|fi
init|=
name|file
operator|+
name|file_count
decl_stmt|;
empty_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|name
argument_list|,
name|O_RDONLY
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|2
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|fs
argument_list|)
condition|)
block|{
name|perror
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|2
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|fs
operator|.
name|st_size
operator|>=
name|MIN_FILE_SIZE
operator|&&
name|fs
operator|.
name|st_size
operator|<=
name|MAX_FILE_SIZE
condition|)
block|{
name|fi
operator|->
name|length
operator|=
name|fs
operator|.
name|st_size
expr_stmt|;
name|fi
operator|->
name|name
operator|=
name|name
expr_stmt|;
name|data
operator|=
operator|(
name|u_char
operator|*
operator|)
name|mmap
argument_list|(
literal|0
argument_list|,
name|fs
operator|.
name|st_size
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
name|data
operator|==
operator|(
name|u_char
operator|*
operator|)
operator|-
literal|1
condition|)
block|{
name|perror
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|2
argument_list|)
expr_stmt|;
block|}
name|gb_simm_process
argument_list|(
name|data
argument_list|,
name|fs
operator|.
name|st_size
argument_list|,
name|fi
operator|->
name|md
argument_list|)
expr_stmt|;
if|if
condition|(
name|flag_relative
condition|)
block|{
name|int
name|d
init|=
name|dist
argument_list|(
name|fi
operator|->
name|md
argument_list|,
name|relative_md
argument_list|)
decl_stmt|;
name|double
name|sim
init|=
literal|1.0
operator|-
name|MIN
argument_list|(
literal|1.0
argument_list|,
call|(
name|double
call|)
argument_list|(
name|d
argument_list|)
operator|/
operator|(
name|MD_LENGTH
operator|*
literal|4
operator|-
literal|1
operator|)
argument_list|)
decl_stmt|;
name|fprintf
argument_list|(
name|stdout
argument_list|,
literal|"%s %llu %u %s %u %3.1f\n"
argument_list|,
name|md_to_str
argument_list|(
name|fi
operator|->
name|md
argument_list|)
argument_list|,
operator|(
name|long
name|long
name|unsigned
operator|)
literal|0
argument_list|,
operator|(
name|unsigned
operator|)
name|fs
operator|.
name|st_size
argument_list|,
name|name
argument_list|,
name|d
argument_list|,
literal|100.0
operator|*
name|sim
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|fprintf
argument_list|(
name|stdout
argument_list|,
literal|"%s %llu %u %s\n"
argument_list|,
name|md_to_str
argument_list|(
name|fi
operator|->
name|md
argument_list|)
argument_list|,
operator|(
name|long
name|long
name|unsigned
operator|)
literal|0
argument_list|,
operator|(
name|unsigned
operator|)
name|fs
operator|.
name|st_size
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
name|munmap
argument_list|(
name|data
argument_list|,
name|fs
operator|.
name|st_size
argument_list|)
expr_stmt|;
name|file_bytes
operator|+=
name|fs
operator|.
name|st_size
expr_stmt|;
name|file_count
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|flag_verbose
condition|)
block|{
name|fprintf
argument_list|(
name|stdout
argument_list|,
literal|"skipping %s (size %llu)\n"
argument_list|,
name|name
argument_list|,
operator|(
name|long
name|long
name|unsigned
operator|)
name|fs
operator|.
name|st_size
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|str_to_md
name|u_char
modifier|*
name|str_to_md
parameter_list|(
name|char
modifier|*
name|str
parameter_list|,
name|u_char
modifier|*
name|md
parameter_list|)
block|{
name|int
name|j
decl_stmt|;
if|if
condition|(
operator|!
name|md
operator|||
operator|!
name|str
condition|)
return|return
literal|0
return|;
name|bzero
argument_list|(
name|md
argument_list|,
name|MD_LENGTH
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|MD_LENGTH
operator|*
literal|2
condition|;
name|j
operator|++
control|)
block|{
name|char
name|ch
init|=
name|str
index|[
name|j
index|]
decl_stmt|;
if|if
condition|(
name|ch
operator|>=
literal|'0'
operator|&&
name|ch
operator|<=
literal|'9'
condition|)
block|{
name|md
index|[
name|j
operator|/
literal|2
index|]
operator|=
operator|(
name|md
index|[
name|j
operator|/
literal|2
index|]
operator|<<
literal|4
operator|)
operator|+
operator|(
name|ch
operator|-
literal|'0'
operator|)
expr_stmt|;
block|}
else|else
block|{
name|ch
operator||=
literal|32
expr_stmt|;
if|if
condition|(
name|ch
operator|<
literal|'a'
operator|||
name|ch
operator|>
literal|'f'
condition|)
break|break;
name|md
index|[
name|j
operator|/
literal|2
index|]
operator|=
operator|(
name|md
index|[
name|j
operator|/
literal|2
index|]
operator|<<
literal|4
operator|)
operator|+
operator|(
name|ch
operator|-
literal|'a'
operator|+
literal|10
operator|)
expr_stmt|;
block|}
block|}
return|return
operator|(
name|j
operator|!=
name|MD_LENGTH
operator|*
literal|2
operator|||
name|str
index|[
name|j
index|]
operator|!=
literal|0
operator|)
condition|?
literal|0
else|:
name|md
return|;
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
name|argv
index|[]
parameter_list|)
block|{
name|int
name|ch
decl_stmt|,
name|j
decl_stmt|;
name|strncpy
argument_list|(
name|cmd
argument_list|,
name|basename
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
argument_list|,
literal|8
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
name|getopt
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
literal|"dhr:vw"
argument_list|)
operator|)
operator|!=
operator|-
literal|1
condition|)
block|{
switch|switch
condition|(
name|ch
condition|)
block|{
case|case
literal|'d'
case|:
name|flag_debug
operator|++
expr_stmt|;
break|break;
case|case
literal|'r'
case|:
if|if
condition|(
operator|!
name|optarg
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: missing argument for -r\n"
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
name|str_to_md
argument_list|(
name|optarg
argument_list|,
name|relative_md
argument_list|)
condition|)
name|flag_relative
operator|=
name|optarg
expr_stmt|;
else|else
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: not a valid fingerprint\n"
argument_list|,
name|optarg
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
break|break;
case|case
literal|'v'
case|:
name|flag_verbose
operator|++
expr_stmt|;
break|break;
case|case
literal|'w'
case|:
break|break;
default|default :
name|usage
argument_list|()
expr_stmt|;
return|return
operator|(
name|ch
operator|!=
literal|'h'
operator|)
return|;
block|}
block|}
name|argc
operator|-=
name|optind
expr_stmt|;
name|argv
operator|+=
name|optind
expr_stmt|;
if|if
condition|(
name|argc
operator|==
literal|0
condition|)
name|usage
argument_list|()
expr_stmt|;
name|rabin_reset
argument_list|()
expr_stmt|;
if|if
condition|(
name|flag_verbose
operator|&&
name|flag_relative
condition|)
block|{
name|fprintf
argument_list|(
name|stdout
argument_list|,
literal|"distances are relative to %s\n"
argument_list|,
name|flag_relative
argument_list|)
expr_stmt|;
block|}
name|file
operator|=
operator|(
name|File
operator|*
operator|)
name|calloc
argument_list|(
name|argc
argument_list|,
sizeof|sizeof
argument_list|(
name|File
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|argc
condition|;
name|j
operator|++
control|)
name|process_file
argument_list|(
name|argv
index|[
name|j
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|flag_verbose
condition|)
block|{
name|fprintf
argument_list|(
name|stdout
argument_list|,
literal|"%li bytes in %i files\n"
argument_list|,
operator|(
name|long
operator|)
name|file_bytes
argument_list|,
name|file_count
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
